#ifndef REDIS_STARTER_CPP_UTILS_H
#define REDIS_STARTER_CPP_UTILS_H

#include <ostream>
#include <deque>
#include <string>
#include <memory>
#include <cctype>
#include <algorithm>

using namespace std;

bool cmpCIgnoreCase(char a, char b);
bool cmpIgnoreCase(const std::string& a, const std::string& b);


ostream &writeEscapedString(ostream &out, string const &s);

template<typename T>
T pop(unique_ptr<deque<T>> &q) {
    T e = std::move(q->front());
    q.get()->pop_front();
    return  (e);
}
#endif //REDIS_STARTER_CPP_UTILS_H
