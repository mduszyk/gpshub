#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "user/dstypes.h"
#include "socket/Epoll.h"
#include "server/CmdPkg.h"
#include "user/User.h"
#include "user/UserIdGenerator.h"
#include "server/Session.h"


class CommandHandler {

    public:
        CommandHandler(IdUserMap* id_umap, NickUserMap* nick_umap,
            UserIdGenerator* idgen);
        virtual ~CommandHandler();
        void handle(CmdPkg* pkg, Session* event);
        void quit(User* u);

    private:
        IdUserMap* id_umap;
        NickUserMap* nick_umap;
        UserIdGenerator* idgen;
        void registerNick(CmdPkg* pkg, Session* event);
        void addBuddies(CmdPkg* pkg, Session* event);
        void removeBuddies(CmdPkg* pkg, Session* event);

};

#endif // COMMANDPROCESSOR_H
