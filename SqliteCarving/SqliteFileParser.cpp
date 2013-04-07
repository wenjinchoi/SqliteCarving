//
//  SqliteFileParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "SqliteFileParser.h"
#include <fstream>

// FIXIT: Not good
void SqliteFileParser::setInputFile(std::string file)  {
    file_ = file;
    
    std::ifstream rfile;
    rfile.open(file_.c_str());
    rfile.seekg(16);
    
    char s1;
    rfile.get(s1);
    char s2;
    rfile.get(s2);
    pageSize_ = (int)s1<<8 | s2;
}

int SqliteFileParser::pageSize() {
    return pageSize_;
}

std::vector<unsigned char> SqliteFileParser::pageAt(int index) {
    currentPage_ = index;
    
    std::ifstream rfile;
    rfile.open(file_.c_str());
    
    int pos = (index - 1) * pageSize_;
    rfile.seekg(pos);
    
    int i = 0;
    while (i < pageSize_) {
        char c;
        rfile.get(c);
        page_.push_back(c);
        ++i;
    }
    
    return page_;
}
