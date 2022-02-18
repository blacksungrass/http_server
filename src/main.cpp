//
// Created by blacksungrass on 2021/12/31.
//

#include "server/http_server.h"
#include "app/helloworld_application.h"
#include <iostream>
#include "app/html_application.h"
#include "app/short_link_application.h"
using namespace std;
int main(){
    //html_application app("/home/kali/Desktop/tiny_http_server/root/");
    short_link_application app("/home/kali/Desktop/tiny_http_server/root/");
    http_server server("0.0.0.0",23333);
    bool ret = server.start(app);
    if(!ret){
        cout<<"start server failed"<<endl;
    }
    return 0;
}