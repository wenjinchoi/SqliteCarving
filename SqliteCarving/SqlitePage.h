//
//  SqlitePage.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/19/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqlitePage__
#define __SqliteCarving__SqlitePage__

#include <iostream>

#include "basedef.h"

namespace sp {

class SqlitePage {
public:
    enum PageType {
        kIndexInternalNode,
        kIndexLeafNode,
        kTableInternalNode,
        kTableLeafNode,
        kUnknown,
    };

    struct PageHeader {
        uint16_t firstFreeBlockOffset;
        uint16_t numOfCells;
        uint16_t firstCellOffset;
        uint8_t  numOfFragments;
        uint32_t reserved;  // for table leaf else
    };
    
    struct CellInfo {
        uint64_t offset;  // offset to page start
        uint64_t length;
    };
    
    struct BlockArea {
        uint64_t begin; // offset to page start
        uint64_t end;   // offset to page start
    };
    
    typedef std::vector<uint32_t> CellPointers;
    typedef std::vector<CellInfo> CellList;
    typedef std::vector<BlockArea> BlockAreas;
    
    
    explicit SqlitePage(base::bytes_t& page)
        : page_(page)
    { }
    
    SqlitePage(base::bytes_it begin, base::bytes_it end)
        : page_(begin, end)
    { }
    
    PageType pageType();
    
    PageHeader pageHeader();
    
    CellPointers cellPointers();
    
    CellList cellList();
    
    // 获取页的空闲块(FreeBlock)
    BlockAreas freeBlocks();
    
private:
    base::bytes_it cellPointersOffset();
    
    base::bytes_t page_;
    
};
    
} // namespace sp

#endif /* defined(__SqliteCarving__SqlitePage__) */
