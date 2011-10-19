#include "thread/ConditionVariable.h"
#include "thread/ThreadSyncException.h"

ConditionVariable::ConditionVariable() {
    int i;
    if ((i = pthread_cond_init(&cond, NULL)) != 0) {
        throw ThreadSyncException("Failed initializing condition variable, return value: " + i);
    }
}

ConditionVariable::~ConditionVariable() {
    int i;
    if ((i = pthread_cond_destroy(&cond)) != 0) {
        throw ThreadSyncException("Failed destroying condition variable, return value: " + i);
    }
}

void ConditionVariable::signal() {
    int i;
    if ((i = pthread_cond_signal(&cond)) != 0) {
        throw ThreadSyncException("Failed signalling condition variable, return value: " + i);
    }
}

void ConditionVariable::wait(Mutex& mtx) {
    int i;
    if ((i = pthread_cond_wait(&cond, mtx.getPthreadPtr())) != 0) {
        throw ThreadSyncException("Failed waiting on condition variable, return value: " + i);
    }
}

void ConditionVariable::wait(ScopeLock& lock) {
    int i;
    if ((i = pthread_cond_wait(&cond, lock.getPthreadPtr())) != 0) {
        throw ThreadSyncException("Failed waiting on condition variable, return value: " + i);
    }
}
