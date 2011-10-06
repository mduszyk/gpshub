#ifndef UTIL_H
#define UTIL_H

#include <sys/socket.h>

bool compare_sockaddr(struct sockaddr *sa1, struct sockaddr *sa2);

int toint(char* buf, int index);

short toshort(char* buf, int index);

void tobytes(char* buf, int index, short val);

void tobytes(char* buf, int index, int val);

#endif // UTIL_H
