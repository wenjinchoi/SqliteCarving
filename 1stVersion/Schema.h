//
//  Schema.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//
//  已经被整合到 SqliteFile 中，本类即将废弃

#ifndef __SqliteCarving__Schema__
#define __SqliteCarving__Schema__

#include "basedef.h"

#include <iostream>
#include <string>

using std::string;
using std::vector;

class SchemaParser {
    string schema_;
    vector<string> columns_;
    vector<string> column_names_;
    vector<base::sql_type> sql_type_vec_;
    
public:
    SchemaParser() {}
    SchemaParser(string schema) : schema_(schema) {}
    
    // TODO: 实现新接口
    vector<string> column_names();
    vector<base::sql_type> sql_types();
    
    // 老接口
    vector<base::sql_type> parse();

private:
    base::sql_type getSqlTypeFor(string column);
};

#endif /* defined(__SqliteCarving__Schema__) */
