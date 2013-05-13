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

// FIXIT: 需要移走到其他地方
typedef map<base::serial_type, base::sql_type> sTypeSqlTypeMap;
typedef map<base::serial_type, base::content_size> recordFormatMap;
    
typedef vector<base::sql_type> SqlTypeTmpl;
typedef vector<string> CellData;
    
struct RecordFormat {
    long serialType;
    long serialTypeSize;
    long contentSize;
};

typedef vector<RecordFormat> RecordFormats;
    
class FreeBlock {    
public:    
    FreeBlock();
    FreeBlock(base::bytes_t& bytes) : freeBlock_(bytes) {}
    FreeBlock(base::bytes_it begin, base::bytes_it end)
      : freeBlock_(begin, end) {}
    ~FreeBlock();
    
    // 返回 FreeBlock 的大小（字节数）
    size_t size();
    
    // 设置用于匹配的 SQL Type 模版
    void setSqlTypeTmpl(SqlTypeTmpl& sqlTypeTmpl);
    
    // 根据设置的模版解析 CellData
    // 如果没有设置模版，则返回空数组
    vector<CellData> parseCellDatas();

private:
    base::bytes_t freeBlock_;
    SqlTypeTmpl sqlTypeTmpl_;
    std::vector<CellData> cellDatas_;
    
    string getData(base::bytes_it data_start,
                         RecordFormats::iterator rf_pos);
    
    CellData parseData(base::bytes_it data_start,
                       RecordFormats& rf);
    
    RecordFormats matchBytes(base::bytes_it begin);
    
};

} // namespace sqliteparser

#endif /* defined(__SqliteCarving__FreeBlock__) */
