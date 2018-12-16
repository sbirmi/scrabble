#include <algorithm>
#include <random>

#include "game.hpp"

using namespace Game;

#define  MAX_RAND_STRING      (8)

std::string
randomString() {
   static std::string lastRandomString = "";
   std::string resp = lastRandomString;
   static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";

   while (resp == lastRandomString) {
      resp = "";
      for (unsigned int i=0; i<MAX_RAND_STRING; ++i) {
         resp.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
      }
   }
   lastRandomString = resp;
   return resp;
}


//
// Player methods
//
bool
Player::maybeAddClient(Client *cl) {
   // Check if client is already connected. If not, add him now
   clients[cl->hdl] = cl;
   return true;
}

bool
Player::maybeRemoveClient(const Handle& hdl) {
   Client *cl = clients[hdl];
   delete cl;

   clients.erase(hdl);
   return true;
}


//
// Instance methods
//

//
// Convenience methods
//
std::string
Inst::stringify(const Json::Value& json) {
   return jsonWriter->write(json);
}

Json::Value
Inst::jsonify(unsigned int i) {
   return Json::Value(i);
}

Json::Value
Inst::jsonify(const std::string& s1) {
   Json::Value json;
   json.append(Json::Value(s1));
   return json;
}

Json::Value
Inst::jsonify(const std::string& s1, const std::string& s2) {
   Json::Value json;
   json.append(Json::Value(s1));
   json.append(Json::Value(s2));
   return json;
}

Json::Value
Inst::jsonify(const std::string& s1, const std::string& s2,
              const std::string& s3) {
   Json::Value json;
   json.append(Json::Value(s1));
   json.append(Json::Value(s2));
   json.append(Json::Value(s3));
   return json;
}

HandleResponse
Inst::generateResponse(const Handle& hdl, Json::Value json) {
   return HandleResponse(hdl, stringify(json));
}

//
//
//
Json::Value
Inst::get_score_json() {
   Json::Value respJson = jsonify("SCORE");
   
   for (const auto& pl : players) {
      respJson.append(Json::Value(pl->name));
      respJson.append(jsonify(pl->score));
   }

   return respJson;
}

Json::Value
Inst::get_player_turn_message() {
   return jsonify("TURN", players[turnIndex]->name,
                  players[turnIndex]->turnkey);
}

Json::Value
Inst::get_others_turn_message() {
   return jsonify("TURN", players[turnIndex]->name);
}

//
// Game event handling
//
void
Inst::next_turn() {
   turnIndex = (turnIndex + 1) % maxPlayers;
   for (auto const& pl : players) {
      pl->state = await_turn;
   }
   players[turnIndex]->turnkey = randomString();
   players[turnIndex]->state = turn;
}

void
Inst::start_game(HandleResponseList &hrl) {
   // pick who will start the game
   std::cout << __PRETTY_FUNCTION__ << " first turn=" << turnIndex << std::endl;

   std::cout << tiles.size() << std::endl;

   // issue tiles
   for (unsigned int plIdx=0; plIdx < maxPlayers; ++plIdx) {
      issue_tiles(plIdx, hrl);
   }

   next_turn();
   broadcast_turn_messages(hrl);
}

std::string
Inst::issue_tiles(unsigned int plIdx, HandleResponseList& hrl) {
   std::string racktiles = "";
   unsigned int tilesNeeded = 7 - players[plIdx]->hand.size();

   for (unsigned int i=0; i<tilesNeeded && tiles.size(); ++i) {
      char letter = tiles.back();
      racktiles.push_back(letter);
      players[plIdx]->hand.push_back(letter);
      tiles.pop_back();
   }

   for (auto const& hdlClient : players[plIdx]->clients) {
      hrl.push_back(
         generateResponse(hdlClient.first,
                          jsonify("RACKTILES", racktiles)));
   }

   return racktiles;
}

void
Inst::dump_game_state(const Handle& hdl, HandleResponseList &hrl) {
   // Show connected players + score (game may not
   // have started)
   if (players.size() > 0) {
      hrl.push_back(generateResponse(hdl, get_score_json()));
   }

   if (gameOver) {
      return;
   }

   // If we have enough players, game has started
   if (players.size() == maxPlayers) {
      ClientMode cm = handleMode[hdl];
      // If connected as a player, share tiles
      if (cm >= 0) {
         hrl.push_back(generateResponse(hdl,
                  jsonify("RACKTILES", players[cm]->hand)));
      }

      // Send whose turn it is
      Json::Value msg = (cm == (int)turnIndex) ? get_player_turn_message() : get_others_turn_message();
      hrl.push_back(generateResponse(hdl, msg));
   }
}



//
// Message processing
//
void
Inst::broadcast_json_message(HandleResponseList &hrl, const Json::Value& json) {
   for (const auto& pl : players) {
      for (const auto& hdlClient : pl->clients) {
         hrl.push_back(generateResponse(hdlClient.first, json));
      }
   }
   for (const auto& hdlClient : viewers) {
      hrl.push_back(generateResponse(hdlClient.first, json));
   }
}

void
Inst::broadcast_json_message(HandleResponseList &hrl, const Json::Value& playerJson, const Json::Value& othersJson) {
   unsigned int plIdx = 0;
   for (const auto& pl : players) {
      for (const auto& hdlClient : pl->clients) {
         hrl.push_back(generateResponse(
               hdlClient.first,
               (plIdx==turnIndex)?playerJson:othersJson));
      }
      ++plIdx;
   }
   for (const auto& hdlClient : viewers) {
      hrl.push_back(generateResponse(hdlClient.first, othersJson));
   }
}

void
Inst::broadcast_score_messages(HandleResponseList &hrl) {
   broadcast_json_message(hrl, get_score_json());
}

void
Inst::broadcast_turn_messages(HandleResponseList &hrl) {
   broadcast_json_message(hrl, get_player_turn_message(), get_others_turn_message());
}

HandleResponseList
Inst::process_cmd_view(const Handle& hdl, const Json::Value &json) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   HandleResponseList hrl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 1) {
      std::cerr << "message error: command should be 1 token long" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "message format")));
      return hrl;
   }

   if (handleMode[hdl] == ModeIdle) {
      // client hadn't introduced itself. Move it to viewer
      viewers[hdl] = new Client(hdl, viewer);
      handleMode[hdl] = ModeViewer;

      hrl.push_back(generateResponse(hdl, jsonify("VIEW-OKAY")));
      dump_game_state(hdl, hrl);
      return hrl;
   } else {
      std::cerr << "client already connected and is in mode " << handleMode[hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "already connected")));
      return hrl;
   }

   return hrl;
}

HandleResponseList
Inst::process_cmd_join(const Handle& hdl, const Json::Value &json) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   HandleResponseList hrl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 3 || !json[1].isString() || !json[2].isString()) {
      std::cerr << "message error: not 3 tokens or input doesn't have strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "message format")));
      return hrl;
   }

   if (handleMode[hdl] == ModeIdle) {
      // client hadn't introduced itself. Allow it to connect

      // Check if username matches any of the existing users. If so,
      // try to join as that player
      for (unsigned int i=0; i<players.size(); ++i) {
         auto pl = players[i];
         if (pl->name == json[1].asString()) {
            if (pl->passwd == json[2].asString()) {
               handleMode[hdl] = i;
               pl->maybeAddClient(new Client(hdl, pl->state));

               hrl.push_back(generateResponse(hdl, jsonify("JOIN-OKAY")));
               dump_game_state(hdl, hrl);
               return hrl;
            } else {
               // bad password
               hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "bad password")));
               return hrl;
            }
         }
      }

      // Looks like we didn't match any existing player. If we
      // hit max players, stop here
      if (players.size() == maxPlayers) {
         hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "max players reached")));
         return hrl;
      }

      // Join as a new player
      int plIdx = players.size();
      handleMode[hdl] = plIdx;
      players.push_back(new Player(json[1].asString(), json[2].asString(), wait_for_more_players));
      players[plIdx]->maybeAddClient(new Client(hdl, wait_for_more_players));
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-OKAY")));

      // announce to the world that a new player joined
      broadcast_score_messages(hrl);

      std::cout << "here" << std::endl;
      if (players.size() == maxPlayers) {
         // start the game
         start_game(hrl);
      }

      return hrl;

   } else {
      // already a viewer or a player
      std::cerr << "client already connected. ignoring message as " << handleMode[hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "already joined")));
   }

   return hrl;
}

HandleResponseList
Inst::process_cmd_pass(const Handle& hdl, const Json::Value &json) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   HandleResponseList hrl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 2 || !json[1].isString()) {
      std::cerr << "message error: not 2 tokens or input doesn't have strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "message format")));
      return hrl;
   }

   if (handleMode[hdl] != (int)turnIndex) {
      std::cerr << "out of turn: turnIndex = " << turnIndex << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "out of turn")));
      return hrl;
   }

   if (json[1].asString() != players[turnIndex]->turnkey) {
      std::cerr << "out of turn: turnkey mismatch" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "bad turnkey")));
      return hrl;
   }

   // Player turnIndex wants to pass
   hrl.push_back(generateResponse(hdl, jsonify("PASS-OKAY")));
   next_turn(); 
   broadcast_turn_messages(hrl);
   return hrl;
}

HandleResponseList
Inst::process_cmd(const Handle& hdl, const Json::Value &json) {
   auto cmd = json[0].asString();

   if (cmd == "VIEW") {
      return process_cmd_view(hdl, json);
   } else if (cmd == "JOIN") {
      return process_cmd_join(hdl, json);
   } else if (cmd == "PASS") {
      return process_cmd_pass(hdl, json);
   }

   return HandleResponseList();
}

Inst::Inst(unsigned int _gid) :
      gid(_gid),
      jsonReader(new Json::Reader()),
      jsonWriter(new Json::FastWriter()) {
   std::mt19937 rng;
   rng.seed(std::random_device()());

   tiles = "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFGGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ  ";
   assert(tiles.length() == 100);

   std::uniform_int_distribution<std::mt19937::result_type> dist6(0, maxPlayers-1);
   turnIndex = dist6(rng);
   std::cout << "turnIndex = " << turnIndex << std::endl;
   srand(time(NULL));
   random_shuffle(tiles.begin(), tiles.end());

   gameOver = false;
}

void
Inst::handle_appear(const Handle& hdl) {
   std::cerr << __PRETTY_FUNCTION__ << std::endl;

   handleMode[hdl] = ModeIdle;
   std::cout << "handleMode.size() ==" << handleMode.size() << std::endl;
}

void
Inst::handle_disappear(const Handle& hdl) {
   ClientMode mode = handleMode[hdl];
   std::cerr << __PRETTY_FUNCTION__ << ", mode=" << mode << std::endl;


   if (mode == ModeIdle) {
      // nothing more to do
   } else if (mode == ModeViewer) {
      auto hdlClient = viewers.find(hdl);
      if (hdlClient != viewers.end()) {
         std::cerr << __PRETTY_FUNCTION__ << "Found viewer client and deleted it" << std::endl;
         delete hdlClient->second;
         viewers.erase(hdl);
      }
   } else if (mode < (int)players.size()) {
      // client is connected as a player
      for (const auto& pl : players) {
         pl->maybeRemoveClient(hdl);
      }
   } else {
      assert(0 && "internal mode says player is connected");
   }

   handleMode.erase(hdl);
}

HandleResponseList
Inst::process_msg(const Handle& hdl,
                  const server::message_ptr& msg) {
   HandleResponseList hrl;
   std::cerr << __PRETTY_FUNCTION__  << msg->get_payload() << std::endl;

   Json::Value json;
   bool res;
   
   res = jsonReader->parse(msg->get_payload(), json);
   if (!res) {
      std::cerr << "message error: couldn't parse: " << msg->get_payload() << std::endl;
      return hrl;
   }

   if (json.empty()) {
      std::cerr << "message error: empty json" << std::endl;
      return hrl;
   }

   if (!json.isArray()) {
      std::cerr << "message error: input not an array" << std::endl;
      return hrl;
   }

   if (json.size() == 0) {
      std::cerr << "message error: array empty" << std::endl;
      return hrl;
   }

   auto cmdJson = json[0];

   if (!cmdJson.isString()) {
      std::cerr << "message error: CMD not a string" << std::endl;
      return hrl;
   }

   return process_cmd(hdl, json);
}
