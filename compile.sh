#! /bin/bash
g++ -std=c++14 -O3 -o ./Debug/CryptoProgram CryptoProgram.cpp;
g++ -std=c++14 -O3 -o ./Debug/DES.o -c DES.cpp;
mpic++ -std=c++14 -O3 -o ./Debug/Mode.o -c Mode.cpp;
mpic++ -std=c++14 -O3 -o ./Debug/Execute.o -c Execute.cpp
cd ./Debug
mpic++ -std=c++14 -O3 -o Execute Execute.o Mode.o DES.o
rm -f *.o
