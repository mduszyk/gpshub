#include "thread/ScopeLock.h"

ScopeLock::ScopeLock(Mutex& m) : mtx(m) {
    mtx.lock();
}

ScopeLock::~ScopeLock() {
    mtx.unlock();
}

pthread_mutex_t* ScopeLock::getPthreadPtr() {
    return mtx.getPthreadPtr();
}
