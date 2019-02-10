
function processGameOverMessage(msg) {
   document.getElementById("shufbutton").disabled = true;
   document.getElementById("exchbutton").disabled = true;
   document.getElementById("passbutton").disabled = true;
   document.getElementById("playbutton").disabled = true;
}

function processExchOkayMessage(msg) {
   if (myName != currTurnName) {
      chat_game_message(currTurnName + " exchanged letters");
      return;
   } else {
      chat_game_message("Letters exchanged");
   }
   // move all the tiles back to the rack
   click_shuf_tiles();

   // Remove the letter from rack if present there.
   // tile.remove() is sufficient for tiles on the
   // board.
   for (var lidx=0; lidx<msg[1].length; ++lidx) {
      var letterToRemove = upperLetterOrBlank(msg[1][lidx]);
      for (var i=0; i<handTilesUi.length; ++i) {
         var tile = handTilesUi[i];
         var handLetter = upperLetterOrBlank(tileText(tile));

         if (handLetter == letterToRemove) {
            console.log("Tile is in rack at " + racktiles.indexOf(tile));
            removeTileFromRack(tile);
            tile.remove()
            handTilesUi.splice(i, 1);
            break;
         }
      }
   }
}

function processPassOkayMessage(msg) {
   chat_game_message(currTurnName + " passed");
}

function processPlayOkayMessage(msg) {
   if (myName != currTurnName) {
      chat_game_message(currTurnName + " played " + msg[2] + " (" + msg[3] + ")");
      return;
   }
   // move all the tiles back to the rack
   click_shuf_tiles();

   // Remove the letter from rack if present there.
   // tile.remove() is sufficient for tiles on the
   // board.
   for (var lidx=0; lidx<msg[1].length; ++lidx) {
      var letterToRemove = upperLetterOrBlank(msg[1][lidx]);
      for (var i=0; i<handTilesUi.length; ++i) {
         var tile = handTilesUi[i];
         var handLetter = upperLetterOrBlank(tileText(tile));

         if (handLetter == letterToRemove) {
            console.log("tile is also in the rack " + racktiles.indexOf(tile));
            removeTileFromRack(tile);
            removeTileFromExchArea(tile);
            tile.remove()
            handTilesUi.splice(i, 1);
            break;
         }
      }
   }
   chat_game_message("Word made '" + msg[2] + "' score " + msg[3]);
}

function processScoreMessage(scoreMsg) {
   // Add to local playerScoreList
   console.log(scoreMsg);
   var tiles_in_bag = scoreMsg[1];
   document.getElementById("tilesInBagUi").innerHTML = tiles_in_bag;

   var j = 0;
   // First token is message is "SCORE" so
   // start from 1
   for (var i=2; i<scoreMsg.length; i+=2) {
      if (playerScoreList.length <= j) {
         // Add a UI element to show score
         // for the newly discovered player
         var bottompanel = document.getElementById("bottompanel");
         var cell = bottompanel.insertCell(-1);
         cell.style.width = "30px";

         var cell = bottompanel.insertCell(-1);
         var playername = document.createElement("div");
         playername.className = "player";
         playername.id = "playername" + j;
         playername.innerHTML = scoreMsg[i];

         var playerscore = document.createElement("div");
         playerscore.className = "player";
         playerscore.id = "playerscore" + j;

         if (scoreMsg[i] == myName) {
            playername.style.textDecoration = "underline";
         }

         cell.appendChild(playername);
         cell.appendChild(playerscore);
      }
      playerScoreList[j] = [ scoreMsg[i], scoreMsg[i+1] ];
      document.getElementById("playerscore" + j).innerHTML = scoreMsg[i+1];
      j++;
   }
}

function processRackTilesMessage(msg) {
   var newtiles = msg[1];
   for (var i=0; i<newtiles.length; ++i) {
      var tile = newtiles[i];
      var tileUi = createRackTile(tile);
      handTilesUi.push(tileUi);
      setTileInRack(tileUi, 0);
   }
   document.getElementById("shufbutton").disabled = false;
}

function processTurnMessage(msg) {
   var buttons_disabled = true;
   if (msg[1] == myName) {
      // my turn
      state = ClientState.turn;
      buttons_disabled = false;
      turnKey = msg[2];
      chat_game_message("Your turn");

      document.getElementById("popAudio").play();
   } else {
      chat_game_message(msg[1] + "'s turn");
      state = ClientState.wait_turn;
      turnKey = "";
   }
   currTurnName = msg[1];

   // Enabled/disable move buttons
   document.getElementById("exchbutton").disabled = buttons_disabled;
   document.getElementById("passbutton").disabled = buttons_disabled;
   document.getElementById("playbutton").disabled = buttons_disabled;

   for (var plIdx=0; plIdx<playerScoreList.length; ++plIdx) {
      var className = "player";
      if (playerScoreList[plIdx][0] == msg[1]) {
         className = "playerturn";
      }
      var uiele = document.getElementById("playername" + plIdx);
      uiele.className = className;
   }
}

function processBoardTilesMessage(msg) {
   for (var i=1; i<msg.length; ++i) {
      var letter = msg[i][0];
      var row = msg[i][1];
      var col = msg[i][2];
      var tileUi = createBoardTile(letter);

      if (boardtiles[row][col]) {
         var oldTile = boardtiles[row][col];
         setTileInRack(oldTile, 0);
         boardtiles[row][col] = null;
      }
      setTileOnBoard(tileUi, row, col);
   }
}

function clearBoardAndRack() {
   while (handTilesUi.length > 0) {
      var tileUi = handTilesUi[handTilesUi.length - 1];
      removeTileFromBoard(tileUi);
      removeTileFromRack(tileUi);
      removeTileFromExchArea(tileUi);
      tileUi.remove();
      handTilesUi.splice(handTilesUi.length - 1, 1);
   }

   for (var r=0; r<15; ++r) {
      for (var c=0; c<15; ++c) {
         if (boardtiles[r][c]) {
            boardtiles[r][c].remove();
            boardtiles[r][c] = null;
         }
      }
   }

   turnKey = "";
   myName = "";

   // remove player score and separators
   for (var i=0; i<playerScoreList.length*2; ++i) {
      var bottompanel = document.getElementById("bottompanel");
      bottompanel.children[bottompanel.children.length-1].remove();
   }

   playerScoreList = [];
}

// Socket event handling
function sock_onerror(event) {
   console.log(event);
   hide_moves_ui();
   connectStateTxn(ConnectState.disconnected);
}

function sock_onclose(event) {
   console.log(event);
   connectStateTxn(ConnectState.disconnected);
   hide_moves_ui();
   showStatus("disconnect: connect error");
}

function processListMovesOkayMessage(msg) {
   moves_ui_clear();
   moves_ui_add_players(msg[1]);
   moves_ui_add_moves(msg[2]);
   show_moves_ui();
}

function sock_onmessage(event) {
   var msg = JSON.parse(event.data);
   console.log("Client state=" + state + " msg=" + msg);

   if (msg[0] == "LIST-MOVES-OKAY") {
      processListMovesOkayMessage(msg);
      return;
   } else if (msg[0] == "LIST-MOVES-BAD") {
      return;
   }

   if (state == ClientState.start) {
      // ignore all messages
      return;

   } else if (state == ClientState.wait_viewer_ack) {
      // viewer mode
      if (msg[0] == "VIEW-OKAY") {
         state = ClientState.viewer;
         connectStateTxn(ConnectState.connected);
         return;
      } else if (msg[0] == "VIEW-BAD") {
         alert(msg[1]);
         state = ClientState.start;
         connectStateTxn(ConnectState.disconnected);
         return;
      } else {
         // ignore all other messages
      }
      return;

   } else if (state == ClientState.wait_join_ack) {
      // connecting as player
      if (msg[0] == "JOIN-OKAY") {
         state = ClientState.wait_game_start;
         connectStateTxn(ConnectState.connected);
         return;
      } else if (msg[0] == "JOIN-BAD") {
         state = ClientState.start;
         connectStateTxn(ConnectState.disconnected);
         return;
      } else {
         // ignore all other messages
      }
      return;

   } else if (state == ClientState.wait_game_start) {
      if (msg[0] == "RACKTILES") {
         processRackTilesMessage(msg);
         return;
      }

   } else if (state == ClientState.wait_turn_ack) {
      // just made a move and waitnig for confirmation

      if (msg[0] == "PLAY-BAD" || msg[0] == "PASS-BAD" || msg[0] == "EXCH-BAD") {
         chat_game_message("Bad move: " + msg[1]);
         state = ClientState.turn;
         click_shuf_tiles();
         return;
      } else if (msg[0] == "EXCH-OKAY") {
         state = ClientState.wait_turn;
         processExchOkayMessage(msg);
         return;
      } else if (msg[0] == "PLAY-OKAY") {
         state = ClientState.wait_turn;
         processPlayOkayMessage(msg);
         return;
      } else if (msg[0] == "PASS-OKAY") {
         state = ClientState.wait_turn;
         processPassOkayMessage(msg);
         return;
      }

   }

   // Commands that work in several state
   if (msg[0] == "GAME-OVER") {
      processGameOverMessage(msg);
      chat_game_message("Game over");

   } else if (msg[0] == "CHAT-MSG") {
      var dt = new Date(0);
      dt.setUTCSeconds(msg[1]);
      chat_add_message(dt.toLocaleTimeString(), msg[2], msg[3]);

   } else if (msg[0] == "SCORE") {
      processScoreMessage(msg);

      for (var i=0; i<playerScoreList.length; i++) {
         showPlayerScore(i, playerScoreList[i][1]);
      }

   } else if (msg[0] == "TURN") {
      processTurnMessage(msg);

   } else if (msg[0] == "BOARDTILES") {
      processBoardTilesMessage(msg);

   } else if (msg[0] == "RACKTILES") {
      processRackTilesMessage(msg);

   } else if (msg[0] == "EXCH-OKAY") {
      state = ClientState.wait_turn;
      processExchOkayMessage(msg);

   } else if (msg[0] == "PLAY-OKAY") {
      state = ClientState.wait_turn;
      processPlayOkayMessage(msg);

   } else if (msg[0] == "PASS-OKAY") {
      state = ClientState.wait_turn;
      processPassOkayMessage(msg);
      return;

   }
}

function connect_as_viewer() {
   sock.send(JSON.stringify( [ "VIEW" ] ));
   state = ClientState.wait_viewer_ack;
}


function connect_as_player() {
   var name = document.getElementById("name").value;
   var passwd = document.getElementById("password").value;
   sock.send(JSON.stringify( [ "JOIN", name, passwd ] ));
   state = ClientState.wait_join_ack;
}

function resetSock() {
   if (sock) {
      sock.close();
   }
   sock = null;
}
