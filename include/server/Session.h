#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#include "socket/Socket.h"
#include "util/CircularBuffer.h"
#include "user/User.h"
#include "server/CmdPkg.h"
#include <queue>

struct Session {
    Socket* sock;
    CircularBuffer* buf;
    User* user;
    void* ptr;
    // cmd pkg send queue
    std::queue<CmdPkg*> send_queue;
};


#endif // SESSION_H_INCLUDED
