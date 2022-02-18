//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_REQUEST_H
#define TEST_REQUEST_H

#include <string>
#include <map>

enum class http_method{
    GET,POST
};
class request {
private:
    enum class parse_state{
        FIRST_LINE,HEADERS,CONTENT
    };
private:
    parse_state m_parse_state;
    std::size_t m_data_len;
    bool m_failure,m_finished;

    std::size_t parse_first_line(const char* data,int len);
    std::size_t parse_headers(const char* data,int len);
    std::size_t parse_content(const char* data,int len);
public:
    http_method m_method;
    std::string m_url;
    std::string m_version;
    std::map<std::string,std::string> m_headers;
    std::string m_data;
public:
    request();
    request(request&& o) noexcept ;
    request& operator=(request&& o) noexcept;
    std::size_t parse(const char* data,std::size_t len);
    bool failure() const;
    bool finished() const;
    std::string to_string() const;
    bool operator==(const request& o) const;
};



#endif //TEST_REQUEST_H
