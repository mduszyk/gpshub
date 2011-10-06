#ifndef SCOPELOCKWR_H
#define SCOPELOCKWR_H

#include "thread/RwLock.h"

/*
cpp spuuports RAII instead of finally so locking/unlocking
is done in constructor/destructor
*/
class ScopeLockWr {

    public:
        ScopeLockWr(RwLock& l);
        virtual ~ScopeLockWr();

    private:
        RwLock& lock;

};

#endif // SCOPELOCKWR_H
