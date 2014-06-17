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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
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
                if(nIntValue < 0 || nIntValue >= ADDR_MAX_VALUE_4096 || nDecValue < 0 || nDecValue >= 8)
                {
                    return false;
                }

                /*整合后的结果*/
                mAddrTarget.nAddrValue = nIntValue*8 + nDecValue;

                /*判断长度是否超限*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096 * 8)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_DB_BIT:
        {
            /*按"."拆分*/
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                strList = DealWithString::split(sElsePass, ".");

                if(strList.size() == 2)
                {
                    string sWordStr = strList.at(0);
                    string sBitStr = strList.at(1);

                    /*块部分的转换*/
                    int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*高位部分的转换*/
                    int nWordValue = DealWithString::stringToInt(sWordStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*低位部分的转换*/
                    int nBitValue = DealWithString::stringToInt(sBitStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*小数部分不能超过16，也不能为负数*/
                    if(nBlockValue < 0 || nBlockValue >= 513 || nWordValue < 0 || nWordValue >= 8193 || nBitValue < 0 || nBitValue >= 8)
                    {
                        return false;
                    }

                    /*整合后的结果*/
                    mAddrTarget.nAddrValue = (nBlockValue * 8193 + nWordValue)*8 + nBitValue;

                    /*判断长度是否超限*/
                    if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 513*8193*8)
                    {
                        return false;
                    }
                    return true;
                }
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                /*块部分的转换*/
                int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*高位部分的转换*/
                int nWordValue = DealWithString::stringToInt(sElsePass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                mAddrTarget.nAddrValue = nBlockValue * 8193 + nWordValue;

                /*判断长度是否超限*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 65535 * 8193)
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
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096)
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
            sRegNameList.push_back("I");
            sRegNameList.push_back("Q");
            sRegNameList.push_back("M");
            sRegNameList.push_back("DB_BIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("DB_W");
            sRegNameList.push_back("DB_D");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("DB_D");
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
            sRegNameList.push_back("I");
            sRegNameList.push_back("Q");
            sRegNameList.push_back("M");
            sRegNameList.push_back("DB_BIT");
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("DB_W");
            sRegNameList.push_back("DB_D");
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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_DB_BIT:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
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
                if(nIntValue < 0 || nIntValue >= ADDR_MAX_VALUE_4096 || nDecValue < 0 || nDecValue >= 8)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nDecValue + nInterval >= 8)
                    {
                        nIntValue += (nDecValue + nInterval)/8;
                        nDecValue = (nDecValue + nInterval)%8;
                    }
                    else
                    {
                        nDecValue += nInterval;
                    }

                    /*整合后的结果*/
                    int nStartValue = nIntValue*8 + nDecValue;

                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096*8)
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
    case REG_TYPE_DB_BIT:
        {
            /*按"."拆分*/
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                strList = DealWithString::split(sElsePass, ".");

                if(strList.size() == 2)
                {
                    string sWordStr = strList.at(0);
                    string sBitStr = strList.at(1);

                    /*块部分的转换*/
                    int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*高位部分的转换*/
                    int nWordValue = DealWithString::stringToInt(sWordStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*低位部分的转换*/
                    int nBitValue = DealWithString::stringToInt(sBitStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*判断数据合法性*/
                    if(nBlockValue < 0 || nBlockValue >= 513 || nWordValue < 0 || nWordValue >= 8193 || nBitValue < 0 || nBitValue >= 8)
                    {
                        return ADDR_OUT_BOUND;
                    }

                    char buf[10] = {0};
                    string sTmpStr;
                    for(int i = 0; i < nCopyNum; i++)
                    {
                        if(nBitValue + nInterval >= 8)
                        {
                            nWordValue += (nBitValue + nInterval)/8;
                            if(nWordValue >= 8193)
                            {
                                nBlockValue += nWordValue/8193;
                                nWordValue = nWordValue%8193;
                            }
                            nBitValue = (nBitValue + nInterval)%8;
                        }
                        else
                        {
                            nBitValue += nInterval;
                        }

                        /*整合后的结果*/
                        int nStartValue = (nBlockValue*8193 + nWordValue)*8 + nBitValue;

                        if(nStartValue + mTmpAddr.nAddrLen <= 513*8193*8)
                        {
                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockValue);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加整数部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordValue);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr + ".";

                            /*添加小数部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBitValue);
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
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            nInterval = nInterval*2;
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                /*块部分的转换*/
                int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*高位部分的转换*/
                int nWordValue = DealWithString::stringToInt(sElsePass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nWordValue + nInterval >= 8193)
                    {
                        nBlockValue += (nWordValue + nInterval)/8193;
                        nWordValue = (nWordValue + nInterval)%8193;
                    }
                    else
                    {
                        nWordValue += nInterval;
                    }

                    /*整合后的结果*/
                    int nStartValue = nBlockValue*8193 + nWordValue;

                    if(nStartValue + mTmpAddr.nAddrLen <= 100*8193)
                    {
                        /*添加块部分*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nBlockValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr + ":";

                        /*添加整数部分*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nWordValue);
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nInterval = nInterval*2;
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_4096)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096)
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
 * Function: unpack read data
 * Parameters: address list
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = checkRcvData(mUnpackProp, rcvList, CHECK_READ);
    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    if(mUnpackProp.bNeedSetData)
    {
        /*取有效数据*/
        int nSize = rcvList.size();
        vector<uchar> nTmpRcvList ;
        nTmpRcvList.clear();
        for(int i = 0; i < nSize; i++)
        {
            nTmpRcvList.push_back(rcvList.at(i));
            if(i < nSize -1)
            {
                if(rcvList.at(i) == 0x10 && rcvList.at(i+1) == 0x10)
                {
                    i++;
                }
            }
        }

        targetList.clear();
        nSize = nTmpRcvList.size();
        for(int i = 24; i < nSize - 3; i++)
        {
            targetList.push_back(nTmpRcvList.at(i));
        }

        eCmnInfo = CMN_NOMAL_CODE;
        return eCmnInfo;
    }

    return eCmnInfo;
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
    if(nRcvSize < 1)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    mUnpackProp.bNeedSetData = false;
    switch(mUnpackProp.nCurrTimes)
    {
    case 1:                               //第一次校验
    case 6:                               //第六次校验
    case 7:                               //第七次校验
        {
            if(nRcvSize == 1)
            {
                if(rcvList.front() == 0x10)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
                else
                {
                    eCmnInfo = DATA_CHECK_ERROR;
                    return eCmnInfo;
                }
            }
            break;
        }
    case 2:                               //第二次校验
        {
            if(nRcvSize == 2)
            {
                if(rcvList.at(0) == 0x10 && rcvList.at(1) == 0x02)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
                else
                {
                    eCmnInfo = DATA_CHECK_ERROR;
                    return eCmnInfo;
                }
            }
            break;
        }
    case 3:                               //第三次校验
        {
            if(nRcvSize >= 10)
            {
                if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                {
                    /*读写校验一样*/
                    uchar nCheckByte = rcvList.at(nRcvSize -1);
                    rcvList.pop_back();
                    ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                    if(nCheckResult == nCheckByte)
                    {
                        eCmnInfo = CMN_NOMAL_CODE;
                        return eCmnInfo;
                    }
                    else
                    {
                        eCmnInfo = DATA_CHECK_ERROR;
                        return eCmnInfo;
                    }
                }
            }
            break;
        }
    case 4:                               //第四次校验
        {
            if(nRcvSize == 1)
            {
                if(rcvList.front() == 0x02)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
                else
                {
                    eCmnInfo = DATA_CHECK_ERROR;
                    return eCmnInfo;
                }
            }
            break;
        }
    case 5:                               //第五次校验
        {
            if(CHECK_READ == eCheckType)            //读校验
            {
                /*长度大于等于29，且结束符为10 03，且校验正确*/
                if(nRcvSize >= 29)
                {
                    if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                    {
                        /*读写校验一样*/
                        uchar nCheckByte = rcvList.at(nRcvSize -1);
                        rcvList.pop_back();
                        ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                        if(nCheckResult == nCheckByte)
                        {
                            eCmnInfo = CMN_NOMAL_CODE;
                            mUnpackProp.bNeedSetData = true;
                            return eCmnInfo;
                        }
                        else
                        {
                            eCmnInfo = DATA_CHECK_ERROR;
                            return eCmnInfo;
                        }
                    }
                }
            }
            else if(CHECK_WRITE == eCheckType)            //写校验
            {
                /*长度大于等于24，且结束符为10 03，且校验正确*/
                if(nRcvSize >= 24)
                {
                    if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                    {
                        /*读写校验一样*/
                        uchar nCheckByte = rcvList.at(nRcvSize -1);
                        rcvList.pop_back();
                        ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                        if(nCheckResult == nCheckByte)
                        {
                            eCmnInfo = CMN_NOMAL_CODE;
                            mUnpackProp.bNeedSetData = true;
                            return eCmnInfo;
                        }
                        else
                        {
                            eCmnInfo = DATA_CHECK_ERROR;
                            return eCmnInfo;
                        }
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

    return eCmnInfo;
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    switch(nPkgIndex)
    {
    case 1:
        {
            /*******************添加第一次握手包************************
              * send(1)       02
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /*******************添加第二次握手包************************
              * send(26)       01 03 02 17  00 9f 01 3c  00 90 01 14  00 00 14 02  00 0f 05 01  00 03 00 10  03 3d
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x17);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x9f);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x3c);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x90);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x14);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x14);

            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0f);
            mHandShake.sendDataList.push_back(0x05);
            mHandShake.sendDataList.push_back(0x01);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);

            mHandShake.sendDataList.push_back(0x3d);
            mHandShake.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /*******************添加第三次握手包************************
              * send(1)       10
              * return(12)    01 03 20 56  30 30 2e 36  30 10 03 4f
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 12;
            break;
        }
    case 4:
        {
            /*******************添加第四次握手包************************
              * send(2)       10 02
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /*******************添加第五次握手包************************
              * send(23)       00 0d 00 03 e0   04 00 80 00 02   01 06 01 00 00    01 02 02 01 00   10 03 7d
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0d);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xe0);

            mHandShake.sendDataList.push_back(0x04);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x80);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x06);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x7d);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 6:
        {
            /*******************添加第六次握手包************************
              * send(1)        10
              * return(23)     00 0c 03 03  d0 04 00 80  01 06 00 02  00 01 02 02  01 00 01 00  10 03 4f
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 23;
            break;
        }
    case 7:
        {
            /*******************添加第七次握手包************************
              * send(2)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 8:
        {
            /*******************添加第八次握手包************************
              * send(2)        00 0c 03 03 05   01 10 03 1b
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x05);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x1b);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 9:
        {
            /*******************添加第九次握手包************************
              * send(1)        10
              * return(9)      00 0c 03 03 05   01 10 03 1b
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 9;
            break;
        }
    case 10:
        {
            /*******************添加第十次握手包************************
              * send(2)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 11:
        {
            /*******************添加第十一次握手包************************
              * send(27)       00 0c 03 03 f1   00 32 01 00 00   ff ff 00 08 00   00 f0 00 00 03   00 03 01 00 10   03 24
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xf1);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x32);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x08);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xf0);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x24);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 12:
        {
            /*******************添加第十二次握手包************************
              * send(1)        10
              * return(10)     00 0c 03 03  b0 01 00 10  03 ae
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 10;
            break;
        }
    case 13:
        {
            /*******************添加第十三次握手包************************
              * send(1)        10
              * return(1)      02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 14:
        {
            /*******************添加第十四次握手包************************
              * send(1)        10
              * return(29)     00 0c 03 03  f1 00 32 03  00 00 ff ff  00 08 00 00  00 00 f0 00  00 02 00 02  00 f0 10 03  d7
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 29;
            break;
        }
    case 15:
        {
            /*******************添加第十五次握手包************************
              * send(1)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 16:
        {
            /*******************添加第十六次握手包************************
              * send(10)      00 0c 03 03 b0   01 00 10 03 ae
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xb0);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xae);

            mHandShake.nReturnLen = 1;
            nCmnTimes = 1;
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

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes: 次数从1开始
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    switch(nCurrTimes)
    {
    case 1:                 //10
    case 4:
    case 7:
    case 10:
    case 15:
    case 16:
        {
            if(nRcvSize != 1)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x10)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 2:                 //10 02
    case 5:
    case 8:
    case 11:
        {
            if(nRcvSize != 2)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x10 && rcvList.at(1) == 0x02)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 3:                 //01 03 20 56  30 30 2e 36  30 10 03 4f
        {
            if(nRcvSize != 12)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 6:                 // 00 0c 03 03  d0 04 00 80  01 06 00 02  00 01 02 02  01 00 01 00  10 03 4f
        {
            if(nRcvSize != 23)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 9:                 // 00 0c 03 03 05   01 10 03 1b
        {
            if(nRcvSize != 9)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 12:                 // 00 0c 03 03  b0 01 00 10  03 ae
        {
            if(nRcvSize != 10)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 13:                 // 02
        {
            if(nRcvSize != 1)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x02)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 14:                 // 00 0c 03 03  f1 00 32 03  00 00 ff ff  00 08 00 00  00 00 f0 00  00 02 00 02  00 f0 10 03  d7
        {
            if(nRcvSize != 29)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    default:
        {
            break;
        }
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*对当前打包的地址赋值*/
    mCurrAddr.mAddrProp = mCmnProp.mAddrInfo.mAddrProp;

    /*功能码*/
    switch(mCmnProp.eSendType)
    {
    case CMN_READ:
        {
            mCurrAddr.nValueList.clear();
            return makeReadPackage(mCmnProp, nCmnTimes, mSendPackge);
            break;
        }
    case CMN_WRITE:
        {
            /*如果写数据为空，则返回*/
            if(mCmnProp.mAddrInfo.nValueList.empty()) break;

            mCurrAddr.nValueList = mCmnProp.mAddrInfo.nValueList;
            return makeWritePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case CMN_READ_BEFORE_WRITE:
        {
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            break;
        }
    default:
        {
            break;
        }
    }

    return false;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makeReadPackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge)
{
    /*初始化*/
    ADDR_TARGET_PROP mAddrInfo = mCmnProp.mAddrInfo.mAddrProp;

    mSendPackge.nTotalPkgNum = 7;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*块部分的转换*/
    int nBlockValue = 0;
    int nWordValue = 0;
    uchar nRwLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.nAddrValue/8;
            nWordValue = nWordValue*8;

            /*读写字节长度*/
            nRwLen = mAddrInfo.nAddrLen/8;
            if(mAddrInfo.nAddrLen%8 != 0)
            {
                nRwLen++;
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.nAddrValue*8;

            /*读写字节长度*/
            nRwLen = mAddrInfo.nAddrLen*2;
            break;
        }
    case REG_TYPE_DB_BIT:
        {
            nBlockValue = mAddrInfo.nAddrValue/(8193*8);
            nWordValue = (mAddrInfo.nAddrValue/8)%8193;
            nWordValue = nWordValue*8;

            /*读写字节长度*/
            nRwLen = mAddrInfo.nAddrLen/8;
            if(mAddrInfo.nAddrLen%8 != 0)
            {
                nRwLen++;
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            nBlockValue = mAddrInfo.nAddrValue/8193;
            nWordValue = mAddrInfo.nAddrValue%8193;
            nWordValue = nWordValue*8;

            /*读写字节长度*/
            nRwLen = mAddrInfo.nAddrLen*2;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            /* 功能码*/
            nFunNum = 0x81;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_QW:
        {
            /* 功能码*/
            nFunNum = 0x82;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            /* 功能码*/
            nFunNum = 0x83;
            break;
        }
    case REG_TYPE_DB_BIT:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            /* 功能码*/
            nFunNum = 0x84;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    ushort nCrcResult;
    uchar nCrcResultL;
    switch(mSendPackge.nPkgIndex)
    {
    case 1:
        {
            /***************** 生成第一次发送的包 ***********************/
            /* 发送：02
             * 返回：10
             */
            mSendPackge.sendDataList.clear();
            mSendPackge.sendDataList.push_back(0x02);
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /***************** 生成第二次发送的包 ***********************/
            /* 发送：00 0c 03 03  f1 次数 32 01  00 00 33 01  00 0e 00 00  04 01 12 0a  10 10 02 00  02 00 00 81  00 00 08 10 03 77

             * 返回：10 02
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xf1);

            if(nCmnTimes == 0x10)
            {
                /* index = 5*/
                mSendPackge.sendDataList.push_back(0x10);

                /* index = 6*/
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                /* index = 5*/
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 6*/
            mSendPackge.sendDataList.push_back(0x32);

            /* index = 7 */
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 9*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 10, 写跟读不同的地方， 读：0x33,写：0x44 */
            mSendPackge.sendDataList.push_back(0x33);

            /* index = 11 */
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 12 */
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 13 */
            mSendPackge.sendDataList.push_back(0x0e);

            /* index = 14*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 15, 写跟读不同的地方， 读：0x00,写：0x05*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 16, 写跟读不同的地方， 读：0x04,写：0x05*/
            mSendPackge.sendDataList.push_back(0x04);

            /* index = 17*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 18*/
            mSendPackge.sendDataList.push_back(0x12);

            /* index = 19*/
            mSendPackge.sendDataList.push_back(0x0a);

            /* index = 20*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 21*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 22*/
            mSendPackge.sendDataList.push_back(0x02);

            /* index = 23*/
            mSendPackge.sendDataList.push_back(0x00);

            if(nRwLen == 0x10)
            {
                /* index = 24*/
                mSendPackge.sendDataList.push_back(nRwLen);

                /* index = 25*/
                mSendPackge.sendDataList.push_back(nRwLen);
            }
            else
            {
                /* index = 24*/
                mSendPackge.sendDataList.push_back(nRwLen);
            }

            /*设置值, 读取长度*/
            uchar nLowByte = (nBlockValue) & 0xff;
            uchar nHighByte = (nBlockValue >> 8) & 0xff;

            /*高字节先存*/
            if(nHighByte == 0x10)
            {
                /* index = 25*/
                mSendPackge.sendDataList.push_back(nHighByte);

                /* index = 26*/
                mSendPackge.sendDataList.push_back(nHighByte);
            }
            else
            {
                /* index = 25*/
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /*低字节后存*/
            if(nLowByte == 0x10)
            {
                /* index = 6*/
                mSendPackge.sendDataList.push_back(nLowByte);

                /* index = 27*/
                mSendPackge.sendDataList.push_back(nLowByte);
            }
            else
            {
                /* index = 26*/
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 27*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 28, 29, 30, 读取的地址值, 高字节在前*/
            nTmpValue = (nWordValue >> 16) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = (nWordValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 31*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 32*/
            mSendPackge.sendDataList.push_back(0x03);

            /*计算校验码*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /***************** 生成第三次发送的包 ***********************/
            /* 发送：10
             * 返回：00 0c 03 03  b0 01 00 10  03 ae
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 11;
            }
            else
            {
                mSendPackge.nReturnLen = 10;
            }
            break;
        }
    case 4:
        {
            /***************** 生成第四次发送的包 ***********************/
            /* 发送：10
             * 返回：02
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /***************** 生成第五次发送的包 ***********************/
            /* 发送：10
             * 返回：00 0c 03 03  f1 01 32 01  00 00 33 01  00 0e 00 00  04 01 12 0a  10 10 02 00  02 00 00 81  00 00 08 10 03 74
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = -1;
            break;
        }
    case 6:
        {
            /***************** 生成第六次发送的包 ***********************/
            /* 发送：10 02
             * 返回：10
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x02);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 7:
        {
            /***************** 生成第七次发送的包 ***********************/
            /* 发送：00 0c 03 03  b0 01 01 10 03 af
             * 返回：10
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xb0);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 6*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x03);

            /*计算校验码*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 9*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;

            /*通信次数的递加*/
            if(nCmnTimes >= 255 || nCmnTimes < 1)
            {
                nCmnTimes = 1;
            }
            else
            {
                nCmnTimes++;
            }
            break;
        }
    default:
        {
            return false;
        }
    }

    return true;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makeWritePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 7;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*写的数值*/
    if(mAddrInfo.nValueList.empty()) return false;

    /*块部分的转换*/
    int nBlockValue = 0;
    int nWordValue = 0;
    uchar nBitValue = 0;
    ushort nWriteType = 0;
    ushort nOffsetLen = 0;
    ushort nFunNum = 0;
    ushort nRwUnit = 0;
    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();

    int nRwLen = 1;
    uchar nAddrRwLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
//    case REG_TYPE_I:     不可写
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_DB_BIT:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.mAddrProp.nAddrValue/8;
            nBitValue = mAddrInfo.mAddrProp.nAddrValue%8;

            /*如果是VB则偏移地址 —— 乘以8 */
            if(REG_TYPE_DB_BIT == eRegType)
            {
                nBlockValue = (mAddrInfo.mAddrProp.nAddrValue/8)/8193;
                nWordValue = (mAddrInfo.mAddrProp.nAddrValue/8)%8193;
                nBitValue = mAddrInfo.mAddrProp.nAddrValue%8;
            }

            nWordValue = nWordValue*8 + nBitValue;

            /*写入长度, 15*/
            nRwLen = 1;
            nAddrRwLen = 0x04 + nRwLen;

            /*写入的是位,31、32*/
            nWriteType = 0x0003;

            /*偏移量,33、34*/
            nOffsetLen = 0x0001;

            /*读写单位, 01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
            nRwUnit = 0x01;

            /*添加数据*/
            nTmpWriteList.clear();
            if(!mAddrInfo.nValueList.empty())
            {
                if(mAddrInfo.nValueList.at(0) == 1)
                {
                    nTmpWriteList.push_back(1);
                }
                else
                {
                    nTmpWriteList.push_back(0);
                }
            }
            else
            {
                return false;
            }

            /*如果为空，则返回空*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
//    case REG_TYPE_IW:   不可写
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.mAddrProp.nAddrValue*8;

            /*写入长度, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;

            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*写入的是位,31、32*/
            nWriteType = 0x3004;

            /*偏移量,33、34*/
            nOffsetLen = nRwLen *8;

            /*读写单位, 01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
            nRwUnit = 0x02;

            /*添加数据*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/2; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                /*高字节先存*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*低字节后存*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            /*如果为空，则返回空*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DB_W:
        {
            nBlockValue = (mAddrInfo.mAddrProp.nAddrValue)/8193;
            nWordValue = (mAddrInfo.mAddrProp.nAddrValue)%8193;
            nWordValue = nWordValue*8;

            /*写入长度, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;
            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*写入的是位,31、32*/
            nWriteType = 0x3004;

            /*偏移量,33、34*/
            nOffsetLen = nRwLen *8;

            /*读写单位, 01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
            nRwUnit = 0x02;

            /*添加数据*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/2; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                /*高字节先存*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*低字节后存*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            /*如果为空，则返回空*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DB_D:
        {
            nBlockValue = (mAddrInfo.mAddrProp.nAddrValue)/8193;
            nWordValue = (mAddrInfo.mAddrProp.nAddrValue)%8193;
            nWordValue = nWordValue*8;

            /*写入长度, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;
            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*写入的是位,31、32*/
            nWriteType = 0x3004;

            /*偏移量,33、34*/
            nOffsetLen = nRwLen *8;

            /*读写单位, 01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
            nRwUnit = 0x02;

            /*添加数据*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/4; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*4 + 3);
                uchar nLowByte = mAddrInfo.nValueList.at(i*4 + 2);

                /*高字节先存*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*低字节后存*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }

                nHighByte = mAddrInfo.nValueList.at(i*4 + 1);
                nLowByte = mAddrInfo.nValueList.at(i*4);

                /*高字节先存*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*低字节后存*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            if(nRwLen%4 == 2)
            {
                for(int i = (nRwLen/4)*2; i < nRwLen/2; i++ )
                {
                    uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                    uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                    /*高字节先存*/
                    nTmpWriteList.push_back(nHighByte);
                    if(nHighByte == 0x10)
                    {
                        nTmpWriteList.push_back(nHighByte);
                    }

                    /*低字节后存*/
                    nTmpWriteList.push_back(nLowByte);
                    if(nLowByte == 0x10)
                    {
                        nTmpWriteList.push_back(nLowByte);
                    }
                }
            }

            /*如果为空，则返回空*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            /* 功能码*/
            nFunNum = 0x0081;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_QW:
        {
            /* 功能码*/
            nFunNum = 0x0082;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            /* 功能码*/
            nFunNum = 0x0083;
            break;
        }
    case REG_TYPE_DB_BIT:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            /* 功能码*/
            nFunNum = 0x0084;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    ushort nCrcResult;
    uchar nCrcResultL;
    switch(mSendPackge.nPkgIndex)
    {
    case 1:
        {
            /***************** 生成第一次发送的包 ***********************/
            mSendPackge.sendDataList.clear();
            mSendPackge.sendDataList.push_back(0x02);
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /***************** 生成第二次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xf1);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 6*/
            mSendPackge.sendDataList.push_back(0x32);

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 9*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 10, 写跟读不同的地方， 读：0x33,写：0x44*/
            mSendPackge.sendDataList.push_back(0x44);

            /* index = 11*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 12*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 13*/
            mSendPackge.sendDataList.push_back(0x0e);

            /* index = 14*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 15, 写的长度*/
            mSendPackge.sendDataList.push_back(nAddrRwLen);
            if(nAddrRwLen == 0x10)
            {
                mSendPackge.sendDataList.push_back(nAddrRwLen);
            }

            /* index = 16, 写跟读不同的地方， 读：0x04,写：0x05*/
            mSendPackge.sendDataList.push_back(0x05);

            /* index = 17*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 18*/
            mSendPackge.sendDataList.push_back(0x12);

            /* index = 19*/
            mSendPackge.sendDataList.push_back(0x0a);

            /* index = 20*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 21*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 22*/
            mSendPackge.sendDataList.push_back(nRwUnit);

            /*设置值, 写入长度*/
            uchar nLowByte = (nRwLen) & 0xff;
            uchar nHighByte = (nRwLen >> 8) & 0xff;

            /* index = 23, 高字节先存*/
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 24, 低字节后存*/
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /*设置值, 块的位置*/
            nLowByte = (nBlockValue) & 0xff;
            nHighByte = (nBlockValue >> 8) & 0xff;

            /* index = 25, 高字节先存*/
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 26, 低字节后存*/
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 27*/
            mSendPackge.sendDataList.push_back(nFunNum & 0xff);

            /* index = 28, 29, 30, 读取的地址值, 高字节在前*/
            nLowByte = (nWordValue >> 16) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            nLowByte = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            nLowByte = (nWordValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 31*/
            nHighByte = (nWriteType >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nHighByte);

            /* index = 32*/
            nLowByte = (nWriteType >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);

            /* index = 33*/
            nHighByte = (nOffsetLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 34*/
            nLowByte = (nOffsetLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 35, 设置的值*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x03);

            /*计算校验码*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 38*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /***************** 生成第三次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 11;
            }
            else
            {
                mSendPackge.nReturnLen = 10;
            }
            break;
        }
    case 4:
        {
            /***************** 生成第四次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /***************** 生成第五次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*返回数据的长度*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 25;
            }
            else
            {
                mSendPackge.nReturnLen = 24;
            }
            break;
        }
    case 6:
        {
            /***************** 生成第六次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x02);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 7:
        {
            /***************** 生成第七次发送的包 ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xb0);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 6*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x03);

            /*计算校验码*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 9*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 1;

            /*通信次数的递加*/
            if(nCmnTimes >= 255 || nCmnTimes < 1)
            {
                nCmnTimes = 1;
            }
            else
            {
                nCmnTimes++;
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return true;
}

/******************************************************************
 * Function: merge global address to m_globalReadPlcAddrList
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    targetList.clear();

    if(mSourceAddrList.empty()) return false;

    /*按站号分，放在一个map中，站号为key*/
    int nTotalSize = mSourceAddrList.size();

    /*超过最大读写长度 则等于最大读写长度*/
    if(nMaxRWlen > MAX_RW_LEN)
    {
        nMaxRWlen = MAX_RW_LEN;
    }

    ADDR_TARGET_PROP mTmpAddr;

    /*按寄存器号整理*/
    vector<ADDR_TARGET_PROP > mTmpList0;
    vector<ADDR_TARGET_PROP > mTmpList1;
    vector<ADDR_TARGET_PROP > mTmpList2;
    vector<ADDR_TARGET_PROP > mTmpList3;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);


        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_I:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_IW;

                /*起始位置为所在字的起始位置*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_IW:
            {
                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Q:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_QW;

                /*起始位置为所在字的起始位置*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_QW:
            {
                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_M:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_MW;

                /*起始位置为所在字的起始位置*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MW:
            {
                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DB_BIT:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_DB_W;

                /*起始位置为所在字的起始位置*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DB_W:
        case REG_TYPE_DB_D:
            {
                mTmpList3.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpList0 = PlcMathMethod::addrBubbleSort(mTmpList0);
    mTmpList0 = PlcMathMethod::addrSortOut(mTmpList0, nMaxRWlen, 1, true);
    int tmpSize = mTmpList0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList0.at(i));
    }

    mTmpList1 = PlcMathMethod::addrBubbleSort(mTmpList1);
    mTmpList1 = PlcMathMethod::addrSortOut(mTmpList1, nMaxRWlen, 1, true);
    tmpSize = mTmpList1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList1.at(i));
    }

    mTmpList2 = PlcMathMethod::addrBubbleSort(mTmpList2);
    mTmpList2 = PlcMathMethod::addrSortOut(mTmpList2, nMaxRWlen, 1, true);
    tmpSize = mTmpList2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList2.at(i));
    }

    mTmpList3 = PlcMathMethod::addrBubbleSort(mTmpList3);
    mTmpList3 = PlcMathMethod::addrSortOut(mTmpList3, nMaxRWlen, 1, true);
    tmpSize = mTmpList3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList3.at(i));
    }

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
 * Function: 校验字符串的地址
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
    return PLC_MASTER;
}

/******************************************************************
 * Function: 获得默认站号
 * Parameters: null
 * Return: 默认站号
 ******************************************************************/
int PlcProtocol::getDefaultStation()
{
    int nStation = 2;
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
    mSerialInfo.BAUD_RATE = 19200;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 3;
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
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            return false;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_DB_BIT:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            return true;
            break;
        }
    default:
        return false;
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
            eRegType = REG_TYPE_I;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_Q;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_M;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_DB_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_IW;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_QW;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_MW;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_DB_W;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_DB_D;
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

    if(DealWithString::compare("I", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("Q", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("M", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("DB_BIT", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("IW", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("QW", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("MW", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("DB_W", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("DB_D", sRegName)))
    {
        nRegIndex = 8;
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
    return SERIAL_PORT;
}
