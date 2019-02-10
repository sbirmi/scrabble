#ifndef __STORAGE_COMMON_HPP
#define __STORAGE_COMMON_HPP

#include <string>
#include <vector>

/* This file contains common definitions used between
 * storage.hpp, lobby.hpp, and game.hpp
 */

typedef struct {
   unsigned int gid;
   unsigned int maxPlayers;
   unsigned int turnIndex;
   unsigned int wordsMade;
   unsigned int passesMade;
   unsigned int gameOver;
   std::string tiles;
} StorageGame;

typedef std::vector<StorageGame> StorageGameList;

typedef struct {
   unsigned int row;
   unsigned int col;
   char letter;
   int score;
} StorageBoardTile;

typedef std::vector<StorageBoardTile> StorageBoardTileList;

typedef struct {
   unsigned int pid;
   unsigned int plIdx;
   std::string playerName;
   std::string secretKey;
   int score;
   std::string hand;
   unsigned int state;
   std::string turnKey;
   int endScoreAdj;
} StoragePlayer;

typedef std::vector<StoragePlayer> StoragePlayerList;

typedef struct {
   unsigned int moveType;
   std::string longestWord;
   int score;
} StorageMove;

typedef std::vector<StorageMove> StorageMoveList;

#endif
