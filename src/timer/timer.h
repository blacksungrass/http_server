//
// Created by blacksungrass on 2022/1/6.
//

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include "../thread_pool/thread_pool.h"
#include <sys/time.h>
#include <map>

class timer_event;

class timer {
    using event = std::pair<timeval,std::function<void()>>;
private:
    thread_pool& m_thread_pool;
    std::vector<event> m_events;
    timer_event* m_head;
    int64_t m_id;
    std::mutex m_mutex;
    std::map<int64_t,timer_event*> m_record;
public:
    explicit timer(thread_pool& pool);
    int64_t add_timer(const std::chrono::milliseconds& d,std::function<void()>&& func);
    int64_t add_timer(const std::chrono::microseconds& d,std::function<void()>&& func);
    int64_t add_timer(const std::chrono::seconds& d,std::function<void()>&& func);
    std::chrono::microseconds tick();
    bool del_timer(int64_t id);
};


#endif //TEST_TIMER_H
