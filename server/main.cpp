// 'Hello World!' program 
 
#include <iostream>
#include "BombermanServerConfig.hpp"
 
int main()
{
  std::cout << "Hello World!" << std::endl;
  fprintf(stdout,"%s Version %d.%d\n",
          argv[0],
          BombermanServer_VERSION_MAJOR,
          BombermanServer_VERSION_MINOR);
  return 0;
}