#ifndef COMMANDSERVER_H
#define COMMANDSERVER_H

#include "server/Server.h"
#include "server/CommandHandler.h"
#include "socket/Socket.h"
#include "socket/Epoll.h"
#include "thread/SyncMap.h"
#include "user/User.h"

#define CMDPKG_MAX 512

class CommandServer : public Server {

    public:
        CommandServer(char* port, IdUserMap* umap, CommandHandler* cmdHandler);
        virtual ~CommandServer();

        void loop();
        void stop();

        static void incomingConnectionClbk(EpollEvent* event);
        static void clientSocketClbk(EpollEvent* event);
        static void closeConnectionClbk(EpollEvent* event);

        void send(Session* s, CmdPkg* p);

    private:
        Socket* tcpSocket;
        IdUserMap* umap;
        CommandHandler* cmdHandler;
        Epoll* epl;
        char cmd_buf[CMDPKG_MAX];

        void incomingConnection(EpollEvent* event);
        void incomingData(EpollEvent* event);
        void closeConnection(EpollEvent* event);
        void send(EpollEvent* event);

};

#endif // COMMANDSERVER_H
