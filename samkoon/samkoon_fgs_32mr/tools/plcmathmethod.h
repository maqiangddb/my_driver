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
    static vector<ADDR_TARGET_PROP > addrSortOutForCV(vector<ADDR_TARGET_PROP > sourceList, int nMaxLen, int nNarrow = 1, bool bByteReg = false);
};

#endif // CHECKSUMALG_H
