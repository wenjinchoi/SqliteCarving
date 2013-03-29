//
//  utils.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/30/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__utils__
#define __SqliteCarving__utils__

#include <iostream>
#include <vector>

std::pair<int, unsigned long> parseVarint(std::vector<unsigned char> varintBytes,
                                          int offset=0);

bool isOdd(int num);

#endif /* defined(__SqliteCarving__utils__) */
