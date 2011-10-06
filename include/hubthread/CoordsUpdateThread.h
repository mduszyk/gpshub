#ifndef COORDSUPDATETHREAD_H
#define COORDSUPDATETHREAD_H

#include "thread/Thread.h"

/**
    Sends friends positions to user
*/
class CoordsUpdateThread : public Thread
{
    public:
        CoordsUpdateThread();
        virtual ~CoordsUpdateThread();
    protected:
    private:
};

#endif // COORDSUPDATETHREAD_H
