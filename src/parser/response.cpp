//
// Created by blacksungrass on 2021/12/19.
//

#include "response.h"
#include "../util/util.h"
#include <bits/stdc++.h>
using namespace std;

void response::set_status_code(int status_code) {
    m_status_code = status_code;
}

void response::set_version(const std::string &version) {
    m_version = version;
}

void response::set_status_string(const std::string &s) {
    m_status_string = s;
}

void response::add_header(const std::string &key, const std::string &value) {
    string tmp(key);
    tolower(tmp);
    m_headers[tmp] = value;
}

void response::append_content(const std::string &s) {
    m_content.append(s);
    auto it = m_headers.find("content-length");
    string val = std::to_string(m_content.size());
    if(it==m_headers.end()){
        m_headers["content-length"] = val;
    }
    else{
        it->second = val;
    }
}

string response::to_string() const {
    stringstream ss;
    ss<<m_version<<" "<<m_status_code<<" "<<m_status_string<<"\r\n";
    for(const auto&[k,v]:m_headers){
        ss<<k<<":"<<v<<"\r\n";
    }
    ss<<"\r\n";
    ss<<m_content;
    return ss.str();
}

bool response::close_connection() const{
    return m_close_connection;
}

void response::set_close_connection(bool close){
    m_close_connection = close;
}