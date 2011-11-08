#include <iostream>
#include <sys/socket.h>
#include <fcntl.h>
#include "server/CommandServer.h"
#include "socket/Socket.h"
#include "socket/SocketException.h"
#include "socket/Epoll.h"
#include "socket/netutil.h"
#include "user/User.h"
#include "util/CircularBuffer.h"
#include "server/CmdPkg.h"
#include "log/macros.h"

#define MAX_PKG_LEN 512

CommandServer::CommandServer(char* port, IdUserMap* umap, CommandHandler* cmdHandler) {
    tcpSocket = new Socket(NULL, port, SOCK_STREAM);
    this->umap = umap;
    this->cmdHandler = cmdHandler;
    read_buf = (char*) malloc(MAX_PKG_LEN);
}

CommandServer::~CommandServer() {
    delete tcpSocket;
    free(read_buf);
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

    Session* server_session = new Session();
    server_session->ptr = this;
    server_session->sock = tcpSocket;
    server_session->buf = NULL;
    server_session->user = NULL;

    EpollEvent* server_event = new EpollEvent();
    server_event->fd = tcpSocket->getFd();
    server_event->clbk = CommandServer::incomingConnectionClbk;
    server_event->ptr = server_session;

    /* Monitor server socket for incomming connections in edge triggered mode
       EPOLLIN - the associated file is available for read
       EPOLLET - sets  the  Edge  Triggered  behavior */
    epl->addEvent(server_event, EPOLLIN | EPOLLET);

    try {
        epl->loop();
    } catch (EpollException& e) {
        LOG_ERROR("Epoll loop error: " << e.what() << ": " << strerror(e.getErrno()));
    }

    delete epl;
    delete server_event;
    delete server_session;

    LOG_INFO("CommandServer end");
}

void CommandServer::incomingConnectionClbk(EpollEvent* event) {
    Session* session = (Session*) event->ptr;
    ((CommandServer*)session->ptr)->incomingConnection(event);
}

void CommandServer::incomingDataClbk(EpollEvent* event) {
    Session* session = (Session*) event->ptr;
    ((CommandServer*)session->ptr)->incomingData(event);
}

void CommandServer::closeConnectionClbk(EpollEvent* event) {
    Session* session = (Session*) event->ptr;
    LOG_DEBUG("Peer closing connection: " << *(session->sock));
    ((CommandServer*)session->ptr)->closeConnection(event);
}

/**
    We have a notification on the listening socket,
    which means one or more incoming connections.
*/
void CommandServer::incomingConnection(EpollEvent* event) {
    Session* server_session = (Session*) event->ptr;
    LOG_DEBUG("Connections waiting for accept");

    Socket* new_sock;
    // accept all incomming connections
    for(;;) {
        try {
            new_sock = tcpSocket->Accept();
            // TODO find out why it is not working (segmantation fault)
            //new_sock = server_session->sock->Accept();
            LOG_DEBUG("Accepted connection: " << *new_sock);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN || e.getErrno() == EWOULDBLOCK) {
                // all incoming connections are processed
            } else {
                LOG_ERROR("Socket accpet error: " << e.what() << ": " << strerror(e.getErrno()));
            }
            break;
        }

        // make socket non blocking
        new_sock->addFlags(O_NONBLOCK);

        // important to later delete new_event and edata
        Session* new_session = new Session();
        new_session->ptr = this;
        new_session->sock = new_sock;
        new_session->buf = new CircularBuffer(MAX_PKG_LEN * 2);

        EpollEvent* new_event = new EpollEvent();
        new_event->fd = new_sock->getFd();
        new_event->clbk = CommandServer::incomingDataClbk;
        new_event->ptr = new_session;

        epl->addEvent(new_event, EPOLLIN | EPOLLET | EPOLLRDHUP);
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
            n = session->sock->Recv(read_buf, MAX_PKG_LEN);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN) {
                // EAGAIN means we have read all
            } else {
                LOG_ERROR("Socket recv error: " << e.what() << ": " << strerror(e.getErrno()));
            }
            break;
        }

        if (n == 0) {
            /* End of file. The remote has closed the connection. */
            closeConnection(event);
            break;
        }

        session->buf->add(read_buf, n);

        // read pkg header
        session->buf->read(read_buf, 3);
        pkg_len = toushort(read_buf, 1);

        LOG_DEBUG("pkg_len: " << pkg_len << ", buf size: " << session->buf->size());

        if (session->buf->size() >= pkg_len) {
            // we have whole package - process it
            session->buf->get(read_buf, pkg_len);
            CmdPkg* pkg = new CmdPkg(read_buf);
            cmdHandler->handle(pkg, session);
            delete pkg;
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
        LOG_ERROR("Socket close error: " << e.what() << ": " << strerror(e.getErrno()));
    }

    delete session->sock;
    delete session->buf;
    delete session;
    delete event;
}

void CommandServer::stop() {
    LOG_INFO("Stopping CommandServer");
    //epl->stop();

    // TODO cleanup

}
