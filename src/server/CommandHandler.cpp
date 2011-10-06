#include "server/CommandHandler.h"
#include <iostream>
#include <string.h>

CommandHandler::CommandHandler(IdUserMap* id_umap, NickUserMap* nick_umap, UserIdGenerator* idgen) {
    this->id_umap = id_umap;
    this->nick_umap = nick_umap;
    this->idgen = idgen;
}

CommandHandler::~CommandHandler() {
    //dtor
}

void CommandHandler::handle(CmdPkg* pkg, EpollEvent* event) {
#ifdef DEBUG
    cout << "CommandHandler::handle() package, type: " << (int)pkg->getType() << ", length: " << pkg->getLen() << ", data: " << pkg->getData() << endl;
#endif // DEBUG

    switch (pkg->getType()) {
        case CmdPkg::REGISTER_NICK:
            registerNick(pkg, event);
            break;

        case CmdPkg::ADD_BUDDIES:
            addBuddies(pkg, event);
            break;

        case CmdPkg::REMOVE_BUDDIES:
            removeBuddies(pkg, event);
            break;

        default:
            cout << "CommandHandler::handle() unknown package, type: " << (int)pkg->getType() << endl;
            break;
    }

}

void CommandHandler::registerNick(CmdPkg* pkg, EpollEvent* event) {
#ifdef DEBUG
    cout << "CommandHandler::registerNick(): " << pkg->getData() << endl;
#endif // DEBUG

    // check if nick is free
    if (this->nick_umap->count(pkg->getData()) > 0) {
#ifdef DEBUG
        cout << "CommandHandler::registerNick(): nick taken!" << endl;
#endif // DEBUG
        // send register nick ack with nick taken status
        CmdPkg pkg_fail(CmdPkg::REGISTER_NICK_ACK, 4);
        pkg_fail.getData()[0] = 0;
        event->sock->Send(pkg_fail.getBytes(), pkg_fail.getLen());
        return;
    }

#ifdef DEBUG
    cout << "CommandHandler::registerNick(): nick available" << endl;
#endif // DEBUG

    // generate user id
    unsigned int id = idgen->generate();
    if (id == 0) {
        cerr << "CommandHandler::registerNick(): all ids are taken" << endl;
    }

#ifdef DEBUG
    cout << "CommandHandler::registerNick(): user got id: " << id << endl;
#endif // DEBUG

    // copy nick from package
    int nick_len = strlen(pkg->getData());
    char* nick = (char*) malloc(nick_len + 1);
    memcpy(nick, pkg->getData(), nick_len + 1);

    // create user
    User* usr = new User(id, nick);
    usr->setSockPtr(event->sock);

    // add to user maps
    nick_umap->put(nick, usr);
    id_umap->put(id, usr);

    // store reference to user in epoll event
    event->ptr = usr;

    // send rgister nick ack with generated id
    CmdPkg pkg_ok(CmdPkg::REGISTER_NICK_ACK, 8);
    pkg_ok.getData()[0] = 1;
    pkg_ok.setInt(1, id);
#ifdef DEBUG
    cout << "CommandHandler::registerNick(): sending ACK, len: " << pkg_ok.getLen() << endl;
#endif // DEBUG
    event->sock->Send(pkg_ok.getBytes(), pkg_ok.getLen());

}

void CommandHandler::addBuddies(CmdPkg* pkg, EpollEvent* event) {
#ifdef DEBUG
    cout << "CommandHandler::addBuddies(): data: " << pkg->getData() << endl;
#endif // DEBUG
    User* usr = (User*) event->ptr;

    // prepare user id package
    int len_nick = strlen(usr->getNick()) + 1;
    int uid_pkg_len = 3 + sizeof(int) + len_nick;
    CmdPkg pkg_uid(CmdPkg::BUDDIES_IDS, uid_pkg_len);
    pkg_uid.setInt(0, usr->getId());
    memcpy(pkg_uid.getData() + sizeof(int), usr->getNick(), len_nick);

    // initialize buddies ids map and ids pkg len var
    std::map<int, char*> ids_map;
    int ids_pkg_len = 0;

    // add each buddy to user's
    char* data = pkg->getData();
    int a = 0;
    for (int i = 0; i < strlen(data) + 1; i++) {
        if (data[i] == ',' || data[i] == '\0') {
            //char* nick = (char*) malloc(i - a + 1);
            char* nick = new char(i - a + 1);
            nick[i - a] = '\0';
            memcpy(nick, data + a, i - a);
            usr->getBuddies().insert( nick );
            if (nick_umap->count(nick) > 0) {
                // add buddy to ids map
                User* buddy = nick_umap->get(nick);
                ids_map[buddy->getId()] = nick;
                // increase ids pkg len
                ids_pkg_len += i - a + 1;
                // send user's id to buddy
                buddy->getSockPtr()->Send(pkg_uid.getBytes(), pkg_uid.getLen());
            }
            a = i + 1;
        }
    }

    // send back buddies ids
    if (ids_pkg_len > 0) {
        // header length
        ids_pkg_len += 3;
        // space for ids
        ids_pkg_len += sizeof(int) * ids_map.size();
        CmdPkg pkg_ids(CmdPkg::BUDDIES_IDS, ids_pkg_len);
        std::map<int, char*>::const_iterator end = ids_map.end();
        int offset = 0;
        for (std::map<int, char*>::const_iterator it = ids_map.begin(); it != end; ++it) {
            int id = it->first;
            char* nick = it->second;
            pkg_ids.setInt(offset, id);
            offset += sizeof(int);
            int n = strlen(nick) + 1;
            memcpy(pkg_ids.getData() + offset, nick, n);
            offset += n;
        }
        // send buddies ids pkg
        event->sock->Send(pkg_ids.getBytes(), pkg_ids.getLen());
    }

}

void CommandHandler::removeBuddies(CmdPkg* pkg, EpollEvent* event) {
#ifdef DEBUG
    cout << "CommandHandler::removeBuddies(): data: " << pkg->getData() << endl;
#endif // DEBUG

    char* data = pkg->getData();
    User* usr = (User*) event->ptr;
    int a = 0;
    for (int i = 0; i < strlen(data) + 1; i++) {
        if (data[i] == ',' || data[i] == '\0') {
            //char* nick = (char*) malloc(i - a + 1);
            char* nick = new char(i - a + 1);
            nick[i - a] = '\0';
            memcpy(nick, data + a, i - a);
            usr->getBuddies().erase( nick );
            delete nick;
            a = i + 1;
        }
    }

}

void CommandHandler::quit(User* u) {
#ifdef DEBUG
    cout << "CommandHandler::quit(): user: " << u->getNick() << ", id: " << u->getId() << endl;
#endif // DEBUG

    int id = u->getId();
    char* nick = u->getNick();

    id_umap->erase(id);
    nick_umap->erase(nick);

    delete u;
    free(nick);

}
