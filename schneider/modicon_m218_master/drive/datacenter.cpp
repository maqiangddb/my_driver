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

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_MX:
        {
            int nSize = readDataList.regWordList[0].size();
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[0].resize(nValueListSize, emptyWordValue);
            }

            if(bUserData)  //是用户设置的值
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*赋值*/
                int nStartPos = 0;
                int nBitPost = 0;
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetLen; i++)
                {
                    nStartPos = (mAddr.nAddrValue + i)/16;
                    nBitPost = (mAddr.nAddrValue + i)%16;

                    /*设置状态*/
                    readDataList.regWordList[0][nStartPos].eErrorStatus = eSuccess;

                    /*设置值*/
                    nTmpValue = readDataList.regWordList[0].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= 1 << nBitPost;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regWordList[0][nStartPos].nValue = nTmpValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*数据存储区的容器不够则重新分配空间*/
                nSize = readDataList.regWordList[0].size();
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/2;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nSetDataLen, emptyWordValue);
                }

                /*赋值*/
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[0][nStartAddr + i].eErrorStatus = eSuccess;

                    /*设置值*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[0][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_MW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
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
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
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
    case REG_TYPE_MX:
        {
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[0].resize(nValueListSize, emptyWordValue);
            }

            /*赋值*/
            int nStartPos = 0;
            int nBitPost = 0;
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                nStartPos = (mAddr.nAddrValue + i)/16;
                nBitPost = (mAddr.nAddrValue + i)%16;

                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(nStartPos).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[0].at(nStartPos).nValue;
                if(nTmpValue & (1 << nBitPost))
                {
                    mTmpValueProp.nValue = 1;
                }
                else
                {
                    mTmpValueProp.nValue = 0;
                }
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_MW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[0].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
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
