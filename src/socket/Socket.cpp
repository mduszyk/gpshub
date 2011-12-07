#include "socket/Socket.h"
#include "socket/SocketException.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>


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
Socket::Socket(const char* host, const char* port, int socktype) {
    this->host = NULL;
    this->port = NULL;
    if (host != NULL) {
        int n = strlen(host) + 1;
        this->host = (char*) malloc(n);
        memcpy(this->host, host, n);
    }
    if (port != NULL) {
        int n = strlen(port) + 1;
        this->port = (char*) malloc(n);
        memcpy(this->port, port, n);
    }

    this->socktype = socktype;
    // set socket fd to -1 which means that socket isn't open
    this->sockfd = -1;
}

Socket::~Socket() {
    //freeaddrinfo(servinfo);
    shutdown(sockfd, 2);
    free(host);
    free(port);
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
        throw SocketException(
            std::string("getaddrinfo failed: ") + gai_strerror(rv));
    }
}

void Socket::Bind() throw(SocketException) {

    if (sockfd != -1) {
        throw SocketException("can't bind already initialized socket");
    }

    Getaddrinfo();

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
        == -1) {
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
    // store sockets addr struct
    memcpy(&addr, p->ai_addr, p->ai_addrlen);
    freeaddrinfo(servinfo);
}

void Socket::Connect() throw(SocketException) {

    if (sockfd != -1) {
        throw SocketException("can't connect already initialized socket");
    }

    Getaddrinfo();

     // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            == -1) {
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
    // store sockets addr struct
    memcpy(&addr, p->ai_addr, p->ai_addrlen);
    freeaddrinfo(servinfo);
}

int Socket::Recvfrom(char* buf, int len, struct sockaddr_storage* their_addr)
throw(SocketException) {
    socklen_t addr_len = sizeof *their_addr;
    int n;
    if ((n = recvfrom(sockfd, buf, len, 0,
                (struct sockaddr *)their_addr, &addr_len)) == -1) {
        throw SocketException("failed to recvfrom");
    }

    return n;
}

int Socket::Sendto(char* buf, int len, struct sockaddr_storage* their_addr)
throw(SocketException) {
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
    sockaddr_storage new_addr;
    socklen_t addr_size = sizeof addr;
    int new_fd = accept(sockfd, (struct sockaddr*) &new_addr, &addr_size);
    if (new_fd == -1) {
        throw SocketException(
            "accept failed, try to check if socket is binded and listening");
    }

    Socket* new_sock = new Socket(new_fd);
    // copy addr struct to new socket
    memcpy(&new_sock->addr, &new_addr, addr_size);
    if (new_addr.ss_family == AF_INET) {
        // IPv4
        char* new_host = (char*) malloc(INET_ADDRSTRLEN);
        char* new_port = (char*) malloc(6);
        struct sockaddr_in* sa = (struct sockaddr_in*) &new_addr;
        inet_ntop(AF_INET, &(sa->sin_addr), new_host, INET_ADDRSTRLEN);
        sprintf(new_port, "%u", sa->sin_port);
        new_sock->setHost(new_host);
        new_sock->setPort(new_port);
    } else if (new_addr.ss_family == AF_INET6) {
        // IPv6
        char* new_host = (char*) malloc(INET6_ADDRSTRLEN);
        char* new_port = (char*) malloc(6);
        struct sockaddr_in6* sa6 = (sockaddr_in6*) &new_addr;
        inet_ntop(AF_INET6, &(sa6->sin6_addr), new_host, INET6_ADDRSTRLEN);
        sprintf(new_port, "%u", sa6->sin6_port);
        new_sock->setHost(new_host);
        new_sock->setPort(new_port);
    }

    return new_sock;
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

char*& Socket::getHost() {
    return host;
}

char*& Socket::getPort() {
    return port;
}

void Socket::setHost(char* host) {
    this->host = host;
}

void Socket::setPort(char* port) {
    this->port = port;
}

struct sockaddr_storage* Socket::getAddrPtr() {
    return &this->addr;
}
