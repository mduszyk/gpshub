#ifndef COMPONENTREGISTRY_H
#define COMPONENTREGISTRY_H

#include "server/CommandServer.h"
#include "server/GpsDataServer.h"

class ComponentRegistry {

    public:
        static void setCommandServer(CommandServer* cmd_server);
        static CommandServer* getCommandServer();
        static void setGpsDataServer(GpsDataServer* gps_server);
        static GpsDataServer* getGpsDataServer();

    private:
        ComponentRegistry();
        virtual ~ComponentRegistry();

        static CommandServer* cmd_server;
        static GpsDataServer* gps_server;

};

#endif // COMPONENTREGISTRY_H
