//
//  SqliteParser.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/19/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__SqliteParser__
#define __SqliteCarving__SqliteParser__

#include <iostream>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

#include "SqliteFile.h"
#include "SqlitePage.h"
#include "FreeBlock.h"

namespace sp {

class SqliteParser {
public:
    typedef boost::shared_ptr<SqliteFile> SqliteFile_ptr;
    typedef boost::shared_ptr<SqlitePage> SqlitePage_ptr;
    
    enum ParseReturnType {
        kSucess,
        kInvalidSqliteFile,
        kNonExistentTableName,
        kNoAnyResult,
    };
    
    // SqliteParser();
    
    void setSqliteFile(const std::string& filepath);
    void setTableName(const std::string& tableName);
    
    ParseReturnType parseDeletedDatas();
    
    sp::CellDatas getDatas();
    
private:
    sp::CellDatas parseFreeBlocks(SqlitePage::BlockAreas& freeBlocks);
    
    SqliteFile_ptr sqliteFile_;
    SqliteFile::Bytes_ptr pageContent_ptr_;
    SqlitePage_ptr currentPage_ptr_;
    std::string tableName_;
    
    sp::CellDatas cellDatas_;
    
};
    
} // namespace sp

#endif /* defined(__SqliteCarving__SqliteParser__) */
