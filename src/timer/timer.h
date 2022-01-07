//
// Created by blacksungrass on 2022/1/6.
//

#ifndef TEST_TIMER_H
#define TEST_TIMER_H

#include "../thread_pool/thread_pool.h"
#include <sys/time.h>


class timer {
    using event = std::pair<timeval,std::function<void()>>;
private:
    thread_pool& m_thread_pool;
    std::vector<event> m_events;
    //const long int m_max_tick_time = 20;//milliseconds！！！！
public:
    explicit timer(thread_pool& pool);
    void add_timer(const std::chrono::milliseconds& d,std::function<void()>&& func);
    void add_timer(const std::chrono::microseconds& d,std::function<void()>&& func);
    void add_timer(const std::chrono::seconds& d,std::function<void()>&& func);
    std::chrono::microseconds tick();
};


#endif //TEST_TIMER_H
