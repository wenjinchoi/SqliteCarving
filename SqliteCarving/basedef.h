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
    unsigned long offset;
    unsigned long length;
};
  
// Block area: [begin, end]
struct blockArea {
    unsigned long begin;
    unsigned long end;
};

} // namespace base
#endif
