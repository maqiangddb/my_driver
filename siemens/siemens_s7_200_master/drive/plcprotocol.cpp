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
    case REG_TYPE_S:
    case REG_TYPE_SM:
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
    case REG_TYPE_VB_BIT:
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
                if(nIntValue < 0 || nDecValue < 0 || nDecValue >= 8)
                {
                    return false;
                }

                /*整合后的结果*/
                mAddrTarget.nAddrValue = nIntValue*8 + nDecValue;

                /*判断长度是否超限*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 10240*8)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_AIW:
    case REG_TYPE_AQW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_64)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_VW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10238)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_VD:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10236)
            {
                return false;
            }
            return bSuccess;
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
            sRegNameList.push_back("S");
            sRegNameList.push_back("SM");
            sRegNameList.push_back("VB_BIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("SMW");
            sRegNameList.push_back("VB");
            sRegNameList.push_back("VW");
            sRegNameList.push_back("VD");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("AIW");
            sRegNameList.push_back("AQW");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("VD");
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
            sRegNameList.push_back("S");
            sRegNameList.push_back("SM");
            sRegNameList.push_back("VB_BIT");
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("SMW");
            sRegNameList.push_back("VB");
            sRegNameList.push_back("VW");
            sRegNameList.push_back("VD");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("AIW");
            sRegNameList.push_back("AQW");
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
    case REG_TYPE_S:
    case REG_TYPE_SM:
    case REG_TYPE_VB_BIT:
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
    case REG_TYPE_SW:
    case REG_TYPE_SMW:
    case REG_TYPE_VB:
    case REG_TYPE_VW:
    case REG_TYPE_VD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_AIW:
    case REG_TYPE_AQW:
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
    case REG_TYPE_S:
    case REG_TYPE_SM:
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
    case REG_TYPE_VB_BIT:
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
                if(nIntValue < 0 || nDecValue < 0 || nDecValue >= 8)
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

                    if(nStartValue + mTmpAddr.nAddrLen <= 10240*8)
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
    case REG_TYPE_AIW:
    case REG_TYPE_AQW:
        {
            nInterval = nInterval*2;
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_64)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_64)
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
    case REG_TYPE_SMW:
    case REG_TYPE_VB:
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
            }
            break;
        }
    case REG_TYPE_VD:
        {
            nInterval = nInterval*2;
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_10236)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_10236)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_VW:
        {
            nInterval = nInterval*2;
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_10238)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_10238)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_256)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_256)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
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
        targetList.clear();
        for(int i = 25; i < nSize - 2; i++)
        {
            targetList.push_back(rcvList.at(i));
        }
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

    /*第一次读校验*/
    if(mUnpackProp.nCurrTimes == 1)
    {
        if(rcvList.front() == 0xe5)
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
    else if(mUnpackProp.nCurrTimes == 2)
    {
        /*读写校验一样*/
        uchar nCheckByte = rcvList.at(nRcvSize -2);
        rcvList.pop_back();
        rcvList.pop_back();
        ushort nCheckResult = PlcMathMethod::getSumCheck(rcvList, 4);
        if((nCheckResult & 0xff) == nCheckByte)
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

    return eCmnInfo;
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: address list
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

    mSendPackge.nTotalPkgNum = 2;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;
    uchar nStationId = mAddrInfo.nPlcStationIndex & 0xff;
    ushort nCrcResult;
    uchar nCrcResultL;

    if(mSendPackge.nPkgIndex == 1)
    {
        mSendPackge.sendDataList.clear();

        /* index = 0, 起始码*/
        mSendPackge.sendDataList.push_back(0x68);

        /* index = 1, 发送长度*/
        mSendPackge.sendDataList.push_back(0x1b);

        /* index = 2*/
        mSendPackge.sendDataList.push_back(0x1b);

        /* index = 3*/
        mSendPackge.sendDataList.push_back(0x68);

        /* index = 4, PLC站号 */
        mSendPackge.sendDataList.push_back(nStationId);

        /* index = 5*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 6*/
        mSendPackge.sendDataList.push_back(0x6c);

        /* index = 7*/
        mSendPackge.sendDataList.push_back(0x32);

        /* index = 8*/
        mSendPackge.sendDataList.push_back(0x01);

        /* index = 9*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 10*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 11*/
        mSendPackge.sendDataList.push_back(0xa1);

        /* index = 12*/
        mSendPackge.sendDataList.push_back(0xa1);

        /* index = 13*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 14*/
        mSendPackge.sendDataList.push_back(0x0e);

        /* index = 15*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 16*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 17*/
        mSendPackge.sendDataList.push_back(0x04);

        /* index = 18*/
        mSendPackge.sendDataList.push_back(0x01);

        /* index = 19*/
        mSendPackge.sendDataList.push_back(0x12);

        /* index = 20*/
        mSendPackge.sendDataList.push_back(0x0a);

        /* index = 21*/
        mSendPackge.sendDataList.push_back(0x10);

        ushort nFunNum = 0;
        int nAddrValue = 0;
        int nAddrLen = 1;
        REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
        switch(eRegType)
        {
        case REG_TYPE_I:
        case REG_TYPE_Q:
        case REG_TYPE_M:
        case REG_TYPE_S:
        case REG_TYPE_SM:
        case REG_TYPE_VB_BIT:
            {
                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x02);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度*/
                if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
                {
                    nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
                }
                else
                {
                    nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
                }

                /* 地址值*/
                nAddrValue = (mAddrInfo.nAddrValue/8)*8;

                /*如果是VB则偏移地址 —— 乘以8 */
                if(REG_TYPE_VB_BIT == eRegType)
                {
                    nAddrValue *= 8;
                }

                /*返回长度*/
                m_nReturnLen = 27 + nAddrLen;
                break;
            }
        case REG_TYPE_IW:
        case REG_TYPE_QW:
        case REG_TYPE_MW:
        case REG_TYPE_SW:
        case REG_TYPE_SMW:
        case REG_TYPE_VB:
        case REG_TYPE_VW:
        case REG_TYPE_VD:
        case REG_TYPE_AIW:
        case REG_TYPE_AQW:
            {
                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x04);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度*/
                nAddrLen = mAddrInfo.nAddrLen;

                /* 地址值*/
                nAddrValue = mAddrInfo.nAddrValue * 8;

                /*返回长度*/
                m_nReturnLen = 27 + nAddrLen*2;
                break;
            }
        case REG_TYPE_TV:
            {
                /*TV和CV寄存器的11、12读写都为0x00，其余都是0xa1*/
                mSendPackge.sendDataList[11] = 0x00;
                mSendPackge.sendDataList[12] = 0x00;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x1f);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度*/
                nAddrLen = mAddrInfo.nAddrLen;

                /* 地址值*/
                nAddrValue = mAddrInfo.nAddrValue;

                /*返回长度*/
                m_nReturnLen = 27 + nAddrLen * 5;
                break;
            }
        case REG_TYPE_CV:
            {
                /*TV和CV寄存器的11、12读写都为0x00，其余都是0xa1*/
                mSendPackge.sendDataList[11] = 0x00;
                mSendPackge.sendDataList[12] = 0x00;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x1e);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度*/
                nAddrLen = mAddrInfo.nAddrLen;

                /* 地址值*/
                nAddrValue = mAddrInfo.nAddrValue;

                /*返回长度*/
                m_nReturnLen = 27 + nAddrLen * 3;
                break;
            }
        default:
            {
                mSendPackge.sendDataList.clear();
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
        case REG_TYPE_S:
        case REG_TYPE_SW:
            {
                /* 功能码*/
                nFunNum = 0x0004;
                break;
            }
        case REG_TYPE_SM:
        case REG_TYPE_SMW:
            {
                /* 功能码*/
                nFunNum = 0x0005;
                break;
            }
        case REG_TYPE_VB_BIT:
        case REG_TYPE_VB:
        case REG_TYPE_VW:
        case REG_TYPE_VD:
            {
                /* 功能码*/
                nFunNum = 0x0184;
                break;
            }

        case REG_TYPE_AIW:
            {
                /* 功能码*/
                nFunNum = 0x0006;
                break;
            }
        case REG_TYPE_AQW:
            {
                /* 功能码*/
                nFunNum = 0x0007;
                break;
            }
        case REG_TYPE_TV:
            {
                /* 功能码*/
                nFunNum = 0x001f;
                break;
            }
        case REG_TYPE_CV:
            {
                /* 功能码*/
                nFunNum = 0x001e;
                break;
            }
        default:
            {
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }
        }

        /*设置值, 读取长度*/
        uchar nLowByte = (nAddrLen) & 0xff;
        uchar nHighByte = (nAddrLen >> 8) & 0xff;

        /* index = 24, 低字节在前*/
        mSendPackge.sendDataList.push_back(nLowByte);

        /* index = 25, 高字节在后*/
        mSendPackge.sendDataList.push_back(nHighByte);

        /* index = 26, 27, 0x0081, 26：01：V存储器 00：其它, 27: 存储器类型*/
        mSendPackge.sendDataList.push_back((nFunNum >> 8) & 0xff);
        mSendPackge.sendDataList.push_back((nFunNum >> 0) & 0xff);

        /* index = 28, 29, 30, 读取的地址值, 高字节在前*/
        mSendPackge.sendDataList.push_back((nAddrValue >> 16) & 0xff);
        mSendPackge.sendDataList.push_back((nAddrValue >> 8) & 0xff);
        mSendPackge.sendDataList.push_back((nAddrValue >> 0) & 0xff);

        /*计算校验码*/
        nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 4);
        nCrcResultL = nCrcResult & 0xff;

        /* index = 31*/
        mSendPackge.sendDataList.push_back(nCrcResultL);

        /* index = 32*/
        mSendPackge.sendDataList.push_back(0x16);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 1;
    }
    else if(mSendPackge.nPkgIndex == 2)
    {
        /***************** 生成第二次发送的包 ***********************/
        mSendPackge.sendDataList.clear();

        /* vector index = 0*/
        mSendPackge.sendDataList.push_back(0x10);

        /*站号 vector index = 1*/
        mSendPackge.sendDataList.push_back(nStationId);

        /*开始地址 vector index = 2*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 3*/
        mSendPackge.sendDataList.push_back(0x5c);

        /* index = 4, 计算校验码*/
        nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);
        nCrcResultL = nCrcResult & 0xff;
        mSendPackge.sendDataList.push_back(nCrcResultL);

        /*vector index = 5*/
        mSendPackge.sendDataList.push_back(0x16);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = m_nReturnLen;
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
    /*初始化*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 2;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*写的数值*/
    if(mAddrInfo.nValueList.empty()) return false;

    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();
    uchar nStationId = mAddrInfo.mAddrProp.nPlcStationIndex & 0xff;
    ushort nCrcResult;
    uchar nCrcResultL;

    if(mSendPackge.nPkgIndex == 1)
    {
        mSendPackge.sendDataList.clear();

        /* index = 0, 起始码*/
        mSendPackge.sendDataList.push_back(0x68);

        /* index = 1, 发送长度*/
        mSendPackge.sendDataList.push_back(0x20);

        /* index = 2*/
        mSendPackge.sendDataList.push_back(0x20);

        /* index = 3*/
        mSendPackge.sendDataList.push_back(0x68);

        /* index = 4, PLC站号 */
        mSendPackge.sendDataList.push_back(nStationId);

        /* index = 5*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 6*/
        mSendPackge.sendDataList.push_back(0x6c);

        /* index = 7*/
        mSendPackge.sendDataList.push_back(0x32);

        /* index = 8*/
        mSendPackge.sendDataList.push_back(0x01);

        /* index = 9*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 10*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 11*/
        mSendPackge.sendDataList.push_back(0xa1);

        /* index = 12*/
        mSendPackge.sendDataList.push_back(0xa1);

        /* index = 13*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 14*/
        mSendPackge.sendDataList.push_back(0x0e);

        /* index = 15*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 16*/
        mSendPackge.sendDataList.push_back(0x05);

        /* index = 17*/
        mSendPackge.sendDataList.push_back(0x05);

        /* index = 18*/
        mSendPackge.sendDataList.push_back(0x01);

        /* index = 19*/
        mSendPackge.sendDataList.push_back(0x12);

        /* index = 20*/
        mSendPackge.sendDataList.push_back(0x0a);

        /* index = 21*/
        mSendPackge.sendDataList.push_back(0x10);

        ushort nWriteType = 0;
        ushort nOffsetLen = 0;
        ushort nFunNum = 0;
        int nAddrValue = 0;
        int nRwLen = 1;
        REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
        switch(eRegType)
        {
        case REG_TYPE_Q:
        case REG_TYPE_M:
        case REG_TYPE_S:
        case REG_TYPE_SM:
        case REG_TYPE_VB_BIT:
            {
                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x01);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*写入长度, 24,25*/
                nRwLen = 1;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue;

                /*写入的是位,31、32*/
                nWriteType = 0x0003;

                /*偏移量,33、34*/
                nOffsetLen = 0x0001;

                /*如果是VB则偏移地址 —— 乘以8 */
                if(REG_TYPE_VB_BIT == eRegType)
                {
                    mSendPackge.sendDataList[6] = 0x7c;
                }

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
                    mSendPackge.sendDataList.clear();
                    return false;
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_IW:
        case REG_TYPE_QW:
        case REG_TYPE_MW:
        case REG_TYPE_SW:
        case REG_TYPE_SMW:
        case REG_TYPE_VW:
            {
                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen * 2;

                int nSetListSize = mAddrInfo.nValueList.size();
                if(nRwLen > nSetListSize)
                {
                    nRwLen = (nSetListSize/2) *2;
                }

                /*修改长度*/
                mSendPackge.sendDataList[1] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (nRwLen + 0x04) & 0xff;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x02);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue * 8;

                /*初始化要写的数值*/
                nTmpWriteList.clear();
                for(int i = 0; i < nRwLen/2; i++ )
                {
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2 + 1));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2));
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_AQW:
            {
                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen;

                int nSetListSize = mAddrInfo.nValueList.size();
                if(nRwLen > nSetListSize)
                {
                    nRwLen = (nSetListSize/2) *2;
                }

                /*修改长度*/
                mSendPackge.sendDataList[1] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (nRwLen + 0x04) & 0xff;

                mSendPackge.sendDataList[6] = 0x7c;
                mSendPackge.sendDataList[11] = 0x00;
                mSendPackge.sendDataList[12] = 0x10;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x04);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue * 8;

                /*初始化要写的数值*/
                nTmpWriteList.clear();
                for(int i = 0; i < nRwLen/2; i++ )
                {
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2 + 1));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2));
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_VB:
            {
                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen;

                int nSetListSize = mAddrInfo.nValueList.size();
                if(nRwLen > nSetListSize/2)
                {
                    nRwLen = nSetListSize/2;
                }

                /*修改长度*/
                mSendPackge.sendDataList[1] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (nRwLen + 0x04) & 0xff;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x02);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue * 8;

                /*初始化要写的数值*/
                nTmpWriteList.clear();
                for(int i = 0; i < nRwLen; i++)
                {
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(2*i));
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_VD:
            {
                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen * 2;

                int nSetListSize = mAddrInfo.nValueList.size();
                if(nRwLen > nSetListSize)
                {
                    nRwLen = (nSetListSize/2)*2;
                }

                /*修改长度*/
                mSendPackge.sendDataList[1] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (nRwLen + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (nRwLen + 0x04) & 0xff;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x02);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue * 8;

                /*初始化要写的数值*/
                nTmpWriteList.clear();
                for(int i = 0; i < nRwLen/4; i++ )
                {
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*4 + 3));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*4 + 2));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*4 + 1));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*4));
                }

                if(nRwLen%4 == 2)
                {
                    for(int i = (nRwLen/4)*2; i < nRwLen/2; i++ )
                    {
                        nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2 + 1));
                        nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2));
                    }
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_TV:
            {
                /*修改长度*/
                mSendPackge.sendDataList[1] = (mAddrInfo.mAddrProp.nAddrLen*5 + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (mAddrInfo.mAddrProp.nAddrLen*5 + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (mAddrInfo.mAddrProp.nAddrLen*5 + 0x04) & 0xff;
                mSendPackge.sendDataList[11] = 0x00;
                mSendPackge.sendDataList[12] = 0x00;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x1f);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen;

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *5*8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue;

                /*初始化要写的数值*/
                nTmpWriteList.clear();

                /* 如果寄存器没添加，则初始化寄存器数据存储 */
                //            while(readDataList.regBitList.size() < MAX_BIT_REG_NUN)
                //            {
                //                vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
                //                readDataList.regBitList.push_back(mTmpValueList);
                //            }

                //            if(readDataList.regBitList.at(8).size() < nAddrValue + nRwLen)
                //            {
                //                ADDR_BIT_VALUE_PROP emptyBitValue;
                //                emptyBitValue.nValue = 0;
                //                emptyBitValue.eErrorStatus = READ_FAIL;
                //                readDataList.regBitList[8].resize(nAddrValue + nRwLen, emptyBitValue);
                //            }

                if(mAddrInfo.nValueList.size() < nRwLen*2)
                {
                    mAddrInfo.nValueList.resize(nRwLen*2, 0);
                }

                for(int i = 0; i < nRwLen; i++)
                {
                    /*读取tc的值*/
                    //                if(readDataList.regBitList.at(8).nValue == 1)
                    //                {
                    //                    nTmpWriteList.push_back(0x02);
                    //                }
                    //                else
                    //                {
                    //                    nTmpWriteList.push_back(0x00);
                    //                }

                    nTmpWriteList.push_back(0x00);

                    nTmpWriteList.push_back(0x00);
                    nTmpWriteList.push_back(0x00);

                    /*添加要写入的值*/
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2 + 1));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2));
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_CV:
            {
                /*修改长度*/
                mSendPackge.sendDataList[1] = (mAddrInfo.mAddrProp.nAddrLen*3 + 0x1f) & 0xff;
                mSendPackge.sendDataList[2] = (mAddrInfo.mAddrProp.nAddrLen*3 + 0x1f) & 0xff;
                mSendPackge.sendDataList[16] = (mAddrInfo.mAddrProp.nAddrLen*3 + 0x04) & 0xff;
                mSendPackge.sendDataList[11] = 0x00;
                mSendPackge.sendDataList[12] = 0x00;

                /* index = 22, 02 读取长度，01为1 bit， 02为1 byte， 04为1 word， 06为1 double word*/
                mSendPackge.sendDataList.push_back(0x1e);

                /* index = 23*/
                mSendPackge.sendDataList.push_back(0x00);

                /*读取长度, 24,25*/
                nRwLen = mAddrInfo.mAddrProp.nAddrLen;

                /*写入的是字,31、32*/
                nWriteType = 0x0004;

                /*偏移量,33、34*/
                nOffsetLen = nRwLen *3*8;

                /* 地址值*/
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue;

                /*初始化要写的数值*/
                nTmpWriteList.clear();

                /* 如果寄存器没添加，则初始化寄存器数据存储 */
                if(mAddrInfo.nValueList.size() < nRwLen*2)
                {
                    mAddrInfo.nValueList.resize(nRwLen*2, 0);
                }

                for(int i = 0; i < nRwLen; i++)
                {
                    /*读取tc的值*/
                    nTmpWriteList.push_back(0x00);

                    /*添加要写入的值*/
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2 + 1));
                    nTmpWriteList.push_back(mAddrInfo.nValueList.at(i*2));
                }

                /*如果为空，则返回空*/
                if(nTmpWriteList.empty())
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        default:
            {
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }
        }

        switch(eRegType)
        {
            //    case REG_TYPE_I:
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
        case REG_TYPE_S:
        case REG_TYPE_SW:
            {
                /* 功能码*/
                nFunNum = 0x0004;
                break;
            }
        case REG_TYPE_SM:
        case REG_TYPE_SMW:
            {
                /* 功能码*/
                nFunNum = 0x0005;
                if(nAddrValue <= 29)
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }
        case REG_TYPE_VB_BIT:
        case REG_TYPE_VB:
        case REG_TYPE_VW:
        case REG_TYPE_VD:
            {
                /* 功能码*/
                nFunNum = 0x0184;
                break;
            }

        case REG_TYPE_AIW:
            {
                /* 功能码*/
                nFunNum = 0x0006;
                break;
            }
        case REG_TYPE_AQW:
            {
                /* 功能码*/
                nFunNum = 0x0007;
                break;
            }
        case REG_TYPE_TV:
            {
                /* 功能码*/
                nFunNum = 0x001f;
                break;
            }
        case REG_TYPE_CV:
            {
                /* 功能码*/
                nFunNum = 0x001e;
                break;
            }
        default:
            {
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }
        }

        /*设置值, 读取长度*/
        uchar nLowByte = (nRwLen) & 0xff;
        uchar nHighByte = (nRwLen >> 8) & 0xff;

        /* index = 24, 低字节在前*/
        mSendPackge.sendDataList.push_back(nLowByte);

        /* index = 25, 高字节在后*/
        mSendPackge.sendDataList.push_back(nHighByte);

        /* index = 26, 27, 0x0081, 26：01：V存储器 00：其它, 27: 存储器类型*/
        mSendPackge.sendDataList.push_back((nFunNum >> 8) & 0xff);
        mSendPackge.sendDataList.push_back((nFunNum >> 0) & 0xff);

        /* index = 28, 29, 30, 读取的地址值, 高字节在前*/
        mSendPackge.sendDataList.push_back((nAddrValue >> 16) & 0xff);
        mSendPackge.sendDataList.push_back((nAddrValue >> 8) & 0xff);
        mSendPackge.sendDataList.push_back((nAddrValue >> 0) & 0xff);

        /* index = 31*/
        nHighByte = (nWriteType >> 8) & 0xff;
        mSendPackge.sendDataList.push_back(nHighByte);

        /* index = 32*/
        nLowByte = (nWriteType >> 0) & 0xff;
        mSendPackge.sendDataList.push_back(nLowByte);

        /* index = 33*/
        nHighByte = (nOffsetLen >> 8) & 0xff;
        mSendPackge.sendDataList.push_back(nHighByte);

        /* index = 34*/
        nLowByte = (nOffsetLen >> 0) & 0xff;
        mSendPackge.sendDataList.push_back(nLowByte);

        /* index = 35, 设置的值*/
        int nWriteLen = nTmpWriteList.size();
        for(int i = 0; i < nWriteLen; i++)
        {
            mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
        }

        /*计算校验码*/
        nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 4);
        nCrcResultL = nCrcResult & 0xff;

        /* index = 36*/
        mSendPackge.sendDataList.push_back(nCrcResultL);

        /* index = 37*/
        mSendPackge.sendDataList.push_back(0x16);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 1;
    }
    else if(mSendPackge.nPkgIndex == 2)
    {
        /***************** 生成第二次发送的包 ***********************/
        mSendPackge.sendDataList.clear();

        /* vector index = 0*/
        mSendPackge.sendDataList.push_back(0x10);

        /*站号 vector index = 1*/
        mSendPackge.sendDataList.push_back(nStationId);

        /*开始地址 vector index = 2*/
        mSendPackge.sendDataList.push_back(0x00);

        /* index = 3*/
        mSendPackge.sendDataList.push_back(0x5c);

        /* index = 4, 计算校验码*/
        nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);
        nCrcResultL = nCrcResult & 0xff;
        mSendPackge.sendDataList.push_back(nCrcResultL);

        /*vector index = 5*/
        mSendPackge.sendDataList.push_back(0x16);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 24;
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

    /*按寄存器号整理 位的缓存*/
    vector<ADDR_TARGET_PROP > mTmpList0;
    vector<ADDR_TARGET_PROP > mTmpList1;
    vector<ADDR_TARGET_PROP > mTmpList2;
    vector<ADDR_TARGET_PROP > mTmpList3;
    vector<ADDR_TARGET_PROP > mTmpList4;
    vector<ADDR_TARGET_PROP > mTmpList5;
    vector<ADDR_TARGET_PROP > mTmpList6;
    vector<ADDR_TARGET_PROP > mTmpList7;

    /*字的缓存*/
    vector<ADDR_TARGET_PROP > mTmpList8;
    vector<ADDR_TARGET_PROP > mTmpList9;

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
        case REG_TYPE_S:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_SW;

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
        case REG_TYPE_SW:
            {
                mTmpList3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SM:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_SMW;

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

                mTmpList4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SMW:
            {
                mTmpList4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_VB_BIT:
            {
                /*转换成对应的字地址类型寄存器*/
                mTmpAddr.nRegIndex = REG_TYPE_VB;

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

                mTmpList5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_VB:
        case REG_TYPE_VW:
        case REG_TYPE_VD:
            {
                mTmpList5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_AIW:
            {
                mTmpList6.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_AQW:
            {
                mTmpList7.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TV:
            {
                mTmpList8.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CV:
            {
                mTmpList9.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*写地址不行地址字节处理*/
    bool bByteType = true;
    if(bWriteAddr)
    {
        bByteType = false;
    }
    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpList0 = PlcMathMethod::addrBubbleSort(mTmpList0);
    mTmpList0 = PlcMathMethod::addrSortOut(mTmpList0, nMaxRWlen, 1, bByteType);
    int tmpSize = mTmpList0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList0.at(i));
    }

    mTmpList1 = PlcMathMethod::addrBubbleSort(mTmpList1);
    mTmpList1 = PlcMathMethod::addrSortOut(mTmpList1, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList1.at(i));
    }

    mTmpList2 = PlcMathMethod::addrBubbleSort(mTmpList2);
    mTmpList2 = PlcMathMethod::addrSortOut(mTmpList2, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList2.at(i));
    }

    mTmpList3 = PlcMathMethod::addrBubbleSort(mTmpList3);
    mTmpList3 = PlcMathMethod::addrSortOut(mTmpList3, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList3.at(i));
    }

    mTmpList4 = PlcMathMethod::addrBubbleSort(mTmpList4);
    mTmpList4 = PlcMathMethod::addrSortOut(mTmpList4, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList4.at(i));
    }

    mTmpList5 = PlcMathMethod::addrBubbleSort(mTmpList5);
    mTmpList5 = PlcMathMethod::addrSortOut(mTmpList5, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList5.at(i));
    }

    mTmpList6 = PlcMathMethod::addrBubbleSort(mTmpList6);
    mTmpList6 = PlcMathMethod::addrSortOut(mTmpList6, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList6.at(i));
    }

    mTmpList7 = PlcMathMethod::addrBubbleSort(mTmpList7);
    mTmpList7 = PlcMathMethod::addrSortOut(mTmpList7, nMaxRWlen, 1, bByteType);
    tmpSize = mTmpList7.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList7.at(i));
    }

    mTmpList8 = PlcMathMethod::addrBubbleSort(mTmpList8);
    mTmpList8 = PlcMathMethod::addrSortOut(mTmpList8, nMaxRWlen/2, 1);
    tmpSize = mTmpList8.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList8.at(i));
    }

    mTmpList9 = PlcMathMethod::addrBubbleSort(mTmpList9);
    mTmpList9 = PlcMathMethod::addrSortOut(mTmpList9, nMaxRWlen/2, 1);
    tmpSize = mTmpList9.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList9.at(i));
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
    mSerialInfo.BAUD_RATE = 9600;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 2;
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
    case REG_TYPE_AIW:
        {
            return false;
            break;
        }
    default:
        break;
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
            eRegType = REG_TYPE_S;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_SM;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_VB_BIT;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_IW;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_QW;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_MW;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_SW;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_SMW;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_VB;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_VW;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_VD;
            break;
        }
    case 14:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 15:
        {
            eRegType = REG_TYPE_CV;
            break;
        }
    case 16:
        {
            eRegType = REG_TYPE_AIW;
            break;
        }
    case 17:
        {
            eRegType = REG_TYPE_AQW;
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
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("SM", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("VB_BIT", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("IW", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("QW", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("MW", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("SW", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("SMW", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("VB", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("VW", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("VD", sRegName)))
    {
        nRegIndex = 13;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 14;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 15;
        return true;
    }
    else if((DealWithString::compare("AIW", sRegName)))
    {
        nRegIndex = 16;
        return true;
    }
    else if((DealWithString::compare("AQW", sRegName)))
    {
        nRegIndex = 17;
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
