#ifndef __GAME_HPP
#define __GAME_HPP

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "json_util.hpp"
#include "conn.hpp"
#include "word_list.hpp"
#include "storage.hpp"

typedef unsigned int GameId;

typedef websocketpp::server<websocketpp::config::asio> server;

typedef int ClientMode;
#define ModeIdle     (-2)
#define ModeViewer   (-1)

typedef std::map<const Handle, ClientMode, std::owner_less<Handle>> HandleMode;

namespace Game {

class Inst;

struct PlayMove {
   char letter;
   unsigned int row;
   unsigned int col;
};

enum ClientState {
   // wait_for_handshake
   //  "JOIN":
   //    if join is successful:
   //       # of players = 1
   //          --> wait_for_second_player
   //       # of players = 2
   //          --> game start { await_turn, turn }
   //       
   //    if join fails (bad password etc):
   //       go to viewer
   //
   //  "VIEW": --> viewer
   //  anything else:
   //    stay in this state
   wait_for_handshake,

   // viewers stay in this state
   viewer,

   // waiting for second player
   //  "OTHER_JOIN":
   //    --> game start { await_turn, turn }
   wait_for_more_players,

   // game started
   await_turn,
   turn,

   // game completed
   game_over,
};

class Client {
   friend class Inst;
   friend class Player;
private:
   enum ClientState state;
   const Handle& hdl;
public:
   Client(const Handle& _hdl,
          ClientState _state) :
      state(_state), hdl(_hdl) {};
};

typedef std::map<const Handle, Client*, std::owner_less<Handle>> HandleClient;

class Player {
   friend class Inst;
private:
   unsigned int pid;
   HandleClient clients;
   std::string name;
   std::string passwd;
   int score = 0;
   std::string hand;
   // Since there maybe multiple clients,
   // we track the state of the game
   // here once a player connects
   enum ClientState state;
   std::string turnkey;
   int endScoreAdj = 0;

public:
   Player(unsigned int _pid,
          std::string _name,
          std::string _passwd,
          enum ClientState _state) :
      pid(_pid), name(_name), passwd(_passwd),
      state(_state) {};

   // Used to restore from storage
   Player(unsigned int _pid,
          std::string _name,
          std::string _passwd,
          int _score,
          std::string _hand,
          enum ClientState _state,
          std::string _turnKey,
          int _endScoreAdj) :
      pid(_pid), name(_name), passwd(_passwd),
      score(_score), hand(_hand),
      state(_state), turnkey(_turnKey),
      endScoreAdj(_endScoreAdj) {};

   bool maybeAddClient(Client *);
   bool maybeRemoveClient(const Handle &);
};

class Inst {
private:
   GameId gid;
   const WordList *wl;
   Storage::Inst *storage;
   const unsigned int maxPlayers;
   std::vector<Player *> players;
   bool gameOver;
   HandleClient viewers;
   HandleMode handleMode;
   int boardscoreRC[15][15] = {0};
   unsigned int tempBoardScoreRC[15][15] = {0};
   char boardRC[15][15];
   char tempBoardRC[15][15];
   unsigned int wordsMade;
   unsigned int passesMade;

   Json::Reader *jsonReader;
   Json::FastWriter *jsonWriter;

   unsigned int turnIndex = 0;
   std::string tiles;
   StorageMoveList moveList;

   Json::Value get_score_json();
   Json::Value get_player_turn_message();
   Json::Value get_others_turn_message();

   bool touches_old_tile(int r, int c);

   // Game event handling
   void shuffle_bag();
   bool is_new_tile(unsigned int r,
                    unsigned int c) const;
   void start_game(HandleResponseList &hrl);
   // pass indicates if the last move was a pass
   void next_turn(HandleResponseList &hrl, bool pass);
   std::string play_word_score(HandleResponseList& hrl,
                               const Handle& hdl, 
                               const PlayMove &,
                               bool wordAlongRow,
                               int *score);
   // returns negative value if invalid word
   bool play_score(HandleResponseList& hrl,
                   const Handle& hdl,
                   const std::vector<PlayMove>&,
                   const std::string &strJsonMove);
   std::string issue_tiles(unsigned int plIdx,
                           HandleResponseList& hrl);
   bool exchange_tiles(std::string,
                       const Handle& hdl, 
                       HandleResponseList&);

   // Dump game state
   void dump_game_state(const Handle& hdl, HandleResponseList &hrl);

   // Received message handling
   void broadcast_json_message(HandleResponseList &hrl, const Json::Value& json);
   void broadcast_json_message(HandleResponseList &hrl, const Json::Value& playerJson, const Json::Value& othersJson);

   void broadcast_score_messages(HandleResponseList &hrl);
   void broadcast_turn_messages(HandleResponseList &hrl);

   bool process_cmd(
         const Handle&hdl,
         const Json::Value& val,
         HandleResponseList& hrl);
   bool process_cmd_chat(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_view(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_join(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_pass(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_exch(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_play(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_list_moves(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);
   bool process_cmd_check(
         const Handle& hdl,
         const Json::Value &cmdJson,
         HandleResponseList& hrl);

   // convenience methods
   std::string stringify(const Json::Value& json);
   HandleResponse generateResponse(
         const Handle& hdl, Json::Value json);

public:
   Inst(unsigned int _gid, const WordList *,
        Storage::Inst *,
        const unsigned int,
        Json::Reader *, Json::FastWriter *);

   // Used to restore from storage
   Inst(unsigned int _gid, const WordList *,
        Storage::Inst *,
        unsigned int _maxPlayers,
        bool _gameOver,
        unsigned int _wordsMade,
        unsigned int _passesMade,
        Json::Reader *,
        Json::FastWriter *,
        unsigned int _turnIndex,
        std::string tiles);

   // Used for restoring from storage
   void loadBoardTiles(const StorageBoardTileList& boardTileList);
   void loadPlayers(const StoragePlayerList& playerList);
   void loadMoves(const StorageMoveList& moveList);

   void handle_appear(const Handle &hdl);
   void handle_disappear(const Handle &hdl);
   // Returns if the game status has changed
   // such that lobby viewers should be notified
   bool process_msg(
         const Handle& hdl,
         const server::message_ptr& msg,
         HandleResponseList& hrl);

   Json::Value status();

   // Test helper methods
   bool test_word_check(const Handle& hdl, const Json::Value& cmdJson, HandleResponseList& hrl);
   void test_join_player(const Handle& hdl, const Json::Value& cmdJson, HandleResponseList& hrl);
   void test_set_viewer(const Handle& hdl, const Json::Value& cmdJson, HandleResponseList& hrl);
};

}

#endif
