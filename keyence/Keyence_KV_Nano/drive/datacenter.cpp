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
        /* REG_TYPE_RLY存在readDataList.regBitList.at(0) */
        /* REG_TYPE_T存在readDataList.regBitList.at(1) */
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        /* REG_TYPE_DM存在readDataList.regWordList.at(0) */
        /* REG_TYPE_TM存在readDataList.regWordList.at(1) */
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* 初始化结束 */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*写的数据为空则返回*/
    int nListSize = valueList.size();
    //if(nListSize > 200) return false;
    //    if(nListSize <= 0) return ;

    /*把数据保存到对应的容器中*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType].size();
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
                nSize = readDataList.regBitList[eRegType].size();
                if(nSize < mAddr.nAddrValue + nListSize)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize, emptyBitValue);
                }

                /*赋值*/
                int nStartAddr = mAddr.nAddrValue;
                for(int i = 0; i < nListSize; i++)
                {
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    uchar nTmpValue = valueList.at(i);
                    nTmpValue = PlcMathMethod::asciiToIntValue(nTmpValue);
                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/4)
            {
                nSetLen = nListSize/4;
            }

            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值*/
                    unsigned short usTemp1 = valueList.at(i*4 + 0);
                    unsigned short usTemp2 = valueList.at(i*4 + 1);
                    unsigned short usTemp3 = valueList.at(i*4 + 2);
                    unsigned short usTemp4 = valueList.at(i*4 + 3);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值*/
                    unsigned short usTemp1 = valueList.at(i*11 + 2);
                    unsigned short usTemp2 = valueList.at(i*11 + 3);
                    unsigned short usTemp3 = valueList.at(i*11 + 4);
                    unsigned short usTemp4 = valueList.at(i*11 + 5);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CTC:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值*/
                    unsigned short usTemp1 = valueList.at(i*11 + 7);
                    unsigned short usTemp2 = valueList.at(i*11 + 8);
                    unsigned short usTemp3 = valueList.at(i*11 + 9);
                    unsigned short usTemp4 = valueList.at(i*11 + 10);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_C:
    case REG_TYPE_T:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType].size();
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
                nSize = readDataList.regBitList[eRegType].size();
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }

                if(nSize < mAddr.nAddrValue + nSetLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize, emptyBitValue);
                }

                int nStartAddr = mAddr.nAddrValue;
                for(int i = 0; i < nSetLen; i++)
                {
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    uchar nTmpValue = valueList.at(i*11 + 0);
                    nTmpValue = PlcMathMethod::asciiToIntValue(nTmpValue);

                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpValue;
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
        /* 0x存在readDataList.regBitList.at(0) */
        /* 1x存在readDataList.regBitList.at(1) */
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        /* 3x存在readDataList.regWordList.at(0) */
        /* 4x存在readDataList.regWordList.at(1) */
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* 初始化结束 */

    nResultList.clear();
    //if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return nResultList;

    /*容器不够时，添加这个对象*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*把数据保存到对应的容器中*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
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
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
    case REG_TYPE_CTH:
    case REG_TYPE_CTC:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 8].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[eRegType - 8].at(mAddr.nAddrValue + i).nValue;

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
