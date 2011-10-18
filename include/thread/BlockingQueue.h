#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#define _MULTI_THREADED
#include <pthread.h>
#include <queue>
#include "thread/Mutex.h"
#include "thread/ConditionVariable.h"


template<class T>
class BlockingQueue {

    public:
        BlockingQueue();
        virtual ~BlockingQueue();
        void put(T t);
        T pull();

    private:
        std::queue<T> queue;
//        pthread_cond_t cond;
//        pthread_mutex_t lock;
        Mutex mtx;
        ConditionVariable cond;

};

// The declarations and definitions of the class template
// member functions should all be in the same header file.

template<class T>
BlockingQueue<T>::BlockingQueue() {
//    pthread_mutex_init(&lock, NULL);
//    pthread_cond_init(&cond, NULL);
}

template<class T>
BlockingQueue<T>::~BlockingQueue() {
//    pthread_mutex_destroy(&lock);
//    pthread_cond_destroy(&cond);
}

template<class T>
void BlockingQueue<T>::put(T t) {
//    pthread_mutex_lock(&lock);
    ScopeLock lock(mtx);
    queue.push(t);
    cond.signal();
//    pthread_cond_signal(&cond);
//    pthread_mutex_unlock(&lock);
}

template<class T>
T BlockingQueue<T>::pull() {
//    pthread_mutex_lock(&lock);
    ScopeLock lock(mtx);
    while(queue.empty()) {
//        pthread_cond_wait(&cond, &lock);
        cond.wait(lock);
    }

    T t = queue.front();
    queue.pop();

//    pthread_mutex_unlock(&lock);

    return t;
}

#endif // BLOCKINGQUEUE_H
