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


vector<base::sql_type> testTmpl();
    
vector<Record> parseRecordsFromFreeBlock(base::bytes_it begin,
                                         base::bytes_it end,
                                         vector<base::sql_type> tmpl);
    
} // namespace sqliteparser


// ------ old -------
/*
using std::string;

// typedef std::pair<int, unsigned> varint;
typedef std::vector<unsigned char> vec_uchar;

void build_map();
void parseTableLeafCell(vec_uchar cellContent);

class SqliteCarving {
public:
    SqliteCarving();
    ~SqliteCarving();
    
    void setFile(const string sqliteFile);
    void setParameters(const int sqlitePageSize);
    void carve();

private:
    void parseTableLeafCell();
    void parseTableLeafCellPlayload();
    void parseColumnValue();
    
    int sqlitePageSize_;
};
*/
 
#endif /* defined(__SqliteCarving__CellParser__) */
