#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"


//TODO move to server.hpp
void ctrl_c(int );
//**********************

//server variable
int listenSock, epollFd;
uint16_t port;
std::unordered_set<int> clientFds;
epoll_event event, events[MAX_EVENTS];

//game
char **map;
std::map < int, char* > players;
int lastId = 0;
bool gameStarted = false;
MapSize *mapSize, ms;

int main(int argc, char ** argv){
	mapSize = &ms;

	mapSize -> x = X_FIELDS;
	mapSize -> y = Y_FIELDS;

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

	//game
	map = new char *[X_FIELDS];
	for (int i = 0; i < X_FIELDS; i++) map[i] = new char[Y_FIELDS];
	generateMap(map,X_FIELDS,Y_FIELDS,10,4);

	int parsedMapSize = X_FIELDS*Y_FIELDS;
	char *parsedMap = new char[parsedMapSize];

	// int parsedPlayersSize = MAX_PLAYERS*14;
	// char *parsedPlayers = new char[parsedPlayersSize];

	// TODO add new thread, which will count down the time and inform main thread if any bomb explode or the round will end
	// TODO delete destroyed objects and players
	// TODO count points
	// TODO send final classification and start new round
	// TDOD freeze round if is less than 2 players

    while(true){
		int resultCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);

		for(int i = 0; i < resultCount; i++){

			if( events[i].events == EPOLLIN && events[i].data.fd == listenSock ) {
				int clientFd = connectNewClient(event, listenSock, epollFd);
				if(clientFd == -1) continue;

				clientFds.insert(clientFd);

				// lastId = addPlayer(players, lastId, clientFd, map, X_FIELDS, Y_FIELDS);
				// TODO add player to set

				players[clientFd] = toChar(++lastId);

				if(gameStarted){
					//TODO parse players set to char* and send to all players
					parsedMap = convertToOneDimension(map,X_FIELDS,Y_FIELDS);
					sendToOne(parsedMap, parsedMapSize + 1, clientFd, clientFds);
				}
				continue;
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				char buffer[READ_BUFFER];
				int count = read(clientFd, buffer, READ_BUFFER);
				if (count > 0) {
					// printf(buffer);

					handlePlayersMsg(map, buffer, clientFd, players, mapSize);

					// parsedMap = convertToOneDimension(map,X_FIELDS,Y_FIELDS);
					// sendToAll(parsedMap, parsedMapSize + 1, clientFds);
					//TODO parse players set to char* and send to all players
				}  
			}

		}

    }
}


//TODO move to server.cpp
void ctrl_c(int ){
    for(int clientFd : clientFds) {
        removeClient(clientFd, clientFds);
		players.erase(clientFd);
	}
    close(listenSock);
    printf("Closing server\n");
    exit(0);
}
//**********************