//
// Created by blacksungrass on 2021/12/20.
//

#include "file_response.h"
#include <sys/stat.h>
#include <bits/stdc++.h>
using namespace std;

file_response::file_response() {
    m_file_length = 0;
    m_file_path = "";
}

void file_response::set_file(const std::string &file_path) {
    if(m_file_length>0||!m_file_path.empty()){

    }

}

std::string file_response::to_string() const {
    return response::to_string();
}
