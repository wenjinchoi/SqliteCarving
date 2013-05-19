//
//  CellParser.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__CellParser__
#define __SqliteCarving__CellParser__

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "basedef.h"
#include "utils.h"

using std::vector;
using std::map;
using std::string;

namespace sqliteparser {

typedef vector<base::sql_type> RecordTmpl;
typedef vector<string> Records;
    
typedef vector<string> Record;
    
typedef map<base::serial_type, base::sql_type> sTypeSqlTypeMap;
typedef map<base::serial_type, base::content_size> recordFormatMap;
    
struct RecordFormat {
    long serialType;
    long serialTypeSize;
    long contentSize;
};

// FIXIT: 只用作测试，待废弃
vector<base::sql_type> testTmpl();
    
vector<Record> parseRecordsFromFreeBlock(base::bytes_it begin,
                                         base::bytes_it end,
                                         vector<base::sql_type> tmpl);
    
} // namespace sqliteparser
 
#endif /* defined(__SqliteCarving__CellParser__) */
