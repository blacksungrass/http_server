//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_PARSER_H
#define TEST_PARSER_H

#include <vector>
#include "request.h"

class parser {
private:
    bool m_failure;
    request m_cur_request;
    std::vector<request> m_requests;
    std::string m_buffer;
public:
    parser();
    bool parse(const char* data,std::size_t len);
    bool has_results() const;
    std::vector<request> get_results();
};


#endif //TEST_PARSER_H
