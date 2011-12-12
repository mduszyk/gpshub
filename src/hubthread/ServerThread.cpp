#include <exception>
#include <iostream>
#include "hubthread/ServerThread.h"
#include "log/macros.h"


ServerThread::ServerThread(Server* srv) {
    this->srv = srv;
}

ServerThread::~ServerThread() {}

void ServerThread::run() {
    try {
        // run server loop
        this->srv->loop();
    } catch (std::exception& e) {
        LOG_ERROR("Server error: " << e.what());
    }
}
