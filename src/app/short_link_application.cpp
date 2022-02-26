//
// Created by blacksungrass on 2022/2/13.
//

#include "short_link_application.h"
#include "../parser/file_response.h"
using namespace std;

/*
 * 客户端数据有错误
 */
static response* client_data_error(){
    //todo
    auto* ret = new response();
    ret->set_version("HTTP/1.1");
    ret->set_status_code(404);
    ret->set_status_string("FILE NOT FOUND");
    ret->add_header("server","hello world server");
    ret->add_header("content-type","text/html");
    ret->add_header("connection","keep-alive");
    ret->set_content("<h1>File Not Found!</h1>");
    ret->pack();
    return ret;
}

/*
 * 不支持的http method
 */
static response* method_not_support(){
    //todo
    auto* ret = new response();
    ret->set_version("HTTP/1.1");
    ret->set_status_code(404);
    ret->set_status_string("FILE NOT FOUND");
    ret->add_header("server","hello world server");
    ret->add_header("content-type","text/html");
    ret->add_header("connection","keep-alive");
    ret->set_content("<h1>File Not Found!</h1>");
    ret->pack();
    return ret;
}

short_link_application::short_link_application(const string& dir)
:m_root_dir(dir){
    for(int i=0;i<10;++i){
        m_convert_table.push_back('0'+i);
    }
    for(int i=0;i<26;++i){
        m_convert_table.push_back('a'+i);
        m_convert_table.push_back('A'+i);
    }
}

std::string short_link_application::generate_key(){
    string ret("0000000");
    int id = m_cur_id.fetch_add(1);
    int n = m_convert_table.size();
    for(int i=6;i>=0;--i){
        ret[i] = m_convert_table[id%n];
        id /= n;
    }
    return ret;
}

response* short_link_application::get_link_from_key(const request& req){
    if(req.m_url.size()<=1)
        return client_data_error();
    string key = req.m_url.substr(1);
    unique_lock lock(m_mutex);
    if(m_map.count(key)){
        string link = m_map[key];
        lock.unlock();
        auto* ret = new response();
        ret->set_version("HTTP/1.1");
        ret->set_status_code(302);
        ret->set_status_string(link);
        ret->add_header("server","hello world server");
        ret->add_header("content-type","text/html");
        ret->add_header("connection","keep-alive");
        ret->pack();
        return ret;
    }
    else{
        lock.unlock();
        return client_data_error();
    }
}

response* short_link_application::get_key_from_link(const request& req){
    auto p1 = req.m_data.find(':');
    auto p2 = req.m_data.find('}');
    if(p1==string::npos||p2==string::npos)
        return client_data_error();
    string link = req.m_data.substr(p1+1,p2-p1-1);
    if(link.empty())
        return client_data_error();
    string key;
    unique_lock lock(m_mutex);
    if(m_inv_map.count(link)){
        key = m_inv_map[link];
    }
    else{
        key = generate_key();
        m_map[key] = link;
        m_inv_map[link] = key;
    }
    lock.unlock();
    auto* ret = new response();
    ret->set_version("HTTP/1.1");
    ret->set_status_code(200);
    ret->set_status_string("OK");
    ret->add_header("server","hello world server");
    ret->add_header("content-type","text/html");
    ret->add_header("connection","keep-alive");
    ret->set_content(key);
    ret->pack();
    return ret;
}



response* short_link_application::m_handle_request(const request& req){
    if(req.m_method==http_method::GET){
        if(req.m_url.size()>1)
            return get_link_from_key(req);
        else{
            //show index
            auto *ret = new file_response();
            auto& resp = *ret;
            resp.set_version("HTTP/1.1");
            resp.set_status_code(200);
            resp.set_status_string("OK");
            resp.add_header("server","hello world server");
            resp.add_header("content-type","text/html");
            resp.add_header("connection","keep-alive");
            resp.set_content(m_root_dir+"index.html");
            resp.pack();
            return ret;
        }
    }
    else if(req.m_method==http_method::POST){
        return get_key_from_link(req);
    }
    return method_not_support();
}