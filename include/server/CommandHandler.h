#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include "user/dstypes.h"
#include "socket/Epoll.h"
#include "server/CmdPkg.h"
#include "user/User.h"
#include "user/UserIdGenerator.h"
#include "server/EventData.h"


class CommandHandler {

    public:
        CommandHandler(IdUserMap* id_umap, NickUserMap* nick_umap, UserIdGenerator* idgen);
        virtual ~CommandHandler();
        void handle(CmdPkg* pkg, EventData* event);
        void quit(User* u);

    private:
        IdUserMap* id_umap;
        NickUserMap* nick_umap;
        UserIdGenerator* idgen;
        void registerNick(CmdPkg* pkg, EventData* event);
        void addBuddies(CmdPkg* pkg, EventData* event);
        void removeBuddies(CmdPkg* pkg, EventData* event);

};

#endif // COMMANDPROCESSOR_H
