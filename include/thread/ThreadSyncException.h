#ifndef THREADSYNCEXCEPTION_H
#define THREADSYNCEXCEPTION_H

#include "thread/ThreadException.h"

class ThreadSyncException : public ThreadException
{
    public:
        ThreadSyncException(const char* message);
        ThreadSyncException(const std::string& message);

};

#endif // THREADSYNCEXCEPTION_H
