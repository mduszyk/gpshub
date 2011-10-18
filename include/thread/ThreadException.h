#ifndef THREADEXCEPTION_H
#define THREADEXCEPTION_H

#include <stdexcept>
using namespace std;

class ThreadException : public std::runtime_error {

    public:
        ThreadException(char* message);
        ThreadException(const string& message);

};

#endif // THREADEXCEPTION_H
