#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"



void handlePlayersMsg(std::list<Message>* hdList, char **map, char *buffer, int clientFd, std::map < int, Player>* players, GameSettings *gameSettings){
    // printf("%s\n", players[clientFd].getId());

    char move = buffer[0];
    size_t sizeOfBuffer = strlen(buffer);

    char playerId = (*players)[clientFd].getCharId();
    // printf("%c \n", playerId);
    switch(move) {
        case 'B': // set bomb
            if(sizeOfBuffer >= 5){
                // printf("11111\n");
                char rawMessage[7];
                // printf("222222\n");
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                int xI = toInt(x);
                int yI = toInt(y);
                if(xI >= gameSettings->mapX || yI >= gameSettings->mapY) return;
                // printf("33333  %d %d\n");
                map[toInt(x)][toInt(y)] = 'o';
                // printf("444444\n");
                rawMessage[0] = 'B';
                rawMessage[1] = playerId;
                rawMessage[2] = buffer[1];
                rawMessage[3] = buffer[2];
                rawMessage[4] = buffer[3];
                rawMessage[5] = buffer[4];
                rawMessage[6] = '\n';
                // printf("555555\n");
                (*players)[clientFd].setX(x);
                (*players)[clientFd].setY(y);

                Message mg(7, rawMessage, 0, clientFd);
                hdList->push_back(mg);
            }
            break;
        case 'P': //move to other place
            if(sizeOfBuffer >= 5){
                char rawMessage[7];
                rawMessage[0] = 'P';
                rawMessage[1] = playerId;
                rawMessage[2] = buffer[1];
                rawMessage[3] = buffer[2];
                rawMessage[4] = buffer[3];
                rawMessage[5] = buffer[4];
                rawMessage[6] = '\n';
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                (*players)[clientFd].setX(x);
                (*players)[clientFd].setY(y);
                
                Message mg(7, rawMessage, 0, clientFd);
                hdList->push_back(mg);
            } 
            break;
        case 'F':  //set map size
            if(sizeOfBuffer >= 5) {
                // printf("%s\n", buffer);
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                // printf("%d %d\n", toInt(x), toInt(y));
                gameSettings->mapX = toInt(x);
                gameSettings->mapY = toInt(y);
                // printf("%d %d\n", gameSettings->mapX, gameSettings->mapY);
                char rawMessage[6];
                rawMessage[0] = 'F';
                rawMessage[1] = x[0];
                rawMessage[2] = x[1];
                rawMessage[3] = y[0];
                rawMessage[4] = y[1];
                rawMessage[5] = '\n';

                Message mg(6, rawMessage, 0, clientFd);
                hdList->push_back(mg);
            }
            break;
        case 'N': //set name
            if(sizeOfBuffer >= 3){
                char len[2] = { buffer[1], buffer[2] };
                int leng = toInt(len);
                char rawMessage[leng + 5];
                if(sizeOfBuffer >= leng + 3){
                    char *name = new char[leng];
                    // printf("22222222222222\n");
                    rawMessage[1] = playerId;
                    rawMessage[2] = buffer[1]; rawMessage[3] = buffer[2];
                    for(int i = 0; i < leng; i++) { 
                        name[i] = buffer[i+3];
                        rawMessage[i+4] = buffer[i+3];
                    }
                    rawMessage[leng + 4] = '\n';
                    rawMessage[0] = 'N';
                    (*players)[clientFd].setName(name, leng);

                    char tmp2[2];
                    tmp2[0] = 'Y';
                    tmp2[1] = '\n';
                    Message mg2(2, tmp2, clientFd, 0);
                    
                    
                    char tmp3[3];
                    tmp3[0] = 'Q';
                    tmp3[1] = (*players)[clientFd].getCharId();
                    tmp3[2] = '\n';
                    Message mg3(3, tmp3, clientFd, 0);
                    
                    Message mg(leng + 5, rawMessage, 0, clientFd);
                    
                    hdList->push_back(mg2);
                    hdList->push_back(mg3);
                    sendLowerNames(players, clientFd, hdList);
                    hdList->push_back(mg);
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

                char *pts = toChar3(killer.getPoints());
                char rawMessage2[6];
                rawMessage2[0] = 'A';
                rawMessage2[1] = (*players)[killer.getFd()].getCharId();
                rawMessage2[2] = pts[0];
                rawMessage2[3] = pts[1];
                rawMessage2[4] = pts[2];
                rawMessage2[5] = '\n';

                Message mg2(6, rawMessage2, 0, 0);
                hdList->push_back(mg2);
            }
            break;
        case 'G': 
            (*players)[clientFd].ready();
            // players[clientFd].ready();
            // printf("ready: %d\n", player.isReady());
            // players = checkConnections(players); // sent control message to check if everyone is still connected
            break;
        case 'T': 
            if(sizeOfBuffer >= 3){
                char time[2] = { buffer[1], buffer[2] };
                gameSettings->time = toInt(time[2]);
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
    if(buffer[0] == charId) {
        char rawMessage[2] = {charId, '\n'};
        std::time_t t = std::time(0); 
        (*players)[clientFd].setLastSeen(t);

        Message mg(2, rawMessage, clientFd, 0);
        hdList->push_back(mg);
    }
}


// int isEveryoneReady(std::map < int, Player> *players, char **map, GameSettings *gameSettings){
//     for(std::map<int, Player>::iterator player = players->begin(); player != players->end(); ++player){
//         // printf("%s\n", player->second.getName());
//         if(!player->second.isReady()) return 0;
//     }

//     generateMap(map, gameSettings->mapX, gameSettings->mapY, 6, 4);
//     return 1;
// }


Player findPlayerById(std::map<int, Player> *players, int id){
    for(std::map<int, Player>::iterator player = players->begin(); player != players->end(); ++player){
        int playerId = player->second.getId();
        if(playerId == id){
             return player->second; 
        }
    }
}

char* generateWritableMap(char ** map, int x, int y, int boxes, int stones){
    generateMap(map, x, y, boxes, stones);
    return convertToOneDimension(map, x, y);
}

void generateMap(char **map, int x, int y, int boxes, int stones){
    resetMap(map, x, y);
    if(boxes + stones >= x*y - MAX_PLAYERS*2){
        boxes = x+y;
        stones = boxes;
    }

    insertObjects(map, 's', stones, x, y);
    insertObjects(map, 'b', boxes, x, y);
} 

char* convertToOneDimension(char **map, int x, int y){
    int size = x*y + 1;
    char *oneDimension = new char[size];
    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++) {
            oneDimension[i*x+j] = map[i][j];
        }
    } 
    oneDimension[size-1] = '\n';
    return oneDimension;
}

void insertObjects(char **map, char token, int number, int x, int y){
    srand (time(NULL));
    while(number > 0) {
        int tokenX, tokenY;
        tokenX = rand() % x;
        tokenY = rand() % y;
        if(map[tokenX][tokenY] == '_'){
            map[tokenX][tokenY] = token;
            number--;
        }
    }
}

void resetMap(char **map, int x, int y){
    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++) {
            map[i][j] = '_';
        }
    }      
}
