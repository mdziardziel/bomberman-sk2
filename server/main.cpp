#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <sstream>
#include <string>

#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"

// server socket
int listenSock;

// client sockets
std::unordered_set<int> clientFds;

// handles SIGINT
void ctrl_c(int);

// sends data to clientFds
void sendToAll(char * buffer, int count);

// converts cstring to port
uint16_t readPort(char * txt);

// sets SO_REUSEADDR
void setReuseAddr(int sock);

//int to string
std::string tostr (int x);



int main(int argc, char ** argv){

	// get and validate port number
    char* port_num;
	if(argc != 2) {
        printf( "Run with default port %d\n\n", DEFAULT_PORT);
        port_num = toChar(DEFAULT_PORT);
    } else {
		printf( "Run with port %s\n\n", argv[1]);
        port_num = argv[1];    
    }
    auto port = readPort(port_num);
	
    // create socket
    listenSock = socket(AF_INET, SOCK_STREAM, 0);
    if(listenSock == -1) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	
    // graceful ctrl+c exit
    signal(SIGINT, ctrl_c);
    // prevent dead sockets from throwing pipe errors on write
    signal(SIGPIPE, SIG_IGN);

    setReuseAddr(listenSock);

    // bind to any address and port provided in arguments
    sockaddr_in serverAddr{.sin_family=AF_INET, .sin_port=htons((short)port), .sin_addr={INADDR_ANY}};
    int res = bind(listenSock, (sockaddr*) &serverAddr, sizeof(serverAddr));
    if(res) { 
		perror("bind failed");
		exit(EXIT_FAILURE);
	}

    // enter listening mode
    res = listen(listenSock, 1);
    if(res) { 
		perror("listen failed");
		exit(EXIT_FAILURE);
	}
/****************************/

    int epollFd =  epoll_create1(0);
	if (epollFd == -1) {
		perror("epoll_create1");
		exit(EXIT_FAILURE);
	}

    epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = listenSock;

    epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSock, &event);

	// map
	char **map;
	map = new char *[X_FIELDS];
	for (int i = 0; i < X_FIELDS; i++) map[i] = new char[Y_FIELDS];
	generateMap(map,X_FIELDS,Y_FIELDS,10,4);
	int parsedMapSize = X_FIELDS*Y_FIELDS;
	char *parsedMap = new char[parsedMapSize];

    while(true){
        int resultCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);

		for(int i = 0; i < resultCount; i++){

			if( events[i].events == EPOLLIN && events[i].data.fd == listenSock ) {
				// prepare placeholders for client address
				sockaddr_in clientAddr{0};
				socklen_t clientAddrSize = sizeof(clientAddr);

				// accept new connection
				auto clientFd = accept(listenSock, (sockaddr *) &clientAddr, &clientAddrSize);
				if (clientFd == -1) {
					perror("accept failed");
				}
				// setnonblocking(clientFd);
                event.events = EPOLLIN | EPOLLET;
				event.data.fd = clientFd;
				if (epoll_ctl(epollFd, EPOLL_CTL_ADD, clientFd, &event) == -1) {
					perror("epoll_ctl: clientFd");
					exit(EXIT_FAILURE);
				}

				// add client to all clients set
				clientFds.insert(clientFd);

				// tell who has connected
				printf("new connection from: %s:%hu (fd: %d)\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port),
						clientFd);

				// send map with new player to all
				addPlayerToMap(map, toChar(clientFd%10)[0], X_FIELDS,Y_FIELDS);
				parsedMap = convertToOneDimension(map,X_FIELDS,Y_FIELDS);
				sendToAll(parsedMap, parsedMapSize + 1);
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				char buffer[5];
				int count = read(clientFd, buffer, 255);
				if (count > 0) {
					printf(buffer);
					if (count > 4) {
						char move = buffer[0];
						char data[4] = { buffer[1], buffer[2], buffer[3], buffer[4] };
						// send map to all after move
						handlePlayersMove(map, move, data, toChar(clientFd%10)[0], X_FIELDS, Y_FIELDS);
						parsedMap = convertToOneDimension(map,X_FIELDS,Y_FIELDS);
						sendToAll(parsedMap, parsedMapSize + 1);
					}
				}  
			}

		}

    }

/****************************/
}

void removeClient(int clientFd){
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

void ctrl_c(int){
    for(int clientFd : clientFds)
        removeClient(clientFd);
    close(listenSock);
    printf("Closing server\n");
    exit(0);
}

void sendToAll(char * buffer, int count){
    int res;
    decltype(clientFds) bad;
    for(int clientFd : clientFds){
        res = write(clientFd, buffer, count);
		// printf("res: %d, count: %d\n", res, count);
        if(res!=count)
            bad.insert(clientFd);
    }
    for(int clientFd : bad){
		removeClient(clientFd);
    }
}

void sendToOne(char * buffer, int count, int clientFd){
    int res = write(clientFd, buffer, count);
    if(res!=count) removeClient(clientFd);
}