#include "thread/ThreadException.h"

ThreadException::ThreadException(char* message) : std::runtime_error(message) {
    //ctor
}

ThreadException::ThreadException(const std::string& message) : std::runtime_error(message)
{
    //ctor2
}
