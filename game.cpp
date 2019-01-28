#include <algorithm>
#include <ctime>
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
   {1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1},
   {1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1},
   {1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1},
   {1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1},
   {3, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 3},
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

   unsigned int size_before = clients.size();
   clients.erase(hdl);
   if (size_before != clients.size()) {
      std::cerr << __PRETTY_FUNCTION__  << " player " << name
                << " number of clients before " << size_before
                << " and after " << clients.size() << std::endl;
   }
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
   respJson.append(Json::Value((unsigned int)tiles.size()));

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
Inst::touches_old_tile(int r, int c) {
   if (r-1 >= 0 && boardRC[r-1][c] != ' ') { return true; }
   if (r+1 < 15 && boardRC[r+1][c] != ' ') { return true; }
   if (c-1 >= 0 && boardRC[r][c-1] != ' ') { return true; }
   if (c+1 < 15 && boardRC[r][c+1] != ' ') { return true; }

   return false;
}

//
// Game event handling
//
void
Inst::shuffle_bag() {
   random_shuffle(tiles.begin(), tiles.end());
}

bool
Inst::is_new_tile(unsigned int r, unsigned int c) const {
   return (boardRC[r][c] == ' ' && tempBoardRC[r][c] != ' ');
}

void
Inst::next_turn(HandleResponseList &hrl, bool pass) {
   if (pass) {
      ++passesMade;
      storage->update_game_passes_made(gid, passesMade);
   } else {
      if (passesMade != 0) {
         passesMade = 0;
         storage->update_game_passes_made(gid, passesMade);
      }
   }

   // Check if the number of passes equals N + 1, i.e.,
   // the same player as has passed twice or if the
   // current player finished playing all the tiles
   if (players[turnIndex]->hand == "" ||
       passesMade == maxPlayers + 1) {

      bool exhaustedTiles = players[turnIndex]->hand == "";

      gameOver = true;
      storage->update_game_over(gid);
      for (const auto& pl : players) {
         pl->state = game_over;
         storage->update_player_state(pl->pid, pl->state);
      }

      if (exhaustedTiles) {
         std::cout << players[turnIndex]->name << " is out of tiles" << std::endl;
      } else {
         std::cout << players[turnIndex]->name << " has passed twice" << std::endl;
      }

      // Subtracts points from all other players' hand
      // and add it to us
      int accumulated_score = 0;
      for (const auto& pl : players) {
         int this_player_score = 0;
         for (const auto& letter : pl->hand) {
            this_player_score += letterScore[letter]; 
         }

         std::cout << "Player " << pl->name
                   << " is left with '" << pl->hand << "'. Subtracting "
                   << this_player_score << std::endl;
         pl->score -= this_player_score;

         storage->update_player_end_adj_score(pl->pid, this_player_score);
         storage->update_player_score(pl->pid, pl->score);

         accumulated_score += this_player_score;
      }
      if (exhaustedTiles) {
         players[turnIndex]->score += accumulated_score;
         storage->update_player_end_adj_score(players[turnIndex]->pid, accumulated_score);
         storage->update_player_score(players[turnIndex]->pid, players[turnIndex]->score);
      }

      broadcast_score_messages(hrl);
      broadcast_json_message(hrl, jsonify("GAME-OVER"));
      return;
   }


   turnIndex = (turnIndex + 1) % maxPlayers;
   storage->update_game_turn_index(gid, turnIndex);

   for (auto const& pl : players) {
      if (pl->state != await_turn) {
         pl->state = await_turn;
         storage->update_player_state(pl->pid, pl->state);
      }
   }
   players[turnIndex]->turnkey = randomString();
   storage->update_player_turnkey(players[turnIndex]->pid, players[turnIndex]->turnkey);

   players[turnIndex]->state = turn;
   storage->update_player_state(players[turnIndex]->pid, turn);

   broadcast_turn_messages(hrl);
}

std::string
Inst::play_word_score(HandleResponseList& hrl,
                      const Handle& hdl,
                      const PlayMove &pm,
                      bool wordAlongRow,
                      int *score) {
   std::string word = "";
   *score = 0;

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
      return "";
   }

   std::string uppercaseWord = word;
   for (unsigned int i=0; i<word.size(); ++i) {
      uppercaseWord[i] = toupper(word[i]);
   }

   if (!wl->is_valid(uppercaseWord)) {
      // word not in the dictionary
      std::cerr << __FUNCTION__ << " word isn't in the word list" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "invalid word '" + uppercaseWord + "'")));
      *score = -1;
      return "";
   }

   // it's a valid word. Calculate the score for it
   unsigned int wordMult = 1;
   for (unsigned int r=minR; r <= (unsigned int) maxR; ++r) {
      for (unsigned int c=minC; c <= (unsigned int) maxC; ++c) {
         char letter = tempBoardRC[r][c];
         bool is_new = is_new_tile(r, c);

         *score += letterScore[letter] * (is_new ? letterMultiplier[r][c] : 1);
         wordMult *= is_new ? wordMultiplier[r][c] : 1;
      }
   }

   *score *= wordMult;

   std::cout << __FUNCTION__ << " word=" << word << " score=" << *score << std::endl;
   return word;
}

bool
Inst::play_score(HandleResponseList& hrl,
                 const Handle& hdl,
                 const std::vector<PlayMove>& play,
                 const std::string &strJsonMove) {
   // Verify each character played in the move were in the hand
   std::string tilesplayed;
   for (const auto& pm : play) {
      if (pm.letter == ' ') {
         std::cerr << __FUNCTION__ << " blank letter can't be assigned" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "unassigned blank letter")));
         return false;
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
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "non-alphabet letter")));
         return false;
      }

      if (idx == std::string::npos) {
         std::cerr << __FUNCTION__ << " unsing letter not in hand" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "using letter not in hand")));
         return false;
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
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "improper placement of tile")));
         return false;
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
            hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "two tiles played on the same spot")));
            return false;
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
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "played tiles must be in the same row or column")));
         return false;
      }
   }

   // if this is the first move, one of the tiles must
   // be at the center of the board
   if (wordsMade == 0) {
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
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "first turn must cover the center")));
         return false;
      }

//      if (play.size() == 1) {
//         // first word can't be a single tile
//         std::cerr << __FUNCTION__ << " first turn must play two tiles" << std::endl;
//         return -1;
//      }
   }
  
   // check that new tiles touch at least an old tile
   if (wordsMade > 0) {
      bool connects = false;

      for (const auto& pm : play) {
         if (touches_old_tile(pm.row, pm.col)) {
            connects = true;
            break;
         }
      }

      if (!connects) {
         std::cerr << __FUNCTION__ << " new word doesn't touch old tiles" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "played word must touch existing tiles")));
         return false;
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
               hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "improper placement of tiles")));
               return false;
            }
         }
      }

      if (samecol) {
         for (unsigned int row=minRow; row<=maxRow; ++row) {
            if (tempBoardRC[row][minCol] == ' ') {
               std::cerr << __FUNCTION__ << " there is a gap between the tiles played" << std::endl;
               hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "improper placement of tiles")));
               return false;
            }
         }
      }
   }

   // calculate score while checking the words are valid
   bool first = true;
   unsigned int total_score = 0;
   std::string word = "";
   std::string longest_word = "";

   for (const auto& pm : play) {
      int word_score;

      word = play_word_score(hrl, hdl, pm, !samerow, &word_score);
      if (word_score < 0) {
         // invalid word
         return false;
      }
      if (word.length() > longest_word.length()) {
         longest_word = word;
      }
      total_score += word_score;

      if (first) {
         word = play_word_score(hrl, hdl, pm, samerow, &word_score);
         if (word_score < 0) {
            // invalid word
            return false;
         }
         if (word.length() > longest_word.length()) {
            longest_word = word;
         }
         total_score += word_score;

         first = false;
      }
   }

   if (play.size() == 7) {
      // We have a bingo at our hand!
      total_score += 50;
   }

   // Update Moves
   storage->add_player_move_play(gid, players[turnIndex]->pid,
         strJsonMove, longest_word, total_score);

   for (const auto& pm : play) {
      storage->add_board_tile(gid, pm.row, pm.col,
                              tempBoardRC[pm.row][pm.col],
                              tempBoardScoreRC[pm.row][pm.col]);
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

   std::string letters_played = "";
   for (auto& pm : play) {
      letters_played.push_back(pm.letter);
   }
   broadcast_json_message(hrl,
                          jsonify("PLAY-OKAY", letters_played,
                                  longest_word, total_score));
   players[turnIndex]->score += total_score;
   storage->update_player_score(players[turnIndex]->pid,
                                players[turnIndex]->score);

   return total_score;
}

void
Inst::start_game(HandleResponseList &hrl) {
   // pick who will start the game
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // issue tiles
   for (unsigned int plIdx=0; plIdx < maxPlayers; ++plIdx) {
      issue_tiles(plIdx, hrl);
   }

   next_turn(hrl, false);
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

   storage->update_player_hand(players[plIdx]->pid, players[plIdx]->hand);
   storage->update_game_tiles(gid, tiles);

   for (auto const& hdlClient : players[plIdx]->clients) {
      hrl.push_back(
         generateResponse(hdlClient.first,
                          jsonify("RACKTILES", racktiles)));
   }

   return racktiles;
}

bool
Inst::exchange_tiles(std::string _removed,
                     const Handle& hdl,
                     HandleResponseList& hrl) {
   std::string removed = _removed;
   for (unsigned int i=0; i<removed.length(); ++i) {
      if (islower(removed[i])) {
         removed[i] = ' ';
      }
   }

   // build what the hand would look like with the tiles removed
   std::string handtiles = players[turnIndex]->hand;
   for (auto const& letter : removed) {
      size_t idx = handtiles.find(letter);

      if (idx == std::string::npos) {
         // playing a character that's not in the hand
         std::cerr << __FUNCTION__ << " playing a tile that's not in player's hand" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "playing a tile not in hand")));
         return false;
      }

      handtiles.erase(idx, 1);
   }

   // remove letters from hand
   players[turnIndex]->hand = handtiles;
   storage->add_player_move_exch(gid, players[turnIndex]->pid, removed);

   broadcast_json_message(hrl, jsonify("EXCH-OKAY", _removed),
                          jsonify("EXCH-OKAY"));

   // issue new tiles
   issue_tiles(turnIndex, hrl);

   // add removed tiles to the bag
   tiles += removed;
   shuffle_bag();
   storage->update_game_tiles(gid, tiles);

   return true;
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

bool
Inst::process_cmd_view(const Handle& hdl, const Json::Value &json,
                       HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // parse message size etc
   if (json.size() != 1) {
      std::cerr << "message error: command should be 1 token long" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "message format")));
      return false;
   }

   if (handleMode[hdl] == ModeIdle) {
      // client hadn't introduced itself. Move it to viewer
      viewers[hdl] = new Client(hdl, viewer);
      handleMode[hdl] = ModeViewer;

      hrl.push_back(generateResponse(hdl, jsonify("VIEW-OKAY")));
      dump_game_state(hdl, hrl);
      return false;
   } else {
      std::cerr << "client already connected and is in mode " << handleMode[hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("VIEW-BAD", "already connected")));
      return false;
   }

   return false;
}

bool
Inst::process_cmd_chat(const Handle& hdl, const Json::Value &json,
                       HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // parse message size etc
   if (json.size() != 2 || !json[1].isString()) {
      std::cerr << "message error: command didn't have a message or doesn't have string message" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("CHAT-BAD", "message format or message")));
      return false;
   }

   if (handleMode[hdl] == ModeIdle) {
      // client hadn't introduced itself. Move it to viewer
      std::cerr << "Client hasn't introduced itself as a viewer/player yet" << std::endl;
      return false;
   }

   Json::Value chatmsg = jsonify("CHAT-MSG");

   std::time_t now = std::time(nullptr);
   std::time_t localtime = std::mktime(std::localtime(&now));

   std::string name = "Viewer";
   if (handleMode[hdl] >= 0) {
      name = players[handleMode[hdl]]->name;
   }

   chatmsg.append(Json::Value((unsigned int) localtime));
   chatmsg.append(Json::Value(name));
   chatmsg.append(Json::Value(json[1].asString()));

   broadcast_json_message(hrl, chatmsg);

   return false;
}

bool
Inst::process_cmd_join(const Handle& hdl, const Json::Value &json,
                       HandleResponseList &hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   Json::Value respJson;

   if (gameOver) {
      std::cerr << "game over already" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "game over")));
      return false;
   }

   // parse message size etc
   if (json.size() != 3 || !json[1].isString() || !json[2].isString()) {
      std::cerr << "message error: not 3 tokens or input doesn't have strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "message format")));
      return false;
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
               return false;
            } else {
               // bad password
               hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "bad password")));
               return false;
            }
         }
      }

      // Looks like we didn't match any existing player. If we
      // hit max players, stop here
      if (players.size() == maxPlayers) {
         hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "max players reached")));
         return false;
      }

      // Join as a new player
      int plIdx = players.size();
      handleMode[hdl] = plIdx;

      unsigned int pid = storage->add_player(gid, plIdx,
            json[1].asString(), json[2].asString(),
            wait_for_more_players);
      std::cout << "Storage player id = " << pid << std::endl;

      players.push_back(new Player(pid, json[1].asString(), json[2].asString(),
                                   wait_for_more_players));
      players[plIdx]->maybeAddClient(new Client(hdl, wait_for_more_players));
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-OKAY")));


      // announce to the world that a new player joined
      broadcast_score_messages(hrl);

      if (players.size() == maxPlayers) {
         // start the game
         start_game(hrl);
      }

      return true;

   } else {
      // already a viewer or a player
      std::cerr << "client already connected. ignoring message as " << handleMode[hdl] << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("JOIN-BAD", "already joined")));
   }

   return false;
}

bool
Inst::process_cmd_pass(const Handle& hdl, const Json::Value& json,
                       HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;
   Json::Value respJson;

   // parse message size etc
   if (json.size() != 2 || !json[1].isString()) {
      std::cerr << "message error: not 2 tokens or input doesn't have strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "message format")));
      return false;
   }

   if (handleMode[hdl] != (int)turnIndex) {
      std::cerr << "out of turn: turnIndex = " << turnIndex << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "out of turn")));
      return false;
   }

   if (json[1].asString() != players[turnIndex]->turnkey) {
      std::cerr << "out of turn: turnkey mismatch" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PASS-BAD", "bad turnkey")));
      return false;
   }

   storage->add_player_move_pass(gid, players[turnIndex]->pid);

   // Player turnIndex wants to pass
   broadcast_json_message(hrl, jsonify("PASS-OKAY"));

   next_turn(hrl, true); 
   return false;
}

bool
Inst::process_cmd_exch(const Handle& hdl, const Json::Value& cmdJson,
                       HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // parse message for correctness
   if (cmdJson.size() != 3 || !cmdJson[1].isString() || !cmdJson[2].isString()) {
      std::cerr << "message error: length or all arguments being strings" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "message format")));
      return false;
   }

   if (cmdJson[2].asString().length() == 0 || cmdJson[2].asString().length() > 7) {
      std::cerr << "message error: incorrect number of letters" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "incorrect number of letters")));
      return false;
   }

   if (handleMode[hdl] != (int)turnIndex) {
      std::cerr << "out of turn: turnIndex = " << turnIndex << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "out of turn")));
      return false;
   }

   if (cmdJson[1].asString() != players[turnIndex]->turnkey) {
      std::cerr << "out of turn: turnkey mismatch" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "bad turnkey")));
      return false;
   }

   if (tiles.length() < 7) {
      std::cerr << "too few letters in bag to exchange tiles. count=" << tiles.length() << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("EXCH-BAD", "too few tiles")));
      return false;
   }

   // TODO check that provided letters are a-zA-Z or a blank tile

   // TODO move hdl check from exchange_tiles to here

   // All checks passed. Go ahead and replace the tiles
   bool success = exchange_tiles(cmdJson[2].asString(), hdl, hrl);
   if (success) {
      next_turn(hrl, false);
   }

   return false;
}

bool
Inst::process_cmd_play(const Handle& hdl, const Json::Value& cmdJson,
                       HandleResponseList& hrl) {
   std::cout << __PRETTY_FUNCTION__ << std::endl;

   // parse message size etc
   if (cmdJson.size() < 2 || cmdJson.size() > 9) {
      std::cerr << "message error: incorrect number of tiles played" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "incorrect number of tiles played")));
      return false;
   }

   for (unsigned int i=2; i<cmdJson.size(); ++i) {
      if (!cmdJson[i].isArray() || cmdJson[i].size()!=3 ||
          !cmdJson[i][0].isString() || !cmdJson[i][1].isUInt() || !cmdJson[i][2].isUInt()) {
         std::cerr << "message error: incorrect message format" << std::endl;
         hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "message format")));
         return false;
      }
   }

   if (handleMode[hdl] != (int)turnIndex) {
      std::cerr << "out of turn: turnIndex = " << turnIndex << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "out of turn")));
      return false;
   }

   if (cmdJson[1].asString() != players[turnIndex]->turnkey) {
      std::cerr << "out of turn: turnkey mismatch" << std::endl;
      hrl.push_back(generateResponse(hdl, jsonify("PLAY-BAD", "bad turnkey")));
      return false;
   }

   // TODO check that provided letters are a-zA-Z

   std::vector<PlayMove> play;
   for (unsigned int i=2; i<cmdJson.size(); ++i) {
      PlayMove pm = { cmdJson[i][0].asString()[0],
                      cmdJson[i][1].asUInt(),
                      cmdJson[i][2].asUInt() };
      play.push_back(pm);
   }

   bool success = play_score(hrl, hdl, play, stringify(cmdJson));
   if (!success) {
      return false;
   }

   ++wordsMade;
   storage->update_game_words_made(gid, wordsMade);

   // notify everyone that new tiles are appearing on the board
   Json::Value boardtiles = jsonify("BOARDTILES");
   for (const auto& pm : play) {
      boardtiles.append(jsonify(pm.letter, pm.row, pm.col));
   }
   broadcast_json_message(hrl, boardtiles);

   // issue new tiles to the player
   issue_tiles(turnIndex, hrl);

   // advertise score to everyone
   broadcast_score_messages(hrl);

   next_turn(hrl, false); 
   return true;
}

bool
Inst::process_cmd(const Handle& hdl, const Json::Value &json,
                  HandleResponseList& hrl) {
   auto cmd = json[0].asString();

   if (cmd == "VIEW") {
      return process_cmd_view(hdl, json, hrl);
   } else if (cmd == "CHAT-SEND") {
      return process_cmd_chat(hdl, json, hrl);
   } else if (cmd == "JOIN") {
      return process_cmd_join(hdl, json, hrl);
   } else if (gameOver) {
      std::cout << "Game over. Can't process " << cmd << std::endl;
      return false;
   } else if (cmd == "PASS") {
      return process_cmd_pass(hdl, json, hrl);
   } else if (cmd == "EXCH") {
      return process_cmd_exch(hdl, json, hrl);
   } else if (cmd == "PLAY") {
      return process_cmd_play(hdl, json, hrl);
   }

   return false;
}

Inst::Inst(unsigned int _gid, const WordList *_wl,
           Storage::Inst *_storage,
           const unsigned int _maxPlayers,
           Json::Reader *_jsonReader,
           Json::FastWriter *_jsonWriter) :
      gid(_gid),
      wl(_wl),
      storage(_storage),
      maxPlayers(_maxPlayers),
      gameOver(false),
      wordsMade(0),
      passesMade(0),
      jsonReader(_jsonReader),
      jsonWriter(_jsonWriter) {
   srand(time(NULL));

   tiles = "AAAAAAAAABBCCDDDDEEEEEEEEEEEEFFGGGHHIIIIIIIIIJKLLLLMMNNNNNNOOOOOOOOPPQRRRRRRSSSSTTTTTTUUUUVVWWXYYZ  ";
   assert(tiles.length() == 100);
   shuffle_bag();

   std::mt19937 rng;
   rng.seed(std::random_device()());
   std::uniform_int_distribution<std::mt19937::result_type> dist6(0, maxPlayers-1);
   turnIndex = dist6(rng);

   storage->add_game(gid, maxPlayers, turnIndex, tiles);

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

bool
Inst::process_msg(const Handle& hdl,
                  const server::message_ptr& msg,
                  HandleResponseList& hrl) {
   std::cerr << __PRETTY_FUNCTION__  << msg->get_payload() << std::endl;

   Json::Value json;
   bool res;
   
   res = jsonReader->parse(msg->get_payload(), json);
   if (!res) {
      std::cerr << "message error: couldn't parse: " << msg->get_payload() << std::endl;
      return false;
   }

   if (json.empty()) {
      std::cerr << "message error: empty json" << std::endl;
      return false;
   }

   if (!json.isArray()) {
      std::cerr << "message error: input not an array" << std::endl;
      return false;
   }

   if (json.size() == 0) {
      std::cerr << "message error: array empty" << std::endl;
      return false;
   }

   auto cmdJson = json[0];

   if (!cmdJson.isString()) {
      std::cerr << "message error: CMD not a string" << std::endl;
      return false;
   }

   return process_cmd(hdl, json, hrl);
}

Json::Value
Inst::status() {
   Json::Value json;
   json.append(Json::Value("GAME"));
   json.append(Json::Value(gid));
   json.append(Json::Value(gameOver));
   json.append(Json::Value(maxPlayers));
   if (maxPlayers == players.size()) {
      json.append(Json::Value(turnIndex));
   } else {
      json.append(Json::Value(-1));
   }
   json.append(Json::Value((unsigned int)tiles.size()));

   for (const auto& pl : players) {
      Json::Value plsc;
      plsc.append(Json::Value(pl->name));
      plsc.append(pl->score);
      json.append(plsc);
   }

   return json;
}
