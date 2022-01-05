//
// Created by blacksungrass on 2021/12/31.
//

#include "helloworld_application.h"
#include <bits/stdc++.h>
using namespace std;

response helloworld_application::handle_request(const request& req){
    response resp;
    resp.set_version("HTTP/1.1");
    resp.set_status_code(200);
    resp.set_status_string("OK");
    //resp.add_header("server","hello world server");
    resp.add_header("content-type","text/html");
    resp.add_header("connection","keep-alive");
    resp.append_content("<html><h1>Hello world!</h1></html>");
    resp.set_close_connection(true);
    return resp;
}