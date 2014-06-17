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
    if(nListSize > 1000) return false;

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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
        {
            int nSize = readDataList.regBitList[eRegType].size();

            if(bUserData)//如果是用户写的数据
            {
                int nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
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
            else//如果是从PLC读取的数据
            {
                /*赋值*/
                int nSetLen = mAddr.nAddrLen/16;
                if(mAddr.nAddrLen%16 != 0)
                {
                    nSetLen += 1;
                }

                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + nSetLen*16)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nSetLen*16, emptyBitValue);
                }

                int nStartAddr = (mAddr.nAddrValue/16)*16;
                for(int i = 0; i < nSetLen; i++)
                {
                    ushort nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0))) & 0x0f;
                    ushort nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1))) & 0x0f;
                    ushort nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2))) & 0x0f;
                    ushort nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3))) & 0x0f;
                    ushort nTmpValue = (nTmpValue1 << 12) + (nTmpValue2 << 8) + (nTmpValue3 << 4) + nTmpValue4;

                    for(int j = 0; j < 16; j++)
                    {
                        readDataList.regBitList[eRegType][nStartAddr + i*16 + j].eErrorStatus = eSuccess;
                        if((nTmpValue & (1 << j)) > 0)
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            int nSize = readDataList.regBitList[eRegType].size();

            if(bUserData)//如果是用户写的数据
            {
                int nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
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
            else//如果是从PLC读取的数据
            {
                /*赋值*/
                int nSetLen = mAddr.nAddrLen/16;
                if(mAddr.nAddrLen%16 != 0)
                {
                    nSetLen += 1;
                }

                if(nSetLen > nListSize/16)
                {
                    nSetLen = nListSize/16;
                }

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + nSetLen*16)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nSetLen*16, emptyBitValue);
                }

                int nStartAddr = (mAddr.nAddrValue/16)*16;
                for(int i = 0; i < nSetLen; i++)
                {
                    ushort nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 0))) & 0x0f;
                    ushort nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 1))) & 0x0f;
                    ushort nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 2))) & 0x0f;
                    ushort nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 3))) & 0x0f;
                    ushort nTmpValue5 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 4))) & 0x0f;
                    ushort nTmpValue6 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 5))) & 0x0f;
                    ushort nTmpValue7 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 6))) & 0x0f;
                    ushort nTmpValue8 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 7))) & 0x0f;
                    ushort nTmpValue9 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 8))) & 0x0f;
                    ushort nTmpValue10 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 9))) & 0x0f;
                    ushort nTmpValue11 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 10))) & 0x0f;
                    ushort nTmpValue12 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 11))) & 0x0f;
                    ushort nTmpValue13 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 12))) & 0x0f;
                    ushort nTmpValue14 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 13))) & 0x0f;
                    ushort nTmpValue15 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 14))) & 0x0f;
                    ushort nTmpValue16 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 15))) & 0x0f;
                    ushort nTmpValue = (nTmpValue1 << 0) + (nTmpValue2 << 1) + (nTmpValue3 << 2) + (nTmpValue4 << 3)
                                       + (nTmpValue5 << 4) + (nTmpValue6 << 5) + (nTmpValue7 << 6) + (nTmpValue8 << 7)
                                       + (nTmpValue9 << 8) + (nTmpValue10 << 9) + (nTmpValue11 << 10) + (nTmpValue12 << 11)
                                       + (nTmpValue13 << 12) + (nTmpValue14 << 13) + (nTmpValue15 << 14) + (nTmpValue16 << 15);

                    for(int j = 0; j < 16; j++)
                    {
                        readDataList.regBitList[eRegType][nStartAddr + i*16 + j].eErrorStatus = eSuccess;
                        if((nTmpValue & (1 << j)) > 0)
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                int nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else//如果是从PLC读取的数据
            {
                int nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 12 8 4 0*/
                    ushort tmp1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    ushort tmp2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    ushort tmp3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    ushort tmp4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;
                    nTmpValue = tmp1 + tmp2 + tmp3 + tmp4;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_DRC:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
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
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uint nLowByte = valueList.at(4*i) & 0xff;
                    uint nHighByte = valueList.at(4*i + 1) & 0xff;
                    uint nLowByte1 = valueList.at(4*i + 2) & 0xff;
                    uint nHighByte1 = valueList.at(4*i + 3) & 0xff;

                    nTmpValue = (nHighByte1 << 24) + (nLowByte1 << 16) + (nHighByte << 8) + nLowByte;
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
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
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

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
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
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
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_DRC:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            /*赋值*/
            uint nTmpValue = 0;
            int nGetLen = mAddr.nAddrLen/2;
            for(int i = 0; i < nGetLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[0].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regDoubleWordList[0].at(mAddr.nAddrValue + i).nValue;

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
