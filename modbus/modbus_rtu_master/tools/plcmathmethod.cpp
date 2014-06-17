#include "plcmathmethod.h"

PlcMathMethod::PlcMathMethod()
{
}

/*************************************************************************
 * Funciton: 和校验函数
 * Parameters: pCharData:需要校验的数据指针
               nLen:总校验长度
 * Return: 和校验的结果
*************************************************************************/
ushort PlcMathMethod::getSumCheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*下标小于0，则从新赋值*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*校验*/
    ushort nSumTotal = 0;
    if(pCharData.empty()) return nSumTotal;

    int nLen = pCharData.size();
    for(int i = nStartPos; i < nLen; i++)
    {
        nSumTotal += pCharData.at(i);
    }
    return nSumTotal;
}

/*************************************************************************
 * Funciton: CRC校验函数
 * Parameters: pCharData:需要校验的数据指针
               nLen:总校验长度
 * Return: CRC校验的结果
*************************************************************************/
ushort PlcMathMethod::getCRCcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*下标小于0，则从新赋值*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*校验*/
    ushort CRC = 0xffff;
    int nLen = pCharData.size();
    for (int i = nStartPos; i < nLen; i++)
    {
        CRC ^= pCharData.at(i);
        for (int j = 0; j < 8; j++)
        {
            if (CRC & 1)
            {
                CRC >>= 1;
                CRC ^= 0xA001;
            }
            else
            {
                CRC >>= 1;
            }
        }
    }
    return (CRC);
}

/*************************************************************************
 * Funciton: LRC校验函数
 * Parameters: pCharData:需要校验的数据指针
               nLen:总校验长度
 * Return: LRC校验的结果
*************************************************************************/
ushort PlcMathMethod::getLRCcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*下标小于0，则从新赋值*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*校验*/
    ushort LRC = 0;
    int nLen = pCharData.size();
    for(int i = nStartPos; i < nLen; i++)
    {
        LRC += pCharData.at(i);
    }

    LRC = ~(LRC);
    LRC = (LRC & 0xff) + 1;

    return (LRC);
}

/*************************************************************************
 * Funciton: XOR校验函数
 * Parameters: pCharData:需要校验的数据指针
               nLen:总校验长度
 * Return: XOR校验的结果
*************************************************************************/
ushort PlcMathMethod::getXORcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*下标小于0，则从新赋值*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    int nLen = pCharData.size();
    if(nStartPos >= nLen) return 0;

    /*校验*/
    ushort XOR;
    XOR = pCharData.at(nStartPos);
    for(int i = nStartPos + 1; i < nLen; i++)
    {
        XOR = XOR ^ pCharData[i];
    }
    return (XOR);
}

/*************************************************************************
 * Funciton: 数字转换为ASCII码函数
 * Parameters: iNeedToChange:需要转换的数字
 * Return: 转换的结果
*************************************************************************/
char PlcMathMethod::intToAsciiValue(char nIntValue)
{
    if(nIntValue >= 0)
    {
        if (nIntValue < 10)
        {
            nIntValue += 0x30;
        }
        else if(nIntValue < 16)
        {
            nIntValue = (nIntValue-10) + 0x41;
        }
    }
    return nIntValue;
}

/*************************************************************************
 * Funciton: ASCII码转换为数字函数
 * Parameters: iNeedToChange:需要转换的ASCII码
 * Return: 转换的结果
*************************************************************************/
char PlcMathMethod::asciiToIntValue(char nAsciiValue)
{
    if(nAsciiValue >= 0x30 && nAsciiValue <= 0x39)
    {
        nAsciiValue -= 0x30;
    }
    else if(nAsciiValue > 0x40 && nAsciiValue < 0x47)
    {
        nAsciiValue = nAsciiValue - 0x41 + 10;
    }

    return nAsciiValue;
}

/*************************************************************************
 * Funciton: 地址的排序整理
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
vector<ADDR_TARGET_PROP > PlcMathMethod::addrBubbleSort(vector<ADDR_TARGET_PROP> sourceList)
{
    int size = sourceList.size();

    ADDR_TARGET_PROP addrHeadValue;
    ADDR_TARGET_PROP addrCompareValue;
    ADDR_TARGET_PROP tmpAddrValue;

    for(int i = 0; i < size; i++)
    {
        for(int j = i +1; j < size; j++)
        {
            addrHeadValue = sourceList.at(i);
            addrCompareValue = sourceList.at(j);
            if(addrHeadValue.nAddrValue > addrCompareValue.nAddrValue)
            {
                /*值交换*/
                tmpAddrValue = addrHeadValue;
                addrHeadValue = addrCompareValue;
                addrCompareValue = tmpAddrValue;

                sourceList[i] = addrHeadValue;
                sourceList[j] = addrCompareValue;
            }
        }
    }
    return sourceList;
}

/*************************************************************************
 * Funciton: 地址的整理
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
vector<ADDR_TARGET_PROP > PlcMathMethod::addrSortOut(vector<ADDR_TARGET_PROP> sourceList, int nMaxLen, int nNarrow, bool bByteReg ,bool bWriteBit)
{
    if(sourceList.empty()) return sourceList;
    if(nMaxLen <= 0 || nNarrow <= 0) return sourceList;

    vector<ADDR_TARGET_PROP > mResultList;
    mResultList.clear();

    int nIntervalLen = nMaxLen*nNarrow;
    int nSourceSize = sourceList.size();

    /*判断第一个地址是否属于0——nIntervalLen长度中，属于则添加起始地址*/
    int nLastValue = 0;
    READ_WRITE_COM_TYPE eLastRWtype = OTHER_CONTROL_TYPE;
    ADDR_TARGET_PROP tmpAddrValue;
    ADDR_TARGET_PROP mNewAddr;
    ADDR_TARGET_PROP mLastAddr;

    /*添加起始地址*/
    tmpAddrValue = sourceList.at(0);
    if(!bWriteBit){
        tmpAddrValue.nAddrLen += (tmpAddrValue.nAddrValue%nNarrow);
        tmpAddrValue.nAddrValue = (tmpAddrValue.nAddrValue/nNarrow) * nNarrow;
    }
    /*如果地址长度过长*/
    while(tmpAddrValue.nAddrLen > nIntervalLen)
    {
        mNewAddr= tmpAddrValue;
        mNewAddr.nAddrLen = nIntervalLen;

        tmpAddrValue.nAddrLen -= nIntervalLen;
        tmpAddrValue.nAddrValue += nIntervalLen;
        mResultList.push_back(mNewAddr);
    }

    /*添加最后一个地址*/
    if(tmpAddrValue.nAddrLen > 0)
    {
        mResultList.push_back(tmpAddrValue);
    }
    eLastRWtype = tmpAddrValue.eAddrRWprop;
    nLastValue = tmpAddrValue.nAddrValue;

    /*从第二个地址开始 添加后面的不重复的地址*/
    for(int i = 1; i < nSourceSize; i++)
    {
        tmpAddrValue = sourceList.at(i);
        mLastAddr = mResultList.back();

        int nNewLen = tmpAddrValue.nAddrValue - nLastValue + tmpAddrValue.nAddrLen;
        if(tmpAddrValue.nAddrValue < nLastValue)
        {
            tmpAddrValue.nAddrLen = nNewLen;
            tmpAddrValue.nAddrValue = nLastValue;
        }

        if(tmpAddrValue.nAddrValue - nLastValue + tmpAddrValue.nAddrLen > nIntervalLen)
        {
            nLastValue = (tmpAddrValue.nAddrValue/nNarrow) * nNarrow;
            tmpAddrValue.nAddrLen += (tmpAddrValue.nAddrValue%nNarrow);
            tmpAddrValue.nAddrValue = nLastValue;

            /*如果地址长度过长*/
            while(tmpAddrValue.nAddrLen > nIntervalLen)
            {
                mNewAddr = tmpAddrValue;
                mNewAddr.nAddrLen = nIntervalLen;

                tmpAddrValue.nAddrLen -= nIntervalLen;
                tmpAddrValue.nAddrValue += nIntervalLen;
                mResultList.push_back(mNewAddr);
            }

            /*添加最后一个地址*/
            if(tmpAddrValue.nAddrLen > 0)
            {
                mResultList.push_back(tmpAddrValue);
            }
            eLastRWtype = tmpAddrValue.eAddrRWprop;
            nLastValue = tmpAddrValue.nAddrValue;
        }
        else if(tmpAddrValue.nAddrValue - (mLastAddr.nAddrValue + mLastAddr.nAddrLen) > 32*nNarrow)
        {
            /*超过自定义读写长度,目前自定义值为15*/
            nLastValue = (tmpAddrValue.nAddrValue/nNarrow) * nNarrow;
            tmpAddrValue.nAddrLen += (tmpAddrValue.nAddrValue%nNarrow);
            tmpAddrValue.nAddrValue = nLastValue;

            /*如果地址长度过长*/
            while(tmpAddrValue.nAddrLen > nIntervalLen)
            {
                mNewAddr = tmpAddrValue;
                mNewAddr.nAddrLen = nIntervalLen;

                tmpAddrValue.nAddrLen -= nIntervalLen;
                tmpAddrValue.nAddrValue += nIntervalLen;
                nLastValue = tmpAddrValue.nAddrValue;
                mResultList.push_back(mNewAddr);
            }

            /*添加最后一个地址*/
            if(tmpAddrValue.nAddrLen > 0)
            {
                mResultList.push_back(tmpAddrValue);
            }
            eLastRWtype = tmpAddrValue.eAddrRWprop;
        }
        else
        {
            if(tmpAddrValue.eAddrRWprop >= SCENE_CONTROL_ONCE_R)
            {
                /*从新计算地址长度*/
                if(nNewLen > mLastAddr.nAddrLen)
                {
                    mLastAddr.nAddrLen = nNewLen;
                }

                if(tmpAddrValue.eAddrRWprop <  mLastAddr.eAddrRWprop)
                {
                    mLastAddr.eAddrRWprop = tmpAddrValue.eAddrRWprop;
                }
                mResultList.pop_back();
                mResultList.push_back(mLastAddr);
            }
        }
    }

    /*如果是一个字节寄存器, 则地址需要处理*/
    if(bByteReg)
    {
        int nSize = mResultList.size();
        for(int i = 0; i < nSize; i++)
        {
            if(mResultList[i].nAddrLen%2 != 0)
            {
                if(mResultList[i].nAddrValue > 0)
                {
                    mResultList[i].nAddrValue = mResultList[i].nAddrValue -1;
                }
                mResultList[i].nAddrLen = mResultList[i].nAddrLen/2 + 2;
            }
            else
            {
                mResultList[i].nAddrLen = mResultList[i].nAddrLen/2 + 1;
            }
        }
    }

    return mResultList;
}
