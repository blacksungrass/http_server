//
// Created by blacksungrass on 2022/1/6.
//

#include "timer.h"
#include <bits/stdc++.h>
using namespace std;



static bool operator<(const timeval& a,const timeval& b){
    if(a.tv_sec==b.tv_sec){
        return a.tv_usec<b.tv_usec;
    }
    return a.tv_sec<b.tv_sec;
}
static bool operator>(const timeval& a,const timeval& b){
    if(a.tv_sec==b.tv_sec&&a.tv_usec==b.tv_usec)
        return false;
    else
        return !(a<b);
}


timer::timer(thread_pool& pool)
:m_thread_pool(pool){

}

void timer::add_timer(const std::chrono::seconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::seconds>(d);
    add_timer(time,move(func));
}

void timer::add_timer(const chrono::milliseconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::microseconds>(d);
    add_timer(time,move(func));
}

void timer::add_timer(const chrono::microseconds & d,std::function<void()>&& func) {
    timeval now{};
    gettimeofday(&now,nullptr);
    now.tv_usec += d.count();
    if(now.tv_usec>1000000){
        now.tv_sec += now.tv_usec/1000000;
        now.tv_usec %= 1000000;
    }
    m_events.emplace_back(now,func);
    push_heap(m_events.begin(),m_events.end(),[](const event& a,const event& b){
        return a.first>b.first;
    });
}

chrono::microseconds timer::tick() {
    timeval now{};
    gettimeofday(&now,nullptr);
    long int d = 0;
    while(!m_events.empty()){
        if(now<m_events[0].first){
            d = m_events[0].first.tv_sec-now.tv_sec;
            d *= 1000000;
            d += m_events[0].first.tv_usec-now.tv_usec;
            break;
        }
        pop_heap(m_events.begin(),m_events.end(),[](const event& a,const event& b){
            return a.first>b.first;
        });
        m_thread_pool.add_task(move(m_events.back().second));
        m_events.pop_back();
    }
    return chrono::microseconds(d);
}
