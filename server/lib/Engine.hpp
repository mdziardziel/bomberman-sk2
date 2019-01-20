#ifndef ENGINE_HPP
#define ENGINE_HPP

#define MAX_PLAYERS 8

#include <map>
#include <string.h>
#include <unordered_set>
#include <list>
#include "Helpers.hpp"
#include <string>
#include <stdlib.h>
#include <time.h>

// #include <time.h>

struct GameSettings{
    int mapX;
    int mapY;
    int time;

    GameSettings(){
        time = 60;
        mapX = 8;
        mapY = 8;
    }

    char* getCharX(){
        return toChar2(mapX);
    }

    char* getCharY(){
        return toChar2(mapY);
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
        nameSize = 1;
    }
    Player(){
        fd = 0;
        id = -1;
        points = 0;
        readyToPlay = 0;
        x = -1;
        y = -1;
        char nothing[1] = {' '};
        name = nothing;
        lastSeen = time(0);
        nameSize = 1;
    }

    int getFd(){
        return fd;
    }
    char* getX(){
        return toChar2(x);
    }
    char* getY(){
        return toChar2(y);
    }
    int getIntX(){
        return x;
    }
    int getIntY(){
        return y;
    }
    int getPoints(){
        return points;
        }
    char getState(){
        return toChar1(readyToPlay);
    }
    time_t getLastSeen(){
        return lastSeen;
    }
    int addPoint(){
        return ++points;
    }
    void resetPoints(){
        points = 0;
    }
    int isReady(){
        return readyToPlay; 
    }
    char getCharId(){
        return toChar1(id);
    }
    int getId(){
        return id;
    }
    void ready(){
        readyToPlay = 1;
    }
    void notReady(){
        readyToPlay = 0;
    }
    char* getName(){
        return name;
    }
    int getNameSize(){
        return nameSize;
    }
    void setId(int d){
        id = d;
    }
    void setName(char* n, int m){
        name = n;
        nameSize = m;
    }
    void setX(char *xx){
        x = toInt(xx);
    }
    void setY(char *yy){
        y = toInt(yy);
    }
    void setX(int xx){
        x = xx;
    }
    void setY(int yy){
        y = yy;
    }
    void setLastSeen(time_t t){
        lastSeen = t;
    }
};

char* generateWritableMap(char **, GameSettings gs, int boxes);
void generateMap(char **map, GameSettings gs, int boxes);
char* convertToOneDimension(char **map, GameSettings gameSettings);
void insertBoxes(char **map, int number, GameSettings gs);
void resetMap(char **map, GameSettings gs);
char * addPlayer(std::map < int, char* > players, int lastId, int clientFd);
void handlePlayersMsg(std::list<Message>* hdList, char **map, char *buffer, int clientFd, std::map < int, Player>* players, GameSettings *gameSettings, int remainingTime);
int findFdById(std::map<int, Player> *players, int id);
void sendLowerNames(std::map < int, Player> *players, int clientFd, std::list<Message> *hdList);
void receivePing(char *buffer, std::map < int, Player> *players, int clientFd, std::list<Message> *hdList);
void sendTime(int remainingTime, std::list<Message> *list, int clientFd);
void sendPoints(char id, int ptsInt, std::list<Message> *list);
void sendMapSies(GameSettings gs, std::list<Message> *list, int fd);
void sendPlayerPosition(char playerId, char* x, char* y, std::list<Message> *list, int fd);
void sendPlyersPositions(std::list<Message> *list, std::map < int, Player>* players);
void generatePlyersPositions(std::map < int, Player>* players, GameSettings gs, char** map);
int getLastId(std::map < int, Player>* players);
void reuseId(std::map < int, Player>* players, int id);
int validatePosition(int f, int ogr);
void generatePlyersPosition(std::map < int, Player>* players, GameSettings gs, char** map, int clientFd);
int validateName(std::map < int, Player>* players, char *name, int len);
int getWinner(std::map < int, Player>* players);
void updatePlayersStateAfterRound(std::map < int, Player>* players, std::list<Message>* hdList);
void sendPointsToAll(std::map < int, Player>* players, std::list<Message>* hdList);
void sendPlayerStateToAll(char state, int clientFd, std::list<Message>* hdList, std::map < int, Player>* players);
void sendAllPlayersState(std::list<Message>* hdList, std::map < int, Player>* players);

#endif