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
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,8);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 160000)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 9000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if (mAddrTarget.nAddrValue < 9000 || mAddrTarget.nAddrValue > 10000)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 10000)
            {
                return false;
            }

            /*由8000开始，偏移到以0开始*/
            mAddrTarget.nAddrValue -= 9000;
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if (mAddrTarget.nAddrValue < ADDR_MAX_VALUE_200 || mAddrTarget.nAddrValue > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen/2 > ADDR_MAX_VALUE_256)
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
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
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
            sRegNameList.push_back("SM");
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D_BIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("32CV");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("32CV");
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
            sRegNameList.push_back("SM");
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D_BIT");
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("32CV");
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
    case REG_TYPE_D_BIT:
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
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
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_32CV:
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
    case REG_TYPE_D_BIT:
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

                    if(nStartValue + mTmpAddr.nAddrLen <= 160000)
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
                }
                return ADDR_CORRECT;
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 8);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

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
                    sprintf(buf, "%o", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
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

            /*不能为负数*/
            if(nStartValue < 0 || nStartValue >= 9000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 9000)
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
    case REG_TYPE_SD:
    case REG_TYPE_SM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*不能为负数*/
            if(nStartValue < 0 || nStartValue >= 10000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 10000)
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
    case REG_TYPE_S:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_1000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_1000)
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
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*不能为负数*/
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
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_200)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_200)
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
    case REG_TYPE_32CV:
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

                if(nStartValue + mTmpAddr.nAddrLen/2 <= ADDR_MAX_VALUE_256)
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
        for(int i = 7; i < nRcvSize; i++)
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

    mUnpackProp.bNeedSetData = false;

    if (nRcvSize < 10)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    if (rcvList.at(0) == 0x06 && rcvList.at(nRcvSize - 3) == 0x03)
    {
        uchar nRcvHighByte = rcvList.at(nRcvSize - 2);//PLC返回数据中的校验
        uchar nRcvLowByte = rcvList.at(nRcvSize - 1);

        vector<uchar> templist;
        templist.clear();
        int size = nRcvSize - 2;
        for (int i=1; i<size; i++)
        {
            templist.push_back(rcvList.at(i));
        }

        ushort nCheckResult = PlcMathMethod::getSumCheck(templist);
        uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
        uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

        if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
        {
            mUnpackProp.bNeedSetData = true;
            eCmnInfo = CMN_NOMAL_CODE;
        }
        else
        {
            eCmnInfo = DATA_CHECK_ERROR;
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
    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*对当前打包的地址赋值*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;
    mCurrAddr.mAddrProp = mAddrInfo.mAddrProp;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
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
            if(REG_TYPE_D_BIT == eRegType)
            {
                mCurrAddr.nValueList = mAddrInfo.nValueList;

                mSendPackge.sendDataList.push_back(0x02);//开头字符STX

                uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.sendDataList.push_back(0x35);//命令号码51
                mSendPackge.sendDataList.push_back(0x31);

                uint nAddrValue = mAddrInfo.mAddrProp.nAddrValue/16;
                nAddrValue = nAddrValue * 2 + 0x1c00;

                uchar uctmp1 = (nAddrValue >> 12) & 0xf;
                uchar uctmp2 = (nAddrValue >> 8) & 0xf;
                uchar uctmp3 = (nAddrValue >> 4) & 0xf;
                uchar uctmp4 = (nAddrValue >> 0) & 0xf;

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x32);

                mSendPackge.sendDataList.push_back(0x03);//结束符

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = 14;
            }
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            if(mAddrInfo.nValueList.empty() || mCurrAddr.nValueList.empty()) break;
            if(mAddrInfo.nValueList.size() < 4)
            {
                mAddrInfo.nValueList.push_back(0);
            }

            if(REG_TYPE_D_BIT == eRegType)
            {
                mSendPackge.sendDataList.push_back(0x02);//开头字符STX

                uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.sendDataList.push_back(0x36);//命令号码61
                mSendPackge.sendDataList.push_back(0x31);

                uint nAddrValue = mAddrInfo.mAddrProp.nAddrValue/16;
                nAddrValue = nAddrValue * 2 + 0x1c00;

                uchar uctmp1 = (nAddrValue >> 12) & 0xf;
                uchar uctmp2 = (nAddrValue >> 8) & 0xf;
                uchar uctmp3 = (nAddrValue >> 4) & 0xf;
                uchar uctmp4 = (nAddrValue >> 0) & 0xf;

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x32);

                /*输入值 4 0 12 8的顺序写入，读取返回的顺序也是4 0 12 8*/
                int nValue = 0;
                if (mAddrInfo.nValueList.size() == 4)
                {
                    int nTmp1 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(0));//4
                    int nTmp2 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(1));//0
                    int nTmp3 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(2));//12
                    int nTmp4 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(3));//8
                    /*读取值*/
                    nValue = ((nTmp3 << 12) & 0xf000) + ((nTmp4 << 8) & 0x0f00) + ((nTmp1 << 4) & 0x00f0) + (nTmp2& 0x000f);

                    int nBitPos = mAddrInfo.mAddrProp.nAddrValue%16;
                    /*修改写入值, >0置位*/
                    if(mCurrAddr.nValueList.front() > 0)
                    {
                        nValue |= 1 << nBitPos;
                    }
                    else
                    {
                        nValue &= ~(1 << nBitPos);
                    }
                    /*按4 0 12 8顺序写入*/
                    nTmpValue = PlcMathMethod::intToAsciiValue((nValue >> 4) & 0xf);//4
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((nValue >> 0) & 0xf);//0
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((nValue >> 12) & 0xf);//12
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((nValue >> 8) & 0xf);//8
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                mSendPackge.sendDataList.push_back(0x03);//结束符

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = 10;
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
    int nOffSetAddr = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

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
            mSendPackge.sendDataList.push_back(0x02);//开头字符STX

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x35);//命令号码51
            mSendPackge.sendDataList.push_back(0x31);

            switch (eRegType)
            {
            case REG_TYPE_X:
                nOffSetAddr = 0x0000;
                break;
            case REG_TYPE_Y:
                nOffSetAddr = 0x0040;
                break;
            case REG_TYPE_M:
                nOffSetAddr = 0x0080;
                break;
            case REG_TYPE_SM:
                nOffSetAddr = 0x03e0;
                break;
            case REG_TYPE_S:
                nOffSetAddr = 0x0300;
                break;
            case REG_TYPE_T:
                nOffSetAddr = 0x0380;
                break;
            case REG_TYPE_C:
                nOffSetAddr = 0x03a0;
                break;
            default:
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }

            nAddrValue = (mAddrInfo.nAddrValue/8) + nOffSetAddr;

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            uctmp1 = (nAddrLen >> 4) & 0xf;
            uctmp2 = (nAddrLen >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));

            mSendPackge.sendDataList.push_back(0x03);//结束符

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 10 + nAddrLen*2;
            break;
        }
    case REG_TYPE_D_BIT:
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_32CV:
        {
            mSendPackge.sendDataList.push_back(0x02);//开头字符STX

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x35);//命令号码51
            mSendPackge.sendDataList.push_back(0x31);

            switch (eRegType)
            {
            case REG_TYPE_D:
            case REG_TYPE_D_BIT:
                nOffSetAddr = 0x1c00;
                break;
            case REG_TYPE_SD:
                nOffSetAddr = 0x1600;
                break;
            case REG_TYPE_TV:
                nOffSetAddr = 0x1400;
                break;
            case REG_TYPE_CV:
                nOffSetAddr = 0x1800;
                break;
            case REG_TYPE_32CV:
                nOffSetAddr = 0x1a00;
                break;
            default:
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }

            nAddrValue = mAddrInfo.nAddrValue*2 + nOffSetAddr;

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            uctmp1 = (mAddrInfo.nAddrLen*2 >> 4) & 0xf;
            uctmp2 = (mAddrInfo.nAddrLen*2 >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));

            mSendPackge.sendDataList.push_back(0x03);//结束符

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 10 + mAddrInfo.nAddrLen*4;
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
bool PlcProtocol::makeWritePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*初始化*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    bool bSuccess = false;
    int nOffSetAddr = 0;
    int nAddrValue = 0;
    int nAddrLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mSendPackge.sendDataList.push_back(0x02);//开头字符STX

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            if (mAddrInfo.nValueList.size() > 0)
            {
                nTmpValue = mAddrInfo.nValueList.at(0);
                if (nTmpValue)//置位
                {
                    mSendPackge.sendDataList.push_back(0x37);//命令号码70
                    mSendPackge.sendDataList.push_back(0x30);
                }
                else//复位
                {
                    mSendPackge.sendDataList.push_back(0x37);//命令号码71
                    mSendPackge.sendDataList.push_back(0x31);
                }
            }

            switch (eRegType)
            {
            case REG_TYPE_Y:
                nOffSetAddr = 0x0040;
                break;
            case REG_TYPE_M:
                nOffSetAddr = 0x0080;
                break;
            case REG_TYPE_SM:
                nOffSetAddr = 0x03e0;
                break;
            case REG_TYPE_S:
                nOffSetAddr = 0x0300;
                break;
            case REG_TYPE_T:
                nOffSetAddr = 0x0380;
                break;
            case REG_TYPE_C:
                nOffSetAddr = 0x03a0;
                break;
            default:
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }

            nTmpValue = mAddrInfo.mAddrProp.nAddrValue%8;
            nAddrValue = (((mAddrInfo.mAddrProp.nAddrValue/8 + nOffSetAddr) << 3) + nTmpValue) & 0xffff;

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x03);//结束符

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 10;
            bSuccess = true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            mCmnProp.eSendType = CMN_READ_BEFORE_WRITE;
            return makePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_32CV:
        {
            mSendPackge.sendDataList.push_back(0x02);//开头字符STX

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x36);//命令号码61
            mSendPackge.sendDataList.push_back(0x31);

            switch (eRegType)
            {
            case REG_TYPE_D:
            case REG_TYPE_D_BIT:
                nOffSetAddr = 0x1c00;
                break;
            case REG_TYPE_SD:
                nOffSetAddr = 0x1600;
                break;
            case REG_TYPE_TV:
                nOffSetAddr = 0x1400;
                break;
            case REG_TYPE_CV:
                nOffSetAddr = 0x1800;
                break;
            case REG_TYPE_32CV:
                nOffSetAddr = 0x1a00;
                break;
            default:
                mSendPackge.sendDataList.clear();
                return false;
                break;
            }

            nAddrValue = mAddrInfo.mAddrProp.nAddrValue * 2 + nOffSetAddr;

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            nAddrLen = mAddrInfo.mAddrProp.nAddrLen * 2;
            uctmp1 = (nAddrLen >> 4) & 0xf;
            uctmp2 = (nAddrLen >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));

            mAddrInfo.nValueList.resize(nAddrLen);

            for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)
            {
                uctmp1 = mAddrInfo.nValueList.at(i*2 + 0);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                uctmp1 = mAddrInfo.nValueList.at(i*2 + 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            mSendPackge.sendDataList.push_back(0x03);//结束符

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1) & 0xff;//计算校验码

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 10;
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
    vector<ADDR_TARGET_PROP > mTmpListBit6;

    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;

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
        case REG_TYPE_SM:
            {
                mTmpListBit3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_S:
            {
                mTmpListBit4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_T:
            {
                mTmpListBit5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit6.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_D_BIT:
            {
                /*起始位置为所在字的起始位置*/
                mTmpAddr.nRegIndex = REG_TYPE_D;

                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = mTmpAddr.nAddrValue/16;

                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_D:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SD:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TV:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CV:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_32CV:
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
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxRWlen, 8);
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
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 8);
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
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, nMaxRWlen, 8);
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
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, nMaxRWlen, 8);
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
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, nMaxRWlen, 8);
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
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, nMaxRWlen, 8);
    }
    tmpSize = mTmpListBit5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit5.at(i));
    }

    mTmpListBit6 = PlcMathMethod::addrBubbleSort(mTmpListBit6);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit6 = PlcMathMethod::addrSortOut(mTmpListBit6, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit6 = PlcMathMethod::addrSortOut(mTmpListBit6, nMaxRWlen, 8);
    }
    tmpSize = mTmpListBit6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit6.at(i));
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
    mSerialInfo.BAUD_RATE = 19200;
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
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 9000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 160000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen/2 > ADDR_MAX_VALUE_256)
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
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 9000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 160000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen/2 > ADDR_MAX_VALUE_256)
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
            eRegType = REG_TYPE_SM;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_S;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_D_BIT;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_SD;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_CV;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_32CV;
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
    else if((DealWithString::compare("SM", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("D_BIT", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("SD", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("32CV", sRegName)))
    {
        nRegIndex = 12;
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
