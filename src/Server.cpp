#include <iostream>
#include <string>

#include "RespParser.h"
#include "RedisApp.h"
#include "utils.h"

using namespace std;

void testParser() {
    const char *cmdStr = "*1\r\n$4\r\nping\r\n";
    //const char *cmdStr = "*3\r\n$3\r\nset\r\n$3\r\nkot\r\n$4\r\npies\r\n";
    stringstream ss(cmdStr);
    auto parser = RespParser();
    auto app = RedisApp();

    auto cmd = parser.parseCmd(ss);
    auto response = app.cmdHandler(std::move(cmd));
    writeEscapedString(cout, response);
}

int main(int argc, char **argv) {
    //testParser();
    //return 0;

    RedisApp app;
    return app.start();
}