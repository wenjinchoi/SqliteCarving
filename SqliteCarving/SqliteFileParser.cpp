//
//  SqliteFileParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include <fstream>

#include "SqliteFileParser.h"

unsigned int sqliteparser::pageSize(std::string sqliteFile) {
    return getValueFromFile<unsigned int>(sqliteFile, 16, 2);
}

unsigned long sqliteparser::sizeOfPages(std::string sqliteFile) {
    return getValueFromFile<unsigned long>(sqliteFile, 28, 4);
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
