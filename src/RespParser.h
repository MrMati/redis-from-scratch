#ifndef REDIS_STARTER_CPP_RESPPARSER_H
#define REDIS_STARTER_CPP_RESPPARSER_H

#include <string>
#include <deque>
#include <iostream>
#include <sstream>
#include <cstring>
#include <memory>

using namespace std;

struct RespValue {
    enum class Type {
        Invalid,
        BulkString,
        Array
    };

    explicit RespValue() : type(Type::Invalid) {}

    explicit RespValue(Type type, string val)
            : type(type), string_value(std::move(val)) {}

    explicit RespValue(Type type, unique_ptr<deque<unique_ptr<RespValue>>> vec)
            : type(type), array_value(std::move(vec)) {}

    Type type;

    string string_value;
    unique_ptr<deque<unique_ptr<RespValue>>> array_value;
};


class RespParser {
public:
    unique_ptr<RespValue> parseCmd(stringstream &ss);

private:
    void checkSeparator(stringstream &ss);

    unique_ptr<RespValue> parseBulkString(stringstream &ss);

    unique_ptr<RespValue> parseArray(stringstream &ss);


};


#endif //REDIS_STARTER_CPP_RESPPARSER_H
