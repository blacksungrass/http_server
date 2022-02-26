//
// Created by blacksungrass on 2021/12/20.
//

#include "application.h"
#include "../parser/response.h"
#include "../parser/request.h"
#include <iostream>
#include <exception>


response* application::m_handle_request(const request& req){
    throw std::logic_error("can not use application class directly");
    return nullptr;
}

response* application::handle_request(const request& req){
    response* ret = this->m_handle_request(req);
    if(req.m_version=="HTTP/1.0"){
        auto it = req.m_headers.find("keep-alive");
        if(it!=end(req.m_headers)&&it->second=="close"){
            ret->set_keep_alive(false);
        }
    }
    return ret;
}