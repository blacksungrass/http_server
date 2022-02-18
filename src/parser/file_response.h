//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_FILE_RESPONSE_H
#define TEST_FILE_RESPONSE_H

#include "response.h"

class file_response : public response{
private:
    std::string m_file_name;
    std::size_t m_file_length = 0;
    void* m_file_address = nullptr;
public:
    file_response();
    file_response(const file_response&);
    void set_content(const std::string& s) override;
    void pack() override;
    ~file_response();
};


#endif //TEST_FILE_RESPONSE_H
