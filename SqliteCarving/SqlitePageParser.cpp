//
//  SqlitePageParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "SqlitePageParser.h"
#include <algorithm>

using namespace sqlparser;

bool SqlitePageParser::isTableLeafPage()
{
    if (page_.empty()) return false;
    return page_.at(0) == 0x0D ? true : false;
}

bool SqlitePageParser::isIndexLeafPage()
{
    if (page_.empty()) return false;
    return page_.at(0) == 0x0A ? true : false;
}

std::vector<uint16_t> SqlitePageParser::parseCellPointerArray(
    vec_char_it begin,
    int numOfCells)
{
    std::vector<uint16_t> cellPointers;
    int arrayOffset =  0;
    for (int i = 0; i < numOfCells; ++i)
    {
        uint8_t i1 = *(begin + arrayOffset);
        uint8_t i2 = *(begin + arrayOffset + 1);
        cellPointers.push_back(static_cast<uint16_t>(i1<<8 | i2));
        arrayOffset += 2;
    }
    return cellPointers;
}

std::vector<std::pair<uint16_t, uint16_t> > SqlitePageParser::freeBlockAreaList()
{
    
    uint16_t first_begin = numOfCells_ * 2 + 8;
    uint16_t first_end = cellList_[0] - 1;
    
    freeBlockAreaList_.clear();
    freeBlockAreaList_.push_back(std::make_pair(first_begin, first_end));
    
    //TODO: free block list
}

void SqlitePageParser::parsePage()
{
    firstFreeBlockOffset_ = static_cast<unsigned int>(page_[1]<<8 |
                                                      page_[2]);
    numOfCells_ = static_cast<unsigned int>(page_[3]<<8 | page_[4]);
    cellContentOffset_ = static_cast<unsigned int>(page_[5]<<8 | page_[6]);
    numOfFragments_ = static_cast<unsigned int>(page_[7]);
    
    cellList_ =  parseCellPointerArray(page_.begin() + 8,
                                                   numOfCells_);
}

