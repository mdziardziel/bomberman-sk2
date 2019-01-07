#include "Engine.hpp"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <stdio.h>
#include <sstream>
#include <string>

void handlePlayersMove(char **map, char move, char *data, char playerId, int x, int y){
    char charX[2] = {data[0], data[1]};
    char charY[2] = {data[2], data[3]};
    int dataX = toInt(charX);
    int dataY = toInt(charY);
    printf("%d, %d\n", dataX, dataY);
    switch(move) {
        case 'o': // set bomb
            map[dataX][dataY] = 'o';
            break;
        case 'w': //move to other place
            removePlayerFromMap(map, playerId, x, y);
            map[dataX][dataY] = playerId;
            break;
    }
}

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


/// move to math library

char * toChar(int x) {
	std::stringstream str;
	str << x;
	return &str.str()[0];
}

int toInt(char * number) {
    int ret;
    std::stringstream s(number);
    s >> ret;
    // int ret = (int)number - 48;
    return ret;
}