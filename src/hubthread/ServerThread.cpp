#include <exception>
#include <iostream>
#include "hubthread/ServerThread.h"
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
        cerr << "Server loop error: " << e.what() << endl;
    }
}
