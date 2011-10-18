#ifndef SOCKET_H
#define SOCKET_H

#include "socket/SocketException.h"
#include <netdb.h>
#include <sys/socket.h>

class Socket
{
    public:
        Socket(char* host, char* port, int socktype);
        virtual ~Socket();
        void Bind() throw(SocketException);
        void Listen() throw(SocketException);
        Socket* Accept() throw(SocketException);
        void Connect() throw(SocketException);
        void addFlags(int flags) throw(SocketException);
        int Recvfrom(char* buf, int buf_len, struct sockaddr_storage* their_addr) throw(SocketException);
        int Sendto(char* buf, int buf_len, struct sockaddr_storage* their_addr) throw(SocketException);
        int Recv(char* buf, int buf_len) throw(SocketException);
        int Send(char* buf, int buf_len) throw(SocketException);
        void SendAll(char* buf, int* len) throw(SocketException);
        void Close() throw(SocketException);
        int getFd();
        char*& getHost();
        char*& getPort();
        struct sockaddr_storage* getAddrPtr();

    protected:
        Socket(int sfd);
        void setHost(char* host);
        void setPort(char* port);

    private:
        int sockfd;
        char* host;
        char* port;
        int socktype;
        struct sockaddr_storage addr;
        struct addrinfo *servinfo, *p;
        void Getaddrinfo() throw(SocketException);

};

#endif // SOCKET_H
