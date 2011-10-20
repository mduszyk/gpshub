#ifndef EPOLLEXCEPTION_H
#define EPOLLEXCEPTION_H

#include <stdexcept>
#include <errno.h>

class EpollException : public std::runtime_error {

    public:
        EpollException(char* message);
        EpollException(const std::string& message);
        int getErrno();

    private:
        int Errno;

};

#endif // EPOLLEXCEPTION_H
