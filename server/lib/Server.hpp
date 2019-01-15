#ifndef Server_HPP
#define Server_HPP

#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <error.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <poll.h> 
#include <thread>
#include <unordered_set>
#include <signal.h>
#include <sstream>
#include <string>
#include <map>
#include "Engine.hpp"

void removeClient(int clientFd, std::unordered_set<int> &clientFds);
uint16_t readPort(char * txt);
void setReuseAddr(int sock);
// void ctrl_c(int );
void sendToOne(const char * buffer, int count, int clientFd);
void sendToAll(const char * buffer, int count, std::map<int, Player> players);
void sendToAlmostAll(const char * buffer, int count, std::map<int, Player> players, int skipFd);

uint16_t getPortNumber(int defaultPort,int argc, char **argv);
int createSocket(int ai, int ss, int flag);
void bindToAddressAndPort(int port, int listenSock);
void enterListeningMode(int listenSock);
int createEpoll();
int connectNewClient(epoll_event event, int listenSock, int epollFd);
// std::map<int, Player> checkConnections(std::map<int, Player> players);
void sendToOne(char * buffer, int count, int clientFd);

#endif