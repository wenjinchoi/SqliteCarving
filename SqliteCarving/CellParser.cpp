//
//  CellParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "CellParser.h"
#include "utils.h"

#include <bitset>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

// SQL Type
const int SQL_TYPE_INT = 1;
const int SQL_TYPE_TEXT = 2;
const int SQL_TYPE_FLOAT = 4;
const int SQL_TYPE_BLOB = 8;
const int SQL_TYPE_NULL = 16;

// Column Type
//                             含义         数据宽度(字节数)
const int STYPE_NULL = 0;   // NULL        0
const int STYPE_INT8 = 1;   // signed int  1
const int STYPE_INT16 = 2;  // signed int  2
const int STYPE_INT24 = 3;  // signed int  3
const int STYPE_INT32 = 4;  // signed int  4
const int STYPE_INT48 = 5;  // signed int  6
const int STYPE_INT64 = 6;  // signed int  8
const int STYPE_FLOAT = 7;  // IEEE float  8
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

const unsigned char testdata[] = {
    0x2B, 0x01, 0x12, 0x00, 0x01, 0x17, 0x00, 0x04,
    0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x00,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x30, 0x30,
    0x38, 0x36, 0x50, 0xE2, 0x43, 0x20, 0x00, 0xFF,
    0x01, 0x52, 0x65, 0x63, 0x6F, 0x72, 0x64, 0x20,
    0x31, 0x00, 0x00, 0x00, 0x00
};

 
typedef pair<int, unsigned> varint;
typedef vector<unsigned char> vec_uchar;


struct ColumnType {
    int sType;    // Type value  sType col 1 (varint) ... col N (varint)
    int colType;  // Column Type
    int size;     // Data Length
};

struct ColSizeValue
{
    int size;
    string value;
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

uint64_t vec_uchar2uint64_t(vec_uchar::iterator pos, size_t length)
{
    uint64_t value = static_cast<uint64_t>(*pos);
    size_t i = 1;
    while (i < length) {
        value <<= 8;
        value |= static_cast<uint64_t>(*(pos + i));
        ++i;
    }
    return value;
}

float vec_uchar2float(vec_uchar::iterator pos, size_t length)
{
    uint64_t value = vec_uchar2uint64_t(pos, length);
    // FIXIT: may be not good.
    return *((float *)&value);
}

ColSizeValue parseColumeValue (ColumnType columnType,
                               vec_uchar playloadContent,
                               int offset)
{
    ColSizeValue colSizeVal;
    colSizeVal.size = -1;
    colSizeVal.value = "";
    vec_uchar::iterator pos = playloadContent.begin() + offset;
    switch (columnType.sType) {
        case STYPE_NULL:
            colSizeVal.size = 0;
            colSizeVal.value = "Null";
            break;
        case STYPE_INT8:
            colSizeVal.size = 1;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT16:
            colSizeVal.size = 2;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT24:
            colSizeVal.size = 3;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT32:
            colSizeVal.size = 4;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
             break;
        case STYPE_INT48:
            colSizeVal.size = 6;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT64:
            colSizeVal.size = 8;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_FLOAT:
            colSizeVal.size = 8;
            colSizeVal.value = to_string(vec_uchar2float(pos, colSizeVal.size));
            break;
        case STYPE_CONST0:
            colSizeVal.size = 0;
            colSizeVal.value = to_string(0);
            break;
        case STYPE_CONST1:
            colSizeVal.size = 0;
            colSizeVal.value = to_string(1);
            break;
        default:
            if (columnType.colType == SQL_TYPE_BLOB)
            {
                // TODO: finish it.
            }
            else if (columnType.colType == SQL_TYPE_TEXT)
            {
                colSizeVal.size = columnType.size;
                string s(pos, pos + columnType.size);
                colSizeVal.value = s;
            }
            else
            {
                cout << "Unknown column type: " << columnType.sType << endl;
            }
            break;
    }
    return colSizeVal;
}

void parseTableLeafCellPlayload(vec_uchar playloadContent)
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
        cols.push_back(make_columnType(sType.second));
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
    
void parseTableLeafCell(vec_uchar cellContent)
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
        vec_uchar::iterator pos_start = cellContent.begin() + cellOffset;
        vec_uchar::iterator pos_end = cellContent.begin() + playloadSize.second;
        vec_uchar playloadContent(pos_start, pos_end);
        parseTableLeafCellPlayload(playloadContent);
    }
    cout << "Parsing cell done." << endl;
    
    
}

void testo()
{
    int length = sizeof(testdata)/sizeof(unsigned char);
    vec_uchar vtd;
    for (int i = 0; i < length; ++i) {
        vtd.push_back(testdata[i]);
    }
    
    cout << "Test Data: " << endl;
    copy(vtd.begin(), vtd.end(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;
    
    parseTableLeafCell(vtd);
}
