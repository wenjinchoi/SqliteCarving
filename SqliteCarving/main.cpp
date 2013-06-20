//
//  main.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include <bitset>
#include <iostream>
#include <algorithm>
#include <boost/scoped_ptr.hpp>

#include "basedef.h"
#include "utils.h"

// New design
#include "SqliteFile.h"
#include "SqlitePage.h"
#include "FreeBlock.h"
#include "SqliteParser.h"

using namespace sp;
using namespace std;

void testSqliteFile() {
    string filepath = "/Users/wenjinchoi/Desktop/samsung_GT-9250-4.2.2-mmssms.db";
    SqliteFile* sf = new SqliteFile(filepath);
    if (sf->isSqliteFile()) {
        cout << "It's Sqlite File.";
    } else {
        cout << "It's not Sqlite File.";
    }
    cout << endl << endl;
    
    cout << "SQL Types:" << endl;
    SqliteFile::SqlTypes sqltypes =  sf->sqlTypesFor("sms");
    copy(sqltypes.begin(), sqltypes.end(),
         ostream_iterator<int>(cout, " "));
    cout << endl << endl;
    
    cout << "Column Name:" << endl;
    SqliteFile::ColumnNames columns =  sf->columnNamesFor("sms");
    copy(columns.begin(), columns.end(),
         ostream_iterator<string>(cout, " "));
    cout << endl;
    
    delete sf;
    
}


void test_util() {
    char mem[2] = { 0x2B, 0x12 };
    int v = getValueFromMem<int>(mem, 2);
    std::cout << "value: " << v << std::endl;
}

void test_util_float() {
    // TODO: 需完成从内存块取 Float 的功能
    // float f = 3.14;
    uint8_t mem[4] = { 0xC3, 0xF5, 0x48, 0x40 };
    unsigned int i = getValueFromMem<unsigned int>((char *)mem, 4);
    reverse32InByte(&i);
    float *f = (float *)&i;
    cout << *f << endl;
}


sp::CellDatas parseFreeBlocks(
    SqliteFile::Bytes_ptr page_ptr,
    SqlitePage::BlockAreas& freeBlocks,
    SqliteFile::SqlTypes sqlTypes)
{
    sp::CellDatas result;
    for (SqlitePage::BlockAreas::iterator pos = freeBlocks.begin();
         pos != freeBlocks.end(); ++pos)
    {
        sp::FreeBlock *freeblock =
            new sp::FreeBlock(page_ptr->begin() + pos->begin,
                                         page_ptr->begin() + pos->end);
        freeblock->setSqlTypeTmpl(sqlTypes);
        sp::CellDatas datas = freeblock->parseCellDatas();
        result.insert(result.end(), datas.begin(), datas.end());
        delete freeblock;
    }
    return result;
}

// for test
void displayOutput(vector<sp::CellData>& output)
{
    for (vector<sp::CellData>::iterator pos = output.begin();
         pos != output.end(); ++pos) {
        copy(pos->begin(), pos->end(),
             ostream_iterator<string>(cout, " "));
        cout << endl << endl;
    }
}

void testMain2()
{
    string filepath = "/Users/wenjinchoi/Desktop/samsung_GT-9250-4.2.2-mmssms.db";
    boost::scoped_ptr<SqliteFile> sqliteFile(new SqliteFile(filepath));
    
    SqliteFile::SqlTypes tmpl = sqliteFile->sqlTypesFor("sms");
    tmpl.erase(tmpl.begin()); // 第一列的数据会被覆盖，移除之
    
    sp::CellDatas result;
    for (uint64_t pageNum = 2; pageNum < sqliteFile->numOfPages();
         ++pageNum)
    {
        SqliteFile::Bytes_ptr page_ptr(new base::bytes_t);
        sqliteFile->readPageTo(pageNum, page_ptr);
        
        SqlitePage sqlitePage(*page_ptr);
        SqlitePage::BlockAreas freeBlocks = sqlitePage.freeBlocks();
        
        sp::CellDatas cellDatas = parseFreeBlocks(page_ptr, freeBlocks, tmpl);
        
        result.insert(result.end(), cellDatas.begin(), cellDatas.end());
    }
    
    // Output
    displayOutput(result);
}

void testMain3()
{
    string filepath = "/Users/wenjinchoi/Desktop/samsung_GT-9250-4.2.2-mmssms.db";
    
    SqliteParser sqliteparser;
    sqliteparser.setSqliteFile(filepath);
    sqliteparser.setTableName("sms");
    SqliteParser::ParseReturnType ret = sqliteparser.parseDeletedDatas();
    sp::CellDatas cellDatas = sqliteparser.getDatas();
    
    cout << "ret: " << ret << endl;
    displayOutput(cellDatas);
}

void test_cmd() {
    string result = getSchemaFor("/Users/wenjinchoi/Desktop/samsung_GT-9100-4.0.4_mmssms.db", "sms");
    cout << "Schema for sms:" << endl;
    cout << result;
}

int main(int argc, char * argv[])
{
//    test_util();
//    testSqliteFileParser();
//    test_util_float();
//    testCellParser2();
//    test_shema();
//    test_cmd();
    
//    string filename = string("/Users/wenjinchoi/Desktop/samsung_GT-9100-4.0.4_mmssms.db");
//    string tableName = string("sms");
    
//    if (argc < 3) {
//        cout << "Usage:" << endl;
//        cout << "  SqliteCarving [database path] [table name]" << endl;
//        exit(1);
//    }
//    
//    string filename = string(argv[1]);
//    string tableName = string(argv[2]);    
//    testMain(filename, tableName);
//    return 0;
    
//    testSqliteFile();
//    testMain2();
    testMain3();
    
}

