//
//  Cell.h
//  SqliteCarving
//
//  Created by Wenjin Choi on 5/4/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#ifndef __SqliteCarving__Cell__
#define __SqliteCarving__Cell__

#include "basedef.h"

#include <iostream>
#include <string>
#include <vector>

class Cell {

public:
    Cell();
    Cell(base::bytes_t& cell) :
      cell_(cell),
      begin_(cell.end()),
      end_(cell.end()) {}
    
    typedef int sql_type;
    typedef int serial_type;
    typedef int content_size;
    
    typedef std::vector<sql_type> RecordTmpl;
    
    void setMatchingTmpl(RecordTmpl& recordTmpl)
    { recordTmpl_ = recordTmpl; }
    
    void parse();
    
    base::bytes_it beginAt() { return begin_; }
    base::bytes_it endAt() { return end_; }
    
    std::vector<serial_type> header();
    std::vector<std::string> data();
    
    
private:
    base::bytes_t cell_;
    RecordTmpl recordTmpl_;
    base::bytes_it begin_;
    base::bytes_it end_;
    std::vector<serial_type> header_;
    std::vector<std::string> data_;
    
    // base::bytes_it isMatch(RecordTmpl& recordTmpl);
    
};

#endif /* defined(__SqliteCarving__Cell__) */