#include "thread/ScopeLockRd.h"

ScopeLockRd::ScopeLockRd(RwLock& l) : lock(l) {
    lock.rdlock();
}

ScopeLockRd::~ScopeLockRd() {
    lock.unlock();
}
