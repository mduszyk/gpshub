#include "thread/ConditionVariable.h"

ConditionVariable::ConditionVariable() {
    pthread_cond_init(&cond, NULL);
}

ConditionVariable::~ConditionVariable() {
    pthread_cond_destroy(&cond);
}

void ConditionVariable::signal() {
    pthread_cond_signal(&cond);
}

void ConditionVariable::wait(Mutex& mtx) {
    pthread_cond_wait(&cond, mtx.getPthreadPtr());
}

void ConditionVariable::wait(ScopeLock& lock) {
    pthread_cond_wait(&cond, lock.getPthreadPtr());
}
