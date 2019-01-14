#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"
#include <thread>
#include <unistd.h>


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
int lastId = -1;
bool gameStarted = false;
GameSettings *gameSettings, gs;


void timer();
std::thread timerThread(timer);


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

				Player player(clientFd, ++lastId);
				players[clientFd] = player;

				char connectedChar[2] = {'O', '\n'};
				sendToOne(connectedChar, 2, clientFd, players);

				if(gameSettings->started){
					char *parsedMap = convertToOneDimension(map, gameSettings->mapX,gameSettings->mapY);
					players = sendToOne(parsedMap, sizeof(parsedMap), clientFd, players);
				}
				continue;
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				char *buffer = (char*)malloc(sizeof(char) * READ_BUFFER);
				int count = read(clientFd, buffer, READ_BUFFER);
				if (count > 0) {
					printf("Message from clientFd: %d: %s\n",clientFd, buffer);

					std::list<HandleData> hdList = handlePlayersMsg(map, buffer, clientFd, players, gameSettings);
					for (std::list<HandleData>::iterator hd = hdList.begin(); hd != hdList.end(); ++hd){
					
						Message msg = hd->message;
						// printf("33333333333333\n");
						if(hd->playerSet) players[hd->player.getFd()] = hd->player;
				

						Player player = players[clientFd];
						// printf("444444444443333\n");

						// printf("fd: %d, name: %s, points %d, ready: %d, x: %s, y: %s\n", 
								// player.getFd(), player.getName(), player.getPoints(), player.isReady(), player.getX(), player.getY());
						sleep(3);
						if(msg.length > 0){
							if(msg.fd){
								sendToOne(msg.content, msg.length, msg.fd, players);
							}else {
								players = sendToAll(msg.content, msg.length, players);
							}
							free(msg.content);
						}
					}
				}  
				free(buffer);
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
    printf("\nClosing server\n");
	timerThread.join();
	printf("Join thread\n");
    exit(0);
}
//**********************

void timer(){
	
}