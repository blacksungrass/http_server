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
#include "../util/util.h"
using namespace std;

http_server::http_server(const string& listen_address, u_short listen_port, const std::string& root_dir)
:   http_server(listen_address,listen_port,root_dir,thread::hardware_concurrency()){

}

http_server::http_server(const string& listen_address, u_short listen_port, const string& root_dir, unsigned int thread_cnt)
:   m_listen_address(listen_address),
    m_listen_port(listen_port),
    m_root_dir(root_dir),
    m_epoll_fd(-1),
    m_listen_fd(-1),
    m_thread_pool(thread_cnt){

    int ret = inet_aton(m_listen_address.c_str(),&m_listen_ip.sin_addr);
    if(ret==0)
        throw exception();
    m_listen_ip.sin_port = htons(m_listen_port);
    m_listen_ip.sin_family = AF_INET;
    m_listen_mode = TriggerMode::ET;
    m_conn_mode = TriggerMode::ET;
    INFO("create server at %s:%d",m_listen_address.c_str(),m_listen_port);
}

bool http_server::start(application& app) {
    m_listen_fd = socket(AF_INET,SOCK_STREAM,0);
    if(m_listen_fd<0)
        return false;
    int one = 1;
    setsockopt(m_listen_fd,SOL_SOCKET,SO_REUSEADDR,&one,sizeof(one));
    make_nonblocking(m_listen_fd);
    socklen_t len = sizeof(m_listen_ip);
    int ret = bind(m_listen_fd,(sockaddr*)&m_listen_ip,len);
    if(ret){
        perror("bind failed");
        return false;
    }
    ret = listen(m_listen_fd,5);
    if(ret){
        perror("listen failed");
        return false;
    }
    INFO("%s","server start listening");
    m_epoll_fd = epoll_create(5);
    if(m_epoll_fd<0)
        return false;
    int tmp_event_flag = EPOLLIN;
    if(m_listen_mode==TriggerMode::ET){
        tmp_event_flag |= EPOLLET;
    }
    if(!add_to_epoll(m_listen_fd,m_epoll_fd,tmp_event_flag)){
        return false;
    }
    epoll_event event_vector[http_server::MAX_EPOLL_EVENTS_CNT];
    bool exit = false;
    while(!exit){
        TRACE("%s","start epoll_wait");
        int num = epoll_wait(m_epoll_fd,event_vector,http_server::MAX_EPOLL_EVENTS_CNT,-1);
        TRACE("epoll_wait got %d events",num);
        for(int i=0;i<num;++i){
            epoll_event ev = event_vector[i];
            if(ev.data.fd==m_listen_fd){
                TRACE("%s","epoll_wait got events on listen_fd");
                bool error_flag = false;
                while(true){
                    sockaddr_in client_ip;
                    socklen_t client_ip_len = sizeof(sockaddr_in);
                    int client_fd = accept(m_listen_fd,(sockaddr*)&client_ip,&client_ip_len);
                    TRACE("accept return client_fd=%d",client_fd);
                    if(client_fd<0){
                        if(errno==EWOULDBLOCK||errno==EAGAIN){
                            TRACE("errno=%d(%s)",errno,strerror(errno));
                            break;
                        }
                        else{
                            TRACE("errno=%d(%s)",errno,strerror(errno));
                            error_flag = true;
                            break;
                        }
                    }
                    make_nonblocking(client_fd);

                    auto it = m_connections.find(client_fd);
                    if(it!=m_connections.end()){
                        INFO("erase a old connection at client_fd=%d",client_fd);
                        m_connections.erase(it);
                    }
                    m_connections.emplace(piecewise_construct,forward_as_tuple(client_fd),\
                    forward_as_tuple(client_ip,client_fd,m_epoll_fd,app,m_conn_mode));
                }
                if(error_flag){
                    break;
                }
            }
            else{
                int fd = ev.data.fd;
                auto it = m_connections.find(fd);
                if(it==m_connections.end()){
                    continue;
                }
                connection& conn = it->second;
                if(ev.events&EPOLLIN){
                    m_thread_pool.add_task([&conn](){
                        conn.handle_read();
                    });
                }
                if(ev.events&EPOLLOUT){
                    m_thread_pool.add_task([&conn](){
                        conn.handle_write();
                    });
                }
                if(ev.events&EPOLLHUP){
                    m_thread_pool.add_task([&conn](){
                        conn.handle_close();
                    });
                }
            }
        }

    }

}