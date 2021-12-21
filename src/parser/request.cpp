//
// Created by blacksungrass on 2021/12/19.
//

#include "request.h"
#include <sstream>
#include "../util/util.h"

using namespace std;

request::request() {
    m_parse_state = parse_state::FIRST_LINE;
    m_method = http_method::GET;
    m_data_len = 0;
    m_failure = false;
    m_finished = false;
}

static string strip(const string& s){
    auto pos1 = s.find_first_not_of(' ');
    auto pos2 = s.find_last_not_of(' ');
    if(pos1>pos2)
        return {};
    return {s,pos1,pos2-pos1+1};
}

static tuple<bool,http_method,string,string> parse_first_line(const string& line){
    int pos1 = -1;
    int pos2 = -1;
    auto error_return_value = make_tuple(false,http_method::GET,"","");
    for(int i=0;i<line.size();++i)
    {
        if(line[i]==' ')
        {
            if(pos1==-1)
            {
                pos1 = i;
            }
            else if(pos2==-1)
            {
                pos2 = i;
            }
            else
            {
                return error_return_value;
            }
        }
    }
    string method_str = line.substr(0,pos1);
    string url = line.substr(pos1+1,pos2-pos1-1);
    string version = line.substr(pos2+1);
    if(method_str.size()<=0||url.size()<=0||version.size()<=0)
        return error_return_value;
    for(char& c:method_str)
    {
        c = toupper(c);
    }
    http_method method;
    if(method_str=="GET")
        method = http_method::GET;
    else if(method_str=="POST")
        method = http_method::POST;
    else
        return error_return_value;
    return make_tuple(true,method,url,version);
}

size_t request::parse_first_line(const char *data, int len) {
    if(len==0)
        return 0;
    string tmp;
    int i;
    for(i=1;i<len;++i)
    {
        if(data[i-1]=='\r'&&data[i]=='\n')
        {
            tmp = string(data,i-1);
            break;
        }
    }
    if(i==len)//did not find \r\n
        return 0;
    auto [success,method,url,version] = ::parse_first_line(tmp);
    if(!success)
    {
        m_failure = true;
        return 0;
    }
    m_method = method;
    m_url = move(url);
    m_version = move(version);
    m_parse_state = parse_state::HEADERS;
    return i+1;
}

static tuple<bool,string,string> parse_header(const string& line){
    size_t pos = string::npos;
    auto error_return_value = make_tuple(false,"","");
    for(int i=0;i<line.size();++i)
    {
        char c = line[i];
        if(c==':')
        {
            pos = i;
            break;
        }
    }
    if(pos==string::npos)
        return error_return_value;
    string key(line,0,pos);
    string value(line,pos+1);
    key = strip(key);
    value = strip(value);
    if(key.empty()||value.empty())
        return error_return_value;
    tolower(key);
    tolower(value);
    return make_tuple(true, key, value);
}

size_t request::parse_headers(const char *data, int len) {
    int p1 = 0,p2 = 1;
    while(p2<len)
    {
        if(data[p2-1]=='\r'&&data[p2]=='\n')
        {
            if(p1+1==p2)//遇到空行
            {
                p1 = p2+1;
                auto it = m_headers.find("content-length");
                if(it!=m_headers.end())//如果在header中有content-length字段
                {
                    try
                    {
                        m_data_len = stoi(it->second);
                    }
                    catch (...)
                    {
                        m_failure = true;
                        break;
                    }
                    if(m_data_len<0)
                    {
                        m_failure = true;
                        break;
                    }
                }
                if(m_data_len==0)
                    m_finished = true;
                m_parse_state = parse_state::CONTENT;
                break;
            }
            string tmp(data+p1,p2-p1-1);
            auto [success,key,value] = parse_header(tmp);
            if(!success)
            {
                m_failure = true;
                return p2+1;
            }
            m_headers[key] = value;
            p1 = p2+1;
            ++p2;
        }
        ++p2;
    }
    return p1;
}

size_t request::parse_content(const char *data, int len) {
    if(m_failure)
        return 0;
    size_t cnt = min(static_cast<size_t>(len),m_data_len-m_data.size());
    if(cnt==0)
    {
        m_finished = true;
        return 0;
    }
    m_data.append(data,cnt);
    if(m_data_len==m_data.size())
        m_finished = true;
    return cnt;
}

size_t request::parse(const char *data, size_t len){
    if(m_failure||m_finished)
        return 0;
    size_t pos = 0;
    if(m_parse_state==parse_state::FIRST_LINE)
    {
        pos = parse_first_line(data,len);
        if(m_failure)
            return 0;
    }
    if(m_parse_state==parse_state::HEADERS)
    {
        pos += parse_headers(data+pos,len-pos);
        if(m_failure)
            return 0;
        if(m_finished)
            return pos;
    }
    if(m_parse_state==parse_state::CONTENT)
    {
        pos += parse_content(data+pos,len-pos);
        if(m_failure)
            return 0;
        if(m_finished)
            return pos;
    }
    return pos;
}

bool request::finished() const {
    return m_finished;
}

bool request::failure() const {
    return m_failure;
}

string request::to_string() const {
    stringstream ss;
    if(m_method==http_method::GET)
        ss<<"GET ";
    else if(m_method==http_method::POST)
        ss<<"POST ";
    ss<<m_url<<" "<<m_version<<"\r\n";
    for(const auto&[k,v]:m_headers)
    {
        ss<<k<<":"<<v<<"\r\n";
    }
    ss<<"\r\n";
    ss<<m_data;
    return ss.str();
}

bool request::operator==(const request &o) const {
    return m_parse_state==o.m_parse_state
    &&m_method==o.m_method
    &&m_url==o.m_url
    &&m_version==o.m_version
    &&m_headers==o.m_headers
    &&m_data_len==o.m_data_len
    &&m_data==o.m_data
    &&m_failure==o.m_failure
    &&m_finished==o.m_finished;
}

request::request(request &&o) noexcept {
    /*
    m_parse_state = o.m_parse_state;
    m_method = o.m_method;
    m_url = move(o.m_url);
    m_version = move(o.m_version);
    m_headers = move(o.m_headers);
    m_data_len = o.m_data_len;
    m_data = move(o.m_data);
    m_failure = o.m_failure;
    m_finished = o.m_finished;
     */
    *this = move(o);
}

request &request::operator=(request &&o) noexcept {
    m_parse_state = o.m_parse_state;
    m_method = o.m_method;
    m_url = move(o.m_url);
    m_version = move(o.m_version);
    m_headers = move(o.m_headers);
    m_data_len = o.m_data_len;
    m_data = move(o.m_data);
    m_failure = o.m_failure;
    m_finished = o.m_finished;
    return *this;
}
