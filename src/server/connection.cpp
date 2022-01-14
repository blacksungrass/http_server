//
// Created by blacksungrass on 2021/12/20.
//

#include "connection.h"
#include "../util/util.h"
#include "../log/logger.h"
#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/epoll.h>

using namespace std;

/*
 * 目前的问题是，connection这个对象的生命周期管理
 * 目前的形式是，connection什么时候析构由对象自己决定
 * 但是在目前的线程池框架下，可能会出现的问题是，线程池种的一个线程执行connection析构了，而线程池中的另一个线程开始执行connection的成员函数
 * 目前之所以没出问题，是因为EPOLLONESHOT保证了同一时刻在线程池种对于同一个connection只执行一个成员函数
 * 但是如果加入timer，就失去了EPOLLONESHOT的保证，也就可能会出现空指针错误
 * 看来只能使用智能指针试试看了
 */

void connection::check_inactive(shared_ptr<connection> self){
    TRACE("doing check_inactive at connection %s:%d at fd=%d",self->m_client_address.c_str(),
          self->m_client_port,self->m_sock_fd);
    const int max_inactive_time = 3;//seconds
    timeval now{};
    gettimeofday(&now,nullptr);
    if(now.tv_sec-self->m_last_active_time.tv_sec>max_inactive_time){
        self->handle_close();
    }
    else{
        self->m_timer.add_timer(chrono::seconds(max_inactive_time),[self](){
           self->check_inactive(self);
        });
    }
}


connection::connection(const sockaddr_in &client_ip, int sock_fd, int epoll_fd,application& app,TriggerMode mode,
                       timer& timer)
:   m_client_ip(client_ip),
    m_sock_fd(sock_fd),
    m_epoll_fd(epoll_fd),
    m_client_address(inet_ntoa(client_ip.sin_addr)),
    m_client_port(ntohs(client_ip.sin_port)),
    m_app(app),
    m_mode(mode),
    m_timer(timer){

    INFO("create a new connection from %s:%d at fd=%d",m_client_address.c_str(),m_client_port,m_sock_fd);

    make_nonblocking(m_sock_fd);
    unsigned int event = EPOLLIN|EPOLLONESHOT|EPOLLHUP;
    if(m_mode==TriggerMode::ET){
        event |= EPOLLET;
    }
    add_to_epoll(m_sock_fd,m_epoll_fd,event);

    gettimeofday(&m_last_active_time,nullptr);

}

void connection::init(){
    TRACE("connection %s:%d at fd=%d doing init",m_client_address.c_str(),m_client_port,m_sock_fd)
    auto self = shared_from_this();
    m_timer.add_timer(chrono::seconds(3),[self](){
       self->check_inactive(self);
    });
}

void connection::handle_read() {
    gettimeofday(&m_last_active_time,nullptr);
    TRACE("connection %s:%d at fd=%d handle_read",m_client_address.c_str(),m_client_port,m_sock_fd);
    static const size_t BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    while(true){
        long ret = recv(m_sock_fd,buffer,BUF_SIZE,0);
        if(ret>0){
            m_parser.parse(buffer,ret);
        }
        else if(ret==0){
            ERROR("connection %s:%d fd=%d recv 0, client close connection",m_client_address.c_str(),m_client_port,
            m_sock_fd);
            handle_close();
            //close connection
            return;
        }
        else{
            if(errno==EWOULDBLOCK||errno==EAGAIN){
                break;
            }
            else{
                ERROR("connection %s:%d fd=%d recv failed, errno=%d(%s)",m_client_address.c_str(),m_client_port,\
                m_sock_fd,errno,strerror(errno));
                //log and close connection;
                handle_close();
                return;
            }
        }
    }
    unsigned int event = EPOLLIN|EPOLLHUP|EPOLLONESHOT;
    if(m_mode==TriggerMode::ET){
        event |= EPOLLET;
    }
    if(m_parser.has_results()){
        vector<request> requests = m_parser.get_results();
        vector<response> responses;
        for(const auto& req:requests){
            TRACE("connection %s:%d fd=%d recv req:\r\n%s",m_client_address.c_str(),m_client_port,m_sock_fd,\
            req.to_string().c_str());
            m_response_queue.emplace(m_app.handle_request(req));
        }
        event |= EPOLLOUT;
    }
    modified_epoll(m_sock_fd,m_epoll_fd,event);
}

bool connection::send_response(const response& resp){
    string s = resp.to_string();
    long ret = send(m_sock_fd,s.c_str(),s.size(),0);
    TRACE("connection %s:%d fd=%d send resp:\r\n%s",m_client_address.c_str(),m_client_port,m_sock_fd,s.c_str());
    return ret==s.size();
}

void connection::handle_write() {
    gettimeofday(&m_last_active_time,nullptr);
    TRACE("connection %s:%d at fd=%d handle_write",m_client_address.c_str(),m_client_port,m_sock_fd);
    while(!m_response_queue.empty()){
        const response& resp = m_response_queue.front();
        bool ret = send_response(resp);
        if(!ret||resp.close_connection()){
            //log error and close connection
            handle_close();
            return;
        }
        m_response_queue.pop();
    }
    uint32_t event = EPOLLIN|EPOLLHUP|EPOLLONESHOT;
    if(m_mode==TriggerMode::ET){
        event |= EPOLLET;
    }
    modified_epoll(m_sock_fd,m_epoll_fd,event);
}

void connection::handle_close(){
    TRACE("connection %s:%d at fd=%d handle_close",m_client_address.c_str(),m_client_port,m_sock_fd);
    del_from_epoll(m_sock_fd,m_epoll_fd);
    close(m_sock_fd);
}