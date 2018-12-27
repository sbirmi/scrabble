// For UI
var boardcell_size = 65;
var tilewidth = 63;

var playerScoreList = [];

var boardtiles = new Array(15).fill(null).map(() => new Array(15).fill(null));
var racktiles = [ null, null, null, null, null, null, null ];
//var frozentiles = [];
var rackspotEles = [];

var dragObj = null;

// Tiles in hand. They maybe on the rack
// or on the board
var handTilesUi = [];

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


// For the game protocol
var myName = "";
var turnKey = "";
