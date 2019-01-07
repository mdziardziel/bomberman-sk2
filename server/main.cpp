#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"


//move to server.hpp
void ctrl_c(int );
//**********************

//server variable
int listenSock, epollFd;
uint16_t port;
std::unordered_set<int> clientFds;
epoll_event event, events[MAX_EVENTS];

//game variables
char **map;

int main(int argc, char ** argv){
	port = getPortNumber(DEFAULT_PORT, argc, argv);
    listenSock = createSocket(AF_INET, SOCK_STREAM, 0);

    signal(SIGINT, ctrl_c); 
    signal(SIGPIPE, SIG_IGN);
    setReuseAddr(listenSock);

	bindToAddressAndPort(port, listenSock);
	enterListeningMode(listenSock);
	epollFd = createEpoll();


    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = listenSock;

    epoll_ctl(epollFd, EPOLL_CTL_ADD, listenSock, &event);

	map = new char *[X_FIELDS];
	for (int i = 0; i < X_FIELDS; i++) map[i] = new char[Y_FIELDS];
	generateMap(map,X_FIELDS,Y_FIELDS,10,4);
	int parsedMapSize = X_FIELDS*Y_FIELDS;
	char *parsedMap = new char[parsedMapSize];

    while(true){
        int resultCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);

		for(int i = 0; i < resultCount; i++){

			if( events[i].events == EPOLLIN && events[i].data.fd == listenSock ) {
				int clientFd = connectNewClient(event, listenSock, epollFd);
				if(clientFd == -1) continue;

				clientFds.insert(clientFd);

				// addPlayerToMap(map, 'X', X_FIELDS,Y_FIELDS);

				parsedMap = convertToOneDimension(map,X_FIELDS,Y_FIELDS);
				sendToOne(parsedMap, parsedMapSize + 1, clientFd, clientFds);
				
				continue;
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
						sendToAll(parsedMap, parsedMapSize + 1, clientFds);
					}
				}  
			}

		}

    }
}


//move to server.cpp
void ctrl_c(int ){
    for(int clientFd : clientFds)
        removeClient(clientFd, clientFds);
    close(listenSock);
    printf("Closing server\n");
    exit(0);
}
//**********************