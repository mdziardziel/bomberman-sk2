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

void handlePlayersMsg(char **map, char *buffer, int clientFd, std::map < int, char* > players, MapSize *mapSize){
    char move = buffer[0];
    char *playerId = players[clientFd];

    switch(move) {
        case 'B': // set bomb
            if(sizeof(buffer) >= 5){
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                map[toInt(x)][toInt(y)] = 'o';
            }
            break;
        case 'P': //move to other place
            if(sizeof(buffer) >= 5){
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                //send new position to all
            }
            break;
        case 'F':  //set map size
            if(sizeof(buffer) >= 5) {
                char x[2] = { buffer[1], buffer[2] };
                char y[2] = { buffer[3], buffer[4] };
                mapSize -> x = toInt(x);
                mapSize -> y = toInt(y);
            }
            break;
        case 'N': //set name
            if(sizeof(buffer) >= 3){
                char len[2] = { buffer[1], buffer[2] };
                int leng = toInt(len);
                if(sizeof(buffer) >= leng + 3){
                    char *name = new char[leng];
                    for(int i = 0; i < leng; i++) name[i] = buffer[i+3];
                    //send new players name to all
                }
            }
            break;
        case 'K': 
            if(sizeof(buffer) >= 3){
                char killerId[2] = { buffer[1], buffer[2] };
                //add point to killerId
                //send that playerId dies to all
            }
            break;
        case 'G': 
            //set game ready to actual playerId
            break;
    }
}

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
