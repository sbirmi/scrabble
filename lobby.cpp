#include <iostream>

#include "lobby.hpp"

namespace Lobby {

void
Inst::loadFromStorage() {

   StorageGameList gameList = storage->game_list();
   for (auto const& game : gameList) {
      std::cout << "Loading gid " << game.gid << std::endl;

      Game::Inst *gi = new Game::Inst(
            game.gid, wl, storage,
            game.maxPlayers,
            game.gameOver,
            game.wordsMade, game.passesMade,
            jsonReader, jsonWriter,
            game.turnIndex, game.tiles);
      gameById[game.gid] = gi;
      nextGameId = std::max(game.gid + 1, nextGameId);

      gi->loadBoardTiles(storage->game_board_tile_list(game.gid));
      gi->loadPlayers(storage->game_player_list(game.gid));
      gi->loadMoves(storage->game_move_list(game.gid));
   }
}

std::string
Inst::stringify(const Json::Value& json) {
   return jsonWriter->write(json);
}

HandleResponse
Inst::generateResponse(const Handle& hdl, Json::Value json) {
   return HandleResponse(hdl, stringify(json));
}

void
Inst::broadcast_json_message(HandleResponseList &hrl, const Json::Value& json) {
   for (const auto& hdl : lobbyHandles) {
      hrl.push_back(generateResponse(hdl, json));
   }
}

void
Inst::handle_appear(const Handle &hdl, const std::string resource) {
   std::cout << "resource = " << resource << std::endl;

   if (resource == "/lobby") {
      lobbyHandles.insert(hdl);
      return;
   } else if (resource.substr(0, 6) == "/game/" && resource.size() > 6) {
      std::string gidstr = resource.substr(6, resource.size() - 6);
      int gid = std::stoi(gidstr);

      if (gameById.find(gid) != gameById.end()) {
         gameIdByHdl[ hdl ] = std::stoi(gidstr);

         auto game = gameById.at(gid);
         game->handle_appear(hdl);

         return;
      }

      // trying to access an invalid game id. We'll ignore
      // this connection. fall through and do nothing.
   }

   std::cerr << "Can't parse resource '" << resource
             << "' or game doesn't exist. Ignoring everything on this connection" << std::endl;
}

void
Inst::handle_disappear(const Handle &hdl) {
   if (lobbyHandles.find(hdl) != lobbyHandles.end()) {
      std::cout << "Disconnecting handle from lobby" << std::endl;
      lobbyHandles.erase(hdl);
      return;
   }

   if (gameIdByHdl.find(hdl) == gameIdByHdl.end()) {
      std::cout << "dropping handle (was benig ignored)" << std::endl;
      return;
   }

   auto gid = gameIdByHdl.at(hdl);
   gameIdByHdl.erase(hdl);

   auto game = gameById.at(gid);
   game->handle_disappear(hdl);
}

void
Inst::process_lobby_cmd_list(const Handle& hdl, const Json::Value& json, HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   for (auto it=gameById.begin(); it != gameById.end(); ++it) {
      Json::Value game_status = it->second->status();
      broadcast_json_message(hrl, game_status);
   }
}

void
Inst::process_lobby_cmd_host(const Handle& hdl, const Json::Value& json, HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // parse message size etc
   if (json.size() != 2 || !json[1].isInt()) {
      std::cerr << "message error: command should be 2 token long" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("HOST-BAD", "message format")));
      return;
   }

   unsigned int max_players = json[1].asInt();

   if (max_players == 0 || max_players > 4) {
      std::cerr << "message error: invalid max_players" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("HOST-BAD", "invalid max-players")));
      return;
   }

   GameId gid = nextGameId++;
   std::cout << "Hosting gid=" << gid << " for max players " << max_players << std::endl;

   Game::Inst *gi = new Game::Inst(gid, wl, storage, max_players, jsonReader, jsonWriter);
   gameById[gid] = gi;

   // notify all clients that the game is hosted
   Json::Value game_status = gi->status();
   broadcast_json_message(hrl, game_status);
}

void
Inst::process_lobby_cmd(const Handle& hdl, const Json::Value& json, HandleResponseList &hrl)
{
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   auto cmd = json[0].asString();

   if (cmd == "LIST") {
      process_lobby_cmd_list(hdl, json, hrl);
      return;
   } else if (cmd == "HOST") {
      process_lobby_cmd_host(hdl, json, hrl);
      return;
   }

   std::cerr << "Unknown command " << cmd << std::endl;
}

HandleResponseList
Inst::process_lobby_msg(const Handle& hdl, const std::string msg) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   HandleResponseList hrl;

   Json::Value json;
   
   bool res = jsonReader->parse(msg, json);
   if (!res) {
      std::cerr << "message error: couldn't parse: " << msg << std::endl;
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

   process_lobby_cmd(hdl, json, hrl);
   return hrl;
}

HandleResponseList
Inst::process_msg(
      const Handle& hdl,
      const server::message_ptr& msg) {
   std::cout << __PRETTY_FUNCTION__  << msg->get_payload() << std::endl;

   if (lobbyHandles.find(hdl) != lobbyHandles.end()) {
      return process_lobby_msg(hdl, msg->get_payload());
   }

   if (gameIdByHdl.find(hdl) == gameIdByHdl.end()) {
      std::cout << "handle isn't being tracked" << std::endl;
      return HandleResponseList();
   }

   HandleResponseList hrl;

   auto gid=gameIdByHdl.at(hdl);
   // game must exist as handles are only tracked if the game exists
   bool notifyLobby = gameById.at(gid)->process_msg(hdl, msg, hrl);

   if (notifyLobby) {
      Json::Value game_status = gameById.at(gid)->status();
      broadcast_json_message(hrl, game_status);
   }

   return hrl;
}

Inst::Inst(const WordList *_wl, const std::string _dbfile) :
      nextGameId(1),
      wl(_wl),
      storage(new Storage::Inst(_dbfile)),
      jsonReader(new Json::Reader()),
      jsonWriter(new Json::FastWriter()) {
   std::cout << "Lobby created" << std::endl;

   loadFromStorage();
   std::cout << "Finished loading from storage" << std::endl;
};

}
