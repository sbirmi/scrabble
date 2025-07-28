# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build and Development Commands

All build and development tasks run within Docker using Dockerfile-test for consistency and isolation.

**Build and run the complete application in Docker:**
```bash
# Build test image using local files
docker build -f Dockerfile-test -t scrabble-test .

# Run application container with port mapping
docker run -p 5050:5050 -p 5051:5051 scrabble-test
```

**Build C++ components only in Docker:**
```bash
# Build test container and run make
docker run --rm -v $(pwd):/app -w /app scrabble-test make
```

**Clean build artifacts in Docker:**
```bash
# Run clean command in Docker container
docker run --rm -v $(pwd):/app -w /app scrabble-test make clean
```

**Access the application:**
- Web interface: http://localhost:5050
- Game server (WebSocket): ws://localhost:5051

**Manual testing verification:**
After running the Docker container, manually verify the service works by:
1. Opening http://localhost:5050 in your browser
2. Confirming the Scrabble game interface loads
3. Testing basic game functionality (creating/joining games)

**IMPORTANT:** Do NOT attempt automated verification (curl/wget) while the service is running - ask the user to manually test instead. Wait for user confirmation that testing is complete before stopping the Docker container.

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

The project includes comprehensive unit tests with coverage measurement that run exclusively in Docker using Dockerfile-test:

**Run all tests in Docker:**
```bash
# Build test image first
docker build -f Dockerfile-test -t scrabble-test .

# Run working tests only (game tests currently have API compatibility issues)
docker run --rm scrabble-test /bin/bash -c "cd tests && make test_word_list test_json_util test_storage test_lobby && ./test_word_list && ./test_json_util && ./test_storage && ./test_lobby"
```

**Run quick tests in Docker:**
```bash
# Build test image first (if not already built)
docker build -f Dockerfile-test -t scrabble-test .

# Run quick tests only (direct command due to Makefile path issues)
docker run --rm scrabble-test /bin/bash -c "cd tests && make test_word_list test_json_util && ./test_word_list && ./test_json_util"
```

**Manual Docker testing:**
```bash
# Build test image
docker build -f Dockerfile-test -t scrabble-test .

# Run specific tests
docker run --rm scrabble-test /bin/bash -c "cd tests && make test_word_list && ./test_word_list"

# Interactive testing session
docker run --rm -it scrabble-test /bin/bash
```

**Coverage target:** 90% line coverage for most modules

**Docker-only testing:**
- No local dependencies required
- All tests run in isolated containers using Dockerfile-test
- Consistent environment across different machines
- Tests create their own wordlist.txt and database files

**Note:** Game tests currently have API compatibility issues and require updates. Working tests include: word_list, json_util, storage, and lobby.

**Git Hook Setup:**
```bash
# Set up pre-commit hook to run tests automatically in Docker
./setup-git-hooks.sh
```

The pre-commit hook will:
- Prevent committing wordlist.txt files
- Run quick tests in Docker using Dockerfile-test before each commit
- Abort commit if any tests fail