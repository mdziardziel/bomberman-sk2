#include "Server.hpp"
#include "Helpers.hpp"

void removeClient(int clientFd, std::unordered_set<int> clientFds){
	printf("removing %d\n", clientFd);
	clientFds.erase(clientFd);
	close(clientFd);
}

uint16_t readPort(char * txt){
    char * ptr;
    auto port = strtol(txt, &ptr, 10);
    if(*ptr!=0 || port<1 || (port>((1<<16)-1))) error(1,0,"illegal argument %s", txt);
    return port;
}

void setReuseAddr(int sock){
    const int one = 1;
    int res = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one));
    if(res) error(1,errno, "setsockopt failed");
}

void sendToAll(char * buffer, int count, std::unordered_set<int> clientFds){
    int res;
    decltype(clientFds) bad;
    for(int clientFd : clientFds){
        res = write(clientFd, buffer, count);
        if(res!=count)
            bad.insert(clientFd);
    }
    for(int clientFd : bad){
		removeClient(clientFd, clientFds);
    }
}

void sendToOne(char * buffer, int count, int clientFd, std::unordered_set<int> clientFds){
    int res = write(clientFd, buffer, count);
    if(res!=count) removeClient(clientFd, clientFds);
}

uint16_t getPortNumber(int defaultPort, int argc, char **argv){
    if(argc != 2) {
        printf( "Run with default port %d\n\n", defaultPort);
        return defaultPort;
    } else {
		printf( "Run with port %s\n\n", argv[1]);
        return readPort(argv[1]);    
    }
}

int createSocket(int ai, int ss, int flag){
    int listenSock = socket(ai, ss, flag);
    if(listenSock == -1) error(1,0,"socket failed");
    return listenSock;
}

void bindToAddressAndPort(int port, int listenSock){
    sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
    if(bind(listenSock, (sockaddr*) &serverAddr, sizeof(serverAddr))) error(1,0,"bind failed");
}

void enterListeningMode(int listenSock){
    if(listen(listenSock, 1)) error(1,0,"listen failed");
}

int createEpoll(){
    int epollFd = epoll_create1(0);
	if(epollFd == -1) error(1,0,"create epoll failed");
    return epollFd;
}

int connectNewClient(epoll_event event, int listenSock, int epollFd){
    sockaddr_in clientAddr{0};
    socklen_t clientAddrSize = sizeof(clientAddr);

    int clientFd = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrSize);
    if (clientFd == -1) {
        printf("accept failed");
        return -1;
    }

    event.events = EPOLLIN | EPOLLET;
    event.data.fd = clientFd;

    if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
        printf("watch new client failed");
        close(clientFd);
        printf("removing %d\n", clientFd);
        return -1;
    }

    printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
            clientFd);

    return clientFd;
}