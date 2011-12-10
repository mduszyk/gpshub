#include "ComponentRegistry.h"

ComponentRegistry::ComponentRegistry() {}

ComponentRegistry::~ComponentRegistry() {}

// define static fields - allocate space for them
CommandServer* ComponentRegistry::cmd_server;
GpsDataServer* ComponentRegistry::gps_server;

void ComponentRegistry::setCommandServer(CommandServer* cmd_server) {
    ComponentRegistry::cmd_server = cmd_server;
}

CommandServer* ComponentRegistry::getCommandServer() {
    return cmd_server;
}

void ComponentRegistry::setGpsDataServer(GpsDataServer* gps_server) {
    ComponentRegistry::gps_server = gps_server;
}

GpsDataServer* ComponentRegistry::getGpsDataServer() {
    return gps_server;
}
