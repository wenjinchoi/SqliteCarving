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
#include <string>
#include <vector>

#include "utils.h"

using std::string;

typedef std::pair<int, unsigned> varint;
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

#endif /* defined(__SqliteCarving__CellParser__) */
