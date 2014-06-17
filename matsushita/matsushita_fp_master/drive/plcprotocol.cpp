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
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            if (mAddrSource.sAddrValue.size() == 1)
            {
                /*整合后的结果*/
                mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);
                if(bSuccess == false)
                {
                    return false;
                }
            }
            else
            {
                string IntPass = mAddrSource.sAddrValue;
                IntPass.resize(mAddrSource.sAddrValue.size() - 1);

                char cDecPass = mAddrSource.sAddrValue.at(mAddrSource.sAddrValue.size() - 1);
                string DecPass;
                DecPass.clear();
                DecPass.push_back(cDecPass);

                int nIntValue = DealWithString::stringToInt(IntPass, bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                int nDecValue = DealWithString::stringToInt(DecPass,bSuccess,16);
                if(bSuccess == false)
                {
                    return false;
                }

                /*整合后的结果*/
                mAddrTarget.nAddrValue = nIntValue*16 + nDecValue;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 160000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_WX:
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
    case REG_TYPE_LD:
    case REG_TYPE_SV:
    case REG_TYPE_EV:
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
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("R");
            sRegNameList.push_back("L");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("WX");
            sRegNameList.push_back("WY");
            sRegNameList.push_back("WR");
            sRegNameList.push_back("WL");
            sRegNameList.push_back("DT");
            sRegNameList.push_back("LD");
            sRegNameList.push_back("SV");
            sRegNameList.push_back("EV");
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
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("R");
            sRegNameList.push_back("L");
            sRegNameList.push_back("WX");
            sRegNameList.push_back("WY");
            sRegNameList.push_back("WR");
            sRegNameList.push_back("WL");
            sRegNameList.push_back("DT");
            sRegNameList.push_back("LD");
            sRegNameList.push_back("SV");
            sRegNameList.push_back("EV");
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
    case REG_TYPE_R:
    case REG_TYPE_L:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_WX:
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
    case REG_TYPE_LD:
    case REG_TYPE_SV:
    case REG_TYPE_EV:
    case REG_TYPE_DT:
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
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            int nIntValue = 0;
            int nDecValue = 0;

            if (mStartAddr.sAddrValue.size() == 1)
            {
                /*整合后的结果*/
                nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess,16);
                if(!bSuccess)
                {
                    return ILLEGAL_VALUE;
                }
            }
            else
            {
                string IntPass = mStartAddr.sAddrValue;
                IntPass.resize(mStartAddr.sAddrValue.size() - 1);

                char cDecPass = mStartAddr.sAddrValue.at(mStartAddr.sAddrValue.size() - 1);
                string DecPass;
                DecPass.clear();
                DecPass.push_back(cDecPass);

                nIntValue = DealWithString::stringToInt(IntPass, bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                nDecValue = DealWithString::stringToInt(DecPass,bSuccess,16);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
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
                    mTmpAddr.sAddrValue = "";

                    if(nIntValue != 0)
                    {
                        /*添加整数部分*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }

                    /*添加小数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nDecValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue += sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_WX:
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
    case REG_TYPE_LD:
    case REG_TYPE_SV:
    case REG_TYPE_EV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_10000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_10000)
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
    case REG_TYPE_DT:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_100000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_100000)
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
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    if(nRcvSize < 6)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = rcvList.at(nRcvSize - 2);
    uchar nRcvHighByte = rcvList.at(nRcvSize - 3);

    vector<unsigned char> tmprcvList;
    for (int i=0; i<(nRcvSize - 3); i++)
    {
        tmprcvList.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getXORcheck(tmprcvList) & 0xff;

    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
        /*取有效数据*/
        targetList.clear();
        for(int i = 6; i < nRcvSize - 3; i++)
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
    if(nRcvSize < 6)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = rcvList.at(nRcvSize - 2);
    uchar nRcvHighByte = rcvList.at(nRcvSize - 3);

    vector<unsigned char> tmprcvList;
    for (int i=0; i<(nRcvSize - 3); i++)
    {
        tmprcvList.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getXORcheck(tmprcvList) & 0xff;

    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
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
    int nEndAddr = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    switch(eRegType)
    {
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5 6 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x52);
            mSendPackge.sendDataList.push_back(0x43);
            mSendPackge.sendDataList.push_back(0x43);

            /*寄存器代号 index = 7 */
            if (eRegType == REG_TYPE_T)
            {
                mSendPackge.sendDataList.push_back(0x54);
            }
            else if (eRegType == REG_TYPE_C)
            {
                mSendPackge.sendDataList.push_back(0x43);
            }
            else if (eRegType == REG_TYPE_X)
            {
                mSendPackge.sendDataList.push_back(0x58);
            }
            else if (eRegType == REG_TYPE_Y)
            {
                mSendPackge.sendDataList.push_back(0x59);
            }
            else if (eRegType == REG_TYPE_R)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_L)
            {
                mSendPackge.sendDataList.push_back(0x4c);
            }

            /*开始地址 index = 8 9 10 11 */
            nAddrValue = (mAddrInfo.nAddrValue/16);
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 12 13 14 15 */
            if(mAddrInfo.nAddrLen%16 == 0)
            {
                nAddrLen = mAddrInfo.nAddrLen/16;
            }
            else
            {
                nAddrLen = mAddrInfo.nAddrLen/16 + 1;
            }
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*校验 index = 16 17 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 18 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_WX:
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5 6 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x52);
            mSendPackge.sendDataList.push_back(0x43);
            mSendPackge.sendDataList.push_back(0x43);

            /*寄存器代号 index = 7 */
            if (eRegType == REG_TYPE_WX)
            {
                mSendPackge.sendDataList.push_back(0x58);
            }
            else if (eRegType == REG_TYPE_WY)
            {
                mSendPackge.sendDataList.push_back(0x59);
            }
            else if (eRegType == REG_TYPE_WR)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_WL)
            {
                mSendPackge.sendDataList.push_back(0x4c);
            }

            /*开始地址 index = 8 9 10 11 */
            nAddrValue = mAddrInfo.nAddrValue;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 12 13 14 15 */
            nAddrLen = mAddrInfo.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*校验 index = 16 17 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 18 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_SV:
    case REG_TYPE_EV:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x52);

            /*寄存器代号 index = 5 */
            if (eRegType == REG_TYPE_SV)
            {
                mSendPackge.sendDataList.push_back(0x53);
            }
            else if (eRegType == REG_TYPE_EV)
            {
                mSendPackge.sendDataList.push_back(0x4b);
            }

            /*开始地址 index = 6 7 8 9 */
            nAddrValue = mAddrInfo.nAddrValue;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 10 11 12 13 */
            nAddrLen = mAddrInfo.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*校验 index = 14 15 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 16 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
            break;
        }
    case REG_TYPE_DT:
    case REG_TYPE_LD:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x52);
            mSendPackge.sendDataList.push_back(0x44);

            /*寄存器代号 index = 6 */
            if (eRegType == REG_TYPE_DT)
            {
                mSendPackge.sendDataList.push_back(0x44);
            }
            else if (eRegType == REG_TYPE_LD)
            {
                mSendPackge.sendDataList.push_back(0x4C);
            }

            /*开始地址 index = 7 8 9 10 11 */
            nAddrValue = mAddrInfo.nAddrValue;

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/10000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%10000)/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 12 13 14 15 16 */
            nAddrLen = mAddrInfo.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/10000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%10000)/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*校验 index = 17 18 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 19 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9 + nAddrLen*4;
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
    int nEndAddr = 0;
    int nAddrValue = 0;
    int nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5 6 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x57);
            mSendPackge.sendDataList.push_back(0x43);
            mSendPackge.sendDataList.push_back(0x53);

            /*寄存器代号 index = 7 */
            if (eRegType == REG_TYPE_Y)
            {
                mSendPackge.sendDataList.push_back(0x59);
            }
            else if (eRegType == REG_TYPE_R)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_L)
            {
                mSendPackge.sendDataList.push_back(0x4c);
            }

            /*开始地址 index = 8 9 10 11 */
            nAddrValue = (mAddrInfo.mAddrProp.nAddrValue/16);
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(mAddrInfo.mAddrProp.nAddrValue%16);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*赋值 index = 12 */
            nTmpValue = mAddrInfo.nValueList.front();
            if(nTmpValue == 1)
            {
                /*SET*/
                mSendPackge.sendDataList.push_back(0x31);
            }
            else
            {
                /*RESET*/
                mSendPackge.sendDataList.push_back(0x30);
            }

            /*校验 index = 13 14 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 15 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;

            bSuccess = true;
            break;
        }
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5 6 */
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x57);
            mSendPackge.sendDataList.push_back(0x43);
            mSendPackge.sendDataList.push_back(0x43);

            /*寄存器代号 index = 7 */
            if (eRegType == REG_TYPE_WY)
            {
                mSendPackge.sendDataList.push_back(0x59);
            }
            else if (eRegType == REG_TYPE_WR)
            {
                mSendPackge.sendDataList.push_back(0x52);
            }
            else if (eRegType == REG_TYPE_WL)
            {
                mSendPackge.sendDataList.push_back(0x4c);
            }

            /*开始地址 index = 8 9 10 11 */
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 12 13 14 15 */
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*赋值  index = 16 17 18 19... */
            for (int i=0; i<nAddrLen; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if ((i*2 + 1) < size)
                {
                    uchar cValue = mAddrInfo.nValueList.at(i*2 + 0);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    cValue = mAddrInfo.nValueList.at(i*2 + 1);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }

            /*校验 index = 16+len*4 17+len*4 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 18+len*4 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;

            bSuccess = true;
            break;
        }
    case REG_TYPE_DT:
    case REG_TYPE_LD:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4 5*/
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x57);
            mSendPackge.sendDataList.push_back(0x44);

            /*寄存器代号 index = 6 */
            if (eRegType == REG_TYPE_DT)
            {
                mSendPackge.sendDataList.push_back(0x44);
            }
            else if (eRegType == REG_TYPE_LD)
            {
                mSendPackge.sendDataList.push_back(0x4C);
            }

            /*开始地址 index = 7 8 9 10 11 */
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/10000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%10000)/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 12 13 14 15 16*/
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/10000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%10000)/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*赋值  index = 17 18 19 20... */
            for (int i=0; i<nAddrLen; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if ((i*2 + 1) < size)
                {
                    uchar cValue = mAddrInfo.nValueList.at(i*2 + 0);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    cValue = mAddrInfo.nValueList.at(i*2 + 1);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }

            /*校验 index = 17+len*4 18+len*4 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 19+len*4 */
            mSendPackge.sendDataList.push_back(0x0d);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 9;

            bSuccess = true;
            break;
        }
    case REG_TYPE_SV:
    case REG_TYPE_EV:
        {
            /*开始符固定 index = 0 */
            uchar nTmpValue = 0x25;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 index = 1 2 */
            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*固定 index = 3 4*/
            mSendPackge.sendDataList.push_back(0x23);
            mSendPackge.sendDataList.push_back(0x57);

            /*寄存器代号 index = 5 */
            if (eRegType == REG_TYPE_SV)
            {
                mSendPackge.sendDataList.push_back(0x53);
            }
            else if (eRegType == REG_TYPE_EV)
            {
                mSendPackge.sendDataList.push_back(0x4B);
            }

            /*开始地址 index = 6 7 8 9 */
            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nAddrValue%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*结束地址 index = 10 11 12 13 */
            nAddrLen = mAddrInfo.mAddrProp.nAddrLen;
            nEndAddr = nAddrValue + nAddrLen - 1;

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr/1000) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%1000)/100) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(((nEndAddr%100)/10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((nEndAddr%10) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*赋值  index = 14 15 16 17... */
            for (int i=0; i<nAddrLen; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if ((i*2 + 1) < size)
                {
                    uchar cValue = mAddrInfo.nValueList.at(i*2 + 0);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    cValue = mAddrInfo.nValueList.at(i*2 + 1);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = PlcMathMethod::intToAsciiValue((cValue >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }

            /*校验 index = 14+len*4 15+len*4 */
            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符  index = 16+len*4 */
            mSendPackge.sendDataList.push_back(0x0d);

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
    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;
    vector<ADDR_TARGET_PROP > mTmpListWord5;
    vector<ADDR_TARGET_PROP > mTmpListWord6;
    vector<ADDR_TARGET_PROP > mTmpListWord7;
    vector<ADDR_TARGET_PROP > mTmpListWord8;
    vector<ADDR_TARGET_PROP > mTmpListWord9;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_T:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_X:
        case REG_TYPE_WX:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Y:
        case REG_TYPE_WY:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_R:
        case REG_TYPE_WR:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_L:
        case REG_TYPE_WL:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DT:
            {
                mTmpListWord6.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_LD:
            {
                mTmpListWord7.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SV:
            {
                mTmpListWord8.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_EV:
            {
                mTmpListWord9.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListWord0 = PlcMathMethod::addrBubbleSort(mTmpListWord0);
    mTmpListWord0 = PlcMathMethod::addrSortOut(mTmpListWord0, nMaxRWlen/2, 1);
    int tmpSize = mTmpListWord0.size();
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
    for(int i = tmpSize - 1; i > -1; i--)
    {
        targetList.push_back(mTmpListWord4.at(i));
    }

    mTmpListWord5 = PlcMathMethod::addrBubbleSort(mTmpListWord5);
    mTmpListWord5 = PlcMathMethod::addrSortOut(mTmpListWord5, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord5.at(i));
    }

    mTmpListWord6 = PlcMathMethod::addrBubbleSort(mTmpListWord6);
    mTmpListWord6 = PlcMathMethod::addrSortOut(mTmpListWord6, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord6.at(i));
    }

    mTmpListWord7 = PlcMathMethod::addrBubbleSort(mTmpListWord7);
    mTmpListWord7 = PlcMathMethod::addrSortOut(mTmpListWord7, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord7.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord7.at(i));
    }

    mTmpListWord8 = PlcMathMethod::addrBubbleSort(mTmpListWord8);
    mTmpListWord8 = PlcMathMethod::addrSortOut(mTmpListWord8, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord8.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord8.at(i));
    }

    mTmpListWord9 = PlcMathMethod::addrBubbleSort(mTmpListWord9);
    mTmpListWord9 = PlcMathMethod::addrSortOut(mTmpListWord9, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord9.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord9.at(i));
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
    int nMaxRWlen = 48;
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 160000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_WX:
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
    case REG_TYPE_LD:
    case REG_TYPE_SV:
    case REG_TYPE_EV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_DT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_100000)
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
    case REG_TYPE_R:
    case REG_TYPE_L:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 160000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_WY:
    case REG_TYPE_WR:
    case REG_TYPE_WL:
    case REG_TYPE_LD:
    case REG_TYPE_SV:
    case REG_TYPE_EV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_DT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_100000)
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
            eRegType = REG_TYPE_T;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_X;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_Y;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_R;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_L;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_WX;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_WY;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_WR;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_WL;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_DT;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_LD;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_SV;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_EV;
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

    if(DealWithString::compare("T", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("X", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("Y", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("R", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("L", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("WX", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("WY", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("WR", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("WL", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("DT", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("LD", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("SV", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("EV", sRegName)))
    {
        nRegIndex = 13;
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

