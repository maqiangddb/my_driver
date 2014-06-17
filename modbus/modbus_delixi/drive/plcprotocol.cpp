#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*写数据返回的长度*/
#define WRITE_RETURN_LEN      8

/*读数据返回的数据头部长度*/
#define READ_HEAD_LEN         9

/*读数据返回的数据校验的长度*/
#define CHECK_LEN             4

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
    case REG_TYPE_2P:                   //:
        {
            int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            if(nIntValue < 1 || nIntValue > 4)
            {
                return false;
            }

            mAddrTarget.nAddrValue = nIntValue;
            break;
        }
    case REG_TYPE_3P:
        {
            int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            if(nIntValue != 0)
            {
                return false;
            }

            mAddrTarget.nAddrValue = nIntValue;
            break;
        }
    case REG_TYPE_4P:
        {
            int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            if(nIntValue != 1 && nIntValue != 7)
            {
                return false;
            }

            mAddrTarget.nAddrValue = nIntValue;
            break;
        }
    case REG_TYPE_6P:
        {
            int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            if(nIntValue != 2)
            {
                return false;
            }

            mAddrTarget.nAddrValue = nIntValue;
            break;
        }
    case REG_TYPE_7P:
        {
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
                nIntValue = nIntValue * 100;
                if(nIntValue != 500 || nDecValue < 0 || nDecValue > 31)
                {
                    return false;
                }

                /*整合后的结果*/
                mAddrTarget.nAddrValue = nIntValue + nDecValue;
                return true;
            }
            else
            {
                return false;
            }
            break;
        }
    default:
        {
            return bSuccess;
            break;
        }
    }

    return bSuccess;
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
            break;
        }
    case WORT_ADDR_REG:
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("2P");
            sRegNameList.push_back("3P");
            sRegNameList.push_back("4P");
            sRegNameList.push_back("6P");
            sRegNameList.push_back("7P");
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
            sRegNameList.push_back("2P");
            sRegNameList.push_back("3P");
            sRegNameList.push_back("4P");
            sRegNameList.push_back("6P");
            sRegNameList.push_back("7P");
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
    case REG_TYPE_2P:                   //:
    case REG_TYPE_3P:                   //:
    case REG_TYPE_4P:                   //:
    case REG_TYPE_6P:                   //:
    case REG_TYPE_7P:                   //:
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
    case REG_TYPE_2P:                   //REG_TYPE_2P:
        {
            int nIntValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return ILLEGAL_VALUE;
            }

            if(nIntValue < 1 || nIntValue > 4)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                if(nIntValue + nInterval > 4)
                {
                    break;
                }
                else
                {
                    nIntValue += nInterval;
                }

                /*添加整数部分*/
                memset(buf, 0, 10);
                sprintf(buf, "%d", nIntValue);
                sTmpStr = buf;
                mTmpAddr.sAddrValue = sTmpStr;

                mSourceAddrList.push_back(mTmpAddr);
            }
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_3P:                   //:
        {
//            int nIntValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
//            if(bSuccess == false)
//            {
//                return ILLEGAL_VALUE;
//            }

//            if(nIntValue < 1 || nIntValue > 5)
//            {
//                return ILLEGAL_VALUE;
//            }

//            char buf[10] = {0};
//            string sTmpStr;
//            for(int i = 0; i < nCopyNum; i++)
//            {
//                if(nIntValue + nInterval > 5)
//                {
//                    break;
//                }
//                else
//                {
//                    nIntValue += nInterval;
//                }

//                /*添加整数部分*/
//                memset(buf, 0, 10);
//                sprintf(buf, "%d", nIntValue);
//                sTmpStr = buf;
//                mTmpAddr.sAddrValue = sTmpStr;

//                mSourceAddrList.push_back(mTmpAddr);
//            }
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_4P:                   //REG_TYPE_4P:
        {
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_6P:                   //:
        {
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_7P:                   //:
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
                nIntValue = nIntValue * 100;
                if(nIntValue != 500 || nDecValue < 0 || nDecValue > 31)
                {
                    return ILLEGAL_VALUE;
                }

                nIntValue = nIntValue + nDecValue;

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nIntValue + nInterval > 531)
                    {
                        break;
                    }
                    else
                    {
                        nIntValue += nInterval;
                    }

                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nIntValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                return ADDR_CORRECT;
                break;
            }
            else
            {
                return ADDR_FORMAT_ERROR;
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

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mUnpackProp.mCurrAddr.mAddrProp.nRegIndex);
    int nStart = READ_HEAD_LEN;
    int nEnd = CHECK_LEN;
    if(REG_TYPE_3P == eRegType)
    {
        nStart = 5;
        nEnd = 8;
    }

    if(mUnpackProp.bNeedSetData)
    {
        /*取有效数据*/
        int nSize = rcvList.size();
        targetList.clear();
        for(int i = nStart; i < nSize - nEnd; i++)
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
    if(nRcvSize < 21)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }
    if(rcvList.at(0) != 0x3A || rcvList.at(nRcvSize -1) != 0x0A
       || rcvList.at(nRcvSize -2) != 0x0D)
    {
        return eCmnInfo;
    }

    vector<unsigned char> chekSum;
    chekSum.clear();
    unsigned char temp1;
    unsigned char temp2;
    unsigned char nCheckResult;
    for(int i = 1; i + 1 < nRcvSize - 4;)
    {
        temp1 = PlcMathMethod::asciiToIntValue(rcvList.at(i));
        temp1 = (temp1 << 4) & 0xF0;

        temp2 = PlcMathMethod::asciiToIntValue(rcvList.at(i + 1));
        temp2 = temp2 & 0x0F;

        temp1 = (temp1 | temp2) & 0xFF;
        chekSum.push_back(temp1);
        i = i + 2;
    }

    temp1 = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 4));
    temp1 = (temp1 << 4) & 0xF0;

    temp2 = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 3));
    temp2 = temp2 & 0x0F;

    nCheckResult = (temp1 | temp2) & 0xFF;

    unsigned short nSumCheck = PlcMathMethod::getSumCheck(chekSum);
    nSumCheck = nSumCheck & 0xFF;

    if(nCheckResult == nSumCheck)
    {
        /*设置数据*/
        mUnpackProp.bNeedSetData = true;
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else
    {
        /*设置数据*/
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
    bool bSuccess = false;
    ADDR_TARGET_PROP mAddrInfo = mCmnProp.mAddrInfo.mAddrProp;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    uchar nFumNum = 0;
    ushort nAddrValue = 0;
    int nAddrLen = 1;
    int nReturnLen = 1;
    uchar nTmpValue = 0;
    vector<unsigned char> checkSum;
    checkSum.clear();
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    mSendPackge.sendDataList.clear();
    mSendPackge.sendDataList.push_back(0x3A);//开始字元

    /*站号 vector index = 1*/
    nTmpValue = ((mAddrInfo.nPlcStationIndex >> 4) & 0x0F);
    nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
    mSendPackge.sendDataList.push_back(nTmpValue);

    nTmpValue = (mAddrInfo.nPlcStationIndex & 0x0F);
    nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
    mSendPackge.sendDataList.push_back(nTmpValue);

    checkSum.push_back(mAddrInfo.nPlcStationIndex);

    switch(eRegType)
    {
    case REG_TYPE_3P:
    case REG_TYPE_7P:
        {
            if(eRegType == REG_TYPE_7P)
            {
                //功能码
                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x37);
                checkSum.push_back(0x07);
            }
            else
            {
                //功能码
                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x33);
                checkSum.push_back(0x03);
            }

            //地址做和校验的字符串
            checkSum.push_back((mAddrInfo.nAddrValue >> 8) & 0xFF);
            checkSum.push_back((mAddrInfo.nAddrValue) & 0xFF);

            //值做和校验的字符串
            unsigned int num = 0;
            checkSum.push_back((num >> 24) & 0xFF);

            checkSum.push_back((num >> 16) & 0xFF);

            checkSum.push_back((num >> 8) & 0xFF);

            checkSum.push_back((num >> 0) & 0xFF);

            /*地址 vector index = 1*/
            nTmpValue = ((mAddrInfo.nAddrValue >> 12) & 0x0F);
            //checkSum.push_back(nTmpValue);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((mAddrInfo.nAddrValue >> 8) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((mAddrInfo.nAddrValue >> 4) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.nAddrValue & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*发送数据包值 index = 1*/
            nTmpValue = ((num >> 28) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 24) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 20) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 16) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 12) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 8) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 4) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (num & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            ushort nCheckSUMResult = PlcMathMethod::getSumCheck(checkSum);
            nTmpValue = (nCheckSUMResult >> 4) & 0x0F;
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nCheckSUMResult >> 0) & 0x0F;
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x0D);

            mSendPackge.sendDataList.push_back(0x0A);
            mSendPackge.nReturnLen = 21;

            bSuccess = true;
            break;
        }
    default:
        {
            bSuccess = false;
            break;
        }
    }


    return bSuccess;
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
    vector<unsigned char> checkSum;
    checkSum.clear();
    int rnLen = WRITE_RETURN_LEN;

    uchar nFumNum = 0;
    bool bSuccess = false;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);

    mSendPackge.sendDataList.push_back(0x3A);//开始字元

    /*站号 vector index = 1*/
    nTmpValue = ((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0x0F);
    nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
    mSendPackge.sendDataList.push_back(nTmpValue);

    nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex & 0x0F);
    nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
    mSendPackge.sendDataList.push_back(nTmpValue);

    checkSum.push_back(mAddrInfo.mAddrProp.nPlcStationIndex);

    switch(eRegType)
    {
    case REG_TYPE_2P:
    case REG_TYPE_4P:
    case REG_TYPE_6P:
        {
            //功能码
            if(REG_TYPE_2P == eRegType)
            {
                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x32);
                checkSum.push_back(0x02);
            }
            else if(REG_TYPE_4P == eRegType)
            {
                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x34);
                checkSum.push_back(0x04);
            }
            else
            {
                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x36);
                checkSum.push_back(0x06);
            }

            //地址做和校验的字符串
            checkSum.push_back((mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xFF);
            checkSum.push_back((mAddrInfo.mAddrProp.nAddrValue) & 0xFF);

            //值做和校验的字符串
            unsigned int num = 0;
            unsigned int temp = 0;
            int i = 0;
            if(mAddrInfo.nValueList.size() == 4)
            {
                temp = mAddrInfo.nValueList.at(4*i + 0);
                num = temp & 0xFF;

                temp = mAddrInfo.nValueList.at(4*i + 1);
                num = (temp << 8) | num;

                temp = mAddrInfo.nValueList.at(4*i + 2);
                num = (temp << 16) | num;

                temp = mAddrInfo.nValueList.at(4*i + 3);
                num = (temp << 24) | num;

//                if(REG_TYPE_4P == eRegType ||REG_TYPE_6P == eRegType)
//                {
//                    float nValue = 0;
//                    memcpy(&nValue,&num,4);
//                    num = nValue * 100;
//                }
            }
            checkSum.push_back((num >> 24) & 0xFF);

            checkSum.push_back((num >> 16) & 0xFF);

            checkSum.push_back((num >> 8) & 0xFF);

            checkSum.push_back((num >> 0) & 0xFF);

            /*地址 vector index = 1*/
            nTmpValue = ((mAddrInfo.mAddrProp.nAddrValue >> 12) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((mAddrInfo.mAddrProp.nAddrValue >> 8) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((mAddrInfo.mAddrProp.nAddrValue >> 4) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*发送数据包值 index = 1*/
            nTmpValue = ((num >> 28) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 24) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 20) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 16) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 12) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 8) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = ((num >> 4) & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (num & 0x0F);
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            ushort nCheckSUMResult = PlcMathMethod::getSumCheck(checkSum);
            nTmpValue = (nCheckSUMResult >> 4) & 0x0F;
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nCheckSUMResult >> 0) & 0x0F;
            nTmpValue = PlcMathMethod::intToAsciiValue(nTmpValue);
            mSendPackge.sendDataList.push_back(nTmpValue);


            mSendPackge.sendDataList.push_back(0x0D);

            mSendPackge.sendDataList.push_back(0x0A);

            mSendPackge.nReturnLen = 21;
            bSuccess = true;
            break;
        }
    default:
        {
            break;
        }
    }

    /*添加返回数据*/
    return bSuccess;
}

/******************************************************************
 * Function: merge global address to m_globalReadPlcAddrList
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    targetList = mSourceAddrList;
    return true;
    targetList.clear();

    /*按站号分，放在一个map中，站号为key*/
    int nTotalSize = mSourceAddrList.size();

//    /*超过最大读写长度 则等于最大读写长度*/
//    if(nMaxRWlen > MAX_RW_LEN)
//    {
//        nMaxRWlen = MAX_RW_LEN;
//    }

//    ADDR_TARGET_PROP mTmpAddr;

//    /*按寄存器号整理*/
//    vector<ADDR_TARGET_PROP > mTmpListBit0;
//    vector<ADDR_TARGET_PROP > mTmpListBit1;
//    vector<ADDR_TARGET_PROP > mTmpListWord2;
//    vector<ADDR_TARGET_PROP > mTmpListWord3;

//    for(int i = 0; i < nTotalSize; i++)
//    {
//        mTmpAddr = mSourceAddrList.at(i);

//        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
//        switch(eRegIndex)
//        {
//        case REG_TYPE_0X:
//            {
//                mTmpListBit0.push_back(mTmpAddr);
//                break;
//            }
//        case REG_TYPE_1X:
//            {
//                mTmpListBit1.push_back(mTmpAddr);
//                break;
//            }
//        case REG_TYPE_3X_BIT:
//            {
//                /**/
//                mTmpAddr.nRegIndex = REG_TYPE_P3;

//                /*起始位置为所在字的起始位置*/
//                int tmpStartValue = mTmpAddr.nAddrValue;
//                mTmpAddr.nAddrValue = mTmpAddr.nAddrValue/16;

//                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
//                {
//                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
//                }
//                else
//                {
//                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
//                }

//                mTmpListWord2.push_back(mTmpAddr);
//                break;
//            }
//        case REG_TYPE_4X_BIT:
//            {
//                mTmpAddr.nRegIndex = REG_TYPE_4X;

//                /*起始位置为所在字的起始位置*/
//                int tmpStartValue = mTmpAddr.nAddrValue;
//                mTmpAddr.nAddrValue = mTmpAddr.nAddrValue/16;

//                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
//                {
//                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
//                }
//                else
//                {
//                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
//                }

//                mTmpListWord3.push_back(mTmpAddr);
//                break;
//            }
//        case REG_TYPE_P3:
//        case REG_TYPE_3X_DB:
//            {
//                mTmpListWord2.push_back(mTmpAddr);
//                break;
//            }
//        case REG_TYPE_4X:
//        case REG_TYPE_4X_DB:
//            {
//                mTmpListWord3.push_back(mTmpAddr);
//                break;
//            }
//        default:
//            {
//                break;
//            }
//        }
//    }//end for

//    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
//    mTmpListBit0 = PlcMathMethod::addrBubbleSort(mTmpListBit0);
//    if(bWriteAddr)
//    {
//        /*写地址，则只支持长度为1的写*/
//        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, 1, 1);
//    }
//    else
//    {
//        /*读地址，可支持多个长度*/
//        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxRWlen, 8);
//    }

//    int tmpSize = mTmpListBit0.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListBit0.at(i));
//    }

//    mTmpListBit1 = PlcMathMethod::addrBubbleSort(mTmpListBit1);
//    mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 8);
//    tmpSize = mTmpListBit1.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListBit1.at(i));
//    }

//    mTmpListWord2 = PlcMathMethod::addrBubbleSort(mTmpListWord2);
//    mTmpListWord2 = PlcMathMethod::addrSortOut(mTmpListWord2, nMaxRWlen/2, 1);
//    tmpSize = mTmpListWord2.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListWord2.at(i));
//    }

//    mTmpListWord3 = PlcMathMethod::addrBubbleSort(mTmpListWord3);
//    mTmpListWord3 = PlcMathMethod::addrSortOut(mTmpListWord3, nMaxRWlen/2, 1);
//    tmpSize = mTmpListWord3.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListWord3.at(i));
//    }

//    return true;
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
    int nIntervalTime = 20;
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
    case REG_TYPE_3P:
        {
            if(mAddr.nAddrValue != 0)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_7P:
        {
            if(mAddr.nAddrValue < 500 || mAddr.nAddrValue  > 531)
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
    case REG_TYPE_2P:
        {
            if(mAddr.nAddrValue < 1 || mAddr.nAddrValue  >  4)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_4P:
        {
            if(mAddr.nAddrValue != 1 && mAddr.nAddrValue  !=  7)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_6P:
        {
            if(mAddr.nAddrValue != 2)
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
            eRegType = REG_TYPE_2P;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_3P;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_4P;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_6P;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_7P;
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

    if(DealWithString::compare("2P", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("3P", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("4P", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("6P", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("7P", sRegName)))
    {
        nRegIndex = 4;
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
    return NET_SERIAL_PORT;
}


