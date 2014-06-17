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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_SM:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_M:
        {
            int nSize = readDataList.regBitList[eRegType].size();
            if(bUserData)//如果是用户写的数据
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
            else//如果是从PLC读取的数据
            {
                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < mAddr.nAddrValue + nListSize * 8)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize * 8, emptyBitValue);
                }

                /*赋值*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < nListSize/2; i++)
                {
                    uchar nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(2*i + 0)) << 4) & 0xf0;
                    uchar nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(2*i + 1)) << 0) & 0x0f;
                    uchar nTmpValue = nTmpValue1 + nTmpValue2;

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
    case REG_TYPE_D_BIT:
        {
            int nSize = readDataList.regWordList[0].size();
            if(bUserData)//是用户设置的值
            {
                int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < nValueListSize)
                {
                    readDataList.regWordList[0].resize(nValueListSize, emptyWordValue);
                }

                /*长度不够,补0*/
                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
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
            else//如果是从PLC读取的数据
            {
                /*数据存储区的容器不够则重新分配空间*/
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/4;
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

                    /*设置值 4 0 12 8*/
                    ushort n4Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 4) & 0x00f0;
                    ushort n0Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 0) & 0x000f;
                    ushort n12Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 12) & 0xf000;
                    ushort n8Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 8) & 0x0f00;
                    nTmpValue = n12Bit + n8Bit + n4Bit + n0Bit;

                    readDataList.regWordList[0][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_R:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                if(mAddr.nAddrLen*2 != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen*2, 0);
                }

                for(int i = 0; i < mAddr.nAddrLen; i++)
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
            else//如果是从PLC读取的数据
            {
                for(int i = 0; i < nListSize/4; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 4 0 12 8*/
                    ushort n4Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 4) & 0x00f0;
                    ushort n0Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 0) & 0x000f;
                    ushort n12Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 12) & 0xf000;
                    ushort n8Bit = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 8) & 0x0f00;
                    nTmpValue = n12Bit + n8Bit + n4Bit + n0Bit;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_32CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            /*赋值*/
            int nTmpValue = 0;
            if(bUserData)  //是用户设置的值
            {
                if(mAddr.nAddrLen*2 != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen*2, 0);
                }

                for(int i = 0; i < mAddr.nAddrLen/2; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uint nTmp1 = valueList.at(4*i) & 0xff;
                    uint nTmp2 = valueList.at(4*i + 1) & 0xff;
                    uint nTmp3 = valueList.at(4*i + 2) & 0xff;
                    uint nTmp4 = valueList.at(4*i + 3) & 0xff;

                    nTmpValue = nTmp1 + (nTmp2 << 8) + (nTmp3 << 16) + (nTmp4 << 24);
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                for(int i = 0; i < nListSize/8; i++)
                {
                    /*设置状态*/
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 4 0 12 8*/
                    int nChar1 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 0)) << 4) & 0x000000f0;
                    int nChar2 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 1)) << 0) & 0x0000000f;
                    int nChar3 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 2)) << 12) & 0x0000f000;
                    int nChar4 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 3)) << 8) & 0x00000f00;

                    int nChar5 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 4)) << 20) & 0x00f00000;
                    int nChar6 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 5)) << 16) & 0x000f0000;
                    int nChar7 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 6)) << 28) & 0xf0000000;
                    int nChar8 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 7)) << 24) & 0x0f000000;

                    nTmpValue = nChar1 + nChar2 + nChar3 + nChar4 + nChar5 + nChar6 + nChar7 + nChar8;
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
    case REG_TYPE_SM:
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
    case REG_TYPE_D_BIT:
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
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_R:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
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
    case REG_TYPE_32CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            /*赋值*/
            unsigned int nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen/2; i++)
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
