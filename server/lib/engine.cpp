#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"

// void printToConsole(std::list<Message>* list, char *arr, int len){
// 	Message mg(len, arr, 1, 0);
// 	list->push_back(mg);
// }


void handlePlayersMsg(std::list<Message>* hdList, char **map, char *buffer, int clientFd, std::map < int, Player>* players, GameSettings *gs, int remainingTime){
    // printf("%s\n", players[clientFd].getId());

    char move = buffer[0];
    size_t sizeOfBuffer = strlen(buffer);

    char playerId = (*players)[clientFd].getCharId();

    switch(move) {
        case 'B': // set bomb
            if(sizeOfBuffer >= 5){
                char rawMessage[7];

                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                int xI = toInt(x);
                int yI = toInt(y);
                if(xI >= gs->mapX || yI >= gs->mapY) return;

                map[toInt(x)][toInt(y)] = 'o';

                rawMessage[0] = 'B';
                rawMessage[1] = playerId;
                rawMessage[2] = buffer[1];
                rawMessage[3] = buffer[2];
                rawMessage[4] = buffer[3];
                rawMessage[5] = buffer[4];
                rawMessage[6] = '\n';

                (*players)[clientFd].setX(x);
                (*players)[clientFd].setY(y);

                Message mg(7, rawMessage, 0, clientFd);
                hdList->push_back(mg);
            }
            break;
        case 'P': //move to other place
            if(sizeOfBuffer >= 5){
                char x[2] = {buffer[1], buffer[2]};
                char y[2] = {buffer[3], buffer[4]};

                sendPlayerPosition(playerId, x, y, hdList, clientFd);
                
                (*players)[clientFd].setX(x);
                (*players)[clientFd].setY(y);
            } 
            break;
        case 'F':  //set map size
            if(sizeOfBuffer >= 5) {
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };

                gs->mapX = toInt(x);
                gs->mapY = toInt(y);

                sendMapSies((*gs), hdList, 0);
            }
            break;
        case 'N': //set name
            if(sizeOfBuffer >= 3){
                char len[2] = { buffer[1], buffer[2] };
                int leng = toInt(len);
                char rawMessage[leng + 5];
                if(sizeOfBuffer >= leng + 3){
                    char *name = new char[leng];
                    rawMessage[1] = playerId;
                    rawMessage[2] = buffer[1]; rawMessage[3] = buffer[2];
                    for(int i = 0; i < leng; i++) { 
                        name[i] = buffer[i+3];
                        rawMessage[i+4] = buffer[i+3];
                    }
                    rawMessage[leng + 4] = '\n';
                    rawMessage[0] = 'N';
                    (*players)[clientFd].setName(name, leng);

                    // send confirmtion
                    char tmp2[2];
                    tmp2[0] = 'Y';
                    tmp2[1] = '\n';
                    Message mg2(2, tmp2, clientFd, 0);
                    
                    // send players number
                    char tmp3[3];
                    tmp3[0] = 'Q';
                    tmp3[1] = (*players)[clientFd].getCharId();
                    tmp3[2] = '\n';
                    Message mg3(3, tmp3, clientFd, 0);
                    
                    // send name to all
                    Message mg(leng + 5, rawMessage, 0, clientFd);
                    
                    hdList->push_back(mg2);
                    hdList->push_back(mg3);
                    sendLowerNames(players, clientFd, hdList);
                    hdList->push_back(mg);

                    // send game state if started
                    if(remainingTime > 0){
                        sendMapSies((*gs), hdList, clientFd);

                        char *parsedMap = convertToOneDimension(map, (*gs));
                        Message mg1(sizeof(parsedMap), parsedMap, clientFd, 0);
                        hdList->push_back(mg1);

                        sendTime(remainingTime, hdList, clientFd);
				    }
                }
            }
            break;
        case 'K': 
            if(sizeOfBuffer >= 3){
                char rawMessage[7];
                rawMessage[0] = 'P';
                rawMessage[1] = playerId;
                rawMessage[2] = '-';
                rawMessage[3] = '1';
                rawMessage[4] = '-';
                rawMessage[5] = '1';
                rawMessage[6] = '\n';
                (*players)[clientFd].setX(-1);
                (*players)[clientFd].setY(-1);
               
                Message mg(7, rawMessage, 0, clientFd);
                hdList->push_back(mg);

                int killerId = toInt(buffer[1]);
                Player killer = findPlayerById(players, killerId);
                (*players)[killer.getFd()].addPoint();

        
                sendPoints((*players)[killer.getFd()].getCharId(), killer.getPoints(), hdList);
            }
            break;
        case 'G': 
            (*players)[clientFd].ready();
            break;
        case 'T': 
            if(sizeOfBuffer >= 3){
                char time[2] = { buffer[1], buffer[2] };
                gs->time = toInt(time[2]);
                char rawMessage[4];
                rawMessage[0] = 'T';
                rawMessage[1] = buffer[1];
                rawMessage[2] = buffer[2];
                rawMessage[3] = '\n';

                Message mg(4, rawMessage, 0, clientFd);
                hdList->push_back(mg);
            }
            break;
    }
}

void reuseId(std::map < int, Player>* players, int id){
    int n = getLastId(players);

    if(id == getLastId(players)) return ;

    for(std::map<int, Player>::iterator player = players->end(); player != players->begin(); --player){
         if(player->second.getId() == id + 1){
             (*players)[player->second.getFd()].setId(id);
             reuseId(players, id+1);
             return;
         }
     }
}

int getLastId(std::map < int, Player>* players){
    return players->size();
}

void generatePlyersPositions(std::map < int, Player>* players, GameSettings gs, char** map){
    srand (time(NULL));

	for(std::map<int, Player>::iterator player = players->begin(); player != players->end(); ++player){
        Player pl = player->second;
        while(1) {
            int tokenX = rand() % gs.mapX;
            int tokenY = rand() % gs.mapY;
            if(map[tokenX][tokenY] == '0' && (tokenX*tokenY)%2 != 1){
                map[tokenX][tokenY] == 'X';
                (*players)[pl.getFd()].setX(tokenX);
                (*players)[pl.getFd()].setY(tokenY);
                break;
            }
        }
    }
}


void sendPlayerPosition(char playerId, char* x, char* y, std::list<Message> *list, int fd){
    char rawMessage[7];
    rawMessage[0] = 'P';
    rawMessage[1] = playerId;
    rawMessage[2] = x[0];
    rawMessage[3] = x[1];
    rawMessage[4] = y[0];
    rawMessage[5] = y[1];
    rawMessage[6] = '\n';
    
    Message mg(7, rawMessage, 0, fd);
    list->push_back(mg);
}

void sendPlyersPositions(std::list<Message> *list, std::map < int, Player>* players){
	for(std::map<int, Player>::iterator player = players->begin(); player != players->end(); ++player){
        Player pl = player->second;
        sendPlayerPosition(pl.getCharId(), pl.getX(), pl.getY(), list, 0);
    }
}

void sendMapSies(GameSettings gs, std::list<Message> *list, int fd){
    char *x = gs.getCharX();
    char *y = gs.getCharY();
    char rawMessage[6];
    rawMessage[0] = 'F';
    rawMessage[1] = x[0];
    rawMessage[2] = x[1];
    rawMessage[3] = y[0];
    rawMessage[4] = y[1];
    rawMessage[5] = '\n';

    Message mg(6, rawMessage, 0, fd);
    list->push_back(mg);
}


void sendPoints(char id, int ptsInt, std::list<Message> *list){
    char *pts = toChar3(ptsInt);
    char rawMessage2[6];
    rawMessage2[0] = 'A';
    rawMessage2[1] = id;
    rawMessage2[2] = pts[0];
    rawMessage2[3] = pts[1];
    rawMessage2[4] = pts[2];
    rawMessage2[5] = '\n';

    Message mg2(6, rawMessage2, 0, 0);
    list->push_back(mg2);
}

void sendTime(int remainingTime, std::list<Message> *list, int clientFd){
    char rawMessage[9];
    char *charTime = toChar3(remainingTime);
    rawMessage[0] = 'T';
    rawMessage[1] = charTime[0];
    rawMessage[2] = charTime[1];
    rawMessage[3] = charTime[2];
    rawMessage[4] = '.';
    rawMessage[5] = '0';
    rawMessage[6] = '0';
    rawMessage[7] = '0';
    rawMessage[8] = '\n';
    Message mg(9, rawMessage, clientFd, 0);
    list->push_back(mg);
}

void sendLowerNames(std::map < int, Player> *players, int clientFd, std::list<Message> *hdList){
    for(std::map<int, Player>::iterator player = players->begin(); player->second.getFd() != clientFd; ++player){
        if(player->second.getFd() == clientFd) break;
        char * namePl = player->second.getName();
        int leng = player->second.getNameSize();

        char rawMessage[leng+5];//4
        char *name = new char[leng];
        rawMessage[1] = player->second.getCharId();
        char *nameSize = toChar2(leng);
        rawMessage[2] = nameSize[0]; rawMessage[3] = nameSize[1];
        for(int i = 0; i < leng; i++) rawMessage[i+4] = namePl[i];
        rawMessage[leng + 4] = '\n';
        rawMessage[0] = 'N';

        // printf("hej %s\n", rawMessage);
        Message mg(leng + 5, rawMessage, clientFd, 0);
        hdList->push_back(mg);
    }
}

void receivePing(char *buffer, std::map < int, Player> *players, int clientFd, std::list<Message> *hdList){
    char charId = (*players)[clientFd].getCharId();
    if((int)buffer[0] <= 57 && (int)buffer[0] >= 48) {
        char rawMessage[2] = {buffer[0], '\n'};
        std::time_t t = std::time(0); 
        (*players)[clientFd].setLastSeen(t);

        Message mg(2, rawMessage, clientFd, 0);
        hdList->push_back(mg);
    }
}

Player findPlayerById(std::map<int, Player> *players, int id){
    for(std::map<int, Player>::iterator player = players->begin(); player != players->end(); ++player){
        int playerId = player->second.getId();
        if(playerId == id){
             return player->second; 
        }
    }
}

char* generateWritableMap(char ** map, GameSettings gs, int boxes){
    generateMap(map, gs, boxes);
    return convertToOneDimension(map, gs);
}

void generateMap(char **map, GameSettings gs, int boxes){
    resetMap(map, gs);
    if(boxes*2 >= gs.mapX*gs.mapY - MAX_PLAYERS*2){
        boxes = gs.mapX+gs.mapY;
    }

    insertBoxes(map, boxes, gs);
} 

char* convertToOneDimension(char **map, GameSettings gs){
    int x = gs.mapX;
    int y = gs.mapY;
    int size = x*y + 2;
    char *oneDimension = new char[size];
    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++) {
            oneDimension[i*x+j + 1] = map[i][j];
        }
    } 
    oneDimension[0] = 'M';
    oneDimension[size-1] = '\n';
    return oneDimension;
}

void insertBoxes(char **map, int number, GameSettings gs){
    srand (time(NULL));
    while(number > 0) {
        int tokenX, tokenY;
        tokenX = rand() % gs.mapX;
        tokenY = rand() % gs.mapY;
        if(map[tokenX][tokenY] == '0' && (tokenX*tokenY)%2 != 1){
            map[tokenX][tokenY] = '1';
            number--;
        }
    }
}

void resetMap(char **map, GameSettings gs){
    for (int i = 0; i < gs.mapX; i++){
        for (int j = 0; j < gs.mapY; j++) {
            map[i][j] = '0';
        }
    }      
}
