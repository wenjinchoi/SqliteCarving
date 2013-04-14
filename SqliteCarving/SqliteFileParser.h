//
//  SqliteFileParser.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 4/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqliteFileParser__
#define __SqliteCarving__SqliteFileParser__

#include "basedef.h"

namespace sqliteparse {
unsigned int pageSize(std::string sqliteFile);
unsigned long sizeOfPages(std::string sqliteFile);
bool isAutoVacuum(std::string sqliteFile);
base::bytes_t pageAt(std::string sqliteFile, int index);
} // namespace sqliteparse
    
#endif /* defined(__SqliteCarving__SqliteFileParser__) */
