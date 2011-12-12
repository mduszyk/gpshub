#include "socket/EpollException.h"

EpollException::EpollException(const char* message)
        : std::runtime_error(message) {
    this->Errno = errno;
}

EpollException::EpollException(const std::string& message)
        : std::runtime_error(message) {
    this->Errno = errno;
}

int EpollException::getErrno() {
    return Errno;
}
