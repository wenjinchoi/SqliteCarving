//
//  CellParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "CellParser.h"

#include <bitset>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

const int SQL_TYPE_INT = 1;
const int SQL_TYPE_TEXT = 2;
const int SQL_TYPE_FLOAT = 4;
const int SQL_TYPE_BLOB = 8;
const int SQL_TYPE_NULL = 16;

const int STYPE_NULL = 0;
const int STYPE_INT8 = 1;
const int STYPE_INT16 = 2;
const int STYPE_INT24 = 3;
const int STYPE_INT32 = 4;
const int STYPE_INT48 = 5;
const int STYPE_INT64 = 6;
const int STYPE_FLOAT = 7;
const int STYPE_CONST0 = 8;
const int STYPE_CONST1 = 9;

namespace  {
typedef pair<int, int> SqlTypeInt;
typedef map<const int, SqlTypeInt> StypeSqlTypeMap;
StypeSqlTypeMap stypeSqlTypeMap;
} // namespace

void build_map()
{
    stypeSqlTypeMap.insert(make_pair(STYPE_NULL, make_pair(SQL_TYPE_NULL, 0)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT8, make_pair(SQL_TYPE_INT, 1)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT16, make_pair(SQL_TYPE_INT, 2)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT24, make_pair(SQL_TYPE_INT, 3)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT32, make_pair(SQL_TYPE_INT, 4)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT48, make_pair(SQL_TYPE_INT, 6)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT64, make_pair(SQL_TYPE_INT, 8)));
    stypeSqlTypeMap.insert(make_pair(STYPE_FLOAT, make_pair(SQL_TYPE_FLOAT, 8)));
    stypeSqlTypeMap.insert(make_pair(STYPE_CONST0, make_pair(SQL_TYPE_INT, 0)));
    stypeSqlTypeMap.insert(make_pair(STYPE_CONST1, make_pair(SQL_TYPE_INT, 0)));
}

 
typedef pair<int, unsigned> varint;

const unsigned char testdata[] = {
    0x2B, 0x01, 0x12, 0x00, 0x01, 0x17, 0x00, 0x04,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x30, 0x30,
    0x38, 0x36, 0x50, 0xE2, 0x43, 0x20, 0x00, 0xFF,
    0x01, 0x52, 0x65, 0x63, 0x6F, 0x72, 0x64, 0x20,
    0x31, 0x00, 0x00, 0x00, 0x00
};


// walk max 9 bytes, while the MSB is 1, add the last 7 LSB's to the integer
pair<int, unsigned long> parseVarint(vector<unsigned char> varintBytes, int offset=0) {
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

void testParseVarint() {
    
    vector<unsigned char> td1;
    td1.push_back(0x2B);
    td1.push_back(0x01);
    
    pair<int, unsigned long> r = ::parseVarint(td1, 0);
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

bool isOdd(int num)
{
    return bool(num & 1);
}

struct ColumnType {
    int sType;
    int colType;
    int size;
};

ColumnType make_columnType(int sType) {
    ColumnType columnType;
    columnType.sType = sType;
    StypeSqlTypeMap::iterator pos;
    build_map();
    pos = stypeSqlTypeMap.find(sType);
    if (pos != stypeSqlTypeMap.end()) {
        columnType.colType = pos->second.first;
        columnType.size = pos->second.second;
    }
    else if (sType > 11 && !isOdd(sType))
    {
        columnType.colType = SQL_TYPE_BLOB;
        columnType.size = (sType - 12)/2;
    }
    else if (sType > 12 && isOdd(sType))
    {
        columnType.colType = SQL_TYPE_TEXT;
        columnType.size = (sType - 13)/2;
    }
    else
    {
        // FIXIT: raise error
        cout << "Error sqlite serial type" << endl;
    }
    return columnType;
}

struct ColSizeValue
{
    int size;
    string value;
};


ColSizeValue parseColumeValue (ColumnType columnType, vector<unsigned char> playloadContect, int offset)
{
    ColSizeValue colSizeVal;
    colSizeVal.size = -1;
    colSizeVal.value = "";
    vector<unsigned char>::iterator pos = playloadContect.begin();
    switch (columnType.sType) {
        case STYPE_NULL:
            colSizeVal.size = 0;
            colSizeVal.value = "";
            break;
        case STYPE_INT8:
            colSizeVal.size = 1;
            colSizeVal.value = to_string(*playloadContect.begin());
            break;
        case STYPE_INT16:
            colSizeVal.size = 2;
            colSizeVal.value = to_string((uint16_t)*pos << 8 |
                                         *(pos + 1));
            break;
        case STYPE_INT24:
            colSizeVal.size = 3;
            colSizeVal.value = to_string((uint32_t)*pos << 16 |
                                         (uint32_t)*(pos + 1) << 8 |
                                         *(pos + 2));
            break;
        case STYPE_INT32:
            colSizeVal.size = 4;
            colSizeVal.value = to_string((uint32_t)*pos << 24 |
                                         (uint32_t)*(pos + 1) << 16 |
                                         (uint32_t)*(pos + 2) << 8 |
                                         *(pos + 3));
            break;
        case STYPE_INT48:
            ;
            break;
        case STYPE_INT64:
            ;
            break;
        case STYPE_FLOAT:
            ;
            break;
        case STYPE_CONST0:
            ;
            break;
        case STYPE_CONST1:
            ;
            break;
        default:
            if (columnType.colType == SQL_TYPE_BLOB)
            {
                
            }
            else if (columnType.colType == SQL_TYPE_TEXT)
            {
                
            }
            else
            {
                
            }
            break;
    }
    return colSizeVal;
}

void parseTableLeafCellPlayload(vector<unsigned char> playloadContent)
{
    int playloadOffset = 0;
    vector<ColumnType> cols;
    
    varint headerSize = parseVarint(playloadContent, playloadOffset);
    cout << "Header size: " << headerSize.second << endl;
    playloadOffset += headerSize.first;
    
    varint sType1 = parseVarint(playloadContent, playloadOffset);
    cols.push_back(make_columnType(sType1.second));
    cout << "sType 1: " << sType1.second << endl;
    playloadOffset += sType1.first;
    
    while (playloadOffset < headerSize.second) {
        varint sType = parseVarint(playloadContent, playloadOffset);
        cols.push_back(make_columnType(sType1.second));
        cout << "sType: " << sType.second << endl;
        playloadOffset += sType.first;
    }
    
    int offset = playloadOffset;
    vector<ColumnType>::iterator pos;
    for (pos = cols.begin(); pos != cols.end(); ++pos) {
        ColSizeValue colSizeValue =  parseColumeValue(*pos, playloadContent, offset);
        offset += colSizeValue.size;
        cout << "col Value: " << colSizeValue.value << endl;
    }
}
    
void parseTableLeafCell(vector<unsigned char> cellContent)
{
    int cellOffset = 0;
    varint playloadSize = parseVarint(cellContent, cellOffset);
    cout << "num bytes of playload: " << playloadSize.second << endl;
    cellOffset += playloadSize.first;
    
    varint intKey = parseVarint(cellContent, cellOffset);
    cout << "intKey: " << intKey.second << endl;
    cellOffset += intKey.first;
    
    // FIXIT:
    int sqlPageSize = 1024;
    bool overflow = playloadSize.second > (sqlPageSize - 35);
    if (overflow)
    {
        ;
    }
    else
    {
        vector<unsigned char>::iterator pos_start = cellContent.begin() + cellOffset;
        vector<unsigned char>::iterator pos_end = cellContent.begin() + playloadSize.second;
        vector<unsigned char> playloadContent(pos_start, pos_end);
        parseTableLeafCellPlayload(playloadContent);
    }
    cout << "Parsing cell done." << endl;
    
    
}

void testo()
{
    int length = sizeof(testdata)/sizeof(unsigned char);
    vector<unsigned char> vtd;
    for (int i = 0; i < length; ++i) {
        vtd.push_back(testdata[i]);
    }
    
    cout << "Test Data: " << endl;
    copy(vtd.begin(), vtd.end(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;
    
    parseTableLeafCell(vtd);
}
