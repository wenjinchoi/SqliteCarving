//
//  SqliteFile.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/16/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//


#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cctype> // for toupper
#include <fstream>
#include <sstream>

#include "SqliteFile.h"
#include "utils.h"


bool SqliteFile::isSqliteFile() {
    unsigned long fileSize = filesize(filepath_.c_str());
    if (fileSize >= kMinFileSize) {
        Bytes_ptr bytes_ptr(new base::bytes_t);
        if (kSuccess != this->readPageTo(1, bytes_ptr)) {
            return false;
        }
        std::ostringstream convert;
        copy(bytes_ptr->begin(), bytes_ptr->begin() + 15,
             std::ostream_iterator<base::byte_t>(convert,""));
        std::string result = convert.str();

        if (kSqliteHeader == result) {
            return true;
        } else {
            std::cout << result << std::endl;
            std::cout << kSqliteHeader << std::endl;
            return false;
        }
    } else {
        return false;
    }
}

unsigned int SqliteFile::pageSize() const {
    return getValueFromFile<unsigned int>(filepath_, 16, 2);
}

unsigned long SqliteFile::numOfPages() const {
    unsigned long size =
        getValueFromFile<unsigned long>(filepath_, 28, 4);
    if (size != 0) {
        return size;
    } else {
        unsigned long num = filesize(filepath_.c_str())/pageSize();
        return num;
    }
}

// return page, but status is unknown
base::bytes_t SqliteFile::pageAt(unsigned long pageNum) const {
    base::bytes_t page;
    page.clear();
    
    // TODO: 检查路径是否合法
    // TODO: 检查 pageNum 是否越界
    unsigned int pageSize = this->pageSize();
    std::ifstream file;
    file.open(filepath_.c_str(),
              std::ios::in | std::ios::binary | std::ios::ate);
    std::fstream::pos_type pos = (pageNum - 1) * pageSize;
    if (file.is_open()) {
        file.seekg(pos, std::ios::beg);
        page.resize(pageSize);
        file.read((char *)&page[0], pageSize);
        file.close();
    } else {
        // TODO: Log it
    }
    return page;
}

// return status
SqliteFile::ReadPageReturnTypes SqliteFile::readPageTo(
    unsigned long pageNum,
    Bytes_ptr bytes_ptr)
{
    ReadPageReturnTypes ret = kUnknown;
    if (pageNum > this->numOfPages()) {
        ret = kPageNumOutOfRange;
    } else {
        unsigned int pageSize = this->pageSize();
        std::ifstream file;
        file.open(filepath_.c_str(),
                  std::ios::in | std::ios::binary | std::ios::ate);
        std::fstream::pos_type pos = (pageNum - 1) * pageSize;
        if (file.is_open()) {
            file.seekg(pos, std::ios::beg);
            bytes_ptr->clear();
            bytes_ptr->resize(pageSize);
            // TODO: 是否可以优化？例如 back_inserter
            char *memblock = new char[pageSize];
            file.read(memblock, pageSize);
            file.close();
            copy(&memblock[0], &memblock[pageSize], bytes_ptr->begin());
            delete memblock;
            ret = kSuccess;
        } else {
            ret = kOpenFileError;
        }
    }
    return ret;
}

bool SqliteFile::isTableExists(std::string& tableName) {
    // TODO
    return false;
}

// TODO: not good
std::string getCreateTableSQLFor(const std::string& dbFilePath,
                                 const std::string& tableName) {
    std::string command = "sqlite3 ";
    command += dbFilePath;
    command += " \".schema ";
    command += tableName;
    command += "\"";
    
    char output[4096];
    std::string schema;
    FILE *fp = popen(command.c_str(), "r");
    while(!feof(fp)) {
        while (fgets(output, sizeof(output), fp) != NULL) {
            schema.append(output);
        }
    }
    pclose(fp);
    
    std::string search_str("CREATE TABLE ");
    search_str += tableName;
    size_t beg = upperString(schema).find(upperString(search_str));
    size_t end = upperString(schema).find(";", beg);
    std::string result;
    if (beg == std::string::npos) {
        result = "";
    } else {
        result.assign(schema, beg, end - beg);
    }
    return result;
    
}

// return 组合后的 SqlType
base::sql_type getSqlTypeFor(std::string column) {
    base::sql_type sql_type = 0;
    if (upperString(column).find("INTEGER") != std::string::npos) {
        sql_type |= base::SQL_TYPE_INT;
    } else if (upperString(column).find("FLOAT") != std::string::npos) {
        sql_type |= base::SQL_TYPE_FLOAT;
    } else if (upperString(column).find("TEXT") != std::string::npos) {
        sql_type |= base::SQL_TYPE_TEXT;
    } else if (upperString(column).find("BLOB") != std::string::npos) {
        sql_type |= base::SQL_TYPE_BLOB;
    }
    
    if (!(upperString(column).find("DEFAULT") != std::string::npos) &&
        !(upperString(column).find("PRIMARY KEY") != std::string::npos)) {
        sql_type |= base::SQL_TYPE_NULL;
    }
    
    return sql_type;
}

SqliteFile::SqlTypes SqliteFile::sqlTypesFor(const std::string& tableName) {
    std::string createTableSQL = getCreateTableSQLFor(filepath_, tableName);
    createTableSQL.erase(createTableSQL.begin(),
                         createTableSQL.begin() + createTableSQL.find("("));
    std::vector<std::string> columns;
    // split by ","
    boost::split(columns, createTableSQL, boost::is_any_of(","));
    
    SqlTypes sqltypes;
    std::vector<std::string>::iterator pos;
    for (pos = columns.begin(); pos != columns.end(); ++pos) {
        base::sql_type sql_type = getSqlTypeFor(*pos);
        sqltypes.push_back(sql_type);
    }
    return sqltypes;
}

SqliteFile::ColumnNames SqliteFile::columnNamesFor(std::string& tableName)
{
    // TODO
}

