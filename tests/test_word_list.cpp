#include <gtest/gtest.h>
#include <fstream>
#include <filesystem>
#include "word_list.hpp"

class WordListTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test wordlist
        std::ofstream file("test_wordlist.txt");
        file << "HELLO\n";
        file << "WORLD\n"; 
        file << "TEST\n";
        file << "SCRABBLE\n";
        file << "GAME\n";
        file << "CAT\n";
        file << "DOG\n";
        file << "PLAY\n";
        file << "WORD\n";
        file << "LIST\n";
        file.close();
    }

    void TearDown() override {
        std::filesystem::remove("test_wordlist.txt");
    }
};

TEST_F(WordListTest, ValidWords) {
    WordList wl("test_wordlist.txt");
    
    EXPECT_TRUE(wl.is_valid("HELLO"));
    EXPECT_TRUE(wl.is_valid("WORLD"));
    EXPECT_TRUE(wl.is_valid("TEST"));
    EXPECT_TRUE(wl.is_valid("SCRABBLE"));
    EXPECT_TRUE(wl.is_valid("GAME"));
    EXPECT_TRUE(wl.is_valid("CAT"));
    EXPECT_TRUE(wl.is_valid("DOG"));
    EXPECT_TRUE(wl.is_valid("PLAY"));
    EXPECT_TRUE(wl.is_valid("WORD"));
    EXPECT_TRUE(wl.is_valid("LIST"));
}

TEST_F(WordListTest, InvalidWords) {
    WordList wl("test_wordlist.txt");
    
    EXPECT_FALSE(wl.is_valid("INVALID"));
    EXPECT_FALSE(wl.is_valid("NOTFOUND"));
    EXPECT_FALSE(wl.is_valid("MISSING"));
    EXPECT_FALSE(wl.is_valid(""));
    EXPECT_FALSE(wl.is_valid("123"));
    EXPECT_FALSE(wl.is_valid("hello")); // lowercase
}

TEST_F(WordListTest, CaseInsensitive) {
    WordList wl("test_wordlist.txt");
    
    // Test lowercase versions of valid words
    EXPECT_FALSE(wl.is_valid("hello"));
    EXPECT_FALSE(wl.is_valid("world"));
    EXPECT_FALSE(wl.is_valid("test"));
}

TEST_F(WordListTest, EdgeCases) {
    WordList wl("test_wordlist.txt");
    
    // Test single character words if any exist
    EXPECT_FALSE(wl.is_valid("A"));
    EXPECT_FALSE(wl.is_valid("I"));
    
    // Test very long invalid word
    EXPECT_FALSE(wl.is_valid("VERYLONGWORDTHATDOESNOTEXIST"));
}

TEST_F(WordListTest, EmptyFile) {
    // Create empty wordlist
    std::ofstream file("empty_wordlist.txt");
    file.close();
    
    WordList wl("empty_wordlist.txt");
    EXPECT_FALSE(wl.is_valid("HELLO"));
    EXPECT_FALSE(wl.is_valid("ANY"));
    
    std::filesystem::remove("empty_wordlist.txt");
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}