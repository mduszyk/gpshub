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
#include "util/log.h"

#define MAX_PKG_LEN 512

CommandServer::CommandServer(char* port, IdUserMap* umap, CommandHandler* cmdHandler) {
    tcpSocket = new Socket(NULL, port, SOCK_STREAM);
    this->umap = umap;
    this->cmdHandler = cmdHandler;
    read_buf = (char*) malloc(MAX_PKG_LEN);
    listen = true;
}

CommandServer::~CommandServer() {
    delete tcpSocket;
    free(read_buf);
}

void CommandServer::loop() {
    LOG_INFO("Starting command server");

    tcpSocket->Bind();
    // make socket non blocking
    tcpSocket->addFlags(O_NONBLOCK);
    // listen for incoming connections
    tcpSocket->Listen();

    epl = new Epoll();
    EpollEvent* event = new EpollEvent();
    event->creator = this;
    event->sock = tcpSocket;
    event->clbk = CommandServer::incomingConnectionClbk;

    /* Monitor server socket for incomming connections in edge triggered mode
       EPOLLIN - the associated file is available for read
       EPOLLET - sets  the  Edge  Triggered  behavior */
    epl->addEvent(event, EPOLLIN | EPOLLET);

    try {
        epl->loop();
    } catch (exception& e) {
        LOG_ERROR("Epoll loop error: " << e.what());
    }

    delete epl;
    delete event;

    cout << "CommandServer::loop() end" << endl;
}

void CommandServer::incomingConnectionClbk(EpollEvent* event) {
    ((CommandServer*)event->creator)->incomingConnection(event);
}

void CommandServer::incomingDataClbk(EpollEvent* event) {
    ((CommandServer*)event->creator)->incomingData(event);
}

void CommandServer::incomingConnection(EpollEvent* event) {
    LOG_DEBUG("Connections waiting for accept");

    /* We have a notification on the listening socket,
       which means one or more incoming connections. */

    Socket* s;
    // accept all incomming connections
    while (true) {
        try {
            s = event->sock->Accept();
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN || e.getErrno() == EWOULDBLOCK) {
                // all incoming connections are processed
            } else {
                LOG_ERROR("Socket accpet error: " << e.what() << ", errno: " << e.getErrno());
            }
            break;
        }

        // make socket non blocking
        s->addFlags(O_NONBLOCK);

        CircularBuffer* cbuf = new CircularBuffer(MAX_PKG_LEN * 2);

        // important to later delete new_event
        EpollEvent* new_event = new EpollEvent();
        new_event->creator = this;
        new_event->buf = cbuf;
        new_event->sock = s;
        new_event->clbk = CommandServer::incomingDataClbk;
        ((Epoll*)event->epl)->addEvent(new_event, EPOLLIN | EPOLLET);
    }
}



void CommandServer::incomingData(EpollEvent* event) {
    LOG_DEBUG("Incomming data");

    /* We have data on the fd waiting to be read. We must read
       whatever data is available completely, if we are running
       in edge-triggered (EPOLLET) mode and won't get a notification
       again for the same data. */
    while (1) {
        int n;
        try {
            n = event->sock->Recv(read_buf, MAX_PKG_LEN);
        } catch (SocketException& e) {
            if (e.getErrno() == EAGAIN) {
                // EAGAIN means we have read all
            } else {
                LOG_ERROR("Socket recv error: " << e.what() << ", errno: " << e.getErrno());
            }
            break;
        }

        if (n == 0) {
            /* End of file. The remote has closed the connection. */
            User* u = (User*) event->ptr;

            LOG_INFO("User closing connection: " << *u);

            if (event->ptr != NULL)
                cmdHandler->quit(u);

            ((Epoll*)event->epl)->removeEvent(event);

            try {
                event->sock->Close();
            } catch (SocketException& e) {
                LOG_ERROR("Socket close error: " << e.what() << ", errno: " << e.getErrno());
            }

            delete event->sock;
            delete event->buf;
            delete event;

            break;
        }

        event->buf->add(read_buf, n);

        // read pkg header
        event->buf->read(read_buf, 3);
        short pkg_len = toshort(read_buf, 1);

        LOG_DEBUG("pkg_len: " << pkg_len << ", buf size: " << event->buf->size());

        if (event->buf->size() >= pkg_len) {
            // we have whole package - process it
            event->buf->get(read_buf, pkg_len);
            CmdPkg* pkg = new CmdPkg(read_buf);
            cmdHandler->handle(pkg, event);
            delete pkg;
        }
    }

}

void CommandServer::stop() {
    listen = false;
}
