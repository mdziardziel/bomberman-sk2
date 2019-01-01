// 'Hello World!' program 
 
#include <iostream>
#include "ServerConfig.hpp"
 
int main()
{
  std::cout << "Hello World!" << std::endl;
  std::cout << BombermanServer_VERSION_MAJOR << std::endl;
  std::cout << BombermanServer_VERSION_MINOR << std::endl;
  return 0;
}