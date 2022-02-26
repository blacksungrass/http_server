//
// Created by blacksungrass on 2022/2/13.
//

#ifndef TEST_HTML_APPLICATION_H
#define TEST_HTML_APPLICATION_H

#include "application.h"

class html_application:public application {
public:
    explicit html_application(const std::string&);
private:
    response* m_handle_request(const request& req) override;
    std::string m_root_dir;
};


#endif //TEST_HTML_APPLICATION_H
