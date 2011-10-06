#include "server/CmdPkg.h"
#include "stdlib.h"
#include "socket/netutil.h"
#include <string.h>


CmdPkg::CmdPkg(char* buf) {
    short len = toshort(buf, 1);
    bytes = (char*) malloc(len + 1);
    bytes[len] = '\0';

    memcpy(bytes, buf, len);
}

CmdPkg::CmdPkg(char type, short len) {
    bytes = (char*) malloc(len + 1);
    bytes[len] = '\0';

    bytes[0] = type;
    tobytes(bytes, 1, len);
}

CmdPkg::~CmdPkg() {
    free(bytes);
}

char CmdPkg::getType() {
    return bytes[0];
}

short CmdPkg::getLen() {
    return toshort(bytes, 1);
}

char* CmdPkg::getData() {
    return bytes + 3;
}

char* CmdPkg::getBytes() {
    return bytes;
}

void CmdPkg::setInt(int data_index, int val) {
    tobytes(getData(), data_index, val);
}

void CmdPkg::setShort(int data_index, short val) {
    tobytes(getData(), data_index, val);
}
