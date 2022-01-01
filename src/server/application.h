//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#include "../parser/request.h"
#include "../parser/response.h"

/*
 * application是一个接口，只需要继承application并实现handle_request函数即可
 */
class application {
public:
    virtual response handle_request(const request& req) = 0;
};


#endif //TEST_APPLICATION_H
