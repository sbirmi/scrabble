# Test Makefile with coverage support
CXX = g++
CXXFLAGS = -Wall -I/usr/include/jsoncpp -I.. -std=c++17 -g --coverage
LDFLAGS = -g --coverage
LIBS = -lboost_system -lboost_thread -lpthread -ljsoncpp -lsqlite3 -lgtest -lgtest_main -lstdc++fs

# Source files to test (excluding main)
TEST_SOURCES = ../word_list.cpp ../json_util.cpp ../storage.cpp ../game.cpp ../lobby.cpp
TEST_OBJECTS = $(TEST_SOURCES:.cpp=.o)

# Test files
TEST_FILES = test_word_list.cpp test_json_util.cpp test_storage.cpp test_game.cpp test_lobby.cpp
TEST_EXECUTABLES = $(TEST_FILES:.cpp=)

.PHONY: all test coverage clean setup

all: setup $(TEST_EXECUTABLES)

setup:
	@echo "Setting up test environment..."
	@echo "HELLO\nWORLD\nTEST\nSCRABBLE\nGAME" > test_wordlist.txt

# Individual test targets
test_word_list: test_word_list.cpp ../word_list.o
	$(CXX) $(CXXFLAGS) -o $@ $< ../word_list.o $(LIBS)

test_json_util: test_json_util.cpp ../json_util.o
	$(CXX) $(CXXFLAGS) -o $@ $< ../json_util.o $(LIBS)

test_storage: test_storage.cpp ../storage.o
	$(CXX) $(CXXFLAGS) -o $@ $< ../storage.o $(LIBS)

test_game: test_game.cpp ../game.o ../storage.o ../word_list.o ../json_util.o
	$(CXX) $(CXXFLAGS) -o $@ $< ../game.o ../storage.o ../word_list.o ../json_util.o $(LIBS)

test_lobby: test_lobby.cpp ../lobby.o ../storage.o ../json_util.o
	$(CXX) $(CXXFLAGS) -o $@ $< ../lobby.o ../storage.o ../json_util.o $(LIBS)

# Compile source files with coverage
../%.o: ../%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: all
	@echo "Running all tests..."
	@for test in $(TEST_EXECUTABLES); do \
		echo "Running $$test..."; \
		./$$test || exit 1; \
	done

coverage: test
	@echo "Generating coverage report..."
	@gcov $(TEST_SOURCES)
	@echo "Coverage files generated. Summary:"
	@for gcov_file in *.gcov; do \
		if [ -f "$$gcov_file" ]; then \
			filename=$$(basename "$$gcov_file" .gcov); \
			lines_exec=$$(grep -c '    #####:' "$$gcov_file" 2>/dev/null || echo 0); \
			lines_total=$$(grep -c '^    [0-9#-]' "$$gcov_file" 2>/dev/null || echo 1); \
			if [ $$lines_total -gt 0 ]; then \
				coverage_pct=$$(echo "scale=1; ($$lines_total - $$lines_exec) * 100 / $$lines_total" | bc -l 2>/dev/null || echo "0"); \
				echo "$$filename: $$coverage_pct% coverage"; \
			fi; \
		fi; \
	done

clean:
	rm -f $(TEST_EXECUTABLES) *.o ../*.o *.gcov *.gcda *.gcno ../*.gcda ../*.gcno test_*.sqlite3 test_wordlist.txt