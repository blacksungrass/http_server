//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <string>
#include <fcntl.h>

void tolower(std::string&);

void toupper(std::string&);

void make_nonblocking(int fd);

#endif //TEST_UTIL_H


