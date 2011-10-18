#ifndef CONDITIONVARIABLE_H
#define CONDITIONVARIABLE_H

#define _MULTI_THREADED
#include <pthread.h>
#include "thread/ScopeLock.h"

class ConditionVariable {

    public:
        ConditionVariable();
        virtual ~ConditionVariable();
        void signal();
        void wait(Mutex& mtx);
        void wait(ScopeLock& lock);

    private:
        pthread_cond_t cond;

};

#endif // CONDITIONVARIABLE_H
