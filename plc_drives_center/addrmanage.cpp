#include "addrmanage.h"
#include "tools/plcmathmethod.h"

AddrManage::AddrManage()
{
    m_nReadLevel = 1;
    m_nWriteLevel = 1;
    m_nRLevel1UseTimes = 0;
    m_nWLevel1UseTimes = 0;
    m_nGetAddrTimes = 0;
    m_nMaxTimes = 100;

    m_nReadIndex1 = 0;
    m_nReadIndex2 = 0;
    m_nReadIndex3 = 0;
    m_nReadIndex4 = 0;

    m_nOldReadLevel = 1;
    m_nOldReadIndex = 0;
    m_readAddrListbuf.clear();
}

/******************************************************************
 * Function: ���ö���ַ
 * Parameters: address list
 * Return: null
 ******************************************************************/
void AddrManage::setReadAddrList(vector<ADDR_RW_STRUCT> mSourceAddrList, bool bCover)
{
    /*������ǣ������������ԭʼ����*/
    if(bCover)
    {
        m_readAddrListLevel1.clear();
        m_readAddrListLevel2.clear();
        m_readAddrListLevel3.clear();
        m_readAddrListLevel4.clear();
        m_nGetAddrTimes = 0;
        m_nReadLevel = 1;
        m_nOldReadLevel = 1;
        m_nOldReadIndex = 0;

        m_nReadIndex1 = 0;
        m_nReadIndex2 = 0;
        m_nReadIndex3 = 0;
        m_nReadIndex4 = 0;

        m_nMaxTimes = mSourceAddrList.size()*10;
    }
    else
    {
        for(int i = 0; i < m_readAddrListLevel1.size(); i++)
        {
            m_readAddrListLevel2.push_back(m_readAddrListLevel1.at(i));
        }
        m_nGetAddrTimes = 0;
        m_nReadLevel = 1;
        m_nOldReadLevel = 1;
        m_nOldReadIndex = 0;

        m_nReadIndex1 = 0;
        m_nReadIndex2 = 0;
        m_nReadIndex3 = 0;
        m_nReadIndex4 = 0;
        m_readAddrListLevel1.clear();
    }

    /*����µ�����*/
    int nSize = mSourceAddrList.size();
    ADDR_RW_STRUCT mTmpAddr;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);
        if(mTmpAddr.mAddrProp.nAddrValue + mTmpAddr.mAddrProp.nAddrLen > MAXADDR
           || mTmpAddr.mAddrProp.nAddrValue < 0)
        {
            LOGE("addrmanage setReadAddrList nAddrValue value is: %d, nAddrlen is: %d",
                 mTmpAddr.mAddrProp.nAddrValue,mTmpAddr.mAddrProp.nAddrLen);
            continue;
        }
        READ_WRITE_COM_TYPE eAddrRWType = mTmpAddr.mAddrProp.eAddrRWprop;
        switch(eAddrRWType)
        {
        case SCENE_CONTROL_ONCE_R:
        case SCENE_CONTROL_LOOP_R:
        case ALARM_LOOP_R:
        case RECIPE_ONCE_R:
        case GLOBAL_ONCE_R:
        case GLOBAL_LOOP_R:
        case MACRO_ONCE_R:
        case MACRO_LOOP_R:
        case DATA_COLLECT_ONCE_R:
        case DATA_COLLECT_LOOP_R:
            {
                /*��Ӷ�������*/
                setAddrToList(mTmpAddr);
                break;
            }
        default:
            {
                return ;
                break;
            }
        }
    }
}

/******************************************************************
 * Function: ����д��ַ
 * Parameters: address list
 * Return: null
 ******************************************************************/
void AddrManage::setWriteAddrList(ADDR_RW_STRUCT mSourceAddr)
{
    /*�жϵ�ַ������ �������д�����򷵻�*/
    READ_WRITE_COM_TYPE eAddrRWType = mSourceAddr.mAddrProp.eAddrRWprop;
    switch(eAddrRWType)
    {
    case SCENE_CONTROL_ONCE_W:
    case SCENE_CONTROL_LOOP_W:
    case MACRO_ONCE_W:
    case MACRO_LOOP_W:
    case GLOBAL_ONCE_W:
    case GLOBAL_LOOP_W:
        {
            break;
        }
    default:
        return ;
    }

    /*���д������*/
    setAddrToList(mSourceAddr);
}

/******************************************************************
 * Function: ȡ���¸���Ҫ���ĵ�ַ
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool AddrManage::getNextReadAddr(ADDR_RW_STRUCT &mReadAddr)
{
    int nListSize = 0;
    switch(m_nReadLevel)
    {
    case 1:
        {
            /*�ȼ�1��ʹ�ô�����1*/
            m_nRLevel1UseTimes++;

            if(m_nRLevel1UseTimes > 2)
            {
                m_nReadLevel = 2;
            }

            /*�ȼ�1����Ϊ���������ȼ�2*/
            if(m_readAddrListLevel1.empty())
            {
                m_nReadLevel = 2;
                m_nReadIndex1 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*���ȼ�1�����Ϸ���ȡ������*/
            nListSize = m_readAddrListLevel1.size();
            if(m_nReadIndex1 < 0 || m_nReadIndex1 >= nListSize)
            {
                m_nReadIndex1 = 0;
            }
            mReadAddr = m_readAddrListLevel1.at(m_nReadIndex1);

            /*��¼��ǰ��ַ*/
            m_nOldReadLevel = 1;
            m_nOldReadIndex = m_nReadIndex1;
            m_nReadIndex1++;
            break;
        }
    case 2:
        {
            /*�ȼ�1���������� �ٴν���ȼ�1*/
            if(m_nRLevel1UseTimes < 3)
            {
                m_nReadLevel = 1;
            }
            else
            {
                m_nReadLevel = 3;
            }

            /*�ȼ�2����Ϊ����������Ӧ�ȼ�*/
            if(m_readAddrListLevel2.empty())
            {
                m_nReadIndex2 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*���ȼ�2�����Ϸ���ȡ������*/
            nListSize = m_readAddrListLevel2.size();
            if(m_nReadIndex2 < 0 || m_nReadIndex2 >= nListSize)
            {
                m_nReadIndex2 = 0;
            }
            mReadAddr = m_readAddrListLevel2.at(m_nReadIndex2);

            /*��¼��ǰ��ַ*/
            m_nOldReadLevel = 2;
            m_nOldReadIndex = m_nReadIndex2;
            m_nReadIndex2++;
            break;
        }
    case 3:
        {
            /*�ȼ�1����3���� �ٴν���ȼ�1*/
            if(m_nRLevel1UseTimes < 4)
            {
                m_nReadLevel = 1;
            }
            else
            {
                m_nReadLevel = 4;
            }

            /*�ȼ�3����Ϊ����������Ӧ�ȼ�*/
            if(m_readAddrListLevel3.empty())
            {
                m_nReadIndex3 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*���ȼ�3�����Ϸ���ȡ������*/
            nListSize = m_readAddrListLevel3.size();
            if(m_nReadIndex3 < 0 || m_nReadIndex3 >= nListSize)
            {
                m_nReadIndex3 = 0;
            }
            mReadAddr = m_readAddrListLevel3.at(m_nReadIndex3);

            /*��¼��ǰ��ַ*/
            m_nOldReadLevel = 3;
            m_nOldReadIndex = m_nReadIndex3;
            m_nReadIndex3++;
            break;
        }
    case 4:
        {
            /*�ȼ�1����4���� �ٴν���ȼ�1*/
            m_nRLevel1UseTimes = 0;
            m_nReadLevel = 1;

            /*�ȼ�4����Ϊ����������Ӧ�ȼ�*/
            if(m_readAddrListLevel4.empty())
            {
                if(m_readAddrListLevel1.empty() && m_readAddrListLevel2.empty() && m_readAddrListLevel3.empty())
                {
                    m_nReadIndex4 = 0;
                    return false;
                }
                else
                {
                    return getNextReadAddr(mReadAddr);
                }
            }

            /*���ȼ�4�����Ϸ���ȡ������*/
            nListSize = m_readAddrListLevel4.size();
            if(m_nReadIndex4 < 0 || m_nReadIndex4 >= nListSize)
            {
                m_nReadIndex4 = 0;
            }
            mReadAddr = m_readAddrListLevel4.at(m_nReadIndex4);

            /*��¼��ǰ��ַ*/
            m_nOldReadLevel = 4;
            m_nOldReadIndex = m_nReadIndex4;
            m_nReadIndex4++;
            break;
        }
    default:
        {
            m_nRLevel1UseTimes = 0;
            m_nReadLevel = 1;
            m_nOldReadLevel = 1;
            m_nOldReadIndex = 0;
            return false;
        }
    }

    /*���������ַ��������*/
    if(++m_nGetAddrTimes >= m_nMaxTimes)
    {
        m_nGetAddrTimes = 0;
 //       reCollateAddrpriority();
    }
    return true;
}

/******************************************************************
 * Function: ȡ���¸���Ҫд�ĵ�ַ
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool AddrManage::getNextWriteAddr(ADDR_RW_STRUCT &mWriteAddr)
{
    switch(m_nWriteLevel)
    {
    case 1:
        {
            /*�ȼ�1��ʹ�ô�����1*/
            m_nWLevel1UseTimes++;

            if(m_nWLevel1UseTimes > 1)
            {
                m_nWriteLevel = 2;
            }

            /*�ȼ�1����Ϊ���������ȼ�2*/
            if(m_writeAddrListLevel1.empty())
            {
                m_nWriteLevel = 2;
                return getNextWriteAddr(mWriteAddr);
            }

            /*���ȼ�1�����Ϸ���ȡ������*/
            mWriteAddr = m_writeAddrListLevel1.front();
            m_writeAddrListLevel1.pop_front();
            break;
        }
    case 2:
        {
            /*�ȼ�1���������� �ٴν���ȼ�1*/
            if(m_nWLevel1UseTimes < 3)
            {
                m_nWriteLevel = 1;
            }
            else
            {
                m_nWriteLevel = 3;
            }

            /*�ȼ�2����Ϊ����������Ӧ�ȼ�*/
            if(m_writeAddrListLevel2.empty())
            {
                return getNextWriteAddr(mWriteAddr);
            }

            mWriteAddr = m_writeAddrListLevel2.front();
            m_writeAddrListLevel2.pop_front();
            break;
        }
    case 3:
        {
            /*�������ȼ�������*/
            m_nWriteLevel = 1;

            m_nWLevel1UseTimes = 0;

            /*�ȼ�3����Ϊ����������Ӧ�ȼ�1*/
            if(m_writeAddrListLevel3.empty())
            {
                if(m_writeAddrListLevel1.empty() && m_writeAddrListLevel2.empty())
                {
                    return false;
                }
                else
                {
                    return getNextWriteAddr(mWriteAddr);
                }
            }

            mWriteAddr = m_writeAddrListLevel3.front();
            m_writeAddrListLevel3.pop_front();
            break;
        }
    default:
        {
            m_nWLevel1UseTimes = 0;
            m_nWriteLevel = 1;
            return false;
        }
    }

    return true;
}

/******************************************************************
 * Function: ��ӵ�ַ����д��������ȥ
 * Parameters: address
 * Return: null
 ******************************************************************/
void AddrManage::setAddrToList(ADDR_RW_STRUCT mAddr)
{
    READ_WRITE_COM_TYPE eTmpRWtype = mAddr.mAddrProp.eAddrRWprop;
    switch(eTmpRWtype)
    {
    case SCENE_CONTROL_ONCE_W:
    case MACRO_ONCE_W:
    case GLOBAL_ONCE_W:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel1, true)) break;

            m_writeAddrListLevel1.push_back(mAddr);
            break;
        }
    case SCENE_CONTROL_LOOP_W:
    case GLOBAL_LOOP_W:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel2, true)) break;

            m_writeAddrListLevel2.push_back(mAddr);
            break;
        }
    case MACRO_LOOP_W:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel3, true)) break;

            m_writeAddrListLevel3.push_back(mAddr);
            break;
        }
    case SCENE_CONTROL_ONCE_R:
    case SCENE_CONTROL_LOOP_R:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel1) > -1) break;

            int index = PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel2);
            if(index > -1 && m_readAddrListLevel2.size() > index)
            {
                m_readAddrListLevel2.erase(m_readAddrListLevel2.begin() + index);
            }

            m_readAddrListLevel1.push_back(mAddr);
            m_nMaxTimes += 1;
            break;
        }
    case GLOBAL_ONCE_R:
    case GLOBAL_LOOP_R:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel2) >-1) break;

            int index = PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel1);
            if(index > -1 && m_readAddrListLevel1.size() > index)
            {
                m_readAddrListLevel1.erase(m_readAddrListLevel1.begin() + index);
            }

            m_readAddrListLevel2.push_back(mAddr);
            m_nMaxTimes += 3;
            break;
        }
    case MACRO_ONCE_R:
    case MACRO_LOOP_R:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel3) > -1) break;

            m_readAddrListLevel3.push_back(mAddr);
            m_nMaxTimes += 6;
            break;
        }
    case ALARM_LOOP_R:
    case DATA_COLLECT_ONCE_R:
    case DATA_COLLECT_LOOP_R:
        {
            /*�����������ӣ�������*/
            if(PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel4) > -1) break;

            m_readAddrListLevel4.push_back(mAddr);
            m_nMaxTimes += 10;
            break;
        }
    case RECIPE_ONCE_R:
    case OTHER_CONTROL_TYPE:
    default:
        {
            break;
        }
    }
}

/******************************************************************
 * Function: �Ե�ַ��ֵ
 * Parameters: address
 * Return: null
 ******************************************************************/
bool AddrManage::setAddrValue(ADDR_TARGET_PROP mAddr, vector<unsigned char> valueList)
{
    bool bSuccess = true;

    /*���ݵ�ǰ�ȼ��ź��±�Ų�ѯ��ַ*/
    switch(m_nOldReadLevel)
    {
    case 1:
        {
            /*�жϵ�ַ�±��Ƿ�Ƿ�*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel1.size())
            {
                /*ȡ��ֵַ*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel1.at(m_nOldReadIndex);

                /*��������ַ��ȣ���ַ��ֵ���ȣ���ʹ�ô�����1*/
                if(PlcMathMethod::compareAddr(mCurrAddr.mAddrProp,mAddr))
                {
                    if(mCurrAddr.nValueList != valueList)
                    {
                        mCurrAddr.nUseTimes = 0;
                        mCurrAddr.nValueList = valueList;
                        m_readAddrListLevel1[m_nOldReadIndex] = mCurrAddr;
                    }
                    else
                    {
                        mCurrAddr.nUseTimes++;
                        if(mCurrAddr.nUseTimes > 5)
                        {
                            mCurrAddr.nUseTimes = 0;
                            mCurrAddr.nValueList = valueList;
                            m_readAddrListLevel1.erase(m_readAddrListLevel1.begin() + m_nOldReadIndex);// = mCurrAddr;
                            m_readAddrListLevel2.insert(m_readAddrListLevel2.begin(),mCurrAddr);
                            if(m_nReadIndex1 > 0)
                            {
                                m_nReadIndex1--;
                            }
                        }
                        else
                        {
                            m_readAddrListLevel1[m_nOldReadIndex] = mCurrAddr;
                        }

                        bSuccess = false;
                    }
                }
            }
            break;
        }
    case 2:
        {
            /*�жϵ�ַ�±��Ƿ�Ƿ�*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel2.size())
            {
                /*ȡ��ֵַ*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel2.at(m_nOldReadIndex);
                /*��������ַ��ȣ���ַ��ֵ���ȣ���ʹ�ô�����1*/
                if(PlcMathMethod::compareAddr(mCurrAddr.mAddrProp,mAddr))
                {
                    if(mCurrAddr.nValueList != valueList)
                    {
                        //mCurrAddr.nUseTimes++;
                        mCurrAddr.nValueList = valueList;
                        //m_readAddrListLevel2[m_nOldReadIndex] = mCurrAddr;

                        mCurrAddr.nUseTimes = 0;
                        m_readAddrListLevel2.erase(m_readAddrListLevel2.begin() + m_nOldReadIndex);
                        m_readAddrListLevel1.insert(m_readAddrListLevel1.begin(),mCurrAddr);
                        if(m_nReadIndex2 > 0)
                        {
                            m_nReadIndex2--;
                        }
                        m_nReadIndex1++;

                    }
                    else
                    {
                        bSuccess = false;
                    }
                }
            }
            break;
        }
    case 3:
        {
            /*�жϵ�ַ�±��Ƿ�Ƿ�*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel3.size())
            {
                /*ȡ��ֵַ*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel3.at(m_nOldReadIndex);
                /*��������ַ��ȣ���ַ��ֵ���ȣ���ʹ�ô�����1*/
                if(PlcMathMethod::compareAddr(mCurrAddr.mAddrProp, mAddr))
                {
                    if(mCurrAddr.nValueList != valueList)
                    {
                        mCurrAddr.nUseTimes++;
                        mCurrAddr.nValueList = valueList;
                        m_readAddrListLevel3[m_nOldReadIndex] = mCurrAddr;
                    }
                    else
                    {
                        bSuccess = false;
                    }
                }
            }
            break;
        }
    case 4:
        {
            /*�жϵ�ַ�±��Ƿ�Ƿ�*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel4.size())
            {
                /*ȡ��ֵַ*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel4.at(m_nOldReadIndex);
                /*��������ַ��ȣ���ַ��ֵ���ȣ���ʹ�ô�����1*/
                if(PlcMathMethod::compareAddr(mCurrAddr.mAddrProp, mAddr))
                {
                    if(mCurrAddr.nValueList != valueList)
                    {
                        mCurrAddr.nUseTimes++;
                        mCurrAddr.nValueList = valueList;
                        m_readAddrListLevel4[m_nOldReadIndex] = mCurrAddr;
                    }
                    else
                    {
                        bSuccess = false;
                    }
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return bSuccess;
}

/******************************************************************
 * Function: ���ݶ�����ֵ�ĸı�����Ե�ַ���ȼ����·���
 * Parameters: null
 * Return: null
 ******************************************************************/
void AddrManage::reCollateAddrpriority()
{
    /*�������±긴λ*/
    m_nReadLevel = 1;
    m_nReadIndex1 = 0;
    m_nReadIndex2 = 0;
    m_nReadIndex3 = 0;
    m_nReadIndex4 = 0;

    /*��������0*/
    m_nGetAddrTimes = 0;

    int nSize = 0;
    int nMaxUseTimes = 0;
    int nAvgTimes = 0;
    int nTmpTimes = 0;
    ADDR_RW_STRUCT mTmpAddr;

    /*��ʱ����*/
    vector<ADDR_RW_STRUCT > tmpListLevel1;
    vector<ADDR_RW_STRUCT > tmpListLevel2;
    vector<ADDR_RW_STRUCT > tmpListLevel3;
    vector<ADDR_RW_STRUCT > tmpListLevel4;

    /*������ȼ�1����Ч������*/
    nSize = m_readAddrListLevel1.size();
    for(int i = 0; i < nSize; i++)
    {
        /*ȡ���ֵ*/
        if(m_readAddrListLevel1.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*���¸�ֵ*/
    nAvgTimes = nMaxUseTimes/2;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = m_readAddrListLevel1.at(i);
        nTmpTimes = mTmpAddr.nUseTimes;
        mTmpAddr.nUseTimes = 0;

        READ_WRITE_COM_TYPE eTmpRWtype = mTmpAddr.mAddrProp.eAddrRWprop;
        switch(eTmpRWtype)
        {
        case SCENE_CONTROL_ONCE_R:
        case SCENE_CONTROL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*�仯��������0���ŵ��ȼ�1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        default:
            {
                if(nTmpTimes >= nAvgTimes)
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                else
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                break;
            }
        }
    }

    /*������ȼ�2����Ч������*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel2.size();
    for(int i = 0; i < nSize; i++)
    {
        /*ȡ���ֵ*/
        if(m_readAddrListLevel2.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*���¸�ֵ*/
    nAvgTimes = nMaxUseTimes/2;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = m_readAddrListLevel2.at(i);
        nTmpTimes = mTmpAddr.nUseTimes;
        mTmpAddr.nUseTimes = 0;

        READ_WRITE_COM_TYPE eTmpRWtype = mTmpAddr.mAddrProp.eAddrRWprop;
        switch(eTmpRWtype)
        {
        case SCENE_CONTROL_ONCE_R:
        case SCENE_CONTROL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*�仯��������0���ŵ��ȼ�1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        case GLOBAL_ONCE_R:
        case GLOBAL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else if(nTmpTimes < nAvgTimes)
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        case MACRO_ONCE_R:
        case MACRO_LOOP_R:
            {
                if(nTmpTimes >= nAvgTimes)
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                break;
            }
        case ALARM_LOOP_R:
        case DATA_COLLECT_ONCE_R:
        case DATA_COLLECT_LOOP_R:
            {
                /*һ��Ҳû�仯�����ŵ��ȼ�3*/
                tmpListLevel3.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }

    /*������ȼ�3����Ч������*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel3.size();
    for(int i = 0; i < nSize; i++)
    {
        /*ȡ���ֵ*/
        if(m_readAddrListLevel3.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*���¸�ֵ*/
    nAvgTimes = nMaxUseTimes/2;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = m_readAddrListLevel3.at(i);
        nTmpTimes = mTmpAddr.nUseTimes;
        mTmpAddr.nUseTimes = 0;

        READ_WRITE_COM_TYPE eTmpRWtype = mTmpAddr.mAddrProp.eAddrRWprop;
        switch(eTmpRWtype)
        {
        case SCENE_CONTROL_ONCE_R:
        case SCENE_CONTROL_LOOP_R:
            {
                /*һ��Ҳû�仯�����ŵ��ȼ�2*/
                tmpListLevel2.push_back(mTmpAddr);
                break;
            }
        case GLOBAL_ONCE_R:
        case GLOBAL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                break;
            }
        case MACRO_ONCE_R:
        case MACRO_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�4*/
                    tmpListLevel4.push_back(mTmpAddr);
                }
                else if(nTmpTimes < nAvgTimes)
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                break;
            }
        case ALARM_LOOP_R:
        case DATA_COLLECT_ONCE_R:
        case DATA_COLLECT_LOOP_R:
            {
                if(nTmpTimes > 0)
                {
                    /*�仯��������ƽ��ֵ���ŵ��ȼ�3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*һ��Ҳû�仯�����ŵ��ȼ�4*/
                    tmpListLevel4.push_back(mTmpAddr);
                }
                break;
            }
        default:
            {
                break;
            }
        }
    }

    /*������ȼ�4����Ч������*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel4.size();
    for(int i = 0; i < nSize; i++)
    {
        /*ȡ���ֵ*/
        if(m_readAddrListLevel4.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*���¸�ֵ*/
    nAvgTimes = nMaxUseTimes/2;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = m_readAddrListLevel4.at(i);
        nTmpTimes = mTmpAddr.nUseTimes;
        mTmpAddr.nUseTimes = 0;

        if(nTmpTimes > 0)
        {
            /*�仯��������ƽ��ֵ���ŵ��ȼ�3*/
            tmpListLevel3.push_back(mTmpAddr);
        }
        else
        {
            /*һ��Ҳû�仯�����ŵ��ȼ�4*/
            tmpListLevel4.push_back(mTmpAddr);
        }
    }

    /*�����ֵ��ԭ��������*/
    m_readAddrListLevel1.clear();
    m_readAddrListLevel2.clear();
    m_readAddrListLevel3.clear();
    m_readAddrListLevel4.clear();

    m_readAddrListLevel1 = tmpListLevel1;
    m_readAddrListLevel2 = tmpListLevel2;
    m_readAddrListLevel3 = tmpListLevel3;
    m_readAddrListLevel4 = tmpListLevel4;
}

/******************************************************************
 * Function: ���д��ַ
 * Parameters: null
 * Return: null
 ******************************************************************/
void AddrManage::clearWriteAddr()
{
    m_writeAddrListLevel1.clear();
    m_writeAddrListLevel2.clear();
    m_writeAddrListLevel3.clear();
}

/******************************************************************
 * Function: �������ַ
 * Parameters: null
 * Return: null
 ******************************************************************/
void AddrManage::clearReadAddr()
{
    m_readAddrListLevel1.clear();
    m_readAddrListLevel2.clear();
    m_readAddrListLevel3.clear();
    m_readAddrListLevel4.clear();
}

/*�л����棬��¼��ַ*/
void AddrManage::setAddrListBuf(vector<ADDR_TARGET_PROP> addrList)
{
    m_readAddrListbuf.clear();
    m_readAddrListbuf = addrList;
}

/*�л����棬��ַ*/
vector<ADDR_TARGET_PROP> AddrManage::getAddrListBuf()
{
    return m_readAddrListbuf;
}
