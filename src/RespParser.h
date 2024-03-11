#ifndef REDIS_STARTER_CPP_RESPPARSER_H
#define REDIS_STARTER_CPP_RESPPARSER_H

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <cstring>

using namespace std;

struct RespValue {
    enum class Type {
        Invalid,
        BulkString,
        Array
    };

    explicit RespValue(Type type) : type(type) {}

    Type type;

    string string_value;
    vector<RespValue> array_value;
};


class RespParser {
public:
    RespValue *parseCmd(stringstream &ss);

private:
    void checkSeparator(stringstream &ss);

    RespValue *parseBulkString(stringstream &ss);

    RespValue *parseArray(stringstream &ss);


};


#endif //REDIS_STARTER_CPP_RESPPARSER_H
