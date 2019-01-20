#include "Server.hpp" 
#include "Helpers.hpp"

void removeClient(int clientFd, std::map<int, Player> &players){
    // char *prt = {'r', 'e', 'm', 'o', 'v', 'i', 'n', 'g', toChar1(clientFd)};
    // printToConsole(&hdList, prt, strlen(prt));
	// printf("removing %d\n", clientFd);
	players.erase(clientFd);
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

void sendToOne(const char * buffer, int count, int clientFd){
    if(write(clientFd, buffer, count) == count) {
        // printf("Message to (fd) %d: %s", clientFd, buffer);
        if((int)buffer[0] > 57 || (int)buffer[0] < 48) printf("SEND %d %d (%.*s)\n", count, clientFd,count, buffer);

    }
}
void sendToAll(const char * buffer, int count, std::map<int, Player> players){
    for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
        if(player->second.getId() == -1) continue;
        if(write(player->first, buffer, count) == count){
            // printf("Message to (id) %d: %s", player->second.getId(), buffer);
            if((int)buffer[0] > 57 || (int)buffer[0] < 48) printf("SEND %d %d (%.*s)\n", count, player->first,count, buffer);

        }
    }
}

void sendToAlmostAll(const char * buffer, int count, std::map<int, Player> players, int skipFd){
    for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
        if(player->second.getId() == -1) continue;
        int clientFd = player->first;
        if(skipFd == clientFd) continue;
        if(write(clientFd, buffer, count) == count){
            if((int)buffer[0] > 57 || (int)buffer[0] < 48) printf("SEND %d %d (%.*s)\n", count, player->first,count, buffer);
        }
    }
}

uint16_t getPortNumber(int defaultPort, int argc, char **argv){
    if(argc < 2) {
        printf( "Run with default port %d\n", defaultPort);
        return defaultPort;
    } else {
		printf( "Run with port %s\n", argv[1]);
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
    if(listen(listenSock, 9)) error(1,0,"listen failed");
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