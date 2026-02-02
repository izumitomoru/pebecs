#!/bin/zsh

cd ./

clang++ -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio -o ./build/build&&./build/build
#cd build/
#./build
#sudo clang++ -std=c++23 -I./src/include -L/usr/lib src/functions.cpp src/main.cpp -o /usr/bin/template

# sym link with /usr/bin/
#ln -s /usr/bin/template ./build/template
