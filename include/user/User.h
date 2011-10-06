#ifndef USER_H
#define USER_H

#include <sys/socket.h>
#include <cstdatomic>
#include "Coordinates.h"
#include "thread/SyncHashSet.h"
#include "util/hashmaphelper.h"
#include "socket/Socket.h"

typedef SyncHashSet<char*, StrHash, StrEqual> BuddiesSet;

class User
{
    public:
        User(int id, char* nick);
        virtual ~User();
        int getId();
        char* getNick();
        Coordinates* getSlot1();
        Coordinates* getSlot2();
        Coordinates* getEmpty();
        void setEmpty(Coordinates* c);
        Coordinates* getReady();
        void setReady(Coordinates* c);
        sockaddr_storage* getAddrUdpPtr();
        void setAddrUdp(sockaddr_storage addr);
        BuddiesSet& getBuddies();
        bool isUdpReady();
        void setSockPtr(Socket* sock);
        Socket* getSockPtr();

    private:
        int id;
        char* nick;
        BuddiesSet buddies;
        Coordinates* slot1;
        Coordinates* slot2;
        Coordinates* empty;
        std::atomic<Coordinates*> ready;
        sockaddr_storage addr_udp;
        bool udp_ready;
        Socket* sock;

};

#endif // USER_H
