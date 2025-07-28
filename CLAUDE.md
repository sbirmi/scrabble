# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

**Build the C++ game server:**
```bash
make
```

**Clean build artifacts:**
```bash
make clean
```

**Run the complete application:**
```bash
# Terminal 1: Start the game server (WebSocket backend)
./game-server 5051 games.sqlite3

# Terminal 2: Start the web UI (Flask frontend)
python3 scrabble-ui.py
```

**Access the application:**
- Web interface: http://localhost:5050
- Game server (WebSocket): ws://localhost:5051

## Architecture Overview

This is a multiplayer Scrabble game with a client-server architecture:

### Backend (C++)
- **game-server**: Main WebSocket server handling game logic, built with websocketpp
- **Game::Inst**: Core game state management (game.hpp/cpp)
- **Lobby::Inst**: Game lobby and matchmaking (lobby.hpp/cpp) 
- **Storage::Inst**: SQLite persistence layer (storage.hpp/cpp)
- **WordList**: Dictionary validation (word_list.hpp/cpp)
- **broadcast_server**: WebSocket connection management (game-server.hpp)

### Frontend
- **scrabble-ui.py**: Flask web server serving static assets and templates
- **templates/**: Jinja2 HTML templates (game.html, lobby.html)
- **js/**: Client-side JavaScript modules (game.js, lobby.js, ui.js, globals.js)
- **media/**: Audio assets (pop.wav)

### Game Flow
1. Players access web UI via Flask server
2. JavaScript establishes WebSocket connection to C++ game server
3. Game state persisted in SQLite database
4. Real-time communication via JSON messages over WebSocket

### Key Dependencies
- **C++**: jsoncpp, boost (system/thread), websocketpp, sqlite3
- **Python**: Flask
- **External**: wordlist.txt file required for word validation

### Communication Protocol
The client-server communication uses JSON messages over WebSocket for:
- JOIN/VIEW game commands
- PLAY/PASS/EXCH game moves  
- CHAT messages
- Real-time game state updates

Game state includes board tiles, player hands, scores, and turn management across multiple concurrent games.

## Testing

The project includes comprehensive unit tests with coverage measurement:

**Run all tests:**
```bash
make test
```

**Run tests with coverage:**
```bash
cd tests
./run_tests.sh
```

**Individual test modules:**
```bash
cd tests
make test_word_list && ./test_word_list
make test_json_util && ./test_json_util  
make test_storage && ./test_storage
make test_game && ./test_game
make test_lobby && ./test_lobby
```

**Coverage target:** 90% line coverage for most modules

**Test dependencies:**
- libgtest-dev: Google Test framework
- gcov: Coverage analysis (part of gcc)
- bc: Calculator for coverage percentage