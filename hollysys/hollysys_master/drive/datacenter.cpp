#include "datacenter.h"
#include "plcprotocol.h"
#include "../tools/plcmathmethod.h"

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

    /*写的数据为空则返回*/
    int nListSize = valueList.size();
    if(nListSize > 200) return false;

    /*把数据保存到对应的容器中*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDoubleWordValue;
    emptyDoubleWordValue.nValue = 0;
    emptyDoubleWordValue.eErrorStatus = READ_DATA_FAIL;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_IX:
    case REG_TYPE_QX:
    case REG_TYPE_MX:
        {
            int nSize = readDataList.regBitList[eRegType].size();
            if(bUserData)       //如果是用户写的数据
            {
                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
                }

                /*赋值*/
                for(int i = 0; i < mAddr.nAddrLen; i++)
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[eRegType - 3].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType - 3].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            if(mAddr.nAddrLen*2 != nListSize)
            {
                valueList.resize(mAddr.nAddrLen*2, 0);
            }

            /*赋值*/
            unsigned int nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[eRegType - 3][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    if(2*i +3 < valueList.size())
                    {
                        nLowByte = valueList.at(2*i + 2);
                        nHighByte = valueList.at(2*i +3);
                        unsigned int temp = 0;
                        temp = nLowByte & 0xff;
                        temp = (temp << 16);

                        nTmpValue = nTmpValue | temp;

                        temp = nHighByte & 0xff;
                        temp = (temp << 24);
                        nTmpValue = nTmpValue | temp;
                    }
                    readDataList.regDoubleWordList[eRegType - 3][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[eRegType - 3][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    uchar nHighByte = valueList.at(i);
                    uchar nLowByte = valueList.at(i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);

                    if(i + 3 < valueList.size())
                    {
                        unsigned int temp = 0;
                        nHighByte = valueList.at(i + 2);
                        nLowByte = valueList.at(i + 3);

                        temp = nHighByte & 0xff;
                        temp = temp << 8;
                        temp = temp + (nLowByte & 0xff);
                        temp = temp << 16;

                        nTmpValue = nTmpValue | temp;
                    }
                    readDataList.regDoubleWordList[eRegType - 3][mAddr.nAddrValue + i].nValue = nTmpValue;
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

    /*容器不够时，添加这个对象*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDoubleWordValue;
    emptyDoubleWordValue.nValue = 0;
    emptyDoubleWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*把数据保存到对应的容器中*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_IX:
    case REG_TYPE_QX:
    case REG_TYPE_MX:
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[eRegType - 3].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType - 3].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            /*赋值*/
            unsigned int nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[eRegType - 3].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regDoubleWordList[eRegType - 3].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                if(mAddr.nAddrLen > 1)
                {
                   ushort temp = (nTmpValue >> 16) & 0xffff;
                   mTmpValueProp.nValue = temp & 0xff;
                   nResultList.push_back(mTmpValueProp);

                   mTmpValueProp.nValue = (temp >> 8) & 0xff;
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
