#ifndef __GAME_HPP
#define __GAME_HPP

#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <websocketpp/common/thread.hpp>

#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

#include <string>
#include <map>
#include <set>
#include <utility>
#include <vector>

#include "word_list.hpp"

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection_hdl Handle;
typedef std::pair<const Handle&, std::string> HandleResponse;
typedef std::vector<HandleResponse> HandleResponseList;

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
   HandleClient clients;
   std::string name;
   int score = 0;
   std::string hand;
   std::string passwd;
   // Since there maybe multiple clients,
   // we track the state of the game
   // here once a player connects
   enum ClientState state;
   std::string turnkey;

public:
   Player(std::string _name,
          std::string _passwd,
          enum ClientState _state) :
      name(_name), passwd(_passwd),
   state(_state) {};

   bool maybeAddClient(Client *);
   bool maybeRemoveClient(const Handle &);
};

class Inst {
private:
   unsigned int gid;
   const WordList *wl;
   const unsigned int maxPlayers;
   std::vector<Player *> players;
   bool gameOver;
   HandleClient viewers;
   HandleMode handleMode;
   int boardscoreRC[15][15] = {0};
   unsigned int tempBoardScoreRC[15][15] = {0};
   char boardRC[15][15];
   char tempBoardRC[15][15];
   unsigned int movesMade;

   Json::Reader *jsonReader;
   Json::FastWriter *jsonWriter;

   unsigned int turnIndex = 0;
   std::string tiles;

   Json::Value get_score_json();
   Json::Value get_player_turn_message();
   Json::Value get_others_turn_message();

   bool touches_old_tile(int r, int c);

   // Game event handling
   bool is_new_tile(unsigned int r,
                    unsigned int c) const;
   void start_game(HandleResponseList &hrl);
   void next_turn(HandleResponseList &hrl);
   int play_word_score(const PlayMove &,
                       bool wordAlongRow);
   // returns negative value if invalid word
   int play_score(const std::vector<PlayMove>&);
   std::string issue_tiles(unsigned int plIdx,
                           HandleResponseList& hrl);

   // Dump game state
   void dump_game_state(const Handle& hdl, HandleResponseList &hrl);

   // Received message handling
   void broadcast_json_message(HandleResponseList &hrl, const Json::Value& json);
   void broadcast_json_message(HandleResponseList &hrl, const Json::Value& playerJson, const Json::Value& othersJson);

   void broadcast_score_messages(HandleResponseList &hrl);
   void broadcast_turn_messages(HandleResponseList &hrl);

   HandleResponseList process_cmd(
         const Handle&hdl,
         const Json::Value& val);
   HandleResponseList process_cmd_view(
         const Handle& hdl,
         const Json::Value &cmdJson);
   HandleResponseList process_cmd_join(
         const Handle& hdl,
         const Json::Value &cmdJson);
   HandleResponseList process_cmd_pass(
         const Handle& hdl,
         const Json::Value &cmdJson);
   HandleResponseList process_cmd_play(
         const Handle& hdl,
         const Json::Value &cmdJson);

   // convenience methods
   std::string stringify(const Json::Value& json);
   Json::Value jsonify(int);
   Json::Value jsonify(const std::string&);
   Json::Value jsonify(const std::string&, const std::string&);
   Json::Value jsonify(const std::string&, const std::string&,
                       const std::string&);
   Json::Value jsonify(const char letter, unsigned int row,
                       unsigned int col);
   HandleResponse generateResponse(
         const Handle& hdl, Json::Value json);

public:
   Inst(unsigned int _gid, const WordList *);

   void handle_appear(const Handle &hdl);
   void handle_disappear(const Handle &hdl);
   HandleResponseList process_msg(
         const Handle& hdl,
         const server::message_ptr& msg);
};

}

#endif
