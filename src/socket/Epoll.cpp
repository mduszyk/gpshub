#include "socket/Epoll.h"
#include "socket/EpollException.h"
#include <iostream>
#include "log/macros.h"
using namespace std;


Epoll::Epoll() {
    efd = epoll_create1(0);
    if (efd == -1) {
        throw EpollException("failed to create epoll fd");
    }

    events_size = edelta;
    events = (epoll_event*) malloc(events_size * (sizeof event));

    nevents = 0;
}

Epoll::~Epoll() {
    free(events);
    close(efd);
}

void Epoll::addEvent(EpollEvent* eev, int eflags) throw(EpollException) {
    if (nevents + 1 > events_size) {
        free(events);
        events_size += edelta;
        events = (epoll_event*) malloc(events_size * (sizeof event));
    }

    eev->epl = this;

    event.data.ptr = eev;
    event.events = eflags;
    int s = epoll_ctl(efd, EPOLL_CTL_ADD, eev->sock->getFd(), &event);
    if (s == -1) {
        throw EpollException("unable to add socket to epoll");
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
    int s = epoll_ctl(efd, EPOLL_CTL_DEL, eev->sock->getFd(), &event);
    if (s == -1) {
        throw EpollException("unable to remove socket from epoll");
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

    while (1) {
        int n, i;

        n = epoll_wait(efd, events, events_size, -1);
        if (n == -1) {
            throw EpollException("error waiting for event");
        }
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                (events[i].events & EPOLLHUP) ||
                (!(events[i].events & EPOLLIN)))
            {
                /* An error has occured on this fd, or the socket is not
                   ready for reading (why were we notified then?) */
                int fd = ((EpollEvent*)events[i].data.ptr)->sock->getFd();
                LOG_WARN("An error has occured on epoll monitored file descriptor, closing fd: " << fd);
                close(fd);
                delete (EpollEvent*) events[i].data.ptr;
                continue;
            }

            // run callback
            ((EpollEvent*)events[i].data.ptr)->clbk( (EpollEvent*)events[i].data.ptr );

        }
    }

}
