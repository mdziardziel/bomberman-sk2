#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 8

#include <map>
#include <string.h>
#include <unordered_set>
#include <list>
#include "Helpers.hpp"
#include <string>
// #include <time.h>

struct GameSettings{
    int mapX;
    int mapY;
    int time;

    GameSettings(){
        time = 300;
        mapX = 8;
        mapY = 8;
    }
};

class Message {

    std::string content;
    int length;
    int fd;
    int skipFd;

public: 

    Message(int len, char* cont, int f, int sf){
        content = std::string(cont);
        length = len;
        fd = f;
        skipFd = sf;
    }
    Message(){}

    const char* getContent(){
        return content.c_str();
    }

    int getLength(){
        return length;
    }

    int getFd(){
        return fd;
    }

    int getSkipFd(){
        return skipFd;
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
    time_t lastSeen;

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
        lastSeen = time(0);
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

    time_t getLastSeen(){return lastSeen;}

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
    void setLastSeen(time_t t) {lastSeen = t;}

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
void handlePlayersMsg(std::list<Message>* hdList, char **map, char *buffer, int clientFd, std::map < int, Player>* players, GameSettings *gameSettings);
Player findPlayerById(std::map<int, Player> *players, int id);
// int isEveryoneReady(std::map < int, Player> *players, char **map, GameSettings &gameSettings);
void sendLowerNames(std::map < int, Player> *players, int clientFd, std::list<Message> *hdList);
void receivePing(char *buffer, std::map < int, Player> *players, int clientFd, std::list<Message> *hdList);

#endif