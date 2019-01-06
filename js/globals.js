// For UI
var game_server_port = 5051;
var boardcell_size = 65;
var tilewidth = 63;

var playerScoreList = [];


// Tiles in hand. They maybe on the rack, on the
// board, or sitting in the exchange area.
var handTilesUi = [];

// List of all tiles on the board
//var boardtiles = new Array(15).fill(null).map(() => new Array(15).fill(null));
// for old browsers
var boardtiles = [
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ],
   [ null, null, null, null, null, null, null, null, null, null, null, null, null, null, null ]
];

// List of all tiles on the rack
var racktiles = [ null, null, null, null, null, null, null ];
// List of tiles in the exchange area
var exchangetiles = []


var rackspotEles = [];

// The tile that's selected
var selectedTile = null;

var letterscore = {
   "A": 1, "B": 3, "C": 3, "D": 2, "E": 1,
   "F": 4, "G": 2, "H": 4, "I": 1, "J": 8,
   "K": 5, "L": 1, "M": 3, "N": 1, "O": 1,
   "P": 3, "Q":10, "R": 1, "S": 1, "T": 1,
   "U": 1, "V": 4, "W": 4, "X": 8, "Y": 4,
   "Z":10, " ": 0,
   "a": 0, "b": 0, "c": 0, "d": 0, "e": 0,
   "f": 0, "g": 0, "h": 0, "i": 0, "j": 0,
   "k": 0, "l": 0, "m": 0, "n": 0, "o": 0,
   "p": 0, "q": 0, "r": 0, "s": 0, "t": 0,
   "u": 0, "v": 0, "w": 0, "x": 0, "y": 0,
   "z": 0,
}

var blankLetterChgTimer = null;

// For the game protocol
var myName = "";
var turnKey = "";

// Client state
var ClientState = Object.freeze({
   "start": 0,
   "wait_viewer_ack": 1,
   "viewer": 2,
   "wait_join_ack": 3,
   "wait_game_start": 4,
   "turn": 5,
   "wait_turn_ack":6,
   "wait_turn": 7,
   "game_over": 8});

var ConnectState = Object.freeze({
   "disconnected": 0,
   "connect_as_player": 1,
   "connect_as_viewer": 2,
   "connected": 3,
});

var connect_state = null;
var state = ClientState.start;
var domain = window.location.host.split(":")[0];
var sock_url = "ws://" + domain + ":" + game_server_port + "/";

var sock = null;

// Popping sound from
// https://freesound.org/people/deraj/sounds/202230/
// License: CC0
var popSound = "pop.wav";
