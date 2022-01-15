//
// Created by blacksungrass on 2022/1/16.
//
#include "timer_util.h"

bool operator<(const timeval& a,const timeval& b){
    if(a.tv_sec==b.tv_sec){
        return a.tv_usec<b.tv_usec;
    }
    return a.tv_sec<b.tv_sec;
}
bool operator>(const timeval& a,const timeval& b){
    if(a.tv_sec==b.tv_sec&&a.tv_usec==b.tv_usec)
        return false;
    else
        return !(a<b);
}