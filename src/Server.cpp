#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <poll.h>
#include <netdb.h>
#include <vector>

using namespace std;

ostream &writeString(ostream &out, string const &s) {
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

int serveClient(int client_fd);


void checkSeparator(stringstream &ss) {
    char temp[2];

    ss.read(temp, 2);
    if (strcmp(temp, "\r\n") != 0) {
        cout << "expected \\r\\n DO SOMETHING ABOUT IT!\n";
    }
}

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

RespValue *parseCmd(stringstream &ss);

RespValue *parseBulkString(stringstream &ss) {
    int len;
    ss >> len;
    checkSeparator(ss);

    string str;
    ss >> str;
    checkSeparator(ss);

    auto *val = new RespValue(RespValue::Type::BulkString);
    val->string_value = str;

    return val;
}

RespValue *parseArray(stringstream &ss) {
    int elementAmount;
    ss >> elementAmount;
    checkSeparator(ss);

    auto elements = new vector<RespValue>;

    for (int i = 0; i < elementAmount; i++) {
        auto element = parseCmd(ss);
        elements->push_back(*element);
    }

    auto val = new RespValue(RespValue::Type::Array);
    val->array_value = *elements;

    return val;
}

RespValue *parseCmd(stringstream &ss) {
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
            cout << "Message type " << firstChar << " unsupported\n";
    }

    return cmd;
}

string cmdHandler(RespValue *cmd) {
    if (cmd->type == RespValue::Type::Array &&
        cmd->array_value.size() == 1 &&
        cmd->array_value[0].string_value == "ping") {
        return {"+PONG\r\n"};
    }
    if (cmd->type == RespValue::Type::Array &&
        cmd->array_value.size() == 2 &&
        cmd->array_value[0].string_value == "echo") {
        auto echoStr = cmd->array_value[1].string_value;
        stringstream retSS;
        retSS << "$";
        retSS << echoStr.size();
        retSS << "\r\n";
        retSS << echoStr;
        retSS << "\r\n";

        return retSS.str();
    }

    return {};
}

void testParser() {
    //const char *cmdStr = "*1\r\n$4\r\nping\r\n";
    const char *cmdStr = "*2\r\n$4\r\necho\r\n$3\r\nhey\r\n";
    stringstream ss(cmdStr);
    auto cmd = parseCmd(ss);
    auto response = cmdHandler(cmd);
    cout << response << endl;
}

int main(int argc, char **argv) {
    //testParser();

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        cerr << "Failed to create server socket\n";
        return 1;
    }

    // Since the tester restarts your program quite often, setting SO_REUSEADDR
    // ensures that we don't run into 'Address already in use' errors
    int reuse = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
        cerr << "setsockopt failed\n";
        return 1;
    }


    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(6379);

    if (bind(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) != 0) {
        cerr << "Failed to bind to port 6379\n";
        return 1;
    }

    int connection_backlog = 5;
    if (listen(server_fd, connection_backlog) != 0) {
        cerr << "listen failed\n";
        return 1;
    }

    while (true) {
        struct sockaddr_in client_addr;
        int client_addr_len = sizeof(client_addr);

        cout << "Waiting for a client to connect...\n";

        int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
        cout << "Client connected\n";

        int pid = fork();

        if (pid == 0) {
            // I am a child!
            close(server_fd);

            serveClient(client_fd);

            cout << "Client disconnected\n";
            return 0; // die
        }

    }

    close(server_fd);
    return 0;
}


int serveClient(int client_fd) {
    char *inBuffer = new char[64];

    struct pollfd fd;
    fd.fd = client_fd;
    fd.events = POLLIN;

    while (true) { // conn loop
        ssize_t currReadLen;
        ssize_t readLen = 0;

        do { // msgStr loop
            if (readLen > 0) {
                int pollRet = poll(&fd, 1, 1);
                if (pollRet == -1) {
                    return 1; // socket closed
                } else if (pollRet == 0) {
                    break; // end of connection
                }
            }
            currReadLen = recv(client_fd, inBuffer + readLen, 64 - readLen, 0);
            if (currReadLen < 0) {
                cerr << "read failed " << strerror(errno) << "\n";
                return 1; // socket closed
            } else if (currReadLen == 0) {
                break; // end of connection
            }
            readLen += currReadLen;
            inBuffer[readLen] = 0;
        } while (readLen < 64);

        if (readLen == 0) {
            break;
        }

        string msgStr = string(inBuffer);

        cout << "received msgStr: ";
        writeString(cout, msgStr);
        cout << endl;

        auto ss = stringstream(msgStr);
        auto msg = parseCmd(ss);
        auto response = cmdHandler(msg);
        cout << "response: ";
        writeString(cout, response);
        cout << endl;

        send(client_fd, response.c_str(), response.size(), 0);

    }

    close(client_fd);
    delete[] inBuffer;
    return 0;
}
