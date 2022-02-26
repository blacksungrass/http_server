//
// Created by blacksungrass on 2022/2/13.
//

#include "html_application.h"
#include "../parser/file_response.h"
#include <string>
#include <unistd.h>
using namespace std;

html_application::html_application(const string& dir)
:m_root_dir(dir){

}

response* get_404_response(){
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

response* html_application::m_handle_request(const request& req){
    string filename = req.m_url;
    int pos = filename.find('/');
    if(pos==string::npos||pos== filename.size()-1)
        filename = "index.html";
    else
        filename = filename.substr(pos+1);
    //todo check file existance ,if not exist, return 404
    filename = m_root_dir+filename;
    if(access(filename.c_str(),R_OK)){
        return get_404_response();
    }
    auto *ret = new file_response();
    auto& resp = *ret;
    resp.set_version("HTTP/1.1");
    resp.set_status_code(200);
    resp.set_status_string("OK");
    resp.add_header("server","hello world server");
    resp.add_header("content-type","text/html");
    resp.add_header("connection","keep-alive");
    resp.set_content(filename);
    resp.pack();
    return ret;
}