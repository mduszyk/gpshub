#include "thread/ThreadSyncException.h"

ThreadSyncException::ThreadSyncException(const char* message) : ThreadException(message)
{
    //ctor
}

ThreadSyncException::ThreadSyncException(const std::string& message) : ThreadException(message)
{
    //dtor
}
