CryptoProgram: CryptoProgram.cpp
mpic++ -std=c++14 -o CryptoProgram CryptoProgram.cpp
Execute: Execute.o Mode.o DES.o
mpic++ -std=c++14 -o Execute Execute.o Mode.o DES.o
DES.o: DES.h DES.cpp
mpic++ -std=c++14 -c DES.cpp
Mode.o: Mode.h DES.h Mode.cpp
mpic++ -std=c++14 -c Mode.cpp
Execute.o: DES.h TimeRun.h Mode.h Execute.cpp
mpic++ -std=c++14 -c Execute.cpp

