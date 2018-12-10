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
#include <tuple>
#include <vector>

typedef websocketpp::server<websocketpp::config::asio> server;
typedef websocketpp::connection_hdl Handle;


typedef std::tuple<const Handle *, std::string> HandleResponse;
typedef std::vector<HandleResponse> HandleResponseList;

typedef int ClientMode;
#define ModeIdle     (-2)
#define ModeViewer   (-1)

typedef std::map<const Handle *, ClientMode> HandleMode;

namespace Game {

class Inst;

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
};

class Client {
   friend class Inst;
   friend class Player;
private:
   enum ClientState state;
   const Handle *hdl;
public:
   Client(const Handle* _hdl,
          ClientState _state) :
      state(_state), hdl(_hdl) {};
};

class Player {
   friend class Inst;
private:
   std::set<Client *> clients;
   std::string name;
   unsigned int score = 0;
   std::string passwd;
   // Since there maybe multiple clients,
   // we track the state of the game
   // here once a player connects
   enum ClientState state;

public:
   Player(std::string _name,
          std::string _passwd,
          enum ClientState _state) :
      name(_name), passwd(_passwd),
   state(_state) {};

   bool maybeAddClient(Client *);
   bool maybeRemoveClient(Client *);
   bool maybeRemoveClient(const Handle *);
};

class Inst {
private:
   unsigned int gid;
   const unsigned int max_players = 2;
   std::vector<Player *> players;
   std::set<Client *> viewers;
   HandleMode handleMode;

   Json::Reader *jsonReader;
   Json::FastWriter *jsonWriter;

   // Game event handling
   Json::Value get_score_json();

   // Dump game state
   void dump_game_state(const Handle& hdl, HandleResponseList &hrl);

   // Received message handling
   HandleResponseList broadcast_score_messages();
   HandleResponseList process_cmd(
         const Handle&hdl,
         const Json::Value& val);
   HandleResponseList process_cmd_view(
         const Handle& hdl,
         const Json::Value &cmdJson);
   HandleResponseList process_cmd_join(
         const Handle& hdl,
         const Json::Value &cmdJson);

   // convenience methods
   std::string stringify(const Json::Value& json);
   Json::Value jsonify(unsigned int);
   Json::Value jsonify(std::string);
   Json::Value jsonify(std::string, std::string);
   HandleResponse generateResponse(
         const Handle& hdl, Json::Value json);

public:
   Inst(unsigned int _gid) :
      gid(_gid),
      jsonReader(new Json::Reader()),
      jsonWriter(new Json::FastWriter()) {};

   void handle_appear(const Handle &hdl);
   void handle_disappear(const Handle &hdl);
   HandleResponseList process_msg(
         const Handle& hdl,
         const server::message_ptr& msg);
};

}

#endif
