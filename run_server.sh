#!/bin/bash

cd server

cmake .

printf "\n"
make
printf "\n"

cd ..

(cd server/bin/ && ./BombermanServer)