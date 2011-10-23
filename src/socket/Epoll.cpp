#include "socket/Epoll.h"
#include "socket/EpollException.h"
#include <iostream>
#include "log/macros.h"


Epoll::Epoll() {
    efd = epoll_create1(0);
    if (efd == -1) {
        throw EpollException("failed to create epoll fd");
    }

    _loop = true;

    events_size = edelta;
    events = (epoll_event*) malloc(events_size * (sizeof event));

    nevents = 0;

    initStopPipe();
}

Epoll::~Epoll() {
    removeEvent(&stop_event);
    free(events);
    close(efd);
    close(stop_pipe[0]);
    close(stop_pipe[1]);
}

void Epoll::initStopPipe() throw(EpollException) {
    if (pipe(stop_pipe) == -1) {
        throw EpollException("Failed to create stoping pipe");
    }

    stop_event.fd = stop_pipe[0];
    stop_event.clbk = NULL;
    addEvent(&stop_event, EPOLLIN | EPOLLET);
}

void Epoll::addEvent(EpollEvent* eev, int eflags) throw(EpollException) {
    if (nevents + 1 > events_size) {
        free(events);
        events_size += edelta;
        events = (epoll_event*) malloc(events_size * (sizeof event));
    }

    event.data.ptr = eev;
    event.events = eflags | EPOLLRDHUP;
    int s = epoll_ctl(efd, EPOLL_CTL_ADD, eev->fd, &event);
    if (s == -1) {
        throw EpollException("unable to add event to epoll");
    }
    nevents++;
}

/**
    Removes event from epoll and deletes EpollEvent from heap.

    In kernel versions before 2.6.9, the EPOLL_CTL_DEL operation required
    a non-NULL pointer in event, even though this argument is ignored.
    Since kernel 2.6.9, event can be specified as NULL when using EPOLL_CTL_DEL.
*/
void Epoll::removeEvent(EpollEvent* eev) throw(EpollException) {
    int s = epoll_ctl(efd, EPOLL_CTL_DEL, eev->fd, &event);
    if (s == -1) {
        throw EpollException("unable to remove event from epoll");
    }
    nevents--;

    if (nevents + 2 * edelta <= events_size) {
        free(events);
        events_size -= edelta;
        events = (epoll_event*) malloc(events_size * (sizeof event));
    }
}

/**
    The event loop
*/
void Epoll::loop() throw(EpollException) {
    int n, i;

    while(_loop) {

        n = epoll_wait(efd, events, events_size, -1);
        if (n == -1) {
            throw EpollException("error waiting for event");
        }
        for (i = 0; i < n; i++) {
            EpollEvent* eev = (EpollEvent*) events[i].data.ptr;
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (events[i].events & EPOLLRDHUP))
            {
                /*
                    EPOLLERR - error on the associated file descriptor
                    EPOLLHUP - hang up on the associated file descriptor,
                        signals an unexpected close of the socket
                    EPOLLRDHUP - peer closed connection, or shut down
                        writing half of connection
                */

                if (eev->fd != stop_pipe[0])
                    end_event_clbk(eev);
                else {
                    removeEvent(&stop_event);
                    close(stop_pipe[0]);
                    close(stop_pipe[1]);
                    initStopPipe();
                }

                continue;
            }

            // run callback if set
            if (eev->clbk != NULL) {
                eev->clbk(eev);
            }
        }
    }

}

void Epoll::stop() {
    _loop = false;
    // send one byte to stop_pipe, it causes to return from epoll_wait
    write(stop_pipe[1], "\0", 1);
}

void Epoll::setEndEventClbk(void (*end_event_clbk)(EpollEvent*)) {
    this->end_event_clbk = end_event_clbk;
}
