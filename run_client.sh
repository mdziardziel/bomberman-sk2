#!/bin/bash

cd client
cmake .

printf "\n"
make
printf "\n"

cd ..

client/bin/./BombermanClient