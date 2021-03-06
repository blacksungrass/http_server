//
// Created by blacksungrass on 2022/1/7.
//

#include <gtest/gtest.h>
#include <bits/stdc++.h>
using namespace std;
#include "../../src/timer/list_timer.h"
#include "../../src/timer/heap_timer.h"

TEST(test_timer,test_timer_1){
    thread_pool pool;
    atomic<int> cnt = 0;
    list_timer t(pool);
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

TEST(test_timer,test_timer_2){
    thread_pool pool;
    atomic<int> cnt = 0;
    list_timer t(pool);
    vector<int> v;
    for(int i=0;i<1000;++i){
        v.emplace_back(i);
    }
    default_random_engine e;
    std::shuffle(begin(v),end(v),e);
    for(int x:v){
        t.add_timer(chrono::milliseconds(x),[x,&cnt]{
            cout<<"x="<<x<<endl;
            ++cnt;
        });
    }

    while(true){
        chrono::microseconds a = t.tick();
        if(a.count()==0){
            break;
        }
        this_thread::sleep_for(a);
    }
}

TEST(test_timer,test_timer_3){
    thread_pool pool;
    atomic<int> cnt = 0;
    heap_timer t(pool);
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

TEST(test_timer,test_timer_4){
    thread_pool pool;
    atomic<int> cnt = 0;
    heap_timer t(pool);
    vector<int> v;
    for(int i=0;i<1000;++i){
        v.emplace_back(i);
    }
    default_random_engine e;
    std::shuffle(begin(v),end(v),e);
    for(int x:v){
        t.add_timer(chrono::milliseconds(x),[x,&cnt]{
            cout<<"x="<<x<<endl;
            ++cnt;
        });
    }

    while(true){
        chrono::microseconds a = t.tick();
        if(a.count()==0){
            break;
        }
        this_thread::sleep_for(a);
    }
}