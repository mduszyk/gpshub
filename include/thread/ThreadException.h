#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H

#include <stdexcept>

class ThreadException : public std::runtime_error {

    public:
        ThreadException(char* message);
        ThreadException(const std::string& message);

};

#endif // THREADEXCEPTION_H
