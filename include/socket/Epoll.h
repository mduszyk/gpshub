#ifndef EPOLL_H
#define EPOLL_H

#include "socket/Socket.h"
#include "socket/EpollException.h"
#include "util/CircularBuffer.h"
#include <sys/epoll.h>


struct EpollEvent {
    Socket* sock;
    CircularBuffer* buf;
    void (*clbk)(EpollEvent*);
    void* epl;
    void* creator;
    void* ptr;
};

class Epoll {

    public:
        Epoll();
        virtual ~Epoll();
        void addEvent(EpollEvent* eev, int eflags) throw(EpollException);
        void removeEvent(EpollEvent* eev) throw(EpollException);
        void loop() throw(EpollException);

    private:
        int efd;
        static const int edelta = 64;
        int events_size;
        int nevents;

        struct epoll_event event;
        struct epoll_event *events;

};

#endif // EPOLL_H
