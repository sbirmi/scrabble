#include <gtest/gtest.h>
#include <jsoncpp/json/json.h>
#include <memory>
#include <sqlite3.h>
#include "../game.hpp"
#include "../word_list.hpp"
#include "../storage.hpp"

class WordCheckTest : public ::testing::Test {
protected:
    WordList* wordlist;
    Storage::Inst* storage;
    Game::Inst* game;
    Json::Reader* jsonReader;
    Json::FastWriter* jsonWriter;

    void SetUp() override {
        // Create a test wordlist file
        std::ofstream testWordlistFile("test_wordlist.txt");
        testWordlistFile << "HELLO\n";
        testWordlistFile << "WORLD\n";
        testWordlistFile << "TEST\n";
        testWordlistFile << "SCRABBLE\n";
        testWordlistFile << "VALID\n";
        testWordlistFile.close();

        // Initialize components
        wordlist = new WordList("test_wordlist.txt");
        storage = new Storage::Inst("test_word_check.sqlite3");
        
        // Create database schema
        createDatabaseSchema();
        
        jsonReader = new Json::Reader();
        jsonWriter = new Json::FastWriter();
        
        // Create a game instance
        game = new Game::Inst(1, wordlist, storage, 2, jsonReader, jsonWriter);
    }

    void TearDown() override {
        delete game;
        delete storage;
        delete wordlist;
        delete jsonReader;
        delete jsonWriter;
        
        // Clean up test files
        std::remove("test_wordlist.txt");
        std::remove("test_word_check.sqlite3");
    }

    Handle createMockHandle() {
        static int counter = 0;
        ++counter;
        auto ptr = std::make_shared<int>(counter);
        return std::weak_ptr<void>(std::static_pointer_cast<void>(ptr));
    }

    void createDatabaseSchema() {
        // Create the necessary database tables for testing
        // Schema extracted from existing games.sqlite3
        const char* createTables = R"(
            CREATE TABLE IF NOT EXISTS `Games` (
                `gid`           INTEGER PRIMARY KEY AUTOINCREMENT,
                `maxPlayers`    INTEGER NOT NULL,
                `turnIndex`     INTEGER NOT NULL,
                `wordsMade`     INTEGER NOT NULL DEFAULT 0,
                `passesMade`    INTEGER NOT NULL DEFAULT 0,
                `gameOver`      INTEGER NOT NULL DEFAULT 0,
                `tiles`         TEXT
            );
            
            CREATE TABLE IF NOT EXISTS `Players` (
                `pid`           INTEGER PRIMARY KEY AUTOINCREMENT,
                `gid`           INTEGER NOT NULL,
                `plIdx`         INTEGER NOT NULL,
                `playerName`    TEXT NOT NULL,
                `secretKey`     INTEGER NOT NULL,
                `score`         INTEGER NOT NULL DEFAULT 0,
                `hand`          TEXT,
                `state`         INTEGER NOT NULL,
                `turnKey`       TEXT,
                `endScoreAdj`   INTEGER NOT NULL DEFAULT 0
            );
            
            CREATE TABLE IF NOT EXISTS `Moves` (
                `mid`           INTEGER PRIMARY KEY AUTOINCREMENT,
                `gid`           INTEGER NOT NULL,
                `pid`           INTEGER NOT NULL,
                `moveType`      INTEGER NOT NULL,
                `moveDetail`    TEXT,
                `longestWord`   TEXT,
                `score`         INTEGER NOT NULL
            );
            
            CREATE TABLE IF NOT EXISTS `BoardTiles` (
                `bid`           INTEGER PRIMARY KEY AUTOINCREMENT,
                `gid`           INTEGER NOT NULL,
                `row`           INTEGER NOT NULL,
                `col`           INTEGER NOT NULL,
                `letter`        INTEGER NOT NULL,
                `score`         INTEGER NOT NULL
            );
        )";
        
        char *errMsg = 0;
        sqlite3* db;
        sqlite3_open("test_word_check.sqlite3", &db);
        int rc = sqlite3_exec(db, createTables, NULL, 0, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }
        sqlite3_close(db);
    }

    void joinPlayerToGame(Handle handle) {
        // Initialize handle properly
        game->handle_appear(handle);
        
        Json::Value joinCmd;
        joinCmd.append("JOIN");
        joinCmd.append("player1");
        joinCmd.append("");  // no password
        
        HandleResponseList responses;
        game->test_join_player(handle, joinCmd, responses);
    }

    void setPlayerAsViewer(Handle handle) {
        // Initialize handle properly
        game->handle_appear(handle);
        
        Json::Value viewCmd;
        viewCmd.append("VIEW");
        
        HandleResponseList responses;
        game->test_set_viewer(handle, viewCmd, responses);
    }
};

// Test valid word checking
TEST_F(WordCheckTest, ValidWordCheck) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("hello");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_TRUE(result);
    ASSERT_EQ(responses.size(), 1);
    
    // Parse the response
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-OKAY");
    EXPECT_EQ(response[1].asString(), "hello");
    EXPECT_TRUE(response[2].asBool());
    EXPECT_EQ(response[3].asString(), "✓ 'HELLO' is a valid word");
}

// Test invalid word checking
TEST_F(WordCheckTest, InvalidWordCheck) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("invalidword");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_TRUE(result);
    ASSERT_EQ(responses.size(), 1);
    
    // Parse the response
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-OKAY");
    EXPECT_EQ(response[1].asString(), "invalidword");
    EXPECT_FALSE(response[2].asBool());
    EXPECT_EQ(response[3].asString(), "✗ 'INVALIDWORD' is not a valid word");
}

// Test case insensitive checking
TEST_F(WordCheckTest, CaseInsensitiveCheck) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    // Test uppercase word
    Json::Value checkCmd1;
    checkCmd1.append("CHECK");
    checkCmd1.append("WORLD");
    
    HandleResponseList responses1;
    bool result1 = game->test_word_check(h1, checkCmd1, responses1);
    
    EXPECT_TRUE(result1);
    ASSERT_EQ(responses1.size(), 1);
    
    Json::Value response1;
    jsonReader->parse(responses1[0].second, response1);
    EXPECT_TRUE(response1[2].asBool());
    
    // Test mixed case word
    Json::Value checkCmd2;
    checkCmd2.append("CHECK");
    checkCmd2.append("WoRlD");
    
    HandleResponseList responses2;
    bool result2 = game->test_word_check(h1, checkCmd2, responses2);
    
    EXPECT_TRUE(result2);
    ASSERT_EQ(responses2.size(), 1);
    
    Json::Value response2;
    jsonReader->parse(responses2[0].second, response2);
    EXPECT_TRUE(response2[2].asBool());
}

// Test whitespace trimming
TEST_F(WordCheckTest, WhitespaceTrimming) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("  test  ");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_TRUE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-OKAY");
    EXPECT_EQ(response[1].asString(), "  test  ");  // Original preserved in response
    EXPECT_TRUE(response[2].asBool());
}

// Test viewer access
TEST_F(WordCheckTest, ViewerAccess) {
    Handle h1 = createMockHandle();
    setPlayerAsViewer(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("valid");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_TRUE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-OKAY");
    EXPECT_TRUE(response[2].asBool());
}

// Test access denied for idle clients
TEST_F(WordCheckTest, AccessDeniedForIdleClients) {
    Handle h1 = createMockHandle();
    // Initialize handle but don't join or set as viewer - client remains idle
    game->handle_appear(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("test");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Access denied");
}

// Test invalid format - no word parameter
TEST_F(WordCheckTest, InvalidFormatNoWord) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    // Missing word parameter
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Invalid format");
}

// Test invalid format - too many parameters
TEST_F(WordCheckTest, InvalidFormatTooManyParams) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("word1");
    checkCmd.append("word2");
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Invalid format");
}

// Test empty word error
TEST_F(WordCheckTest, EmptyWordError) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("   ");  // Only whitespace
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Empty word");
}

// Test multiple words error
TEST_F(WordCheckTest, MultipleWordsError) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append("hello world");  // Multiple words
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Only one word allowed");
}

// Test non-string parameter
TEST_F(WordCheckTest, NonStringParameter) {
    Handle h1 = createMockHandle();
    joinPlayerToGame(h1);
    
    Json::Value checkCmd;
    checkCmd.append("CHECK");
    checkCmd.append(123);  // Number instead of string
    
    HandleResponseList responses;
    bool result = game->test_word_check(h1, checkCmd, responses);
    
    EXPECT_FALSE(result);
    ASSERT_EQ(responses.size(), 1);
    
    Json::Value response;
    jsonReader->parse(responses[0].second, response);
    
    EXPECT_EQ(response[0].asString(), "CHECK-BAD");
    EXPECT_EQ(response[1].asString(), "Invalid format");
}