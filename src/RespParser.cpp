#include "RespParser.h"

void RespParser::checkSeparator(stringstream &ss) {
    char temp[3]{};
    ss.read(temp, 2);
    if (strcmp(temp, "\r\n") != 0) {
        cout << "expected \\r\\n DO SOMETHING ABOUT IT!\n";
    }
}


unique_ptr<RespValue> RespParser::parseBulkString(stringstream &ss) {
    int len;
    ss >> len;
    checkSeparator(ss);

    string str;
    ss >> str;
    checkSeparator(ss);

    return make_unique<RespValue>(RespValue::Type::BulkString, str);
}

unique_ptr<RespValue> RespParser::parseArray(stringstream &ss) {
    int elementAmount;
    ss >> elementAmount;
    checkSeparator(ss);

    auto elements = make_unique<deque<unique_ptr<RespValue>>>();

    for (int i = 0; i < elementAmount; i++) {
        auto element = parseCmd(ss);
        elements->push_back(std::move(element));
    }

    return make_unique<RespValue>(RespValue::Type::Array, std::move(elements));
}

unique_ptr<RespValue> RespParser::parseCmd(stringstream &ss) {
    char firstChar;
    ss >> firstChar;

    unique_ptr<RespValue> cmd;

    switch (firstChar) {
        case '*':
            cmd = parseArray(ss);
            break;
        case '$':
            cmd = parseBulkString(ss);
            break;
        default:
            cmd = make_unique<RespValue>();
            cout << "Message type " << firstChar << " unsupported\n";
            break;
    }

    return cmd;
}