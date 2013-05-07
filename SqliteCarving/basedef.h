//
//  basedef.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/15/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef SqliteCarving_basedef_h
#define SqliteCarving_basedef_h

#include <iostream>
#include <vector>

namespace base {

typedef int sql_type;
typedef long serial_type;
typedef long content_size;
    
    
typedef uint8_t byte_t;
typedef std::vector<uint8_t> bytes_t;
typedef bytes_t::iterator bytes_it;

struct varint_t {
    long long value;
    int length;
};
    
struct tableLeafHeader {
    uint16_t firstFreeBlockOffset;
    uint16_t numOfCells;
    uint16_t firstCellOffset;
    uint8_t  numOfFragments;
};
    
struct cellInfo {
    uint64_t offset;
    uint64_t length;
};
  
// Block area: [begin, end]
struct blockArea {
    uint64_t begin;
    uint64_t end;
};

// SQL Type
const int SQL_TYPE_INT = 1;
const int SQL_TYPE_TEXT = 2;
const int SQL_TYPE_FLOAT = 4;
const int SQL_TYPE_BLOB = 8;
const int SQL_TYPE_NULL = 16;

// Ref: http://sqlite.org/fileformat2.html
// Serial Type | Content Size
//                             含义         数据宽度(字节数)
const int STYPE_NULL   = 0;  // NULL        0
const int STYPE_INT8   = 1;  // signed int  1
const int STYPE_INT16  = 2;  // signed int  2
const int STYPE_INT24  = 3;  // signed int  3
const int STYPE_INT32  = 4;  // signed int  4
const int STYPE_INT48  = 5;  // signed int  6
const int STYPE_INT64  = 6;  // signed int  8
const int STYPE_FLOAT  = 7;  // IEEE float  8
const int STYPE_CONST0 = 8;  // const 0     0
const int STYPE_CONST1 = 9;  // const 1     0
const int STYPE_BLOB   = 12; // blob        (N-12)/2
const int STYPE_TEXT   = 13; // text        (N-13)/2
    
    
} // namespace base
#endif
