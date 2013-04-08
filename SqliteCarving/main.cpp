//
//  main.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include <iostream>
#include <algorithm>

#include "SqliteFileParser.h"
#include "CellParser.h"

using namespace std;

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

void testSqliteFileParser()
{
    SqliteFileParser fparser = SqliteFileParser();
    fparser.setInputFile("/Users/wenjinchoi/Desktop/mmssms.db");
    
    int pageSize = fparser.pageSize();
    cout << "Page Size: " << pageSize << endl;
    
    std::vector<char> thePage = fparser.pageAt(2);
    copy(thePage.begin(), thePage.end(),
         ostream_iterator<char>(cout, ""));
}


int main(int argc, const char * argv[])
{
    // testCellParser();
    testSqliteFileParser();
    
    return 0;
}

