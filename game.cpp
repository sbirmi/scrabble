#include "game.hpp"

using namespace Game;


//
// Player methods
//
bool
Player::maybeAddClient(Client *cl) {
   // Check if client is already connected. If not, add him now
   auto resp = clients.insert(cl);
   return resp.second;
}

bool
Player::maybeRemoveClient(Client *cl) {
   return clients.erase(cl);
}
bool
Player::maybeRemoveClient(const Handle *hdl) {
   for (const auto& cl : clients) {
      if (cl->hdl == hdl) {
         std::cerr << "Handle " << hdl << " removed" << std::endl;
         clients.erase(cl);
         return true;
      }
   }
   return false;
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
Inst::jsonify(std::string s1) {
   Json::Value json;
   json.append(Json::Value(s1));
   return json;
}

Json::Value
Inst::jsonify(std::string s1, std::string s2) {
   Json::Value json;
   json.append(Json::Value(s1));
   json.append(Json::Value(s2));
   return json;
}

HandleResponse
Inst::generateResponse(const Handle& hdl, Json::Value json) {
   return HandleResponse(&hdl, stringify(json));
}

//
// Game event handling
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

void
Inst::dump_game_state(const Handle& hdl, HandleResponseList &hrl) {
   if (players.size() > 0) {
      hrl.push_back(generateResponse(hdl, get_score_json()));
   }
}



//
// Message processing
//
HandleResponseList
Inst::broadcast_score_messages() {
   HandleResponseList hrl;
   Json::Value respJson = get_score_json();

   for (const auto& pl : players) {
      for (const auto& cl : pl->clients) {
         hrl.push_back(generateResponse(*cl->hdl, respJson));
      }
   }
   for (const auto& cl : viewers) {
      hrl.push_back(generateResponse(*cl->hdl, respJson));
   }

   return hrl;
}

HandleResponseList
Inst::process_cmd_view(const Handle& hdl, const Json::Value &json) {
   std::cout << __PRETTY_FUNCTION__ << " hdl=" << &hdl << std::endl;
   HandleResponseList hrl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 1) {
      std::cerr << "message error: command should be 1 token long" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "message format")));
      return hrl;
   }

   if (handleMode[&hdl] == ModeIdle) {
      // client hadn't introduced itself. Move it to viewer
      viewers.insert(new Client(&hdl, viewer));
      handleMode[&hdl] = ModeViewer;

      hrl.push_back(generateResponse(hdl, jsonify("VIEW-OKAY")));
      dump_game_state(hdl, hrl);
      return hrl;
   } else {
      std::cerr << "client already connected and is in mode " << handleMode[&hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "already connected")));
      return hrl;
   }

   return hrl;
}

HandleResponseList
Inst::process_cmd_join(const Handle& hdl, const Json::Value &json) {
   std::cout << __PRETTY_FUNCTION__ << " hdl=" << &hdl << std::endl;
   HandleResponseList hrl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 3 || !json[1].isString() || !json[2].isString()) {
      std::cerr << "message error: not 3 tokens or input doesn't have strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "message format")));
      return hrl;
   }

   if (handleMode[&hdl] == ModeIdle) {
      // client hadn't introduced itself. Allow it to connect

      // Check if username matches any of the existing users. If so,
      // try to join as that player
      for (unsigned int i=0; i<players.size(); ++i) {
         auto pl = players[i];
         if (pl->name == json[1].asString()) {
            if (pl->passwd == json[2].asString()) {
               handleMode[&hdl] = i;
               pl->maybeAddClient(new Client(&hdl, pl->state));

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
      if (players.size() == max_players) {
         hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "max players reached")));
         return hrl;
      }

      // Join as a new player
      int plIdx = players.size();
      handleMode[&hdl] = plIdx;
      players.push_back(new Player(json[1].asString(), json[2].asString(), wait_for_more_players));
      players[plIdx]->maybeAddClient(new Client(&hdl, wait_for_more_players));
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-OKAY")));

      // announce to the world that a new player joined
      for ( const auto& hr : broadcast_score_messages() ) {
         hrl.push_back(hr);
      }

      if (players.size() == max_players) {
         // start the game
      }

      return hrl;

   } else {
      // already a viewer or a player
      std::cerr << "client already connected. ignoring message as " << handleMode[&hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "already joined")));
   }

   return hrl;
}

HandleResponseList
Inst::process_cmd(const Handle& hdl, const Json::Value &json) {
   auto cmd = json[0].asString();

   if (cmd == "VIEW") {
      return process_cmd_view(hdl, json);
   } else if (cmd == "JOIN") {
      return process_cmd_join(hdl, json);
   }

   return HandleResponseList();
}

void
Inst::handle_appear(const Handle &hdl) {
   std::cerr << __PRETTY_FUNCTION__ << ": " << &hdl << std::endl;

   handleMode[&hdl] = ModeIdle;
   std::cout << "handleMode.size() ==" << handleMode.size() << std::endl;
}

void
Inst::handle_disappear(const Handle &hdl) {
   ClientMode mode = handleMode[&hdl];
   std::cerr << __PRETTY_FUNCTION__ << ": hdl=" << &hdl << ", mode=" << mode << std::endl;


   if (mode == ModeIdle) {
      // nothing more to do
   } else if (mode == ModeViewer) {
      for (const auto& cl : viewers) {
         if (cl->hdl == &hdl) {
            std::cerr << "Found viewer client and deleted it" << std::endl;
            viewers.erase(cl);
            break;
         }
      }
   } else if (mode < (int)players.size()) {
      // client is connected as a player
      for (const auto& pl : players) {
         pl->maybeRemoveClient(&hdl);
      }
   } else {
      assert(0 && "internal mode says player is connected");
   }

   handleMode.erase(&hdl);
}

HandleResponseList
Inst::process_msg(const Handle& hdl,
                  const server::message_ptr& msg) {
   HandleResponseList hrl;
   std::cerr << __PRETTY_FUNCTION__ << ": "
             << (&hdl) << ", " << msg->get_payload() << std::endl;

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
