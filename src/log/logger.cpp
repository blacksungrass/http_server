//
// Created by blacksungrass on 2021/12/19.
//

#include "logger.h"
#include <unistd.h>
#include <sys/time.h>
#include <sstream>
#include <fcntl.h>
#include <exception>
#include <cstdarg>
#include <iostream>
#include <cstring>
using namespace std;

const size_t logger::MAX_RECORD_PER_FILE = 20000;
const size_t logger::MAX_QUEUE_SIZE = 1000;

logger global_default_logger("server","/tmp/http_server/");

static string generate_filename(const string& log_name,pid_t pid,timeval time,int log_idx){
    stringstream ss;
    ss<<log_name<<"_"<<pid<<"_"<<time.tv_sec<<"."<<time.tv_usec<<"_"<<log_idx;
    return ss.str();
}

void logger::writer_thread() {
    while(true){
        unique_lock lock(m_mutex);
        if(m_exited)
            return;
        m_cv.wait(lock,[this]()->bool{
           return !this->m_queue.empty()||m_exited;
        });
        if(m_exited)
            return;
        queue<string> q;
        swap(q,m_queue);
        lock.unlock();
        stringstream ss;
        int cnt = q.size();
        while(!q.empty()){
            ss<<q.front()<<"\r\n";
            q.pop();
        }
        string s = ss.str();
        write(m_log_file_fd,s.c_str(),s.size());
        m_log_cnt += cnt;
        if(m_log_cnt>logger::MAX_RECORD_PER_FILE){
            m_log_cnt = 0;
            ++m_log_idx;
            string new_file_name = generate_filename(m_log_name,m_pid,m_create_time,m_log_idx);
            m_log_file_fd = open((m_log_dir+m_log_file_name).c_str(),O_WRONLY|O_CREAT|O_APPEND,0600);
        }
    }
}

logger::logger(const std::string& log_name, const std::string& log_dir) {
    m_log_name = log_name;
    m_log_dir = log_dir;
    m_log_cnt = 0;
    m_log_idx = 0;
    m_exited = false;
    gettimeofday(&m_create_time,nullptr);
    m_pid = getpid();
    m_log_file_name = generate_filename(m_log_name,m_pid,m_create_time,m_log_idx);
    m_log_file_fd = open((m_log_dir+m_log_file_name).c_str(),O_WRONLY|O_CREAT|O_APPEND,0600);
    if(m_log_file_fd<0){
        cerr<<"open log file failed, errno="<<errno<<"("<<strerror(errno)<<")"<<endl;
        throw exception();
    }
    m_thread = thread(&logger::writer_thread,this);
}

bool logger::add_log(const std::string &log_msg, log_level level) {
    string level_string;
    switch(level){
        case log_level::TRACE:
            level_string = "TRACE";
            break;
        case log_level::DEBUG:
            level_string = "DEBUG";
            break;
        case log_level::ERROR:
            level_string = "ERROR";
            break;
        case log_level::INFO:
            level_string = "INFO";
            break;
        case log_level::DEADLY:
            level_string = "DEADLY";
            break;
        default:
            return false;
    }
    timeval cur_time;
    gettimeofday(&cur_time,nullptr);
    stringstream ss;
    ss<<"["<<level_string<<"] "<<cur_time.tv_sec<<"."<<cur_time.tv_usec<<":"<<log_msg;
    lock_guard lock(m_mutex);
    if(m_queue.size()>logger::MAX_QUEUE_SIZE){
        return false;
    }
    m_queue.emplace(ss.str());
    m_cv.notify_one();
    return true;
}

bool logger::add_log(log_level level, const char *fmt, ...) {
    va_list valst;
    va_start(valst, fmt);
    char buffer[1024];
    int m = vsnprintf(buffer, 1024, fmt, valst);
    buffer[m] = '\0';
    string msg(buffer);
    return add_log(msg,level);
}

logger::~logger() {
    unique_lock lock(m_mutex);
    m_exited = true;
    m_cv.notify_all();
    lock.unlock();
    m_thread.join();
}

