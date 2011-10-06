#ifndef SCOPELOCKRD_H
#define SCOPELOCKRD_H

#include "thread/RwLock.h"

/*
cpp spuuports RAII instead of finally so locking/unlocking
is done in constructor/destructor
*/
class ScopeLockRd {

    public:
        ScopeLockRd(RwLock& l);
        virtual ~ScopeLockRd();

    private:
        RwLock& lock;

};

#endif // SCOPELOCKRD_H
