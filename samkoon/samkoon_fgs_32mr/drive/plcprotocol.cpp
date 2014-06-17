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
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,8);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_128)
            {
                return false;
            }

            return bSuccess;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
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
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
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
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue < ADDR_MAX_VALUE_200)
            {
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 200)
                {
                    return false;
                }
            }
            else
            {
                if(mAddrTarget.nAddrLen%2 != 0)
                {
                    mAddrTarget.nAddrLen += 1;
                }

                if(mAddrTarget.nAddrValue > 255)
                {
                    return false;
                }
            }

            return bSuccess;
            break;
        }
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_32)
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
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("M");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("AI");
            sRegNameList.push_back("AO");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("CV");
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
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("AI");
            sRegNameList.push_back("AO");
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
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
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
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 8);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_128)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_128)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%o", nStartValue);
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
    case REG_TYPE_D:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= 8192)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 8192)
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
                else
                {
                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
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

                if(nStartValue < ADDR_MAX_VALUE_200)
                {
                    /*如果长度不满足则复制下一个*/
                    if(nStartValue + mTmpAddr.nAddrLen > ADDR_MAX_VALUE_200)
                    {
                        nCopyNum++;
                        continue;
                    }

                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else if(nStartValue <= 255 && nStartValue + mTmpAddr.nAddrLen <= 257)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;
                    mTmpAddr.nAddrLen = 2;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_32)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_32)
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
CMN_RW_ERROR_CODE PlcProtocol::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<unsigned char> rcvList, vector<unsigned char> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = checkRcvData(mUnpackProp, rcvList, CHECK_READ);
    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    if(mUnpackProp.bNeedSetData)
    {
        /*取有效数据*/
        int nSize = rcvList.size();
        targetList.clear();
        for(int i = READ_HEAD_LEN; i < nSize - CHECK_LEN; i++)
        {
            targetList.push_back(rcvList.at(i));
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
    if(nRcvSize < 3)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvHighByte = rcvList.at(nRcvSize -1);
    uchar nRcvLowByte = rcvList.at(nRcvSize -2);

    rcvList.pop_back();
    rcvList.pop_back();

    ushort nCheckResult = PlcMathMethod::getCRCcheck(rcvList);

    uchar highByte = (nCheckResult >> 8) & 0xff;
    uchar lowByte = nCheckResult & 0xff;

    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
        /*设置数据*/
        mUnpackProp.bNeedSetData = true;
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else
    {
        eCmnInfo = DATA_CHECK_ERROR;
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
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*初始化*/
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*对当前打包的地址赋值*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;
    mCurrAddr.mAddrProp = mAddrInfo.mAddrProp;

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
            mCurrAddr.nValueList = mAddrInfo.nValueList;
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

    return true;
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

    uchar nFumNum = 0;
    ushort nAddrValue = 0;
    int nAddrLen = 1;
    int nReturnLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*功能码*/
            nFumNum = 0x01;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = (mAddrInfo.nAddrValue/8)*8;

            /*算返回的字节长度*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nReturnLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nReturnLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*算取得地址长度*/
            nAddrLen = mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen;
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*功能码*/
            nFumNum = 0x03;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            /*算取得地址长度*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*算返回的字节长度*/
            nReturnLen = nAddrLen *2;
            break;
        }
    case REG_TYPE_CV:
        {
            /*功能码*/
            nFumNum = 0x03;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            if(nAddrValue < 200)
            {
                /*算取得地址长度*/
                nAddrLen = mAddrInfo.nAddrLen;

                nReturnLen = nAddrLen *2;

                /*算返回的字节长度*/
                if(nAddrValue + nAddrLen > 200)
                {
                    return false;
                }
            }
            else if(nAddrValue < 256)
            {
                /*算取得地址长度*/
                nAddrLen = mAddrInfo.nAddrLen;
                if(nAddrValue + nAddrLen > 256)
                {
                    nAddrLen = 256 - nAddrValue;
                }

                nReturnLen = nAddrLen*4;
            }
            else
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
    case REG_TYPE_X:
        {
            break;
        }
    case REG_TYPE_Y:
        {
            /*偏移10000*/
            nAddrValue += 10000;
            break;
        }
    case REG_TYPE_M:
        {
            /*偏移30000*/
            nAddrValue += 30000;
            break;
        }
    case REG_TYPE_T:
        {
            /*偏移 60768*/
            nAddrValue += 60768;
            break;
        }
    case REG_TYPE_C:
        {
            /*偏移 60512*/
            nAddrValue += 60512;
            break;
        }
    case REG_TYPE_D:
        {
            /*偏移 40000*/
            nAddrValue += 40000;
            break;
        }
    case REG_TYPE_TV:
        {
            /*偏移 60256*/
            nAddrValue += 60256;
            break;
        }
    case REG_TYPE_CV:
        {
            /*偏移 60000*/
            nAddrValue += 60000;
            break;
        }
    case REG_TYPE_AI:
        {
            /*偏移 20000*/
            nAddrValue += 20000;
            break;
        }
    case REG_TYPE_AO:
        {
            /*偏移 20512*/
            nAddrValue += 20512;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    /*站号 vector index = 0*/
    uchar nTmpValue = 0;
    memcpy(&nTmpValue, &mAddrInfo.nPlcStationIndex, 1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*对功能码赋值 vector index = 1*/
    mSendPackge.sendDataList.push_back(nFumNum);

    /*开始地址 vector index = 2*/
    nTmpValue = (nAddrValue >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 3*/
    nTmpValue = nAddrValue & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*读取长度*/
    /*vector index = 4*/
    nTmpValue = (nAddrLen >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 5*/
    nTmpValue = (nAddrLen) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*计算校验码*/
    ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
    uchar nCrcResultL = nCrcResult & 0xff;
    uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

    /*vector index = 6*/
    mSendPackge.sendDataList.push_back(nCrcResultL);
    /*vector index = 7*/
    mSendPackge.sendDataList.push_back(nCrcResultH);

    /*返回数据的长度*/
    if(mAddrInfo.nPlcStationIndex == 0)
    {
        mSendPackge.nReturnLen = 0;
    }
    else
    {
        mSendPackge.nReturnLen = nReturnLen + READ_HEAD_LEN + CHECK_LEN;
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

    if(mAddrInfo.nValueList.empty()) return false;

    /*站号 vector index = 0*/
    uchar nTmpValue = 0;
    memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    if(nTmpValue == 0)
    {
        mSendPackge.nReturnLen = 0;
    }
    else
    {
        mSendPackge.nReturnLen = WRITE_RETURN_LEN;
    }

    uchar nFumNum = 0;
    bool bSuccess = false;
    ushort nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
        {
            break;
        }
    case REG_TYPE_Y:
        {
            /*偏移10000*/
            nAddrValue += 10000;
            break;
        }
    case REG_TYPE_M:
        {
            /*偏移30000*/
            nAddrValue += 30000;
            break;
        }
    case REG_TYPE_T:
        {
            /*偏移 60768*/
            nAddrValue += 60768;
            break;
        }
    case REG_TYPE_C:
        {
            /*偏移 60512*/
            nAddrValue += 60512;
            break;
        }
    case REG_TYPE_D:
        {
            /*偏移 40000*/
            nAddrValue += 40000;
            break;
        }
    case REG_TYPE_TV:
        {
            /*偏移 60256*/
            nAddrValue += 60256;
            break;
        }
    case REG_TYPE_CV:
        {
            /*偏移 60000*/
            nAddrValue += 60000;
            break;
        }
    case REG_TYPE_AI:
        {
            /*偏移 20000*/
            nAddrValue += 20000;
            break;
        }
    case REG_TYPE_AO:
        {
            /*偏移 20512*/
            nAddrValue += 20512;
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*对功能码赋值 vector index = 1*/
            nFumNum = 0x05;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*开始地址 vector index = 2*/
            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = nAddrValue & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*赋值*/
            nTmpValue = mAddrInfo.nValueList.front();
            if(nTmpValue == 1)
            {
                /*vector index = 4*/
                mSendPackge.sendDataList.push_back(0xff);

                /*vector index = 5*/
                mSendPackge.sendDataList.push_back(0x00);
            }
            else
            {
                /*vector index = 4*/
                mSendPackge.sendDataList.push_back(0x00);

                /*vector index = 5*/
                mSendPackge.sendDataList.push_back(0x00);
            }

            bSuccess = true;
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            if(1 == mAddrInfo.mAddrProp.nAddrLen)
            {
                /*对功能码赋值 vector index = 1*/
                nFumNum = 0x06;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*开始地址 vector index = 2*/
                nTmpValue = (nAddrValue >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = nAddrValue & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*赋值*/
                if(mAddrInfo.nValueList.size() >= 2)
                {
                    /*vector index = 4*/
                    mSendPackge.sendDataList.push_back(mAddrInfo.nValueList.at(1));
                    /*vector index = 5*/
                    mSendPackge.sendDataList.push_back(mAddrInfo.nValueList.at(0));
                }
                else
                {
                    /*vector index = 4*/
                    mSendPackge.sendDataList.push_back(0x00);
                    /*vector index = 5*/
                    mSendPackge.sendDataList.push_back(mAddrInfo.nValueList.at(0));
                }
            }
            else if(mAddrInfo.mAddrProp.nAddrLen > 1)
            {
                /*对功能码赋值 vector index = 1*/
                nFumNum = 0x10;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*开始地址 vector index = 2*/
                nTmpValue = (nAddrValue >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = nAddrValue & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*地址长度*/
                /*vector index = 4*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 5*/
                nTmpValue = mAddrInfo.mAddrProp.nAddrLen & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 6*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen*2) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                int nLen = 0;
                int nValueSize = mAddrInfo.nValueList.size();

                /*取发送长度*/
                nLen = mAddrInfo.mAddrProp.nAddrLen;
                if(2*nLen > nValueSize)
                {
                    nLen = nValueSize/2;
                }

                for(int i = 0; i < nLen; i++)
                {
                    nTmpValue = mAddrInfo.nValueList.at(2*i + 1);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(2*i + 0);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }
            bSuccess = true;
            break;
        }
    case REG_TYPE_CV:
        {
            /*对功能码赋值 vector index = 1*/
            nFumNum = 0x10;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*开始地址 vector index = 2*/
            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = nAddrValue & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*地址长度*/
            /*vector index = 4*/
            int nLen = mAddrInfo.mAddrProp.nAddrLen;
            if(nAddrValue >= 60200)
            {
                nLen = nLen / 2;
            }
            nTmpValue = (nLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = nLen & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            bSuccess = true;

            if(nAddrValue < 60200)
            {
                /*vector index = 6*/
                nTmpValue = (nLen*2) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                int nValueSize = mAddrInfo.nValueList.size();

                /*取发送长度*/
                if(2*nLen > nValueSize)
                {
                    nLen = nValueSize/2;
                }

                for(int i = 0; i < nLen; i++)
                {
                    nTmpValue = mAddrInfo.nValueList.at(2*i + 1);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(2*i + 0);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }
            else if(nAddrValue < 60256)
            {
                if(nLen + nAddrValue > 60256)
                {
                    nLen = 60256 - nAddrValue;
                }
                /*vector index = 6*/
                nTmpValue = (nLen*4) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                int nValueSize = mAddrInfo.nValueList.size();

                /*取发送长度*/
                if(4*nLen > nValueSize)
                {
                    nLen = nValueSize/4;
                }

                for(int i = 0; i < nLen; i++)
                {
                    nTmpValue = mAddrInfo.nValueList.at(4*i + 1);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 0);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 3);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 2);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }
            else
            {
                bSuccess = false;
            }
            break;
        }
    default:
        {
            break;
        }
    }

    /*计算校验码*/
    ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
    uchar nCrcResultL = nCrcResult & 0xff;
    uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

    /*vector index = 6*/
    mSendPackge.sendDataList.push_back(nCrcResultL);
    /*vector index = 7*/
    mSendPackge.sendDataList.push_back(nCrcResultH);

    if(false == bSuccess)
    {
        mSendPackge.sendDataList.clear();
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
    if(mSourceAddrList.empty()) return false;
    targetList.clear();

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
    mTmpList.resize(10, mTmpList1);

    vector<ADDR_TARGET_PROP > mCvList;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_X:
        case REG_TYPE_Y:
        case REG_TYPE_M:
        case REG_TYPE_T:
        case REG_TYPE_C:
        case REG_TYPE_D:
        case REG_TYPE_TV:
        case REG_TYPE_AI:
        case REG_TYPE_AO:
            {
                mTmpList[eRegIndex].push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CV:
            {
                if(mTmpAddr.nAddrValue < 200)
                {
                    mTmpList[eRegIndex].push_back(mTmpAddr);
                }
                else
                {
                    mTmpAddr.nAddrLen = 2;
                    mCvList.push_back(mTmpAddr);
                }
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
    for(int i = 0; i < 5; i++)
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

    /*REG_TYPE_D ~ REG_TYPE_AO*/
    for(int i = 5; i < 10; i++)
    {
        mTmpList[i] = PlcMathMethod::addrBubbleSort(mTmpList[i]);
        mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], nMaxRWlen/2, 1);

        int tmpSize = mTmpList[i].size();
        for(int k = 0; k < tmpSize; k++)
        {
            targetList.push_back(mTmpList[i].at(k));
        }
    }

    /*CV寄存器的56个*/
    mCvList = PlcMathMethod::addrBubbleSort(mCvList);
    mCvList = PlcMathMethod::addrSortOutForCV(mCvList, nMaxRWlen/2, 1);
    int tmpSize = mCvList.size();
    for(int k = 0; k < tmpSize; k++)
    {
        targetList.push_back(mCvList.at(k));
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
    int nStation = 1;
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_128)
            {
                return false;
            }

            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > 8192)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue < 200)
            {
                if(mAddr.nAddrValue + mAddr.nAddrLen > 200)
                {
                    return false;
                }
            }
            else if(mAddr.nAddrValue + mAddr.nAddrLen > 312)
            {

            }
            return true;
            break;
        }
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32)
            {
                return false;
            }
            return true;
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
    case REG_TYPE_Y:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_128)
            {
                return false;
            }

            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > 8192)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue < 200)
            {
                if(mAddr.nAddrValue + mAddr.nAddrLen > 200)
                {
                    return false;
                }
            }
            else if(mAddr.nAddrValue + mAddr.nAddrLen > 312)
            {

            }
            return true;
            break;
        }
    case REG_TYPE_AO:
        {
            /*判断长度是否超限*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32)
            {
                return false;
            }
            return true;
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
            eRegType = REG_TYPE_T;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_CV;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_AI;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_AO;
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
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("AI", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("AO", sRegName)))
    {
        nRegIndex = 9;
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
