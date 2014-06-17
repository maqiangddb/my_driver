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
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 1048576)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_0X:
    case REG_TYPE_1X:
    case REG_TYPE_3X:
    case REG_TYPE_4X:
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            return true;
            break;
        }
    default:
        {
            return false;
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
            sRegNameList.push_back("0x");
            sRegNameList.push_back("1x");
            sRegNameList.push_back("3x_bit");
            sRegNameList.push_back("4x_bit");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("3x");
            sRegNameList.push_back("4x");
            sRegNameList.push_back("3x_dw");
            sRegNameList.push_back("4x_dw");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("3x_dw");
            sRegNameList.push_back("4x_dw");
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
            sRegNameList.push_back("0x");
            sRegNameList.push_back("1x");
            sRegNameList.push_back("3x_bit");
            sRegNameList.push_back("4x_bit");
            sRegNameList.push_back("3x");
            sRegNameList.push_back("4x");
            sRegNameList.push_back("3x_dw");
            sRegNameList.push_back("4x_dw");
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
    case REG_TYPE_0X:
    case REG_TYPE_1X:
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_3X:
    case REG_TYPE_4X:
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
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
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
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

                    if(nStartValue + mTmpAddr.nAddrLen <= 1048576)
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
    case REG_TYPE_0X:
    case REG_TYPE_1X:
    case REG_TYPE_3X:
    case REG_TYPE_4X:
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_65536)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536)
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
    if(nRcvSize < 12)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 3));//取返回数据中校验的低位
    uchar nRcvHighByte = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 4));//取返回数据中校验的高位

    vector<unsigned char> tmprcvList;
    for (int i=0; i<(nRcvSize - 5)/2; i++)
    {
        uchar ucTmp1 = PlcMathMethod::asciiToIntValue(rcvList.at(i*2 + 1));
        uchar ucTmp2 = PlcMathMethod::asciiToIntValue(rcvList.at(i*2 + 2));
        ucTmp1 = (ucTmp1 << 4) & 0xf0;
        ucTmp2 = (ucTmp2 << 0) & 0x0f;
        tmprcvList.push_back(ucTmp1 + ucTmp2);
    }

    ushort nCheckResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;

    uchar highByte = (nCheckResult >> 4) & 0xf;
    uchar lowByte = nCheckResult & 0xf;

    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
        /*取有效数据*/
        targetList.clear();
        for(int i = 7; i < nRcvSize - 4; i++)
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
    if(nRcvSize != 17)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 3));//取返回数据中校验的低位
    uchar nRcvHighByte = PlcMathMethod::asciiToIntValue(rcvList.at(nRcvSize - 4));//取返回数据中校验的高位

    vector<uchar> tmprcvList;
    for (int i=0; i<(nRcvSize - 5)/2; i++)
    {
        uchar ucTmp1 = PlcMathMethod::asciiToIntValue(rcvList.at(i*2 + 1));
        uchar ucTmp2 = PlcMathMethod::asciiToIntValue(rcvList.at(i*2 + 2));
        ucTmp1 = (ucTmp1 << 4) & 0xf0;
        ucTmp2 = (ucTmp2 << 0) & 0x0f;
        tmprcvList.push_back(ucTmp1 + ucTmp2);
    }

    ushort nCheckResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;

    uchar highByte = (nCheckResult >> 4) & 0xf;
    uchar lowByte = nCheckResult & 0xf;

    /*write success*/
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
            if(REG_TYPE_4X_BIT == eRegType)
            {
                mCurrAddr.nValueList = mAddrInfo.nValueList;

                vector<uchar> tmpList;

                /*校验开始*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                tmpList.push_back(nTmpValue);

                nTmpValue = 0x03;
                tmpList.push_back(nTmpValue);

                int nSendAdd = (mAddrInfo.mAddrProp.nAddrValue/16);
                nTmpValue = (nSendAdd >> 8) & 0xff;
                tmpList.push_back(nTmpValue);
                nTmpValue = (nSendAdd >> 0) & 0xff;
                tmpList.push_back(nTmpValue);

                nTmpValue = 0x00;
                tmpList.push_back(nTmpValue);
                nTmpValue = 0x01;
                tmpList.push_back(nTmpValue);

                ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                /*校验完毕*/

                /*开始符固定 index = 0*/
                nTmpValue = 0x3a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*把站号，地址，长度转成ASC码 index = 1-12*/
                if (tmpList.size() == 6)
                {
                    for (int i=0; i<6; i++)
                    {
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                    }
                }

                /*校验 index = 13 14*/
                mSendPackge.sendDataList.push_back(highByte);
                mSendPackge.sendDataList.push_back(lowByte);

                /*结束符 index = 15 16*/
                nTmpValue = 0x0d;
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 0x0a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = 15;
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

            if(REG_TYPE_4X_BIT == eRegType)
            {
                vector<uchar> tmpList;

                /*校验开始*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                tmpList.push_back(nTmpValue);

                nTmpValue = 0x06;
                tmpList.push_back(nTmpValue);

                int nSendAdd = (mAddrInfo.mAddrProp.nAddrValue/16);
                nTmpValue = (nSendAdd >> 8) & 0xff;
                tmpList.push_back(nTmpValue);
                nTmpValue = (nSendAdd >> 0) & 0xff;
                tmpList.push_back(nTmpValue);

                /*输入值12 8 4 0的顺序写入，读取返回的顺序也是12 8 4 0*/
                int nValue = 0;
                if (mAddrInfo.nValueList.size() == 4)
                {
                    int nTmp1 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(0));//12
                    int nTmp2 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(1));//8
                    int nTmp3 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(2));//4
                    int nTmp4 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(3));//0
                    /*读取值*/
                    nValue = ((nTmp1 << 12) & 0xf000) + ((nTmp2 << 8) & 0x0f00) + ((nTmp3 << 4) & 0x00f0) + (nTmp4& 0x000f);

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

                    nTmpValue = ((nValue >> 8) & 0xff);
                    tmpList.push_back(nTmpValue);

                    nTmpValue = ((nValue >> 0) & 0xff);
                    tmpList.push_back(nTmpValue);
                }

                ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                /*校验完毕*/

                /*开始符固定 index = 0*/
                nTmpValue = 0x3a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*把站号，地址，长度转成ASC码 index = 1-12*/
                if (tmpList.size() == 6)
                {
                    for (int i=0; i<6; i++)
                    {
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                    }
                }

                /*校验 index = 13 14*/
                mSendPackge.sendDataList.push_back(highByte);
                mSendPackge.sendDataList.push_back(lowByte);

                /*结束符 index = 15 16*/
                nTmpValue = 0x0d;
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 0x0a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = 17;
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

    uchar nFumNum = 0;
    ushort nAddrValue = 0;
    int nAddrLen = 1;
    int nSendLen = 1;
    int nRetrunLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    switch(eRegType)
    {
    case REG_TYPE_0X:
        {
            /*功能码*/
            nFumNum = 0x01;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = (mAddrInfo.nAddrValue/8)*8;

            /*算返回的字节长度*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*算取得地址长度*/
            nSendLen = mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen;
            nRetrunLen = nAddrLen*2;
            break;
        }
    case REG_TYPE_1X:
        {
            /*功能码*/
            nFumNum = 0x02;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = (mAddrInfo.nAddrValue/8)*8;

            /*算返回的字节长度*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*算取得地址长度*/
            nSendLen = mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen;
            nRetrunLen = nAddrLen*2;
            break;
        }
    case REG_TYPE_3X_BIT:
        {
            /*功能码*/
            nFumNum = 0x04;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue/16;

            /*算取得地址长度*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            /*算返回的字节长度*/
            nSendLen = nAddrLen;
            nRetrunLen = nAddrLen*4;
            break;
        }
    case REG_TYPE_3X:
    case REG_TYPE_3X_DOUBLE:
        {
            /*功能码*/
            nFumNum = 0x04;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            /*算取得地址长度*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*算返回的字节长度*/
            nSendLen = nAddrLen;
            nRetrunLen = nAddrLen*4;
            break;
        }
    case REG_TYPE_4X_BIT:
        {
            /*功能码*/
            nFumNum = 0x03;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue/16;

            /*算取得地址长度*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            /*算返回的字节长度*/
            nSendLen = nAddrLen;
            nRetrunLen = nAddrLen*4;
            break;
        }
    case REG_TYPE_4X:
    case REG_TYPE_4X_DOUBLE:
        {
            /*功能码*/
            nFumNum = 0x03;

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            /*算取得地址长度*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*算返回的字节长度*/
            nSendLen = nAddrLen;
            nRetrunLen = nAddrLen*4;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    vector<uchar> tmpList;
    /*校验开始*/
    uchar nTmpValue = 0;
    memcpy(&nTmpValue, &mAddrInfo.nPlcStationIndex, 1);
    tmpList.push_back(nTmpValue);

    nTmpValue = nFumNum & 0xff;
    tmpList.push_back(nTmpValue);

    nTmpValue = (nAddrValue >> 8) & 0xff;
    tmpList.push_back(nTmpValue);
    nTmpValue = (nAddrValue >> 0) & 0xff;
    tmpList.push_back(nTmpValue);

    nTmpValue = (nSendLen >> 8) & 0xff;
    tmpList.push_back(nTmpValue);
    nTmpValue = (nSendLen >> 0) & 0xff;
    tmpList.push_back(nTmpValue);

    ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
    /*校验完毕*/

    /*开始符固定 index = 0*/
    nTmpValue = 0x3a;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*把站号，地址，长度转成ASC码 index = 1-12*/
    if (tmpList.size() == 6)
    {
        for (int i=0; i<6; i++)
        {
            nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);
            nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
    }

    /*校验 index = 13 14*/
    mSendPackge.sendDataList.push_back(highByte);
    mSendPackge.sendDataList.push_back(lowByte);

    /*结束符 index = 15 16*/
    nTmpValue = 0x0d;
    mSendPackge.sendDataList.push_back(nTmpValue);
    nTmpValue = 0x0a;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*返回数据的长度*/
    if(mAddrInfo.nPlcStationIndex == 0)
    {
        mSendPackge.nReturnLen = 0;
    }
    else
    {
        mSendPackge.nReturnLen = 11 + nRetrunLen;
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:
        {
            vector<uchar> tmpList;
            /*校验开始*/
            uchar nTmpValue = 0;
            memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
            tmpList.push_back(nTmpValue);

            nTmpValue = 0x05;
            tmpList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
            tmpList.push_back(nTmpValue);
            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xff;
            tmpList.push_back(nTmpValue);

            /*赋值*/
            nTmpValue = mAddrInfo.nValueList.front();
            if(nTmpValue == 1)
            {
                tmpList.push_back(0xff);
                tmpList.push_back(0x00);
            }
            else
            {
                tmpList.push_back(0x00);
                tmpList.push_back(0x00);
            }

            ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            /*校验完毕*/

            /*开始符固定 index = 0*/
            nTmpValue = 0x3a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*把站号，地址，长度转成ASC码 index = 1-12*/
            if (tmpList.size() == 6)
            {
                for (int i=0; i<6; i++)
                {
                    nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }

            /*校验 index = 13 14*/
            mSendPackge.sendDataList.push_back(highByte);
            mSendPackge.sendDataList.push_back(lowByte);

            /*结束符 index = 15 16*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);
            nTmpValue = 0x0a;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            if(mAddrInfo.mAddrProp.nPlcStationIndex == 0)
            {
                mSendPackge.nReturnLen = 0;
            }
            else
            {
                mSendPackge.nReturnLen = 17;
            }
            bSuccess = true;
            break;
        }
    case REG_TYPE_4X_BIT:
        {
            mCmnProp.eSendType = CMN_READ_BEFORE_WRITE;
            return makePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case REG_TYPE_4X:
    case REG_TYPE_4X_DOUBLE:
        {
            if(1 == mAddrInfo.mAddrProp.nAddrLen)
            {
                vector<uchar> tmpList;
                /*校验开始*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                tmpList.push_back(nTmpValue);

                nTmpValue = 0x06;
                tmpList.push_back(nTmpValue);

                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
                tmpList.push_back(nTmpValue);
                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xff;
                tmpList.push_back(nTmpValue);

                /*赋值*/
                if(mAddrInfo.nValueList.size() >= 2)
                {
                    tmpList.push_back(mAddrInfo.nValueList.at(1));
                    tmpList.push_back(mAddrInfo.nValueList.at(0));
                }
                else
                {
                    tmpList.push_back(0x00);
                    tmpList.push_back(0x00);
                }

                ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                /*校验完毕*/

                /*开始符固定 index = 0*/
                nTmpValue = 0x3a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*把站号，地址，长度转成ASC码 index = 1-12*/
                if (tmpList.size() == 6)
                {
                    for (int i=0; i<6; i++)
                    {
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                        nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
                        mSendPackge.sendDataList.push_back(nTmpValue);
                    }
                }

                /*校验 index = 13 14*/
                mSendPackge.sendDataList.push_back(highByte);
                mSendPackge.sendDataList.push_back(lowByte);

                /*结束符 index = 15 16*/
                nTmpValue = 0x0d;
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 0x0a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                if(mAddrInfo.mAddrProp.nPlcStationIndex == 0)
                {
                    mSendPackge.nReturnLen = 0;
                }
                else
                {
                    mSendPackge.nReturnLen = 17;
                }
            }
            else if(mAddrInfo.mAddrProp.nAddrLen > 1)
            {
                vector<uchar> tmpList;
                /*校验开始*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                tmpList.push_back(nTmpValue);

                nTmpValue = 0x10;
                tmpList.push_back(nTmpValue);

                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
                tmpList.push_back(nTmpValue);
                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xff;
                tmpList.push_back(nTmpValue);

                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xff;
                tmpList.push_back(nTmpValue);
                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 0) & 0xff;
                tmpList.push_back(nTmpValue);

                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen*2) & 0xff;
                tmpList.push_back(nTmpValue);

                mAddrInfo.nValueList.resize(mAddrInfo.mAddrProp.nAddrLen*2);

                if (REG_TYPE_4X == eRegType)
                {
                    for(int i = 0; i < mAddrInfo.mAddrProp.nAddrLen; i++)
                    {
                        nTmpValue = mAddrInfo.nValueList.at(2*i + 1);
                        tmpList.push_back(nTmpValue);

                        nTmpValue = mAddrInfo.nValueList.at(2*i + 0);
                        tmpList.push_back(nTmpValue);
                    }
                }
                else
                {
                    for(int i = 0; i < mAddrInfo.mAddrProp.nAddrLen/2; i++)
                    {
                        nTmpValue = mAddrInfo.nValueList.at(4*i + 3);
                        tmpList.push_back(nTmpValue);

                        nTmpValue = mAddrInfo.nValueList.at(4*i + 2);
                        tmpList.push_back(nTmpValue);

                        nTmpValue = mAddrInfo.nValueList.at(4*i + 1);
                        tmpList.push_back(nTmpValue);

                        nTmpValue = mAddrInfo.nValueList.at(4*i + 0);
                        tmpList.push_back(nTmpValue);
                    }
                }

                ushort nCheckResult = PlcMathMethod::getLRCcheck(tmpList) & 0xff;
                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                /*校验完毕*/

                /*开始符固定 index = 0*/
                nTmpValue = 0x3a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*把站号，地址，长度转成ASC码 index = 1-12*/
                int size = tmpList.size();
                for (int i=0; i<size; i++)
                {
                    nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 4) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    nTmpValue = PlcMathMethod::intToAsciiValue((tmpList.at(i) >> 0) & 0xf);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                /*校验 index = 13 14*/
                mSendPackge.sendDataList.push_back(highByte);
                mSendPackge.sendDataList.push_back(lowByte);

                /*结束符 index = 15 16*/
                nTmpValue = 0x0d;
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 0x0a;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*返回数据的长度*/
                if(mAddrInfo.mAddrProp.nPlcStationIndex == 0)
                {
                    mSendPackge.nReturnLen = 0;
                }
                else
                {
                    mSendPackge.nReturnLen = 17;
                }
            }
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

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_0X:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_1X:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_3X_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_3X;

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
        case REG_TYPE_4X_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_4X;

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
        case REG_TYPE_3X:
        case REG_TYPE_3X_DOUBLE:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_4X:
        case REG_TYPE_4X_DOUBLE:
            {
                mTmpListWord1.push_back(mTmpAddr);
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_4X_BIT:
    case REG_TYPE_3X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 1048576)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_0X:
    case REG_TYPE_1X:
    case REG_TYPE_3X:
    case REG_TYPE_4X:
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65536)
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
    case REG_TYPE_0X:
    case REG_TYPE_4X:
    case REG_TYPE_4X_DOUBLE:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_4X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 1048576)
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
            eRegType = REG_TYPE_0X;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_1X;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_3X_BIT;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_4X_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_3X;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_4X;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_3X_DOUBLE;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_4X_DOUBLE;
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

    if(DealWithString::compare("0x", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("1x", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("3x_bit", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("4x_bit", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("3x", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("4x", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("3x_dw", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("4x_dw", sRegName)))
    {
        nRegIndex = 7;
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

