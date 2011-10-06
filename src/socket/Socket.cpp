#include "socket/Socket.h"
#include "socket/SocketException.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

using namespace std;

/**
    Creates socket object by passing opened socket fd.
*/
Socket::Socket(int sfd) {
    this->sockfd = sfd;
}

/**
    Creates socket object by host and port,
    socket is going to be open when Bind or Connect is invoked.
*/
Socket::Socket(char* host, char* port, int socktype) {
    this->host = host;
    this->port = port;
    this->socktype = socktype;
    // set socket fd to -1 which means that socket isn't open
    this->sockfd = -1;
}

Socket::~Socket() {
    //freeaddrinfo(servinfo);
    close(sockfd);
}

void Socket::Getaddrinfo() throw(SocketException) {
    int rv;
    struct addrinfo hints;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // set to AF_INET to force IPv4
    hints.ai_socktype = socktype;
    if (host == NULL) {
        hints.ai_flags = AI_PASSIVE; // use my IP
    }

    if ((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
        throw SocketException(string("getaddrinfo failed: ") + gai_strerror(rv));
    }
}

void Socket::Bind() throw(SocketException) {

    if (sockfd != -1) {
        throw SocketException("can't bind already initialized socket");
    }

    Getaddrinfo();

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            //perror("echolistener: socket");
            continue;
        }
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            //perror("echolistener: bind");
            continue;
        }
        break;
    }

    if (p == NULL) {
        throw SocketException("failed to bind socket");
    }
    freeaddrinfo(servinfo);
}

void Socket::Connect() throw(SocketException) {

    if (sockfd != -1) {
        throw SocketException("can't connect already initialized socket");
    }

    Getaddrinfo();

     // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            //perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            //perror("client: connect");
            continue;
        }
        break;
    }
    if (p == NULL) {
        throw SocketException("failed to connect");
    }
    freeaddrinfo(servinfo);
}

int Socket::Recvfrom(char* buf, int len, struct sockaddr_storage* their_addr) throw(SocketException) {
    socklen_t addr_len = sizeof *their_addr;
    int n;
    if ((n = recvfrom(sockfd, buf, len, 0,
                (struct sockaddr *)their_addr, &addr_len)) == -1) {
        throw SocketException("failed to recvfrom");
    }

    return n;
}

int Socket::Sendto(char* buf, int len, struct sockaddr_storage* their_addr) throw(SocketException) {
    socklen_t addr_len = sizeof *their_addr;
    int n;
    if ((n = sendto(sockfd, buf, len, 0,
               (struct sockaddr *)their_addr, addr_len)) == -1) {
         throw SocketException("failed to sendto");
    }

    return n;
}

int Socket::Recv(char* buf, int len) throw(SocketException) {
    int n;
    if ((n = recv(sockfd, buf, len, 0)) == -1) {
        throw SocketException("failed to recv");
    }

    return n;
}

int Socket::Send(char* buf, int len) throw(SocketException) {
    int n;
    if ((n = send(sockfd, buf, len, 0)) == -1) {
        throw SocketException("failed to send");
    }

    return n;
}

void Socket::SendAll(char* buf, int* len) throw(SocketException) {
    int total = 0;  // how many bytes we've sent
    int left = *len; // how many we have left to send
    int n;

    while(total < *len) {
        n = send(sockfd, buf + total, left, 0);
        if (n == -1) {
            *len = total;
            throw SocketException("failed to send all");
        }
        total += n;
        left -= n;
    }

    *len = total; // return number actually sent here
}

void Socket::addFlags(int new_flags) throw(SocketException) {
    int flags, s;

    flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        throw SocketException("fcntl failed to get current flags");
    }

    flags |= new_flags;
    s = fcntl(sockfd, F_SETFL, flags);
    if (s == -1) {
        throw SocketException("fcntl failed to set flags");
    }
}

void Socket::Listen() throw(SocketException) {
    int s;
    s = listen(sockfd, SOMAXCONN);
    if (s == -1) {
        throw SocketException("listening on socket failed");
    }
}

Socket* Socket::Accept() throw(SocketException) {
    struct sockaddr_storage new_addr;
    socklen_t addr_size = sizeof new_addr;
    int new_fd = accept(sockfd, (struct sockaddr*)&new_addr, &addr_size);
    if (new_fd == -1) {
        throw SocketException("accept failed, try to check if socket is binded and listening");
    }
    Socket* sock = new Socket(new_fd);
    // TODO set host and port on sock

    return sock;
}

void Socket::Close() throw(SocketException) {
    int s;
    s = close(sockfd);
    if (s == -1) {
        throw SocketException("closing socket failed");
    }
}

int Socket::getFd() {
    return sockfd;
}
