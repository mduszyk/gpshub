#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>

#include "log/macros.h"
#include "socket/Socket.h"
#include "socket/SocketException.h"
#include "socket/Epoll.h"
#include "socket/netutil.h"
#include "util/CircularBuffer.h"
#include "ComponentRegistry.h"
#include "server/CommandServer.h"
#include "server/CmdPkg.h"
#include "user/User.h"


CommandServer::CommandServer(const char* port, IdUserMap* umap,
                             CommandHandler* cmdHandler) {
    tcpSocket = new Socket(NULL, port, SOCK_STREAM);
    this->umap = umap;
    this->cmdHandler = cmdHandler;
}

CommandServer::~CommandServer() {
    delete tcpSocket;
}

void CommandServer::loop() {
    LOG_INFO("Starting command server, port: " << tcpSocket->getPort());

    tcpSocket->Bind();
    // make socket non blocking
    tcpSocket->addFlags(O_NONBLOCK);
    // listen for incoming connections
    tcpSocket->Listen();

    epl = new Epoll();
    epl->setEndEventClbk(CommandServer::closeConnectionClbk);

    EpollEvent* server_event = new EpollEvent();
    server_event->fd = tcpSocket->getFd();
    server_event->clbk = CommandServer::incomingConnectionClbk;

    /* Monitor server socket for incomming connections in edge triggered mode
       EPOLLIN - the associated file is available for read
       EPOLLET - sets  the  Edge  Triggered  behavior */
    epl->addEvent(server_event, EPOLLIN | EPOLLET);

    try {
        epl->loop();
    } catch (EpollException& e) {
        LOG_ERROR("Epoll loop error: " << e.what() << ": "
                  << strerror(e.getErrno()));
    }

    delete epl;
    delete server_event;

    LOG_INFO("CommandServer end");
}

void CommandServer::incomingConnectionClbk(EpollEvent* event) {
    ComponentRegistry::getCommandServer()->incomingConnection(event);
}

void CommandServer::clientSocketClbk(EpollEvent* event) {
    // EPOLLOUT event happens so try to send waiting packages (if there are any)
    if (event->events & EPOLLOUT)
        ComponentRegistry::getCommandServer()->send(event);
    // EPOLLIN event happens so read incomming data
    if (event->events & EPOLLIN)
        ComponentRegistry::getCommandServer()->incomingData(event);
}

void CommandServer::closeConnectionClbk(EpollEvent* event) {
    //Session* session = (Session*) event->ptr;
    LOG_DEBUG("Peer closing connection: " << *(((Session*) event->ptr)->sock));
    ComponentRegistry::getCommandServer()->closeConnection(event);
}

/**
    We have a notification on the listening socket,
    which means one or more incoming connections.
*/
void CommandServer::incomingConnection(EpollEvent* event) {
    //Session* server_session = (Session*) event->ptr;
    LOG_DEBUG("Connections waiting for accept");

    Socket* new_sock;
    // accept all incomming connections
    for(;;) {
        try {
            new_sock = tcpSocket->Accept();
            // TODO find out why this is not working when buffer in broadcast
            // thread is allocated by malloc (segmantation fault)
            //new_sock = server_session->sock->Accept();
            LOG_DEBUG("Accepted connection: " << *new_sock);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN || e.getErrno() == EWOULDBLOCK) {
                // all incoming connections are processed
            } else {
                LOG_ERROR("Socket accpet error: " << e.what() << ": "
                          << strerror(e.getErrno()));
            }
            break;
        }

        // make socket non blocking
        new_sock->addFlags(O_NONBLOCK);

        // important to later delete new_event and new_session
        Session* new_session = new Session();
        new_session->sock = new_sock;
        new_session->buf = new CircularBuffer(CMDPKG_MAX * 2);

        EpollEvent* new_event = new EpollEvent();
        new_event->fd = new_sock->getFd();
        new_event->clbk = CommandServer::clientSocketClbk;
        new_event->ptr = new_session;

        // add client socket event to epoll, monitor for incomming data and
        // for readines for sending without blocking
        epl->addEvent(new_event, EPOLLIN | EPOLLOUT | EPOLLET | EPOLLRDHUP);
    }
}

/**
    Process command package:
    .---------------.-----------------------.------------.
    | char pkg_type | unsigned short length | char* data |
    '---------------'-----------------------'------------'
*/
void CommandServer::incomingData(EpollEvent* event) {
    Session* session = (Session*) event->ptr;
    LOG_DEBUG("Incomming data: " << *(session->sock));

    /* We have data on the fd waiting to be read. We must read
       whatever data is available completely, if we are running
       in edge-triggered (EPOLLET) mode and won't get a notification
       again for the same data. */
    int n = 0;
    unsigned short pkg_len = 0;
    while(true) {
        try {
            n = session->sock->Recv(cmd_buf, CMDPKG_MAX);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN) {
                // EAGAIN means we have read all
            } else {
                LOG_ERROR("Socket recv error: " << e.what() << ": "
                          << strerror(e.getErrno()));
            }
            break;
        }

        if (n == 0) {
            /* End of file. The remote has closed the connection. */
            closeConnection(event);
            break;
        }

        session->buf->add(cmd_buf, n);

        // read pkg header
        session->buf->read(cmd_buf, 3);
        pkg_len = toushort(cmd_buf, 1);

        LOG_DEBUG("pkg_len: " << pkg_len << ", buf size: "
                  << session->buf->size());

        if (session->buf->size() >= pkg_len) {
            // we have whole package - process it
            session->buf->get(cmd_buf, pkg_len);
            CmdPkg* cmd = new CmdPkg(cmd_buf);
            cmdHandler->handle(cmd, session);
            delete cmd;
        }
    }

}

/**
    Public method used to send pkg to client. It takes intoo account
    possibility that sending might throw exception when operation would block.
*/
void CommandServer::send(Session* s, CmdPkg* p) {
    try {
        s->sock->Send(p->getBytes(), p->getLen());
        delete p;
    } catch(SocketException& e) {
        if (e.getErrno() != EWOULDBLOCK)
            throw;
        // if send trows exception with EWOULDBLOCK errno, add pkg to send queue
        s->send_queue.push(p);
    }
}

/**
   This method is invoked by client socket callback when it is possible to send
   without blocking (EPOLLOUT event). It tries to send each pkg from send_queue
   for specific session. If sending would return EWOULDBLOCK it finishes leaving
   pkg which was couse of it on queue.
*/
void CommandServer::send(EpollEvent* event) {
    Session* s = (Session*) event->ptr;
    while (!s->send_queue.empty()) {
        CmdPkg* p = s->send_queue.front();
        try {
            s->sock->Send(p->getBytes(), p->getLen());
            // if still here, sending succeeded so clean up
            s->send_queue.pop();
            delete p;
        } catch(SocketException& e) {
            if (e.getErrno() != EWOULDBLOCK)
                throw;
            return;
        }
    }
}

void CommandServer::closeConnection(EpollEvent* event) {
    Session* session = (Session*) event->ptr;
    LOG_DEBUG("Closing connection: " << *(session->sock));

    if (session->user != NULL)
        cmdHandler->quit(session->user);

    epl->removeEvent(event);

    try {
        session->sock->Close();
    } catch (SocketException& e) {
        LOG_ERROR("Socket close error: " << e.what() << ": "
                  << strerror(e.getErrno()));
    }

    delete session->sock;
    delete session->buf;
    delete session;
    delete event;
}

void CommandServer::stop() {
    LOG_INFO("Stopping command server...");
    epl->stop();

    // TODO cleanup

}
