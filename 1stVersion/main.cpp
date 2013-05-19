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
#include "SqliteFileParser.h"
#include "SqlitePageParser.h"
#include "CellParser.h"
#include "Schema.h"
#include "utils.h"

// New design
#include "SqliteFile.h"
#include "SqlitePage.h"
#include "FreeBlock.h"
#include "SqliteParser.h"

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

void testCellParser2() {
    const unsigned char testdata[] = {
        0x3F, 0x00, 0x12, 0x00, 0x01, 0x17, 0x00, 0x04,
        0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x30, 0x30,
        0x38, 0x36, 0x50, 0xE2, 0x43, 0x20, 0x00, 0xFF,
        0x01, 0x52, 0x65, 0x63, 0x6F, 0x72, 0x64, 0x20,
        0x31, 0x00, 0x00, 0x00, 0x00
    };
    
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[43]);
    
    cout << "Bytes of cell:" << endl;
    copy(testBlock.begin(), testBlock.end(),
         ostream_iterator<int>(cout, " "));
    cout << endl;
    
    vector<sqliteparser::Record> records;
    vector<base::sql_type> tmpl = sqliteparser::testTmpl();
    records = sqliteparser::parseRecordsFromFreeBlock(testBlock.begin(),
                                                      testBlock.end(),
                                                      tmpl);
    
    if (records.empty()) {
        cout << "Not matched." << endl;
    } else {
        for (vector<sqliteparser::Record>::iterator pos = records.begin(); pos != records.end(); ++pos) {
            cout << "Data:" << endl;
            copy(pos->begin(), pos->end(),
                 ostream_iterator<string>(cout, " "));
            cout << endl;
        }
    }
    
    
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

void testSqliteFileParser()
{
    string filename = string("/Users/wenjinchoi/Desktop/samsung_GT-9250-4.2.2-mmssms.db");
    
    unsigned int pageSize1 = sqliteparser::pageSize(filename);
    unsigned long sizeOfPages1 = sqliteparser::sizeOfPages(filename);
    bool isAutoVacuum1 = sqliteparser::isAutoVacuum(filename);
    cout << "Page Size: " << pageSize1
         << " Size of Pages: " << sizeOfPages1
         << " Is Auto Vacuum:" << isAutoVacuum1 << endl;
    
    cout << "Page at 10: " << endl;
    base::bytes_t thePage = sqliteparser::pageAt(filename, 10);
    copy(thePage.begin(), thePage.end(),
         ostream_iterator<char>(cout, ""));
    cout << endl << endl;
    
    vector<base::blockArea> freeblocks =
        sqliteparser::getFreeBlockAreaList(thePage);
    
    cout << "Free Block Area: " << endl;
    vector<base::blockArea>::iterator pos;
    int i = 1;
    for (pos = freeblocks.begin(); pos != freeblocks.end(); ++pos) {
        cout << "No." << i << " begin: " << pos->begin
             << " end: " << pos->end << endl;
        ++i;
    }
    cout << endl;
    
}

void test_shema() {
    string str_shema = string("CREATE TABLE sms (_id INTEGER PRIMARY KEY AUTOINCREMENT, thread_id INTEGER, address TEXT, person INTEGER, date INTEGER, protocol INTEGER, read INTEGER DEFAULT 0, status INTEGER DEFAULT -1, type INTEGER, reply_path_present INTEGER, subject TEXT, body TEXT, service_center TEXT, locked INTEGER DEFAULT 0, error_code INTEGER DEFAULT 0, seen INTEGER DEFAULT 0, deletable INTEGER DEFAULT 0);");
    
    SchemaParser schema = SchemaParser(str_shema);
    std::vector<base::sql_type> result = schema.parse();
    cout << "SQL TYPE:" << endl;
    copy(result.begin(), result.end(),
         ostream_iterator<base::sql_type>(cout, " "));
}


void testMain(const string& dbFilePath, const string& tableName) {
    string sms_schema = getSchemaFor(dbFilePath, tableName);
    SchemaParser schema = SchemaParser(sms_schema);
    vector<base::sql_type> sms_tmpl = schema.parse();
    sms_tmpl.erase(sms_tmpl.begin());
    
    cout << "SQL TYPE:" << endl;
    copy(sms_tmpl.begin(), sms_tmpl.end(),
         ostream_iterator<base::sql_type>(cout, " "));
    cout << endl << endl;
    
    unsigned long sizeOfPages = sqliteparser::sizeOfPages(dbFilePath);
    
    int dataCount = 1;
    for (int i = 1; i <= sizeOfPages; ++i) {
        base::bytes_t page = sqliteparser::pageAt(dbFilePath, i);
        if (sqliteparser::isTableLeaf(page)) {
            vector<base::blockArea> freeblocks =
                sqliteparser::getFreeBlockAreaList(page);
            for (vector<base::blockArea>::iterator pos = freeblocks.begin();
                 pos != freeblocks.end(); ++pos) {
                // FOR TEST
//                vector<sqliteparser::Record> result =
//                    sqliteparser::parseRecordsFromFreeBlock(page.begin() + pos->begin, page.begin() + pos->end, sms_tmpl);
                vector<sqliteparser::Record> result =
                sqliteparser::parseRecordsFromFreeBlock(page.begin(), page.end(), sms_tmpl);
                if (result.empty()) {
                    cout << "[Unmatched] No matched item in free block at page " << i
                         << " Offset: " << pos->begin << "~" << pos->end
                         << endl;
                } else {
                    for (vector<sqliteparser::Record>::iterator r_pos = result.begin(); r_pos != result.end(); ++r_pos) {
                        cout << "Data " << dataCount++ << ": ";
                        copy(r_pos->begin(), r_pos->end(),
                             ostream_iterator<string>(cout, " "));
                        cout << endl;
                    }
                }
            }
        }
        
    }
}

sqliteparser2::CellDatas parseFreeBlocks(
    SqliteFile::Bytes_ptr page_ptr,
    SqlitePage::BlockAreas& freeBlocks,
    SqliteFile::SqlTypes sqlTypes)
{
    sqliteparser2::CellDatas result;
    for (SqlitePage::BlockAreas::iterator pos = freeBlocks.begin();
         pos != freeBlocks.end(); ++pos)
    {
        sqliteparser2::FreeBlock *freeblock =
            new sqliteparser2::FreeBlock(page_ptr->begin() + pos->begin,
                                         page_ptr->begin() + pos->end);
        freeblock->setSqlTypeTmpl(sqlTypes);
        sqliteparser2::CellDatas datas = freeblock->parseCellDatas();
        result.insert(result.end(), datas.begin(), datas.end());
        delete freeblock;
    }
    return result;
}

// for test
void displayOutput(vector<sqliteparser2::CellData>& output)
{
    for (vector<sqliteparser2::CellData>::iterator pos = output.begin();
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
    
    sqliteparser2::CellDatas result;
    for (uint64_t pageNum = 2; pageNum < sqliteFile->numOfPages();
         ++pageNum)
    {
        SqliteFile::Bytes_ptr page_ptr(new base::bytes_t);
        sqliteFile->readPageTo(pageNum, page_ptr);
        
        SqlitePage sqlitePage(*page_ptr);
        SqlitePage::BlockAreas freeBlocks = sqlitePage.freeBlocks();
        
        sqliteparser2::CellDatas cellDatas = parseFreeBlocks(page_ptr, freeBlocks, tmpl);
        
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
    sqliteparser2::CellDatas cellDatas = sqliteparser.getDatas();
    
    cout << "ret: " << ret << endl;
    displayOutput(cellDatas);
}

void testMainMini(const string& dbFilePath, const string& tableName) {
    string sms_schema = getSchemaFor(dbFilePath, tableName);
    SchemaParser schema = SchemaParser(sms_schema);
    vector<base::sql_type> sms_tmpl = schema.parse();
    sms_tmpl.erase(sms_tmpl.begin());
    
    unsigned long sizeOfPages = sqliteparser::sizeOfPages(dbFilePath);
    
    int dataCount = 1;
    for (int i = 1; i <= sizeOfPages; ++i) {
        base::bytes_t page = sqliteparser::pageAt(dbFilePath, i);
        if (sqliteparser::isTableLeaf(page)) {
            vector<base::blockArea> freeblocks =
            sqliteparser::getFreeBlockAreaList(page);
            for (vector<base::blockArea>::iterator pos = freeblocks.begin();
                 pos != freeblocks.end(); ++pos) {
                vector<sqliteparser::Record> result =
                    sqliteparser::parseRecordsFromFreeBlock(page.begin(), page.end(), sms_tmpl);
                if (result.empty()) {
                    cout << "[Unmatched] No matched item in free block at page " << i
                    << " Offset: " << pos->begin << "~" << pos->end
                    << endl;
                } else {
                    for (vector<sqliteparser::Record>::iterator r_pos = result.begin(); r_pos != result.end(); ++r_pos) {
                        cout << "Data " << dataCount++ << ": ";
                        copy(r_pos->begin(), r_pos->end(),
                             ostream_iterator<string>(cout, " "));
                        cout << endl;
                    }
                }
            }
        }
        
    }
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

