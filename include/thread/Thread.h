#include <pthread.h>

#ifndef THREAD_H
#define THREAD_H


class Thread
{
   public:
      Thread();
      virtual ~Thread();
      void start();
   protected:
      virtual void run();
      static void * entryPoint(void*);
   private:
      pthread_t threadId;

};

#endif // THREAD_H
