//
// Created by blacksungrass on 2021/12/20.
//

#ifndef TEST_CONNECTION_H
#define TEST_CONNECTION_H

#include "../parser/parser.h"
#include "../thread_pool/thread_pool.h"

#include <string>
#include <arpa/inet.h>
#icclude <sys/socket.h>

class connection {
private:
    std::string m_client_address;
    u_short m_client_port;
    sockaddr_in m_client_ip;
    int m_sock_fd;
    thread_pool* m_pool;

public:
    connection(const sockaddr_in& client_ip,int sock_fd);
    void handle_read();
    void handle_write();
};


#endif //TEST_CONNECTION_H
