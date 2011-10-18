#ifndef THREAD_H
#define THREAD_H

#define _MULTI_THREADED
#include <pthread.h>

class Thread
{
   public:
      Thread();
      virtual ~Thread();
      void start();

   protected:
      virtual void run();
      static void * entryPoint(void*);
      pthread_t threadId;

};

#endif // THREAD_H
