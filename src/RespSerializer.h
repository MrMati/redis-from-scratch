#ifndef REDIS_STARTER_CPP_RESPSERIALIZER_H
#define REDIS_STARTER_CPP_RESPSERIALIZER_H

#include <string>
#include <sstream>

using namespace std;

class RespSerializer {
public:
    string nilString = "$-1\r\n";
    string OK = "+OK\r\n";

    string simpleString(const string& str);
    string bulkString(const string& str);
};


#endif //REDIS_STARTER_CPP_RESPSERIALIZER_H
