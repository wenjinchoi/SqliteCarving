//
//  Schema.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__Schema__
#define __SqliteCarving__Schema__

#include "basedef.h"

#include <iostream>
#include <string>


using std::string;

class SchemaParser {
    string schema_;
    std::vector<string> column_vec_;
    std::vector<base::sql_type> sql_type_vec_;
    
public:
    SchemaParser() {}
    SchemaParser(string schema) : schema_(schema) {}
    
    std::vector<base::sql_type> parse();

private:
    base::sql_type getSqlTypeFor(string column);
};

#endif /* defined(__SqliteCarving__Schema__) */
