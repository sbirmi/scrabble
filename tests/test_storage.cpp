#include <gtest/gtest.h>
#include <filesystem>
#include "storage.hpp"

class StorageTest : public ::testing::Test {
protected:
    std::string test_db_file;
    Storage::Inst* storage;

    void SetUp() override {
        test_db_file = "test_storage.sqlite3";
        // Remove any existing test database
        std::filesystem::remove(test_db_file);
        storage = new Storage::Inst(test_db_file);
    }

    void TearDown() override {
        delete storage;
        std::filesystem::remove(test_db_file);
    }
};

TEST_F(StorageTest, DatabaseCreation) {
    EXPECT_TRUE(std::filesystem::exists(test_db_file));
}

TEST_F(StorageTest, GameOperations) {
    // Test adding a game
    storage->add_game(1, 2, 0, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    
    // Test retrieving game list
    StorageGameList games = storage->game_list();
    EXPECT_EQ(games.size(), 1);
    EXPECT_EQ(games[0].gid, 1);
    EXPECT_EQ(games[0].maxPlayers, 2);
    EXPECT_EQ(games[0].turnIndex, 0);
    EXPECT_EQ(games[0].tiles, "ABCDEFGHIJKLMNOPQRSTUVWXYZ");
}

TEST_F(StorageTest, PlayerOperations) {
    // Add a game first
    storage->add_game(1, 2, 0, "TILES");
    
    // Add players
    unsigned int pid1 = storage->add_player(1, 0, "Player1", "12345", 0);
    unsigned int pid2 = storage->add_player(1, 1, "Player2", "67890", 0);
    
    // Test retrieving player list
    StoragePlayerList players = storage->game_player_list(1);
    EXPECT_EQ(players.size(), 2);
    
    EXPECT_EQ(players[0].plIdx, 0);
    EXPECT_EQ(players[0].playerName, "Player1");
    EXPECT_EQ(players[0].secretKey, "12345");
    
    EXPECT_EQ(players[1].plIdx, 1);
    EXPECT_EQ(players[1].playerName, "Player2");
    EXPECT_EQ(players[1].secretKey, "67890");
    
    // Update player hands
    storage->update_player_hand(pid1, "ABCDEFG");
    storage->update_player_hand(pid2, "HIJKLMN");
    
    // Re-fetch to check hands
    players = storage->game_player_list(1);
    EXPECT_EQ(players[0].hand, "ABCDEFG");
    EXPECT_EQ(players[1].hand, "HIJKLMN");
}

TEST_F(StorageTest, BoardTileOperations) {
    // Add a game first
    storage->add_game(1, 2, 0, "TILES");
    
    // Add board tiles
    storage->add_board_tile(1, 7, 7, 'H', 4);
    storage->add_board_tile(1, 7, 8, 'E', 1);
    storage->add_board_tile(1, 7, 9, 'L', 1);
    storage->add_board_tile(1, 7, 10, 'L', 1);
    storage->add_board_tile(1, 7, 11, 'O', 1);
    
    // Test retrieving board tiles
    StorageBoardTileList tiles = storage->game_board_tile_list(1);
    EXPECT_EQ(tiles.size(), 5);
    
    EXPECT_EQ(tiles[0].row, 7);
    EXPECT_EQ(tiles[0].col, 7);
    EXPECT_EQ(tiles[0].letter, 'H');
    EXPECT_EQ(tiles[0].score, 4);
}

TEST_F(StorageTest, MoveOperations) {
    // Add a game first
    storage->add_game(1, 2, 0, "TILES");
    
    // Add players
    unsigned int pid1 = storage->add_player(1, 0, "Player1", "12345", 0);
    
    // Add moves
    storage->add_player_move_play(1, pid1, "HELLO_MOVE", "HELLO", 14);  // Play move
    storage->add_player_move_pass(1, pid1);                              // Pass move
    storage->add_player_move_exch(1, pid1, "ABC");                       // Exchange move
    
    // Test retrieving moves
    StorageMoveList moves = storage->game_move_list(1);
    EXPECT_EQ(moves.size(), 3);
    
    EXPECT_EQ(moves[0].moveType, 2);
    EXPECT_EQ(moves[0].longestWord, "HELLO");
    EXPECT_EQ(moves[0].score, 14);
    
    EXPECT_EQ(moves[1].moveType, 0);
    EXPECT_EQ(moves[1].score, 0);
    
    EXPECT_EQ(moves[2].moveType, 1);
    EXPECT_EQ(moves[2].longestWord, "ABC");
    EXPECT_EQ(moves[2].score, -3);
}

TEST_F(StorageTest, GameUpdates) {
    // Add a game
    storage->add_game(1, 2, 0, "INITIAL");
    
    // Test various updates
    storage->update_game_tiles(1, "UPDATED");
    storage->update_game_words_made(1, 5);
    storage->update_game_passes_made(1, 2);
    storage->update_game_turn_index(1, 1);
    
    // Verify updates
    StorageGameList games = storage->game_list();
    EXPECT_EQ(games.size(), 1);
    EXPECT_EQ(games[0].tiles, "UPDATED");
    EXPECT_EQ(games[0].wordsMade, 5);
    EXPECT_EQ(games[0].passesMade, 2);
    EXPECT_EQ(games[0].turnIndex, 1);
}

TEST_F(StorageTest, PlayerUpdates) {
    // Add game and player
    storage->add_game(1, 2, 0, "TILES");
    unsigned int pid1 = storage->add_player(1, 0, "Player1", "12345", 0);
    
    // Update player
    storage->update_player_hand(pid1, "NEWHAND");
    storage->update_player_score(pid1, 25);
    storage->update_player_turnkey(pid1, "turnkey123");
    
    // Verify updates
    StoragePlayerList players = storage->game_player_list(1);
    EXPECT_EQ(players.size(), 1);
    EXPECT_EQ(players[0].hand, "NEWHAND");
    EXPECT_EQ(players[0].score, 25);
    EXPECT_EQ(players[0].turnKey, "turnkey123");
}

TEST_F(StorageTest, MultipleGames) {
    // Add multiple games
    storage->add_game(1, 2, 0, "GAME1");
    storage->add_game(2, 4, 1, "GAME2");
    storage->add_game(3, 3, 2, "GAME3");
    
    StorageGameList games = storage->game_list();
    EXPECT_EQ(games.size(), 3);
    
    // Verify each game
    for (const auto& game : games) {
        if (game.gid == 1) {
            EXPECT_EQ(game.maxPlayers, 2);
            EXPECT_EQ(game.turnIndex, 0);
            EXPECT_EQ(game.tiles, "GAME1");
        } else if (game.gid == 2) {
            EXPECT_EQ(game.maxPlayers, 4);
            EXPECT_EQ(game.turnIndex, 1);
            EXPECT_EQ(game.tiles, "GAME2");
        } else if (game.gid == 3) {
            EXPECT_EQ(game.maxPlayers, 3);
            EXPECT_EQ(game.turnIndex, 2);
            EXPECT_EQ(game.tiles, "GAME3");
        }
    }
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}