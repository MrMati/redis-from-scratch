#ifndef REDIS_STARTER_CPP_REDISAPP_H
#define REDIS_STARTER_CPP_REDISAPP_H

#include <iostream>
#include <memory>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <utility>
#include <vector>
#include <thread>
#include <map>

#include "RespParser.h"
#include "RespSerializer.h"


struct RedisValue {
    string value;
    time_t expiryTime;

    RedisValue(string val, time_t expiry) : value(std::move(val)), expiryTime(expiry) {}
};


class RedisApp {
public:
    string cmdHandler(RespValue *cmd);

    int start();

private:
    RespParser parser{};
    RespSerializer serialize{};
    map<string, string> dataStore{};

    int createListener();
    void handleClient(int client_fd);

    string echoHandler(string &val);
    string getHandler(string &key);
    string setHandler(string &key, string &value);
};


#endif //REDIS_STARTER_CPP_REDISAPP_H
