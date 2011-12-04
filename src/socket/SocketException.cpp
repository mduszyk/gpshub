#include "socket/SocketException.h"

SocketException::SocketException(char* message)  : std::runtime_error(message) {
    this->Errno = errno;
}

SocketException::SocketException(const std::string& message)
: std::runtime_error(message) {
    this->Errno = errno;
}

int SocketException::getErrno() {
    return Errno;
}
