//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include "../parser/parser.h"
#include "../timer/list_timer.h"
#include "http_server.h"

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

enum class TriggerMode;

class connection:public std::enable_shared_from_this<connection> {
private:
    std::string m_client_address;
    u_short m_client_port;
    sockaddr_in m_client_ip;
    int m_sock_fd;
    int m_epoll_fd;
    parser m_parser;
    application& m_app;
    TriggerMode m_mode;
    timeval m_last_active_time;
    std::queue<response> m_response_queue;
    bool send_response(const response& response);
    list_timer& m_timer;

    void check_inactive(std::shared_ptr<connection> self);
public:
    connection(const sockaddr_in& client_ip,int sock_fd,int epoll_fd, application& app,TriggerMode mode,list_timer& timer);
    void init();
    void handle_read();
    void handle_write();
    void handle_close();
};


#endif //TEST_CONNECTION_H
