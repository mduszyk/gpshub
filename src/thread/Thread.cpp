#include <pthread.h>
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
    if ((i = pthread_create(&threadId, NULL, Thread::entryPoint, (void*)this)) != 0) {
        throw ThreadException("Creating thread failed, return code: " + i);
    }
}

void Thread::run() {
   // Your code goes here
}

/*static */
void* Thread::entryPoint(void * pthis) {
   Thread* t = (Thread*)pthis;
   t->run();
   return NULL;
}
