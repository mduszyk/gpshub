#include "thread/ThreadSyncException.h"

ThreadSyncException::ThreadSyncException(const char* message)
    : ThreadException(message) {}

ThreadSyncException::ThreadSyncException(const std::string& message)
    : ThreadException(message) {}
