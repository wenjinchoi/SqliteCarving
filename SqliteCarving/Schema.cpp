//
//  Schema.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/7/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "Schema.h"

#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

void command() {
    char command[100] = "sqlite3 /Users/wenjinchoi/Desktop/mmssms_del5.db \".schema sms\"";
    char output[512];
    FILE *fp = popen(command, "r");
    while (!feof(fp)) {
        fgets(output, sizeof(output), fp);
        printf("%s", output);
    }
}

base::sql_type SchemaParser::getSqlTypeFor(string column) {
    base::sql_type sql_type = 0;
    if (column.find("INTEGER") != string::npos) {
        sql_type |= base::SQL_TYPE_INT;
    }
    if (column.find("FLOAT") != string::npos) {
        sql_type |= base::SQL_TYPE_FLOAT;
    }
    if (column.find("TEXT") != string::npos) {
        sql_type |= base::SQL_TYPE_TEXT;
    }
    if (column.find("BLOB") != string::npos) {
        sql_type |= base::SQL_TYPE_BLOB;
    }
    if (!(column.find("DEFAULT") != string::npos) &&
        !(column.find("PRIMARY KEY") != string::npos)) {
        sql_type |= base::SQL_TYPE_NULL;
    }
    return sql_type;
}

std::vector<base::sql_type> SchemaParser::parse() {
    string tmp = schema_;
    tmp.erase(tmp.begin(), tmp.begin() + tmp.find("("));
    boost::split(column_vec_, tmp, boost::is_any_of(","));
    
//    for debug
//    std::copy(sVec.begin(), sVec.end(),
//              std::ostream_iterator<string>(std::cout, "\n"));
//    std::cout << std::endl;
    
    
    std::vector<string>::iterator pos;
    for (pos = column_vec_.begin(); pos != column_vec_.end(); ++pos) {
        base::sql_type sql_type = getSqlTypeFor(*pos);
        sql_type_vec_.push_back(sql_type);
    }

    return sql_type_vec_;
}