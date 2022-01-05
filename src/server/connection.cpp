//
// Created by blacksungrass on 2021/12/20.
//

#include "connection.h"
#include "../util/util.h"
#include "../log/logger.h"
#include <bits/stdc++.h>
#include <unistd.h>

using namespace std;


connection::connection(const sockaddr_in &client_ip, int sock_fd, int epoll_fd,application& app,TriggerMode mode)
:   m_client_ip(client_ip),
    m_sock_fd(sock_fd),
    m_epoll_fd(epoll_fd),
    m_client_address(inet_ntoa(client_ip.sin_addr)),
    m_client_port(ntohs(client_ip.sin_port)),
    m_app(app),
    m_mode(mode){

    INFO("create a new connection from %s:%d at fd=%d",m_client_address.c_str(),m_client_port,m_sock_fd);

    make_nonblocking(m_sock_fd);
    int event = EPOLLIN|EPOLLONESHOT|EPOLLHUP;
    if(m_mode==TriggerMode::ET){
        event |= EPOLLET;
    }
    add_to_epoll(m_sock_fd,m_epoll_fd,event);
}

void connection::handle_read() {
    TRACE("connection %s:%d at fd=%d handle_read",m_client_address.c_str(),m_client_port,m_sock_fd);
    static const size_t BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    while(true){
        int ret = recv(m_sock_fd,buffer,BUF_SIZE,0);
        if(ret>0){
            m_parser.parse(buffer,ret);
        }
        else if(ret==0){
            ERROR("connection %s:%d fd=%d recv 0, client close connection",m_client_address.c_str(),m_client_port,\
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
    int event = EPOLLIN|EPOLLHUP|EPOLLONESHOT;
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
    int ret = send(m_sock_fd,s.c_str(),s.size(),0);
    TRACE("connection %s:%d fd=%d send resp:\r\n%s",m_client_address.c_str(),m_client_port,m_sock_fd,s.c_str());
    return ret==s.size();
}

void connection::handle_write() {
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
    //m_container.erase(m_sock_fd);//析构掉自己
    //swap(m_parser,parser());//清空parser，减少内存占用
}