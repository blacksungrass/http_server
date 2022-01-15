//
// Created by blacksungrass on 2022/1/6.
//

#include "list_timer.h"
#include <bits/stdc++.h>
using namespace std;

/*
 * 双向链表的节点结构，每个节点表示一个时间事件，双向链表的head.prev=nullptr,双向链表的tail.next=nullptr
 */
class timer_event{
public:
    int64_t id = 0;
    timeval time;
    function<void()> func;
    timer_event* next = nullptr;
    timer_event* prev = nullptr;
};






list_timer::list_timer(thread_pool& pool)
:m_thread_pool(pool),
 m_head(nullptr),
 m_id(0){

}

int64_t list_timer::add_timer(const std::chrono::seconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::seconds>(d);
    return add_timer(time,move(func));
}

int64_t list_timer::add_timer(const chrono::milliseconds &d, std::function<void()> &&func) {
    chrono::microseconds time = chrono::duration_cast<chrono::microseconds>(d);
    return add_timer(time,move(func));
}

int64_t list_timer::add_timer(const chrono::microseconds & d,std::function<void()>&& func) {
    timeval now{};
    gettimeofday(&now,nullptr);
    now.tv_usec += d.count();
    if(now.tv_usec>1000000){
        now.tv_sec += now.tv_usec/1000000;
        now.tv_usec %= 1000000;
    }
    auto* event = new timer_event();
    event->time = now;
    //先加锁获得事件id，并记录链表指针的位置
    {
        lock_guard lock(m_mutex);
        event->id = ++m_id;
        m_record[event->id] = event;
    }
    event->func = move(func);
    //然后加锁，将事件插入双向链表
    {
        lock_guard lock(m_mutex);
        //如果插入位置是在第一个
        if(m_head==nullptr || m_head->time>event->time){
            event->next = m_head;
            event->prev = nullptr;
            if(m_head!=nullptr){
                m_head->prev = event;
            }
            m_head = event;
            return event->id;
        }
        //从头节点开始顺序遍历，找到插入位置
        auto p2 = m_head->next;
        auto p1 = m_head;
        while(p2!=nullptr){
            if(p2->time>event->time){
                break;
            }
            p1 = p2;
            p2 = p2->next;
        }
        //找到插入位置在p1到p2之间
        event->next = p2;
        p1->next = event;
        event->prev = p1;
        if(p2!=nullptr){
            p2->prev = event;
        }
        return event->id;
    }
}

chrono::microseconds list_timer::tick() {
    timeval now{};
    long int d = 0;
    gettimeofday(&now,nullptr);
    {
        lock_guard lock(m_mutex);
        while(m_head!=nullptr){
            if(m_head->time>now){
                d = m_head->time.tv_sec-now.tv_sec;
                d *= 1000000;
                d += m_head->time.tv_usec-now.tv_usec;
                break;
            }
            m_thread_pool.add_task(move(m_head->func));
            auto tmp = m_head;
            m_head = m_head->next;
            if(m_head!=nullptr){
                m_head->prev = nullptr;
            }
            m_record.erase(tmp->id);
            delete tmp;
        }
        return chrono::microseconds(d);
    }
}

bool list_timer::del_timer(int64_t id){
    {
        lock_guard lock(m_mutex);
        auto it = m_record.find(id);
        if(it==m_record.end()){
            return false;
        }
        timer_event* event = it->second;
        m_record.erase(it);
        //如果是最左边的节点
        if(event->prev==nullptr){
            m_head = m_head->next;
            if(m_head!=nullptr){
                m_head->prev = nullptr;
            }
            delete event;
            return true;
        }
        //如果是最右边的节点,且不是最左边节点
        if(event->next==nullptr){
            event->prev->next = nullptr;
            delete event;
            return true;
        }
        //如果是中间节点
        event->prev->next = event->next;
        event->next->prev = event->prev;
        delete event;
        return true;
    }
}
