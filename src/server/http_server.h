//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_HTTP_SERVER_H
#define TEST_HTTP_SERVER_H

#include <map>
#include <string>

#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

#include "../thread_pool/thread_pool.h"
#include "../log/logger.h"
#include "../app/application.h"
#include "connection.h"
#include "../timer/list_timer.h"
#include <memory>

class connection;

enum class TriggerMode{
    LT,ET
};

class http_server {
private:
    const static int MAX_EPOLL_EVENTS_CNT = 65536;
    std::string m_listen_address;//监听的ip地址（字符串格式）
    sockaddr_in m_listen_ip;//监听的地址（包括ip和端口)
    u_short m_listen_port;//听的端口
    int m_epoll_fd;//epoll文件描述符
    int m_listen_fd;//监听socket文件描述符
    TriggerMode m_listen_mode;//监听的模式（LT或ET）
    TriggerMode m_conn_mode;//客户端连接的模式(LT或ET)
    thread_pool m_thread_pool;//线程池
    std::vector<std::shared_ptr<connection>> m_connections;
    list_timer m_timer;
public:
    http_server(const std::string& listen_address,u_short listen_port);
    http_server(const std::string& listen_address,u_short listen_port,unsigned int thread_cnt);

    bool start(application& app);
};


#endif //TEST_HTTP_SERVER_H
