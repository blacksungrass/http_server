//
// Created by blacksungrass on 2022/1/16.
//

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include <sys/time.h>

bool operator<(const timeval& a,const timeval& b);
bool operator>(const timeval& a,const timeval& b);

#endif //TEST_TIMER_H
