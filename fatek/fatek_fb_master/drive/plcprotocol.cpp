#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

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
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_DRC:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if (mAddrTarget.nAddrValue < ADDR_MAX_VALUE_200)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen/2 - ADDR_MAX_VALUE_200 > ADDR_MAX_VALUE_56)
            {
                return false;
            }

            /*由200开始，偏移到以0开始*/
            mAddrTarget.nAddrValue -= 200;
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
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("R");
            sRegNameList.push_back("D");
            sRegNameList.push_back("TR");
            sRegNameList.push_back("CR");
            sRegNameList.push_back("F");
            sRegNameList.push_back("DRC");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("DRC");
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
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("R");
            sRegNameList.push_back("D");
            sRegNameList.push_back("TR");
            sRegNameList.push_back("CR");
            sRegNameList.push_back("F");
            sRegNameList.push_back("DRC");
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
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
    case REG_TYPE_DRC:
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
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue > ADDR_MAX_VALUE_10000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen < ADDR_MAX_VALUE_10000)
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
    case REG_TYPE_DRC:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue > ADDR_MAX_VALUE_256)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen/2 < ADDR_MAX_VALUE_256)
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
CMN_RW_ERROR_CODE PlcProtocol::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<unsigned char> rcvList, vector<unsigned char> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = checkRcvData(mUnpackProp, rcvList, CHECK_READ);
    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    if(mUnpackProp.bNeedSetData)
    {
        /*取有效数据*/
        targetList.clear();

        int nRcvSize = rcvList.size() - 3;
        for(int i = 6; i < nRcvSize; i++)
        {
            targetList.push_back(rcvList.at(i));
        }

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
 * Function: check recerve is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType)
{
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    if(nRcvSize < 4)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    if(rcvList.back() != 0x03 || rcvList.front() != 0x02)
    {
        eCmnInfo = DATA_CHECK_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = rcvList.at(nRcvSize - 2);
    uchar nRcvHighByte = rcvList.at(nRcvSize - 3);

    rcvList.pop_back();
    rcvList.pop_back();
    rcvList.pop_back();

    ushort nCheckResult = PlcMathMethod::getSumCheck(rcvList) & 0xff;

    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

    /*write success*/
    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
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
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
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

    int nAddrValue = 0;
    int nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x36);

            /*读字节长度 index = 5 6*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7 8*/
            switch (eRegType)
            {
            case REG_TYPE_X:
                mSendPackge.sendDataList.push_back(0x57);
                mSendPackge.sendDataList.push_back(0x58);
                break;
            case REG_TYPE_Y:
                mSendPackge.sendDataList.push_back(0x57);
                mSendPackge.sendDataList.push_back(0x59);
                break;
            case REG_TYPE_M:
                mSendPackge.sendDataList.push_back(0x57);
                mSendPackge.sendDataList.push_back(0x4d);
                break;
            case REG_TYPE_S:
                mSendPackge.sendDataList.push_back(0x57);
                mSendPackge.sendDataList.push_back(0x53);
                break;
            default:
                break;
            }

            /*起始地址 index = 9 10 11 12*/
            nAddrValue = (mAddrInfo.nAddrValue/16)*16;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*校验码 index = 13 14*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x34);

            /*读字节长度 index = 5 6*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }
            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrLen*16) >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrLen*16) >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7*/
            if (eRegType == REG_TYPE_T)
            {
                mSendPackge.sendDataList.push_back(0x54);
            }
            else
            {
                mSendPackge.sendDataList.push_back(0x43);
            }

            /*起始地址 index = 8 9 10 11*/
            nAddrValue = (mAddrInfo.nAddrValue/16)*16;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*校验码 index = 12 13*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 14*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*16;
            break;
        }
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_F:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x36);

            /*读字节长度 index = 5 6*/
            nAddrLen = mAddrInfo.nAddrLen;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7*/
            if (eRegType == REG_TYPE_R)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_D)
            {
                mSendPackge.sendDataList.push_back(0x44);
            }
            else if(eRegType == REG_TYPE_F)
            {
                mSendPackge.sendDataList.push_back(0x46);
            }

            /*起始地址 index = 8 9 10 11 12*/
            nAddrValue = mAddrInfo.nAddrValue;
            uchar tmp1 = nAddrValue/10000;
            uchar tmp2 = (nAddrValue%10000)/1000;
            uchar tmp3 = (nAddrValue%1000)/100;
            uchar tmp4 = (nAddrValue%100)/10;
            uchar tmp5 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp5));

            /*校验码 index = 13 14*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_TR:
    case REG_TYPE_CR:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x36);

            /*读字节长度 index = 5 6*/
            nAddrLen = mAddrInfo.nAddrLen;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7 8*/
            if (REG_TYPE_TR == eRegType)
            {
                mSendPackge.sendDataList.push_back(0x52);
                mSendPackge.sendDataList.push_back(0x54);
            }
            else if(REG_TYPE_CR == eRegType)
            {
                mSendPackge.sendDataList.push_back(0x52);
                mSendPackge.sendDataList.push_back(0x43);
            }

            /*起始地址 index = 9 10 11 12*/
            nAddrValue = mAddrInfo.nAddrValue;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*校验码 index = 13 14*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_DRC:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x36);

            /*读字节长度 index = 5 6*/
            if (mAddrInfo.nAddrLen == 1)//防止该地址选择16位数据类型
            {
                mAddrInfo.nAddrLen = 2;
            }
            nAddrLen = mAddrInfo.nAddrLen/2;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7 8 9*/
            mSendPackge.sendDataList.push_back(0x44);
            mSendPackge.sendDataList.push_back(0x52);
            mSendPackge.sendDataList.push_back(0x43);

            /*起始地址 index = 10 11 12 13*/
            nAddrValue = mAddrInfo.nAddrValue + 200;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*校验码 index = 14 15*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 16*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*8;
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
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makeWritePackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*初始化*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    bool bSuccess = false;
    int nAddrValue = 0;
    int nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4 5*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x32);
            nTmpValue = mAddrInfo.nValueList.front();
            if(nTmpValue == 1)
            {
                /*SET*/
                mSendPackge.sendDataList.push_back(0x33);
            }
            else
            {
                /*RESET*/
                mSendPackge.sendDataList.push_back(0x34);
            }

            /*寄存器代号 index = 6*/
            if (eRegType == REG_TYPE_Y)
            {
                mSendPackge.sendDataList.push_back(0x59);
            }
            else if(eRegType == REG_TYPE_M)
            {
                mSendPackge.sendDataList.push_back(0x4d);
            }
            else if(eRegType == REG_TYPE_S)
            {
                mSendPackge.sendDataList.push_back(0x53);
            }

            /*起始地址 index = 7 8 9 10*/
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*校验码 index = 11 12*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 13*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;
            bSuccess = true;
            break;
        }
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_F:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x37);

            /*写长度 index = 5 6*/
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7*/
            if (eRegType == REG_TYPE_R)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_D)
            {
                mSendPackge.sendDataList.push_back(0x44);
            }
            else
            {
                mSendPackge.sendDataList.push_back(0x46);
            }

            /*起始地址 index = 8 9 10 11 12*/
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            uchar tmp1 = nAddrValue/10000;
            uchar tmp2 = (nAddrValue%10000)/1000;
            uchar tmp3 = (nAddrValue%1000)/100;
            uchar tmp4 = (nAddrValue%100)/10;
            uchar tmp5 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp5));

            /*赋值 index = 13+(nAddrLen-1)*4 14+(nAddrLen-1)*4 15+(nAddrLen-1)*4 16+(nAddrLen-1)*4*/
            for (int i=0; i<nAddrLen; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if (i*2 + 1 < size)
                {
                    nTmpValue = mAddrInfo.nValueList.at(i*2 + 1);

                    uchar nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue1);

                    uchar nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue2);

                    nTmpValue = mAddrInfo.nValueList.at(i*2 + 0);

                    nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue1);

                    nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue2);
                }
            }

            /*校验码 index = 13+nAddrLen*4 14+nAddrLen*4 */
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15+nAddrLen*4 */
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;
            bSuccess = true;
            break;
        }
    case REG_TYPE_TR:
    case REG_TYPE_CR:

        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x37);

            /*写长度 index = 5 6*/
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7 8*/
            if (eRegType == REG_TYPE_TR)
            {
                mSendPackge.sendDataList.push_back(0x52);
                mSendPackge.sendDataList.push_back(0x54);
            }
            else
            {
                mSendPackge.sendDataList.push_back(0x52);
                mSendPackge.sendDataList.push_back(0x43);
            }

            /*起始地址 index = 9 10 11 12*/
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*赋值 index = 13+(nAddrLen-1)*4 14+(nAddrLen-1)*4 15+(nAddrLen-1)*4 16+(nAddrLen-1)*4*/
            for (int i=0; i<nAddrLen; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if (i*2 + 1 < size)
                {
                    nTmpValue = mAddrInfo.nValueList.at(i*2 + 1);

                    uchar nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue1);

                    uchar nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue2);

                    nTmpValue = mAddrInfo.nValueList.at(i*2 + 0);

                    nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue1);

                    nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue2);
                }
            }

            /*校验码 index = 13+nAddrLen*4 14+nAddrLen*4*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15+nAddrLen*4*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;
            bSuccess = true;
            break;
        }
    case REG_TYPE_DRC:
        {
            /*开始符固定 index = 0*/
            uchar nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2*/
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*命令码 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x34);
            mSendPackge.sendDataList.push_back(0x37);

            /*写长度 index = 5 6*/
            if (mAddrInfo.mAddrProp.nAddrLen == 1)//防止该地址选择16位数据类型
            {
                mAddrInfo.mAddrProp.nAddrLen = 2;
            }
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen/2;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*寄存器代号 index = 7 8 9*/
            mSendPackge.sendDataList.push_back(0x44);
            mSendPackge.sendDataList.push_back(0x52);
            mSendPackge.sendDataList.push_back(0x43);

            /*起始地址 index = 10 11 12 13*/
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue + 200;
            uchar tmp1 = nAddrValue/1000;
            uchar tmp2 = (nAddrValue%1000)/100;
            uchar tmp3 = (nAddrValue%100)/10;
            uchar tmp4 = nAddrValue%10;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(tmp4));

            /*赋值 index = 14 15 16 17 18 19 20 21...*/
            int size = mAddrInfo.nValueList.size();
            if(nAddrLen > size/4)
            {
                nAddrLen = size/4;
            }

            for (int i = 0; i < nAddrLen; i++)
            {
                nTmpValue = mAddrInfo.nValueList.at(i*4 + 3);

                uchar nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue1);

                uchar nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue2);

                nTmpValue = mAddrInfo.nValueList.at(i*4 + 2);

                nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue1);

                nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue2);

                nTmpValue = mAddrInfo.nValueList.at(i*4 + 1);

                nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue1);

                nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue2);

                nTmpValue = mAddrInfo.nValueList.at(i*4 + 0);

                nTmpValue1 = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue1);

                nTmpValue2 = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue2);
            }

            /*校验码 index = 14+nAddrLen*8 15+nAddrLen*8*/
            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 16+nAddrLen*8*/
            mSendPackge.sendDataList.push_back(0x03);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;
            bSuccess = true;
            break;
        }
    default:
        {
            break;
        }
    }

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
    vector<ADDR_TARGET_PROP > mTmpListBit0;
    vector<ADDR_TARGET_PROP > mTmpListBit1;
    vector<ADDR_TARGET_PROP > mTmpListBit2;
    vector<ADDR_TARGET_PROP > mTmpListBit3;
    vector<ADDR_TARGET_PROP > mTmpListBit4;
    vector<ADDR_TARGET_PROP > mTmpListBit5;

    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;

    vector<ADDR_TARGET_PROP > mTmpListDWord0;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_X:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Y:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_M:
            {
                mTmpListBit2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_S:
            {
                mTmpListBit3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_T:
            {
                mTmpListBit4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_R:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_D:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TR:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CR:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_F:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DRC:
            {
                mTmpListDWord0.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit0 = PlcMathMethod::addrBubbleSort(mTmpListBit0);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxRWlen, 16);
    }
    int tmpSize = mTmpListBit0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit0.at(i));
    }

    mTmpListBit1 = PlcMathMethod::addrBubbleSort(mTmpListBit1);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 16);
    }
    tmpSize = mTmpListBit1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit1.at(i));
    }

    mTmpListBit2 = PlcMathMethod::addrBubbleSort(mTmpListBit2);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, nMaxRWlen, 16);
    }
    tmpSize = mTmpListBit2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit2.at(i));
    }

    mTmpListBit3 = PlcMathMethod::addrBubbleSort(mTmpListBit3);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, nMaxRWlen, 16);
    }
    tmpSize = mTmpListBit3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit3.at(i));
    }

    mTmpListBit4 = PlcMathMethod::addrBubbleSort(mTmpListBit4);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        int nRwLen = nMaxRWlen/16;
        if(nMaxRWlen%16 != 0)
        {
            nRwLen += 1;
        }
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, nRwLen, 16);
    }
    tmpSize = mTmpListBit4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit4.at(i));
    }

    mTmpListBit5 = PlcMathMethod::addrBubbleSort(mTmpListBit5);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        int nRwLen = nMaxRWlen/16;
        if(nMaxRWlen%16 != 0)
        {
            nRwLen += 1;
        }
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, nRwLen, 16);
    }
    tmpSize = mTmpListBit5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit5.at(i));
    }

    mTmpListWord0 = PlcMathMethod::addrBubbleSort(mTmpListWord0);
    mTmpListWord0 = PlcMathMethod::addrSortOut(mTmpListWord0, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord0.at(i));
    }

    mTmpListWord1 = PlcMathMethod::addrBubbleSort(mTmpListWord1);
    mTmpListWord1 = PlcMathMethod::addrSortOut(mTmpListWord1, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord1.at(i));
    }

    mTmpListWord2 = PlcMathMethod::addrBubbleSort(mTmpListWord2);
    mTmpListWord2 = PlcMathMethod::addrSortOut(mTmpListWord2, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord2.at(i));
    }

    mTmpListWord3 = PlcMathMethod::addrBubbleSort(mTmpListWord3);
    mTmpListWord3 = PlcMathMethod::addrSortOut(mTmpListWord3, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord3.at(i));
    }

    mTmpListWord4 = PlcMathMethod::addrBubbleSort(mTmpListWord4);
    mTmpListWord4 = PlcMathMethod::addrSortOut(mTmpListWord4, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord4.at(i));
    }

    mTmpListDWord0 = PlcMathMethod::addrBubbleSort(mTmpListDWord0);
    mTmpListDWord0 = PlcMathMethod::addrSortOut(mTmpListDWord0, nMaxRWlen/2, 1);
    tmpSize = mTmpListDWord0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListDWord0.at(i));
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
    mSerialInfo.DATA_BIT = 7;
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_DRC:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    default:
        break;
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_R:
    case REG_TYPE_D:
    case REG_TYPE_TR:
    case REG_TYPE_CR:
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_DRC:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    default:
        break;
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
            eRegType = REG_TYPE_S;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_R;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_TR;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_CR;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_F;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_DRC;
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
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("R", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("TR", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("CR", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("F", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("DRC", sRegName)))
    {
        nRegIndex = 11;
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


