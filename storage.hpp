#ifndef __STORAGE_HPP
#define __STORAGE_HPP

#include <sqlite3.h>
#include <string>

#include "storage_common.hpp"

namespace Storage {

class Inst {
   typedef int (*SQLITECALLBACK)(void *, int, char **, char **);

   const std::string filename;
   sqlite3* db;

public:
   Inst(std::string);
   ~Inst();

private:
   void insert(const char *cmd);
   void fetch(const char *cmd,
              SQLITECALLBACK cb,
              void *ctx);

   // Helpers used to fetch state
   unsigned int get_sql_player_id(unsigned int gid, unsigned int plIdx);

public:
   StorageGameList game_list();
   StorageBoardTileList game_board_tile_list(unsigned int gid);
   StoragePlayerList game_player_list(unsigned int gid);
   StorageMoveList game_move_list(unsigned int gid);

   // Helpers used by Game::Inst
   void add_game(unsigned int gid,
         unsigned int maxPlayers,
         unsigned int turnIndex,
         const std::string tiles);
   void update_game_tiles(unsigned int gid,
         const std::string &tiles);
   void update_game_words_made(unsigned int gid,
         unsigned int wordsMade);
   void update_game_passes_made(unsigned int gid,
         unsigned int passesMade);
   void update_game_turn_index(unsigned int gid,
         unsigned int turnIndex);
   void update_game_over(unsigned int gid);

   void add_board_tile(unsigned int gid,
         unsigned int row, unsigned int col,
         char letter, int score);

   // Player methods
   unsigned int add_player(unsigned int gid,
         unsigned int plIdx,
         const std::string &playerName,
         const std::string &secretKey,
         unsigned int state);
   void update_player_hand(unsigned int pid,
         const std::string &hand);
   void update_player_state(unsigned int pid,
         unsigned int state);
   void update_player_score(unsigned int pid,
         int score);
   void update_player_end_adj_score(unsigned int pid,
         int deltaScore);
   void update_player_turnkey(unsigned int pid,
         const std::string &turnKey);
   void add_player_move_pass(unsigned int gid,
         unsigned int pid);
   void add_player_move_exch(unsigned int gid,
         unsigned int pid, const std::string &lettersRemoved);
   void add_player_move_play(unsigned int gid,
         unsigned int pid,
         const std::string &strJsonMove,
         const std::string &longest_word,
         int total_score);
};

};

#endif
