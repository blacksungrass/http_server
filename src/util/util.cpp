//
// Created by blacksungrass on 2021/12/20.
//

#include "util.h"
#include <sys/epoll.h>
#include <bits/stdc++.h>
using namespace std;

void tolower(string& s){
    for(char& c:s)
    {
        c = tolower(c);
    }
}

void toupper(string& s){
    for(char& c:s)
    {
        c = toupper(c);
    }
}

void make_nonblocking(int fd){
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
}

bool add_to_epoll(int fd,int epoll_fd,uint32_t event){
    epoll_event e;
    e.data.fd = fd;
    e.events = event;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_ADD,fd,&e);
    return ret==0;
}

bool modified_epoll(int fd,int epoll_fd,uint32_t event){
    epoll_event e;
    e.data.fd = fd;
    e.events = event;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_MOD,fd,&e);
    return ret==0;
}

bool del_from_epoll(int fd,int epoll_fd){
    epoll_event e;
    e.data.fd = fd;
    int ret = epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,&e);
    return ret==0;
}