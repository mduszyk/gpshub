#include "thread/Thread.h"
#include "thread/ThreadException.h"

Thread::Thread() {
    //ctor
}

Thread::~Thread() {
    //dtor
}

void Thread::start() {
    int i;
    if ((i = pthread_create(&threadId, NULL, Thread::entryPoint, (void*)this))
            != 0) {
        if (EAGAIN == i) {
            throw ThreadException("Failed creating thread, EAGAIN: "
                "Insufficient resources to create another thread");
        }
        if (EPERM == i) {
            throw ThreadException("Failed creating thread, EPERM: "
                "No permission to set the scheduling policy");
        }
        if (EINVAL == i) {
            throw ThreadException("Failed creating thread, EINVAL: "
                "Invalid settings in attr");
        }
        throw ThreadException("Failed creating thread, return value: " + i);
    }
}

void Thread::run() {
   // Your code goes here
}

void Thread::join() {
    int i;
    if ((i = pthread_join(threadId, &retval)) != 0) {
        throw ThreadException("Failed joining thread, error: " + i);
    }
}

/* static */
void* Thread::entryPoint(void * pthis) {
   Thread* t = (Thread*)pthis;
   t->run();
   return NULL;
}
