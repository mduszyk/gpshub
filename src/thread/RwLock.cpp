#include "thread/RwLock.h"
#include "thread/ThreadSyncException.h"

RwLock::RwLock() {
    int i;
    if ((i = pthread_rwlock_init(&lock, NULL)) != 0) {
        throw ThreadSyncException("Failed initializing rwlock, return value: " + i);
    }
}

RwLock::~RwLock() {
    int i;
    if ((i = pthread_rwlock_destroy(&lock)) != 0) {
        throw ThreadSyncException("Failed destroying rwlock, return value: " + i);
    }
}

void RwLock::wrlock() {
    int i;
    if ((i = pthread_rwlock_wrlock(&lock)) != 0) {
        throw ThreadSyncException("Failed write locking rwlock, return value: " + i);
    }
}

void RwLock::rdlock() {
    int i;
    if ((i = pthread_rwlock_rdlock(&lock)) != 0) {
        throw ThreadSyncException("Failed read locking rwlock, return value: " + i);
    }
}

void RwLock::unlock() {
    int i;
    if ((i = pthread_rwlock_unlock(&lock)) != 0) {
        throw ThreadSyncException("Failed unlocking rwlock, return value: " + i);
    }
}
