#include "ServerConfig.hpp"
#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"
#include <thread>
#include <unistd.h>
#include <atomic>


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
int remainingTime = 0;
GameSettings gameSettings, gs;

void timer(std::list<Message>& list, std::map < int, Player>& players, int &remainingTime, GameSettings &gameSettings);
void sender(std::list<Message>& list, std::map < int, Player>& players);
std::atomic<bool> done(false);

std::list<Message> hdList;

int debugMode = 0;

int main(int argc, char ** argv){
	if(argc > 2 && argv[2][0] == 'd'){
		printf("Run with debug mode\n\n");
		debugMode = 1;
	}
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
	generateMap(map,X_FIELDS,Y_FIELDS,10,4);
	
    while(true){
		int resultCount = epoll_wait(epollFd, events, MAX_EVENTS, -1);

		for(int i = 0; i < resultCount; i++){
			if( events[i].events == EPOLLIN && events[i].data.fd == listenSock ) {
				int clientFd = connectNewClient(event, listenSock, epollFd);
				if(clientFd == -1) continue;

				Player player(clientFd, ++lastId);
				players[clientFd] = player;

				char connectedChar[2] = {'O', '\n'};
				Message mg(2, connectedChar, clientFd, 0);
				hdList.push_back(mg);
				printf("%s\n", mg.getContent());

				if(remainingTime > 0){
					char *parsedMap = convertToOneDimension(map, gameSettings.mapX,gameSettings.mapY);
					Message mg2(sizeof(parsedMap), parsedMap, clientFd, 0);
					hdList.push_back(mg2);
				}
				continue;
			}

			int clientFd = events[i].data.fd;

			if( events[i].events == EPOLLIN) {
				char buffer[READ_BUFFER];
				int count = read(clientFd, buffer, READ_BUFFER);
				if (count > 0) {
					if(debugMode) printf("Message from clientFd: %d: %s\n",clientFd, buffer);

					receivePing(buffer, &players, clientFd, &hdList);
					handlePlayersMsg(&hdList, map, buffer, clientFd, &players, &gameSettings);

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

void timer(std::list<Message>& list, std::map < int, Player> &playersMap, int &remainingTime, GameSettings &gameSettings){
	int roundStartTime;
	while(!done.load()){
		sleep(0.5);
		int numPlayers = 0;
		int readyPlayers = 0;
		for(std::map<int, Player>::iterator playerMap = playersMap.begin(); playerMap != playersMap.end(); ++playerMap){
			Player player = playerMap->second;
			if(std::time(0) - player.getLastSeen() >= MAX_LATENCY){
				char rawMessage[3] = {'R', player.getCharId(), '\n'};
				Message mg(3, rawMessage, 0, 0);
				printf("removing %d\n", player.getFd());
				playersMap.erase(player.getFd());
				close(player.getFd());
        		list.push_back(mg);
			} 

			if(player.isReady()){readyPlayers++;}
			numPlayers++;
		}
		if(numPlayers == readyPlayers && numPlayers >= MIN_PLAYERS && remainingTime <= 0){
			remainingTime = gameSettings.time;

			char rawMessage[2];
			rawMessage[0] = 'S';
			rawMessage[1] = '\n';
			Message mg(2, rawMessage, 0, 0);
			list.push_back(mg);
			roundStartTime = std::time(0);
		}
		if(remainingTime > 0){
			remainingTime = gameSettings.time + roundStartTime -  std::time(0);
		}

		//set player as not ready when time ends
	}
}

void sender(std::list<Message>& list, std::map < int, Player> &playersMap){
	while(!done.load()){
		while(!list.empty()){
			Message msg = list.front();
			list.pop_front();
			// printf("not empty\n");
			if(debugMode) sleep(3);
			if(msg.getLength() > 0){
				if(msg.getFd()){
					// printf("not empty2\n");
					sendToOne(msg.getContent(), msg.getLength(), msg.getFd());
				} else if(msg.getSkipFd()) {
					// printf("not empty3\n");
					sendToAlmostAll(msg.getContent(), msg.getLength(), playersMap, msg.getSkipFd());
				} else{
					// printf("not empty4 %s\n", msg.getContent());
					sendToAll(msg.getContent(), msg.getLength(), playersMap);
				}
			}
		}
		sleep(0.01);
	}
}