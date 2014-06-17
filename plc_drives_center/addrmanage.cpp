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
 * Function: 设置读地址
 * Parameters: address list
 * Return: null
 ******************************************************************/
void AddrManage::setReadAddrList(vector<ADDR_RW_STRUCT> mSourceAddrList, bool bCover)
{
    /*如果覆盖，则清除掉容器原始数据*/
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

    /*添加新的数据*/
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
                /*添加读的数据*/
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
 * Function: 设置写地址
 * Parameters: address list
 * Return: null
 ******************************************************************/
void AddrManage::setWriteAddrList(ADDR_RW_STRUCT mSourceAddr)
{
    /*判断地址的类型 如果不是写类型则返回*/
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

    /*添加写的数据*/
    setAddrToList(mSourceAddr);
}

/******************************************************************
 * Function: 取得下个将要读的地址
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
            /*等级1的使用次数加1*/
            m_nRLevel1UseTimes++;

            if(m_nRLevel1UseTimes > 2)
            {
                m_nReadLevel = 2;
            }

            /*等级1容器为空则跳到等级2*/
            if(m_readAddrListLevel1.empty())
            {
                m_nReadLevel = 2;
                m_nReadIndex1 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*检查等级1容器合法并取出数据*/
            nListSize = m_readAddrListLevel1.size();
            if(m_nReadIndex1 < 0 || m_nReadIndex1 >= nListSize)
            {
                m_nReadIndex1 = 0;
            }
            mReadAddr = m_readAddrListLevel1.at(m_nReadIndex1);

            /*记录当前地址*/
            m_nOldReadLevel = 1;
            m_nOldReadIndex = m_nReadIndex1;
            m_nReadIndex1++;
            break;
        }
    case 2:
        {
            /*等级1少于两次则 再次进入等级1*/
            if(m_nRLevel1UseTimes < 3)
            {
                m_nReadLevel = 1;
            }
            else
            {
                m_nReadLevel = 3;
            }

            /*等级2容器为空则跳到对应等级*/
            if(m_readAddrListLevel2.empty())
            {
                m_nReadIndex2 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*检查等级2容器合法并取出数据*/
            nListSize = m_readAddrListLevel2.size();
            if(m_nReadIndex2 < 0 || m_nReadIndex2 >= nListSize)
            {
                m_nReadIndex2 = 0;
            }
            mReadAddr = m_readAddrListLevel2.at(m_nReadIndex2);

            /*记录当前地址*/
            m_nOldReadLevel = 2;
            m_nOldReadIndex = m_nReadIndex2;
            m_nReadIndex2++;
            break;
        }
    case 3:
        {
            /*等级1少于3次则 再次进入等级1*/
            if(m_nRLevel1UseTimes < 4)
            {
                m_nReadLevel = 1;
            }
            else
            {
                m_nReadLevel = 4;
            }

            /*等级3容器为空则跳到对应等级*/
            if(m_readAddrListLevel3.empty())
            {
                m_nReadIndex3 = 0;
                return getNextReadAddr(mReadAddr);
            }

            /*检查等级3容器合法并取出数据*/
            nListSize = m_readAddrListLevel3.size();
            if(m_nReadIndex3 < 0 || m_nReadIndex3 >= nListSize)
            {
                m_nReadIndex3 = 0;
            }
            mReadAddr = m_readAddrListLevel3.at(m_nReadIndex3);

            /*记录当前地址*/
            m_nOldReadLevel = 3;
            m_nOldReadIndex = m_nReadIndex3;
            m_nReadIndex3++;
            break;
        }
    case 4:
        {
            /*等级1少于4次则 再次进入等级1*/
            m_nRLevel1UseTimes = 0;
            m_nReadLevel = 1;

            /*等级4容器为空则跳到对应等级*/
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

            /*检查等级4容器合法并取出数据*/
            nListSize = m_readAddrListLevel4.size();
            if(m_nReadIndex4 < 0 || m_nReadIndex4 >= nListSize)
            {
                m_nReadIndex4 = 0;
            }
            mReadAddr = m_readAddrListLevel4.at(m_nReadIndex4);

            /*记录当前地址*/
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

    /*重新整理地址的周期数*/
    if(++m_nGetAddrTimes >= m_nMaxTimes)
    {
        m_nGetAddrTimes = 0;
 //       reCollateAddrpriority();
    }
    return true;
}

/******************************************************************
 * Function: 取得下个将要写的地址
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool AddrManage::getNextWriteAddr(ADDR_RW_STRUCT &mWriteAddr)
{
    switch(m_nWriteLevel)
    {
    case 1:
        {
            /*等级1的使用次数加1*/
            m_nWLevel1UseTimes++;

            if(m_nWLevel1UseTimes > 1)
            {
                m_nWriteLevel = 2;
            }

            /*等级1容器为空则跳到等级2*/
            if(m_writeAddrListLevel1.empty())
            {
                m_nWriteLevel = 2;
                return getNextWriteAddr(mWriteAddr);
            }

            /*检查等级1容器合法并取出数据*/
            mWriteAddr = m_writeAddrListLevel1.front();
            m_writeAddrListLevel1.pop_front();
            break;
        }
    case 2:
        {
            /*等级1少于两次则 再次进入等级1*/
            if(m_nWLevel1UseTimes < 3)
            {
                m_nWriteLevel = 1;
            }
            else
            {
                m_nWriteLevel = 3;
            }

            /*等级2容器为空则跳到对应等级*/
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
            /*根据优先级来分配*/
            m_nWriteLevel = 1;

            m_nWLevel1UseTimes = 0;

            /*等级3容器为空则跳到对应等级1*/
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
 * Function: 添加地址到读写容器里面去
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
            /*如果包含则不添加，测试用*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel1, true)) break;

            m_writeAddrListLevel1.push_back(mAddr);
            break;
        }
    case SCENE_CONTROL_LOOP_W:
    case GLOBAL_LOOP_W:
        {
            /*如果包含则不添加，测试用*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel2, true)) break;

            m_writeAddrListLevel2.push_back(mAddr);
            break;
        }
    case MACRO_LOOP_W:
        {
            /*如果包含则不添加，测试用*/
            if(PlcMathMethod::getbContainWAddr(mAddr, m_writeAddrListLevel3, true)) break;

            m_writeAddrListLevel3.push_back(mAddr);
            break;
        }
    case SCENE_CONTROL_ONCE_R:
    case SCENE_CONTROL_LOOP_R:
        {
            /*如果包含则不添加，测试用*/
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
            /*如果包含则不添加，测试用*/
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
            /*如果包含则不添加，测试用*/
            if(PlcMathMethod::getbContainAddr(mAddr, m_readAddrListLevel3) > -1) break;

            m_readAddrListLevel3.push_back(mAddr);
            m_nMaxTimes += 6;
            break;
        }
    case ALARM_LOOP_R:
    case DATA_COLLECT_ONCE_R:
    case DATA_COLLECT_LOOP_R:
        {
            /*如果包含则不添加，测试用*/
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
 * Function: 对地址赋值
 * Parameters: address
 * Return: null
 ******************************************************************/
bool AddrManage::setAddrValue(ADDR_TARGET_PROP mAddr, vector<unsigned char> valueList)
{
    bool bSuccess = true;

    /*根据当前等级号和下标号查询地址*/
    switch(m_nOldReadLevel)
    {
    case 1:
        {
            /*判断地址下标是否非法*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel1.size())
            {
                /*取地址值*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel1.at(m_nOldReadIndex);

                /*如果满足地址相等，地址的值不等，则使用次数加1*/
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
            /*判断地址下标是否非法*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel2.size())
            {
                /*取地址值*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel2.at(m_nOldReadIndex);
                /*如果满足地址相等，地址的值不等，则使用次数加1*/
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
            /*判断地址下标是否非法*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel3.size())
            {
                /*取地址值*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel3.at(m_nOldReadIndex);
                /*如果满足地址相等，地址的值不等，则使用次数加1*/
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
            /*判断地址下标是否非法*/
            if(m_nOldReadIndex >=0 && m_nOldReadIndex < m_readAddrListLevel4.size())
            {
                /*取地址值*/
                ADDR_RW_STRUCT mCurrAddr = m_readAddrListLevel4.at(m_nOldReadIndex);
                /*如果满足地址相等，地址的值不等，则使用次数加1*/
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
 * Function: 根据读数据值的改变次数对地址优先级重新分配
 * Parameters: null
 * Return: null
 ******************************************************************/
void AddrManage::reCollateAddrpriority()
{
    /*读容器下标复位*/
    m_nReadLevel = 1;
    m_nReadIndex1 = 0;
    m_nReadIndex2 = 0;
    m_nReadIndex3 = 0;
    m_nReadIndex4 = 0;

    /*计算重置0*/
    m_nGetAddrTimes = 0;

    int nSize = 0;
    int nMaxUseTimes = 0;
    int nAvgTimes = 0;
    int nTmpTimes = 0;
    ADDR_RW_STRUCT mTmpAddr;

    /*临时缓存*/
    vector<ADDR_RW_STRUCT > tmpListLevel1;
    vector<ADDR_RW_STRUCT > tmpListLevel2;
    vector<ADDR_RW_STRUCT > tmpListLevel3;
    vector<ADDR_RW_STRUCT > tmpListLevel4;

    /*添加优先级1的有效读次数*/
    nSize = m_readAddrListLevel1.size();
    for(int i = 0; i < nSize; i++)
    {
        /*取最大值*/
        if(m_readAddrListLevel1.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*重新赋值*/
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
                    /*一次也没变化过，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*变化次数大于0，放到等级1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        default:
            {
                if(nTmpTimes >= nAvgTimes)
                {
                    /*变化次数少于平均值，放到等级1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                else
                {
                    /*一次也没变化过，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                break;
            }
        }
    }

    /*添加优先级2的有效读次数*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel2.size();
    for(int i = 0; i < nSize; i++)
    {
        /*取最大值*/
        if(m_readAddrListLevel2.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*重新赋值*/
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
                    /*一次也没变化过，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*变化次数大于0，放到等级1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        case GLOBAL_ONCE_R:
        case GLOBAL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*一次也没变化过，放到等级3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else if(nTmpTimes < nAvgTimes)
                {
                    /*变化次数少于平均值，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*变化次数大于平均值，放到等级1*/
                    tmpListLevel1.push_back(mTmpAddr);
                }
                break;
            }
        case MACRO_ONCE_R:
        case MACRO_LOOP_R:
            {
                if(nTmpTimes >= nAvgTimes)
                {
                    /*变化次数少于平均值，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                else
                {
                    /*一次也没变化过，放到等级3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                break;
            }
        case ALARM_LOOP_R:
        case DATA_COLLECT_ONCE_R:
        case DATA_COLLECT_LOOP_R:
            {
                /*一次也没变化过，放到等级3*/
                tmpListLevel3.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }

    /*添加优先级3的有效读次数*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel3.size();
    for(int i = 0; i < nSize; i++)
    {
        /*取最大值*/
        if(m_readAddrListLevel3.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*重新赋值*/
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
                /*一次也没变化过，放到等级2*/
                tmpListLevel2.push_back(mTmpAddr);
                break;
            }
        case GLOBAL_ONCE_R:
        case GLOBAL_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*一次也没变化过，放到等级3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*变化次数少于平均值，放到等级2*/
                    tmpListLevel2.push_back(mTmpAddr);
                }
                break;
            }
        case MACRO_ONCE_R:
        case MACRO_LOOP_R:
            {
                if(nTmpTimes <= 0)
                {
                    /*一次也没变化过，放到等级4*/
                    tmpListLevel4.push_back(mTmpAddr);
                }
                else if(nTmpTimes < nAvgTimes)
                {
                    /*变化次数少于平均值，放到等级3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*变化次数大于平均值，放到等级2*/
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
                    /*变化次数少于平均值，放到等级3*/
                    tmpListLevel3.push_back(mTmpAddr);
                }
                else
                {
                    /*一次也没变化过，放到等级4*/
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

    /*添加优先级4的有效读次数*/
    nMaxUseTimes = 0;
    nSize = m_readAddrListLevel4.size();
    for(int i = 0; i < nSize; i++)
    {
        /*取最大值*/
        if(m_readAddrListLevel4.at(i).nUseTimes > nMaxUseTimes)
        {
            nMaxUseTimes = nTmpTimes;
        }
    }

    /*重新赋值*/
    nAvgTimes = nMaxUseTimes/2;
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr = m_readAddrListLevel4.at(i);
        nTmpTimes = mTmpAddr.nUseTimes;
        mTmpAddr.nUseTimes = 0;

        if(nTmpTimes > 0)
        {
            /*变化次数少于平均值，放到等级3*/
            tmpListLevel3.push_back(mTmpAddr);
        }
        else
        {
            /*一次也没变化过，放到等级4*/
            tmpListLevel4.push_back(mTmpAddr);
        }
    }

    /*添加数值到原来容器中*/
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
 * Function: 清除写地址
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
 * Function: 清除读地址
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

/*切换画面，记录地址*/
void AddrManage::setAddrListBuf(vector<ADDR_TARGET_PROP> addrList)
{
    m_readAddrListbuf.clear();
    m_readAddrListbuf = addrList;
}

/*切换画面，地址*/
vector<ADDR_TARGET_PROP> AddrManage::getAddrListBuf()
{
    return m_readAddrListbuf;
}
