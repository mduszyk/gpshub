#ifndef RWLOCK_H
#define RWLOCK_H

#define _MULTI_THREADED
#include <pthread.h>

/*
reader/writer lock
Multiple readers can lock such an object
without blocking each other, but when a
single writer acquires the lock it has
exclusive access to the resource.
*/
class RwLock {

    public:
        RwLock();
        virtual ~RwLock();
        void wrlock();
        void rdlock();
        void unlock();

    private:
        pthread_rwlock_t lock;

};

#endif // RWLOCK_H
