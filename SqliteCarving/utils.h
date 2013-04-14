//
//  utils.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/30/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__utils__
#define __SqliteCarving__utils__

#include <iostream>
#include <vector>
#include <fstream>

std::pair<int, unsigned long> parseVarint(std::vector<unsigned char> varintBytes,
                                          int offset=0);

bool isOdd(int num);


template <typename T>
T getValueFromMem(char* memblock, int length) {
    T value = T();
    value <<= length * 8;
    value |= static_cast<T>(memblock[0]);
    int i = 1;
    while (i < length) {
        value <<= 8;
        value |= static_cast<T>(memblock[i]);
        ++i;
    }
    return value;
}

template <typename T>
T getValueFromFile(std::string file, long offset, unsigned int length) {
    std::ifstream input;
    input.open(file.c_str(),
               std::ios::in | std::ios::binary | std::ios::ate);
    T value = T();
    if (input.is_open()) {
        char* memblock = new char[length];
        input.seekg(offset, std::ios::beg);
        input.read(memblock, length);
        value = getValueFromMem<T>(memblock, length);
        delete [] memblock;
        input.close();
    } else {
        std::cout << "Unable to open file." << std::endl;
    }
    return value;
}

#endif /* defined(__SqliteCarving__utils__) */
