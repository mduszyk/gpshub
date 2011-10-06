#include "socket/netutil.h"
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>

bool compare_sockaddr(struct sockaddr *sa1, struct sockaddr *sa2) {
    if (sa1->sa_family != sa2->sa_family)
        return false;

    if (sa1->sa_family == AF_INET) {
        return ((struct sockaddr_in*)sa1)->sin_addr.s_addr == ((struct sockaddr_in*)sa2)->sin_addr.s_addr
            && ((struct sockaddr_in*)sa1)->sin_port == ((struct sockaddr_in*)sa2)->sin_port;
    }

    return memcmp(&((struct sockaddr_in6*)sa1)->sin6_addr, &((struct sockaddr_in6*)sa2)->sin6_addr, sizeof(in6_addr)) == 0
            && ((struct sockaddr_in6*)sa1)->sin6_port == ((struct sockaddr_in6*)sa2)->sin6_port;
}

int toint(char* buf, int index) {
    int* netval = (int*) &buf[index];
    return (int) ntohl(*netval);
}

short toshort(char* buf, int index) {
    short* netval = (short*) &buf[index];
    return (short) ntohs(*netval);
}

void tobytes(char* buf, int index, short val) {
    unsigned short netval = htons(val);
//    buf[index] = (netval >> 8) & 0xFF;
//    buf[index + 1] = netval & 0xFF;
    memcpy(&buf[index], &netval, sizeof netval);
}

void tobytes(char* buf, int index, int val) {
    unsigned int netval = htonl(val);
//    buf[index] = (netval >> 24) & 0xFF;
//    buf[index + 1] = (netval >> 16) & 0xFF;
//    buf[index + 2] = (netval >> 8) & 0xFF;
//    buf[index + 3] = netval & 0xFF;
    memcpy(&buf[index], &netval, sizeof netval);
}
