#include "localaddrs.h"

LocalAddrs::LocalAddrs()
{
    /* ����Ĵ���û��ӣ����ʼ���Ĵ������ݴ洢 */
    /*��ʼ��λ*/
    while(m_localAddrValueList.regBitList.size() < 1)
    {
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        m_localAddrValueList.regBitList.push_back(mTmpValueList);
    }

    /*��ʼ����*/
    while(m_localAddrValueList.regWordList.size() < 2)
    {
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        m_localAddrValueList.regWordList.push_back(mTmpValueList);
    }

    /*��ʼ���ַ���*/
    while(m_localAddrValueList.regStringList.size() < 1)
    {
        vector<ADDR_STRING_PROP > mTmpValueList;
        m_localAddrValueList.regStringList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */
}

/******************************************************************
 * Function: ���ݵ�ֵַ������ֵ
 * Parameters:
 * Return: true or false
 ******************************************************************/
void LocalAddrs::setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<uchar> dataList)
{
    if(dataList.empty()) return ;
    if(mAddr.nAddrLen <= 0 || mAddr.nAddrValue < 0) return;

    /*�����ݱ��浽��Ӧ��������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = CMN_NOMAL_CODE;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = CMN_NOMAL_CODE;

    ADDR_STRING_PROP emptyStrValue;
    emptyStrValue.nValue = "";
    emptyStrValue.eErrorStatus = CMN_NOMAL_CODE;

    int nListSize = dataList.size();
    int nRegIndex = mAddr.nRegIndex;
    switch(nRegIndex)
    {
    case 0:           //register index is bit
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regBitList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regBitList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            if(mAddr.nAddrLen != nListSize)
            {
                dataList.resize(mAddr.nAddrLen, 0);
                nListSize = mAddr.nAddrLen;
            }

            /*��ֵ*/
            for(int i = 0; i < nListSize; i++)
            {
                if(dataList.at(i) > 0)
                {
                    m_localAddrValueList.regBitList[0][mAddr.nAddrValue + i].nValue = 1;
                }
                else
                {
                    m_localAddrValueList.regBitList[0][mAddr.nAddrValue + i].nValue = 0;
                }
            }
            break;
        }
    case 1:           //register index is word
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            if(mAddr.nAddrLen*2 != nListSize)
            {
                dataList.resize(mAddr.nAddrLen*2, 0);
                nListSize = mAddr.nAddrLen*2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����ֵ*/
                nTmpValue = dataList.at(2*i + 1);
                nTmpValue &= 0xff;
                nTmpValue = (nTmpValue << 8) & 0xff00;
                nTmpValue += dataList.at(2*i);
                m_localAddrValueList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
            }
            break;
        }
    case 2:           //register index is RWI
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            if(mAddr.nAddrLen*2 != nListSize)
            {
                dataList.resize(mAddr.nAddrLen*2, 0);
                nListSize = mAddr.nAddrLen*2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����ֵ*/
                nTmpValue = dataList.at(2*i + 1);
                nTmpValue &= 0xff;
                nTmpValue = (nTmpValue << 8) & 0xff00;
                nTmpValue += dataList.at(2*i);
                m_localAddrValueList.regWordList[1][mAddr.nAddrValue + i].nValue = nTmpValue;
            }
            break;
        }
    case 3:           //register index is string
        {
            break;
        }
    default:
        {
            break;
        }
    }
}


/******************************************************************
 * Function: ���ݵ�ֵַ�����ַ���
 * Parameters:
 * Return: true or false
 ******************************************************************/
void LocalAddrs::setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string> strList)
{
    if(strList.empty()) return ;
    if(mAddr.nRegIndex != 3) return ;
    if(mAddr.nAddrLen <= 0 || mAddr.nAddrValue < 0) return;

    int nListSize = strList.size();

    ADDR_STRING_PROP emptyStrValue;
    emptyStrValue.nValue = "";
    emptyStrValue.eErrorStatus = CMN_NOMAL_CODE;

    /*���ݴ洢�����������������·���ռ�*/
    int nSize = m_localAddrValueList.regStringList[0].size();
    if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
    {
        m_localAddrValueList.regStringList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyStrValue);
    }

    if(mAddr.nAddrLen != nListSize)
    {
        strList.resize(mAddr.nAddrLen, "");
    }

    /*��ֵ*/
    for(int i = 0; i < mAddr.nAddrLen; i++)
    {
        m_localAddrValueList.regStringList[0][mAddr.nAddrValue +i].nValue = strList.at(i);
    }
}

/******************************************************************
 * Function: ���ݵ�ֵַ��ȡ��ֵ
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool LocalAddrs::getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP> &valueList)
{
    valueList.clear();
    if(mAddr.nAddrLen <= 0 || mAddr.nAddrValue < 0) return false;

    /*�����ݱ��浽��Ӧ��������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = CMN_NOMAL_CODE;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = CMN_NOMAL_CODE;

    ADDR_STRING_PROP emptyStrValue;
    emptyStrValue.nValue = "";
    emptyStrValue.eErrorStatus = CMN_NOMAL_CODE;

    int nRegIndex = mAddr.nRegIndex;
    switch(nRegIndex)
    {
    case 0:           //register index is bit
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regBitList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regBitList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            ADDR_BIT_VALUE_PROP nTmpValue;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                nTmpValue = m_localAddrValueList.regBitList[0].at(mAddr.nAddrValue + i);
                valueList.push_back(nTmpValue);
            }
            return true;
            break;
        }
    case 1:           //register index is word
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ADDR_BIT_VALUE_PROP mTmpValueProp;
            mTmpValueProp.eErrorStatus = CMN_NOMAL_CODE;
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����ֵ*/
                nTmpValue = m_localAddrValueList.regWordList[0].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                valueList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                valueList.push_back(mTmpValueProp);
            }
            return true;
            break;
        }
    case 2:           //register index is RWI
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = m_localAddrValueList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                m_localAddrValueList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ADDR_BIT_VALUE_PROP mTmpValueProp;
            mTmpValueProp.eErrorStatus = CMN_NOMAL_CODE;
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����ֵ*/
                nTmpValue = m_localAddrValueList.regWordList[1].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                valueList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                valueList.push_back(mTmpValueProp);
            }
            return true;
            break;
        }
    case 3:           //register index is string
        {
            break;
        }
    default:
        {
            break;
        }
    }

    return false;
}

/******************************************************************
 * Function: ���ݵ�ֵַ��ȡ�ַ���
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool LocalAddrs::getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string> &strList)
{
    strList.clear();
    if(mAddr.nRegIndex != 3) return false;
    if(mAddr.nAddrLen <= 0 || mAddr.nAddrValue < 0) return false;

    /*�����ݱ��浽��Ӧ��������*/
    ADDR_STRING_PROP emptyStrValue;
    emptyStrValue.nValue = "";
    emptyStrValue.eErrorStatus = CMN_NOMAL_CODE;

    /*���ݴ洢�����������������·���ռ�*/
    int nSize = m_localAddrValueList.regStringList[0].size();
    if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
    {
        m_localAddrValueList.regStringList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyStrValue);
    }

    /*��ֵ*/
    for(int i = 0; i < mAddr.nAddrLen; i++)
    {
        strList.push_back(m_localAddrValueList.regStringList[0].at(mAddr.nAddrValue +i).nValue);
    }

    return true;
}
