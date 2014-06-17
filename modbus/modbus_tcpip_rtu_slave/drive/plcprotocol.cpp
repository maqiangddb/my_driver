#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*写数据返回的长度*/
#define WRITE_RETURN_LEN      8

/*读数据返回的数据头部长度*/
#define READ_HEAD_LEN         3

/*读数据返回的数据校验的长度*/
#define CHECK_LEN             2

PlcProtocol::PlcProtocol()
{
}

/******************************************************************
 * Function: change register value from string to int
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget)
{
    /*先初始化目标地址*/
    mAddrTarget.nUserPlcId = mAddrSource.nUserPlcId;
    mAddrTarget.eAddrRWprop = mAddrSource.eAddrRWprop;
    mAddrTarget.nAddrLen = mAddrSource.nAddrLen;
    mAddrTarget.nAddrValue = 0;
    mAddrTarget.nPlcStationIndex = mAddrSource.nPlcStationIndex;

    bool bSuccess = false;
    int nRegIndex = 0;
    bSuccess = getRegIndexFromName(mAddrSource.sRegName, nRegIndex);
    if(!bSuccess) return false;

    mAddrTarget.nRegIndex = nRegIndex;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            /*按"."拆分*/
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,".");
            if(strList.size() == 2)
            {
                string sIntStr = strList.at(0);
                string sDecStr = strList.at(1);

                /*高位部分的转换*/
                int nIntValue = DealWithString::stringToInt(sIntStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*低位部分的转换*/
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*小数部分不能超过16，也不能为负数*/
                if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
                {
                    return false;
                }

                /*整合后的结果*/
                mAddrTarget.nAddrValue = nIntValue*16 + nDecValue;

                /*判断长度是否超限*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536 * 16)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    default:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    }

    return false;
}

/******************************************************************
 * Function: get register name list
 * Parameters: null
 * Return: null
 ******************************************************************/
vector<string > PlcProtocol::getRegNameList(ADDR_REG_TYPE eAddrType)
{
    vector<string > sRegNameList;
    sRegNameList.clear();

    switch(eAddrType)
    {
    case BIT_ADDR_REG:
        {
            sRegNameList.push_back("LB");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
            break;
        }
    case STRING_ADDR_REG:
        {
            break;
        }
    case ALL_ADDR_REG:
    case READ_ADDR_REG:
    case WRITE_ADDR_REG:
        {
            sRegNameList.push_back("LB");
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
            break;
        }
    default:
        {
            break;
        }
    }

    return sRegNameList;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType)
{
    bool bSuccess = false;

    ADDR_TARGET_PROP mTargetAddr;
    bSuccess = this->addrStringToInt(mAddrProp, mTargetAddr);
    if(!bSuccess)
    {
        return ILLEGAL_VALUE;
    }

    int nRegIndex = 0;
    if(!getRegIndexFromName(mAddrProp.sRegName, nRegIndex)) return ILLEGAL_REG;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:                   //REG_TYPE_0X:
    case REG_TYPE_1X:                   //REG_TYPE_1X:
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_3X:                   //REG_TYPE_3X:
    case REG_TYPE_4X:                   //REG_TYPE_4X:
    case REG_TYPE_3X_DB:                   //REG_TYPE_3X_DB:
    case REG_TYPE_4X_DB:                   //REG_TYPE_4X_DB:
        {
            if(eAddrRegType != WORT_ADDR_REG)
            {
                return IS_WORD_ADDR;
            }
            break;
        }
    default:
        {
            return ILLEGAL_REG;
            break;
        }
    }

    return ADDR_CORRECT;
}

/******************************************************************
 * Function: 地址的多重复制
 * Parameters: address list
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP> &mSourceAddrList, int nInterval, int nCopyNum)
{
    /*初始化*/
    mSourceAddrList.clear();
    bool bSuccess = false;
    ADDR_CHECK_SOURCE_PROP mTmpAddr;
    mTmpAddr = mStartAddr;

    int nRegIndex = 0;
    if(!getRegIndexFromName(mTmpAddr.sRegName, nRegIndex)) return ILLEGAL_REG;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            /*按“.”拆分*/
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,".");
            if(strList.size() == 2)
            {
                string sIntStr = strList.at(0);
                string sDecStr = strList.at(1);

                /*高位部分的转换*/
                int nIntValue = DealWithString::stringToInt(sIntStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*低位部分的转换*/
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*小数部分不能超过16，也不能为负数*/
                if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nDecValue + nInterval > 15)
                    {
                        nIntValue += (nDecValue + nInterval)/16;
                        nDecValue = (nDecValue + nInterval)%16;
                    }
                    else
                    {
                        nDecValue += nInterval;
                    }

                    /*整合后的结果*/
                    int nStartValue = nIntValue*16 + nDecValue;

                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536 * 16)
                    {
                        /*添加整数部分*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr + ".";

                        /*添加小数部分*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nDecValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue += sTmpStr;

                        mSourceAddrList.push_back(mTmpAddr);
                    }
                    else
                    {
                        break;
                    }
                }
                return ADDR_CORRECT;
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    case REG_TYPE_0X:                   //REG_TYPE_0X:
    case REG_TYPE_1X:                   //REG_TYPE_1X:
    case REG_TYPE_3X:                   //REG_TYPE_3X:
    case REG_TYPE_4X:                   //REG_TYPE_4X:
    case REG_TYPE_3X_DB:                //REG_TYPE_3X_DB:
    case REG_TYPE_4X_DB:                //REG_TYPE_4X_DB:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_65536)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return ADDR_CORRECT;
}

/******************************************************************
 * Function: check recerve is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType)
{
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    if(nRcvSize < 12)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvHighByte = rcvList.at(4);
    uchar nRcvLowByte = rcvList.at(5);
    ushort nDataLen = (nRcvHighByte << 8) + nRcvLowByte;
    nDataLen += 6;

    if(nRcvSize == nDataLen)
    {
        /*设置数据*/
        mUnpackProp.bNeedSetData = true;
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else
    {
        eCmnInfo = DATA_LEN_ERROR;
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    return false;
}

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes: 次数从1开始
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    return CMN_NOMAL_CODE;
}

/******************************************************************
 * Function: 获得"从站函数，从串口接收到的数据到驱动处理，成功后返回打包好的发送数据" getSlaveDataStr 的句柄
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::getSlaveDataStr(vector<unsigned char> rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg)
{
    /*先清除返回值*/
    mSendPkg.sendDataList.clear();

    UNPACK_NEED_PROP mCheckProp;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;
    if(rcvList.empty())
    {
        eCmnInfo = NO_RCV_DATA;
        return eCmnInfo;
    }

    /*检查站号是否正确，站号也是PLC的地址值*/
    int nRcvSize = rcvList.size();

    /*长度不够则返回*/
    if(nRcvSize < 12)
    {
        eCmnInfo = checkRcvData(mCheckProp, rcvList, CHECK_READ);
        if(eCmnInfo == CMN_NOMAL_CODE)
        {
            eCmnInfo = CHECK_OK_UNRCV_ALL;
        }
        else
        {
            eCmnInfo = UNRCV_ALL_DATA;
        }
        return eCmnInfo;
    }

    /*站号不匹配，返回*/
    uchar nList6 = rcvList.at(6);
    if(nList6 !=0 && nList6 != nStationId)
    {
        eCmnInfo = STATION_ERROR;
        return eCmnInfo;
    }

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

    ushort nStartAddr = 0;
    ushort nAddrLen = 0;
    int nRegSize = 0;
    uchar nByteLen = 0x01;

    /*功能码*/
    uchar nFumNum = rcvList.at(7);

    /* index = 0, 事务处理标识符 –由服务器复制 –通常为 0*/
    mSendPkg.sendDataList.push_back(rcvList.at(0));

    /* index = 1, 事务处理标识符 –由服务器复制 –通常为 0*/
    mSendPkg.sendDataList.push_back(rcvList.at(1));

    /* index = 2, 协议标识符= 0*/
    mSendPkg.sendDataList.push_back(rcvList.at(2));

    /* index = 3, 协议标识符= 0*/
    mSendPkg.sendDataList.push_back(rcvList.at(3));

    /* index = 4, 长度字段 （上半部分字节） = 0 (所有的消息长度小于256)*/
    mSendPkg.sendDataList.push_back(0x00);

    /* index = 5, 长度字段  (下半部分字节)   = 后面字节的数量*/
    mSendPkg.sendDataList.push_back(0x06);

    /*站号 vector index = 6*/
    mSendPkg.sendDataList.push_back(nList6);

    /*对功能码赋值 vector index = 7*/
    mSendPkg.sendDataList.push_back(nFumNum);

    /*声明空的数据对象*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    /*校验错误，返回*/
    bool bDataCorrect = true;
    eCmnInfo = checkRcvData(mCheckProp, rcvList, CHECK_READ);
    if(eCmnInfo != CMN_NOMAL_CODE)
    {
        bDataCorrect = false;
        mSendPkg.sendDataList.push_back(eCmnInfo);
    }

    /*是否需要添加校验码*/
    if(bDataCorrect)
    {
        switch(nFumNum)
        {
        case 0x01:          //读REG_TYPE_0X数据
            {
                if(nRcvSize < 12)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*开始地址*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*读取长度*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*取数据*/
                nRegSize = readDataList.regBitList[nFumNum -1].size();
                if(nRegSize < nStartAddr + nAddrLen + 8)
                {
                    readDataList.regBitList[nFumNum -1].resize(nStartAddr + nAddrLen + 8, emptyBitValue);
                }

                /*对读取字节数初始化1,到时需要修改 index = 8*/
                mSendPkg.sendDataList.push_back(nByteLen);

                int pos = 0;
                uchar nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    if(pos >= 8)
                    {
                        pos = 0;
                        mSendPkg.sendDataList.push_back(nTmpValue);
                        nTmpValue = 0;
                    }

                    if(1 == readDataList.regBitList[nFumNum -1].at(nStartAddr + i).nValue)
                    {
                        nTmpValue |= 1 << pos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << pos);
                    }

                    pos++;
                }

                /*把剩下的位数补全*/
                int nIndex = 0;
                while(pos < 8)
                {
                    if(1 == readDataList.regBitList[nFumNum -1].at(nStartAddr + nAddrLen + nIndex).nValue)
                    {
                        nTmpValue |= 1 << pos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << pos);
                    }

                    pos++;
                    nIndex++;
                }
                mSendPkg.sendDataList.push_back(nTmpValue);

                /*对读取字节数修改*/
                nByteLen = mSendPkg.sendDataList.size() - 9;
                mSendPkg.sendDataList[8] = nByteLen;
                mSendPkg.sendDataList[4] = (nByteLen +3) >> 8 & 0xff;
                mSendPkg.sendDataList[5] = (nByteLen +3) & 0xff;
                break;
            }
        case 0x02:          //读REG_TYPE_1X数据
            {
                /*不支持02功能码*/
                mSendPkg.sendDataList.push_back(FUN_ERROR_CODE);
                eCmnInfo = FUN_ERROR_CODE;
                break;
            }
        case 0x03:          //读REG_TYPE_4X数据
            {
                if(nRcvSize < 12)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*开始地址*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*读取长度*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*取数据*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                /*对读取字节数初始化1,到时需要修改*/
                mSendPkg.sendDataList.push_back(nByteLen);

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = readDataList.regWordList[0].at(nStartAddr + i).nValue;

                    /*设置值*/
                    uchar nHighByte = (nTmpValue >> 8) & 0xff;
                    uchar nLowByte = nTmpValue & 0xff;

                    /*先添加高字节，后添加低字节*/
                    mSendPkg.sendDataList.push_back(nHighByte);
                    mSendPkg.sendDataList.push_back(nLowByte);
                }

                /*对读取字节数修改*/
                nByteLen = mSendPkg.sendDataList.size() - 9;
                mSendPkg.sendDataList[8] = nByteLen;
                mSendPkg.sendDataList[4] = (nByteLen +3) >> 8 & 0xff;
                mSendPkg.sendDataList[5] = (nByteLen +3) & 0xff;
                break;
            }
        case 0x04:          //读REG_TYPE_3X数据
            {
                if(nRcvSize < 12)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*开始地址 vector index = 2*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*读取长度*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*取数据*/
                nRegSize = readDataList.regWordList[1].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[1].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                /*对读取字节数初始化1,到时需要修改*/
                mSendPkg.sendDataList.push_back(nByteLen);

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = readDataList.regWordList[1].at(nStartAddr + i).nValue;

                    /*设置值*/
                    uchar nHighByte = (nTmpValue >> 8) & 0xff;
                    uchar nLowByte = nTmpValue & 0xff;

                    /*先添加高字节，后添加低字节*/
                    mSendPkg.sendDataList.push_back(nHighByte);
                    mSendPkg.sendDataList.push_back(nLowByte);
                }

                /*对读取字节数修改*/
                nByteLen = mSendPkg.sendDataList.size() - 9;
                mSendPkg.sendDataList[8] = nByteLen;
                mSendPkg.sendDataList[4] = (nByteLen +3) >> 8 & 0xff;
                mSendPkg.sendDataList[5] = (nByteLen +3) & 0xff;
                break;
            }
        case 0x05:          //写REG_TYPE_0X数据, 写单个数据
            {
                if(nRcvSize != 12)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = DATA_LEN_ERROR;
                    break;
                }

                /*开始地址 vector index = 2*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + 1 >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*第五个第六个是数据*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*取数据*/
                nRegSize = readDataList.regBitList[0].size();
                if(nRegSize < nStartAddr + 1)
                {
                    readDataList.regBitList[0].resize(nStartAddr + 1, emptyBitValue);
                }

                if(0xff00 == nAddrLen)
                {
                    readDataList.regBitList[0][nStartAddr].nValue = 1;
                }
                else if(0x0000 == nAddrLen)
                {
                    readDataList.regBitList[0][nStartAddr].nValue = 0;
                }
                else   //错误，返回错误码
                {
                    mSendPkg.sendDataList.push_back(DATA_VALUE_ERROR);
                    eCmnInfo = ADDR_LEN_ERROR;
                    break;
                }

                mSendPkg.sendDataList.clear();
                mSendPkg.sendDataList = rcvList;
                break;
            }
        case 0x0f:          //写REG_TYPE_0X数据, 写批量数据
            {
                if(nRcvSize < 14)   //错误，返回错误码
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*开始地址*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*设置长度*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*字节个数*/
                nByteLen = rcvList.at(12);

                /*这样写更安全*/
                nByteLen = rcvList.size() - 13;

                /*写数据*/
                nRegSize = readDataList.regBitList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regBitList[0].resize(nStartAddr + nAddrLen, emptyBitValue);
                }

                if(nByteLen*8 < nAddrLen)
                {
                    nAddrLen = nByteLen*8;
                }

                uchar nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = rcvList.at(13 + i/8);

                    if(nTmpValue & (1 << i%8))
                    {
                        readDataList.regBitList[0][nStartAddr + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[0][nStartAddr + i].nValue = 0;
                    }
                }

                /*添加返回数据*/
                mSendPkg.sendDataList.clear();
                for(int k = 0; k < 12; k++)
                {
                    mSendPkg.sendDataList.push_back(rcvList.at(k));
                }
                break;
            }
        case 0x06:          //写REG_TYPE_4X数据， 写单个数据
            {
                if(nRcvSize != 12)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = DATA_LEN_ERROR;
                    break;
                }

                /*开始地址 vector index = 2*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + 1 >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*第五个第六个是数据*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*写数据*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + 1)
                {
                    readDataList.regWordList[0].resize(nStartAddr + 1, emptyWordValue);
                }

                readDataList.regWordList[0][nStartAddr].nValue = nAddrLen;

                mSendPkg.sendDataList.clear();
                mSendPkg.sendDataList = rcvList;
                break;
            }
        case 0x10:          //写REG_TYPE_4X数据, 写批量数据
            {
                if(nRcvSize < 14)   //错误，返回错误码
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*开始地址 vector index = 2*/
                nStartAddr = rcvList.at(8);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(9);

                /*设置长度*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(10);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(11);

                /*检查地址的范围是否正确*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*字节个数*/
                nByteLen = rcvList.at(12);

                /*这样写更安全*/
                nByteLen = rcvList.size() - 13;

                /*取数据*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                if(nByteLen < nAddrLen *2)
                {
                    nAddrLen = nByteLen/2;
                }

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    /*设置值*/
                    uchar nHighByte = rcvList.at(13 + 2*i);
                    uchar nLowByte = rcvList.at(13 + 2*i + 1);

                    nTmpValue = nHighByte;
                    nTmpValue = (nTmpValue << 8) & 0xff00;
                    nTmpValue += nLowByte;

                    readDataList.regWordList[0][nStartAddr + i].nValue = nTmpValue;
                }

                /*添加返回数据*/
                mSendPkg.sendDataList.clear();
                for(int k = 0; k < 12; k++)
                {
                    mSendPkg.sendDataList.push_back(rcvList.at(k));
                }
                break;
            }
        default:
            {
                mSendPkg.sendDataList.push_back(FUN_ERROR_CODE);
                eCmnInfo = FUN_ERROR_CODE;
                break;
            }
        }
    }

    if(nList6 == 0)
    {
        mSendPkg.sendDataList.clear();
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: merge global address to m_globalReadPlcAddrList
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    targetList.clear();
    return true;
}

/******************************************************************
 * Function: 获得默认最大的读写长度
 * Parameters:
 * Return: 最大长度
 ******************************************************************/
int PlcProtocol::getMaxRwLen()
{
    int nMaxRWlen = 64;
    return nMaxRWlen;
}

/******************************************************************
 * Function: 设置当前地址
 * Parameters: null
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp)
{
    /*取寄存器的名称列表*/
    int nStartPos = -1;
    vector<string > regNameList = getRegNameList();
    int nSize = regNameList.size();

    /*一个个判断*/
    ADDR_CHECK_ERROR_INFO eErrorInfo = ADDR_CORRECT;
    bool bSuccess = false;
    string sRegName ;
    vector<string > compareNameList;
    vector<int > maxIndexList ;
    vector<int > startPosList;
    compareNameList.clear();
    maxIndexList.clear();
    startPosList.clear();
    for(int i = 0; i < nSize; i++)
    {
        sRegName = regNameList.at(i);

        /*包含这个寄存器名字*/
        bSuccess = DealWithString::getIsContainSubstr(sAddrStr, sRegName, nStartPos);
        if(bSuccess)
        {
            compareNameList.push_back(sRegName);
            maxIndexList.push_back(i);
            startPosList.push_back(nStartPos);
        }
    }

    int nCompareSize = compareNameList.size();
    if(nCompareSize < 1)
    {
        return ILLEGAL_REG;
    }

    /*取第一个与剩下的比较*/
    int nIndex = maxIndexList.at(0);
    sRegName = compareNameList.at(0);
    nStartPos = startPosList.at(0);
    for(int i = 1; i < nCompareSize; i++)
    {
        if(sRegName.length() < compareNameList.at(i).length())
        {
            nIndex = maxIndexList.at(i);
            sRegName = compareNameList.at(i);
            nStartPos = startPosList.at(i);
        }
        else if(nStartPos > startPosList.at(i))
        {
            nIndex = maxIndexList.at(i);
            sRegName = compareNameList.at(i);
            nStartPos = startPosList.at(i);
        }
    }

    /*创建校验结构体*/

    /*取寄存器名字和地址值的长度*/
    int nNameLen = sRegName.length();
    int nValueLen = sAddrStr.length() - (nStartPos + nNameLen);
    if(nValueLen > 0)
    {
        /*取地址的值*/
        sAddrProp.sRegName = sRegName;
        sAddrProp.nAddrLen = 1;
        sAddrProp.sAddrValue = sAddrStr.substr(nStartPos + nNameLen, nValueLen);
        eErrorInfo = checkAddrCorrect(sAddrProp, eAddrRegType);

        return eErrorInfo;
    }
    else
    {
        return ILLEGAL_VALUE;
    }
}

/******************************************************************
 * Function: 获得当前PLC的类型
 * Parameters: null
 * Return: null
 ******************************************************************/
PLC_FUN_TYPE PlcProtocol::getPlcType()
{
    return PLC_SLAVE;
}

/******************************************************************
 * Function: 获得默认站号
 * Parameters: null
 * Return: 默认站号
 ******************************************************************/
int PlcProtocol::getDefaultStation()
{
    int nStation = 0;
    return nStation;
}

/******************************************************************
 * Function: 获得默认本协议的默认串口信息
 * Parameters: null
 * Return: SERIAL_PORT_PROP结构体，串口信息
 ******************************************************************/
SERIAL_PORT_PROP PlcProtocol::getDefaultSerialInfo()
{
    SERIAL_PORT_PROP mSerialInfo;
    mSerialInfo.BAUD_RATE = 9600;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 1;
    mSerialInfo.STOP_BIT = 1;

    return mSerialInfo;
}

/******************************************************************
 * Function: 获得默认超时时间
 * Parameters: null
 * Return: 默认超时时间,单位：ms
 ******************************************************************/
int PlcProtocol::getDefaultTimeout()
{
    int nTimeout = 1000;
    return nTimeout;
}

/******************************************************************
 * Function: 获得默认最小采集周期
 * Parameters: null
 * Return: 默认最小采集周期,单位：毫秒
 ******************************************************************/
int PlcProtocol::getDefaultCollectRate()
{
    int nCollectRate = 1;
    return nCollectRate;
}

/******************************************************************
 * Function: 获得帧间隔时间
 * Parameters: null
 * Return: 获得帧间隔时间,单位：毫秒
 ******************************************************************/
int PlcProtocol::getDefaultIntervalTime()
{
    int nIntervalTime = 10;
    return nIntervalTime;
}

/******************************************************************
 * Function: 获得地址是否可读
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    /*只有以下四种寄存器支持写*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536 * 16)
            {
                return false;
            }
            break;
        }
    default:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            break;
        }
    }
    return true;
}

/******************************************************************
 * Function: 获得地址是否可写
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    /*只有以下四种寄存器支持写*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536 * 16)
            {
                return false;
            }
            break;
        }
    default:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            break;
        }
    }

    return true;
}


/*************************************************************************
 * Funciton: int 类型转换成寄存器的枚举类型
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
REG_TYPE_INDEX PlcProtocol::getRegTypeFromInt(int nRegIndex)
{
    REG_TYPE_INDEX eRegType = REG_TYPE_NULL;
    switch(nRegIndex)
    {
    case 0:
        {
            eRegType = REG_TYPE_0X;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_1X;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_3X_BIT;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_4X_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_3X;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_4X;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_3X_DB;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_4X_DB;
            break;
        }
    default:
        {
            break;
        }
    }

    return eRegType;
}

/*************************************************************************
 * Funciton: 通过名字获得寄存器的下标号
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
bool PlcProtocol::getRegIndexFromName(string sRegName, int &nRegIndex)
{
    if(sRegName.empty()) return false;

    if(DealWithString::compare("LB", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("RWI", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("LW", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else
    {
        return false;
    }
}

/*************************************************************************
 * Funciton: 获取协议的连接类型
 * Parameters:
 * Return: 协议类型
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return NET_PORT;
}

