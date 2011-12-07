#ifndef COORDSBROADCASTTHREAD_H
#define COORDSBROADCASTTHREAD_H

#include "thread/Thread.h"
#include "thread/BlockingQueue.h"
#include "user/dstypes.h"
#include "user/User.h"
#include "socket/Socket.h"

/**
    Broadcasts user's position to his friends
*/
class CoordsBroadcastThread : public Thread {

    public:
        CoordsBroadcastThread(IdUserMap* id_umap, NickUserMap* nick_umap,
                              BlockingQueue<int>* uqueue, Socket* udpSocket);
        virtual ~CoordsBroadcastThread();
        void run();

    private:
        static const int BUF_LEN = 16;

        BlockingQueue<int>* uqueue;
        IdUserMap* id_umap;
        NickUserMap* nick_umap;
        Socket* udpSocket;
        char buf[BUF_LEN];
        void broadcast(User* u, Coordinates* coords);

};

#endif // COORDSBROADCASTTHREAD_H
