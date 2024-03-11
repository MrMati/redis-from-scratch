#include "RespSerializer.h"


string RespSerializer::simpleString(const string &str) {
    auto ss = stringstream("+");
    ss << str;
    ss << "\r\n";

    return ss.str();
}

string RespSerializer::bulkString(const string &str) {
    stringstream ss;
    ss << "$";
    ss << str.size();
    ss << "\r\n";
    ss << str;
    ss << "\r\n";

    return ss.str();
}


