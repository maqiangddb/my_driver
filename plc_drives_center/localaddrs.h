#ifndef LOCALADDRS_H
#define LOCALADDRS_H

#include "tools/structHead.h"

class LocalAddrs
{
public:
    LocalAddrs();

    /*设置写数据的地址,除字符串*/
    void setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<uchar > dataList);

    /*设置写字符串的值*/
    void setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string > strList);

    /*获得读数据的值，除字符串*/
    bool getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP > &valueList);

    /*获得读字符串的值*/
    bool getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string > &strList);

public:
    /*地址的值集合*/
    STATION_VALUE_PROP m_localAddrValueList;
};

#endif // LOCALADDRS_H
