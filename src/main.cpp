#include <unistd.h>
#include <getopt.h>
#include <iostream>
#include <exception>

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

#define VERSION "0.7.1_beta"

#define DEFAULT_TCP "9990"
#define DEFAULT_UDP "9991"
#define DEFAULT_LOG_LEVEL "DEBUG2"
#define DEFAULT_LOG_FILE "stdout"
#define DEFAULT_THREAD_NUM 1


void print_help(char* argv[]);
void print_version();
void start_gpshub(const char* port_cmd, const char* port_gps, int num_thread);

int main(int argc, char *argv[]) {
    const char* port_cmd = DEFAULT_TCP;
    const char* port_gps = DEFAULT_UDP;
    const char* log_level = DEFAULT_LOG_LEVEL;
    const char* log_file = DEFAULT_LOG_FILE;
    int thread_num = DEFAULT_THREAD_NUM;

    static struct option long_options[] = {
       {"tcp",           required_argument, 0, 't'},
       {"udp",           required_argument, 0, 'u'},
       {"log-level",     required_argument, 0, 'l'},
       {"log-file",      required_argument, 0, 'f'},
       {"thread-number", required_argument, 0, 'n'},
       {"version",       no_argument,       0, 'v'},
       {"help",          no_argument,       0, 'h'},
       {0, 0, 0, 0}
    };
    // getopt_long stores the option index here
    int option_index = 0;

    int opt;
    while((opt = getopt_long(argc, argv, "t:u:l:f:n:vh", long_options,
                            &option_index)) != -1)
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
            case 'n':
                thread_num = atoi(optarg);
                if (thread_num < 1) {
                    fprintf(stderr, "ERROR: Invalid thread number parameter: '%s'\n", optarg);
                    print_help(argv);
                }
                break;
            case 'v':
                print_version();
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

    FILE* log_fd;
    if (strcmp("stdout", log_file) == 0) {
        log_fd = stdout;
    } else if(strcmp("stderr", log_file) == 0) {
        log_fd = stderr;
    } else {
        log_fd = fopen(log_file, "w");
        if (log_fd == NULL) {
            fprintf(stderr, "ERROR: Can't open log file %s", log_file);
            exit(1);
        }
    }
    // set logger's output file
    Output2FILE::Stream() = log_fd;

    start_gpshub(port_cmd, port_gps, thread_num);

    fclose(log_fd);
    exit(0);
}

void print_help(char* argv[]) {
    printf("Usage:\n");
    printf("%s [-t TCP_PORT] [-u UDP_PORT] [-l LOG_LEVEL] [-f LOG_FILE] [-n THREAD_NUMBER] [-v] [-h]\n\n", argv[0]);

    printf("  -t, --tcp\n");
    printf("      set tcp port, default: %s\n", DEFAULT_TCP);
    printf("  -u, --udp\n");
    printf("      set udp port, default: %s\n\n", DEFAULT_UDP);

    printf("  -l, --log-level\n");
    printf("      set log level, default: %s\n", DEFAULT_LOG_LEVEL);
    printf("      available levels: ERROR, WARNING, INFO, DEBUG, DEBUG1, DEBUG2\n");
    printf("  -f, --log-file\n");
    printf("      set log output file, default: %s\n", DEFAULT_LOG_FILE);
    printf("      accepted values: stdout, stderr, filename\n\n");

    printf("  -n, --thread-number\n");
    printf("      set number of consumer threads, default: %d\n",
           DEFAULT_THREAD_NUM);
    printf("      accepted values grather than 0\n\n");

    printf("  -v, --version\n");
    printf("      print version\n\n");

    printf("  -h, --help\n");
    printf("      print this help\n\n");

    exit(1);
}

void print_version() {
#ifdef DEBUG
    printf("%s DEBUG\n", VERSION);
#else
    printf("%s\n", VERSION);
#endif
    exit(0);
}

CommandServer* global_cmdsrv;
GpsDataServer* global_gpssrv;
std::vector<CoordsBroadcastThread>* global_bthreads;

void start_gpshub(const char* port_cmd, const char* port_gps, int thread_num) {
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

    std::vector<CoordsBroadcastThread> bthreads(thread_num,
        CoordsBroadcastThread(&id_umap, &nick_umap, &uqueue,
                              gpssrv.getUdpSocket()));

    // start broadcasting threads
    for (int i = 0; i < thread_num; i++) {
        bthreads[i].start();
    }

    // start gps data server
    sthread.start();

    // start command server
    try {
        cmdsrv.loop();
    } catch (std::exception& e) {
        LOG_ERROR("Server loop error: " << e.what());
        exit(1);
    }

}
