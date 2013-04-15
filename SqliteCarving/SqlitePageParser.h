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
            
vector<unsigned int> getCellPointers(base::bytes_it begin,
                                     unsigned int numOfCells);

vector<base::cellInfo> getCellList(base::bytes_t& page);

vector<base::blockArea> getFreeBlockAreaList(base::bytes_t& page);
    
} // namespace sqlparser

#endif /* defined(__SqliteCarving__SqlitePageParser__) */
