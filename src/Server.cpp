#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

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

int main(int argc, char **argv) {
    // You can use print statements as follows for debugging, they'll be visible when running tests.
    cout << "Logs from your program will appear here!\n";


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

    /*struct timeval t;
    t.tv_sec = 0;
    t.tv_usec = 0;
    if (setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &t, sizeof(t))) {
        cerr << "setsockopt failed\n";
        return 1;
    }*/

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

    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);

    cout << "Waiting for a client to connect...\n";

    int client_fd = accept(server_fd, (struct sockaddr *) &client_addr, (socklen_t *) &client_addr_len);
    cout << "Client connected\n";

    char *inBuffer = new char[64];

    while (true) {
        ssize_t currReadLen;
        ssize_t readLen = 0;
        int msg = 0;

        do {
            currReadLen = recv(client_fd, inBuffer + readLen, 64 - readLen, 0);
            if (currReadLen < 0) {
                cerr << "read failed " << strerror(errno) << "\n";
                return 1;
            } else if (currReadLen == 0) {
                // client disconnected, keeping msg=0
                break;
            }
            readLen += currReadLen;
            inBuffer[readLen] = 0;

            if (strcmp(inBuffer, "*1\r\n$4\r\nping\r\n") == 0) {
                msg = 1;
                break;
            }

        } while (readLen < 64);

        if(msg == 0) { // msg==0 means client disconnected
                break;
        }

        cout << "received msg " << msg << ": ";
        writeString(cout, string(inBuffer));
        cout << endl;

        inBuffer[0] = 0;

        if (msg == 1) {
            const char *resp = "+PONG\r\n";
            send(client_fd, resp, strlen(resp), 0);
        }
    }

    close(server_fd);

    delete[] inBuffer;

    return 0;
}
