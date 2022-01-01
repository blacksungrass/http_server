//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_FILE_RESPONSE_H
#define TEST_FILE_RESPONSE_H

#include "response.h"

class file_response : public response{
private:
    std::string m_file_path;
    std::size_t m_file_length;
public:
    file_response();
    void set_file(const std::string& file_path);
    std::string to_string() const override;
};


#endif //TEST_FILE_RESPONSE_H
