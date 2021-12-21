//
// Created by blacksungrass on 2021/12/20.
//

#include <gtest/gtest.h>
#include <bits/stdc++.h>
#include "../../src/parser/parser.h"
#include "../../src/parser/request.h"
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

vector<string> random_split(const string& s,int n = 10)
{
    default_random_engine e;
    uniform_int_distribution<size_t> dist(1,s.size()-1);
    vector<size_t> v{0,s.size()};
    for(int i=1;i<n;++i)
    {
        v.emplace_back(dist(e));
    }
    sort(begin(v),end(v));
    vector<string> ret;
    for(int i=0;i<v.size()-1;++i)
    {
        ret.emplace_back(s,v[i],v[i+1]-v[i]);
    }
    return ret;
}

TEST(test_parser,test_parser_1)
{
    string data = read_file("/home/kali/Desktop/http_server/http_server/test/parser/request2.txt");
    default_random_engine e;
    uniform_int_distribution<unsigned int> dist(0,data.size());
    vector<unsigned int> segs(10);
    for(int i=1;i<9;++i)
    {
        segs[i] = dist(e);
    }
    segs[9] = data.size();
    sort(begin(segs),end(segs));
    vector<string> strings;
    for(int i=0;i<9;++i)
    {
        strings.emplace_back(data,segs[i],segs[i+1]-segs[i]);
    }
    parser p;
    for(const string& s:strings)
    {
        bool r = p.parse(s.c_str(),s.size());
        ASSERT_EQ(r,true);
    }
    ASSERT_EQ(p.has_results(),true);
    vector<request> reqs = p.get_results();
    ASSERT_EQ(reqs.size(),1);
    request req;
    req.parse(data.c_str(),data.size());
    ASSERT_EQ(req,reqs.front());
}

TEST(test_parser,test_parser_2)
{
    string data1 = read_file("/home/kali/Desktop/http_server/http_server/test/parser/request1.txt");
    string data2 = read_file("/home/kali/Desktop/http_server/http_server/test/parser/request2.txt");
    request req1,req2;
    req1.parse(data1.c_str(),data1.size());
    req2.parse(data2.c_str(),data2.size());
    string data = data1+data2;
    parser p;
    for(const string& s:random_split(data))
    {
        bool r = p.parse(s.c_str(),s.size());
        ASSERT_EQ(r,true);
    }
    ASSERT_EQ(p.has_results(),true);
    vector<request> reqs = p.get_results();
    ASSERT_EQ(reqs.size(),2);
    ASSERT_EQ(req1,reqs[0]);
    ASSERT_EQ(req2,reqs[1]);
}