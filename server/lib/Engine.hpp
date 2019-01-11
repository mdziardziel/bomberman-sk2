#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 6

#include <map>
#include <string.h>
#include <unordered_set>

struct MapSize{
    int x;
    int y;
};

struct Message {
    // Message(int len, char* cont);

    char * content;
    int length;

    Message(int len, char* cont){
        content = cont;
        length = len;
    }
    Message(){
        length = 0;
    }
};


class Player{

    char* id;
    int points;
    char* x;
    char* y;
    bool readyToPlay;
    char* name;
    int fd;

    public:

    Player(int f, char* newId){
        fd = f;
        id = newId;
        points = 0;
        readyToPlay = false;
    }
    Player(){points = 0; readyToPlay = false;}

    int getFd(){return fd;}

    // void setFd(int f){fd = f;}

    int addPoint(){return ++points;}

    char* getId(){
        char* nc = new char[2];
        nc[0] = 'h';
        nc[1] = 'w';
        return id;
        }
    void ready(){readyToPlay = true;}
    char* getName(){return name;}

    void setName(char* n){
        name = n;
    }
};

struct HandleData {
    Message message;
    Player player;

    HandleData(Player pl, int len, char * msgA){
        Message msg(len, msgA);
        message = msg;
        player = pl;
    }
    HandleData(){
        Message msg;
        message = msg;
    }
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
// char* generatePlayersId(int newId);
HandleData handlePlayersMsg(char **map, char *buffer, int clientFd, std::map < int, Player> players, MapSize *mapSize);
Player findPlayerById(std::map<int, Player> players, char* id);


#endif