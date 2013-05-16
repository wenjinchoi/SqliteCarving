//
//  SqliteFileParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include <fstream>

#include "SqliteFileParser.h"

//std::ifstream::pos_type filesize(const char* filename)
//{
//    std::ifstream in(filename, std::ifstream::in | std::ifstream::binary);
//    in.seekg(0, std::ifstream::end);
//    return in.tellg();
//}

unsigned int sqliteparser::pageSize(std::string sqliteFile) {
    return getValueFromFile<unsigned int>(sqliteFile, 16, 2);
}

unsigned long sqliteparser::sizeOfPages(std::string sqliteFile) {
    unsigned long size =
        getValueFromFile<unsigned long>(sqliteFile, 28, 4);
    if (size != 0) {
        return size;
    } else {
        unsigned long num =
            filesize(sqliteFile.c_str())/pageSize(sqliteFile);
        return num;
    }
}

bool sqliteparser::isAutoVacuum(std::string sqliteFile) {
    int v = getValueFromFile<int>(sqliteFile, 52, 4);
    return static_cast<bool>(v);
}

base::bytes_t sqliteparser::pageAt(std::string sqliteFile, int index) {
    base::bytes_t page;
    page.clear();
    
    if (sqliteFile.empty()) {
        std::cout << "The input file is undefined." << std::endl;
        return page;
    }
    
    int pageSize = sqliteparser::pageSize(sqliteFile);
    std::ifstream file;
    file.open(sqliteFile.c_str(),
               std::ios::in | std::ios::binary | std::ios::ate);
    std::fstream::pos_type pos = (index - 1) * pageSize;
    if (file.is_open()) {
        file.seekg(pos, std::ios::beg);
        page.resize(pageSize);
        file.read((char *)&page[0], pageSize);
        file.close();
    } else {
        std::cout << "Unable to open file." << std::endl;
    }
    return page;
}
