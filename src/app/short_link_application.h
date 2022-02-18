//
// Created by blacksungrass on 2022/2/13.
//

#ifndef TEST_SHORT_LINK_APPLICATION_H

#define TEST_SHORT_LINK_APPLICATION_H

#include "application.h"
#include <map>
#include <mutex>
#include <atomic>

class short_link_application:public application {
public:
    explicit short_link_application(const std::string&);
    response* handle_request(const request& req) override;
private:
    std::map<std::string,std::string> m_map;
    std::atomic<int> m_cur_id;
    std::mutex m_mutex;
    std::map<std::string,std::string> m_inv_map;
    std::string m_convert_table;
    std::string m_root_dir;

    std::string generate_key();
    response* get_key_from_link(const request& req);
    response* get_link_from_key(const request& req);
};


#endif //TEST_SHORT_LINK_APPLICATION_H
