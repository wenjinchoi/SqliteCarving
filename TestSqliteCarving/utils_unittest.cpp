//
//  utils_unittest.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/18/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "utils.h"

#define GTEST_USE_OWN_TR1_TUPLE 1
#include <gtest/gtest.h>

TEST(UtilsTest, ParseVarintBaseTest) {
    unsigned char testdata[] = { 0x85, 0x7B, 0x01 };
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[2]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(763, vint.value);
    EXPECT_EQ(2, vint.length);
}

TEST(UtilsTest, ParseVarintEqualZero) {
    unsigned char testdata[] = { 0x00, 0x01 };
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[1]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(0, vint.value);
    EXPECT_EQ(1, vint.length);
}

TEST(UtilsTest, ParseVarintEqualOne) {
    unsigned char testdata[] = { 0x01, 0x02 };
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[1]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(1, vint.value);
    EXPECT_EQ(1, vint.length);
}

TEST(UtilsTest, ParseVarintOneByte) {
    base::bytes_t testBlock;
    testBlock.push_back(0x02);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(2, vint.value);
    EXPECT_EQ(1, vint.length);
}

TEST(UtilsTest, ParseVarintMaxValueOfOneByte) {
    unsigned char testdata[] = { 0x7F, 0x01 };
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[1]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(127, vint.value);
    EXPECT_EQ(1, vint.length);
}

TEST(UtilsTest, ParseVarintEqualULONG_LONG_MAX) {
    unsigned char testdata[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0x01 };
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[9]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(ULONG_LONG_MAX, vint.value);
    EXPECT_EQ(9, vint.length);
}

TEST(UtilsTest, ParseVarintEqualLONG_LONG_MAX) {
    unsigned char testdata[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0x7F};
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[9]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(LONG_LONG_MAX, vint.value);
    EXPECT_EQ(9, vint.length);
}

TEST(UtilsTest, ParseVarintNotCompleted) {
    unsigned char testdata[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF};
    base::bytes_t testBlock = base::bytes_t(&testdata[0], &testdata[7]);
    base::varint_t vint = parseVarint(testBlock.begin(), testBlock.end());
    EXPECT_EQ(0, vint.value);
    EXPECT_EQ(0, vint.length);
}