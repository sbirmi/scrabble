all: game-server

CPPFLAGS=-Wall -I/usr/include/jsoncpp -ggdb
LDFLAGS=-ggdb

game-server.o: game-server.cpp game-server.hpp game.hpp
	g++ ${CPPFLAGS} -c game-server.cpp

broadcast_server.o: broadcast_server.cpp game-server.hpp
	g++ ${CPPFLAGS} -c broadcast_server.cpp

game.o: game.cpp game.hpp
	g++ ${CPPFLAGS} -c game.cpp

word_list.o: word_list.cpp word_list.hpp
	g++ ${CPPFLAGS} -c word_list.cpp

game-server: broadcast_server.o game-server.o game.o word_list.o
	g++ \
		${LDFLAGS} \
		broadcast_server.o \
		game-server.o \
		game.o \
		word_list.o \
		-lboost_system -lboost_thread -lpthread -ljsoncpp \
		-o game-server


clean:
	rm -f game-server *.o
