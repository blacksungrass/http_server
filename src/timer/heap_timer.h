//
// Created by blacksungrass on 2022/1/16.
//

#ifndef TEST_HEAP_TIMER_H
#define TEST_HEAP_TIMER_H

#include "timer_util.h"
#include "../thread_pool/thread_pool.h"
#include <queue>
#include <utility>
#include <functional>
#include <chrono>

class heap_timer {
    using event = std::pair<timeval,std::function<void()>>;
private:
    thread_pool& m_thread_pool;
    std::mutex m_mutex;
    std::priority_queue<event,std::deque<event>,bool(*)(const event&,const event&)> m_queue;
public:
    explicit heap_timer(thread_pool& pool);
    void add_timer(const std::chrono::milliseconds& d,std::function<void()>&& func);
    void add_timer(const std::chrono::microseconds& d,std::function<void()>&& func);
    void add_timer(const std::chrono::seconds& d,std::function<void()>&& func);
    std::chrono::microseconds tick();
};


#endif //TEST_HEAP_TIMER_H
