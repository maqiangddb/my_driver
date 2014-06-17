#include "datacenter.h"
#include "plcprotocol.h"

DataCenter::DataCenter()
{
}

/******************************************************************
* Function: 设置值到plc容器中
* Parameters: address list
* bUserData 为true表示从应用程序取得的数据，需要高地位转换，为false，表示从协议读取到的数据，
* Return: null
******************************************************************/
bool DataCenter::setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess)
{
    /* 如果寄存器没添加，则初始化寄存器数据存储 */
    while(readDataList.regBitList.size() < MAX_BIT_REG_NUN)
    {
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        vector<ADDR_DOUBLE_WORD_PROP > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* 初始化结束 */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*长度不对则返回*/
    if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return false;

    /*写的数据为空则返回*/
    int nListSize = valueList.size();
    if(nListSize > 200) return false;
    //    if(nListSize <= 0) return ;

    /*把数据保存到对应的容器中*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList.at(eRegType).size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            if(bUserData)       //如果是用户写的数据
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*赋值*/
                for(int i = 0; i < nSetLen; i++)
                {
                    readDataList.regBitList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*数据存储区的容器不够则重新分配空间*/
                nSize = readDataList.regBitList.at(eRegType).size();
                if(nSize < mAddr.nAddrValue + nListSize *8)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize *8, emptyBitValue);
                }

                /*赋值*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < nListSize; i++)
                {
                    uchar nTmpValue = valueList.at(i);
                    for(int j = 0; j < 8; j++)
                    {
                        readDataList.regBitList[eRegType][nStartAddr + i*8 + j].eErrorStatus = eSuccess;
                        if(nTmpValue & (1 << j))
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*8 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*8 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType -5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/2)
            {
                nSetLen = nListSize/2;
            }

            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            if(mAddr.nAddrValue < 200)
            {
                int nSize = readDataList.regWordList[eRegType -5].size();
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
                }

                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }

                /*赋值*/
                ushort nTmpValue = 0;
                if(bUserData)  //是用户设置的值
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*设置状态*/
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                        /*设置值,低字节在前 高字节在后*/
                        uchar nLowByte = valueList.at(2*i);
                        uchar nHighByte = valueList.at(2*i +1);
                        nTmpValue = nHighByte & 0xff;
                        nTmpValue = (nTmpValue << 8);
                        nTmpValue += nLowByte;
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                    }
                }
                else      //如果是从PLC读取的数据
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*设置状态*/
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                        /*设置值 高字节在前 低字节在后*/
                        uchar nHighByte = valueList.at(2*i);
                        uchar nLowByte = valueList.at(2*i +1);
                        nTmpValue = nHighByte & 0xff;
                        nTmpValue = nTmpValue << 8;
                        nTmpValue = nTmpValue + (nLowByte & 0xff);
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                    }
                }
            }
            else
            {
                int nSize = readDataList.regDoubleWordList[0].size();
                int nAddrValue = mAddr.nAddrValue - 200;
                if(nSize < nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regDoubleWordList[0].resize(nAddrValue + mAddr.nAddrLen, emptyDWordValue);
                }

                nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                /*赋值*/
                uint nTmpValue = 0;
                if(bUserData)  //是用户设置的值
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*设置状态*/
                        readDataList.regDoubleWordList[0][nAddrValue + i].eErrorStatus = eSuccess;

                        /*设置值,低字节在前 高字节在后*/
                        uint nLowByte = valueList.at(4*i );
                        uint nHighByte = valueList.at(4*i +1);
                        uint nLowByte1 = valueList.at(4*i +2);
                        uint nHighByte1 = valueList.at(4*i +3);
                        nTmpValue = (nHighByte1 << 24) + (nLowByte1 << 16) + (nHighByte << 8) + nLowByte;
                        readDataList.regDoubleWordList[0][nAddrValue + i].nValue = nTmpValue;
                    }
                }
                else      //如果是从PLC读取的数据
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*设置状态*/
                        readDataList.regDoubleWordList[0][nAddrValue + i].eErrorStatus = eSuccess;

                        /*设置值 高字节在前 低字节在后*/
                        uint nLowByte = valueList.at(4*i +3);
                        uint nHighByte = valueList.at(4*i +2);
                        uint nLowByte1 = valueList.at(4*i +1);
                        uint nHighByte1 = valueList.at(4*i +0);
                        nTmpValue = (nHighByte1 << 8) + (nLowByte1 << 0) + (nHighByte << 24) + (nLowByte << 16);
                        readDataList.regDoubleWordList[0][nAddrValue + i].nValue = nTmpValue;
                    }
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }//end switch

    return true;
}

/******************************************************************
* Function: 获得包含站号的STATION_VALUE_PROP在stationList容器中的下标
* Parameters: stationList，nStationId
* Return: null
******************************************************************/
bool DataCenter::getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList)
{
    /* 如果寄存器没添加，则初始化寄存器数据存储 */
    while(readDataList.regBitList.size() < MAX_BIT_REG_NUN)
    {
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        vector<ADDR_DOUBLE_WORD_PROP > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* 初始化结束 */

    nResultList.clear();
    if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return false;

    /*容器不够时，添加这个对象*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*把数据保存到对应的容器中*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[eRegType].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType -5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 200)
            {
                /*数据存储区的容器不够则重新分配空间*/
                int nSize = readDataList.regWordList[eRegType -5].size();
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
                }

                /*赋值*/
                ushort nTmpValue = 0;
                for(int i = 0; i <  mAddr.nAddrLen; i++)
                {
                    /*设置状态*/
                    mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).eErrorStatus;

                    /*设置值*/
                    nTmpValue = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).nValue;

                    mTmpValueProp.nValue = nTmpValue & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                    nResultList.push_back(mTmpValueProp);
                }
            }
            else
            {
                /*数据存储区的容器不够则重新分配空间*/
                int nSize = readDataList.regDoubleWordList[0].size();
                int nAddrValue = mAddr.nAddrValue - 200;
                if(nSize < nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regDoubleWordList[0].resize(nAddrValue + mAddr.nAddrLen, emptyDWordValue);
                }

                /*赋值*/
                uint nTmpValue = 0;
                for(int i = 0; i <  mAddr.nAddrLen; i++)
                {
                    /*设置状态*/
                    mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[0].at(nAddrValue + i).eErrorStatus;

                    /*设置值*/
                    nTmpValue = readDataList.regDoubleWordList[0].at(nAddrValue + i).nValue;

                    mTmpValueProp.nValue = nTmpValue & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 16) & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 24) & 0xff;
                    nResultList.push_back(mTmpValueProp);
                }
            }
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    return true;
}

#if 0
/******************************************************************
* Function: 获得包含站号的STATION_VALUE_PROP在stationList容器中的下标
* Parameters: stationList，nStationId
* Return: null
******************************************************************/
int DataCenter::getVectorIndex(vector<STATION_VALUE_PROP> stationList, int nStationId)
{
    int index = -1;
    if(stationList.empty()) return index;

    int nSize = stationList.size();
    STATION_VALUE_PROP mTmpValue;
    for(int i = 0; i < nSize; i++)
    {
        mTmpValue = stationList.at(i);
        if(mTmpValue.nStationId == nStationId)
        {
            index = i;
            return index;
        }
    }

    return index;
}
#endif
