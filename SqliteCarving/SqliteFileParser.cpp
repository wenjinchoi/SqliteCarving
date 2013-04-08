//
//  SqliteFileParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "SqliteFileParser.h"
#include <fstream>

void SqliteFileParser::setInputFile(std::string file)  {
    file_ = file;
    
    std::ifstream rfile;
    rfile.open(file_.c_str(),
               std::ios::in | std::ios::binary | std::ios::ate);
    
    if (rfile.is_open())
    {
        char* memblock1 = new char[2];
        rfile.seekg(16, std::ios::beg);
        rfile.read(memblock1, 2);
        pageSize_ = static_cast<unsigned int>(memblock1[0]<<8 | memblock1[1]);
        delete [] memblock1;
        
        char* memblock2 = new char[4];
        rfile.seekg(28, std::ios::beg);
        rfile.read(memblock2, 4);
        sizeOfPages_ = static_cast<unsigned long>(memblock2[0]<<24 |
                                                  memblock2[1]<<16 |
                                                  memblock2[2]<<8 |
                                                  memblock2[3]);
        delete [] memblock2;
        
        char* memblock3 = new char[4];
        rfile.seekg(52, std::ios::beg);
        rfile.read(memblock3, 4);
        long b = static_cast<long>(memblock3[0]<<24 |
                                   memblock3[1]<<16 |
                                   memblock3[2]<<8 |
                                   memblock3[3]);
        isAutoVacuum_ = b != 0 ? true : false;
        delete [] memblock3;
        
        rfile.close();
    }
    else
    {
        std::cout << "Unable to open file." << std::endl;
    }
}

int SqliteFileParser::pageSize() {
    return pageSize_;
}

std::vector<char> SqliteFileParser::pageAt(int index) {
    currentPage_ = index;
    
    std::ifstream rfile;
    rfile.open(file_.c_str(),
               std::ios::in | std::ios::binary | std::ios::ate);
    std::fstream::pos_type pos = (index - 1) * pageSize_;
    
    if (rfile.is_open()) {
        rfile.seekg(pos, std::ios::beg);
        char* memblock = new char[pageSize_];
        rfile.read(memblock, pageSize_);
        for (int i = 0; i < pageSize_; ++i) {
            page_.push_back(memblock[i]);
        }
        rfile.close();
        delete[] memblock;
        return page_;
    }
    else {
        std::cout << "Unable to open file." << std::endl;
        page_.clear();
        return page_;
    }
}
