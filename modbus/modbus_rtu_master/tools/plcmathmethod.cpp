#include "plcmathmethod.h"

PlcMathMethod::PlcMathMethod()
{
}

/*************************************************************************
 * Funciton: ��У�麯��
 * Parameters: pCharData:��ҪУ�������ָ��
               nLen:��У�鳤��
 * Return: ��У��Ľ��
*************************************************************************/
ushort PlcMathMethod::getSumCheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*�±�С��0������¸�ֵ*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*У��*/
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
 * Funciton: CRCУ�麯��
 * Parameters: pCharData:��ҪУ�������ָ��
               nLen:��У�鳤��
 * Return: CRCУ��Ľ��
*************************************************************************/
ushort PlcMathMethod::getCRCcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*�±�С��0������¸�ֵ*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*У��*/
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
 * Funciton: LRCУ�麯��
 * Parameters: pCharData:��ҪУ�������ָ��
               nLen:��У�鳤��
 * Return: LRCУ��Ľ��
*************************************************************************/
ushort PlcMathMethod::getLRCcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*�±�С��0������¸�ֵ*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    /*У��*/
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
 * Funciton: XORУ�麯��
 * Parameters: pCharData:��ҪУ�������ָ��
               nLen:��У�鳤��
 * Return: XORУ��Ľ��
*************************************************************************/
ushort PlcMathMethod::getXORcheck(vector<unsigned char> pCharData, int nStartPos)
{
    /*�±�С��0������¸�ֵ*/
    if(nStartPos < 0)
    {
        nStartPos = 0;
    }

    int nLen = pCharData.size();
    if(nStartPos >= nLen) return 0;

    /*У��*/
    ushort XOR;
    XOR = pCharData.at(nStartPos);
    for(int i = nStartPos + 1; i < nLen; i++)
    {
        XOR = XOR ^ pCharData[i];
    }
    return (XOR);
}

/*************************************************************************
 * Funciton: ����ת��ΪASCII�뺯��
 * Parameters: iNeedToChange:��Ҫת��������
 * Return: ת���Ľ��
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
 * Funciton: ASCII��ת��Ϊ���ֺ���
 * Parameters: iNeedToChange:��Ҫת����ASCII��
 * Return: ת���Ľ��
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
 * Funciton: ��ַ����������
 * Parameters:
 * Return: ת���Ľ��
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
                /*ֵ����*/
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
 * Funciton: ��ַ������
 * Parameters:
 * Return: ת���Ľ��
*************************************************************************/
vector<ADDR_TARGET_PROP > PlcMathMethod::addrSortOut(vector<ADDR_TARGET_PROP> sourceList, int nMaxLen, int nNarrow, bool bByteReg ,bool bWriteBit)
{
    if(sourceList.empty()) return sourceList;
    if(nMaxLen <= 0 || nNarrow <= 0) return sourceList;

    vector<ADDR_TARGET_PROP > mResultList;
    mResultList.clear();

    int nIntervalLen = nMaxLen*nNarrow;
    int nSourceSize = sourceList.size();

    /*�жϵ�һ����ַ�Ƿ�����0����nIntervalLen�����У������������ʼ��ַ*/
    int nLastValue = 0;
    READ_WRITE_COM_TYPE eLastRWtype = OTHER_CONTROL_TYPE;
    ADDR_TARGET_PROP tmpAddrValue;
    ADDR_TARGET_PROP mNewAddr;
    ADDR_TARGET_PROP mLastAddr;

    /*�����ʼ��ַ*/
    tmpAddrValue = sourceList.at(0);
    if(!bWriteBit){
        tmpAddrValue.nAddrLen += (tmpAddrValue.nAddrValue%nNarrow);
        tmpAddrValue.nAddrValue = (tmpAddrValue.nAddrValue/nNarrow) * nNarrow;
    }
    /*�����ַ���ȹ���*/
    while(tmpAddrValue.nAddrLen > nIntervalLen)
    {
        mNewAddr= tmpAddrValue;
        mNewAddr.nAddrLen = nIntervalLen;

        tmpAddrValue.nAddrLen -= nIntervalLen;
        tmpAddrValue.nAddrValue += nIntervalLen;
        mResultList.push_back(mNewAddr);
    }

    /*������һ����ַ*/
    if(tmpAddrValue.nAddrLen > 0)
    {
        mResultList.push_back(tmpAddrValue);
    }
    eLastRWtype = tmpAddrValue.eAddrRWprop;
    nLastValue = tmpAddrValue.nAddrValue;

    /*�ӵڶ�����ַ��ʼ ��Ӻ���Ĳ��ظ��ĵ�ַ*/
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

            /*�����ַ���ȹ���*/
            while(tmpAddrValue.nAddrLen > nIntervalLen)
            {
                mNewAddr = tmpAddrValue;
                mNewAddr.nAddrLen = nIntervalLen;

                tmpAddrValue.nAddrLen -= nIntervalLen;
                tmpAddrValue.nAddrValue += nIntervalLen;
                mResultList.push_back(mNewAddr);
            }

            /*������һ����ַ*/
            if(tmpAddrValue.nAddrLen > 0)
            {
                mResultList.push_back(tmpAddrValue);
            }
            eLastRWtype = tmpAddrValue.eAddrRWprop;
            nLastValue = tmpAddrValue.nAddrValue;
        }
        else if(tmpAddrValue.nAddrValue - (mLastAddr.nAddrValue + mLastAddr.nAddrLen) > 32*nNarrow)
        {
            /*�����Զ����д����,Ŀǰ�Զ���ֵΪ15*/
            nLastValue = (tmpAddrValue.nAddrValue/nNarrow) * nNarrow;
            tmpAddrValue.nAddrLen += (tmpAddrValue.nAddrValue%nNarrow);
            tmpAddrValue.nAddrValue = nLastValue;

            /*�����ַ���ȹ���*/
            while(tmpAddrValue.nAddrLen > nIntervalLen)
            {
                mNewAddr = tmpAddrValue;
                mNewAddr.nAddrLen = nIntervalLen;

                tmpAddrValue.nAddrLen -= nIntervalLen;
                tmpAddrValue.nAddrValue += nIntervalLen;
                nLastValue = tmpAddrValue.nAddrValue;
                mResultList.push_back(mNewAddr);
            }

            /*������һ����ַ*/
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
                /*���¼����ַ����*/
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

    /*�����һ���ֽڼĴ���, ���ַ��Ҫ����*/
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
