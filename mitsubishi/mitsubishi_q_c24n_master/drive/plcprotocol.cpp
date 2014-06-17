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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_B:
    case REG_TYPE_W:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess, 16);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_SB:
    case REG_TYPE_SW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess, 16);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1024)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_M:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 8192)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 11135)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_Z:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 10)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    default:
        {
            return false;
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
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("M");
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("V");
            sRegNameList.push_back("S");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("W");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("Z");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            break;
        }
    case DWORT_ADDR_REG:
        {
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
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("M");
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("V");
            sRegNameList.push_back("S");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D");
            sRegNameList.push_back("W");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("Z");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_B:
    case REG_TYPE_W:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_2048)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_2048)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
    case REG_TYPE_SB:
    case REG_TYPE_SW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_1024)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_1024)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
    case REG_TYPE_M:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= 8191)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 8191)
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
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_2048)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_2048)
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
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_512)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_512)
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
    case REG_TYPE_Z:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= 10)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 10)
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
    case REG_TYPE_D:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= 11136)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 11136)
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
        for(int i = 40; i < nSize - 4; i++)
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
    if(nRcvSize < 44)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    mUnpackProp.bNeedSetData = false;

    /*倒数第三个是结束符0x03*/
    if(rcvList.at(nRcvSize -3) != 0x03)
    {
        eCmnInfo = DATA_CHECK_ERROR;
        return eCmnInfo;
    }

    uchar nRcvResultL = rcvList.at(nRcvSize -1);
    uchar nRcvResultH = rcvList.at(nRcvSize -2);

    int nSize = rcvList.size();
    vector<uchar> nTmpRcvList ;
    nTmpRcvList.clear();
    for(int i = 2; i < nSize -4; i++)
    {
        nTmpRcvList.push_back(rcvList.at(i));
        if(rcvList.at(i) == 0x10 && rcvList.at(i+1) == 0x10)
        {
            i++;
        }
    }

    /*计算校验码*/
    ushort nCrcResult = PlcMathMethod::getSumCheck(nTmpRcvList) & 0xff;
    uchar nCheckL = nCrcResult & 0x0f;
    uchar nCheckH = (nCrcResult >> 4) & 0x0f;
    nCheckL = PlcMathMethod::intToAsciiValue(nCheckL);
    nCheckH = PlcMathMethod::intToAsciiValue(nCheckH);

    if(nRcvResultL == nCheckL && nRcvResultH == nCheckH)
    {
        mUnpackProp.bNeedSetData = true;
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else
    {
        mUnpackProp.bNeedSetData = false;
        eCmnInfo = DATA_CHECK_ERROR;
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: mHandShakeList, nPkgIndex:从1开始
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

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*局部变量的定义*/
    int nWordValue = 0;
    int nRepeatNum = 0;
    ushort nRwLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*算起始地址，一次最小取一个byte*/
            nWordValue = (mAddrInfo.nAddrValue/8)*8;

            /*算返回的字节长度*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nRwLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nRwLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*最低支持一个字的长度读写（16位）*/
            nRwLen = (nRwLen/2) + (nRwLen%2);
            nRwLen *= 16;
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            nWordValue = mAddrInfo.nAddrValue;

            /*读写字节长度*/
            nRwLen = mAddrInfo.nAddrLen;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    switch (eRegType)
    {
    case REG_TYPE_X:
        {
            /* 功能码*/
            nFunNum = 0x9c;
            break;
        }
    case REG_TYPE_Y:
        {
            /* 功能码*/
            nFunNum = 0x9d;
            break;
        }
    case REG_TYPE_M:
        {
            /* 功能码*/
            nFunNum = 0x90;
            break;
        }
    case REG_TYPE_L:
        {
            /* 功能码*/
            nFunNum = 0x92;
            break;
        }
    case REG_TYPE_F:
        {
            /* 功能码*/
            nFunNum = 0x93;
            break;
        }
    case REG_TYPE_V:
        {
            /* 功能码*/
            nFunNum = 0x94;
            break;
        }
    case REG_TYPE_S:
        {
            /* 功能码*/
            nFunNum = 0x98;
            break;
        }
    case REG_TYPE_B:
        {
            /* 功能码*/
            nFunNum = 0xa0;
            break;
        }
    case REG_TYPE_SB:
        {
            /* 功能码*/
            nFunNum = 0xa1;
            break;
        }
    case REG_TYPE_T:
        {
            /* 功能码, 线圈c0，触点c1*/
            nFunNum = 0xc1;
            break;
        }
    case REG_TYPE_C:
        {
            /* 功能码, 线圈c3，触点c4*/
            nFunNum = 0xc4;
            break;
        }
    case REG_TYPE_D:
        {
            /* 功能码*/
            nFunNum = 0xa8;
            break;
        }
    case REG_TYPE_W:
        {
            /* 功能码*/
            nFunNum = 0xb4;
            break;
        }
    case REG_TYPE_SW:
        {
            /* 功能码*/
            nFunNum = 0xb5;
            break;
        }
    case REG_TYPE_TV:
        {
            /* 功能码*/
            nFunNum = 0xc2;
            break;
        }
    case REG_TYPE_CV:
        {
            /* 功能码*/
            nFunNum = 0xc5;
            break;
        }
    case REG_TYPE_Z:
        {
            /* 功能码*/
            nFunNum = 0x94;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    /***************** 生成第一次发送的包 ***********************/
    /* 发送：02
     * 返回：10
     */
    mSendPackge.sendDataList.clear();

    /* index = 0*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 1*/
    mSendPackge.sendDataList.push_back(0x02);

    /* index = 2*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 3*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 4*/
    mSendPackge.sendDataList.push_back(0xfc);

    /* index = 5*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 6*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 7*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 8*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 9*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 10*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 11*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 12*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 13*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 14*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 15*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 16*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 17*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 18*/
    mSendPackge.sendDataList.push_back(0x1a);

    /* index = 19*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 20*/
    mSendPackge.sendDataList.push_back(0x1c);

    /* index = 21*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 22*/
    mSendPackge.sendDataList.push_back(0x0a);

    /* index = 23*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 24*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 25*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 26*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 27*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 28*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 29*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 30*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 31*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 32*/
    mSendPackge.sendDataList.push_back(0x04);

    /* index = 33*/
    mSendPackge.sendDataList.push_back(0x01);

    /* index = 34*/
    mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
    if(nCmnTimes == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
    }

    /* index = 35*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 36*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 37*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 38*/
    mSendPackge.sendDataList.push_back(nFunNum);

    /* index = 39*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 40*/
    nTmpValue = (nWordValue) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 41*/
    nTmpValue = (nWordValue >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 42*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 43*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 44*/
    nTmpValue = (nRwLen) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 45*/
    nTmpValue = (nRwLen >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /*计算校验码*/
    ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
    uchar nCrcResultL = nCrcResult & 0x0f;
    uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
    nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
    nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

    /* index = 46*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 47*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 48*/
    mSendPackge.sendDataList.push_back(nCrcResultH);

    /* index = 49*/
    mSendPackge.sendDataList.push_back(nCrcResultL);

    mSendPackge.nReturnLen = -1;

    /*通信次数的递加*/
    if(nCmnTimes >= 255 || nCmnTimes < 3)
    {
        nCmnTimes = 3;
    }
    else
    {
        nCmnTimes++;
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

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*写的数值*/
    if(mAddrInfo.nValueList.empty()) return false;

    /***************** 生成第一次发送的包 ***********************/
    /* 发送：02
     * 返回：10
     */
    /*局部变量的定义*/
    int nRwLen = 1;
    int nWordValue = 0;
    int nRepeatNum = 0;
    ushort nOffsetLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;
    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();

    mSendPackge.sendDataList.clear();

    /* index = 0*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 1*/
    mSendPackge.sendDataList.push_back(0x02);

    /* index = 2*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 3*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 4*/
    mSendPackge.sendDataList.push_back(0xfc);

    /* index = 5*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 6*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 7*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 8*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 9*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 10*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 11*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 12*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 13*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 14*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 15*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 16*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 17*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 18*/
    mSendPackge.sendDataList.push_back(0x22);     //读1a

    /* index = 19*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 20*/
    mSendPackge.sendDataList.push_back(0x1c);

    /* index = 21*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 22*/
    mSendPackge.sendDataList.push_back(0x0a);

    /* index = 23*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 24*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 25*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 26*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 27*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 28*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 29*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 30*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 31*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 32*/
    mSendPackge.sendDataList.push_back(0x14);         //读04

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch (eRegType)
    {
    case REG_TYPE_Y:
        {
            /* 功能码*/
            nFunNum = 0x9d;
            break;
        }
    case REG_TYPE_M:
        {
            /* 功能码*/
            nFunNum = 0x90;
            break;
        }
    case REG_TYPE_L:
        {
            /* 功能码*/
            nFunNum = 0x92;
            break;
        }
    case REG_TYPE_F:
        {
            /* 功能码*/
            nFunNum = 0x93;
            break;
        }
    case REG_TYPE_V:
        {
            /* 功能码*/
            nFunNum = 0x94;
            break;
        }
    case REG_TYPE_S:
        {
            /* 功能码*/
            nFunNum = 0x98;
            break;
        }
    case REG_TYPE_B:
        {
            /* 功能码*/
            nFunNum = 0xa0;
            break;
        }
    case REG_TYPE_SB:
        {
            /* 功能码*/
            nFunNum = 0xa1;
            break;
        }
    case REG_TYPE_T:
        {
            /* 功能码, 线圈c0，触点c1*/
            nFunNum = 0xc1;
            break;
        }
    case REG_TYPE_C:
        {
            /* 功能码, 线圈c3，触点c4*/
            nFunNum = 0xc4;
            break;
        }
    case REG_TYPE_D:
        {
            /* 功能码*/
            nFunNum = 0xa8;
            break;
        }
    case REG_TYPE_W:
        {
            /* 功能码*/
            nFunNum = 0xb4;
            break;
        }
    case REG_TYPE_SW:
        {
            /* 功能码*/
            nFunNum = 0xb5;
            break;
        }
    case REG_TYPE_TV:
        {
            /* 功能码*/
            nFunNum = 0xc2;
            break;
        }
    case REG_TYPE_CV:
        {
            /* 功能码*/
            nFunNum = 0xc5;
            break;
        }
    case REG_TYPE_Z:
        {
            /* 功能码*/
            nFunNum = 0x94;
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
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            nWordValue = mAddrInfo.mAddrProp.nAddrValue;

            /*写入长度, 15*/
            nRwLen = 1;

            /*偏移长度*/
            nOffsetLen = 0x22;

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

            /* index = 18, 修改偏移长度*/
            mSendPackge.sendDataList[18] = nOffsetLen;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(0x02);         //读01

            /* index = 34*/
            mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            if(nCmnTimes == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 35*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 38*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 39*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 40*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 41*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 42*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 43*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 44*/
            nTmpValue = (nWordValue) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 45*/
            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 46*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 47*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 48*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 49*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 50*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 51*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 52, 设置的值*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /* index = 53*/
            mSendPackge.sendDataList.push_back(0x00);

            /*计算校验码*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            /* index = 54*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 55*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 56*/
            mSendPackge.sendDataList.push_back(nCrcResultH);

            /* index = 57*/
            mSendPackge.sendDataList.push_back(nCrcResultL);
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            nWordValue = mAddrInfo.mAddrProp.nAddrValue;

            /*写入长度, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen;

            /*偏移量*/
            nOffsetLen = 0x1a + nRwLen*2;

            /*添加数据, 低位先存，高位后存*/
            int nSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSize/2)
            {
                nRwLen = nSize/2;
            }
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen; i++ )
            {
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);

                /*低字节后存*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nRepeatNum++;
                    nTmpWriteList.push_back(nLowByte);
                }

                /*高字节先存*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nRepeatNum++;
                    nTmpWriteList.push_back(nHighByte);
                }
            }

            /*如果为空，则返回空*/
            if(nTmpWriteList.empty())
            {
                mSendPackge.sendDataList.clear();
                return false;
            }

            /* index = 18, 修改偏移长度*/
            mSendPackge.sendDataList[18] = nOffsetLen;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(0x01);         //读01

            /* index = 34*/
            mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            if(nCmnTimes == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 35*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 38*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 39*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 40*/
            nTmpValue = (nWordValue) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 41*/
            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 42*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 43*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 44*/
            mSendPackge.sendDataList.push_back(mAddrInfo.mAddrProp.nAddrLen);
            if(mAddrInfo.mAddrProp.nAddrLen == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(mAddrInfo.mAddrProp.nAddrLen);
            }

            /* index = 45*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 46, 设置的值*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /*计算校验码*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            /* index = 47*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 48*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 49*/
            mSendPackge.sendDataList.push_back(nCrcResultH);

            /* index = 50*/
            mSendPackge.sendDataList.push_back(nCrcResultL);
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    mSendPackge.nReturnLen = -1;

    /*通信次数的递加*/
    if(nCmnTimes >= 255 || nCmnTimes < 3)
    {
        nCmnTimes = 3;
    }
    else
    {
        nCmnTimes++;
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
    vector<vector<ADDR_TARGET_PROP > > mTmpList;

    vector<ADDR_TARGET_PROP > mTmpList1;
    mTmpList.resize(17, mTmpList1);

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_X:
        case REG_TYPE_Y:
        case REG_TYPE_M:
        case REG_TYPE_L:
        case REG_TYPE_F:
        case REG_TYPE_V:
        case REG_TYPE_S:
        case REG_TYPE_B:
        case REG_TYPE_SB:
        case REG_TYPE_T:
        case REG_TYPE_C:
        case REG_TYPE_D:
        case REG_TYPE_W:
        case REG_TYPE_SW:
        case REG_TYPE_Z:
        case REG_TYPE_TV:
        case REG_TYPE_CV:
           {
                mTmpList[eRegIndex].push_back(mTmpAddr);
                break;
           }
        default:
            {
                break;
            }
        }
    }//end for

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    /*REG_TYPE_X ~ REG_TYPE_C*/
    for(int i = 0; i < 11; i++)
    {
        mTmpList[i] = PlcMathMethod::addrBubbleSort(mTmpList[i]);
        if(bWriteAddr)
        {
            /*写地址，则只支持长度为1的写*/
            mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], 1, 1);
        }
        else
        {
            /*读地址，可支持多个长度*/
            mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], nMaxRWlen, 8);
        }

        int tmpSize = mTmpList[i].size();
        for(int k = 0; k < tmpSize; k++)
        {
            targetList.push_back(mTmpList[i].at(k));
        }
    }

    /*REG_TYPE_D ~ REG_TYPE_CV*/
    for(int i = 11; i < 17; i++)
    {
        mTmpList[i] = PlcMathMethod::addrBubbleSort(mTmpList[i]);
        mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], nMaxRWlen/2, 1);

        int tmpSize = mTmpList[i].size();
        for(int k = 0; k < tmpSize; k++)
        {
            targetList.push_back(mTmpList[i].at(k));
        }
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
    case REG_TYPE_X:
        {
            return false;
            break;
        }
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
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
            eRegType = REG_TYPE_X;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_Y;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_M;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_L;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_F;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_V;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_S;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_B;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_SB;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_W;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_SW;
            break;
        }
    case 14:
        {
            eRegType = REG_TYPE_Z;
            break;
        }
    case 15:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 16:
        {
            eRegType = REG_TYPE_CV;
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

    if(DealWithString::compare("X", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("Y", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("M", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("L", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("F", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("V", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("B", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("SB", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("W", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("SW", sRegName)))
    {
        nRegIndex = 13;
        return true;
    }
    else if((DealWithString::compare("Z", sRegName)))
    {
        nRegIndex = 14;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 15;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 16;
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

