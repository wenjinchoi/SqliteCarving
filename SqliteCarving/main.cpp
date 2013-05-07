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

#include "basedef.h"
#include "SqliteFileParser.h"
#include "SqlitePageParser.h"
#include "CellParser.h"
#include "Schema.h"

//#include "utils.h"

using namespace std;

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
    
    vector<string> records;
    records = sqliteparser::parseRecordsFromFreeBlock(testBlock.begin(),
                                                      testBlock.end());
    
    if (records.empty()) {
        cout << "Not matched." << endl;
    } else {
        cout << "Data:" << endl;
        copy(records.begin(), records.end(),
             ostream_iterator<string>(cout, " "));
        cout << endl;
    }
    
    
}

/*
void testCellParser()
{
    const unsigned char testdata[] = {
        0x2B, 0x01, 0x12, 0x00, 0x01, 0x17, 0x00, 0x04,
        0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x30, 0x30,
        0x38, 0x36, 0x50, 0xE2, 0x43, 0x20, 0x00, 0xFF,
        0x01, 0x52, 0x65, 0x63, 0x6F, 0x72, 0x64, 0x20,
        0x31, 0x00, 0x00, 0x00, 0x00
    };
    
    build_map();
    
    int length = sizeof(testdata)/sizeof(unsigned char);
    vec_uchar vtd;
    for (int i = 0; i < length; ++i) {
        vtd.push_back(testdata[i]);
    }
    
    cout << "Test Data: " << endl;
    copy(vtd.begin(), vtd.end(), ostream_iterator<int>(cout, " "));
    cout << endl << endl;
    
    parseTableLeafCell(vtd);
}
*/

void reverse32InByte( void *val )
{
    unsigned int v = *((unsigned int *)val) ;
    v = ((v & 0x000000FF) << 24) | ((v & 0x0000FF00) << 8) |
    ((v & 0x00FF0000) >> 8) | ((v & 0xFF000000) >> 24) ;
    *((unsigned int *)val) = v ;
};

void test_util() {
    char mem[2] = { 0x2B, 0x12 };
    int v = getValueFromMem<int>(mem, 2);
    std::cout << "value: " << v << std::endl;
}

void test_util_float() {
    // float f = 3.14;
    uint8_t mem[4] = { 0xC3, 0xF5, 0x48, 0x40 };
    unsigned int i = getValueFromMem<unsigned int>((char *)mem, 4);
    reverse32InByte(&i);
    float *f = (float *)&i;
    cout << *f << endl;
}

void testSqliteFileParser()
{
    string filename = string("/Users/wenjinchoi/Desktop/mmssms_del5.db");
    
    unsigned int pageSize1 = sqliteparser::pageSize(filename);
    unsigned long sizeOfPages1 = sqliteparser::sizeOfPages(filename);
    bool isAutoVacuum1 = sqliteparser::isAutoVacuum(filename);
    cout << "Page Size: " << pageSize1
         << " Size of Pages: " << sizeOfPages1
         << " Is Auto Vacuum:" << isAutoVacuum1 << endl;
    
    cout << "Page at 2: " << endl;
    base::bytes_t thePage = sqliteparser::pageAt(filename, 2);
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

/*
void testSqlitePageParser()
{
    SqliteFileParser fparser = SqliteFileParser();
    fparser.setInputFile("/Users/wenjinchoi/Desktop/mmssms_2infreeblock.db");
    base::bytes_t thePage = fparser.pageAt(2);
    
    using sqlparser::SqlitePageParser;
    SqlitePageParser pparser = SqlitePageParser(thePage);
    pparser.parsePage();
    
    std::vector<uint16_t> cellList = pparser.cellList();
    std::cout << "Cell list: ";
    copy(cellList.begin(), cellList.end(),
         ostream_iterator<uint16_t>(std::cout, " "));
    std::cout << endl;
    
    std::cout << "First free block offset: "
              << pparser.firstFreeBlockOffset() << std::endl;
    
    std::vector<std::pair<uint16_t, uint16_t> > freeBlocArea;
    freeBlocArea = pparser.freeBlockAreaList();
    std::cout << "Free Block Aree List: ";
    
    std::vector<std::pair<uint16_t, uint16_t> >::iterator pos;
    for (pos = freeBlocArea.begin(); pos != freeBlocArea.end(); ++pos) {
        std::cout << "[" << pos->first << ", " << pos->second << "], ";
    }
    std::cout << std::endl;
}
*/


int main(int argc, const char * argv[])
{
    // testCellParser();
//    testSqlitePageParser();
//    test_util();
//    testSqliteFileParser();
//    test_util_float();
//    testCellParser2();
    test_shema();
    return 0;
}

