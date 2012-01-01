#ifndef COMPONENTREGISTRY_H
#define COMPONENTREGISTRY_H

#include <iostream>
#include "server/CommandServer.h"
#include "server/GpsDataServer.h"
#include "hubthread/CoordsBroadcastThread.h"

class ComponentRegistry {

    public:

        static void setCommandServer(CommandServer* cmd_server);
        static CommandServer* getCommandServer();

        static void setGpsDataServer(GpsDataServer* gps_server);
        static GpsDataServer* getGpsDataServer();

        static void setBroadcastThreads(
            std::vector<CoordsBroadcastThread>* bthreads);
        static std::vector<CoordsBroadcastThread>* getBroadcastThreads();

    private:

        ComponentRegistry();
        virtual ~ComponentRegistry();

        static CommandServer* cmd_server;
        static GpsDataServer* gps_server;
        static std::vector<CoordsBroadcastThread>* bthreads;

};

#endif // COMPONENTREGISTRY_H
