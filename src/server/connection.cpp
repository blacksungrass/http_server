//
// Created by blacksungrass on 2021/12/20.
//

#include "connection.h"
#include "../util/util.h"
#include <bits/stdc++.h>
using namespace std;


connection::connection(const sockaddr_in &client_ip, int sock_fd, int epoll_fd,application& app,TriggerMode mode)
:   m_client_ip(client_ip),
    m_sock_fd(sock_fd),
    m_epoll_fd(epoll_fd),
    m_client_address(inet_ntoa(client_ip.sin_addr)),
    m_client_port(ntohs(client_ip.sin_port)),
    m_app(app),
    m_mode(mode){

    make_nonblocking(m_sock_fd);
    int event = EPOLLIN|EPOLLONESHOT|EPOLLHUP;
    if(m_mode==TriggerMode::ET){
        event |= EPOLLET;
    }
    add_to_epoll(m_sock_fd,m_epoll_fd,event);
}

void connection::handle_read() {
    static const size_t BUF_SIZE = 1024;
    char buffer[BUF_SIZE];
    while(true){
        int ret = recv(m_sock_fd,buffer,BUF_SIZE,0);
        if(ret>0){
            m_parser.parse(buffer,ret);
        }
        else if(ret==0){
            handle_close();
            //close connection
            return;
        }
        else{
            if(errno==EWOULDBLOCK||errno==EAGAIN){
                break;
            }
            else{
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
            m_response_queue.emplace(m_app.handle_request(req));
        }
        event |= EPOLLOUT;
    }
    modified_epoll(m_sock_fd,m_epoll_fd,event);
}

bool connection::send_response(const response& resp){
    string s = resp.to_string();
    int ret = send(m_sock_fd,s.c_str(),s.size(),0);
    return ret==s.size();
}

void connection::handle_write() {
    while(!m_response_queue.empty()){
        const response& resp = m_response_queue.front();
        bool ret = send_response(resp);
        if(!ret){
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
    del_from_epoll(m_sock_fd,m_epoll_fd);
    //swap(m_parser,parser());//清空parser，减少内存占用
}