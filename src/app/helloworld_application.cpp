//
// Created by blacksungrass on 2021/12/31.
//

#include "helloworld_application.h"
#include <bits/stdc++.h>
#include <file_response.h>

using namespace std;

response* helloworld_application::handle_request(const request& req){
    auto *ret = new file_response();
    auto& resp = *ret;
    resp.set_version("HTTP/1.1");
    resp.set_status_code(200);
    resp.set_status_string("OK");
    resp.add_header("server","hello world server");
    resp.add_header("content-type","text/html");
    resp.add_header("connection","keep-alive");
    resp.set_content("index.html");
    resp.pack();
    return ret;
}