#include "thread/ThreadSyncException.h"

ThreadSyncException::ThreadSyncException(char* message) : ThreadException(message)
{
    //ctor
}

ThreadSyncException::ThreadSyncException(const string& message) : ThreadException(message)
{
    //dtor
}
