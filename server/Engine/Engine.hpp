#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 6

char* generateWritableMap(char **, int x, int y, int boxes, int stones);
void generateMap(char **map, int x, int y, int boxes, int stones);
char* convertToOneDimension(char **map, int x, int y);
void insertObjects(char **map, char token, int number, int x, int y);
void resetMap(char **map, int x, int y);
void handlePlayersMove(char **map, char move, char *data, char playerId, int x, int y);
void addPlayerToMap(char **map, char playerId, int x, int y);
void removePlayerFromMap(char **map, char playerId, int x, int y);

// move to math library

char * toChar(int x);
int toInt(char * number);

#endif