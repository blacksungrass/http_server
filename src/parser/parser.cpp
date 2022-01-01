//
// Created by blacksungrass on 2021/12/19.
//

#include "parser.h"

#include <bits/stdc++.h>
using namespace std;

bool parser::parse(const char *data, size_t len) {
    if(m_failure)
        return false;
    m_buffer.append(data,len);//todo 加入对buffer长度的限制
    const char* p = m_buffer.c_str();
    len = m_buffer.size();
    while(true){
        size_t n = m_cur_request.parse(p,len);
        if(m_cur_request.failure()){
            m_failure = true;
            return false;
        }
        p += n;
        len -= n;
        if(m_cur_request.finished()){
            m_requests.emplace_back();
            swap(m_requests.back(),m_cur_request);
        }
        else{
            m_buffer.erase(begin(m_buffer),begin(m_buffer)+(p-m_buffer.c_str()));
            break;
        }
    }
    return true;
}

bool parser::has_results() const {
    return !m_requests.empty();
}

vector<request> parser::get_results() {
    vector<request> ret;
    swap(ret,m_requests);
    return ret;
}

parser::parser() {
    m_failure = false;
}
