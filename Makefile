all: game-server

CPPFLAGS=-Wall -I/usr/include/jsoncpp -ggdb
LDFLAGS=-ggdb

storage.o: storage.cpp storage.hpp storage_common.hpp
	g++ ${CPPFLAGS} -c storage.cpp

game-server.o: game-server.cpp game-server.hpp game.hpp
	g++ ${CPPFLAGS} -c game-server.cpp

broadcast_server.o: broadcast_server.cpp game-server.hpp
	g++ ${CPPFLAGS} -c broadcast_server.cpp

game.o: game.cpp game.hpp conn.hpp storage.hpp
	g++ ${CPPFLAGS} -c game.cpp

word_list.o: word_list.cpp word_list.hpp
	g++ ${CPPFLAGS} -c word_list.cpp

lobby.o: lobby.cpp lobby.hpp conn.hpp storage_common.hpp
	g++ ${CPPFLAGS} -c lobby.cpp

json_util.o: json_util.cpp json_util.hpp
	g++ ${CPPFLAGS} -c json_util.cpp

game-server: broadcast_server.o game-server.o game.o word_list.o lobby.o json_util.o storage.o
	g++ \
		${LDFLAGS} \
		broadcast_server.o \
		game-server.o \
		game.o \
		word_list.o \
		lobby.o \
		json_util.o \
		storage.o \
		-lboost_system -lboost_thread -lpthread -ljsoncpp -lsqlite3 \
		-o game-server


test:
	@echo "Running unit tests..."
	@cd tests && ./run_tests.sh

test-setup:
	@echo "Setting up test environment..."
	@sudo apt-get update && sudo apt-get install -y libgtest-dev gcov bc

clean:
	rm -f game-server *.o
	@cd tests && make clean 2>/dev/null || true
