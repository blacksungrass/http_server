//
// Created by blacksungrass on 2021/12/19.
//

#include "response.h"
#include "../util/util.h"
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <logger.h>

using namespace std;

void response::set_status_code(int status_code) {
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    m_status_code = status_code;
}

void response::set_version(const std::string &version) {
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    m_version = version;
}

void response::set_status_string(const std::string &s) {
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    m_status_string = s;
}

void response::add_header(const std::string &key, const std::string &value) {
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    string tmp(key);
    tolower(tmp);
    m_headers[tmp] = value;
}

void response::set_content(const std::string &s) {
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    if(s.empty())
        return;
    m_content.append(s);

}

void response::pack(){
    if(m_packed){
        throw logic_error("can not pack a response twice");
    }
    auto it = m_headers.find("content-length");
    string val = std::to_string(m_content.size());
    if(it==m_headers.end()){
        m_headers["content-length"] = val;
    }
    else{
        it->second = val;
    }
    stringstream ss;
    ss<<m_version<<" "<<m_status_code<<" "<<m_status_string<<"\r\n";
    for(const auto&[k,v]:m_headers){
        ss<<k<<":"<<v<<"\r\n";
    }
    ss<<"\r\n";
    m_header_content = ss.str();
    m_vec = new iovec[2];
    m_vec[0].iov_base = m_header_content.data();
    m_vec[0].iov_len = m_header_content.size();
    m_vec[1].iov_base = m_content.data();
    m_vec[1].iov_len = m_content.size();
    m_packed = true;
}


int response::send(int sockfd){
    if(!m_packed){
        throw logic_error("response needed to be packed before be sent");
    }
    ssize_t r = writev(sockfd,m_vec,2);
    if(r==0){
        INFO("connection at sock=%d closed by client",sockfd);
        return -1;
    }

    if(r<0){
        if(errno==EWOULDBLOCK||errno==EAGAIN)
            return 1;
        else{
            INFO("response.send errno=%s",strerror(errno));
            return -1;
        }

    }

    if(r<=m_vec[0].iov_len){
        m_vec[0].iov_base += r;
        m_vec[0].iov_len -= r;
    }
    else{
        r -= m_vec[0].iov_len;
        m_vec[0].iov_base += m_vec[0].iov_len;
        m_vec[0].iov_len = 0;
        m_vec[1].iov_base += r;
        m_vec[1].iov_len -= r;
    }
    if(m_vec[0].iov_len==0&&m_vec[1].iov_len==0)
        return 0;
    else
        return 1;
}

response::~response(){
    if(m_packed&&m_vec!=nullptr){
        delete[] m_vec;
        m_vec = nullptr;
    }
}

response::response(const response &other) {
    m_version = other.m_version;
    m_status_code = other.m_status_code;
    m_status_string = other.m_status_string;
    m_headers = other.m_headers;
    m_content = other.m_content;
    if(other.m_packed){
        m_packed = other.m_packed;
        m_header_content = other.m_header_content;
        m_vec = new iovec[2];
        m_vec[0].iov_base = m_header_content.data();
        m_vec[0].iov_len = m_header_content.size();
        m_vec[1].iov_base = m_content.data();
        m_vec[1].iov_len = m_content.size();
    }
}

bool response::is_keep_alive() const {
    return m_keep_alive;
}

void response::set_keep_alive(bool b) {
    m_keep_alive = b;
}
