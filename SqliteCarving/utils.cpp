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
std::pair<int, unsigned long> parseVarint(vector<unsigned char> varintBytes, int offset) {
    vector<unsigned char>::iterator pos = varintBytes.begin() + offset;
    vector<unsigned char> viBytes(pos, varintBytes.end());
    unsigned long value = 0;
    bool isCompleted = false;
    int byte_num = 0;
    while (byte_num < 9 && byte_num < viBytes.size() && !isCompleted) {
        uint8_t viByte = uint8_t(viBytes[byte_num]);
        bitset<8> b(viBytes[byte_num]);
        // not the last varint byte
        if (b.test(7) && byte_num < 8)
        {
            value = (value << 7) | (viByte & 0b01111111);
        }
        // last one, stop and finalyze the integer
        else if (b.test(7) && byte_num == 8)
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
        byte_num += 1;
    }
    if (isCompleted) {
        return make_pair(byte_num, value);
    }
    else
    {
        return make_pair(0, 0);
    }
}

bool isOdd(int num)
{
    return bool(num & 1);
}

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