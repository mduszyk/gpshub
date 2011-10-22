#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include "server/Server.h"
#include "server/CommandHandler.h"
#include "socket/Socket.h"
#include "socket/Epoll.h"
#include "thread/SyncMap.h"
#include "user/User.h"
#include "server/EventData.h"


class CommandServer : public Server {

    public:
        CommandServer(char* port, IdUserMap* umap, CommandHandler* cmdHandler);
        virtual ~CommandServer();
        void loop();
        void stop();
        void incomingConnection(EpollEvent* event);
        void incomingData(EpollEvent* event);
        static void incomingConnectionClbk(EpollEvent* event);
        static void incomingDataClbk(EpollEvent* event);

    private:
        Socket* tcpSocket;
        IdUserMap* umap;
        CommandHandler* cmdHandler;
        Epoll* epl;
        char* read_buf;

};

#endif // COMMANDSERVER_H
