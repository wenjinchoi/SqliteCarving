//
//  SqliteFileParser.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqliteFileParser__
#define __SqliteCarving__SqliteFileParser__

#include <iostream>
#include <vector>
#include <string>

class SqliteFileParser {
public:
    SqliteFileParser(): pageSize_(0),
                        sizeOfPages_(0),
                        currentPage_(1) {}
    
    void setInputFile(std::string file);
    
    int pageSize(){ return pageSize_; }
    
    unsigned long sizeOfPages() { return sizeOfPages_; }
    
    std::vector<char> pageAt(int index);
    
private:
    std::string file_;
    unsigned long currentPage_;
    
    unsigned int pageSize_;
    unsigned long sizeOfPages_;
    bool isAutoVacuum_;
    
    std::vector<char> page_;
};

#endif /* defined(__SqliteCarving__SqliteFileParser__) */
