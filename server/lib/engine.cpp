#include "Engine.hpp"
#include "Helpers.hpp"

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

int handlePlayersMsg(std::unordered_set<int> readyToPlay, char *rawMessage,char **map, char *buffer, int clientFd, std::map<int, char*> players, MapSize *mapSize){
    char move = buffer[0];
    size_t sizeOfBuffer = strlen(buffer);
    char *playerId = players[clientFd];
    switch(move) {
        case 'B': // set bomb
            if(sizeOfBuffer >= 5){
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                map[toInt(x)][toInt(y)] = 'o';
                //send to everything bomb
            }
            break;
        case 'P': //move to other place
            if(sizeOfBuffer >= 5){
                rawMessage[0] = 'P';
                rawMessage[1] = playerId[0];
                rawMessage[2] = playerId[1];
                rawMessage[3] = buffer[1];
                rawMessage[4] = buffer[2];
                rawMessage[5] = buffer[3];
                rawMessage[6] = buffer[4];
                rawMessage[7] = '\n';
                
                return 7;
            }
            break;
        case 'F':  //set map size
            if(sizeOfBuffer >= 5) {
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                mapSize -> x = toInt(x);
                mapSize -> y = toInt(y);
            }
            break;
        case 'N': //set name

            if(sizeOfBuffer >= 3){
                char len[2] = { buffer[1], buffer[2] };
                int leng = toInt(len);
                if(sizeOfBuffer >= leng + 3){
                    char *name = new char[leng];
                    rawMessage[1] = playerId[0]; rawMessage[2] = playerId[1];
                    rawMessage[3] = buffer[1]; rawMessage[4] = buffer[2];
                    for(int i = 0; i < leng; i++) { 
                        name[i] = buffer[i+3];
                        rawMessage[i+5] = buffer[i+3];
                    }
                    rawMessage[leng + 5] = '\n';
                    rawMessage[0] = 'N';
                    return leng + 5;
                }
            }
            break;
        case 'K': 
            if(sizeOfBuffer >= 3){
                char killerId[2] = { buffer[1], buffer[2] };
                //add point to killerId
                rawMessage[0] = 'P';
                rawMessage[1] = playerId[0];
                rawMessage[2] = playerId[1];
                rawMessage[3] = '-';
                rawMessage[4] = '1';
                rawMessage[5] = '-';
                rawMessage[6] = '1';
                rawMessage[7] = '\n';
               
                return 7;
            }
            break;
        case 'G': 
            //set game ready to actual playerId
            break;
    }
    return 0;
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

void removePlayerFromMap(char **map, char playerId, int x, int y){
    for (int i = 0; i < x; i++){
        for (int j = 0; j < y; j++) {
            if(map[i][j] == playerId) {
                map[i][j] = '_';
                return;
            }
        }
    } 
}

void addPlayerToMap(char **map, char playerId, int x, int y){
    insertObjects(map, playerId, 1, x, y);
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
