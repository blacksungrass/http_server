//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include "../parser/parser.h"
#include "http_server.h"

#include <string>
#include <arpa/inet.h>
#include <sys/socket.h>

enum class TriggerMode;

class connection {
private:
    std::string m_client_address;
    u_short m_client_port;
    sockaddr_in m_client_ip;
    int m_sock_fd;
    int m_epoll_fd;
    parser m_parser;
    application& m_app;
    TriggerMode m_mode;
    std::queue<response> m_response_queue;
    bool send_response(const response& response);
public:
    connection(const sockaddr_in& client_ip,int sock_fd,int epoll_fd, application& app,TriggerMode mode);
    void handle_read();
    void handle_write();
    void handle_close();
};


#endif //TEST_CONNECTION_H
