all: game-server

CPPFLAGS=-Wall -I/usr/include/jsoncpp -ggdb
LDFLAGS=-ggdb -lboost_system -lboost_thread -lpthread -ljsoncpp

game-server.o: game-server.cpp game-server.hpp game.hpp
	g++ ${CPPFLAGS} -c game-server.cpp

broadcast_server.o: broadcast_server.cpp game-server.hpp
	g++ ${CPPFLAGS} -c broadcast_server.cpp

game.o: game.cpp game.hpp
	g++ ${CPPFLAGS} -c game.cpp

game-server: broadcast_server.o game-server.o game.o
	g++ \
		${LDFLAGS} \
		-o game-server \
		broadcast_server.o \
		game-server.o \
		game.o


clean:
	rm -f game-server *.o
