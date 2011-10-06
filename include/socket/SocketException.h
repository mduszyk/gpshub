#ifndef SOCKETEXCEPTION_H
#define SOCKETEXCEPTION_H

#include <errno.h>
#include <stdexcept>
using namespace std;

class SocketException : public std::runtime_error {

    public:
        SocketException(char* message);
        SocketException(const string& message);
        int getErrno();

    private:
        int Errno;

};

#endif // SOCKETEXCEPTION_H
