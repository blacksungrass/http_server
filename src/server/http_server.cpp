//
// Created by blacksungrass on 2021/12/20.
//

#include "http_server.h"
#include "../util/util.h"
#include <signal.h>
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
    m_thread_pool(thread_cnt),
    m_listen_ip{},
    m_timer(m_thread_pool),
    m_connections(1024){

    int ret = inet_aton(m_listen_address.c_str(),&m_listen_ip.sin_addr);
    if(ret==0)
        throw exception();
    m_listen_ip.sin_port = htons(m_listen_port);
    m_listen_ip.sin_family = AF_INET;
    m_listen_mode = TriggerMode::ET;
    m_conn_mode = TriggerMode::ET;
    INFO("create server at %s:%d",m_listen_address.c_str(),m_listen_port);
}

bool exit_server = false;

void handle_signal(int signum){
    exit_server = true;
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
    unsigned int tmp_event_flag = EPOLLIN;
    if(m_listen_mode==TriggerMode::ET){
        tmp_event_flag |= EPOLLET;
    }
    if(!add_to_epoll(m_listen_fd,m_epoll_fd,tmp_event_flag)){
        return false;
    }
    epoll_event event_vector[http_server::MAX_EPOLL_EVENTS_CNT];

    auto old_handler = signal(SIGINT,handle_signal);
    long t = -1;
    while(!exit_server){
        TRACE("start epoll_wait with timeout=%ld",t);
        int num = epoll_wait(m_epoll_fd,event_vector,http_server::MAX_EPOLL_EVENTS_CNT,t);
        TRACE("epoll_wait got %d events",num);
        for(int i=0;i<num;++i){
            epoll_event ev = event_vector[i];
            if(ev.data.fd==m_listen_fd){
                TRACE("%s","epoll_wait got events on listen_fd");
                bool error_flag = false;
                while(true){
                    sockaddr_in client_ip{};
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

                    auto p = make_shared<connection>(client_ip,client_fd,m_epoll_fd,app,m_conn_mode,m_timer);
                    p->init();
                    m_connections[client_fd] = p;

                }
                if(error_flag){
                    break;
                }
            }
            else{
                int fd = ev.data.fd;

                auto conn = m_connections[fd];
                if(!conn)
                    continue;
                if(ev.events&EPOLLIN){
                    TRACE("got EPOLLIN at fd=%d",fd);
                    m_thread_pool.add_task([conn](){
                        conn->handle_read();
                    });
                }
                if(ev.events&EPOLLOUT){
                    TRACE("got EPOLLOUT at fd=%d",fd);
                    m_thread_pool.add_task([conn](){
                        conn->handle_write();
                    });
                }
                if(ev.events&EPOLLHUP){
                    TRACE("got EPOLLHUP at fd=%d",fd);
                    m_thread_pool.add_task([conn](){
                        conn->handle_close();
                    });
                }
            }
        }
        t = chrono::duration_cast<chrono::milliseconds>(m_timer.tick()).count();
        if(t==0){
            t = -1;
        }

    }

    printf("%s\n","server exit gracefully");
    signal(SIGINT,old_handler);
    return true;
}