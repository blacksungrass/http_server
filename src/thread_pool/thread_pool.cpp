//
// Created by blacksungrass on 2021/12/19.
//

#include "thread_pool.h"
#include <exception>
#include <iostream>
#include "../log/logger.h"

using namespace std;

const int thread_pool::MAX_QUEUE_LENGTH = 200;

thread_pool::thread_pool():thread_pool(thread::hardware_concurrency()){

}

thread_pool::thread_pool(unsigned int num_workers)
:exited(false),
 m_num_workers(num_workers){
    if(m_num_workers==0){
        throw invalid_argument("线程池的线程数应大于0");
    }
    for(int i=0;i<m_num_workers;++i){
        m_workers.emplace_back(&thread_pool::run,this);
    }
}


void thread_pool::run() {
    while(true){
        unique_lock lock(m_mutex);
        if(exited){
            return;
        }
        m_cv.wait(lock,[this]()->bool{
            return !(this->m_task_queue.empty());
        });
        if(exited){
            return;
        }
        function<void()> f = (move(m_task_queue.front()));
        m_task_queue.pop();
        lock.unlock();
        try{
            f();
        }
        catch (...){
            ERROR("thread %lld of thread pool got exception",this_thread::get_id());
        }
    }
    TRACE("thread %lld of thread pool exited",this_thread::get_id());
}

bool thread_pool::add_task(function<void()> &&f) {
    lock_guard lock(m_mutex);
    if(m_task_queue.size()>thread_pool::MAX_QUEUE_LENGTH)
        return false;
    m_task_queue.emplace(move(f));
    m_cv.notify_one();
    return true;
}

thread_pool::~thread_pool(){
    {
        lock_guard lock(m_mutex);
        while(!m_task_queue.empty())
            m_task_queue.pop();
        exited = true;
        m_task_queue.push([](){});
    }
    m_cv.notify_all();
    for(thread& th:m_workers){
        th.join();
    }
    m_workers.clear();
}