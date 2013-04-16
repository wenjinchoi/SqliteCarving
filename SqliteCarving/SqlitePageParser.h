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

#include "basedef.h"
#include "utils.h"

using std::vector;

namespace sqliteparser {
    
bool isTableLeaf(base::bytes_t& page);
bool isIndexLeaf(base::bytes_t& page);

using base::tableLeafHeader;
tableLeafHeader getTableLeafHeader(base::bytes_t& page);

// 从页中，获取 cell 指针列表
vector<unsigned int> getCellPointers(base::bytes_it begin,
                                     unsigned int numOfCells);

// 获取页中 cellInfo 列表，并计算出每个 Cell 的长度
// 指向已存在的数据，包含了每条数据 offset 和 length
vector<base::cellInfo> getCellList(base::bytes_t& page);

// 获取页中的空闲块列表（vector）
vector<base::blockArea> getFreeBlockAreaList(base::bytes_t& page);
    
} // namespace sqlparser

#endif /* defined(__SqliteCarving__SqlitePageParser__) */
