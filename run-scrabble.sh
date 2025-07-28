#!/bin/bash
python3 scrabble-ui.py &
./game-server 5051 games.sqlite3
