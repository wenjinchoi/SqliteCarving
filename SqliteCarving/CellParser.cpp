//
//  CellParser.cpp
//  SqliteCarving
//
//  Created by Wenjin Choi on 3/27/13.
//  Copyright (c) 2013 wenjin choi. All rights reserved.
//

#include "CellParser.h"

#include <bitset>
#include <algorithm>
#include <functional>
#include <numeric>
#include <sstream>


namespace sqliteparser {
    
// 已实现自动分析 schema 的模版，以下只是测试样例，不再使用
vector<base::sql_type> testTmpl() {
    RecordTmpl tmpl;
    //tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_TEXT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT);
    tmpl.push_back(base::SQL_TYPE_INT);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_TEXT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_TEXT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_TEXT | base::SQL_TYPE_NULL);
    tmpl.push_back(base::SQL_TYPE_INT);
    tmpl.push_back(base::SQL_TYPE_INT);
    tmpl.push_back(base::SQL_TYPE_INT);
    tmpl.push_back(base::SQL_TYPE_INT);
    return tmpl;
}

// FIXIT: 以下 Map 要找个适合的地方放置
recordFormatMap getRecordFormatMap() {
    map<base::serial_type, base::content_size> rf_map;
    rf_map[base::STYPE_NULL]   = 0;
    rf_map[base::STYPE_INT8]   = 1;
    rf_map[base::STYPE_INT16]  = 2;
    rf_map[base::STYPE_INT24]  = 3;
    rf_map[base::STYPE_INT32]  = 4;
    rf_map[base::STYPE_INT48]  = 6;
    rf_map[base::STYPE_INT64]  = 8;
    rf_map[base::STYPE_FLOAT]  = 8;
    rf_map[base::STYPE_CONST0] = 0;
    rf_map[base::STYPE_CONST1] = 0;
    return rf_map;
}

sTypeSqlTypeMap serialTypeSqlTypeMap() {
    map<base::serial_type, base::sql_type> smap;
    smap[base::STYPE_NULL]   = base::SQL_TYPE_NULL;
    smap[base::STYPE_INT8]   = base::SQL_TYPE_INT;
    smap[base::STYPE_INT16]  = base::SQL_TYPE_INT;
    smap[base::STYPE_INT24]  = base::SQL_TYPE_INT;
    smap[base::STYPE_INT32]  = base::SQL_TYPE_INT;
    smap[base::STYPE_INT48]  = base::SQL_TYPE_INT;
    smap[base::STYPE_INT64]  = base::SQL_TYPE_INT;
    smap[base::STYPE_FLOAT]  = base::SQL_TYPE_FLOAT;
    smap[base::STYPE_CONST0] = base::SQL_TYPE_INT;
    smap[base::STYPE_CONST1] = base::SQL_TYPE_INT;
    smap[base::STYPE_BLOB]   = base::SQL_TYPE_BLOB;
    smap[base::STYPE_TEXT]   = base::SQL_TYPE_TEXT;
    return smap;
}

// return 0 with unknown SQL type.
// else, return SQL_TYPE
base::sql_type sqlTypeFor(long serialTypeValue) {
    sTypeSqlTypeMap smap = serialTypeSqlTypeMap();
    base::sql_type sqlType;
    if (0 <= serialTypeValue && serialTypeValue <= 9) {
        sTypeSqlTypeMap::iterator smap_pos;
        smap_pos = smap.find(static_cast<int>(serialTypeValue));
        sqlType = smap_pos->second;
    } else if (serialTypeValue >= 12 && !isOdd(serialTypeValue)) {
        sqlType = base::SQL_TYPE_BLOB;
    } else if (serialTypeValue >= 13 && isOdd(serialTypeValue)) {
        sqlType = base::SQL_TYPE_TEXT;
    } else {
        
        sqlType = 0;
    }
    return sqlType;
}
    
RecordFormat recordFormatFor(long sTypeValue) {
    recordFormatMap rf_map = getRecordFormatMap();
    RecordFormat rf;
    base::sql_type sqlType = sqlTypeFor(sTypeValue);
    switch (sqlType) {
        case base::SQL_TYPE_NULL:
        case base::SQL_TYPE_INT:
        case base::SQL_TYPE_FLOAT:
            rf.serialType = sTypeValue;
            rf.contentSize = rf_map[static_cast<int>(sTypeValue)];
            break;
        case base::SQL_TYPE_BLOB:
            rf.serialType = base::STYPE_BLOB;
            rf.contentSize = (sTypeValue - 12) / 2;
            break;
        case base::SQL_TYPE_TEXT:
            rf.serialType = base::STYPE_TEXT;
            rf.contentSize = (sTypeValue - 13) / 2;
            break;
        default:
            break;
    }
    return rf;
}

// 匹配一段 bytes 是否符合模版
// 如果不匹配，返回一个空的 vector<RecordFormat>
vector<RecordFormat> matchByesForTmpl(base::bytes_it begin,
                      base::bytes_it end,
                      vector<base::sql_type>& tmpl) {
    // return equal(begin, end, tmpl.begin(), std::bit_or<base::byte_t>());
    vector<RecordFormat> reccordFormats;
    size_t tmpl_size = tmpl.size();
    int matchedCount = 0;
    int tmpl_pos = 0;
    base::bytes_it bytes_pos = begin;
    while (bytes_pos < end) {
        base::varint_t vint = parseVarint(bytes_pos, end);
        bytes_pos += vint.length;
        
        // varint value >> SQL_TYPE >> Record Format
        // Step 1: 先判断 SQL TYPE
        // Fail point 1: unknown Serial type.
        base::sql_type sqlType = sqlTypeFor(vint.value);
        if (!sqlType) break;
            
        // Step 2: 判断是否匹配, 根据 SQL TYPE 计算 Data 长度
        if (tmpl[tmpl_pos++] & sqlType) {
            RecordFormat rf = recordFormatFor(vint.value);
            rf.serialTypeSize = vint.length;
            reccordFormats.push_back(rf);
            ++matchedCount;
            if (matchedCount == tmpl_size) return reccordFormats;
        } else {
            // Fail point 2: not matched.
            break;
        }
    }
    // Fail point 3: out of bytes.
    reccordFormats.clear();
    return reccordFormats;
}

// get RecordFormat's contentSize
/*
class SumContentSize : public
    std::binary_function<long, RecordFormat, long> {
    long operator()(long initValue, RecordFormat rf) const {
        return initValue + rf.contentSize;
    }
};
 */

long calcHeaderSize(vector<RecordFormat>& recordFormats) {
    long size = 0;
    for (vector<RecordFormat>::iterator pos = recordFormats.begin();
         pos != recordFormats.end(); ++pos) {
        size += pos->serialTypeSize;
    }
    return size;
}

string getStringFor(base::serial_type serialType, base::bytes_t& bytes) {
    sTypeSqlTypeMap smap = serialTypeSqlTypeMap();
    sTypeSqlTypeMap::iterator smap_pos;
    smap_pos = smap.find(serialType);
    base::sql_type sqlType = smap_pos->second;
    
    string result;
    std::ostringstream convert;
    if (sqlType == base::SQL_TYPE_NULL) {
        convert << 0;
        result = convert.str();
    } else if (sqlType == base::SQL_TYPE_INT) {
        uint64_t intValue =
            getValueFromMem<uint64_t>((char *)&bytes[0],
                                 static_cast<int>(bytes.size()));
        convert << intValue;
        result = convert.str();
    } else if (sqlType == base::SQL_TYPE_TEXT) {
        copy(bytes.begin(), bytes.end()-1,
             std::ostream_iterator<uint8_t>(convert,""));
        convert << bytes.back();
        result = convert.str();
    }
    // FIXIT: need to parse float and blob 
      else if (sqlType == base::SQL_TYPE_FLOAT) {
        result = string("Float Value(Not completed)");
    } /*else if (sqlType == base::SQL_TYPE_BLOB) {
     
    }
    */
    
    return result;
}

// FIXIT: 可能需要根据吓一条记录，再对上一条记录优化
// FIXIT: 函数太长了，需要再拆分下
// 根据 sqlite db 的 schema, 获取指定表的的 schema(Template)
// Record 的 Types 是 Varint，
// 解析变长整数后与预设的 Map 进行匹配，得到相应的 SQL_TYPE
// 当连续读取数据的 SQL_TYPE 都满足 Template，则判断找到一条记录
// 再根据 Template 读取后面的 Data 区内容
// 如果完全满足当前的 RecordTmpl，则取出 Data 区的数据
// Data 区尾不能跨越到已存在数据块
// 如果仍未到达 end，则继续解析
vector<Record> parseRecordsFromFreeBlock(base::bytes_it begin,
                                         base::bytes_it end,
                                         vector<base::sql_type> tmpl) {
    // type 1, type 2, type 3, ..., data 1, data 2, data 3, ...
    // tmpl 1, tmpl 2, tmpl 3, ...|
    vector<Record> records;
    // FIXIT: 需要分析db文件提起模版
    // tmpl = testTmpl();
    base::bytes_it pos;
    for (pos = begin; pos < end;) {
        vector<RecordFormat> rf = matchByesForTmpl(pos, end, tmpl);
        if (!rf.empty()) {
            // 如果匹配模版
            // pos += calcHeaderSize(rf);
            base::bytes_it tmp_pos = pos + calcHeaderSize(rf);
            Record record;
            for (vector<RecordFormat>::iterator rf_pos = rf.begin();
                 rf_pos != rf.end(); ++rf_pos) {
                // 避免取 Data 时超出 FreeBlock 边界
                string content;                
                if (tmp_pos > end) {
                    content = "[Missed]";
                } else {
                    base::bytes_it end_it = tmp_pos + rf_pos->contentSize;;
                    
                    if (end_it > end) {
                        end_it = end;
                    }
                    
                    base::bytes_t bytes = base::bytes_t(tmp_pos, end_it);
                    if (bytes.empty()) {
                        content = "[NULL]";
                    } else {
                        content =
                        getStringFor(rf_pos->serialType, bytes);
                    }
                }

                record.push_back(content);
                // 如果是 pos += rf_pos->contentSize，可能会跳过一些数据
                tmp_pos += rf_pos->contentSize;
            }
            records.push_back(record);
            pos += calcHeaderSize(rf);
        } else {
            ++pos;
        }
    }
    return records;
}

} // namespace sqliteparser


