#include <iostream>
#include <exception>
#include <unistd.h>
#include "server/CommandServer.h"
#include "server/GpsDataServer.h"
#include "hubthread/ServerThread.h"
#include "socket/SocketException.h"
#include "thread/BlockingQueue.h"
#include "hubthread/CoordsBroadcastThread.h"
#include "server/CommandHandler.h"
#include "user/dstypes.h"
#include "user/UserIdGenerator.h"
#include "log/macros.h"


using namespace std;

int main(int argc, char *argv[]) {
    char* port_cmd = "9990";
    char* port_gps = "9991";

    if (argc == 3) {
        port_cmd = argv[1];
        port_gps = argv[2];
    }

    // debug is not compiled with release version
    FILELog::ReportingLevel() = FILELog::FromString("DEBUG2");

    LOG_INFO("Starting gpshub, cmd: " << port_cmd << ", gps: " << port_gps);

    // id user map: id -> user
    IdUserMap id_umap;
    // nick user map: nick -> user
    NickUserMap nick_umap;

    UserIdGenerator idgen(&id_umap);

    // user update queue
    BlockingQueue<int> uqueue;

    CommandHandler cmdHandler(&id_umap, &nick_umap, &idgen);
    CommandServer cmdsrv(port_cmd, &id_umap, &cmdHandler);
    GpsDataServer gpssrv(port_gps, &id_umap, &uqueue);

    ServerThread sthread(&gpssrv);

    CoordsBroadcastThread bthread(&id_umap, &nick_umap, &uqueue, gpssrv.getUdpSocket());
    bthread.start();

    sthread.start();
    try {
        cmdsrv.loop();
    } catch (exception& e) {
        LOG_ERROR("Server loop error: " << e.what());
        return 1;
    }

    return 0;
}
