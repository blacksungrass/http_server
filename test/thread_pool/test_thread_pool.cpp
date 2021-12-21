//
// Created by blacksungrass on 2021/12/19.
//

#include <gtest/gtest.h>
#include <bits/stdc++.h>
using namespace std;
#include "../../src/thread_pool/thread_pool.h"
mutex m;
int cnt = 0;
TEST(test_thread_pool,test_thread_pool_1){
    thread_pool pool(5);
    cout<<"start"<<endl;
    for(int i=0;i<100;++i){
        pool.add_task([](){
            lock_guard lock(m);
            cout<<cnt<<endl;
            ++cnt;
        });
    }
    this_thread::sleep_for(2s);
}