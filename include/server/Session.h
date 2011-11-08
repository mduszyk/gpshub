#ifndef SESSION_H_INCLUDED
#define SESSION_H_INCLUDED

#include "socket/Socket.h"
#include "util/CircularBuffer.h"
#include "user/User.h"

struct Session {
    Socket* sock;
    CircularBuffer* buf;
    User* user;
    void* ptr;
};


#endif // SESSION_H_INCLUDED
