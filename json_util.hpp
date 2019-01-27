#ifndef __JSON_UTIL_HPP
#define __JSON_UTIL_HPP

#include <json/reader.h>
#include <json/value.h>
#include <json/writer.h>

Json::Value jsonify(int);
Json::Value jsonify(const std::string&);
Json::Value jsonify(const std::string&, const std::string&);
Json::Value jsonify(const std::string&, const std::string&,
                    const std::string&);
Json::Value jsonify(const char letter, unsigned int row,
                    unsigned int col);
Json::Value jsonify(const std::string&, const std::string&,
                    const std::string&, const unsigned int);

#endif
