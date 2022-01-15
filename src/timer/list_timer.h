//
// Created by blacksungrass on 2022/1/6.
//

#ifndef TEST_LIST_TIMER_H
#define TEST_LIST_TIMER_H

#include "../thread_pool/thread_pool.h"
#include "timer_util.h"
#include <sys/time.h>
#include <map>

class timer_event;

class list_timer {
private:
    thread_pool& m_thread_pool;
    timer_event* m_head;
    int64_t m_id;
    std::mutex m_mutex;
    std::map<int64_t,timer_event*> m_record;
public:
    explicit list_timer(thread_pool& pool);
    int64_t add_timer(const std::chrono::milliseconds& d,std::function<void()>&& func);
    int64_t add_timer(const std::chrono::microseconds& d,std::function<void()>&& func);
    int64_t add_timer(const std::chrono::seconds& d,std::function<void()>&& func);
    std::chrono::microseconds tick();
    bool del_timer(int64_t id);
};


#endif //TEST_LIST_TIMER_H
