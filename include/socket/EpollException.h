#ifndef EPOLLEXCEPTION_H
#define EPOLLEXCEPTION_H

#include <stdexcept>
using namespace std;

class EpollException : public std::runtime_error {

    public:
        EpollException(char* message);
        EpollException(const string& message);

};

#endif // EPOLLEXCEPTION_H
