//
//  FreeBlock.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/12/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__FreeBlock__
#define __SqliteCarving__FreeBlock__

#include "basedef.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>


using std::string;
using std::vector;
using std::map;

namespace sqliteparser {

typedef map<base::serial_type, base::sql_type> sTypeSqlTypeMap;
typedef map<base::serial_type, base::content_size> recordFormatMap;
    
typedef vector<base::sql_type> RecordTmpl;
typedef vector<string> Record; // 准备废弃
typedef vector<string> CellData;
    
struct RecordFormat {
    long serialType;
    long serialTypeSize;
    long contentSize;
};
    
    
class FreeBlock {    
public:
    typedef vector<string> CellData;

    // typedef vector<string> Records;
    
    
    
    FreeBlock();
    FreeBlock(base::bytes_t& bytes) : freeBlock_(bytes) {}
    FreeBlock(base::bytes_it begin, base::bytes_it end)
    : freeBlock_(begin, end) {}
    
    ~FreeBlock();
    
    // typedef std::vector<base::serial_type> CellHeader;
    
    size_t size();
    void setTemplate(RecordTmpl& recordTmpl);
    vector<CellData> parseCellDatas();

private:
    base::bytes_t freeBlock_;
    RecordTmpl recordTmpl_;
    std::vector<CellData> cellDatas_;
    
};

} // namespace sqliteparser

#endif /* defined(__SqliteCarving__FreeBlock__) */
