#ifndef MUTEX_H
#define MUTEX_H

#define _MULTI_THREADED
#include <pthread.h>

class Mutex {

    public:
        Mutex();
        virtual ~Mutex();
        void lock();
        void unlock();
        pthread_mutex_t* getPthreadPtr();

    private:
        pthread_mutex_t mtx;

};

#endif // MUTEX_H
