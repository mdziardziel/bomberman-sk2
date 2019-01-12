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
epoll_event event, events[MAX_EVENTS];

//game
char **map;
std::map < int, Player> players;
int lastId = 9;
bool gameStarted = false;
GameSettings *gameSettings, gs;

int main(int argc, char ** argv){
	gameSettings = &gs;

	gameSettings -> mapX = X_FIELDS;
	gameSettings -> mapY = Y_FIELDS;

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

	// int parsedMapSize = X_FIELDS*Y_FIELDS;
	// char *parsedMap = new char[parsedMapSize];

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

				Player player(clientFd, toChar(++lastId));
				players[clientFd] = player;

				if(gameSettings->started){
					char *parsedMap = convertToOneDimension(map, gameSettings->mapX,gameSettings->mapY);
					players = sendToOne(parsedMap, sizeof(parsedMap), clientFd, players);
				}
				continue;
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				char buffer[READ_BUFFER];
				int count = read(clientFd, buffer, READ_BUFFER);
				if (count > 0) {

					std::list<HandleData> hdList = handlePlayersMsg(map, buffer, clientFd, players, gameSettings);
					for (std::list<HandleData>::iterator hd = hdList.begin(); hd != hdList.end(); ++hd){
					
						Message msg = hd->message;
						players[hd->player.getFd()] = hd->player;

						Player player = players[clientFd];

						printf("fd: %d, name: %s, points %d, ready: %d, x: %s, y: %s\n", 
								player.getFd(), player.getName(), player.getPoints(), player.isReady(), player.getX(), player.getY());
						if(msg.length > 0){
							players = sendToAll(msg.content, msg.length, players);
							free(msg.content);
						}
					}
				}  
			}

		}

    }
}


//TODO move to server.cpp
void ctrl_c(int ){
	for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
		printf("removing %d\n", player->first);
		close(player->first);
		players.erase(player->first);
    }
    close(listenSock);
    printf("Closing server\n");
    exit(0);
}
//**********************