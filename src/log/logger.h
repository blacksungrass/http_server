//
// Created by blacksungrass on 2021/12/19.
//

#ifndef TEST_LOGGER_H
#define TEST_LOGGER_H

#include <queue>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>

enum class log_level{
    TRACE,DEBUG,INFO,WARNING,ERROR,DEADLY
};

class logger {
private:
    const static std::size_t MAX_QUEUE_SIZE;
    const static std::size_t MAX_RECORD_PER_FILE;
    std::queue<std::string> m_queue;
    std::thread m_writer_thread;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::string m_log_dir;
    std::string m_log_name;
    std::string m_log_file_name;
    std::thread m_thread;
    struct timeval m_create_time;
    pid_t m_pid;
    int m_log_file_fd;
    void writer_thread();
    int m_log_cnt;
    int m_log_idx;
    bool m_exited;
public:
    logger(std::string log_name,std::string log_dir);
    logger(const logger&) = delete;
    logger& operator=(const logger&) = delete;
    logger(logger&&) = delete;
    logger& operator=(logger&&) = delete;
    bool add_log(const std::string& log_msg,log_level level=log_level::INFO);
    bool add_log(log_level level,const char* fmt,...);
    ~logger();
};



#define LOG_TRACE(logger_name, format, ...)     logger_name.add_log(log_level::TRACE,format,##__VA_ARGS__);
#define LOG_DEBUG(logger_name, format, ...)     logger_name.add_log(log_level::DEBUG,format,##__VA_ARGS__);
#define LOG_INFO(logger_name, format, ...)      logger_name.add_log(log_level::INFO,format,##__VA_ARGS__);
#define LOG_WARNING(logger_name, format, ...)   logger_name.add_log(log_level::WARNING,format,##__VA_ARGS__);
#define LOG_ERROR(logger_name, format, ...)     logger_name.add_log(log_level::ERROR,format,##__VA_ARGS__);
#define LOG_DEADLY(logger_name, format, ...)    logger_name.add_log(log_level::DEADLY,format,##__VA_ARGS__);



#endif //TEST_LOGGER_H
