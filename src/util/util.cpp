//
// Created by blacksungrass on 2021/12/20.
//

#include "util.h"
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

