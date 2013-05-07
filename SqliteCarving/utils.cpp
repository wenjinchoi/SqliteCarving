//
//  utils.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/30/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "utils.h"
#include <bitset>

using std::bitset;
using std::make_pair;
using std::vector;

using std::cout;
using std::endl;

// walk max 9 bytes, while the MSB is 1, add the last 7 LSB's to the integer
base::varint_t parseVarint(base::bytes_it beg,
                           base::bytes_it end,
                           unsigned int offset) {
    base::bytes_it pos = beg + offset;
    unsigned long value = 0;
    bool isCompleted = false;
    int length = 0;
    while (length < 9 && pos < end && !isCompleted) {
        uint8_t viByte = *pos;
        bitset<8> b(viByte);
        // not the last varint byte
        if (b.test(7) && length < 8)
        {
            value = (value << 7) | (viByte & 0b01111111);
        }
        // last one, stop and finalyze the integer
        else if (b.test(7) && length == 8)
        {
            value = (value << 8) | (viByte);
            isCompleted = true;
        }
        // the end of the varint
        else
        {
            value= (value << 7) | (viByte & 0b01111111);
            isCompleted = true;
        }
        length += 1;
    }
    base::varint_t varint;
    if (isCompleted) {
        varint.value = value;
        varint.length = length;
    } else {
        varint.value = 0;
        varint.length = 0;
    }
    return varint;
}



bool isOdd(long num)
{
    return bool(num & 1);
}

/*
void testParseVarint() {
    
    vector<unsigned char> td1;
    td1.push_back(0x2B);
    td1.push_back(0x01);
    
    std::pair<int, unsigned long> r = ::parseVarint(td1, 0);
    cout << "first: " << r.first << " second: " << r.second << endl;
    
    vector<unsigned char> td2;
    td2.push_back(0x81);
    td2.push_back(0x00);
    r = ::parseVarint(td2, 0);
    cout << "first: " << r.first << " second: " << r.second << endl;
    
    vector<unsigned char> td3;
    td3.push_back(0x50);
    td3.push_back(0xE2);
    td3.push_back(0x43);
    td3.push_back(0x20);
    
}
 */

