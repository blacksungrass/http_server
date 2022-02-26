//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_RESPONSE_H
#define TEST_RESPONSE_H

#include <string>
#include <map>
#include <sys/uio.h>

class response {
protected:
    std::string m_version;
    int m_status_code = 200;
    std::string m_status_string;
    std::map<std::string,std::string> m_headers;
    std::string m_content;
    bool m_packed = false;
    std::string m_header_content;
    struct iovec* m_vec = nullptr;
    bool m_keep_alive = true;
public:
    response() = default;
    response(const response&);
    void set_version(const std::string& version);
    void set_status_code(int status_code);
    void set_status_string(const std::string& s);
    void add_header(const std::string& key,const std::string& value);
    bool is_keep_alive() const;
    void set_keep_alive(bool);
    virtual void set_content(const std::string& s);
    virtual void pack();
    /*
     * return 0:sent all data, done
     * return -1:encounter error, close connection
     * return 1:sent part of data, handle next time
     */
    int send(int);
    ~response();
};


#endif //TEST_RESPONSE_H
