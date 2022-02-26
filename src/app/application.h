//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_APPLICATION_H
#define TEST_APPLICATION_H

#include "request.h"
#include "response.h"

/*
 * application是一个接口，只需要继承application并实现handle_request函数即可
 */
class application {
private:
    virtual response* m_handle_request(const request& req);
public:
    response* handle_request(const request& req);

};


#endif //TEST_APPLICATION_H
