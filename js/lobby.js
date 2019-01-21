var domain = window.location.host.split(":")[0];
var sock_url = "ws://" + domain + ":" + game_server_port + "/lobby";
var sock = null;
var connected = false;

// General UI
function ui_text(text) {
   var ui = document.createTextNode(text);
   return ui;
}

// Host
function lobby_host() {
   var pl_count_ui = document.getElementById("host_player_count_ui");

   var host_msg = [ "HOST", pl_count_ui.value * 1 ];
   lobby_send(host_msg);
}

// UI List manipulation
function list_clear(list) {
   if (!list) {
      return;
   }
   while (list.firstChild) {
      list.firstChild.remove();
   }
}

function list_remove_gameid(list, gid) {
   if (!list) {
      return;
   }
   for (var i=0; i < list.children.length; ++i) {
      var ele = list.children[i];
      if (ele.getAttribute("gid") == gid) {
         ele.remove();
         break;
      }
   }
}

function all_list_clear() {
   list_clear(document.getElementById("waitingGames"));
   list_clear(document.getElementById("ongoingGames"));
   list_clear(document.getElementById("completedGames"));
}

function all_list_remove_gameid(gid) {
   list_remove_gameid(document.getElementById("waitingGames"), gid);
   list_remove_gameid(document.getElementById("ongoingGames"), gid);
   list_remove_gameid(document.getElementById("completedGames"), gid);
}

function add_game_info(gid, completed, max_players, turn_index,
                       tiles_in_bag, playerscore) {
   all_list_remove_gameid(gid);

   var list = null;
   if (completed) {
      list = document.getElementById("completedGames");
   } else if (playerscore.length == max_players) {
      list = document.getElementById("ongoingGames");
   } else {
      list = document.getElementById("waitingGames");
   }

   var link = document.createElement("a");
   link.href = "/game/" + gid;
   link.target = "_blank";

   link.appendChild(ui_text("#" + gid));

   if (max_players) {
      link.appendChild(ui_text(" - " + max_players + " players"));
      link.appendChild(ui_text(" - " + tiles_in_bag + " tiles in bag"));
   }

   var li = document.createElement("li");
   li.setAttribute("gid", gid);
   li.appendChild(link);
   li.appendChild(document.createElement("br"));

   if (playerscore) {
      var scoreline_ui = document.createElement("span");

      for (var i=0; i<playerscore.length; ++i) {
         if (i>0) {
            scoreline_ui.appendChild(ui_text(", "));
         }
         var plstr = playerscore[i][0] + " (" + playerscore[i][1] + ")"
         if (turn_index == i) {
            var hl = document.createElement("b");
            hl.appendChild(ui_text(plstr));
            scoreline_ui.appendChild(hl);
         } else {
            scoreline_ui.appendChild(ui_text(plstr));
         }
      }

      li.appendChild(scoreline_ui);
   }

   // Insert in reverse descending order
   for (var i=0; i<list.children.length; ++i) {
      var foundgid = list.children[i].getAttribute("gid");
      if (gid>foundgid) {
         list.insertBefore(li, list.children[i]);
         return;
      }
   }
   list.appendChild(li);
}

// Warning Bar
function show_warning_bar(state, msg) {
   var display = (state) ? "block" : "none";
   document.getElementById("warning_bar_ui").style.display = display;
   document.getElementById("warning_bar_ui").innerHTML = msg;
}

// Socket message processing
function lobby_recv(msg) {
   console.log(msg);

   // [ "GAME", <gameid>, completed=true/false, maxPlayers, turnIndex,
   //   [ <playername1>, <score1> ], [ <playername2>, <score2> ], ... ]
   if (msg[0] == "GAME") {
      var gid = msg[1];
      var completed = msg[2];
      var max_players = msg[3];
      var turn_index = msg[4];
      var tiles_in_bag = msg[5];
      var playerscore = [];

      for (var i=6; i<msg.length; ++i) {
         var pl_score = msg[i];
         playerscore.push(pl_score);
      }

      add_game_info(gid, completed, max_players, turn_index,
                    tiles_in_bag, playerscore);
   }
}

function lobby_send(msg) {
   if (!connected) {
      console.log("lobby_send: disconnected");
      return;
   }
   console.log(msg);
   sock.send(JSON.stringify(msg));
}

function sock_onmessage(event) {
   var msg = JSON.parse(event.data);
   lobby_recv(msg);
}

function sock_onerror(event) {
   console.log(event);
   sock_reset();
}

function sock_onclose(event) {
   console.log(event);
   sock_reset();
}

function sock_onopen(event) {
   connected = true;
   console.log(event);
   show_warning_bar(false, "Connected");
   all_list_clear();
   var list_msg = [ "LIST" ];
   lobby_send(list_msg);
}

function sock_reset() {
   connected = false;
   if (sock) {
      sock.close();
   }

   sock = null;
   show_warning_bar(true, "Disconnected: <a href=''>reconnect</a>");
}

// Connect to lobby
function connect_to_lobby() {
   connected = false;
   show_warning_bar(true, "Connecting");
   sock = new WebSocket(sock_url);
   sock.onclose = sock_onclose;
   sock.onopen = sock_onopen;
   sock.onerror = sock_onerror;
   sock.onmessage = sock_onmessage;
}
