#include "thread/RwLock.h"

RwLock::RwLock() {
    pthread_rwlock_init(&lock, NULL);
}

RwLock::~RwLock() {
    pthread_rwlock_destroy(&lock);
}

void RwLock::wrlock() {
    pthread_rwlock_wrlock(&lock);
}

void RwLock::rdlock() {
    pthread_rwlock_rdlock(&lock);
}

void RwLock::unlock() {
    pthread_rwlock_unlock(&lock);
}
