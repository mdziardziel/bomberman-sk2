#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 6

#include <map>
#include <string.h>
#include <unordered_set>
#include <list>
#include "Helpers.hpp"

struct GameSettings{
    int mapX;
    int mapY;
    int started;
    int time;
};

struct Message {
    // Message(int len, char* cont);

    char * content;
    int length;
    int fd;

    Message(int len, char* cont, int f){
        content = cont;
        length = len;
        fd = f;
    }
    Message(int len, char* cont){
        content = cont;
        length = len;
        fd = 0;
    }
    Message(){
        length = 0;
    }
};


class Player{

    int id;
    int points;
    int x;
    int y;
    int readyToPlay;
    char* name;
    int nameSize;
    int fd;

    public:

    Player(int f, int newId){
        fd = f;
        id = newId;
        points = 0;
        readyToPlay = 0;
        x = -1;
        y = -1;
        char nothing[1] = {' '};
        name = nothing;
    }
    Player(){points = 0; readyToPlay = false;}

    int getFd(){return fd;}

    char* getX(){
        return toChar2(x);
    }
    char* getY(){
        return toChar2(y);
    }
    int getPoints(){return points;}

    // void setFd(int f){fd = f;}

    int addPoint(){return ++points;}

    int isReady(){ return readyToPlay; }

    char getCharId(){ return toChar1(id); }
    int getId(){ return id; }
    void ready(){readyToPlay = 1;}
    char* getName(){return name;}
    int getNameSize(){ return nameSize;}
    
    void setId(int d){ id = d;}
    void setName(char* n, int m){
        name = n;
        nameSize = m;
    }
    void setX(char *xx){x = toInt(xx);}
    void setY(char *yy){ y = toInt(yy);}
    void setX(int xx){x = xx;}
    void setY(int yy){ y = yy;}

};

struct HandleData {
    Message message;
    Player player;
    int playerSet;

    HandleData(Player pl, int len, char * msgA, int f){
        Message msg(len, msgA, f);
        message = msg;
        player = pl;
        playerSet = 1;
    }
    HandleData(Player pl, int len, char * msgA){
        Message msg(len, msgA);
        message = msg;
        player = pl;
        playerSet = 1;
    }
    HandleData(int len, char * msgA){
        Message msg(len, msgA);
        message = msg;
        playerSet = 0;
    }
    HandleData(int len, char * msgA, int f){
        Message msg(len, msgA, f);
        message = msg;
        playerSet = 0;
    }
    HandleData(){
        Message msg;
        message = msg;
        playerSet = 0;
    }
};

char* generateWritableMap(char **, int x, int y, int boxes, int stones);
void generateMap(char **map, int x, int y, int boxes, int stones);
char* convertToOneDimension(char **map, int x, int y);
void insertObjects(char **map, char token, int number, int x, int y);
void resetMap(char **map, int x, int y);
// void addPlayerToMap(char **map, char playerId, int x, int y);
// void removePlayerFromMap(char **map, char playerId, int x, int y);
char * addPlayer(std::map < int, char* > players, int lastId, int clientFd);
// char *findPlayerId(int clientFd, std::map < int, char* > players);
// char* generatePlayersId(int newId);
std::list<HandleData> handlePlayersMsg(char **map, char *buffer, int clientFd, std::map < int, Player> players, GameSettings *gameSettings);
Player findPlayerById(std::map<int, Player> players, int id);
int isEveryoneReady(std::map < int, Player> players, char **map, GameSettings *gameSettings);
std::list<HandleData> sendLowerNames(std::map < int, Player> players, int clientFd, std::list<HandleData> hdList);

#endif