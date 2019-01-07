#include <iostream>

#include "word_list.hpp"

void
WordList::push(const std::string& word) {
   unsigned int idx = word[0] - 'A';
   assert(idx >= 0 && idx < 26);

   words[idx].insert(word);
}

WordList::WordList(std::string _filename) : filename(_filename) {
   std::string line;
   std::ifstream infile(filename);

   while (infile >> line) {
      push(line);
   }
}

bool
WordList::is_valid(const std::string& word) const {
   if (word.size() == 0 || word.size() > 15) {
      return false;
   }

   if (!isupper(word[0])) {
      return false;
   }

   int idx=word[0] - 'A';

   if (idx < 0 || idx >= 26) {
      return false;
   }

   return words[idx].find(word) != words[idx].end();
}
