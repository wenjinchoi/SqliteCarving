//
//  SqliteParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/19/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "SqliteParser.h"

namespace sp {

void SqliteParser::setSqliteFile(const std::string& filepath)
{
    sqliteFile_ = SqliteFile_ptr(new SqliteFile(filepath));
}

void SqliteParser::setTableName(const std::string& tableName)
{
    tableName_ = tableName;
}

SqliteParser::ParseReturnType SqliteParser::parseDeletedDatas() {
    if (!sqliteFile_->isSqliteFile()) {
        return kInvalidSqliteFile;
    }
    if (!sqliteFile_->isTableExists(tableName_)) {
        return kNonExistentTableName;
    }

    for (uint64_t pageNum = 2; pageNum < sqliteFile_->numOfPages();
         ++pageNum)
    {
        pageContent_ptr_ = SqliteFile::Bytes_ptr(new base::bytes_t);
        
        sqliteFile_->readPageTo(pageNum, pageContent_ptr_);
        currentPage_ptr_ = SqlitePage_ptr(new SqlitePage(*pageContent_ptr_));
        SqlitePage::BlockAreas freeBlocks = currentPage_ptr_->freeBlocks();
        sp::CellDatas cellDatas = parseFreeBlocks(freeBlocks);
        cellDatas_.insert(cellDatas_.end(),
                          cellDatas.begin(), cellDatas.end());
    }

    if (cellDatas_.empty()) {
        return kNoAnyResult;
    } else {
        return kSucess;
    }
}

sp::CellDatas SqliteParser::getDatas()
{
    return cellDatas_;
}

// TODO: CellDatas 需要重新定义 
sp::CellDatas SqliteParser::parseFreeBlocks(
    SqlitePage::BlockAreas& freeBlocks)
{
    sp::CellDatas result;
    for (SqlitePage::BlockAreas::iterator pos = freeBlocks.begin();
         pos != freeBlocks.end(); ++pos)
    {
        sp::FreeBlock *freeblock =
            new sp::FreeBlock(pageContent_ptr_->begin() + pos->begin, pageContent_ptr_->begin() + pos->end);
        
        SqliteFile::SqlTypes tmpl = sqliteFile_->sqlTypesFor(tableName_);
        tmpl.erase(tmpl.begin()); // 第一列的数据会被覆盖，移除之
        freeblock->setSqlTypeTmpl(tmpl);
        
        sp::CellDatas datas = freeblock->parseCellDatas();
        result.insert(result.end(), datas.begin(), datas.end());
        delete freeblock;
    }
    return result;
}

} // namespace sp