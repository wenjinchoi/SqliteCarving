//
//  SqlitePageParser.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqlitePageParser__
#define __SqliteCarving__SqlitePageParser__

#include <iostream>
#include <vector>

namespace sqlparser {
typedef std::vector<char> vec_char;
typedef std::vector<char>::iterator vec_char_it;

class SqlitePageParser {
public:
    SqlitePageParser() {}
    SqlitePageParser(std::vector<char> page) { page_ = page; }

    bool isTableLeafPage();
    bool isIndexLeafPage();
    
    void parsePage();
    
    int numOfCells() { return numOfCells_; }
    int numOfFragments() { return  numOfFragments_; }
    int firstFreeBlockOffset() { return firstFreeBlockOffset_; }
    
    std::vector<uint16_t> cellList() { return cellList_; }
    std::vector<std::pair<int, int> > freeBlockList();
    
    std::vector<std::pair<uint16_t, uint16_t> > freeBlockAreaList();

protected:
    std::vector<uint16_t> parseCellPointerArray(vec_char_it begin,
                               int numOfCells);
    
private:
    std::vector<char> page_;
    
    unsigned int firstFreeBlockOffset_;
    unsigned int numOfCells_;
    unsigned int cellContentOffset_;
    unsigned int numOfFragments_;
    
    std::vector<uint16_t> cellList_;
    std::vector<std::pair<uint16_t, uint16_t> > freeBlockAreaList_;
    
};
} // namespace sqlparser

#endif /* defined(__SqliteCarving__SqlitePageParser__) */
