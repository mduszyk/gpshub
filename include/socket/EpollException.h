#ifndef EPOLLEXCEPTION_H
#define EPOLLEXCEPTION_H

#include <stdexcept>
#include <errno.h>
using namespace std;

class EpollException : public std::runtime_error {

    public:
        EpollException(char* message);
        EpollException(const string& message);
        int getErrno();

    private:
        int Errno;

};

#endif // EPOLLEXCEPTION_H
