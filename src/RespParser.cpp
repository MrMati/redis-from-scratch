#include "RespParser.h"

void RespParser::checkSeparator(stringstream &ss) {
    char temp[3]{};
    ss.read(temp, 2);
    if (strcmp(temp, "\r\n") != 0) {
        cout << "expected \\r\\n DO SOMETHING ABOUT IT!\n";
    }
}


RespValue *RespParser::parseBulkString(stringstream &ss) {
    int len;
    ss >> len;
    checkSeparator(ss);

    string str;
    ss >> str;
    checkSeparator(ss);

    auto *val = new RespValue(RespValue::Type::BulkString, str);
    return val;
}

RespValue *RespParser::parseArray(stringstream &ss) {
    int elementAmount;
    ss >> elementAmount;
    checkSeparator(ss);

    vector<RespValue> elements;
    cout << "pies\n";

    for (int i = 0; i < elementAmount; i++) {
        auto element = parseCmd(ss);
        elements.push_back(*element);
    }

    auto val = new RespValue(RespValue::Type::Array, elements);
    return val;
}

RespValue *RespParser::parseCmd(stringstream &ss) {
    char firstChar;
    ss >> firstChar;

    RespValue *cmd;

    switch (firstChar) {
        case '*':
            cmd = parseArray(ss);
            break;
        case '$':
            cmd = parseBulkString(ss);
            break;
        default:
            cmd = new RespValue();
            cout << "Message type " << firstChar << " unsupported\n";
            break;
    }

    return cmd;
}