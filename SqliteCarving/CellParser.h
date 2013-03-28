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
using std::string;

void testo();
void testParseVarint();

class SqliteCarving {
public:
    void setFile(const string sqliteFile);
    void setParameters(const int sqlitePageSize);
    void carve();
private:
    int sqlitePageSize_;
    
    void parseTableLeafCell();
    void parseTableLeafCellPlayload();
    void parseColumnValue();
};

#endif /* defined(__SqliteCarving__CellParser__) */
