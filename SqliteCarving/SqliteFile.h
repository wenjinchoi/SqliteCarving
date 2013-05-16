//
//  SqliteFile.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/16/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqliteFile__
#define __SqliteCarving__SqliteFile__

#include <iostream>
#include <boost/shared_ptr.hpp>

#include "basedef.h"

class SqliteFile {
public:
    typedef std::vector<base::sql_type> SqlTypes;
    typedef std::vector<std::string> ColumnNames;
    typedef boost::shared_ptr<base::bytes_t> Bytes_ptr;
    
    enum ReadPageReturnTypes {
        kUnknown = -1,
        kSuccess,
        kPageNumOutOfRange,
        kOpenFileError,
    };
    
    const int kMinFileSize = 512;
    const std::string kSqliteHeader = "SQLite format 3";
    
    explicit SqliteFile(std::string& filepath)
        : filepath_(filepath)
    { }
    
    bool isSqliteFile();
    
    unsigned int pageSize() const;

    unsigned long numOfPages() const;
    
    // 同时提供了两个方法
    // 此方法不能返回错误码
    base::bytes_t pageAt(unsigned long pageNum) const;
    
    // 尽量用这个方法
    ReadPageReturnTypes readPageTo(unsigned long pageNum,
                                   Bytes_ptr bytes_ptr);
    
    
    bool isTableExists(std::string& tableName);
    
    SqlTypes sqlTypesFor(const std::string& tableName);
    
    ColumnNames columnNamesFor(std::string& tableName);
    
private:
    
    std::string filepath_;

};

#endif /* defined(__SqliteCarving__SqliteFile__) */
