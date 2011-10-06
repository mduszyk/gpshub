#ifndef GPSDATASERVER_H
#define GPSDATASERVER_H

#include "server/Server.h"
#include "socket/Socket.h"
#include "thread/BlockingQueue.h"
#include "user/dstypes.h"
#include "user/User.h"

class GpsDataServer : public Server {

    public:
        GpsDataServer(char* port, IdUserMap* umap, BlockingQueue<int>* uqueue);
        virtual ~GpsDataServer();
        void loop();
        void stop();
        Socket* getUdpSocket();

    private:
        Socket* udpSocket;
        IdUserMap* umap;
        BlockingQueue<int>* uqueue;
        bool listen;
        static const int buf_len = 128;
        char buf[buf_len];
        struct sockaddr_storage their_addr;
        int n;
        void processCoordinates();
        void initAddrUdp();

};

#endif // GPSDATASERVER_H
