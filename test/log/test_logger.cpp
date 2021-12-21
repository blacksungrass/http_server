//
// Created by blacksungrass on 2021/12/20.
//

#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "../../src/log/logger.h"
using namespace std;

static string get_log_dir(){
    string t(__FILE__);
    auto pos = t.rfind('/');
    assert(pos!=string::npos);
    t.resize(pos+1);
    return t+"log_file/";
}

TEST(test_log,test_log_1){
    logger mylog("first",get_log_dir());

    for(int i=0;i<1000;++i){
        LOG_TRACE(mylog,"the variable's name is %d",i);
    }

}