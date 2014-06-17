#ifndef CHECKSUMALG_H
#define CHECKSUMALG_H

#include "structHead.h"

class PlcMathMethod
{
public:
    PlcMathMethod();

    static ushort getSumCheck(vector<uchar > pCharData, int nStartPos = 0);
    static ushort getCRCcheck(vector<uchar > pCharData, int nStartPos = 0);
    static ushort getLRCcheck(vector<uchar > pCharData, int nStartPos = 0);
    static ushort getXORcheck(vector<uchar > pCharData, int nStartPos = 0);
    static char intToAsciiValue(char nIntValue);
    static char asciiToIntValue(char nAsciiValue);
    static vector<ADDR_TARGET_PROP > addrBubbleSort(vector<ADDR_TARGET_PROP > sourceList);
    static vector<ADDR_TARGET_PROP > addrSortOut(vector<ADDR_TARGET_PROP > sourceList, int nMaxLen, int nNarrow = 1, bool bByteReg = false);
    static bool compareAddr(ADDR_TARGET_PROP oneAddr, ADDR_TARGET_PROP otherAddr);
    static int getbContainAddr(ADDR_RW_STRUCT addrProp, vector<ADDR_RW_STRUCT > list);
    static bool getbContainWAddr(ADDR_RW_STRUCT addrProp, deque<ADDR_RW_STRUCT > &list, bool bCoverDataList = false);
};

#endif // CHECKSUMALG_H
