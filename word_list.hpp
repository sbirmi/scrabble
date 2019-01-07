#ifndef __WORD_LIST_HPP
#define __WORD_LIST_HPP

#include <assert.h>
#include <fstream>
#include <set>
#include <string>

typedef std::set<std::string> Words;

class WordList {
private:
   const std::string filename;
   Words words[26];
   void push(const std::string&);

public:
   WordList(std::string _filename);
   bool is_valid(const std::string&) const;
};

#endif
