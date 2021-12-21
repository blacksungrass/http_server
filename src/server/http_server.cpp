//
// Created by blacksungrass on 2021/12/20.
//

#include "http_server.h"
#include "../util/util.h"
#include <unistd.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <bits/stdc++.h>
using namespace std;


static bool add_to_epoll(int fd,int epoll_fd,uint32_t event){
    epoll_event e;
    e.data = fd;
    e.events = event;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&e);
    return ret==0;
}

http_server::http_server(std::string listen_address, u_short listen_port, std::string root_dir):\
m_listen_address(listen_address),
m_listen_port(listen_port),
m_root_dir(root_dir)
{

    int ret = inet_aton(m_listen_address.c_str(),&m_listen_ip.sin_addr);
    if(ret)
        throw exception();
    m_listen_ip.sin_port = htons(m_listen_port);
    m_listen_mode = TriggerMode::ET;
    m_conn_mode = TriggerMode::ET;
}

bool http_server::start() {
    m_listen_fd = socket(AF_INET,SOCK_STREAM,0);
    if(m<listen_fd<0)
        return false;
    socklen_t len = sizeof(m_listen_fd);
    int ret = bind(m_listen_fd,(sockaddr*)&m_listen_ip,len);
    if(ret)
        return false;
    make_nonblocking(m_listen_fd);
    ret = listen(m_listen_fd,5);
    if(ret)
        return false;
    m_epoll_fd = epoll_create(5);
    if(m_epoll_fd<0)
        return false;
    if(!add_to_epoll(m_listen_fd,m_epoll_fd,EPOLLIN)){
        return false;
    }
    epoll_event event_vector[http_server::MAX_EPOLL_EVENTS_CNT];
    while(true){
        int num = epoll_wait(m_epoll_fd,epoll_vector,http_server::MAX_EPOLL_EVENTS_CNT,0);
        for(int i=0;i<num;++i)
        {
            epoll_event ev = event_vector[i];
            if(ev.data==m_listen_fd){

            }
            else{

            }
        }
    }

}