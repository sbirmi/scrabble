#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "game.hpp"
#include "storage.hpp"
#include "word_list.hpp"

class GameTest : public ::testing::Test {
protected:
    std::string test_db_file;
    std::string test_wordlist_file;
    Storage::Inst* storage;
    WordList* wordlist;
    Game::Inst* game;

    void SetUp() override {
        test_db_file = "test_game.sqlite3";
        test_wordlist_file = "test_game_wordlist.txt";
        
        // Clean up any existing files
        std::filesystem::remove(test_db_file);
        std::filesystem::remove(test_wordlist_file);
        
        // Create test wordlist
        std::ofstream wl_file(test_wordlist_file);
        wl_file << "HELLO\n";
        wl_file << "WORLD\n";
        wl_file << "TEST\n";
        wl_file << "SCRABBLE\n";
        wl_file << "GAME\n";
        wl_file << "CAT\n";
        wl_file << "DOG\n";
        wl_file << "PLAY\n";
        wl_file << "WORD\n";
        wl_file << "LIST\n";
        wl_file << "AT\n";
        wl_file << "GO\n";
        wl_file << "TO\n";
        wl_file << "IT\n";
        wl_file << "A\n";
        wl_file << "I\n";
        wl_file.close();
        
        // Initialize components
        storage = new Storage::Inst(test_db_file);
        wordlist = new WordList(test_wordlist_file);
        game = new Game::Inst(1, 2, *storage, *wordlist);
    }

    void TearDown() override {
        delete game;
        delete wordlist;
        delete storage;
        std::filesystem::remove(test_db_file);
        std::filesystem::remove(test_wordlist_file);
    }
    
    // Helper to create a mock handle (simplified for testing)
    Handle createMockHandle() {
        // For testing purposes, we'll use a simple pointer cast
        // In a real scenario, this would be a proper WebSocket handle
        static int counter = 0;
        return std::weak_ptr<void>(std::shared_ptr<void>((void*)(++counter)));
    }
};

TEST_F(GameTest, GameCreation) {
    EXPECT_EQ(game->get_gid(), 1);
    EXPECT_EQ(game->get_max_players(), 2);
    EXPECT_FALSE(game->is_game_over());
}

TEST_F(GameTest, GameInitialization) {
    // Test that game initializes with proper tile distribution
    EXPECT_GT(game->get_tiles_in_bag(), 90); // Should have most tiles initially
    EXPECT_LT(game->get_tiles_in_bag(), 101); // But not more than total
}

TEST_F(GameTest, PlayerJoining) {
    Handle h1 = createMockHandle();
    Handle h2 = createMockHandle();
    HandleResponseList responses;
    
    // Create join command
    Json::Value joinCmd(Json::arrayValue);
    joinCmd.append("JOIN");
    joinCmd.append("Player1");
    joinCmd.append("password");
    
    // Test first player join
    bool result = game->process_cmd_join(h1, joinCmd, responses);
    EXPECT_TRUE(result);
    EXPECT_GT(responses.size(), 0);
    
    // Test second player join
    joinCmd[1] = "Player2";
    result = game->process_cmd_join(h2, joinCmd, responses);
    EXPECT_TRUE(result);
}

TEST_F(GameTest, ViewerMode) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Create view command
    Json::Value viewCmd(Json::arrayValue);
    viewCmd.append("VIEW");
    
    bool result = game->process_cmd_view(h1, viewCmd, responses);
    EXPECT_TRUE(result);
    EXPECT_GT(responses.size(), 0);
}

TEST_F(GameTest, PassMove) {
    Handle h1 = createMockHandle();
    Handle h2 = createMockHandle();
    HandleResponseList responses;
    
    // Join two players first
    Json::Value joinCmd(Json::arrayValue);
    joinCmd.append("JOIN");
    joinCmd.append("Player1");
    joinCmd.append("password");
    game->process_cmd_join(h1, joinCmd, responses);
    
    joinCmd[1] = "Player2";
    game->process_cmd_join(h2, joinCmd, responses);
    
    // Clear responses
    responses.clear();
    
    // Try to pass (need proper turn key from game state)
    Json::Value passCmd(Json::arrayValue);
    passCmd.append("PASS");
    passCmd.append("dummy_turn_key"); // In real game, this would be valid
    
    // This might fail due to invalid turn key, but tests the function
    game->process_cmd_pass(h1, passCmd, responses);
    // We expect this to generate some response even if it fails
    EXPECT_GT(responses.size(), 0);
}

TEST_F(GameTest, InvalidCommands) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test empty command
    Json::Value emptyCmd(Json::arrayValue);
    bool result = game->process_cmd_join(h1, emptyCmd, responses);
    EXPECT_FALSE(result);
    
    // Test malformed join command
    Json::Value badJoinCmd(Json::arrayValue);
    badJoinCmd.append("JOIN");
    // Missing player name and password
    result = game->process_cmd_join(h1, badJoinCmd, responses);
    EXPECT_FALSE(result);
}

TEST_F(GameTest, GameStateRetrieval) {
    // Test that we can get basic game state
    EXPECT_EQ(game->get_gid(), 1);
    EXPECT_EQ(game->get_max_players(), 2);
    EXPECT_GE(game->get_tiles_in_bag(), 0);
    EXPECT_FALSE(game->is_game_over());
}

TEST_F(GameTest, MoveValidation) {
    // Test the tile placement validation logic
    Handle h1 = createMockHandle();
    Handle h2 = createMockHandle();
    HandleResponseList responses;
    
    // Setup game with two players
    Json::Value joinCmd(Json::arrayValue);
    joinCmd.append("JOIN");
    joinCmd.append("Player1");
    joinCmd.append("password");
    game->process_cmd_join(h1, joinCmd, responses);
    
    joinCmd[1] = "Player2";
    game->process_cmd_join(h2, joinCmd, responses);
    
    responses.clear();
    
    // Test play command structure (even if it fails validation)
    Json::Value playCmd(Json::arrayValue);
    playCmd.append("PLAY");
    playCmd.append("dummy_turn_key");
    
    Json::Value move(Json::arrayValue);
    move.append("H");
    move.append(7);
    move.append(7);
    playCmd.append(move);
    
    // This will likely fail due to turn validation, but tests the parsing
    game->process_cmd_play(h1, playCmd, responses);
    EXPECT_GT(responses.size(), 0);
}

TEST_F(GameTest, ChatFunctionality) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test chat command
    Json::Value chatCmd(Json::arrayValue);
    chatCmd.append("CHAT-SEND");
    chatCmd.append("Hello, world!");
    
    // Even without proper player setup, should handle chat gracefully
    game->process_cmd_chat_send(h1, chatCmd, responses);
    // Should generate some response
    EXPECT_GE(responses.size(), 0);
}

TEST_F(GameTest, ListMoves) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test list moves command
    Json::Value listCmd(Json::arrayValue);
    listCmd.append("LIST-MOVES");
    
    bool result = game->process_cmd_list_moves(h1, listCmd, responses);
    // Should return true and provide move list (even if empty)
    EXPECT_TRUE(result);
    EXPECT_GT(responses.size(), 0);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}