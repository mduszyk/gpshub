#include "thread/Mutex.h"
#include "thread/ThreadSyncException.h"

Mutex::Mutex() {
    int i;
    if ((i = pthread_mutex_init(&mtx, NULL)) != 0) {
        if (ENOMEM == i) {
            throw ThreadSyncException("Failed initializing mutex, ENOMEM: "
                "system cannot allocate resources");
        }
        if (EINVAL == i) {
            throw ThreadSyncException("Failed initializing mutex, EINVAL: "
                "argument is not correct");
        }
        throw ThreadSyncException("Failed initializing mutex, return value: "
            + i);
    }
}

Mutex::~Mutex() {
    int i;
    if ((i = pthread_mutex_destroy(&mtx)) != 0) {
        if (EBUSY == i) {
            throw ThreadSyncException("Failed destroying mutex, EBUSY: "
                "mutex is currently owned by another thread");
        }
        if (EINVAL == i) {
            throw ThreadSyncException("Failed destroying mutex, EINVAL: "
                "argument is not correct");
        }
        throw ThreadSyncException("Failed destroying mutex, return value: "
            + i);
    }
}

void Mutex::lock() {
    int i;
    if ((i = pthread_mutex_lock(&mtx)) != 0) {
        if (EINVAL == i) {
            throw ThreadSyncException("Failed locking mutex, EINVAL: "
                "argument is not correct");
        }
        //if (EDESTROYED == i) {
        //    throw ThreadSyncException("Failed locking mutex, EDESTROYED: "
        //        "while waiting for the mutex lock, the mutex was destroyed");
        //}
        //if (EOWNERTERM == i) {
        //    throw ThreadSyncException("Failed locking mutex, EOWNERTERM: "
        //        "thread terminated the holding of the mutex, and the mutex is an ownerterm mutex type");
        //}
        if (EDEADLK == i) {
            throw ThreadSyncException("Failed locking mutex, EDEADLK: "
                "a thread attempted to relock an already held mutex, "
                "and the mutex is an errorcheck mutex type");
        }
        //if (ERECURSE == i) {
        //    throw ThreadSyncException("Failed locking mutex, ERECURSE: "
        //        "recursive mutex cannot be recursively locked again");
        //}
        throw ThreadSyncException("Failed locking mutex, return value: " + i);
    }
}

void Mutex::unlock() {
    int i;
    if ((i = pthread_mutex_unlock(&mtx)) != 0) {
        if (EINVAL == i) {
            throw ThreadSyncException("Failed unlocking mutex, EINVAL: "
                "argument is not correct");
        }
        if (EPERM == i) {
            throw ThreadSyncException("Failed unlocking mutex, EPERM: "
                "mutex is not currently held by the caller");
        }
        throw ThreadSyncException("Failed unlocking mutex, return value: " + i);
    }
}

pthread_mutex_t* Mutex::getPthreadPtr() {
    return &mtx;
}
