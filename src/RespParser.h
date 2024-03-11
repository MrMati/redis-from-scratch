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

    explicit RespValue() : type(Type::Invalid) {
        cout << "RespValue created\n";
    }
    explicit RespValue(Type type, string val) : type(type), string_value(std::move(val)) {
        cout << "RespValue created\n";
    }
    explicit RespValue(Type type, vector<RespValue> &vec) : type(type), array_value(std::move(vec)) {
        cout << "RespValue created\n";
    }

    ~RespValue() {
        cout << "RespValue ded\n";
    }

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
