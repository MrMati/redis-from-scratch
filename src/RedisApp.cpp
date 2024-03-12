#include "RedisApp.h"

#include <utility>

using namespace std;

ostream &writeEscapedString(ostream &out, string const &s) {
    for (auto ch: s) {
        switch (ch) {
            case '\'':
                out << "\\'";
                break;

            case '\"':
                out << "\\\"";
                break;

            case '\?':
                out << "\\?";
                break;

            case '\\':
                out << "\\\\";
                break;

            case '\a':
                out << "\\a";
                break;

            case '\b':
                out << "\\b";
                break;

            case '\f':
                out << "\\f";
                break;

            case '\n':
                out << "\\n";
                break;

            case '\r':
                out << "\\r";
                break;

            case '\t':
                out << "\\t";
                break;

            case '\v':
                out << "\\v";
                break;

            default:
                out << ch;
        }
    }

    return out;
}


string RedisApp::echoHandler(string &val) {
    return serialize.bulkString(val);
}

string RedisApp::getHandler(string &key) {
    if (dataStore.contains(key))
        return serialize.bulkString(dataStore[key]);
    return serialize.nilString;
}

string RedisApp::setHandler(string &key, string &value) {
    dataStore[key] = value;
    return serialize.OK;
}

string RedisApp::cmdHandler(unique_ptr<RespValue> cmd) {
    if (cmd->type != RespValue::Type::Array) return {};

    if (cmd->array_value->size() == 1 &&
            cmd->array_value->at(0)->string_value == "ping") {
        return {"+PONG\r\n"};
    }
    if (cmd->array_value->size() == 2 &&
        cmd->array_value->at(0)->string_value == "echo") {
        return echoHandler(cmd->array_value->at(1)->string_value);
    }
    if (cmd->array_value->size() >= 2 &&
        cmd->array_value->at(0)->string_value == "get") {
        return getHandler(cmd->array_value->at(1)->string_value);
    }
    if (cmd->array_value->size() >= 3 &&
        cmd->array_value->at(0)->string_value == "set") {
        return setHandler(cmd->array_value->at(1)->string_value, cmd->array_value->at(2)->string_value);
    }

    return {};
}

int RedisApp::start() {
    int server_fd = createListener();
    if (server_fd < 0) return server_fd;

    cout << "Server started\n";

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    while (true) {
        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
        cout << "Client connected\n";

        thread t = thread(&RedisApp::handleClient, this, client_fd);
        t.detach();
    }
    close(server_fd);
}

int RedisApp::createListener() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Failed to create server socket\n";
        return -1;
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        cerr << "setsockopt failed\n";
        return -1;
    }


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        cerr << "Failed to bind to port 6379\n";
        return -1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        cerr << "listen failed\n";
        return -1;
    }

    return server_fd;
}

void RedisApp::handleClient(int client_fd) {
    char *inBuffer = new char[64];

    struct pollfd fd;
    fd.fd = client_fd;
    fd.events = POLLIN;

    bool run = true;

    while (run) { // conn loop
        ssize_t currReadLen;
        ssize_t readLen = 0;

        do { // msgStr loop
            if (readLen > 0) {
                int pollRet = poll(&fd, 1, 1);
                if (pollRet == 0) { // end of data
                    break;
                }
            }
            currReadLen = recv(client_fd, inBuffer + readLen, 64 - readLen, 0);
            if (currReadLen < 0) {  // socket closed
                run = false;
                break;
            } else if (currReadLen == 0) {  // end of data
                break;
            }
            readLen += currReadLen;
            inBuffer[readLen] = 0;
        } while (readLen < 64);

        if (readLen == 0) { // no data received
            break;
        }

        string msgStr = string(inBuffer);

        cout << "received msg: ";
        writeEscapedString(cout, msgStr);
        cout << endl;

        auto ss = stringstream(msgStr);
        auto msg = parser.parseCmd(ss);
        auto response = cmdHandler(std::move(msg));
        cout << "response: ";
        writeEscapedString(cout, response);
        cout << endl;

        send(client_fd, response.c_str(), response.size(), 0);
    }

    close(client_fd);
    delete[] inBuffer;
}