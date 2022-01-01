//
// Created by blacksungrass on 2021/12/31.
//

#include "server/http_server.h"
#include "server/helloworld_application.h"
#include <iostream>
using namespace std;
int main(){
    helloworld_application app;
    http_server server("0.0.0.0",23333,"/var/www");
    bool ret = server.start(app);
    if(!ret){
        cout<<"start server failed"<<endl;
    }
    return 0;
}