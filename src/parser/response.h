//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_RESPONSE_H
#define TEST_RESPONSE_H

#include <string>
#include <map>

class response {
private:
    std::string m_version;
    int m_status_code = 200;
    std::string m_status_string;
    std::map<std::string,std::string> m_headers;
    std::string m_content;
    bool m_close_connection = false;
public:
    response() = default;
    void set_version(const std::string& version);
    void set_status_code(int status_code);
    void set_status_string(const std::string& s);
    void add_header(const std::string& key,const std::string& value);
    void append_content(const std::string& s);
    virtual std::string to_string() const;
    bool close_connection() const;
    void set_close_connection(bool);
};


#endif //TEST_RESPONSE_H
