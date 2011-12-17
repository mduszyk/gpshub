#ifndef GPSDATASERVER_H
#define GPSDATASERVER_H

#include <list>
#include "server/Server.h"
#include "server/CmdPkg.h"
#include "socket/Socket.h"
#include "thread/BlockingQueue.h"
#include "user/dstypes.h"
#include "user/User.h"
#include "socket/Epoll.h"

struct PendingCmd {
    CmdPkg* cmd;
    Socket* sock;
};

class GpsDataServer : public Server {

    public:
        GpsDataServer(const char* port, IdUserMap* umap,
                      BlockingQueue<int>* uqueue);
        virtual ~GpsDataServer();
        void loop();
        void stop();
        Socket* getUdpSocket();

        static void timeoutClbk();
        static void incomingDataClbk(EpollEvent* event);

    private:
        static const int BUF_LEN = 128;
        Socket* udpSocket;
        Epoll* epl;
        IdUserMap* umap;
        BlockingQueue<int>* uqueue;
        char buf[BUF_LEN];
        struct sockaddr_storage their_addr;
        int n;
        void processCoordinates();
        void initAddrUdp();
        void sendUdpInitAck(User* u, char status);

        void incomingData(EpollEvent* event);

        std::list<PendingCmd*> pending_cmd_queue;
        void sendPendingCmd();

};

#endif // GPSDATASERVER_H
