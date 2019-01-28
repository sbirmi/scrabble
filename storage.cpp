#include <cassert>
#include <iostream>

#include "storage.hpp"

namespace Storage {

Inst::Inst(const std::string _filename) : filename(_filename) {
   int rc;

   rc = sqlite3_open(filename.c_str(), &db);

   if (rc) {
      std::cerr << "Can't open " << filename << std::endl;
      assert(0 && "Can't open filename");
   }
}

Inst::~Inst() {
   sqlite3_close(db);
}

void
Inst::insert(const char *cmd) {
   char *errMsg = 0;
   int rc = sqlite3_exec(db, cmd, NULL, 0, &errMsg);

   if (rc != SQLITE_OK) {
      std::cerr << errMsg << std::endl;
      assert(0 && "SQL Error");
   }
}

void
Inst::select(const std::string &cmd,
      SQLITECALLBACK cb,
      void *ctx) {
   char *errMsg;
   int rc = sqlite3_exec(db, cmd.c_str(), cb, ctx, &errMsg);

   if (rc != SQLITE_OK) {
      std::cerr << errMsg << std::endl;
      sqlite3_free(errMsg);
      assert(0 && "SQL ERROR");
   }
}

// Helpers used to fetch state
typedef struct {
   unsigned int pid;
} player_id_data;

int
get_sql_player_id_cb(void *data, int argc, char **argv, char **argName)
{
   std::cout << __PRETTY_FUNCTION__ << " argc=" << argc << std::endl;
   assert(argc == 1);

   player_id_data *ctx = (player_id_data *) data;
   ctx->pid = atol(argv[0]);

   return 0;
}

unsigned int
Inst::get_sql_player_id(unsigned int gid, unsigned int plIdx) {
   char *cmd = sqlite3_mprintf("select pid from Players where gid=%u and plIdx=%u;",
         gid, plIdx);
   player_id_data data;
   select(std::string(cmd), get_sql_player_id_cb, &data);
   sqlite3_free(cmd);
   return data.pid;
}

// Helpers used by Game::Inst
void
Inst::add_game(unsigned int gid,
      unsigned int maxPlayers, unsigned int turnIndex,
      const std::string tiles) {
   char *cmd = sqlite3_mprintf("insert into Games "
         "(maxPlayers, turnIndex, tiles) values "
         "(%u, %u, '%q');",
         maxPlayers, turnIndex, tiles.c_str());
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_game_tiles(unsigned int gid,
      const std::string &tiles) {
   char *cmd = sqlite3_mprintf("update Games set tiles='%q' where gid=%u;", tiles.c_str(), gid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_game_words_made(unsigned int gid,
      unsigned int wordsMade) {
   char *cmd = sqlite3_mprintf("update Games set wordsMade=%u where gid=%u;",
                               wordsMade, gid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_game_passes_made(unsigned int gid,
      unsigned int passesMade) {
   char *cmd = sqlite3_mprintf("update Games set passesMade=%u where gid=%u;",
                               passesMade, gid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_game_turn_index(unsigned int gid,
      unsigned int turnIndex) {
   char *cmd = sqlite3_mprintf("update Games set turnIndex=%u where gid=%u;", turnIndex, gid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_game_over(unsigned int gid) {
   char *cmd = sqlite3_mprintf("update Games set gameOver=1 where gid=%u;", gid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::add_board_tile(unsigned int gid,
      unsigned int row, unsigned int col,
      char letter, int score) {
   char *cmd = sqlite3_mprintf("insert into BoardTiles "
         "(gid, row, col, letter, score) "
         "values (%u, %u, %u, '%c', %d);",
         gid, row, col, letter, score);
   insert(cmd);
   sqlite3_free(cmd);
}

// Players
unsigned int
Inst::add_player(unsigned int gid,
                 unsigned int plIdx,
                 const std::string &playerName,
                 const std::string &secretKey,
                 unsigned int state) {
   char *cmd = sqlite3_mprintf("insert into Players "
         "(gid, plIdx, playerName, secretKey, state) "
         "values (%u, %u, '%q', '%q', %u);",
         gid, plIdx,
         playerName.c_str(), secretKey.c_str(),
         state);
   insert(cmd);
   sqlite3_free(cmd);

   return get_sql_player_id(gid, plIdx);
}

void
Inst::update_player_hand(unsigned int pid,
      const std::string &hand) {
   char *cmd = sqlite3_mprintf("update Players set hand='%q' where pid=%u;", hand.c_str(), pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_player_state(unsigned int pid,
      unsigned int state) {
   char *cmd = sqlite3_mprintf("update Players set state=%u where pid=%u;", state, pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_player_score(unsigned int pid,
      int score) {
   char *cmd = sqlite3_mprintf("update Players set score=%d where pid=%u;", score, pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_player_end_adj_score(unsigned int pid,
      int deltaScore) {
   char *cmd = sqlite3_mprintf("update Players set endScoreAdj=%d where pid=%u;",
                               deltaScore, pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::update_player_turnkey(unsigned int pid,
         const std::string &turnKey) {
   char *cmd = sqlite3_mprintf("update Players set turnKey='%q' where pid=%u;",
                               turnKey.c_str(), pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::add_player_move_pass(unsigned int gid,
      unsigned int pid) {
   char *cmd = sqlite3_mprintf("insert into Moves "
         "(gid, pid, moveType, score) values "
         "(%u, %u, 0, 0);",
         gid, pid);
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::add_player_move_exch(unsigned int gid,
      unsigned int pid,
      const std::string &removedLetters) {
   char *cmd = sqlite3_mprintf("insert into Moves "
         "(gid, pid, moveType, longestWord, score) values "
         "(%u, %u, 1, '%q', 0);",
         gid, pid, removedLetters.c_str());
   insert(cmd);
   sqlite3_free(cmd);
}

void
Inst::add_player_move_play(unsigned int gid,
      unsigned int pid,
      const std::string &strJsonMove,
      const std::string &longest_word,
      int total_score) {
   char *cmd = sqlite3_mprintf("insert into Moves "
         "(gid, pid, moveType, moveDetail, longestWord, score) values "
         "(%u, %u, 2, '%q', '%q', %d);",
         gid, pid,
         strJsonMove.c_str(), longest_word.c_str(),
         total_score);
   insert(cmd);
   sqlite3_free(cmd);
}

}; // namespace Storage
