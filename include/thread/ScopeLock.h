#ifndef SCOPELOCK_H
#define SCOPELOCK_H

#include "thread/Mutex.h"

class ScopeLock {

    public:
        ScopeLock(Mutex& m);
        virtual ~ScopeLock();
        pthread_mutex_t* getPthreadPtr();

    private:
        Mutex& mtx;

};

#endif // SCOPELOCK_H
