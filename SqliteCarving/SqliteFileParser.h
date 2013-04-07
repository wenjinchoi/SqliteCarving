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
    
    std::vector<unsigned char> pageAt(int index);

    int pageSize();
    
private:
    std::string file_;
    unsigned int pageSize_;
    unsigned long sizeOfPages_;
    unsigned long currentPage_;
    bool isAutoVacuum;
    std::vector<unsigned char> page_;
};

#endif /* defined(__SqliteCarving__SqliteFileParser__) */
