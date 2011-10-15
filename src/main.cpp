#include <iostream>
#include <exception>
#include <unistd.h>
#include <getopt.h>
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


void print_help(char* argv[]);
void start_gpshub(char* port_cmd, char* port_gps);

int main(int argc, char *argv[]) {
    // defaults
    char* port_cmd = "9990";
    char* port_gps = "9991";
    char* log_level = "DEBUG2";
    char* log_file = NULL;

    static struct option long_options[] = {
       {"tcp",       required_argument, 0, 't'},
       {"udp",       required_argument, 0, 'u'},
       {"log-level", required_argument, 0, 'l'},
       {"log-file",  required_argument, 0, 'f'},
       {"help",      no_argument,       0, 'h'},
       {0, 0, 0, 0}
    };
    // getopt_long stores the option index here
    int option_index = 0;

    int opt;
    while((opt = getopt_long(argc, argv, "t:u:l:f:h", long_options, &option_index)) != -1)
        switch(opt) {
            case 't':
                port_cmd = optarg;
                break;
            case 'u':
                port_gps = optarg;
                break;
            case 'l':
                log_level = optarg;
                break;
            case 'f':
                log_file = optarg;
                break;
            case 'h':
                print_help(argv);
                break;
            case ':':
            case '?':
                print_help(argv);
         }

    if (optind < argc)
        print_help(argv);

    // debug is not compiled with release version
    FILELog::ReportingLevel() = FILELog::FromString(log_level);

    start_gpshub(port_cmd, port_gps);

    exit(0);
}

void print_help(char* argv[]) {
    printf("Usage:\n");
    printf("%s [-t TCP_PORT] [-u UDP_PORT] [-l LOG_LEVEL] [-f LOG_FILE] [-h]\n\n", argv[0]);

    printf("  -t, --tcp\n    set tcp port\n");
    printf("  -u, --udp\n    set udp port\n\n");

    printf("  -l, --log-level\n    set min log level\n");
    printf("  -f, --log-file\n    set log output file\n\n");

    printf("  -h, --help\n    print help\n\n");

    exit(1);
}

void start_gpshub(char* port_cmd, char* port_gps) {
    LOG_INFO("Starting gpshub...");

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
        exit(1);
    }
}
