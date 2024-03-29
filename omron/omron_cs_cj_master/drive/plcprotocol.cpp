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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_32768 * 16)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if (!bSuccess)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if (!bSuccess)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_32768)
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
            sRegNameList.push_back("CIO_BIT");
            sRegNameList.push_back("W_BIT");
            sRegNameList.push_back("H_BIT");
            sRegNameList.push_back("A_BIT");
            sRegNameList.push_back("D_BIT");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("CIO");
            sRegNameList.push_back("W");
            sRegNameList.push_back("H");
            sRegNameList.push_back("A");
            sRegNameList.push_back("D");
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
            sRegNameList.push_back("CIO_BIT");
            sRegNameList.push_back("W_BIT");
            sRegNameList.push_back("H_BIT");
            sRegNameList.push_back("A_BIT");
            sRegNameList.push_back("D_BIT");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("CIO");
            sRegNameList.push_back("W");
            sRegNameList.push_back("H");
            sRegNameList.push_back("A");
            sRegNameList.push_back("D");
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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
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
                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_32768 * 16)
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
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*小数部分不能超过16，也不能为负数*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_32768)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_32768)
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
        int nSize = rcvList.size() - 27;
        targetList.clear();
        for(int i = 23; i < nSize + 23; i++)
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
    if(nRcvSize < 27)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvHighByte = rcvList.at(nRcvSize - 4);
    uchar nRcvLowByte = rcvList.at(nRcvSize - 3);

    vector<uchar> templist;
    templist.clear();
    int size = nRcvSize - 4;
    for (int i=0; i<size; i++)
    {
        templist.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getXORcheck(templist);
    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

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
    switch (nPkgIndex)
    {
    case 1:
        {
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x40);//开始字符@

            uchar ucTmp1 = PlcMathMethod::intToAsciiValue((nStationId >> 4) & 0xf);
            uchar ucTmp2 = PlcMathMethod::intToAsciiValue((nStationId >> 0) & 0xf);

            mHandShake.sendDataList.push_back(ucTmp1);//站号
            mHandShake.sendDataList.push_back(ucTmp2);

            mHandShake.sendDataList.push_back(0x53);//功能码SC，修改PLC状态
            mHandShake.sendDataList.push_back(0x43);
            mHandShake.sendDataList.push_back(0x30);//修改PLC状态为监视模式：0编程；2监视；3运行
            mHandShake.sendDataList.push_back(0x32);

            ushort nCheckResult = PlcMathMethod::getXORcheck(mHandShake.sendDataList);//计算校验

            ucTmp1 = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            ucTmp2 = PlcMathMethod::intToAsciiValue((nCheckResult >> 0) & 0xf);

            mHandShake.sendDataList.push_back(ucTmp1);
            mHandShake.sendDataList.push_back(ucTmp2);

            mHandShake.sendDataList.push_back(0x2a);//结束字符*CR
            mHandShake.sendDataList.push_back(0x0d);

            mHandShake.nReturnLen = 11;
            nCmnTimes = 1;
            return true;
        }
        break;
    default:
        return false;
        break;
    }

    return false;
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
    case 1:
        {
            if (nRcvSize == 11)
            {
                if (rcvList.at(0) == 0x40 && rcvList.at(5) == 0x30 && rcvList.at(6) == 0x30 &&
                    rcvList.at(9) == 0x2a && rcvList.at(10) == 0x0d)
                {
                    return CMN_NOMAL_CODE;
                }
                else
                {
                    return DATA_CHECK_ERROR;
                }
            }
            else
            {
                return DATA_LEN_ERROR;
            }
        }
        break;
    default:
        break;
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
    /*初始化*/
    mSendPackge.nTotalPkgNum = 1;
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

    ushort nAddrValue = 0;
    int nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);//头代码 FA，即46 41
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);//返回等待时间

            mSendPackge.sendDataList.push_back(0x30);//ICF 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//DA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//SA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//SID 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//FINS command code 固定30 31 30 32
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            switch (eRegType)
            {
            case REG_TYPE_CIO_BIT:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            case REG_TYPE_W_BIT:
                mSendPackge.sendDataList.push_back(0x42);//W
                mSendPackge.sendDataList.push_back(0x31);
                break;
            case REG_TYPE_H_BIT:
                mSendPackge.sendDataList.push_back(0x42);//H
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_A_BIT:
                mSendPackge.sendDataList.push_back(0x42);//A
                mSendPackge.sendDataList.push_back(0x33);
                break;
            case REG_TYPE_D_BIT:
                mSendPackge.sendDataList.push_back(0x38);//D
                mSendPackge.sendDataList.push_back(0x32);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            }

            nAddrValue = mAddrInfo.nAddrValue/16;//起始地址
            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)//读取长度
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            uctmp1 = (nAddrLen >> 12) & 0xf;
            uctmp2 = (nAddrLen >> 8) & 0xf;
            uctmp3 = (nAddrLen >> 4) & 0xf;
            uctmp4 = (nAddrLen >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 27 + nAddrLen*4;
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);//头代码 FA，即46 41
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);//返回等待时间

            mSendPackge.sendDataList.push_back(0x30);//ICF 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//DA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//SA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//SID 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//FINS command code 固定30 31 30 32
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            nAddrValue = mAddrInfo.nAddrValue;//起始地址
            switch (eRegType)
            {
            case REG_TYPE_CIO:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            case REG_TYPE_W:
                mSendPackge.sendDataList.push_back(0x42);//W
                mSendPackge.sendDataList.push_back(0x31);
                break;
            case REG_TYPE_H:
                mSendPackge.sendDataList.push_back(0x42);//H
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_A:
                mSendPackge.sendDataList.push_back(0x42);//A
                mSendPackge.sendDataList.push_back(0x33);
                break;
            case REG_TYPE_D:
                mSendPackge.sendDataList.push_back(0x38);//D
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back(0x38);//TV
                mSendPackge.sendDataList.push_back(0x39);
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back(0x38);//CV
                mSendPackge.sendDataList.push_back(0x39);

                nAddrValue = mAddrInfo.nAddrValue + 0x8000;
                break;
            default:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            }

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            uctmp1 = (mAddrInfo.nAddrLen >> 12) & 0xf;
            uctmp2 = (mAddrInfo.nAddrLen >> 8) & 0xf;
            uctmp3 = (mAddrInfo.nAddrLen >> 4) & 0xf;
            uctmp4 = (mAddrInfo.nAddrLen >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 27 + mAddrInfo.nAddrLen*4;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);//头代码 FA，即46 41
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);//返回等待时间

            mSendPackge.sendDataList.push_back(0x30);//ICF 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//DA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//SA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//SID 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//FINS command code 固定30 31 30 32
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            mSendPackge.sendDataList.push_back(0x30);//TC区Memory area code
            mSendPackge.sendDataList.push_back(0x39);

            if (REG_TYPE_T == eRegType)
            {
                nAddrValue = mAddrInfo.nAddrValue;
            }
            else
            {
                nAddrValue = mAddrInfo.nAddrValue + 0x8000;
            }

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            /*index = 20*/
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            if((mAddrInfo.nAddrLen)%8 != 0)
            {
                nAddrLen = (mAddrInfo.nAddrLen)/8 + 1;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrLen)/8;
            }
            nAddrLen = nAddrLen * 16;
            uctmp1 = (nAddrLen/2 >> 12) & 0xf;
            uctmp2 = (nAddrLen/2 >> 8) & 0xf;
            uctmp3 = (nAddrLen/2 >> 4) & 0xf;
            uctmp4 = (nAddrLen/2 >> 0) & 0xf;

            /*index = 26*/
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 27 + nAddrLen;
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
    int nAddrValue = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);//头代码 FA，即46 41
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);//返回等待时间

            mSendPackge.sendDataList.push_back(0x30);//ICF 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//DA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//SA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//SID 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//FINS command code 固定30 31 30 32
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x32);

            switch (eRegType)
            {
            case REG_TYPE_CIO_BIT:
                mSendPackge.sendDataList.push_back(0x33);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            case REG_TYPE_W_BIT:
                mSendPackge.sendDataList.push_back(0x33);//W
                mSendPackge.sendDataList.push_back(0x31);
                break;
            case REG_TYPE_H_BIT:
                mSendPackge.sendDataList.push_back(0x33);//H
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_A_BIT:
                mSendPackge.sendDataList.push_back(0x33);//A
                mSendPackge.sendDataList.push_back(0x33);
                break;
            case REG_TYPE_D_BIT:
                mSendPackge.sendDataList.push_back(0x30);//D
                mSendPackge.sendDataList.push_back(0x32);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x33);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            }

            uint nWordpass = mAddrInfo.mAddrProp.nAddrValue/16;//起始地址
            uchar uctmp1 = (nWordpass >> 12) & 0xf;
            uchar uctmp2 = (nWordpass >> 8) & 0xf;
            uchar uctmp3 = (nWordpass >> 4) & 0xf;
            uchar uctmp4 = (nWordpass >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            uchar ucBitpass = mAddrInfo.mAddrProp.nAddrValue%16;

            nTmpValue = PlcMathMethod::intToAsciiValue((ucBitpass >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((ucBitpass >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            if (mAddrInfo.nValueList.size() > 0)
            {
                nTmpValue = mAddrInfo.nValueList.at(0);
                if (nTmpValue)//置位
                {
                    mSendPackge.sendDataList.push_back(0x30);
                    mSendPackge.sendDataList.push_back(0x31);
                }
                else//复位
                {
                    mSendPackge.sendDataList.push_back(0x30);
                    mSendPackge.sendDataList.push_back(0x30);
                }
            }

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 27;
            bSuccess = true;
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);//头代码 FA，即46 41
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);//返回等待时间

            mSendPackge.sendDataList.push_back(0x30);//ICF 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//DA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//SA2 固定30 30
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);//SID 固定30 30
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);//FINS command code 固定30 31 30 32
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x32);

            nAddrValue = mAddrInfo.mAddrProp.nAddrValue;//起始地址
            switch (eRegType)
            {
            case REG_TYPE_CIO:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            case REG_TYPE_W:
                mSendPackge.sendDataList.push_back(0x42);//W
                mSendPackge.sendDataList.push_back(0x31);
                break;
            case REG_TYPE_H:
                mSendPackge.sendDataList.push_back(0x42);//H
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_A:
                mSendPackge.sendDataList.push_back(0x42);//A
                mSendPackge.sendDataList.push_back(0x33);
                break;
            case REG_TYPE_D:
                mSendPackge.sendDataList.push_back(0x38);//D
                mSendPackge.sendDataList.push_back(0x32);
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back(0x38);//TV
                mSendPackge.sendDataList.push_back(0x39);
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back(0x38);//CV
                mSendPackge.sendDataList.push_back(0x39);

                nAddrValue = mAddrInfo.mAddrProp.nAddrValue + 0x8000;
                break;
            default:
                mSendPackge.sendDataList.push_back(0x42);//CIO
                mSendPackge.sendDataList.push_back(0x30);
                break;
            }

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            uctmp1 = (mAddrInfo.mAddrProp.nAddrLen >> 12) & 0xf;
            uctmp2 = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xf;
            uctmp3 = (mAddrInfo.mAddrProp.nAddrLen >> 4) & 0xf;
            uctmp4 = (mAddrInfo.mAddrProp.nAddrLen >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mAddrInfo.nValueList.resize(mAddrInfo.mAddrProp.nAddrLen * 2);

            for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)
            {
                uctmp1 = mAddrInfo.nValueList.at(i*2 + 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                uctmp1 = mAddrInfo.nValueList.at(i*2 + 0);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 27;
            bSuccess = true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mSendPackge.sendDataList.push_back(0x40);//开始符

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//站号
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x46);
            mSendPackge.sendDataList.push_back(0x41);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x38);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x32);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x46);

            mSendPackge.sendDataList.push_back(0x43);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x32);

            mSendPackge.sendDataList.push_back(0x33);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);
            mSendPackge.sendDataList.push_back(0x30);

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            if (mAddrInfo.nValueList.size() > 0)
            {
                nTmpValue = mAddrInfo.nValueList.at(0);
                if (nTmpValue)//置位
                {
                    mSendPackge.sendDataList.push_back(0x31);
                }
                else//复位
                {
                    mSendPackge.sendDataList.push_back(0x30);
                }
            }

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x39);

            if (REG_TYPE_T == eRegType)//对T区
            {
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue;
            }
            else//对C区
            {
                nAddrValue = mAddrInfo.mAddrProp.nAddrValue + 0x8000;
            }

            uchar uctmp1 = (nAddrValue >> 12) & 0xf;
            uchar uctmp2 = (nAddrValue >> 8) & 0xf;
            uchar uctmp3 = (nAddrValue >> 4) & 0xf;
            uchar uctmp4 = (nAddrValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 39;
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
        case REG_TYPE_T:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CIO_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_CIO;

                /*起始位置为所在字的起始位置*/
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
        case REG_TYPE_W_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_W;

                /*起始位置为所在字的起始位置*/
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

                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_H_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_H;

                /*起始位置为所在字的起始位置*/
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

                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_A_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_A;

                /*起始位置为所在字的起始位置*/
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

                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_D_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_D;

                /*起始位置为所在字的起始位置*/
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

                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CIO:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_W:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_H:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_A:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_D:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TV:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CV:
            {
                mTmpListWord6.push_back(mTmpAddr);
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
        int nMaxTc = 8;
        if(nMaxTc > nMaxRWlen)
        {
            nMaxTc = nMaxRWlen;
        }
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxTc, 8);
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
        int nMaxTc = 8;
        if(nMaxTc > nMaxRWlen)
        {
            nMaxTc = nMaxRWlen;
        }
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxTc, 8);
    }
    tmpSize = mTmpListBit1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit1.at(i));
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
    mSerialInfo.DATA_BIT = 7;
    mSerialInfo.PARITY_TYPE = 2;
    mSerialInfo.STOP_BIT = 2;

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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768 * 16)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768 * 16)
            {
                return false;
            }
            break;
        }
//    case REG_TYPE_T:
//    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
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
            eRegType = REG_TYPE_CIO_BIT;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_W_BIT;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_H_BIT;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_A_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_D_BIT;
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
            eRegType = REG_TYPE_CIO;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_W;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_H;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_A;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 13:
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

    if(DealWithString::compare("CIO_BIT", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("W_BIT", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("H_BIT", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("A_BIT", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("D_BIT", sRegName)))
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
    else if((DealWithString::compare("CIO", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("W", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("H", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("A", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
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
