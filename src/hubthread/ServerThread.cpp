#include <exception>
#include <iostream>
#include "hubthread/ServerThread.h"
#include "util/log.h"

using namespace std;

ServerThread::ServerThread(Server* srv) {
    //ctor
    this->srv = srv;
}

ServerThread::~ServerThread() {
    //dtor
}

void ServerThread::run() {
    try {
        // run server loop
        this->srv->loop();
    } catch (exception& e) {
        LOG_ERROR("Server loop error: " << e.what());
    }
}
