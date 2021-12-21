//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_HTML_APPLICATION_H
#define TEST_HTML_APPLICATION_H

#include "application.h"

class html_application:public application {
    virtual response handle_request(const request& req) override;
};


#endif //TEST_HTML_APPLICATION_H
