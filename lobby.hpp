#ifndef __LOBBY_HPP
#define __LOBBY_HPP

#include <string>
#include <vector>
#include <json/value.h>

#include "json_util.hpp"
#include "conn.hpp"
#include "game.hpp"
#include "word_list.hpp"
#include "storage.hpp"

typedef std::map<GameId, Game::Inst *> GameById;
typedef std::set<Handle,std::owner_less<Handle> > HandleSet;
typedef std::map<const Handle, GameId, std::owner_less<Handle>> HandleGameId;

namespace Lobby {

class Inst {
private:
   GameId nextGameId;
   const WordList *wl;
   Storage::Inst *storage;
   Json::Reader *jsonReader;
   Json::FastWriter *jsonWriter;

   // maps gameid to game instance
   GameById gameById;
   // Handles of clients connected to the lobby
   HandleSet lobbyHandles;
   // maps handle to game
   HandleGameId gameIdByHdl;

   // Convenience methods
   std::string stringify(const Json::Value& json);
   HandleResponse generateResponse(
         const Handle& hdl, Json::Value json);
   void broadcast_json_message(HandleResponseList &hrl, const Json::Value& json);

public:
   Inst(const WordList *_wl, const std::string);
   void handle_appear(const Handle &hdl, const std::string);
   void handle_disappear(const Handle &hdl);
   HandleResponseList process_msg(
         const Handle& hdl,
         const server::message_ptr& msg);

   HandleResponseList process_lobby_msg(const Handle& hdl, const std::string);
   void process_lobby_cmd(const Handle& hdl, const Json::Value&, HandleResponseList &);
   void process_lobby_cmd_list(const Handle& hdl, const Json::Value&, HandleResponseList &);
   void process_lobby_cmd_host(const Handle& hdl, const Json::Value&, HandleResponseList &);
};

}

#endif
