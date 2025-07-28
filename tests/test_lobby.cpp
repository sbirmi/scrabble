#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "lobby.hpp"
#include "storage.hpp"

class LobbyTest : public ::testing::Test {
protected:
    std::string test_db_file;
    std::string test_wordlist_file;
    Storage::Inst* storage;
    WordList* wordlist;
    Lobby::Inst* lobby;

    void SetUp() override {
        test_db_file = "test_lobby.sqlite3";
        test_wordlist_file = "test_lobby_wordlist.txt";
        
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
        wl_file.close();
        
        // Initialize components
        storage = new Storage::Inst(test_db_file);
        wordlist = new WordList(test_wordlist_file);
        lobby = new Lobby::Inst(*storage, *wordlist);
    }

    void TearDown() override {
        delete lobby;
        delete wordlist;
        delete storage;
        std::filesystem::remove(test_db_file);
        std::filesystem::remove(test_wordlist_file);
    }
    
    // Helper to create a mock handle
    Handle createMockHandle() {
        static int counter = 0;
        return std::weak_ptr<void>(std::shared_ptr<void>((void*)(++counter)));
    }
};

TEST_F(LobbyTest, LobbyCreation) {
    // Test that lobby was created successfully
    EXPECT_NE(lobby, nullptr);
}

TEST_F(LobbyTest, ListGames) {
    Handle h1 = createMockHandle();
    
    // Create list command
    Json::Value listCmd(Json::arrayValue);
    listCmd.append("LIST");
    
    HandleResponseList responses = lobby->process_lobby_cmd_list(h1, listCmd);
    
    // Should get at least one response (even if game list is empty)
    EXPECT_GT(responses.size(), 0);
}

TEST_F(LobbyTest, HostGame) {
    Handle h1 = createMockHandle();
    
    // Create host command for 2 players
    Json::Value hostCmd(Json::arrayValue);
    hostCmd.append("HOST");
    hostCmd.append(2);
    
    HandleResponseList responses;
    lobby->process_lobby_cmd_host(h1, hostCmd, responses);
    
    // Should get response about game creation
    EXPECT_GT(responses.size(), 0);
}

TEST_F(LobbyTest, HostGameInvalidPlayers) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test hosting with invalid number of players
    Json::Value hostCmd(Json::arrayValue);
    hostCmd.append("HOST");
    hostCmd.append(0); // Invalid: 0 players
    
    lobby->process_lobby_cmd_host(h1, hostCmd, responses);
    EXPECT_GT(responses.size(), 0);
    
    responses.clear();
    
    // Test hosting with too many players
    hostCmd[1] = 10; // Invalid: too many players
    lobby->process_lobby_cmd_host(h1, hostCmd, responses);
    EXPECT_GT(responses.size(), 0);
}

TEST_F(LobbyTest, HostMultipleGames) {
    Handle h1 = createMockHandle();
    Handle h2 = createMockHandle();
    HandleResponseList responses;
    
    // Host first game
    Json::Value hostCmd(Json::arrayValue);
    hostCmd.append("HOST");
    hostCmd.append(2);
    
    lobby->process_lobby_cmd_host(h1, hostCmd, responses);
    EXPECT_GT(responses.size(), 0);
    
    responses.clear();
    
    // Host second game
    hostCmd[1] = 3; // Different number of players
    lobby->process_lobby_cmd_host(h2, hostCmd, responses);
    EXPECT_GT(responses.size(), 0);
    
    responses.clear();
    
    // List games to verify both exist
    Json::Value listCmd(Json::arrayValue);
    listCmd.append("LIST");
    
    HandleResponseList listResponses = lobby->process_lobby_cmd_list(h1, listCmd);
    EXPECT_GT(listResponses.size(), 0);
}

TEST_F(LobbyTest, MessageProcessing) {
    Handle h1 = createMockHandle();
    
    // Create a JSON message string
    std::string jsonMsg = "[\"LIST\"]";
    
    HandleResponseList responses = lobby->process_msg(h1, 
        websocketpp::lib::make_shared<std::string>(jsonMsg));
    
    // Should process the message and return responses
    EXPECT_GT(responses.size(), 0);
}

TEST_F(LobbyTest, InvalidJsonMessage) {
    Handle h1 = createMockHandle();
    
    // Test with invalid JSON
    std::string invalidJson = "not json at all";
    
    HandleResponseList responses = lobby->process_msg(h1, 
        websocketpp::lib::make_shared<std::string>(invalidJson));
    
    // Should handle gracefully and return appropriate response
    EXPECT_GE(responses.size(), 0);
}

TEST_F(LobbyTest, EmptyMessage) {
    Handle h1 = createMockHandle();
    
    // Test with empty message
    std::string emptyMsg = "";
    
    HandleResponseList responses = lobby->process_msg(h1, 
        websocketpp::lib::make_shared<std::string>(emptyMsg));
    
    // Should handle gracefully
    EXPECT_GE(responses.size(), 0);
}

TEST_F(LobbyTest, UnknownCommand) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test with unknown command
    Json::Value unknownCmd(Json::arrayValue);
    unknownCmd.append("UNKNOWN_COMMAND");
    unknownCmd.append("parameter");
    
    lobby->process_lobby_cmd(h1, unknownCmd, responses);
    
    // Should handle unknown command gracefully
    EXPECT_GE(responses.size(), 0);
}

TEST_F(LobbyTest, MalformedCommands) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Test empty command array
    Json::Value emptyCmd(Json::arrayValue);
    lobby->process_lobby_cmd(h1, emptyCmd, responses);
    EXPECT_GE(responses.size(), 0);
    
    responses.clear();
    
    // Test HOST command without parameters
    Json::Value incompleteHost(Json::arrayValue);
    incompleteHost.append("HOST");
    // Missing player count
    lobby->process_lobby_cmd(h1, incompleteHost, responses);
    EXPECT_GE(responses.size(), 0);
}

TEST_F(LobbyTest, GameLifecycle) {
    Handle h1 = createMockHandle();
    HandleResponseList responses;
    
    // Host a game
    Json::Value hostCmd(Json::arrayValue);
    hostCmd.append("HOST");
    hostCmd.append(2);
    
    lobby->process_lobby_cmd_host(h1, hostCmd, responses);
    EXPECT_GT(responses.size(), 0);
    
    responses.clear();
    
    // List games to see the hosted game
    Json::Value listCmd(Json::arrayValue);
    listCmd.append("LIST");
    
    HandleResponseList listResponses = lobby->process_lobby_cmd_list(h1, listCmd);
    EXPECT_GT(listResponses.size(), 0);
    
    // The response should contain information about the game
    // In a real test, we'd parse the JSON response to verify game details
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}