#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "server/GpsDataServer.h"
#include "server/CmdPkg.h"
#include "socket/Socket.h"
#include "socket/netutil.h"
#include "log/macros.h"
#include "ComponentRegistry.h"


GpsDataServer::GpsDataServer(const char* port, IdUserMap* umap,
                             BlockingQueue<int>* uqueue) {
    udpSocket = new Socket(NULL, port, SOCK_DGRAM);
    this->umap = umap;
    this->uqueue = uqueue;
}

GpsDataServer::~GpsDataServer() {
    delete udpSocket;
}

Socket* GpsDataServer::getUdpSocket() {
    return udpSocket;
}

void GpsDataServer::loop() {
    LOG_INFO("Starting GPS data server, port: " << udpSocket->getPort());

    udpSocket->Bind();

    // epoll will monitor only one descriptor
    // so pass 1 as events list size grow delta
    epl = new Epoll(1);
    // set timeout to 5 sec
    epl->setTimeout(5000);
    epl->setTimeoutClbk(GpsDataServer::timeoutClbk);

    EpollEvent* server_event = new EpollEvent();
    server_event->fd = udpSocket->getFd();
    server_event->clbk = GpsDataServer::incomingDataClbk;

    /* Monitor server socket for incomming data in edge triggered mode
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

    LOG_INFO("GPS data server finished working");
}

void GpsDataServer::timeoutClbk() {
    ComponentRegistry::getGpsDataServer()->sendPendingCmd();
}

void GpsDataServer::incomingDataClbk(EpollEvent* event) {
    ComponentRegistry::getGpsDataServer()->incomingData(event);
}

void GpsDataServer::incomingData(EpollEvent* event) {
    n = udpSocket->Recvfrom(buf, BUF_LEN, &their_addr);

    if (n == 12 || n == 16)
        processCoordinates();
    else if (n == 8)
        initAddrUdp();

    sendPendingCmd();
}

/**
    Process initialize udp package:
    .-----------------.-------------.
    | unsigned int id | int token   |
    '-----------------'-------------'
*/
void GpsDataServer::initAddrUdp() {
    int id = toint(buf, 0);
    int token = toint(buf, 4);

    User* u = umap->get(id);
    if (u == NULL) {
        LOG_WARN("Init package with unknown user id: " << id);
        return;
    }

    LOG_DEBUG("got token: " << token << ", user's token: " << u->getToken());
    if (token != u->getToken()) {
        LOG_WARN("Init package with bad token, user: " << *u);
        sendUdpInitAck(u, 0);
        return;
    }

    // verify ip address TCP vs UDP (ip should be the same)
    if (!ip_equals(u->getSockPtr()->getAddrPtr(), &their_addr)) {
        LOG_WARN("Init package form bad ip address, user: " << *u);
        sendUdpInitAck(u, 0);
        return;
    }

    u->setAddrUdp(their_addr);

    if (IS_LOG_INFO) {
        char socket_str[INET6_ADDRSTRLEN + 6];
        socket_printable(&their_addr, socket_str);
        LOG_INFO("Initialized udp, user: " << *u << ", socket: " << socket_str);
    }

    sendUdpInitAck(u, 1);
}

void GpsDataServer::sendUdpInitAck(User* u, char status) {
    CmdPkg* ack = new CmdPkg(CmdPkg::INITIALIZE_UDP_ACK, 4);
    ack->getData()[0] = status;
    try {
        u->getSockPtr()->Send(ack->getBytes(), ack->getLen());
        delete ack;
    } catch(SocketException& e) {
        if (e.getErrno() != EWOULDBLOCK)
            throw;
        // if send trows exception with EWOULDBLOCK errno, add pkg to send queue
        PendingCmd* pcmd = new PendingCmd();
        pcmd->cmd = ack;
        pcmd->sock = u->getSockPtr();
        pending_cmd_queue.push_back(pcmd);
    }
}

void GpsDataServer::sendPendingCmd() {
    if(!pending_cmd_queue.empty()) {
        std::list<PendingCmd*>::iterator it = pending_cmd_queue.begin();
        std::list<PendingCmd*>::iterator end = pending_cmd_queue.end();
        while (it != end) {
            try {
                PendingCmd* pcmd = *it;
                pcmd->sock->Send(pcmd->cmd->getBytes(), pcmd->cmd->getLen());
                delete pcmd;
                it = pending_cmd_queue.erase(it);
            } catch(SocketException& e) {
                if (e.getErrno() != EWOULDBLOCK)
                    throw;
                it++;
            }
        }
    }
}

/**
    Process package carying coordinates:
    .-----------------.---------------.--------------.----------------.
    | unsigned int id | int longitude | int latitude | [int altitude] |
    '-----------------'---------------'--------------'----------------'
*/
void GpsDataServer::processCoordinates() {
    LOG_DEBUG("GPSPKG id: " << touint(buf, 0)
         << " " <<  toint(buf, 4)
         << " " <<  toint(buf, 8));

    int id = touint(buf, 0);

    User* u = umap->get(id);
    if (u == NULL) {
        LOG_ERROR("UDP channel not initialized for user id: " << id);
        return;
    }

    // verify if package comes form registered udp socket
    if (!socket_equals(&their_addr, u->getAddrUdpPtr())) {
        LOG_WARN("Gps package from unknown address, user: " << *u);
        return;
    }

    u->getEmpty()->longitude = toint(buf, 4);
    u->getEmpty()->latitude = toint(buf, 8);

    if (n == 16)
        u->getEmpty()->altitude = toint(buf, 12);
    else
        u->getEmpty()->altitude = 0;
    LOG_DEBUG("Position update, user: " << *u);
    if (u->getReady() == NULL) {
        /* If ready is set to NULL, it means that user is ready for
           broadcasting position. We want to avoid situation when
           particular user's ids are accumulated on queue. Ready
           is set to NULL by CoordsBroadcastThread after
           finished broadcasting. */
        LOG_DEBUG("Ready for broadcasting position, user: " << *u);
        // set ready slot
        u->setReady(u->getEmpty());

        // swap empty slot
        if (u->getEmpty() == u->getSlot1())
            u->setEmpty(u->getSlot2());
        else
            u->setEmpty(u->getSlot1());

        uqueue->put(id);
    }

}

void GpsDataServer::stop() {
    LOG_INFO("Stopping GSP data server...");
    epl->stop();

    udpSocket->Close();
}

