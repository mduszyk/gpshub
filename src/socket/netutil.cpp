#include "socket/netutil.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

bool socket_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2) {
    if (sa1->ss_family != sa2->ss_family)
        return false;

    if (sa1->ss_family == AF_INET) {
        return ((struct sockaddr_in*)sa1)->sin_addr.s_addr == ((struct sockaddr_in*)sa2)->sin_addr.s_addr
            && ((struct sockaddr_in*)sa1)->sin_port == ((struct sockaddr_in*)sa2)->sin_port;
    }

    return memcmp(&((struct sockaddr_in6*)sa1)->sin6_addr, &((struct sockaddr_in6*)sa2)->sin6_addr, sizeof(in6_addr)) == 0
            && ((struct sockaddr_in6*)sa1)->sin6_port == ((struct sockaddr_in6*)sa2)->sin6_port;
}

bool ip_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2) {
    if (sa1->ss_family != sa2->ss_family)
        return false;

    if (sa1->ss_family == AF_INET) {
        return ((struct sockaddr_in*)sa1)->sin_addr.s_addr == ((struct sockaddr_in*)sa2)->sin_addr.s_addr;
    }

    return memcmp(&((struct sockaddr_in6*)sa1)->sin6_addr, &((struct sockaddr_in6*)sa2)->sin6_addr, sizeof(in6_addr)) == 0;
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

void printable_ip(struct sockaddr_storage* addr, char* out_buf) {
    if (addr->ss_family == AF_INET) {
        struct sockaddr_in* sa = (struct sockaddr_in*) addr;
        inet_ntop(AF_INET, &(sa->sin_addr), out_buf, INET_ADDRSTRLEN);
    } else if (addr->ss_family == AF_INET6) {
        struct sockaddr_in6* sa6 = (sockaddr_in6*) addr;
        inet_ntop(AF_INET6, &(sa6->sin6_addr), out_buf, INET6_ADDRSTRLEN);
    }
}
