// 'Hello World!' program 
 
#include <iostream>
#include "ClientConfig.hpp"
 
int main()
{
  std::cout << "Hello World!" << std::endl;
  std::cout << BombermanClient_VERSION_MAJOR << std::endl;
  std::cout << BombermanClient_VERSION_MINOR << std::endl;
  return 0;
}