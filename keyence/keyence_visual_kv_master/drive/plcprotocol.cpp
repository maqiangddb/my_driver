#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*写数据返回的长度*/
#define WRITE_RETURN_LEN      8

/*读数据返回的数据头部长度*/
#define READ_HEAD_LEN         3

/*读数据返回的数据校验的长度*/
#define CHECK_LEN             0

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
    case REG_TYPE_RLY:                      //REG_TYPE_RLY:
        {
            if (mAddrSource.sAddrValue.size() <= 2)
            {
                int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
                if(bSuccess == false || nIntValue > 15 || nIntValue < 0)
                {
                    return false;
                }
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_16000)//两位数字肯定不会有这么大
                {
                    return false;
                }
                return true;
            }
            else
            {
                string IntPass = DealWithString::mid(mAddrSource.sAddrValue,0,mAddrSource.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mAddrSource.sAddrValue,mAddrSource.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                int IntTemp = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }
                int DecTemp = DealWithString::stringToInt(DecPass,bSuccess);
                if(DecTemp >= 16)
                {
                    return false;
                }
                int nIntValue = IntTemp*16 + DecTemp;
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_16000)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TVC:
    case REG_TYPE_TVP:
    case REG_TYPE_CVC:
    case REG_TYPE_CVP:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
        }
        break;
    case REG_TYPE_DM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_TM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_100)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_CTH:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10)
            {
                return false;
            }
            return true;
        }
    default:
        {
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
            sRegNameList.push_back("RLY");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("DM");
            sRegNameList.push_back("TM");
            sRegNameList.push_back("TVC");
            sRegNameList.push_back("TVP");
            sRegNameList.push_back("CVC");
            sRegNameList.push_back("CVP");
            sRegNameList.push_back("CTH");
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
            sRegNameList.push_back("RLY");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("DM");
            sRegNameList.push_back("TM");
            sRegNameList.push_back("TVC");
            sRegNameList.push_back("TVP");
            sRegNameList.push_back("CVC");
            sRegNameList.push_back("CVP");
            sRegNameList.push_back("CTH");
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
    case REG_TYPE_RLY:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_DM:
    case REG_TYPE_TM:
    case REG_TYPE_TVC:
    case REG_TYPE_TVP:
    case REG_TYPE_CVC:
    case REG_TYPE_CVP:
    case REG_TYPE_CTH:
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
    case REG_TYPE_RLY:                   //REG_TYPE_RLY:
        {
            int nIntValue = 0;
            int nDecValue = 0;
            if (mStartAddr.sAddrValue.size() <= 2)
            {
                nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nIntValue = 0;
            }
            else
            {
                string IntPass = DealWithString::mid(mStartAddr.sAddrValue,0,mStartAddr.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mStartAddr.sAddrValue,mStartAddr.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                nIntValue = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nDecValue = DealWithString::stringToInt(DecPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
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
                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_16000)
                {
                    /*添加整数部分*/
                    if(nIntValue > 0)
                    {
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }
                    else
                    {
                        mTmpAddr.sAddrValue = "";
                    }

                    /*添加小数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nDecValue);
                    if(nIntValue > 0 && nDecValue < 10)
                    {
                        sTmpStr = "0";
                        sTmpStr += buf;
                    }
                    else
                    {
                        sTmpStr = buf;
                    }
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
    case REG_TYPE_T:                    //REG_TYPE_T:
    case REG_TYPE_C:                    //REG_TYPE_1X:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

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
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_DM:                   //REG_TYPE_DM:
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
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_TM:                   //REG_TYPE_DM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_100)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_100)
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
    case REG_TYPE_TVC:                   //REG_TYPE_TVC:
    case REG_TYPE_TVP:
    case REG_TYPE_CVC:
    case REG_TYPE_CVP:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

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
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_10)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_10)
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
    ADDR_TARGET_PROP mAddrInfo = mUnpackProp.mCurrAddr.mAddrProp;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    if(mUnpackProp.bNeedSetData)
    {
        int nSize = rcvList.size();
        int nStart = 0; //有效数据开始地址
        int nValidLen = 0; //有效数据长度
        switch(eRegType)
        {
        case REG_TYPE_RLY:
        case REG_TYPE_DM:
        case REG_TYPE_TM:
            nValidLen = nSize - 2;
            break;
        case REG_TYPE_T:
        case REG_TYPE_C:
            nValidLen = 1;
            break;
        case REG_TYPE_TVC:
        case REG_TYPE_CVC:
            nStart = 2;
            nValidLen = 7;
            break;
        case REG_TYPE_TVP:
        case REG_TYPE_CVP:
            nStart = 8;
            nValidLen = 13;
            break;
        case REG_TYPE_CTH:
            nStart = 2;
            nValidLen = 7;
            break;
        default:
            break;
        }

        if(nValidLen > nSize - 2)
        {
            //LOGE("unpackReadData ERROR....");
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }
        /*取有效数据*/
        targetList.clear();
        for(int i = nStart; i <nValidLen; i++)
        {
            //LOGE("unpackReadData i = %d, value = %x",i,rcvList.at(i));
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

    if(CHECK_READ == eCheckType)//读校验
    {
        int nRcvSize = rcvList.size();
        if(nRcvSize < 2)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }
        if (rcvList.at(nRcvSize - 1) == 0x0a  &&  rcvList.at(nRcvSize - 2) == 0x0d)
        {
            mUnpackProp.bNeedSetData = true;
            eCmnInfo = CMN_NOMAL_CODE;
            return eCmnInfo;
        }
    }
    else if(CHECK_WRITE == eCheckType)//写校验
    {
        int nRcvSize = rcvList.size();
        if(nRcvSize < 4)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }

        if (rcvList.at(0) == 0x4f &&
            rcvList.at(1) == 0x4b &&
            rcvList.at(2) == 0x0d &&
            rcvList.at(3) == 0x0a)//返回O K CR LF是正确的
        {
            eCmnInfo = CMN_NOMAL_CODE;
        }
        else
        {
            eCmnInfo = DATA_CHECK_ERROR;
        }
        //LOGE("checkRcvData %d",eCmnInfo);
        return eCmnInfo;
    }
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
        mHandShake.sendDataList.clear();
        mHandShake.sendDataList.push_back(0x43);
        mHandShake.sendDataList.push_back(0x52);
        mHandShake.sendDataList.push_back(0x0d);
        mHandShake.sendDataList.push_back(0x0a);

        mHandShake.nReturnLen = 4;
        nCmnTimes = 1;
        return true;
    default:
        return false;
    }
}

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes: 次数从1开始
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    if (rcvList.size() == 4)//返回长度为11个字节
    {
        if (rcvList.at(0) == 0x43 &&
            rcvList.at(1) == 0x43 &&
            rcvList.at(2) == 0x0d &&
            rcvList.at(3) == 0x0a)//开始结束符正确
        {
            return CMN_NOMAL_CODE;
        }
    }

    return CMN_CONNECT_FAIL;
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

    //    if(nValueList.empty()) return mSendPackge;

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

    ushort nAddrValue = 0;
    int nReturnLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_RLY:
        {

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
            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

            iTemp1 = (mAddrInfo.nAddrValue/16)/100;
            iTemp2 = ((mAddrInfo.nAddrValue/16)%100)/10;
            iTemp3 = (mAddrInfo.nAddrValue/16)%10;

            iTemp4 = (mAddrInfo.nAddrValue%16)/10;
            iTemp5 = (mAddrInfo.nAddrValue%16)%10;
            /*站号 vector index = 0*/
            uchar nTmpValue = 0x52;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*对功能码赋值 vector index = 1*/
            nTmpValue = 0x44;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*读取长度*/
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 8*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 9*/
            nTmpValue = 0x0a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 3;//有效长度+固定字节长度
            return true;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
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
        char iTemp1,iTemp2,iTemp3;

        iTemp1 = mAddrInfo.nAddrValue/100;
        iTemp2 = (mAddrInfo.nAddrValue%100)/10;
        iTemp3 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(REG_TYPE_T == eRegType)
        {
            nTmpValue = 'T';
        }
        else
        {
            nTmpValue = 'C';
        }

        /*vector index = 3*/
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*读取长度*/
        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 8*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = nReturnLen + 14;

        return true;
        }
    case REG_TYPE_DM:
        {
        /*算起始地址，一次最小取一个byte*/
        nAddrValue = (mAddrInfo.nAddrValue/16)*16;

        /*算返回的字节长度*/
        if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
        }
        else
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
        }

        /*算取得地址长度*/
        char iTemp1,iTemp2,iTemp3,iTemp4;

        iTemp1 = mAddrInfo.nAddrValue/1000;
        iTemp2 = (mAddrInfo.nAddrValue%1000)/100;
        iTemp3 = (mAddrInfo.nAddrValue%100)/10;
        iTemp4 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 3*/
        nTmpValue = 'D';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 4*/
        nTmpValue = 'M';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*读取长度*/
        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 8*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 9*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 10*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 7;
        return true;
        }
    case REG_TYPE_TM:
        {
        /*算起始地址，一次最小取一个byte*/
        nAddrValue = (mAddrInfo.nAddrValue/16)*16;

        /*算返回的字节长度*/
        if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
        }
        else
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
        }

        /*算取得地址长度*/
        char iTemp1,iTemp2;

        iTemp1 = mAddrInfo.nAddrValue/100;
        iTemp2 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 3*/
        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 4*/
        nTmpValue = 'M';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*读取长度*/
        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 8*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 7;

        return true;
        }
    case REG_TYPE_TVC:
    case REG_TYPE_CVC:
        {
        /*算起始地址，一次最小取一个byte*/
        nAddrValue = (mAddrInfo.nAddrValue/16)*16;

        /*算返回的字节长度*/
        if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
        }
        else
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
        }

        /*算取得地址长度*/
        char iTemp1,iTemp2,iTemp3;

        iTemp1 = mAddrInfo.nAddrValue/100;
        iTemp2 = (mAddrInfo.nAddrValue%100)/10;
        iTemp3 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(REG_TYPE_TVC == eRegType)
        {
            nTmpValue = 'T';
        }
        else
        {
            nTmpValue = 'C';
        }
        /*vector index = 3*/
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 8*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 15;

        return true;
        }
    case REG_TYPE_TVP:
    case REG_TYPE_CVP:
        {
        /*算起始地址，一次最小取一个byte*/
        nAddrValue = (mAddrInfo.nAddrValue/16)*16;

        /*算返回的字节长度*/
        if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
        }
        else
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
        }

        /*算取得地址长度*/
        char iTemp1,iTemp2,iTemp3;

        iTemp1 = mAddrInfo.nAddrValue/100;
        iTemp2 = (mAddrInfo.nAddrValue%100)/10;
        iTemp3 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(REG_TYPE_TVP == eRegType)
        {
            nTmpValue = 'T';
        }
        else
        {
            nTmpValue = 'C';
        }
        /*vector index = 3*/
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 8*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 15;

        return true;
        }
    case REG_TYPE_CTH:
        {
        /*算起始地址，一次最小取一个byte*/
        nAddrValue = (mAddrInfo.nAddrValue/16)*16;

        /*算返回的字节长度*/
        if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
        }
        else
        {
            nReturnLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
        }

        /*算取得地址长度*/
        char iTemp1;

        iTemp1 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 3*/
        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 4*/
        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 5*/
        nTmpValue = 'H';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 8*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 15;

        return true;
        }
    default:
        {
            return false;
            break;
        }
    }
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

    uchar nTmpValue = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_RLY:
        {
            //LOGE("COME INTO REG_TYPE_RLY");
            if (mAddrInfo.nValueList.at(0) == 0x01)                 //置位
            {
                nTmpValue = 'S';  /*vector index = 0*/
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'T';  /*vector index = 1*/
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if (mAddrInfo.nValueList.at(0) == 0x00)            //复位
            {
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'S';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                return false;
            }

            /*vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

            iTemp1 = (mAddrInfo.mAddrProp.nAddrValue/16)/100;
            iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue/16)%100)/10;
            iTemp3 = (mAddrInfo.mAddrProp.nAddrValue/16)%10;

            iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%16)/10;
            iTemp5 = (mAddrInfo.mAddrProp.nAddrValue%16)%10;

            /*vector index = 3*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*读取长度*/
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 8*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 9*/
            nTmpValue = 0x0a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 4;//有效长度+固定字节长度

            return true;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if (mAddrInfo.nValueList.at(0) == 0x01)                 //置位
            {
                nTmpValue = 'S';  /*vector index = 0*/
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'T';  /*vector index = 1*/
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if (mAddrInfo.nValueList.at(0) == 0x00)            //复位
            {
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'S';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                return false;
            }

            /*vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            if(REG_TYPE_T == eRegType)
            {
                nTmpValue = 'T';
            }
            else
            {
                nTmpValue = 'C';
            }

            /*vector index = 3*/
            mSendPackge.sendDataList.push_back(nTmpValue);

            char iTemp1,iTemp2,iTemp3;

            iTemp1 = (mAddrInfo.mAddrProp.nAddrValue)/100;
            iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue%100))/10;
            iTemp3 = (mAddrInfo.mAddrProp.nAddrValue)%10;

            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 7*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 8*/
            nTmpValue = 0x0a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 4;//有效长度+固定字节长度

            return true;
        }
    case REG_TYPE_DM:
        {
            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

            iTemp1 = mAddrInfo.mAddrProp.nAddrValue/1000;
            iTemp2 = (mAddrInfo.mAddrProp.nAddrValue%1000)/100;
            iTemp3 = (mAddrInfo.mAddrProp.nAddrValue%100)/10;
            iTemp4 = mAddrInfo.mAddrProp.nAddrValue%10;

            /*vector index = 0*/
            nTmpValue = 'W';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 'R';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 3*/
            nTmpValue = 'D';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 4*/
            nTmpValue = 'M';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 8*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            if (1 < mAddrInfo.nValueList.size())
            {
                int nTemp1 = mAddrInfo.nValueList[0] & 0xff;
                int nTemp2 = mAddrInfo.nValueList[1] & 0xff;
                int iData = nTemp1 + (nTemp2 << 8);

                iTemp1 = iData/10000;
                iTemp2 = (iData%10000)/1000;
                iTemp3 = (iData%1000)/100;
                iTemp4 = (iData%100)/10;
                iTemp5 = iData%10;

                /*vector index = 10*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 11*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 12*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 13*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 14*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*vector index = 15*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 16*/
            uchar nTmpValue = 0x0a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 4;

            return true;
        }
    case REG_TYPE_TM:
        {

        char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

        iTemp1 = mAddrInfo.mAddrProp.nAddrValue/10;
        iTemp2 = mAddrInfo.mAddrProp.nAddrValue%10;

        /*vector index = 0*/
        uchar nTmpValue = 'W';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 1*/
        nTmpValue = 'R';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 3*/
        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 4*/
        nTmpValue = 'M';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if (1 < mAddrInfo.nValueList.size())
        {
            int nTemp1 = mAddrInfo.nValueList[0] & 0xff;
            int nTemp2 = mAddrInfo.nValueList[1] & 0xff;
            int iData = nTemp1 + (nTemp2 << 8);

            iTemp1 = iData/10000;
            iTemp2 = (iData%10000)/1000;
            iTemp3 = (iData%1000)/100;
            iTemp4 = (iData%100)/10;
            iTemp5 = iData%10;

            /*vector index = 8*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 10*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 11*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 12*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }

        /* vector index = 13*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 14*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 4;

        return true;
        }
    case REG_TYPE_TVC:
    case REG_TYPE_CVC:
        {

        char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

        iTemp1 = mAddrInfo.mAddrProp.nAddrValue/100;
        iTemp2 = (mAddrInfo.mAddrProp.nAddrValue%100)/10;
        iTemp3 = mAddrInfo.mAddrProp.nAddrValue%10;

        /* vector index = 0*/
        nTmpValue = 'W';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 1*/
        nTmpValue = 'R';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(eRegType == REG_TYPE_TVC)
        {
           nTmpValue = 'T';
        }
        else
        {
            nTmpValue = 'C';
        }
        /* vector index = 3*/
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if (1 < mAddrInfo.nValueList.size())
        {
            int nTemp1 = mAddrInfo.nValueList[0] & 0xff;
            int nTemp2 = mAddrInfo.nValueList[1] & 0xff;
            int iData = nTemp1 + (nTemp2 << 8);

            iTemp1 = iData/10000;
            iTemp2 = (iData%10000)/1000;
            iTemp3 = (iData%1000)/100;
            iTemp4 = (iData%100)/10;
            iTemp5 = iData%10;

            /*vector index = 8*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 10*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 11*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 12*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }

        /* vector index = 13*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 14*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 4;

        return true;
        }
    case REG_TYPE_TVP:
    case REG_TYPE_CVP:
        {

        char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

        iTemp1 = mAddrInfo.mAddrProp.nAddrValue/100;
        iTemp2 = (mAddrInfo.mAddrProp.nAddrValue%100)/10;
        iTemp3 = mAddrInfo.mAddrProp.nAddrValue%10;

        /* vector index = 0*/
        nTmpValue = 'W';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 1*/
        nTmpValue = 'S';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(eRegType == REG_TYPE_TVP)
        {
           nTmpValue = 'T';
        }
        else
        {
            nTmpValue = 'C';
        }
        /*vector index = 3*/
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if (1 < mAddrInfo.nValueList.size())
        {
            int nTemp1 = mAddrInfo.nValueList[0] & 0xff;
            int nTemp2 = mAddrInfo.nValueList[1] & 0xff;
            int iData = nTemp1 + (nTemp2 << 8);

            iTemp1 = iData/10000;
            iTemp2 = (iData%10000)/1000;
            iTemp3 = (iData%1000)/100;
            iTemp4 = (iData%100)/10;
            iTemp5 = iData%10;

            /*vector index = 8*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 10*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 11*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 12*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }

        /* vector index = 13*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 14*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 4;

        return true;
        }
    case REG_TYPE_CTH:
        {

        char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

        iTemp1 = mAddrInfo.mAddrProp.nAddrValue%10;

        /* vector index = 0*/
        nTmpValue = 'W';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 1*/
        nTmpValue = 'R';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 3*/
        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);


        /*vector index = 4*/
        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 5*/
        nTmpValue = 'H';
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 7*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if (1 < mAddrInfo.nValueList.size())
        {
            int nTemp1 = mAddrInfo.nValueList[0] & 0xff;
            int nTemp2 = mAddrInfo.nValueList[1] & 0xff;
            int iData = nTemp1 + (nTemp2 << 8);

            iTemp1 = iData/10000;
            iTemp2 = (iData%10000)/1000;
            iTemp3 = (iData%1000)/100;
            iTemp4 = (iData%100)/10;
            iTemp5 = iData%10;

            /*vector index = 8*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 10*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 11*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);
            /*vector index = 12*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }

        /* vector index = 13*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /* vector index = 14*/
        nTmpValue = 0x0a;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = 4;

        return true;
        }
    default:
        {
            break;
        }
    }

    return false;
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
    if(mSourceAddrList.empty()) return false;
    targetList.clear();

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
    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;
    vector<ADDR_TARGET_PROP > mTmpListWord5;
    vector<ADDR_TARGET_PROP > mTmpListWord6;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_RLY:
            {
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = mTmpAddr.nAddrValue/16;
//
                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_T:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DM:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TM:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TVC:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TVP:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CVC:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CVP:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CTH:
            {
                mTmpListWord6.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for//把0.x 1.x 2.x····分开
    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
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

    int tmpSize = mTmpListBit0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit0.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
//    mTmpListBit1 = PlcMathMethod::addrBubbleSort(mTmpListBit1);
//    if(bWriteAddr)
//    {
//        /*写地址，则只支持长度为1的写*/
//        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, 1, 1);
//    }
//    else
//    {
//        /*读地址，可支持多个长度*/
//        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 8);
//    }

    tmpSize = mTmpListBit1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit1.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
//    mTmpListBit2 = PlcMathMethod::addrBubbleSort(mTmpListBit2);
//    if(bWriteAddr)
//    {
//        /*写地址，则只支持长度为1的写*/
//        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, 1, 1);
//    }
//    else
//    {
//        /*读地址，可支持多个长度*/
//        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, nMaxRWlen, 8);
//    }

    tmpSize = mTmpListBit2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit2.at(i));
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

    return true;
}

/******************************************************************
 * Function: 获得默认最大的读写长度
 * Parameters:
 * Return: 最大长度
 ******************************************************************/
int PlcProtocol::getMaxRwLen()
{
    int nMaxRWlen = 2;//一次最大读取长度2个字节 一个字
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
    case REG_TYPE_RLY:                      //REG_TYPE_RLY:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_16000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TVC:
    case REG_TYPE_TVP:
    case REG_TYPE_CVC:
    case REG_TYPE_CVP:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_100)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10)
            {
                return false;
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
    case REG_TYPE_RLY:                      //REG_TYPE_RLY:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_16000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TVC:
    case REG_TYPE_TVP:
    case REG_TYPE_CVC:
    case REG_TYPE_CVP:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_100)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10)
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
            eRegType = REG_TYPE_RLY;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_DM;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_TM;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_TVC;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_TVP;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_CVC;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_CVP;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_CTH;
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

    if(DealWithString::compare("RLY", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("DM", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("TM", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("TVC", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("TVP", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("CVC", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("CVP", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("CTH", sRegName)))
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


