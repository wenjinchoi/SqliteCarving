//
//  FreeBlock_unittest.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/17/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//
#define GTEST_USE_OWN_TR1_TUPLE 1
#include <iostream>
#include <gtest/gtest.h>

#include "FreeBlock.h"

TEST(FreeBlockTest, SizeBase)
{
    const unsigned char testdata[] = {
        0x3F, 0x00, 0x12, 0x00, 0x01, 0x17, 0x00, 0x04,
        0x00, 0x01, 0x01, 0x01, 0x00, 0x00, 0x1D, 0x00,
        0x01, 0x01, 0x01, 0x01, 0x01, 0x31, 0x30, 0x30,
        0x38, 0x36, 0x50, 0xE2, 0x43, 0x20, 0x00, 0xFF,
        0x01, 0x52, 0x65, 0x63, 0x6F, 0x72, 0x64, 0x20,
        0x31, 0x00, 0x00, 0x00, 0x00
    };
    // "1 10086 [NULL] 1357005600 [NULL] 0 255 1 [NULL] [NULL] Record 1 [NULL] 0 0 [NULL] [Missed]"
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[43]);
    sp::FreeBlock *fb = new sp::FreeBlock(testBlock);
    EXPECT_EQ(43, fb->size());
    delete fb;
}

TEST(FreeBlockTest, SizeZero)
{
    base::bytes_t testBlock;
    sp::FreeBlock *fb = new sp::FreeBlock(testBlock);
    EXPECT_EQ(0, fb->size());
    delete fb;
}

TEST(FreeBlockTest, SizeOne)
{
    base::bytes_t testBlock;
    testBlock.push_back(01);
    sp::FreeBlock *fb = new sp::FreeBlock(testBlock);
    EXPECT_EQ(1, fb->size());
    delete fb;
}