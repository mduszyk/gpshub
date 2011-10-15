#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "server/GpsDataServer.h"
#include "socket/Socket.h"
#include "socket/netutil.h"
#include "log/macros.h"

using namespace std;

GpsDataServer::GpsDataServer(char* port, IdUserMap* umap, BlockingQueue<int>* uqueue) {
    udpSocket = new Socket(NULL, port, SOCK_DGRAM);
    this->umap = umap;
    this->uqueue = uqueue;
    listen = true;
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

    while(listen) {
        n = udpSocket->Recvfrom(buf, buf_len - 1, &their_addr);

        if (n == 4)
            initAddrUdp();

        if (n == 12 || n == 16)
            processCoordinates();

    }

    LOG_INFO("GPS data server finished working");
}

void GpsDataServer::initAddrUdp() {
    int id = toint(buf, 0);

    User* u = umap->get(id);
    if (u == NULL) {
        LOG_ERROR("Unknown user id: " << id);
        return;
    }

    u->setAddrUdp(their_addr);

    LOG_DEBUG("Initialized user: " << *u);
}

/*
Process package carying coordinates:
int id|int longitude|int latitude[|int altitude]
*/
void GpsDataServer::processCoordinates() {
    LOG_DEBUG("PKG type: " << toint(buf, 0)
         << ", len: " <<  n
         << " " <<  toint(buf, 4)
         << " " <<  toint(buf, 8));

    int id = toint(buf, 0);

    User* u = umap->get(id);
    if (u == NULL) {
        LOG_ERROR("UDP channel not initialized for user id: " << id);
        return;
    }

    // verify user addr
    if (!compare_sockaddr((struct sockaddr *)&their_addr, (struct sockaddr *)u->getAddrUdpPtr())) {
        LOG_DEBUG("PKG from unknown addr, user: " << *u);
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
    listen = false;
    // TODO
    //udpSocket->Close();
}
