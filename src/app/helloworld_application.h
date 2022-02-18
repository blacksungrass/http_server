//
// Created by blacksungrass on 2021/12/31.
//

#ifndef TEST_HELLOWORLD_APPLICATION_H
#define TEST_HELLOWORLD_APPLICATION_H

#include "application.h"


class helloworld_application:public application {
public:
    response* handle_request(const request& req) override;
};


#endif //TEST_HELLOWORLD_APPLICATION_H
