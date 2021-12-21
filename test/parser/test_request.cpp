//
// Created by blacksungrass on 2021/12/19.
//
#include <gtest/gtest.h>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <random>
#include <vector>
#include "../../src/parser/request.h"
#include <unistd.h>
using namespace std;

string read_file(const string& path)
{
    ifstream ifs(path,ifstream::in);
    string ret;
    char buf[1024];
    while(ifs.good())
    {
        ifs.read(buf,1024);
        ret.append(buf,ifs.gcount());
    }
    return ret;
}

TEST(test_parser,test_request_1)
{
    string data = read_file("/home/kali/Desktop/http_server/http_server/test/parser/request1.txt");
    request req;
    size_t n = req.parse(data.c_str(),data.size());
    ASSERT_EQ(n,data.size());
    //cout<<req.to_string()<<endl;
    //cout<<"finished:"<<req.finished()<<endl;
    ASSERT_EQ(req.finished(),true);
}

TEST(test_parser,test_request_2)
{
    string data = read_file("/home/kali/Desktop/http_server/http_server/test/parser/request2.txt");
    request req;
    size_t n = req.parse(data.c_str(),data.size());
    ASSERT_EQ(n,data.size());
    //cout<<req.to_string()<<endl;
    //cout<<"finished:"<<req.finished()<<endl;
    ASSERT_EQ(req.finished(),true);
}

