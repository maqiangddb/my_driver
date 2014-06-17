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
    /* 初始化结束 */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*长度不对则返回*/
    if(mAddr.nAddrLen < 1) return false;

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

    /*设置值的长度*/
    int nSetDataLen = 0;
    uchar nTmpUCValue = 0;
    ushort nTmpUSValue = 0;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_SM:
    case REG_TYPE_VB_BIT:
        {
            int nSize = readDataList.regBitList.at(eRegType).size();
            if(bUserData)       //如果是用户写的数据
            {
                int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/8 +1;

                /*数据存储区的容器不够则重新分配空间*/
                if(nSize < nValueListSize)
                {
                    readDataList.regBitList[eRegType].resize(nValueListSize, emptyBitValue);
                }

                /*设置长度*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen > nListSize)
                {
                    nSetDataLen = nListSize;
                }

                /*赋值*/
                int nStartPos = 0;
                int nBitPost = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    nStartPos = (mAddr.nAddrValue + i)/8;
                    nBitPost = (mAddr.nAddrValue + i)%8;

                    /*设置状态*/
                    readDataList.regBitList[eRegType][nStartPos].eErrorStatus = eSuccess;

                    /*设置值*/
                    nTmpUCValue = readDataList.regBitList[eRegType].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpUCValue |= (1 << nBitPost);
                    }
                    else
                    {
                        nTmpUCValue &= ~(1 << nBitPost);
                    }
                    readDataList.regBitList[eRegType][nStartPos].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*设置长度*/
                nSetDataLen = nListSize;
                if((nSetDataLen -1)*8 > mAddr.nAddrLen)
                {
                    nSetDataLen = mAddr.nAddrLen/8 +1;
                }

                /*数据存储区的容器不够则重新分配空间*/
                int nStartAddr = mAddr.nAddrValue/8;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regBitList[eRegType].resize(nStartAddr + nSetDataLen, emptyBitValue);
                }

                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    nTmpUCValue = valueList.at(i);
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
    case REG_TYPE_SMW:
        {
            int nSize = readDataList.regBitList.at(eRegType -6).size();
            int nValueListSize = mAddr.nAddrValue + mAddr.nAddrLen*2;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regBitList[eRegType -6].resize(nValueListSize, emptyBitValue);
            }

            /*设置长度*/
            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            int nStartAddr = mAddr.nAddrValue;
            if(bUserData)       //如果是用户写的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i +1].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i + 1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i + 1);
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i +1);
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i +1].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i +1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -6][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_VB:
        {
            int nSize = readDataList.regBitList.at(5).size();
            int nValueListSize = mAddr.nAddrValue + mAddr.nAddrLen*2;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regBitList[5].resize(nValueListSize, emptyBitValue);
            }

            /*设置长度*/
            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            int nStartAddr = mAddr.nAddrValue;
            if(bUserData)       //如果是用户写的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i + 1);
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_VW:
        {
            int nSize = readDataList.regBitList.at(5).size();
            int nValueListSize = mAddr.nAddrValue + mAddr.nAddrLen*2;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regBitList[5].resize(nValueListSize, emptyBitValue);
            }

            /*设置长度*/
            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            int nStartAddr = mAddr.nAddrValue;
            if(bUserData)       //如果是用户写的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i + 1);
                    readDataList.regBitList[5][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i + 1);
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i + 1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_VD:
        {
            int nSize = readDataList.regBitList.at(5).size();
            int nValueListSize = mAddr.nAddrValue + mAddr.nAddrLen*2;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regBitList[5].resize(nValueListSize, emptyBitValue);
            }

            /*设置长度*/
            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            if(bUserData)       //如果是用户写的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen/2; i++)
                {
                    /*设置状态  index = 3*/
                    nTmpUCValue = valueList.at(4*i);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 3].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 3].nValue = nTmpUCValue;

                    /*设置状态  index = 2*/
                    nTmpUCValue = valueList.at(4*i + 1);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 2].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 2].nValue = nTmpUCValue;

                    /*设置状态  index = 1*/
                    nTmpUCValue = valueList.at(4*i + 2);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 1].nValue = nTmpUCValue;

                    /*设置状态  index = 0*/
                    nTmpUCValue = valueList.at(4*i + 3);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i].nValue = nTmpUCValue;
                }

                /*设置剩下不足4个字节的值*/
                for(int i = (nSetDataLen/2)*2; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i +1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i +1);
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen/2; i++)
                {
                    /*设置状态  index = 3*/
                    nTmpUCValue = valueList.at(4*i + 3);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 3].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 3].nValue = nTmpUCValue;

                    /*设置状态  index = 2*/
                    nTmpUCValue = valueList.at(4*i + 2);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 2].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 2].nValue = nTmpUCValue;

                    /*设置状态  index = 1*/
                    nTmpUCValue = valueList.at(4*i + 1);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i + 1].nValue = nTmpUCValue;

                    /*设置状态  index = 0*/
                    nTmpUCValue = valueList.at(4*i + 0);
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 4*i].nValue = nTmpUCValue;
                }

                /*设置剩下不足4个字节的值*/
                for(int i = (nSetDataLen/2)*2; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i +1);
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i +1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[5][mAddr.nAddrValue + 2*i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_AIW:
    case REG_TYPE_AQW:
        {
            int nSize = readDataList.regBitList.at(eRegType -10).size();
            int nValueListSize = mAddr.nAddrValue + mAddr.nAddrLen*2;

            /*数据存储区的容器不够则重新分配空间*/
            if(nSize < nValueListSize)
            {
                readDataList.regBitList[eRegType -10].resize(nValueListSize, emptyBitValue);
            }

            /*设置长度*/
            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            int nStartAddr = mAddr.nAddrValue;
            if(bUserData)       //如果是用户写的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i + 1].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i + 1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i +1);
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            else       //如果是从PLC读取的数据
            {
                /*赋值*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i +1);
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i +1].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i +1].nValue = nTmpUCValue;

                    /*设置值*/
                    nTmpUCValue = valueList.at(2*i);
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType -10][nStartAddr + 2*i].nValue = nTmpUCValue;
                }
            }
            break;
        }
    case REG_TYPE_TV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 14].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 14].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            if(bUserData)  //是用户设置的值
            {
                /*设置长度*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen > nListSize/2)
                {
                    nSetDataLen = nListSize/2;
                }

                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpUSValue = nHighByte & 0xff;
                    nTmpUSValue = (nTmpUSValue << 8);
                    nTmpUSValue += nLowByte;
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].nValue = nTmpUSValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                /*设置长度*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen*5 > nListSize)
                {
                    nSetDataLen = nListSize/5;
                }

                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    uchar nHighByte = valueList.at(5*i + 3);
                    uchar nLowByte = valueList.at(5*i + 4);
                    nTmpUSValue = nHighByte & 0xff;
                    nTmpUSValue = nTmpUSValue << 8;
                    nTmpUSValue = nTmpUSValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].nValue = nTmpUSValue;
                }
            }
            break;
        }
    case REG_TYPE_CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 14].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 14].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            if(bUserData)  //是用户设置的值
            {
                /*设置长度*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen > nListSize/2)
                {
                    nSetDataLen = nListSize/2;
                }

                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值,低字节在前 高字节在后*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpUSValue = nHighByte & 0xff;
                    nTmpUSValue = (nTmpUSValue << 8);
                    nTmpUSValue += nLowByte;
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].nValue = nTmpUSValue;
                }
            }
            else      //如果是从PLC读取的数据
            {
                /*设置长度*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen*3 > nListSize)
                {
                    nSetDataLen = nListSize/3;
                }

                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*设置状态*/
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*设置值 高字节在前 低字节在后*/
                    uchar nHighByte = valueList.at(3*i + 1);
                    uchar nLowByte = valueList.at(3*i + 2);
                    nTmpUSValue = nHighByte & 0xff;
                    nTmpUSValue = nTmpUSValue << 8;
                    nTmpUSValue = nTmpUSValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType - 14][mAddr.nAddrValue + i].nValue = nTmpUSValue;
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

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*把数据保存到对应的容器中*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_SM:
    case REG_TYPE_VB_BIT:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType].size();
            if(nSize < (mAddr.nAddrValue + mAddr.nAddrLen)/8 + 1)
            {
                readDataList.regBitList[eRegType].resize((mAddr.nAddrValue + mAddr.nAddrLen)/8 + 1, emptyBitValue);
            }

            /*赋值*/
            int nStartPos = 0;
            int nBitPost = 0;
            uchar nTmpUCValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                nStartPos = (mAddr.nAddrValue + i)/8;
                nBitPost = (mAddr.nAddrValue + i)%8;

                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regBitList[eRegType].at(nStartPos).eErrorStatus;

                /*设置值*/
                nTmpUCValue = readDataList.regBitList[eRegType].at(nStartPos).nValue;
                if(nTmpUCValue & (1 << nBitPost))
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
    case REG_TYPE_SMW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType -6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen*2)
            {
                readDataList.regBitList[eRegType -6].resize(mAddr.nAddrValue + mAddr.nAddrLen*2, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[eRegType -6].at(mAddr.nAddrValue + 2*i + 1);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp = readDataList.regBitList[eRegType -6].at(mAddr.nAddrValue + 2*i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_VW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen*2)
            {
                readDataList.regBitList[5].resize(mAddr.nAddrValue + mAddr.nAddrLen*2, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 2*i + 1);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 2*i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_AIW:
    case REG_TYPE_AQW:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[eRegType -10].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen*2)
            {
                readDataList.regBitList[eRegType -10].resize(mAddr.nAddrValue + mAddr.nAddrLen*2, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[eRegType -10].at(mAddr.nAddrValue + 2*i + 1);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp = readDataList.regBitList[eRegType -10].at(mAddr.nAddrValue + 2*i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_VB:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp.nValue = 0x00;
                nResultList.push_back(mTmpValueProp);
            }
            break ;
        }
    case REG_TYPE_VD:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regBitList[5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen*2)
            {
                readDataList.regBitList[5].resize(mAddr.nAddrValue + mAddr.nAddrLen*2, emptyBitValue);
            }

            /*赋值*/
            for(int i = 0; i < mAddr.nAddrLen/2; i++)
            {
                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 4*i + 3);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 4*i + 2);
                nResultList.push_back(mTmpValueProp);

                /*低字节在前*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 4*i + 1);
                nResultList.push_back(mTmpValueProp);

                /*高字节在后*/
                mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 4*i + 0);
                nResultList.push_back(mTmpValueProp);
            }

            if(mAddr.nAddrLen%2 != 0)
            {
                /*赋值*/
                for(int i = (mAddr.nAddrLen/2)*2; i < mAddr.nAddrLen; i++)
                {
                    /*低字节在前*/
                    mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 2*i + 1);
                    nResultList.push_back(mTmpValueProp);

                    /*高字节在后*/
                    mTmpValueProp = readDataList.regBitList[5].at(mAddr.nAddrValue + 2*i);
                    nResultList.push_back(mTmpValueProp);
                }
            }
            break ;
        }
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*数据存储区的容器不够则重新分配空间*/
            int nSize = readDataList.regWordList[eRegType - 14].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 14].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*赋值*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*设置状态*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 14].at(mAddr.nAddrValue + i).eErrorStatus;

                /*设置值*/
                nTmpValue = readDataList.regWordList[eRegType - 14].at(mAddr.nAddrValue + i).nValue;

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
