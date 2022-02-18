//
// Created by blacksungrass on 2021/12/20.
//

#include "file_response.h"
#include <sys/stat.h>
#include <bits/stdc++.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
using namespace std;

file_response::file_response() = default;

file_response::file_response(const file_response &other):response(other) {
    m_file_name = other.m_file_name;
    m_file_length = other.m_file_length;
    m_file_address = other.m_file_address;
}

static size_t get_file_length(const string& file_name){
    struct stat st{};
    stat(file_name.c_str(), &st);
    return st.st_size;
}

void file_response::set_content(const string &s){
    //todo set root dir from server class
    if(m_packed){
        throw logic_error("can not modify a packed response");
    }
    m_file_name = s;

    if(m_file_address!=nullptr){
        munmap(m_file_address,m_file_length);
    }
    m_file_length = get_file_length(m_file_name);
    int fd = open(m_file_name.c_str(),O_RDONLY);
    m_file_address = mmap(nullptr,m_file_length,PROT_READ,MAP_PRIVATE,fd,0);
}



file_response::~file_response() {
    if(m_file_address!=nullptr){
        munmap(m_file_address,m_file_length);
        m_file_address = nullptr;
    }
}

void file_response::pack() {
    if(m_packed){
        throw logic_error("can not pack a response twice");
    }
    string val;
    if(m_file_address==nullptr){
        val = "0";
    }
    else{
        val = to_string(m_file_length);
    }

    auto it = m_headers.find("content-length");
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
    m_vec[1].iov_base = m_file_address;
    m_vec[1].iov_len = m_file_length;
    m_packed = true;
}
