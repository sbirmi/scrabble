function playername_keypress(event) {
   if (event.keyCode == 13) {
      document.getElementById("password").focus();
   }
}

function password_keypress(event) {
   if (event.keyCode == 13) {
      document.getElementById("but_join").click();
   }
}

function chatInput_keypress(event) {
   if (event.keyCode == 13) {
      document.getElementById("chatSend").click();
   }
}

function boardCell(r, c) {
   return document.getElementById("board_" + r + "_" + c);
}

function selectTile(ele) {
   // if a previous tile is selected, unselect it
   var prevtile = selectedTile;

   if (prevtile) {
      selectedTile = null;
      prevtile.setAttribute("class", "tile");
   }

   if (prevtile != ele && ele) {
      selectedTile = ele;
      ele.setAttribute("class", "tilehighlight");
   }

   if (selectedTile && upperLetterOrBlank(tileText(selectedTile)) == " ") {
      document.getElementById("blbutton").disabled = false;
   } else {
      document.getElementById("blbutton").disabled = true;
   }
}

function showPlayerScore(plIdx, score) {
   var scoreUi = document.getElementById("playerscore" + plIdx);
   scoreUi.innerHTML = score;
}

function showStatus(txt) {
   var el = document.getElementById("status");
   el.innerHTML = txt;
}

function findTileOnBoard(ele) {
   coords = [ -1, -1 ];
   for (var r=0; r<15; r++) {
      var c = boardtiles[r].indexOf(ele);
      if (c > -1) {
         return [ r, c ]
      }
   }
   return [ -1, -1 ];
}

function createBoardTile(letter) {
   var score = letterscore[letter];
	var tile = document.createElement("div");
	document.body.appendChild(tile);
	tile.setAttribute("class", "tile");
	tile.style.width = tilewidth.toString() + "px";
	tile.style.height = tilewidth.toString() + "px";

	var tiletext = document.createElement("div");
   tile.appendChild(tiletext);
	tiletext.setAttribute("class", "tiletext noselect");
   tiletext.innerHTML = letter;
	tiletext.style.width = tilewidth.toString() + "px";
	tiletext.style.height = tilewidth.toString() + "px";

	var tilescore = document.createElement("div");
   tile.appendChild(tilescore);
	tilescore.setAttribute("class", "tilescore noselect");
   tilescore.innerHTML = score.toString();

   tile.style.color = "#404040";
   return tile;
}

function nextLetterOnBlank(tt, spd) {
   var blankTileLetters = " abcdefghijklmnopqrstuvwxyz";
   var idx = blankTileLetters.indexOf(tt.innerHTML);
   idx += spd;

   if (idx < 0) {
      idx += blankTileLetters.length;
   } else {
      idx = idx % blankTileLetters.length;
   }
   tt.innerHTML = blankTileLetters[idx];
}

function createRackTile(letter) {
   var score = letterscore[letter];
	var tile = document.createElement("div");
	document.body.appendChild(tile);
	tile.setAttribute("class", "tile");
	tile.style.width = tilewidth.toString() + "px";
	tile.style.height = tilewidth.toString() + "px";

	var tiletext = document.createElement("div");
   tile.appendChild(tiletext);
	tiletext.setAttribute("class", "tiletext noselect");
   tiletext.innerHTML = letter;
	tiletext.style.width = tilewidth.toString() + "px";
	tiletext.style.height = tilewidth.toString() + "px";

	var tilescore = document.createElement("div");
   tile.appendChild(tilescore);
	tilescore.setAttribute("class", "tilescore noselect");
   tilescore.innerHTML = score.toString();

   tile.setAttribute("onrack", 1);

   function tileclick(ev) {
      var lastSelectedTile = selectedTile;

      if (lastSelectedTile == ev.target) {
         // unselect self and stop
         selectTile(ev.target);
         console.log(ev);
         ev.stopPropagation();
         return;
      }

      if (lastSelectedTile == null) {
         selectTile(ev.target);
         ev.stopPropagation();
         return;
      }

      // If newly selected tile is not on rack,
      // we are toggling highlighted tile
      if (ev.target.getAttribute("onrack") == 0) {
         selectTile(ev.target);
         ev.stopPropagation();
         return;
      }

      // Newly selected tile is on rack,
      // we will assume we want to place the
      // previously selected tile at
      // this new spot
      var rackspot = racktiles.indexOf(ev.target);
      setTileInRack(selectedTile, rackspot);
      removeTileFromBoard(selectedTile);
      removeTileFromExchArea(selectedTile);
      selectTile(selectedTile);
      ev.stopPropagation();
   }
   tile.onclick = function(ev) {
      tileclick(ev);
   }

   if (letter == " ") {
      tile.style.color = "#0000ff";
   }

   return tile;
}

function tiletextUiFromTile(tile) {
   if (!tile) {
      return null;
   }
   for (var i=0; i<tile.children.length; ++i) {
      var obj = tile.children[i];
      if (obj.className.includes("tiletext")) {
         return obj
      }
   }
   return null;
}

function tileText(ele) {
   var tiletext = tiletextUiFromTile(ele);
   if (tiletext) {
      return tiletext.innerHTML;
   }
   return "";
}

function upperLetterOrBlank(letter) {
   return (letter == letter.toLowerCase()) ? " " : letter;
}

function rackposName(idx) {
   return "rackpos" + idx.toString();
}

function coordsInRect(x, y, rect) {
   return (x >= rect.left && x <= rect.right &&
           y >= rect.top && y <= rect.bottom);
}

function removeTileFromBoard(ele) {
   var prevR, prevC;
   [prevR, prevC] = findTileOnBoard(ele);
   if (prevR > -1 && prevC > -1) {
      boardtiles[prevR][prevC] = null;
   }
}

function removeTileFromRack(ele) {
   prevIdx = racktiles.indexOf(ele);
   if (prevIdx > -1) {
      racktiles[prevIdx] = null;
   }
}

function removeTileFromExchArea(ele) {
   var exchIdx = exchangetiles.indexOf(ele);
   if (exchIdx > -1) {
      exchangetiles.splice(exchIdx, 1);
      rearrange_exch_tiles();
   }
}

function setTileInExchArea(ele) {
   if (!ele) {
      return;
   }
   if (exchangetiles.indexOf(ele) >= 0) {
      return;
   }
   ele.setAttribute("onrack", 0);
   // The tile wasn't in the exchange area so
   // needs to be added there
   exchangetiles.push(ele);

   ele.remove();
   document.getElementById("exchangeArea").appendChild(ele);

   rearrange_exch_tiles();
}

function rearrange_exch_tiles() {
   for (var i=0; i<exchangetiles.length; ++i) {
      var tile = exchangetiles[i];
      tile.style.left = ((i % 4) * boardcell_size) + "px";
      tile.style.top = (Math.floor(i/4) * boardcell_size) + "px";
   }
}

function setTileOnBoard(ele, r, c) {
   if (c < 0 || c >= 15 || r < 0 || r >= 15) {
      return false;
   }

   // Check if there is already a tile here. If so, return False
   if (boardtiles[r][c]) {
      return false;
   }

   // find old position of the tile on the board and remove from there

   [prevR, prevC] = findTileOnBoard(ele);
   if (prevR > -1) {
      boardtiles[prevR][prevC] = null;
   }

   boardtiles[r][c] = ele;

   ele.remove();
   var boardEle = boardCell(r, c);
   boardEle.appendChild(ele);
   ele.style.top = "1px";
   ele.style.left = "1px";

   ele.setAttribute("onrack", 0);

   return true;
}

function setTileInRack(ele, idx) {
   if ( idx < 0 || idx >= 7 || ele == null ) {
      return;
   }

   var prevIdx = racktiles.indexOf(ele);

   if (racktiles[idx] == null || prevIdx == idx) {
      if (prevIdx > -1 && prevIdx != idx) {
         racktiles[prevIdx] = null;
      }
      racktiles[idx] = ele;

      ele.remove();
      document.getElementById(rackposName(idx)).appendChild(ele);
      ele.style.top = "1px";
      ele.style.left = "1px";

      ele.setAttribute("onrack", 1);

      return;
   }

   // The tile is getting added to a spot where we already have a tile.

   // See if the tile is picked from some place on the left on the rack.
   // If so, move all tiles to the left
   if (prevIdx > -1 && prevIdx < idx) {
      for (var i=prevIdx; i<idx; i++) {
         racktiles[i] = null;
         setTileInRack(racktiles[i+1], i);
      }
      racktiles[idx] = null;
      setTileInRack(ele, idx);
      return;
   }

   // The tile came from the board
   if (prevIdx > idx) {
      for (var i=prevIdx; i>idx; i--) {
         racktiles[i] = null;
         setTileInRack(racktiles[i-1], i);
      }
      racktiles[idx] = null;
      setTileInRack(ele, idx);
      return;
   }

   // The tile came from the board and we should have a space
   // somewhere on the rack. Place the tile there
   blankIdx = racktiles.indexOf( null );
   if ( blankIdx > -1 ) {
      setTileInRack(ele, blankIdx);
      return;
   }

   console.log("Not sure where the tlie came from!");
   console.log(ele);

}

function click_shuf_tiles() {
   // Unhighlight selected tile
   selectTile(selectedTile);

   // 1. Remove the tile from the board and rack
   console.log("shuf_tiles begin handTilesUi.length = " + handTilesUi.length);
   for (var i=0; i<handTilesUi.length; ++i) {
      var tileUi = handTilesUi[i];

      removeTileFromRack(tileUi);
      removeTileFromExchArea(tileUi);
      removeTileFromBoard(tileUi);
   }

   // 2. place the tiles again on the rack
   for (var i=0; i<handTilesUi.length; ++i) {
      var tileUi = handTilesUi[i];
      var randomPos = Math.floor(Math.random() * 7);
      setTileInRack(tileUi, randomPos);
   }

   console.log("shuf_tiles end handTilesUi.length = " + handTilesUi.length);
}

function click_blank_button() {
   if (!selectedTile) {
      document.getElementById("blbutton").disabled = true;
      return;
   }

   var tt = tiletextUiFromTile(selectedTile);
   if (tt) {
      nextLetterOnBlank(tt, 1);
   }
}

function click_exch_button() {
   if (state != ClientState.turn) {
      return;
   }

   if (exchangetiles.length == 0) {
      return ;
   }

   var exch = [ "EXCH", turnKey, "" ];
   for (var i=0; i<exchangetiles.length; ++i) {
      var tileUi = exchangetiles[i];
      exch[2] += tileText(tileUi);
   }

   sock.send(JSON.stringify(exch));
   state = ClientState.wait_turn_ack;
}

function click_pass_button() {
   if (state != ClientState.turn) {
      return;
   }
   sock.send(JSON.stringify(["PASS", turnKey]));
   state = ClientState.wait_turn_ack;
}

function click_play_button() {
   if (state != ClientState.turn) {
      return;
   }

   var play = [ "PLAY", turnKey ];
   for (var i=0; i<handTilesUi.length; ++i) {
      var tileUi = handTilesUi[i];
      var br, bc;
      [br, bc] = findTileOnBoard(tileUi);
      if (br > -1 && bc > -1) {
         var letter = tileText(tileUi);
         play.push([letter, br, bc]);
      }
   }

   if (play.length == 2) {
      return;
   }

   sock.send(JSON.stringify(play));
   state = ClientState.wait_turn_ack;
}

function setupBoard() {
   var table = document.getElementById("board");

   var style = [
      [ "tw", "bl", "bl", "dl", "bl", "bl", "bl", "tw", "bl", "bl", "bl", "dl", "bl", "bl", "tw" ],
      [ "bl", "dw", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "dw", "bl" ],
      [ "bl", "bl", "dw", "bl", "bl", "bl", "dl", "bl", "dl", "bl", "bl", "bl", "dw", "bl", "bl" ],
      [ "dl", "bl", "bl", "dw", "bl", "bl", "bl", "dl", "bl", "bl", "bl", "dw", "bl", "bl", "dl" ],
      [ "bl", "bl", "bl", "bl", "dw", "bl", "bl", "bl", "bl", "bl", "dw", "bl", "bl", "bl", "bl" ],
      [ "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl" ],
      [ "bl", "bl", "dl", "bl", "bl", "bl", "dl", "bl", "dl", "bl", "bl", "bl", "dl", "bl", "bl" ],
      [ "tw", "bl", "bl", "dl", "bl", "bl", "bl", "home", "bl", "bl", "bl", "dl", "bl", "bl", "tw" ],
      [ "bl", "bl", "dl", "bl", "bl", "bl", "dl", "bl", "dl", "bl", "bl", "bl", "dl", "bl", "bl" ],
      [ "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl" ],
      [ "bl", "bl", "bl", "bl", "dw", "bl", "bl", "bl", "bl", "bl", "dw", "bl", "bl", "bl", "bl" ],
      [ "dl", "bl", "bl", "dw", "bl", "bl", "bl", "dl", "bl", "bl", "bl", "dw", "bl", "bl", "dl" ],
      [ "bl", "bl", "dw", "bl", "bl", "bl", "dl", "bl", "dl", "bl", "bl", "bl", "dw", "bl", "bl" ],
      [ "bl", "dw", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "tl", "bl", "bl", "bl", "dw", "bl" ],
      [ "tw", "bl", "bl", "dl", "bl", "bl", "bl", "tw", "bl", "bl", "bl", "dl", "bl", "bl", "tw" ],
      ];

   for (var r=0; r<15; r++) {
      var row = table.insertRow(r);
      
      for (var c=0; c<15; c++) {
         var cell = row.insertCell(c);

         var holder_div = document.createElement("div");
         holder_div.id = "board_" + r.toString() + "_" + c.toString();
         holder_div.style.position = "relative";
         holder_div.style.width = tilewidth + "px";
         holder_div.style.height = tilewidth + "px";
         cell.appendChild(holder_div);

         var cell_design = document.createElement("div");
         cell_design.style.position = "absolute";
         cell_design.zIndex = 1;
         cell_design.style.width = boardcell_size.toString() + "px";
         cell_design.style.height = (boardcell_size - 8).toString() + "px";
         cell_design.setAttribute("class", style[r][c] + " noselect");
         cell_design.style.paddingTop = "8px";
         holder_div.appendChild(cell_design);

         if (style[r][c] == "tw") {
            cell_design.innerHTML = "Triple<br>Word<br>Score";
         } else if (style[r][c] == "dw") {
            cell_design.innerHTML = "Double<br>Word<br>Score";
         } else if (style[r][c] == "tl") {
            cell_design.innerHTML = "Triple<br>Letter<br>Score";
         } else if (style[r][c] == "dl") {
            cell_design.innerHTML = "Double<br>Letter<br>Score";
         }

         holder_div.setAttribute("row", r);
         holder_div.setAttribute("col", c);

         holder_div.onclick = function (ev) {
            var row = ev.target.getAttribute("row");
            var col = ev.target.getAttribute("col");

            if (!selectedTile) {
               return;
            }
            var success = setTileOnBoard(selectedTile, row, col);
            if (success) {
               removeTileFromExchArea(selectedTile);
               removeTileFromRack(selectedTile);
               selectTile(selectedTile);
            }
         }
      }
   }
}

function setupRack() {
   var tilerackEle = document.getElementById("tilerack");
   var tilerackrowEle = tilerackEle.insertRow(0);

   for (var i=0; i<7; i++) {
      var cell = tilerackrowEle.insertCell(i);
      cell.setAttribute("class", "tileblank noselect");
      cell.id = rackposName(i);
      cell.style.width = tilewidth.toString() + "px";
      cell.style.height = tilewidth.toString() + "px";
      rackspotEles.push(cell);

      cell.setAttribute("spot", i);

      cell.onclick = function (ev) {
         if (ev.target.id.substring(0, 7) != "rackpos") {
            return;
         }
         var spot = ev.target.getAttribute("spot");

         console.log("Dropped tile on rack spot=" + spot);
         setTileInRack(selectedTile, spot);
         removeTileFromBoard(selectedTile);
         removeTileFromExchArea(selectedTile);
         selectTile(selectedTile);
      }
   }
}

function clickExchangeArea() {
   if (!selectedTile) {
      return;
   }

   removeTileFromRack(selectedTile);
   removeTileFromBoard(selectedTile);

   setTileInExchArea(selectedTile);
   selectTile(selectedTile);
}

function buttonsDisabled(state) {
   document.getElementById("shufbutton").disabled = state;
   document.getElementById("exchbutton").disabled = state;
   document.getElementById("passbutton").disabled = state;
   document.getElementById("playbutton").disabled = state;
   if (state) {
      document.getElementById("blbutton").disabled = state;
   }
   movesButtonDisabled(state);
}

function movesButtonDisabled(state) {
   document.getElementById("movesbutton").disabled = state;
}

function join_view_buttons_disabled(disabled) {
   document.getElementById("but_join").disabled = disabled;
   document.getElementById("but_view").disabled = disabled;
}

function show_chat_panel() {
   document.getElementById("chatPanelUi").style.display = "table";
}

function hide_chat_panel() {
   document.getElementById("chatPanelUi").style.display = "none";
}

function clear_chat_panel() {
   var tbody = document.getElementById("chatWindow").children[0];

   while (tbody.firstChild) {
      tbody.removeChild(tbody.firstChild);
   }
}

function chat_add_message(ts, sender, message) {
   var tbody = document.getElementById("chatWindow").children[0];

   var row = tbody.insertRow(tbody.children.length);

   var cell = row.insertCell(0);
   cell.innerHTML = ts;
   cell.style.verticalAlign = "top";

   var cell = row.insertCell(1);
   cell.innerHTML = sender + ":";
   cell.style.verticalAlign = "top";

   var cell = row.insertCell(2);
   cell.innerHTML = message;
   cell.style.verticalAlign = "top";

   row.scrollIntoView(false);
}

function chat_game_message(message) {
   var dt = new Date();
   var hours = dt.getHours();
   var ampm = "AM";
   if (hours > 12) {
      hours -= 12;
      ampm = "PM";
   }
   var minutes = dt.getMinutes();
   minutes = minutes < 10 ? '0' + minutes : minutes;
   var seconds = dt.getSeconds();
   var seconds = seconds < 10 ? '0' + seconds : seconds;
   var strTime = hours + ":" + minutes + ":" + seconds + " " + ampm;
   
   chat_add_message(strTime, "SERVER", message);
}

function show_connect_panel() {
   document.getElementById("connectpanel").style.display = "table";
}

function hide_connect_panel() {
   document.getElementById("connectpanel").style.display = "none";
}

function send_chat_message() {
   var chatInputUi = document.getElementById("chatInput");
   if (!chatInputUi.value) {
      return;
   }
   console.log("CHAT-SEND: " + chatInputUi.value);
   sock.send(JSON.stringify( [ "CHAT-SEND", chatInputUi.value ] ));
   chatInputUi.value = "";
   return chatInputUi;
}

function connectStateTxn(state) {
   if (state == ConnectState.disconnected) {
      clearBoardAndRack();
      resetSock();
      show_connect_panel();
      hide_chat_panel();
      showStatus("disconnected");
      join_view_buttons_disabled(false);
      hide_moves_ui();
      buttonsDisabled(true);

      connect_state = state;


   } else if (state == ConnectState.connect_as_viewer) {
      show_connect_panel();
      hide_chat_panel();
      showStatus("connecting");
      join_view_buttons_disabled(true);
      buttonsDisabled(true);
      movesButtonDisabled(false);

      sock = new WebSocket(sock_url);
      sock.onclose = sock_onclose;
      sock.onerror = sock_onerror;
      sock.onmessage = sock_onmessage;
      sock.onopen = function (event) {
         console.log(event);
         connect_as_viewer();
      }

      connect_state = state;

   } else if (state == ConnectState.connect_as_player) {

      var name = document.getElementById("name").value;
      var passwd = document.getElementById("password").value;

      if (name == "") {
         showStatus("disconnected: PlayerName missing");
         resetSock();
         return;
      }
      if (passwd == "") {
         showStatus("disconnected: SecretKey missing");
         resetSock();
         return;
      }
      myName = name;

      show_connect_panel();
      hide_chat_panel();
      showStatus("connecting");
      join_view_buttons_disabled(true);
      buttonsDisabled(true);

      sock = new WebSocket(sock_url);
      sock.onclose = sock_onclose;
      sock.onerror = sock_onerror;
      sock.onmessage = sock_onmessage;
      sock.onopen = function (event) {
         console.log(event);
         connect_as_player();
      }

      connect_state = state;

   } else if (state == ConnectState.connected) {
      hide_connect_panel();
      show_chat_panel();
      join_view_buttons_disabled(true);
      movesButtonDisabled(false);
      connect_state = state;
   }
}

// Moves UI helpers
function show_moves_ui() {
   var movesUi = document.getElementById("movesUi");
   movesUi.style.display = "block";

   movesUi.style.top = "30px";
   movesUi.style.left = "30px";

}

function hide_moves_ui() {
   document.getElementById("movesUi").style.display = "none";
}

function is_moves_ui_visible() {
   var movesUi = document.getElementById("movesUi");
   return movesUi.style.display == "block";
}

function click_moves_button() {
   var movesUi = document.getElementById("movesUi");
   if (is_moves_ui_visible()) {
      hide_moves_ui();
   } else {
      sock.send(JSON.stringify(["LIST-MOVES"]));
   }
}

function moves_ui_clear_moves() {
   var movesUi = document.getElementById("movesUi");
   var tbody = movesUi.children[1];
   while (tbody.firstChild) {
      tbody.removeChild(tbody.firstChild);
   }
}

function moves_ui_clear() {
   var movesUi = document.getElementById("movesUi");
   var theadRow = movesUi.children[0].children[0];
   while (theadRow.firstChild) {
      theadRow.removeChild(theadRow.firstChild);
   }
   moves_ui_clear_moves();
}

function moves_ui_add_players(players) {
   var movesUi = document.getElementById("movesUi");
   var theadRow = movesUi.children[0].children[0];
   for (var i=0; i<players.length; ++i) {
      var cell = theadRow.insertCell(i);

      cell.innerHTML = players[i];
      cell.setAttribute("class", "movesheader noselect");
   }
}

function moves_ui_add_new_row() {
   var movesUi = document.getElementById("movesUi");
   var tbody = movesUi.children[1];
   return tbody.insertRow(tbody.children.length);
}

function moves_ui_add_moves(moves) {
   var movesUi = document.getElementById("movesUi");
   var tbody = movesUi.children[1];
   var lastRow = null;

   var maxPlayers = movesUi.children[0].children[0].children.length;
   if (maxPlayers == 0) {
      console.log("Can't add moves until players are added");
      return;
   }
   if (tbody.children.length == 0) {
      // add a blank row
      lastRow = moves_ui_add_new_row();
   } else {
      lastRow = tbody.children[tbody.children.length-1];
      if (lastRow.children.length == maxPlayers) {
         lastRow = moves_ui_add_new_row();
      }
   }

   if (moves == null) {
      return;
   }

   // Starting adding elements from the last row
   for (var i=0; i<moves.length; ++i) {
      var cell = lastRow.insertCell(lastRow.children.length);

      cell.setAttribute("class", "movesbody noselect");
      if (moves[i][0] == 0) {
         cell.innerHTML = "<i>passed</i>";
      } else if (moves[i][0] == 1) {
         cell.innerHTML = "<i>exchanged</i>";
      } else {
         cell.innerHTML = moves[i][2] + " (" + moves[i][1] + ")";
      }

      if (lastRow.children.length == maxPlayers) {
         lastRow = moves_ui_add_new_row();
      }
   }

}
