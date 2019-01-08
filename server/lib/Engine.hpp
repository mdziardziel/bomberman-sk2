#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 6

#include <map>

class Player {
    int fd;
    char* id;
    // char* x;
    // char* y;

    public:

    int getFd() { return fd; }
    char* getId() {  return id; }
    void setFd(int f) { fd = f; }
    void setId(char * i) { id = i; }
    // char* getX() { return x; }
    // char* getY() { return y; }
};

char* generateWritableMap(char **, int x, int y, int boxes, int stones);
void generateMap(char **map, int x, int y, int boxes, int stones);
char* convertToOneDimension(char **map, int x, int y);
void insertObjects(char **map, char token, int number, int x, int y);
void resetMap(char **map, int x, int y);
void addPlayerToMap(char **map, char playerId, int x, int y);
void removePlayerFromMap(char **map, char playerId, int x, int y);
char * addPlayer(std::map < int, char* > players, int lastId, int clientFd);
// char *findPlayerId(int clientFd, std::map < int, char* > players);
void handlePlayersMsg(char **map, char *buffer, int clientFd, std::map < int, char* > players, int x, int y);

#endif