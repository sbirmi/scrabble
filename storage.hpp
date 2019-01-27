#ifndef __STORAGE_HPP
#define __STORAGE_HPP

#include <sqlite3.h>
#include <string>

namespace Storage {

class Inst {
   const std::string filename;
   sqlite3* db;

public:
   Inst(std::string);
   ~Inst();


};

};

#endif
