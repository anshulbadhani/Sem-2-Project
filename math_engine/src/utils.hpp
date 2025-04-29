#ifndef UTILS_HPP
#define UTILS_HPP

#include <stdexcept>
#include <iostream>

class Warning : public std::exception {
private:
    std::string msg;

public:
    Warning(const std::string &msg) {}

    const char *what() { return msg.c_str(); }
};

#endif