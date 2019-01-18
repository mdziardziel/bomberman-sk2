#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"
#include <thread>
#include <unistd.h>
#include <atomic>
#include <stdio.h>


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
int remainingTime = 0;
GameSettings gameSettings, gs;

void debugger(std::list<Message>& list);
void timer(std::list<Message>& list, std::map < int, Player>& players, int &remainingTime, GameSettings &gameSettings);
void sender(std::list<Message>& list, std::map < int, Player>& players);
std::atomic<bool> done(false);

std::list<Message> hdList;

int debugMode = 0;

int main(int argc, char ** argv){
	if(argc > 2 && argv[2][0] == 'd'){
		// char *prt = "Run with debug mode\n";
		// printToConsole(&hdList, prt, 20);   
		debugMode = 1;
	}

	std::thread debugerThread(debugger, std::ref(hdList));
	std::thread timerThread(timer, std::ref(hdList), std::ref(players), std::ref(remainingTime), std::ref(gameSettings));
	std::thread senderThread(sender, std::ref(hdList), std::ref(players));

	gameSettings.mapX = X_FIELDS;
	gameSettings.mapY = Y_FIELDS;

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
	
    while(true){
		int resultCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);

		for(int i = 0; i < resultCount; i++){
			if( events[i].events == EPOLLIN && events[i].data.fd == listenSock ) {
				int clientFd = connectNewClient(event, listenSock, epollFd);
				if(clientFd == -1) continue;

				Player player(clientFd, getLastId(&players));
				players[clientFd] = player;

				printf("%d\n", player.getId());

				char connectedChar[2] = {'O', '\n'};
				Message mg(2, connectedChar, clientFd, 0);
				hdList.push_back(mg);
				// printf("%s\n", mg.getContent());

				continue;
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				// printf("%d", clientFd);
				char buffer[READ_BUFFER];
				int count = read(clientFd, buffer, READ_BUFFER);
				if (count > 0) {
					receivePing(buffer, &players, clientFd, &hdList);
					handlePlayersMsg(&hdList, map, buffer, clientFd, &players, &gameSettings, remainingTime);
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
    printf("\nClosing server\n");
	done = true;
	printf("Join thread\n");
    exit(0);
}
//**********************

void debugger(std::list<Message>& list){
	while(!done.load()){
		char str[250];
		// int i;
		scanf("%s", str);
		int i = strlen(str);

		char rawMessage[i+1];
		for(int j = 0; j < i; j++) rawMessage[j] = str[j];
		rawMessage[i] = '\n';

		Message mg(i+1, rawMessage, 0, 0);
		list.push_back(mg);
	}
}


void timer(std::list<Message>& list, std::map < int, Player> &playersMap, int &remainingTime, GameSettings &gameSettings){
	int roundStartTime;
	while(!done.load()){
	
		sleep(2);
		int numPlayers = 0;
		int readyPlayers = 0;

		for(std::map<int, Player>::iterator playerMap = playersMap.begin(); playerMap != playersMap.end(); ++playerMap){
			Player player = playerMap->second;
								// printf("id %c\n", player.getCharId());
			//remove player when not responds
			if(std::time(0) - player.getLastSeen() >= MAX_LATENCY){
				char rawMessage[3] = {'R', player.getCharId(), '\n'};
				Message mg(3, rawMessage, 0, 0);
				list.push_back(mg);

				printf("remove fd: %d\n", player.getFd());
				close(player.getFd());
				int id = player.getId();
				playersMap.erase(player.getFd());

				// maybe some lock
				reuseId(&playersMap, id);
				// printf("last id %d\n", getLastId(&playersMap));
				break;
			} 

			if(player.isReady()){readyPlayers++;}
			numPlayers++;
		}

		//start game when every are ready
		if(numPlayers == readyPlayers && numPlayers >= MIN_PLAYERS && remainingTime <= 0){
			generateMap(map, gameSettings, gs.mapX+gs.mapY);
			// printf("lalalallalalal\n");
			remainingTime = gameSettings.time;

			// generate players positions
			generatePlyersPositions(&playersMap, gameSettings, map); //get map from local, not global

			//send start signal
			char rawMessage[2];
			rawMessage[0] = 'S';
			rawMessage[1] = '\n';
			Message mg(2, rawMessage, 0, 0);
			list.push_back(mg);

			// send map sizes
			sendMapSies(gameSettings, &list, 0);

			//send map 
			char *parsedMap = convertToOneDimension(map, gameSettings);
			Message mg2(strlen(parsedMap), parsedMap, 0, 0);
			sendTime(remainingTime, &hdList, 0);
			hdList.push_back(mg2);

			//send players positions
			sendPlyersPositions(&list, &playersMap);

			sendTime(remainingTime, &list, 0);
			roundStartTime = std::time(0);
		}

		//set remaining time
		if(remainingTime > 0){
			remainingTime = gameSettings.time + roundStartTime -  std::time(0);
			
			// time ends
			if(remainingTime <= 0){

				//set players as not ready
				for(std::map<int, Player>::iterator playerMap = playersMap.begin(); playerMap != playersMap.end(); ++playerMap){
					Player player = playerMap->second;

					playersMap[player.getFd()].notReady();
					
					//send points to all
					sendPoints(player.getCharId(), player.getPoints(), &list);

					//reset points
					playersMap[player.getFd()].resetPoints();

				}
			}
		}
	}
}

void sender(std::list<Message>& list, std::map < int, Player> &playersMap){
	while(!done.load()){
		while(!list.empty()){
			Message msg = list.front();
			list.pop_front();
			if(debugMode) sleep(3);
			if(msg.getLength() > 0){
				if(msg.getFd()){
					sendToOne(msg.getContent(), msg.getLength(), msg.getFd());
				} else if(msg.getSkipFd()) {
					sendToAlmostAll(msg.getContent(), msg.getLength(), playersMap, msg.getSkipFd());
				} else{
					sendToAll(msg.getContent(), msg.getLength(), playersMap);
				}
				if(msg.getContent()[0] == 'S') sleep(1);
			}
			sleep(0.05);
		}
		sleep(0.1);
	}
}