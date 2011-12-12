#include "thread/ThreadException.h"

ThreadException::ThreadException(const char* message)
    : std::runtime_error(message) {}

ThreadException::ThreadException(const std::string& message)
    : std::runtime_error(message) {}
