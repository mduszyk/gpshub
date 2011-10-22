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

    EventData* edata = new EventData();
    edata->ptr = this;
    edata->sock = tcpSocket;

    EpollEvent* server_event = new EpollEvent();
    server_event->fd = tcpSocket->getFd();
    server_event->clbk = CommandServer::incomingConnectionClbk;
    server_event->ptr = edata;

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

    LOG_INFO("CommandServer end");
}

void CommandServer::incomingConnectionClbk(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    ((CommandServer*)edata->ptr)->incomingConnection(event);
}

void CommandServer::incomingDataClbk(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    ((CommandServer*)edata->ptr)->incomingData(event);
}

void CommandServer::closeConnectionClbk(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    LOG_DEBUG("Peer closing connection: " << *(edata->sock));
    ((CommandServer*)edata->ptr)->closeConnection(event);
}

/**
    We have a notification on the listening socket,
    which means one or more incoming connections.
*/
void CommandServer::incomingConnection(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    LOG_DEBUG("Connections waiting for accept");

    Socket* s;
    // accept all incomming connections
    for(;;) {
        try {
            s = edata->sock->Accept();
            LOG_DEBUG("Accepted connection: " << *s);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN || e.getErrno() == EWOULDBLOCK) {
                // all incoming connections are processed
            } else {
                LOG_ERROR("Socket accpet error: " << e.what() << ": " << strerror(e.getErrno()));
            }
            break;
        }

        // make socket non blocking
        s->addFlags(O_NONBLOCK);

        CircularBuffer* cbuf = new CircularBuffer(MAX_PKG_LEN * 2);

        // important to later delete new_event and edata
        EventData* edata = new EventData();
        edata->ptr = this;
        edata->sock = s;
        edata->buf = cbuf;

        EpollEvent* new_event = new EpollEvent();
        new_event->fd = s->getFd();
        new_event->clbk = CommandServer::incomingDataClbk;
        new_event->ptr = edata;

        epl->addEvent(new_event, EPOLLIN | EPOLLET);
    }
}

/**
    Process command package:
    .---------------.-----------------------.------------.
    | char pkg_type | unsigned short length | char* data |
    '---------------'-----------------------'------------'
*/
void CommandServer::incomingData(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    LOG_DEBUG("Incomming data: " << *(edata->sock));

    /* We have data on the fd waiting to be read. We must read
       whatever data is available completely, if we are running
       in edge-triggered (EPOLLET) mode and won't get a notification
       again for the same data. */
    for(;;) {
        int n;
        try {
            n = edata->sock->Recv(read_buf, MAX_PKG_LEN);
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

        edata->buf->add(read_buf, n);

        // read pkg header
        edata->buf->read(read_buf, 3);
        short pkg_len = toshort(read_buf, 1);

        LOG_DEBUG("pkg_len: " << pkg_len << ", buf size: " << edata->buf->size());

        if (edata->buf->size() >= pkg_len) {
            // we have whole package - process it
            edata->buf->get(read_buf, pkg_len);
            CmdPkg* pkg = new CmdPkg(read_buf);
            cmdHandler->handle(pkg, edata);
            delete pkg;
        }
    }

}

void CommandServer::closeConnection(EpollEvent* event) {
    EventData* edata = (EventData*) event->ptr;
    LOG_DEBUG("Closing connection: " << *(edata->sock));

    if (edata->user != NULL)
        cmdHandler->quit(edata->user);

    epl->removeEvent(event);

    try {
        edata->sock->Close();
    } catch (SocketException& e) {
        LOG_ERROR("Socket close error: " << e.what() << ": " << strerror(e.getErrno()));
    }

    delete edata->sock;
    delete edata->buf;
    delete edata;
    delete event;
}

void CommandServer::stop() {
    LOG_INFO("Stopping CommandServer");
    epl->stop();

    // TODO cleanup

}
