
function processGameOverMessage(msg) {
   document.getElementById("shufbutton").disabled = true;
   document.getElementById("exchbutton").disabled = true;
   document.getElementById("passbutton").disabled = true;
   document.getElementById("playbutton").disabled = true;
   sock.close();
}

function processPlayOkayMessage(msg) {
   console.log("processPlayOkayMessage"); // TODO
   console.log(handTilesUi); // TODO
   console.log(racktiles); // TODO
   // move all the tiles back to the rack
   click_shuf_tiles();
   console.log("processPlayOkayMessage: post shuffle"); // TODO
   console.log(handTilesUi); // TODO
   console.log(racktiles); // TODO

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
            if (racktiles.indexOf(tile) > -1) {
               racktiles[racktiles.indexOf(tile)] = null;
            }
            tile.remove()
            handTilesUi.splice(i, 1);
            break;
         }
      }
   }

   console.log("processPlayOkayMessage: end"); // TODO
   console.log(handTilesUi); // TODO
   console.log(racktiles); // TODO
}

function processScoreMessage(scoreMsg) {
   // Add to local playerScoreList
   console.log(scoreMsg);
   var j = 0;
   // First token is message is "SCORE" so
   // start from 1
   for (var i=1; i<scoreMsg.length; i+=2) {
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
   } else {
      state = ClientState.wait_turn;
      turnKey = "";
   }

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

function connect_as_viewer() {
   sock.send(JSON.stringify( [ "VIEW" ] ));
   hide_connect_panel();
   state = ClientState.wait_viewer_ack;
}

function connect_as_player() {
   var name = document.getElementById("name").value;
   var passwd = document.getElementById("password").value;

   if (name == "") {
      alert("username missing");
      return;
   }
   if (passwd == "") {
      alert("password missing");
      return;
   }
   myName = name;
   sock.send(JSON.stringify( [ "JOIN", name, passwd ] ));

   hide_connect_panel();
   state = ClientState.wait_join_ack;
}
