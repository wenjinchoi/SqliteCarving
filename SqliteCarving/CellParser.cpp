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

// FIXIT: only for test.
// Need to auto parse the tmpl.
vector<base::sql_type> testTmpl () {
    RecordTmpl tmpl;
    tmpl.push_back(base::SQL_TYPE_INT | base::SQL_TYPE_NULL);
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
        int intValue =
            getValueFromMem<int>((char *)&bytes[0],
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

// 根据 sqlite db 的 schema, 获取指定表的的 schema(Template)
// Record 的 Types 是 Varint，
// 解析变长整数后与预设的 Map 进行匹配，得到相应的 SQL_TYPE
// 当连续读取数据的 SQL_TYPE 都满足 Template，则判断找到一条记录
// 再根据 Template 读取后面的 Data 区内容
vector<string> parseRecordsFromFreeBlock(base::bytes_it begin,
                                          base::bytes_it end) {
    // type 1, type 2, type 3, ..., data 1, data 2, data 3, ...
    // tmpl 1, tmpl 2, tmpl 3, ...|
    vector<string> records;
    vector<base::sql_type> tmpl = testTmpl();
    base::bytes_it pos;
    for (pos = begin; pos < end;) {
        vector<RecordFormat> rf = matchByesForTmpl(pos, end, tmpl);
        if (!rf.empty()) {
            pos += calcHeaderSize(rf);
            for (vector<RecordFormat>::iterator rf_pos = rf.begin();
                 rf_pos != rf.end(); ++rf_pos) {
                //FIXIT: bytes初始化有问题？
                base::bytes_t bytes = base::bytes_t(pos, pos + rf_pos->contentSize);
                // 根据 Serial Type 将 Data 区字节转换为 string
                string content =
                    getStringFor(rf_pos->serialType, bytes);
                records.push_back(content);
                pos += rf_pos->contentSize;
            }

        } else {
            ++pos;
        }
        // To be continue...
        // 与当前的 RecordTmpl 比较
    }
    // 如果完全满足当前的 RecordTmpl，则取出 Data 区的数据
    // Data 区尾不能跨越到已存在数据块
    // 如果仍未到达 end，则继续解析
    return records;
}

}


// ------ old -------
/*
using namespace std;

uint sqlPageSize = 1024;

namespace  {
typedef pair<int, int> SqlTypeInt;
typedef map<const int, SqlTypeInt> StypeSqlTypeMap;
StypeSqlTypeMap stypeSqlTypeMap;
} // namespace

void build_map()
{
    stypeSqlTypeMap.insert(make_pair(STYPE_NULL, make_pair(SQL_TYPE_NULL, 0)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT8, make_pair(SQL_TYPE_INT, 1)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT16, make_pair(SQL_TYPE_INT, 2)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT24, make_pair(SQL_TYPE_INT, 3)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT32, make_pair(SQL_TYPE_INT, 4)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT48, make_pair(SQL_TYPE_INT, 6)));
    stypeSqlTypeMap.insert(make_pair(STYPE_INT64, make_pair(SQL_TYPE_INT, 8)));
    stypeSqlTypeMap.insert(make_pair(STYPE_FLOAT, make_pair(SQL_TYPE_FLOAT, 8)));
    stypeSqlTypeMap.insert(make_pair(STYPE_CONST0, make_pair(SQL_TYPE_INT, 0)));
    stypeSqlTypeMap.insert(make_pair(STYPE_CONST1, make_pair(SQL_TYPE_INT, 0)));
}

struct ColumnType {
    int sType;    // Type value  sType col 1 (varint) ... col N (varint)
    int colType;  // Column Type
    int size;     // Data Length
};

struct ColSizeValue
{
    int size;
    string value;
};

ColumnType make_columnType(int sType) {
    ColumnType columnType;
    columnType.sType = sType;
    StypeSqlTypeMap::iterator pos;
    pos = stypeSqlTypeMap.find(sType);
    if (pos != stypeSqlTypeMap.end()) {
        columnType.colType = pos->second.first;
        columnType.size = pos->second.second;
    }
    else if (sType > 11 && !isOdd(sType))
    {
        columnType.colType = SQL_TYPE_BLOB;
        columnType.size = (sType - 12)/2;
    }
    else if (sType > 12 && isOdd(sType))
    {
        columnType.colType = SQL_TYPE_TEXT;
        columnType.size = (sType - 13)/2;
    }
    else
    {
        // FIXIT: raise error
        cout << "Error sqlite serial type" << endl;
    }
    return columnType;
}

uint64_t vec_uchar2uint64_t(vec_uchar::iterator pos, size_t length)
{
    uint64_t value = static_cast<uint64_t>(*pos);
    size_t i = 1;
    while (i < length) {
        value <<= 8;
        value |= static_cast<uint64_t>(*(pos + i));
        ++i;
    }
    return value;
}

float vec_uchar2float(vec_uchar::iterator pos, size_t length)
{
    uint64_t value = vec_uchar2uint64_t(pos, length);
    // FIXIT: may be not good.
    return *((float *)&value);
}

ColSizeValue parseColumeValue (ColumnType columnType,
                               vec_uchar playloadContent,
                               int offset)
{
    ColSizeValue colSizeVal;
    colSizeVal.size = -1;
    colSizeVal.value = "";
    vec_uchar::iterator pos = playloadContent.begin() + offset;
    switch (columnType.sType) {
        case STYPE_NULL:
            colSizeVal.size = 0;
            colSizeVal.value = "Null";
            break;
        case STYPE_INT8:
            colSizeVal.size = 1;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT16:
            colSizeVal.size = 2;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT24:
            colSizeVal.size = 3;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT32:
            colSizeVal.size = 4;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
             break;
        case STYPE_INT48:
            colSizeVal.size = 6;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_INT64:
            colSizeVal.size = 8;
            colSizeVal.value = to_string(vec_uchar2uint64_t(pos, colSizeVal.size));
            break;
        case STYPE_FLOAT:
            colSizeVal.size = 8;
            colSizeVal.value = to_string(vec_uchar2float(pos, colSizeVal.size));
            break;
        case STYPE_CONST0:
            colSizeVal.size = 0;
            colSizeVal.value = to_string(0);
            break;
        case STYPE_CONST1:
            colSizeVal.size = 0;
            colSizeVal.value = to_string(1);
            break;
        default:
            if (columnType.colType == SQL_TYPE_BLOB)
            {
                colSizeVal.size = columnType.size;
                string s(pos, pos + columnType.size);
                colSizeVal.value = s;
            }
            else if (columnType.colType == SQL_TYPE_TEXT)
            {
                colSizeVal.size = columnType.size;
                string s(pos, pos + columnType.size);
                colSizeVal.value = s;
            }
            else
            {
                cout << "Unknown column type: " << columnType.sType << endl;
            }
            break;
    }
    return colSizeVal;
}

void parseTableLeafCellPlayload(vec_uchar playloadContent)
{
    int playloadOffset = 0;
    vector<ColumnType> cols;
    
    // Type's header size
    varint headerSize = parseVarint(playloadContent, playloadOffset);
    cout << "Header size: " << headerSize.second << endl;
    playloadOffset += headerSize.first;
    
    // Get type's value
    varint sType1 = parseVarint(playloadContent, playloadOffset);
    cols.push_back(make_columnType(sType1.second));
    cout << "sType 1: " << sType1.second << endl;
    playloadOffset += sType1.first;
    
    while (playloadOffset < headerSize.second) {
        varint sType = parseVarint(playloadContent, playloadOffset);
        cols.push_back(make_columnType(sType.second));
        cout << "sType: " << sType.second << endl;
        playloadOffset += sType.first;
    }
    
    // TODO: process overflow
    // bool overflowReached = false;
    int offset = playloadOffset;
    vector<ColumnType>::iterator pos;
    for (pos = cols.begin(); pos != cols.end(); ++pos) {
        ColSizeValue colSizeValue =  parseColumeValue(*pos, playloadContent, offset);
        offset += colSizeValue.size;
        cout << "col Value: " << colSizeValue.value << endl;
    }
}

uint overflowLocalSize(uint playloadSize, uint usableSize)
{
    uint minEmbeddedFraction = 32;
    uint minLocal = ((usableSize - 12) * minEmbeddedFraction / 255) - 23;
    uint maxLocal = usableSize - 35;
    uint localSize = minLocal + ((playloadSize - minLocal) % (usableSize - 4));
    if (localSize < maxLocal)
    {
        return localSize;
    }
    else
    {
        return minLocal;
    }
}

void parseTableLeafCell(vec_uchar cellContent)
{
    int cellOffset = 0;
    varint playloadSize = parseVarint(cellContent, cellOffset);
    cout << "num bytes of playload: " << playloadSize.second << endl;
    cellOffset += playloadSize.first;
    
    varint intKey = parseVarint(cellContent, cellOffset);
    cout << "intKey: " << intKey.second << endl;
    cellOffset += intKey.first;
    
    int sqlPageSize = 1024;
    bool overflow = playloadSize.second > (sqlPageSize - 35);
    if (overflow)
    {
        uint localBytes = overflowLocalSize(playloadSize.second, sqlPageSize);
        cout << "Overflow cell payload encountered..." << localBytes
             << "bytes are stored in this page." << endl;
        vec_uchar playloadContent(cellContent.begin() + cellOffset,
                                  cellContent.begin() + localBytes);
        parseTableLeafCellPlayload(playloadContent);
    }
    else
    {
        vec_uchar playloadContent(cellContent.begin() + cellOffset,
                                  cellContent.begin() + playloadSize.second);
        parseTableLeafCellPlayload(playloadContent);
    }
    cout << "Parsing cell done." << endl;
    // return;
}

 */
