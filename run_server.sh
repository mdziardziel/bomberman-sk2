#!/bin/bash

cd server

cmake .

printf "\n"
make
printf "\n"

cd ..

server/bin/./BombermanServer