//
//  SqlitePageParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include <algorithm>

#include "SqlitePageParser.h"

namespace sqliteparser {

bool isTableLeaf(base::bytes_t& page) {
    if (page.empty()) return false;
    return page.at(0) == 0x0D ? true : false;
}

bool isIndexLeaf(base::bytes_t& page)
{
    if (page.empty()) return false;
    return page.at(0) == 0x0A ? true : false;
}

tableLeafHeader getTableLeafHeader(base::bytes_t& page) {
    tableLeafHeader tlh;
    tlh.firstFreeBlockOffset =
        getValueFromMem<uint16_t>((char *)&page[1], 2);
    tlh.numOfCells = getValueFromMem<uint16_t>((char *)&page[3], 2);
    tlh.firstCellOffset = getValueFromMem<uint16_t>((char *)&page[5], 2);
    tlh.numOfFragments = getValueFromMem<uint8_t>((char *)&page[7], 1);
    return tlh;
}

vector<unsigned int> getCellPointers(base::bytes_it begin,
                                     int numOfCells) {
    vector<unsigned int> cellPointers;
    int offset =  0;
    for (int i = 0; i < numOfCells; ++i)
    {
        base::byte_t* b = &(*(begin + offset));
        unsigned int co = getValueFromMem<unsigned int>((char *)b, 2);
        cellPointers.push_back(co);
        offset += 2;
    }
    return cellPointers;
}

vector<base::cellInfo> getCellList(base::bytes_t& page) {
    base::bytes_it begin = page.begin() + 8;
    base::tableLeafHeader leafHeader = getTableLeafHeader(page);
    vector<unsigned int> cellPtrs =
        getCellPointers(begin, leafHeader.numOfCells);
    
    vector<base::cellInfo> cellInfos;
    for (vector<unsigned int>::iterator pos = cellPtrs.begin();
         pos != cellPtrs.end(); ++pos) {
        base::cellInfo cell_info;
        cell_info.offset = *pos;
        base::varint_t vint_playload = parseVarint(page.begin() + *pos, page.end());
        base::varint_t vint_rowid = parseVarint(page.begin() + *pos + vint_playload.length, page.end());
        cell_info.length = vint_playload.value + vint_playload.length + vint_rowid.length;
        cellInfos.push_back(cell_info);
    }
    return cellInfos;
}


vector<base::blockArea> getFreeBlockAreaList(base::bytes_t& page) {
    vector<base::blockArea> freeBlockAreaList;
    
    tableLeafHeader tlh = getTableLeafHeader(page);
    vector<base::cellInfo> cellList = getCellList(page);
    if (cellList.empty()) {
        return freeBlockAreaList;
    }
    base::blockArea first;
    first.begin = tlh.numOfCells * 2 + 8;
    first.end = cellList[cellList.size() - 1].offset - 1;
    freeBlockAreaList.push_back(first);
    
    if (cellList.size() > 1) {
        // Cell List 是自底向上增长的，所以这里使用反向迭代器遍历
        vector<base::cellInfo>::reverse_iterator rpos;
        for (rpos =  cellList.rbegin();
             rpos != cellList.rend(); ++rpos) {
            base::blockArea block;
            block.begin = rpos->offset;
            block.end = rpos->offset + rpos->length;
            vector<base::cellInfo>::reverse_iterator tmp_rpos = rpos + 1;
            // FIXIT: make 4 as a const var (最小空闲空间大小）
            if (block.end < tmp_rpos->offset - 4 &&
                block.end < page.size() - 4) {
                freeBlockAreaList.push_back(block);
            }
        }
    }
    return freeBlockAreaList;
    
}

/*
std::vector<std::pair<uint16_t, uint16_t> > freeBlockAreaList()
{
    
    uint16_t first_begin = numOfCells_ * 2 + 8;
    uint16_t first_end = cellList_[0] - 1;
    
    freeBlockAreaList_.clear();
    freeBlockAreaList_.push_back(std::make_pair(first_begin, first_end));
    
    //TODO: free block list
    vector<unsigned char> cellsArea;
    vector<uint16_t>::iterator pos;
    for (pos = cellList_.begin(); pos != cellList_.end(); ++pos) {
        cellsArea.clear();
        for (int i = *pos; i < *pos + 9; ++i) {
            cellsArea.push_back(page_[i]);
        }
        pair<int, unsigned long> varint = parseVarint(cellsArea);
        if (pos == cellList_.begin() &&
            (page_.size() - *pos - varint.second - varint.first) > 3)
        {
            freeBlockAreaList_.push_back(make_pair(*pos + varint.second,
                                                   page_.size() -1 ));
        }
        else if (*(pos-1) - (*pos + varint.second) > 3 )
        {
            freeBlockAreaList_.push_back(make_pair(*pos + varint.second,
                                                   *(pos - 1) - 1));
        }
    }
    
    return freeBlockAreaList_;
}

*/

} // namespace sqliteparser