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
    case REG_TYPE_3P:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }

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
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    ushort temp1 = PlcMathMethod::asciiToIntValue(valueList.at(4*i));
                    ushort temp2 = PlcMathMethod::asciiToIntValue(valueList.at(4*i +1));
                    ushort temp3 = PlcMathMethod::asciiToIntValue(valueList.at(4*i +2));
                    ushort temp4 = PlcMathMethod::asciiToIntValue((valueList.at(4*i +3)));
                    nTmpValue = (temp1 << 12) + (temp2 << 8) + (temp3 << 4) + (temp4 << 0);

                    //LOGE("setReadData ...nTmpValue = %d.",nTmpValue);
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_2P:
    case REG_TYPE_7P:
    case REG_TYPE_4P:
    case REG_TYPE_6P:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDWordValue);
            }

            /*赋值*/
            uint nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                int nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uint nLowByte = valueList.at(4*i) & 0xff;
                    uint nHighByte = valueList.at(4*i + 1) & 0xff;
                    uint nLowByte1 = valueList.at(4*i + 2) & 0xff;
                    uint nHighByte1 = valueList.at(4*i + 3) & 0xff;

                    nTmpValue = (nHighByte1 << 24) + (nLowByte1 << 16) + (nHighByte << 8) + nLowByte;
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                int nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/8)
                {
                    nSetLen = nListSize/8;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 12 8 4 0*/
                    uint nChar1 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 0)) << 12) & 0xf000;
                    uint nChar2 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 1)) << 8) & 0x0f00;
                    uint nChar3 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 2)) << 4) & 0x00f0;
                    uint nChar4 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 3)) << 0) & 0x000f;

                    uint nChar5 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 4)) << 12) & 0xf000;
                    uint nChar6 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 5)) << 8) & 0x0f00;
                    uint nChar7 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 6)) << 4) & 0x00f0;
                    uint nChar8 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 7)) << 0) & 0x000f;

                    uint nHighValue = nChar1 + nChar2 + nChar3 + nChar4;
                    uint nLowValue = nChar5 + nChar6 + nChar7 + nChar8;
                    nTmpValue = (nHighValue << 16) + nLowValue;
//                    float nValue = nTmpValue/100.00;
//                    memcpy(&nTmpValue,&nValue,4);
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].nValue = nTmpValue;
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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        /* 3x存在readDataList.regWordList.at(0) */
        /* 4x存在readDataList.regWordList.at(1) */
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

    addrDoubleWordProp emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*把数据保存到对应的容器中*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3P:
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
    case REG_TYPE_2P:
    case REG_TYPE_7P:
    case REG_TYPE_4P:
    case REG_TYPE_6P:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDWordValue);
            }

            /*赋值*/
            unsigned int nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[eRegType].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regDoubleWordList[eRegType].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 16) & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 24) & 0xff;
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
