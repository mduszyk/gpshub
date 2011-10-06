#include "thread/ScopeLockWr.h"

ScopeLockWr::ScopeLockWr(RwLock& l) : lock(l) {
    lock.wrlock();
}

ScopeLockWr::~ScopeLockWr() {
    lock.unlock();
}
