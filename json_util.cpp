#include "json_util.hpp"

Json::Value
jsonify(int i) {
   return Json::Value(i);
}

Json::Value
jsonify(const std::string& s1) {
   Json::Value json;
   json.append(Json::Value(s1));
   return json;
}

Json::Value
jsonify(const std::string& s1, const std::string& s2) {
   Json::Value json;
   json.append(Json::Value(s1));
   json.append(Json::Value(s2));
   return json;
}

Json::Value
jsonify(const std::string& s1, const std::string& s2,
              const std::string& s3) {
   Json::Value json;
   json.append(Json::Value(s1));
   json.append(Json::Value(s2));
   json.append(Json::Value(s3));
   return json;
}

Json::Value
jsonify(const char letter, unsigned int row, unsigned int col) {
   Json::Value json;
   std::string s=" ";
   s[0] = letter;
   json.append(Json::Value(s));
   json.append(Json::Value(row));
   json.append(Json::Value(col));
   return json;
}

Json::Value
jsonify(const std::string& s1, const std::string& s2,
        const std::string& s3, const unsigned int i) {
   Json::Value json = jsonify(s1);
   json.append(Json::Value(s2));
   json.append(Json::Value(s3));
   json.append(Json::Value(i));
   return json;
}

Json::Value
jsonify(const unsigned int moveType,
        const std::string& longestWord,
        const int score) {
   Json::Value json;
   json.append(Json::Value(moveType));
   json.append(Json::Value(longestWord));
   json.append(Json::Value(score));
   return json;
}

