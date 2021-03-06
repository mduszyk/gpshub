#ifndef NETUTIL_H
#define NETUTIL_H

#include <sys/socket.h>

bool socket_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2);

bool ip_equals(struct sockaddr_storage* sa1, struct sockaddr_storage* sa2);

int toint(char* buf, int index);

unsigned int touint(char* buf, int index);

short toshort(char* buf, int index);

unsigned short toushort(char* buf, int index);

void tobytes(char* buf, int index, short val);

void tobytes(char* buf, int index, unsigned short val);

void tobytes(char* buf, int index, int val);

void tobytes(char* buf, int index, unsigned int val);

void ip_printable(struct sockaddr_storage* addr, char* out_buf);

void socket_printable(struct sockaddr_storage* addr, char* out_buf);

#endif // NETUTIL_H
