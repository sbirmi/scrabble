function boardCell(r, c) {
   return document.getElementById("board_" + r + "_" + c);
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

   if (letter == " ") {
      tile.style.color = "#0000ff";
      tile.onclick = function(ev) {
         var blankTileLetters = " abcdefghijklmnopqrstuvwxyz";
         var idx = blankTileLetters.indexOf(tiletext.innerHTML);
         if (ev.layerX > tilewidth/2) {
            idx += 1;
         } else {
            idx -= 1;
         }
         if (idx == -1) {
            idx += blankTileLetters.length;
         } else {
            idx = idx % blankTileLetters.length;
         }
         tiletext.innerHTML = blankTileLetters[idx];
      }
   }

   dragElement(tile);
   return tile;
}

function tileText(ele) {
   for (var i=0; i<ele.children.length; ++i) {
      var tile = ele.children[i];
      if (tile.className.includes("tiletext")) {
         return tile.innerHTML;
      }
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
   }
}

function dropOn(ele, x, y) {
   // See if we are dropping the tile in the exchange area
   var exchAreaUi = document.getElementById("exchangeArea");
   var exchAreaRect = exchAreaUi.getBoundingClientRect();
   if (coordsInRect(x, y, exchAreaRect)) {
      // If tile is coming from the board, remove it from there
      removeTileFromBoard(dragObj);

      // If tile is from the rack, remove it from there
      removeTileFromRack(dragObj);

      // Add it to exchange
      if (exchangetiles.indexOf(ele) == -1) {
         // The tile wasn't in the exchange area so
         // needs to be added there
         exchangetiles.push(ele);
      }
      return true;
   }


   // See if we are dropping the tile on the rack
   var rackEle = document.getElementById("tilerack");
   var rackEleRect = rackEle.getBoundingClientRect();
   if (coordsInRect(x, y, rackEleRect)) {
      for (var i=0; i < rackspotEles.length; i++) {
         if (coordsInRect(x, y,rackspotEles[i].getBoundingClientRect())) {
            console.log("Dropping tile at rack spot " + i);
            removeTileFromBoard(dragObj);
            removeTileFromExchArea(dragObj);

            setTileInRack(dragObj, i);
            return true;
         }
      }
   }

   // See if we are droppnig the tile on the board
//   console.log("dropping at x=" + x + " y=" + y);
//   console.log(document.getElementById("board_0_0").getBoundingClientRect());
   for (var r=0; r<15; ++r) {
      for (var c=0; c<15; ++c) {
         if (coordsInRect(x, y,
               boardCell(r, c).getBoundingClientRect())) {
            console.log("dropped '" + tileText(dragObj) + "' on board at " + r + "," + c);
            success = setTileOnBoard(dragObj, r, c);
            if (success) {
               removeTileFromExchArea(ele);
               removeTileFromRack(ele);
            }
            return success;
         }
      }
   }
   return false;
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

function dragElement(ele) {
   var deltaX = 0, deltaY = 0, lastX = 0, lastY = 0;
   var startTop = 0, startLeft = 0;

   ele.onmousedown = dragMouseDown;
   ele.addEventListener("touchstart", dragMouseDown, false);
   ele.addEventListener("touchmove", elementDrag, false);
   ele.addEventListener("touchcancel", closeDragElement, false);
   ele.addEventListener("touchend", closeDragElement, false);

   function dragMouseDown(e) {
      e = e || window.event;
      if (e instanceof MouseEvent) {
         lastX = e.clientX;
         lastY = e.clientY;
      } else if (e instanceof TouchEvent) {
         if (e.changedTouches.length > 0) {
            var touch = e.changedTouches[0]
            lastX = touch.pageX;
            lastY = touch.pageY;
         } else {
            return;
         }
      } else {
         return;
      }
      dragObj = e.target.parentElement;
      startTop = dragObj.style.top;
      startLeft = dragObj.style.left;
      e.preventDefault();
      document.onmouseup = closeDragElement;
      document.onmousemove = elementDrag;
      document.ontouchmove = elementDrag;
   }

   function elementDrag(e) {
      e = e || window.event;
      if (!e || !e.clientX) { return; }
      e.preventDefault();
      deltaX = lastX - e.clientX;
      deltaY = lastY - e.clientY;
      lastX = e.clientX;
      lastY = e.clientY;
      dragObj.style.top = (dragObj.offsetTop - deltaY) + "px";
      dragObj.style.left = (dragObj.offsetLeft - deltaX) + "px";
//      ele.style.top = (ele.offsetTop - deltaY) + "px";
//      ele.style.left = (ele.offsetLeft - deltaX) + "px";
   }

   function closeDragElement(e) {
      var stopX = e.clientX;
      var stopY = e.clientY;

      if (e instanceof TouchEvent) {
         if (e.changedTouches.length > 0) {
            var touch = e.changedTouches[0]
            stopX = touch.pageX;
            stopY = touch.pageY;
         } else {
            return;
         }
      }

      var success = dropOn(dragObj, stopX, stopY);
      if (!success) {
         dragObj.style.top = startTop;
         dragObj.style.left = startLeft;
      }
      dragObj = null;
      document.onmouseup = null;
      document.onmousemove = null;
      document.ontouchmove = null;
   }
}

function click_shuf_tiles() {
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
            cell_design.innerHTML = "Triple Word Score";
         } else if (style[r][c] == "dw") {
            cell_design.innerHTML = "Double Word Score";
         } else if (style[r][c] == "tl") {
            cell_design.innerHTML = "Triple Letter Score";
         } else if (style[r][c] == "dl") {
            cell_design.innerHTML = "Double Letter Score";
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
   }
}

function buttonsDisabled(state) {
   document.getElementById("shufbutton").disabled = state;
   document.getElementById("exchbutton").disabled = state;
   document.getElementById("passbutton").disabled = state;
   document.getElementById("playbutton").disabled = state;
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
   cell.innerHTML = "(" + ts + ")";

   var cell = row.insertCell(1);
   cell.innerHTML = sender + ":";

   var cell = row.insertCell(2);
   cell.innerHTML = message;

   row.scrollIntoView(false);
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
      buttonsDisabled(true);

      connect_state = state;


   } else if (state == ConnectState.connect_as_viewer) {
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
      connect_state = state;
   }
}
