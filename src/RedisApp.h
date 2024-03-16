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
#include <deque>
#include <thread>
#include <map>
#include <chrono>

#include "RespParser.h"
#include "RespSerializer.h"
#include "utils.h"

using seconds = long;

struct RedisValue {
    string value;
    seconds expiryTime;

    RedisValue(string val, seconds expiry) : value(std::move(val)), expiryTime(expiry) {}
};



class RedisApp {
public:
    string cmdHandler(unique_ptr<RespValue> cmd);

    int start();

private:
    RespParser parser{};
    RespSerializer serialize{};
    map<string, unique_ptr<RedisValue>> dataStore{};

    int createListener();
    void handleClient(int client_fd);

    string echoHandler(string &val);
    string getHandler(string &key);
    string setHandler(unique_ptr<deque<unique_ptr<RespValue>>> args);
};


#endif //REDIS_STARTER_CPP_REDISAPP_H
