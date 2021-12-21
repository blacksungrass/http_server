//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_HTTP_SERVER_H
#define TEST_HTTP_SERVER_H

#include <string>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include "../thread_pool/thread_pool.h"
#include "../log/logger.h"

enum class TriggerMode{
    LT,ET
};

class http_server {
private:
    const static int MAX_EPOLL_EVENTS_CNT = 65536;
    std::string m_listen_address;
    sockaddr_in m_listen_ip;
    u_short m_listen_port;
    std::string m_root_dir;
    int m_epoll_fd;
    int m_listen_fd;
    TriggerMode m_listen_mode,m_conn_mode;
    thread_pool m_thread_pool;

public:
    http_server(std::string listen_address,u_short listen_port,std::string root_dir);
    bool start();
};


#endif //TEST_HTTP_SERVER_H
