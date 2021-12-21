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
    int m_status_code;
    std::string m_status_string;
    std::map<std::string,std::string> m_headers;
    std::string m_content;
public:
    response() = default;
    void set_version(const string& version);
    void set_status_code(int status_code);
    void set_status_string(const string& s);
    void add_header(const string& key,const string& value);
    void append_content(const string& s);
    virtual string& to_string() const;
};


#endif //TEST_RESPONSE_H
