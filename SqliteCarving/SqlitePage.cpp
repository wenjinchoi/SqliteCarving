//
//  SqlitePage.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/19/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "SqlitePage.h"

#include "utils.h"

namespace {
const base::byte_t kByteOfIndexInternalNode  = 0x02;
const base::byte_t kByteOfIndexLeafNode      = 0x0A;
const base::byte_t kByteOfITableInternalNode = 0x05;
const base::byte_t kByteOfTableLeafNode      = 0x0D;
    
const int kInternalNodeCellPointersOffset = 12;
const int kLeafNodeCellPointersOffset = 8;
    
const int kMinFreeBlockSize = 4;
}

SqlitePage::PageType SqlitePage::pageType()
{
    switch (page_.at(0)) {
        case kByteOfIndexInternalNode:
            return kIndexInternalNode;
        case kByteOfIndexLeafNode:
            return kIndexLeafNode;
        case kByteOfITableInternalNode:
            return kTableInternalNode;
        case kByteOfTableLeafNode:
            return kTableLeafNode;
        default:
            return kUnknown;
    }
}

SqlitePage::PageHeader SqlitePage::pageHeader()
{
    PageHeader header;
    header.firstFreeBlockOffset = getValueFromMem<uint16_t>((char *)&page_[1], 2);
    header.numOfCells = getValueFromMem<uint16_t>((char *)&page_[3], 2);
    header.firstCellOffset = getValueFromMem<uint16_t>((char *)&page_[5], 2);
    header.numOfFragments = getValueFromMem<uint8_t>((char *)&page_[7], 1);
    header.reserved = 0;
    return header;
}



SqlitePage::CellPointers SqlitePage::cellPointers()
{
    CellPointers cellPointers;
    
    base::bytes_it cellPtrsStart = cellPointersOffset();
    
    int offset =  0;
    for (int i = 0; i < pageHeader().numOfCells; ++i)
    {
        base::byte_t* b = &(*(cellPtrsStart + offset));
        uint32_t cellOffset = getValueFromMem<uint32_t>((char *)b, 2);
        cellPointers.push_back(cellOffset);
        offset += 2;
    }
    return cellPointers;
}

SqlitePage::CellList SqlitePage::cellList() {
    CellList cellList;
    CellPointers cellPtrs = cellPointers();
    for (CellPointers::iterator pos = cellPtrs.begin();
         pos != cellPtrs.end(); ++pos) {
        CellInfo cellInfo;
        cellInfo.offset = *pos;
        base::varint_t vint_playload = parseVarint(page_.begin() + *pos, page_.end());
        base::varint_t vint_rowid = parseVarint(page_.begin() + *pos + vint_playload.length, page_.end());
        cellInfo.length = vint_playload.value + vint_playload.length + vint_rowid.length;
        cellList.push_back(cellInfo);
    }
    return cellList;
}

SqlitePage::BlockAreas SqlitePage::freeBlocks()
{
    BlockAreas freeBlockAreaList;
    
    CellList cellList = this->cellList(); // FIXIT
    if (cellList.empty()) {
        return freeBlockAreaList;
    }
    
    BlockArea firstBlock;
    PageHeader header = pageHeader();
    // FIXIT: 8 is not good
    firstBlock.begin = header.numOfCells * 2 + 8;
    firstBlock.end = cellList[cellList.size() - 1].offset - 1;
    freeBlockAreaList.push_back(firstBlock);
    
    if (cellList.size() > 1) {
        // Cell List 是自底向上增长的，所以这里使用反向迭代器遍历
        CellList::reverse_iterator rpos;
        for (rpos =  cellList.rbegin();
             rpos != cellList.rend(); ++rpos) {
            BlockArea block;
            block.begin = rpos->offset;
            block.end = rpos->offset + rpos->length;
            CellList::reverse_iterator tmp_rpos = rpos + 1;
            if (block.end < tmp_rpos->offset - kMinFreeBlockSize &&
                block.end < page_.size() - kMinFreeBlockSize) {
                freeBlockAreaList.push_back(block);
            }
        }
    }
    return freeBlockAreaList;
}

base::bytes_it SqlitePage::cellPointersOffset() {
    base::bytes_it cellPtrsOffset;
    if (pageType() == kIndexInternalNode ||
        pageType() == kTableInternalNode) {
        cellPtrsOffset = page_.begin() + kInternalNodeCellPointersOffset;
    } else {
        cellPtrsOffset = page_.begin() + kLeafNodeCellPointersOffset;
    }
    return cellPtrsOffset;
}
