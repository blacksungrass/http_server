//
// Created by blacksungrass on 2022/1/16.
//

#include "heap_timer.h"
#include <bits/stdc++.h>
using namespace std;

using event = std::pair<timeval,std::function<void()>>;

static bool event_cmp(const event& a,const event& b){
    return a.first<b.first;
}

static bool event_cmp_r(const event& a,const event& b){
    return a.first>b.first;
}

heap_timer::heap_timer(thread_pool& pool)
:m_thread_pool(pool),
 m_queue(event_cmp_r){

}

void heap_timer::add_timer(const std::chrono::seconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::seconds>(d);
    return add_timer(time,move(func));
}

void heap_timer::add_timer(const std::chrono::milliseconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::microseconds>(d);
    return add_timer(time,move(func));
}

void heap_timer::add_timer(const std::chrono::microseconds &d, std::function<void()> &&func) {
    timeval now{};
    gettimeofday(&now,nullptr);
    now.tv_usec += d.count();
    if(now.tv_usec>1000000){
        now.tv_sec += now.tv_usec/1000000;
        now.tv_usec %= 1000000;
    }
    {
        unique_lock lock(m_mutex);
        m_queue.emplace(now,move(func));
    }

}

chrono::microseconds heap_timer::tick() {
    timeval now{};
    long int d = 0;
    gettimeofday(&now,nullptr);
    {
        unique_lock lock(m_mutex);
        while(!m_queue.empty()){
            auto t = m_queue.top();
            if(t.first>now){
                d = t.first.tv_sec-now.tv_sec;
                d *= 1000000;
                d += t.first.tv_usec-now.tv_usec;
                break;
            }
            m_queue.pop();
            m_thread_pool.add_task(move(t.second));
        }
    }
    return chrono::microseconds(d);
}