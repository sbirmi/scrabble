#include <cassert>
#include <iostream>

#include "storage.hpp"

namespace Storage {

Inst::Inst(const std::string _filename) : filename(_filename) {
   int rc;

   rc = sqlite3_open(filename.c_str(), &db);

   if (rc) {
      std::cerr << "Can't open " << filename << std::endl;
      assert(0 && "Can't open filename");
   }
}

Inst::~Inst() {
   sqlite3_close(db);
}

}; // namespace Storage
