#include <algorithm>
#include <map>
#include <random>

#include "game.hpp"

using namespace Game;

#define  MAX_RAND_STRING      (8)

std::map<char, unsigned int> letterScore = {
   {'A', 1}, {'B', 3}, {'C', 3}, {'D', 2}, {'E', 1}, {'F', 4}, {'G', 2},
   {'H', 4}, {'I', 1}, {'J', 8}, {'K', 5}, {'L', 1}, {'M', 3}, {'N', 1},
   {'O', 1}, {'P', 3}, {'Q', 10}, {'R', 1}, {'S', 1}, {'T', 1}, {'U', 1},
   {'V', 4}, {'W', 4}, {'X', 8}, {'Y', 4}, {'Z', 10},
   {'a', 0}, {'b', 0}, {'c', 0}, {'d', 0}, {'e', 0}, {'f', 0}, {'g', 0},
   {'h', 0}, {'i', 0}, {'j', 0}, {'k', 0}, {'l', 0}, {'m', 0}, {'n', 0},
   {'o', 0}, {'p', 0}, {'q', 0}, {'r', 0}, {'s', 0}, {'t', 0}, {'u', 0},
   {'v', 0}, {'w', 0}, {'x', 0}, {'y', 0}, {'z', 0},
};

const unsigned int
letterMultiplier[15][15] = {
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
   {1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1},
   {1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1},
   {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1},
   {1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1},
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
   {1, 1, 2, 1, 1, 1, 2, 1, 2, 1, 1, 1, 2, 1, 1},
   {1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 3, 1},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {2, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2},
   {1, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1},
   {1, 1, 1, 1, 1, 3, 1, 1, 1, 3, 1, 1, 1, 1, 1},
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
};

const unsigned int
wordMultiplier[15][15] = {
   {3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3},
   {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
   {1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1},
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
   {1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {3, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 3},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
   {1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1},
   {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
   {3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3},
};

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
Inst::jsonify(int i) {
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

Json::Value
Inst::jsonify(const char letter, unsigned int row, unsigned int col) {
   Json::Value json;
   std::string s=" ";
   s[0] = letter;
   json.append(Json::Value(s));
   json.append(Json::Value(row));
   json.append(Json::Value(col));
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

bool
Inst::touches_old_tile(unsigned int r, unsigned int c) {
   if (r-1 >= 0 && boardRC[r-1][c] != ' ') { return true; }
   if (r+1 < 15 && boardRC[r+1][c] != ' ') { return true; }
   if (c-1 >= 0 && boardRC[r][c-1] != ' ') { return true; }
   if (c+1 < 15 && boardRC[r][c+1] != ' ') { return true; }

   return false;
}

//
// Game event handling
//
bool
Inst::is_new_tile(unsigned int r, unsigned int c) const {
   return (boardRC[r][c] == ' ' && tempBoardRC[r][c] != ' ');
}

void
Inst::next_turn(HandleResponseList &hrl) {
   // Check if the current player finished playing
   // all the tiles
   if (players[turnIndex]->hand == "") {
      gameOver = true;
      for (const auto& pl : players) {
         pl->state = game_over;
      }

      std::cout << players[turnIndex]->name << " is out of tiles" << std::endl;

      // Subtracts points from all other players' hand
      // and add it to us
      for (const auto& pl : players) {
         int this_player_score = 0;
         for (const auto& letter : pl->hand) {
            this_player_score += letterScore[letter]; 
         }

         std::cout << "Player " << pl->name
                   << " is left with '" << pl->hand << "'. Subtracting "
                   << this_player_score << std::endl;
         pl->score -= this_player_score;
         players[turnIndex]->score += this_player_score;
      }
      broadcast_score_messages(hrl);
      broadcast_json_message(hrl, jsonify("GAME-OVER"));
      return;
   }


   turnIndex = (turnIndex + 1) % maxPlayers;
   for (auto const& pl : players) {
      pl->state = await_turn;
   }
   players[turnIndex]->turnkey = randomString();
   players[turnIndex]->state = turn;

   broadcast_turn_messages(hrl);
}

int
Inst::play_word_score(const PlayMove &pm,
                      bool wordAlongRow) {
   std::string word = "";
   int score = 0;

   int minC=pm.col, maxC = pm.col;
   int minR=pm.row, maxR=pm.row;
   if (wordAlongRow) {
      for (int c=pm.col; c >= 0 && tempBoardRC[pm.row][c] != ' '; --c) {
         minC = c;
      }
      for (int c=pm.col; c < 15 && tempBoardRC[pm.row][c] != ' '; ++c) {
         maxC = c;
      }

      for (int c=minC; c<=maxC; ++c) {
         word.push_back(tempBoardRC[pm.row][c]);
      }

   } else {
      for (int r=pm.row; r >= 0 && tempBoardRC[r][pm.col] != ' '; --r) {
         minR = r;
      }
      for (int r=pm.row; r < 15 && tempBoardRC[r][pm.col] != ' '; ++r) {
         maxR = r;
      }

      for (int r=minR; r<=maxR; ++r) {
         word.push_back(tempBoardRC[r][pm.col]);
      }
   }

   std::cout << __FUNCTION__
             << " " << pm.letter << " " << pm.row << "," << pm.col
             << " [" << minR << "," << maxR << "]"
             << " [" << minC << "," << maxC << "]"
             << " wordAlongRow=" << wordAlongRow
             << " word=" << word << std::endl;

   if (word.size() == 1) {
      return 0;
   }

   std::string uppercaseWord = word;
   for (unsigned int i=0; i<word.size(); ++i) {
      uppercaseWord[i] = toupper(word[i]);
   }

   if (!wl->is_valid(uppercaseWord)) {
      // word not in the dictionary
      std::cerr << __FUNCTION__ << " word isn't in the word list" << std::endl;
      return -1;
   }

   // it's a valid word. Calculate the score for it
   unsigned int wordMult = 1;
   for (unsigned int r=minR; r <= (unsigned int) maxR; ++r) {
      for (unsigned int c=minC; c <= (unsigned int) maxC; ++c) {
         char letter = tempBoardRC[r][c];
         bool is_new = is_new_tile(r, c);

         score += letterScore[letter] * (is_new ? letterMultiplier[r][c] : 1);
         wordMult *= is_new ? wordMultiplier[r][c] : 1;
      }
   }

   score *= wordMult;

   std::cout << __FUNCTION__ << " word=" << word << " score=" << score << std::endl;
   return score;
}

int
Inst::play_score(const std::vector<PlayMove>& play) {
   // Verify each character played in the move were in the hand
   std::string tilesplayed;
   for (const auto& pm : play) {
      if (pm.letter == ' ') {
         std::cerr << __FUNCTION__ << " blank letter can't be assigned" << std::endl;
         return -1;
      }
      tilesplayed.push_back(pm.letter);
   }

   std::string handtiles = players[turnIndex]->hand;
   for (auto const& letter : tilesplayed) {
      size_t idx = std::string::npos;
      if (isupper(letter)) {
         idx = handtiles.find(letter);
      } else if (islower(letter)) {
         // lower letter => blank
         idx = handtiles.find(' ');
      } else {
         // illegal character
         std::cerr << __FUNCTION__ << " non-alphabet letter played" << std::endl;
         return -1;
      }

      if (idx == std::string::npos) {
         // playing a character that's not in the hand
         std::cerr << __FUNCTION__ << " playing a tile that's not in player's hand" << std::endl;
         return -1;
      }

      handtiles[idx] = '*';
   }

   // create a temp board from the actual board and add tiles
   for (unsigned r=0; r<15; ++r) {
      for (unsigned c=0; c<15; ++c) {
         tempBoardRC[r][c] = boardRC[r][c];
         tempBoardScoreRC[r][c] = boardscoreRC[r][c];
      }
   }

   for (const auto& pm : play) {
      if (tempBoardRC[pm.row][pm.col] != ' ') {
         // can't place a tile on top of an existing tiles
         std::cerr << __FUNCTION__ << " playing a new tile on an existing tile" << std::endl;
         return -1;
      }

      tempBoardRC[pm.row][pm.col] = pm.letter;
      tempBoardScoreRC[pm.row][pm.col] = letterScore[pm.letter];
   }

   // check two tiles are not placed on the same place
   for (unsigned int i=0; i<play.size() - 1; ++i) {
      for (unsigned int j=i+1; j<play.size(); ++j) {
         if (play[i].row == play[j].row &&
             play[i].col == play[j].col) {
            // placing more than 1 tile on the same spot
            std::cerr << __FUNCTION__ << " two tiles are played on the same spot" << std::endl;
            return -1;
         }
      }
   }

   // check if tiles are in the same row or in the same column
   bool samerow = true;
   bool samecol = true;

   unsigned int minRow = play[0].row, maxRow = play[0].row;
   unsigned int minCol = play[0].col, maxCol = play[0].col;
   if (play.size() > 1) {
      unsigned firstrow = play[0].row;
      unsigned firstcol = play[0].col;

      for (unsigned int i=1; i<play.size(); ++i) {
         samerow &= (firstrow == play[i].row);
         samecol &= (firstcol == play[i].col);

         minRow = std::min(minRow, play[i].row);
         maxRow = std::max(maxRow, play[i].row);

         minCol = std::min(minCol, play[i].col);
         maxCol = std::max(maxCol, play[i].col);
      }

      if (!samerow && !samecol) {
         // played tiles are not in one row
         std::cerr << __FUNCTION__ << " played tiles are not in the same row or the same column" << std::endl;
         return -1;
      }
   }

   // if this is the first move, one of the tiles must
   // be at the center of the board
   if (movesMade == 0) {
      bool boardCenterOccupied = false;
      for (const auto& pm : play) {
         if (pm.row == 7 && pm.col == 7) {
            boardCenterOccupied = true;
            break;
         }
      }

      if (!boardCenterOccupied) {
         // first move is off-center
         std::cerr << __FUNCTION__ << " first turn must cover center spot" << std::endl;
         return -1;
      }

//      if (play.size() == 1) {
//         // first word can't be a single tile
//         std::cerr << __FUNCTION__ << " first turn must play two tiles" << std::endl;
//         return -1;
//      }
   }
  
   // check that new tiles touch at least an old tile
   if (movesMade > 0) {
      bool connects = false;

      for (const auto& pm : play) {
         if (touches_old_tile(pm.row, pm.col)) {
            connects = true;
            break;
         }
      }

      if (!connects) {
         std::cerr << __FUNCTION__ << " new word doesn't touch old tiles" << std::endl;
         return -1;
      }
   }

   if (play.size() == 1) {
      samecol = false;
   }

   // check that there are no gaps based on whether
   // it's the same row or same column
   if (play.size() > 1) {
      if (samerow) {
         for (unsigned int col=minCol; col<=maxCol; ++col) {
            if (tempBoardRC[minRow][col] == ' ') {
               std::cerr << __FUNCTION__ << " there is a gap between the tiles played" << std::endl;
               return -1;
            }
         }
      }

      if (samecol) {
         for (unsigned int row=minRow; row<=maxRow; ++row) {
            if (tempBoardRC[row][minCol] == ' ') {
               std::cerr << __FUNCTION__ << " there is a gap between the tiles played" << std::endl;
               return -1;
            }
         }
      }
   }

   // calculate score while checking the words are valid
   bool first = true;
   unsigned int total_score = 0;
   for (const auto& pm : play) {
      int word_score;

      word_score = play_word_score(pm, !samerow);
      if (word_score < 0) {
         // invalid word
         return -1;
      }
      total_score += word_score;

      if (first) {
         word_score = play_word_score(pm, samerow);
         if (word_score < 0) {
            // invalid word
            return -1;
         }
         total_score += word_score;

         first = false;
      }
   }

   if (play.size() == 7) {
      // We have a bingo at our hand!
      total_score += 50;
   }


   // the move was successful. copy tempBoardRC/tempBoardScoreRC
   // to boardRC/boardscoreRC
   for (unsigned r=0; r<15; ++r) {
      for (unsigned c=0; c<15; ++c) {
         boardRC[r][c] = tempBoardRC[r][c];
         boardscoreRC[r][c] = tempBoardScoreRC[r][c];
      }
   }

   // remove the tiles from hand
   players[turnIndex]->hand = "";
   for (const auto& c : handtiles) {
      if (c != '*')
         players[turnIndex]->hand.push_back(c);
   }

   return total_score;
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

   next_turn(hrl);
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

   // send boardtiles on the board
   Json::Value boardtiles = jsonify("BOARDTILES");
   for (unsigned int r=0; r<15; ++r) {
      for (unsigned int c=0; c<15; ++c) {
         if (boardRC[r][c] != ' ') {
            boardtiles.append(jsonify(boardRC[r][c], r, c));
         }
      }
   }
   if (boardtiles.size() > 1) {
      hrl.push_back(generateResponse(hdl, boardtiles));
   }

   // If game is over, end the game
   if (gameOver) {
      hrl.push_back(generateResponse(hdl, jsonify("GAME-OVER")));
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
   next_turn(hrl); 
   return hrl;
}

HandleResponseList
Inst::process_cmd_play(const Handle& hdl, const Json::Value &cmdJson) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   HandleResponseList hrl;

   // parse message size etc
   if (cmdJson.size() < 2 || cmdJson.size() > 9) {
      std::cerr << "message error: incorrect number of tiles played" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "incorrect number of tiles played")));
      return hrl;
   }

   for (unsigned int i=2; i<cmdJson.size(); ++i) {
      if (!cmdJson[i].isArray() || cmdJson[i].size()!=3 ||
          !cmdJson[i][0].isString() || !cmdJson[i][1].isUInt() || !cmdJson[i][2].isUInt()) {
         std::cerr << "message error: incorrect message format" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "message format")));
         return hrl;
      }
   }

   if (handleMode[hdl] != (int)turnIndex) {
      std::cerr << "out of turn: turnIndex = " << turnIndex << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "out of turn")));
      return hrl;
   }

   if (cmdJson[1].asString() != players[turnIndex]->turnkey) {
      std::cerr << "out of turn: turnkey mismatch" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "bad turnkey")));
      return hrl;
   }

   std::vector<PlayMove> play;
   std::string word_played;
   for (unsigned int i=2; i<cmdJson.size(); ++i) {
      PlayMove pm = { cmdJson[i][0].asString()[0],
                      cmdJson[i][1].asUInt(),
                      cmdJson[i][2].asUInt() };
      play.push_back(pm);
      word_played.push_back(cmdJson[i][0].asString()[0]);
   }

   int score = play_score(play);
   if (score < 0) {
      std::cerr << "out of turn: play_score() returned negative score" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "bad move")));
      return hrl;
   }

   ++movesMade;

   // Player turnIndex wants to pass
   for (const auto& hdlClient : players[turnIndex]->clients) {
      hrl.push_back(generateResponse(hdlClient.first,
                                     jsonify("PLAY-OKAY", word_played)));
   }

   // notify everyone that new tiles are appearing on the board
   Json::Value boardtiles = jsonify("BOARDTILES");
   for (const auto& pm : play) {
      boardtiles.append(jsonify(pm.letter, pm.row, pm.col));
   }
   broadcast_json_message(hrl, boardtiles);

   // TODO are all players out of tiles? If so, end the game

   // issue new tiles to the player
   issue_tiles(turnIndex, hrl);

   // advertise score to everyone
   players[turnIndex]->score += score;
   broadcast_score_messages(hrl);

   next_turn(hrl); 
   return hrl;
}

HandleResponseList
Inst::process_cmd(const Handle& hdl, const Json::Value &json) {
   auto cmd = json[0].asString();

   if (cmd == "VIEW") {
      return process_cmd_view(hdl, json);
   } else if (gameOver) {
      std::cout << "Game over. Can't process " << cmd << std::endl;
      return HandleResponseList();
   } else if (cmd == "JOIN") {
      return process_cmd_join(hdl, json);
   } else if (cmd == "PASS") {
      return process_cmd_pass(hdl, json);
   } else if (cmd == "PLAY") {
      return process_cmd_play(hdl, json);
   }

   return HandleResponseList();
}

Inst::Inst(unsigned int _gid, const WordList *_wl) :
      gid(_gid),
      wl(_wl),
      maxPlayers(2),
      gameOver(false),
      movesMade(0),
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

   for (unsigned int r=0; r<15; ++r) {
      for (unsigned int c=0; c<15; ++c) {
         boardRC[r][c] = ' ';
      }
   }
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
