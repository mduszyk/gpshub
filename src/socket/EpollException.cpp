#include "socket/EpollException.h"

EpollException::EpollException(char* message) : std::runtime_error(message) {
    //ctor
}

EpollException::EpollException(const string& message) : std::runtime_error(message) {
    //ctor2
}
