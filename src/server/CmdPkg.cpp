#include "server/CmdPkg.h"
#include "stdlib.h"
#include "socket/netutil.h"
#include <string.h>


CmdPkg::CmdPkg(char* buf) {
    is_extbuf = true;
    bytes = buf;
    bytes[getLen()] = '\0';
}

CmdPkg::CmdPkg(char type, unsigned short len) {
    is_extbuf = false;
    bytes = (char*) malloc(len + 1);
    bytes[len] = '\0';

    bytes[0] = type;
    tobytes(bytes, 1, len);
}

CmdPkg::~CmdPkg() {
    if (!is_extbuf)
        free(bytes);
}

char CmdPkg::getType() {
    return bytes[0];
}

unsigned short CmdPkg::getLen() {
    return toushort(bytes, 1);
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

void CmdPkg::setBytes(int data_index, char* buf, int n) {
    memcpy(getData() + data_index, buf, n);
}
