#include "Engine.hpp"
#include "Helpers.hpp"
#include "Server.hpp"

// int addPlayer(std::list<player> players, int lastId, int clientFd, char **map, int x, int y){
//     player pl;
//     pl.fd = clientFd;
//     pl.id = toChar(++lastId);

//     srand (time(NULL));
//     while(true) {
//         int tokenX, tokenY;
//         tokenX = rand() % x;
//         tokenY = rand() % y;
//         if(map[tokenX][tokenY] == '_'){
//             pl.x = toChar(tokenX);
//             pl.y = toChar(tokenY);
//         }
//     }

//     players.insert(pl);

//     return lastId;
// }

std::list<HandleData> handlePlayersMsg(char **map, char *buffer, int clientFd, std::map < int, Player> players, GameSettings *gameSettings){
    std::list<HandleData> hdList;
    Player player = players[clientFd];
    // printf("%s\n", players[clientFd].getId());

    char move = buffer[0];
    size_t sizeOfBuffer = strlen(buffer);

    char playerId = player.getCharId();
    // printf("%c \n", playerId);
    switch(move) {
        case 'B': // set bomb
            if(sizeOfBuffer >= 5){
                // printf("11111\n");
                char *rawMessage = (char*)malloc(sizeof(char) * 7);
                // printf("222222\n");
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                int xI = toInt(x);
                int yI = toInt(y);
                if(xI >= gameSettings->mapX || yI >= gameSettings->mapY) return hdList;
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
                player.setX(x);
                player.setY(y);

                HandleData hd(player, 7, rawMessage);
                hdList.push_back(hd);
            }
            break;
        case 'P': //move to other place
            if(sizeOfBuffer >= 5){
                char *rawMessage = (char*)malloc(sizeof(char) * 7);
                rawMessage[0] = 'P';
                rawMessage[1] = playerId;
                rawMessage[2] = buffer[1];
                rawMessage[3] = buffer[2];
                rawMessage[4] = buffer[3];
                rawMessage[5] = buffer[4];
                rawMessage[6] = '\n';
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                player.setX(x);
                player.setY(y);
                
                HandleData hd(player, 7, rawMessage);
                hdList.push_back(hd);
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
                char *rawMessage = (char*)malloc(sizeof(char) * 6);
                rawMessage[0] = 'F';
                rawMessage[1] = x[0];
                rawMessage[2] = x[1];
                rawMessage[3] = y[0];
                rawMessage[4] = y[1];
                rawMessage[5] = '\n';
                HandleData hd(player, 6, rawMessage);
                hdList.push_back(hd);
            }
            break;
        case 'N': //set name
            if(sizeOfBuffer >= 3){
                char len[2] = { buffer[1], buffer[2] };
                int leng = toInt(len);
                char *rawMessage = (char*)malloc(sizeof(char) * (leng + 5));
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
                    player.setName(name, leng);

                    char *tmp2 = (char*)malloc(sizeof(char) * 2);
                    tmp2[0] = 'Y';
                    tmp2[1] = '\n';
                    HandleData hd1(2, tmp2, clientFd);
                    
                    
                    char *tmp3 = (char*)malloc(sizeof(char) * 3);
                    tmp3[0] = 'Q';
                    tmp3[1] = player.getCharId();
                    tmp3[2] = '\n';
                    HandleData hd2(3, tmp3, clientFd);
                    
                    HandleData hd(player, leng + 5, rawMessage);
                    
                    hdList.push_back(hd1);
                    hdList.push_back(hd2);
                    hdList = sendLowerNames(players, clientFd, hdList);
                    hdList.push_back(hd);
                }
            }
            break;
        case 'K': 
            if(sizeOfBuffer >= 3){
                char* rawMessage = (char*)malloc(sizeof(char) * 7);
                rawMessage[0] = 'P';
                rawMessage[1] = playerId;
                rawMessage[2] = '-';
                rawMessage[3] = '1';
                rawMessage[4] = '-';
                rawMessage[5] = '1';
                rawMessage[6] = '\n';
                player.setX(-1);
                player.setY(-1);
               
                HandleData hd(player, 7, rawMessage);
                hdList.push_back(hd);

                int killerId = toInt(buffer[1]);
                Player killer = findPlayerById(players, killerId);
                killer.addPoint();

                char *pts = toChar3(killer.getPoints());
                char* rawMessage2 = (char*)malloc(sizeof(char) * 6);
                rawMessage2[0] = 'A';
                rawMessage2[1] = killer.getCharId();
                rawMessage2[2] = pts[0];
                rawMessage2[3] = pts[1];
                rawMessage2[4] = pts[2];
                rawMessage2[5] = '\n';

                HandleData hd2(killer, 6, rawMessage2);
                hdList.push_back(hd2);
            }
            break;
        case 'G': 
            player.ready();
            players[clientFd].ready();
            // printf("ready: %d\n", player.isReady());
            players = checkConnections(players); // sent control message to check if everyone is still connected
            if(isEveryoneReady(players, map, gameSettings)){           
                char* rawMessage = (char*)malloc(sizeof(char) * 2);
                rawMessage[0] = 'S';
                rawMessage[1] = '\n';
                HandleData hd2(player, 2, rawMessage);
                hdList.push_back(hd2);
            }
            break;
        case 'T': 
            if(sizeOfBuffer >= 3){
                char time[2] = { buffer[1], buffer[2] };
                gameSettings->time = toInt(time[2]);
                char* rawMessage = (char*)malloc(sizeof(char) * 4);
                rawMessage[0] = 'T';
                rawMessage[1] = buffer[1];
                rawMessage[2] = buffer[2];
                rawMessage[3] = '\n';

                HandleData hd2(player, 4, rawMessage);
                hdList.push_back(hd2);
            }
            break;
    }

    return hdList;
}

std::list<HandleData> sendLowerNames(std::map < int, Player> players, int clientFd, std::list<HandleData> hdList){
    for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
        // printf("%d \n", player->second.getId());
        if(player->second.getFd() == clientFd) break;
        char * namePl = player->second.getName();
        int leng = player->second.getNameSize();

        char *rawMessage = (char*)malloc(sizeof(char) * (leng + 5));//4
        char *name = new char[leng];
        rawMessage[1] = player->second.getCharId();
        char *nameSize = toChar2(leng);
        rawMessage[2] = nameSize[0]; rawMessage[3] = nameSize[1];
        for(int i = 0; i < leng; i++) rawMessage[i+4] = namePl[i];
        rawMessage[leng + 4] = '\n';
        rawMessage[0] = 'N';
        HandleData hd(leng + 5, rawMessage, clientFd);
        hdList.push_back(hd);
    }

    return hdList;
}

int isEveryoneReady(std::map < int, Player> players, char **map, GameSettings *gameSettings){
    for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
        // printf("%s\n", player->second.getName());
        if(!player->second.isReady()) return 0;
    }

    generateMap(map, gameSettings->mapX, gameSettings->mapY, 6, 4);
    return 1;
}


Player findPlayerById(std::map<int, Player> players, int id){
    for(std::map<int, Player>::iterator player = players.begin(); player != players.end(); ++player){
        int playerId = player->second.getId();
        if(playerId == id){
             return player->second; 
        }
    }
}

// char *generatePlayersId(int newId){
//     if(newId < 10 ){
//         char pi[2];
//         pi[0] = '0';
//         pi[1] = toChar(newId)[0];

//         // printf("%s %s %d %s \n", pi, toChar(newId), sizeof(pi), result);
//         return pi;
//     }
//     return toChar(newId);
// }

// char * addPlayer(std::map < int, char* > players, int lastId, int clientFd){
//     players[clientFd] = toChar(lastId);
//     return toChar(lastId);
// }

// char *findPlayerId(int clientFd, std::map < int, char* > players){
//     for(std::list<Player>::iterator player = players.begin(); player != players.end(); ++player){
//         printf("%s\n", player -> getId());
//         if(player -> getFd() == clientFd) { return player -> getId(); }
//     }
//     // raise error
//     return new char[1];
// }

// void removePlayerFromMap(char **map, char playerId, int x, int y){
//     for (int i = 0; i < x; i++){
//         for (int j = 0; j < y; j++) {
//             if(map[i][j] == playerId) {
//                 map[i][j] = '_';
//                 return;
//             }
//         }
//     } 
// }

// void addPlayerToMap(char **map, char playerId, int x, int y){
//     insertObjects(map, playerId, 1, x, y);
// }

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
