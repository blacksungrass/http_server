//
// Created by blacksungrass on 2022/1/7.
//

#include <gtest/gtest.h>
#include <bits/stdc++.h>
using namespace std;
#include "../../src/timer/timer.h"

TEST(test_timer,test_timer_1){
    thread_pool pool;
    atomic<int> cnt = 0;
    timer t(pool);
    for(int i=0;i<20;++i){
        t.add_timer(chrono::seconds(i),[i,&cnt](){
           cout<<"i="<<i<<endl;
           ++cnt;
        });
    }
    while(cnt<20){
        chrono::microseconds a = t.tick();
        cout<<"a="<<a.count()<<endl;
        if(a.count()==0){
            break;
        }
        this_thread::sleep_for(a);
    }

}