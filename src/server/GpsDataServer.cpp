#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include "server/GpsDataServer.h"
#include "socket/Socket.h"
#include "socket/netutil.h"
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
    cout << "GpsDataServer::loop start" << endl;

    udpSocket->Bind();

    while(listen) {
        n = udpSocket->Recvfrom(buf, buf_len - 1, &their_addr);

        if (n == 4)
            initAddrUdp();

        if (n == 12 || n == 16)
            processCoordinates();

    }

    cout << "GpsDataServer::loop end" << endl;
}

void GpsDataServer::initAddrUdp() {
    int id = toint(buf, 0);

    User* u = umap->get(id);
    if (u == NULL) {
        cerr << "GpsDataServer::initAddrUdp() unknown user: " <<  id << endl;
        return;
    }

    u->setAddrUdp(their_addr);

#ifdef DEBUG
    cout << "GpsDataServer::initAddrUdp() initialized id: " << id << endl;
#endif // DEBUG
}

/*
Process package carying coordinates:
int id|int longitude|int latitude[|int altitude]
*/
void GpsDataServer::processCoordinates() {
#ifdef DEBUG
    cout << "GpsDataServer::processCoordinates() len: " <<  n
         << ", PKG: " <<  toint(buf, 0)
         << " " <<  toint(buf, 4)
         << " " <<  toint(buf, 8) << endl;
#endif // DEBUG

    int id = toint(buf, 0);

    User* u = umap->get(id);
    if (u == NULL) {
        cerr << "GpsDataServer::processCoordinates() udp not initialized: " <<  id << endl;
        return;
    }

    // verify user addr
    if (!compare_sockaddr((struct sockaddr *)&their_addr, (struct sockaddr *)u->getAddrUdpPtr())) {
#ifdef DEBUG
    cout << "GpsDataServer::processCoordinates() got BAD PACKAGE id: " << id << endl;
#endif // DEBUG
        return;
    }

    u->getEmpty()->longitude = toint(buf, 4);
    u->getEmpty()->latitude = toint(buf, 8);

    if (n == 16)
        u->getEmpty()->altitude = toint(buf, 12);
    else
        u->getEmpty()->altitude = 0;
#ifdef DEBUG
    cout << "GpsDataServer::processCoordinates() update: " << u->getNick() << endl;
#endif // DEBUG
    if (u->getReady() == NULL) {
#ifdef DEBUG
        cout << "GpsDataServer::processCoordinates() ready: " << u->getNick() << endl;
#endif // DEBUG
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
