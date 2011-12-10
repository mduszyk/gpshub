#ifndef EPOLL_H
#define EPOLL_H

#include "socket/Socket.h"
#include "socket/EpollException.h"
#include "util/CircularBuffer.h"
#include <sys/epoll.h>

#define DEFAULT_EDELTA 64


struct EpollEvent {
    int fd;
    void* ptr;
    void (*clbk)(EpollEvent*);
    uint32_t events;
};

class Epoll {

    public:

        Epoll();

        Epoll(int edelta);

        virtual ~Epoll();

        void addEvent(EpollEvent* eev, int eflags) throw(EpollException);

        void removeEvent(EpollEvent* eev) throw(EpollException);

        void loop() throw(EpollException);

        void stop();

        void setEndEventClbk(void (*end_event_clbk)(EpollEvent*));

        void setTimeoutClbk(void (*timeout_clbk)()) {
            this->timeout_clbk = timeout_clbk;
        }

        void setTimeout(int timeout) {
            this->timeout = timeout;
        }

    private:

        int efd;

        int edelta;

        int events_size;

        int nevents;

        int stop_pipe[2];

        EpollEvent stop_event;

        bool _loop;

        int timeout;

        struct epoll_event event;

        struct epoll_event *events;

        void (*end_event_clbk)(EpollEvent*);

        void (*timeout_clbk)();

        void init();

        void initStopPipe() throw(EpollException);

};

#endif // EPOLL_H
