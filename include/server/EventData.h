#ifndef EVENTDATA_H_INCLUDED
#define EVENTDATA_H_INCLUDED

#include "socket/Socket.h"
#include "util/CircularBuffer.h"
#include "user/User.h"

struct EventData {
    Socket* sock;
    CircularBuffer* buf;
    User* user;
    void* ptr;
};


#endif // EVENTDATA_H_INCLUDED
