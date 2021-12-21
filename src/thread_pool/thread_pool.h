//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_THREAD_POOL_H
#define TEST_THREAD_POOL_H

#include <queue>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <functional>
#include <atomic>

//todo 线程池能否支持移动构造或者移动赋值？


class thread_pool {
private:
    const static int MAX_QUEUE_LENGTH;
    unsigned int m_num_workers;
    void run();
    std::atomic<bool> exited;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::vector<std::thread> m_workers;
    std::queue<std::function<void()>> m_task_queue;//todo 这里似乎只可以放放回置为void的task，是否可以改进为可以放任意的返回类型的函数
public:
    thread_pool();
    thread_pool(unsigned int num_workers);
    thread_pool(const thread_pool&) = delete;
    thread_pool& operator = (const thread_pool&) = delete;
    thread_pool(thread_pool&& o) = delete;
    thread_pool& operator = (thread_pool&& o) = delete;
    ~thread_pool();
    bool add_task(std::function<void()>&& f);
};


#endif //TEST_THREAD_POOL_H
