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

        BuddiesSet& getBuddies();

        Coordinates* getSlot1();
        Coordinates* getSlot2();

        Coordinates* getEmpty();
        void setEmpty(Coordinates* c);

        Coordinates* getReady();
        void setReady(Coordinates* c);


        sockaddr_storage* getAddrUdpPtr();
        void setAddrUdp(sockaddr_storage addr);

        bool isUdpReady();

        void setSockPtr(Socket* sock);
        Socket* getSockPtr();

        int getToken();
        void setToken(int token);

    private:
        int id;
        char* nick;

        BuddiesSet buddies;

        Coordinates* slot1;
        Coordinates* slot2;

        Coordinates* empty;
        std::atomic<Coordinates*> ready;

        // TODO move to Session
        sockaddr_storage addr_udp;
        bool udp_ready;
        // TODO move to session
        Socket* sock;

        int token;

        friend std::ostream& operator<<(std::ostream&, const User&);

};

inline std::ostream& operator<<(std::ostream& stream, const User& u) {
    return stream << u.nick << "(" << u.id << ")";
}

#endif // USER_H
