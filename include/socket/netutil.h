#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>

bool socket_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2);

bool ip_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2);

int toint(char* buf, int index);

short toshort(char* buf, int index);

void tobytes(char* buf, int index, short val);

void tobytes(char* buf, int index, int val);

void printable_ip(struct sockaddr_storage* addr, char* out_buf);

#endif // UTIL_H
