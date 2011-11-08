#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include "server/Server.h"
#include "server/CommandHandler.h"
#include "socket/Socket.h"
#include "socket/Epoll.h"
#include "thread/SyncMap.h"
#include "user/User.h"
#include "server/Session.h"


class CommandServer : public Server {

    public:
        CommandServer(char* port, IdUserMap* umap, CommandHandler* cmdHandler);
        virtual ~CommandServer();

        void loop();
        void stop();

        static void incomingConnectionClbk(EpollEvent* event);
        static void incomingDataClbk(EpollEvent* event);
        static void closeConnectionClbk(EpollEvent* event);

    private:
        Socket* tcpSocket;
        IdUserMap* umap;
        CommandHandler* cmdHandler;
        Epoll* epl;
        char* read_buf;

        void incomingConnection(EpollEvent* event);
        void incomingData(EpollEvent* event);
        void closeConnection(EpollEvent* event);

};

#endif // COMMANDSERVER_H
