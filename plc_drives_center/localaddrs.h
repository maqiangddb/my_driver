#ifndef LOCALADDRS_H
#define LOCALADDRS_H

#include "tools/structHead.h"

class LocalAddrs
{
public:
    LocalAddrs();

    /*����д���ݵĵ�ַ,���ַ���*/
    void setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<uchar > dataList);

    /*����д�ַ�����ֵ*/
    void setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string > strList);

    /*��ö����ݵ�ֵ�����ַ���*/
    bool getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP > &valueList);

    /*��ö��ַ�����ֵ*/
    bool getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string > &strList);

public:
    /*��ַ��ֵ����*/
    STATION_VALUE_PROP m_localAddrValueList;
};

#endif // LOCALADDRS_H
