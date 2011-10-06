#include <stdio.h>
#include "user/User.h"


User::User(int id, char* nick) {
    this->id = id;
    this->nick = nick;
    slot1 = new Coordinates();
    slot2 = new Coordinates();
    empty = slot1;
    ready.exchange(NULL);
    udp_ready = false;
    sock = NULL;
}

User::~User() {
    delete slot1;
    delete slot2;
}

int User::getId() {
    return this->id;
}

Coordinates* User::getSlot1() {
    return slot1;
}

Coordinates* User::getSlot2() {
    return slot2;
}

Coordinates* User::getEmpty() {
    return empty;
}

void User::setEmpty(Coordinates* c) {
    empty = c;
}

Coordinates* User::getReady() {
    return ready.load();
}

void User::setReady(Coordinates* c) {
    ready.exchange(c);
}

sockaddr_storage* User::getAddrUdpPtr() {
    return &addr_udp;
}

void User::setAddrUdp(sockaddr_storage addr) {
    addr_udp = addr;
    udp_ready = true;
}

char* User::getNick() {
    return nick;
}

BuddiesSet& User::getBuddies() {
    return buddies;
}

bool User::isUdpReady() {
    return udp_ready;
}

void User::setSockPtr(Socket* sock) {
    this->sock = sock;
}

Socket* User::getSockPtr() {
    return sock;
}
