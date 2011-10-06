#ifndef SERVERTHREAD_H
#define SERVERTHREAD_H

#include "thread/Thread.h"
#include "server/Server.h"


class ServerThread : public Thread
{
    public:
        ServerThread(Server* srv);
        virtual ~ServerThread();
        void run();
    protected:
    private:
        Server *srv;
};

#endif // SERVERTHREAD_H
