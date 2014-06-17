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
    /*�ȳ�ʼ��Ŀ���ַ*/
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
    case REG_TYPE_P:
    case REG_TYPE_M:
    case REG_TYPE_K:
    case REG_TYPE_L:
    case REG_TYPE_F:
        {
            if (mAddrSource.sAddrValue.size() == 1)
            {
                /*���Ϻ�Ľ��*/
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

                /*���Ϻ�Ľ��*/
                mAddrTarget.nAddrValue = nIntValue*16 + nDecValue;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
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

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10000)
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
            sRegNameList.push_back("P");
            sRegNameList.push_back("M");
            sRegNameList.push_back("K");
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
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
            sRegNameList.push_back("P");
            sRegNameList.push_back("M");
            sRegNameList.push_back("K");
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
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
    case REG_TYPE_P:
    case REG_TYPE_M:
    case REG_TYPE_K:
    case REG_TYPE_L:
    case REG_TYPE_F:
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
 * Function: ��ַ�Ķ��ظ���
 * Parameters: address list
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP> &mSourceAddrList, int nInterval, int nCopyNum)
{
    /*��ʼ��*/
    mSourceAddrList.clear();
    bool bSuccess = false;
    ADDR_CHECK_SOURCE_PROP mTmpAddr;
    mTmpAddr = mStartAddr;

    int nRegIndex = 0;
    if(!getRegIndexFromName(mTmpAddr.sRegName, nRegIndex)) return ILLEGAL_REG;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_P:
    case REG_TYPE_M:
    case REG_TYPE_K:
    case REG_TYPE_L:
    case REG_TYPE_F:
        {
            int nIntValue = 0;
            int nDecValue = 0;

            if (mStartAddr.sAddrValue.size() == 1)
            {
                /*���Ϻ�Ľ��*/
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

                /*���Ϻ�Ľ��*/
                int nStartValue = nIntValue*16 + nDecValue;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096)
                {
                    mTmpAddr.sAddrValue = "";

                    if(nIntValue != 0)
                    {
                        /*�����������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }

                    /*���С������*/
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
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
                    /*�����������*/
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

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
                    /*�����������*/
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
        /*ȡ��Ч����*/
        targetList.clear();

        int nRcvSize = rcvList.size() - 3;
        for(int i = 2; i < nRcvSize; i++)
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

    /*��һ�ζ�У��*/
    if (mUnpackProp.nCurrTimes == 1)
    {
        if (nRcvSize < 1)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }

        if(rcvList.at(0) == 0x06)
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
        if (nRcvSize < 4)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }

        if (rcvList.at(0) == 0x06 && rcvList.at(nRcvSize - 3) == 0x04)
        {
            uchar nRcvHighByte = rcvList.at(nRcvSize - 2);//PLC���������е�У��
            uchar nRcvLowByte = rcvList.at(nRcvSize - 1);

            vector<uchar> templist;
            templist.clear();
            for (int i=2; i<nRcvSize - 2; i++)
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
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: �������Э��İ�
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    return false;
}

/******************************************************************
 * Function: ������Э��İ������н��
 * Parameters: rcvList nCurrTimes: ������1��ʼ
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
    /*��ʼ��*/
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*�Ե�ǰ����ĵ�ַ��ֵ*/
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
    /*��ʼ��*/
    ADDR_TARGET_PROP mAddrInfo = mCmnProp.mAddrInfo.mAddrProp;

    mSendPackge.nTotalPkgNum = 2;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    uchar nTmpValue = 0;
    if(mSendPackge.nPkgIndex == 1)
    {
        nTmpValue = 0x05;
        mSendPackge.sendDataList.push_back(nTmpValue);
        mSendPackge.nReturnLen = 1;
    }
    else if(mSendPackge.nPkgIndex == 2)
    {
        uint nAddrValue = 0;
        int nAddrLen = 1;
        int iOffsetAdd = 0;
        REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
        switch(eRegType)
        {
        case REG_TYPE_P:
        case REG_TYPE_M:
        case REG_TYPE_K:
        case REG_TYPE_L:
        case REG_TYPE_F:
        case REG_TYPE_T:
        case REG_TYPE_C:
            {
                /*�ڶ��η��Ͱ�*/
                mSendPackge.sendDataList.clear();

                nTmpValue = 0x02;//ǰ��4���̶� 0 1 2 3
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x67;
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x46;
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x42;
                mSendPackge.sendDataList.push_back(nTmpValue);

                switch (eRegType)
                {
                case REG_TYPE_P:
                    iOffsetAdd = 0xA0;
                    break;
                case REG_TYPE_M:
                    iOffsetAdd = 0x80;
                    break;
                case REG_TYPE_K:
                    iOffsetAdd = 0xA4;
                    break;
                case REG_TYPE_L:
                    iOffsetAdd = 0xB4;
                    break;
                case REG_TYPE_F:
                    iOffsetAdd = 0xC4;
                    break;
                case REG_TYPE_T:
                    iOffsetAdd = 0xE4;
                    break;
                case REG_TYPE_C:
                    iOffsetAdd = 0xEA;
                    break;
                default:
                    iOffsetAdd = 0xA0;
                    break;
                }

                nAddrValue = (mAddrInfo.nAddrValue/8)*8;//��ʼ��ַ 4 5
                nAddrValue = nAddrValue/8 + iOffsetAdd;

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)//��ȡ���� 6 7
                {
                    nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
                }
                else
                {
                    nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
                }

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x04;//���� 8
                mSendPackge.sendDataList.push_back(nTmpValue);

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.nReturnLen = 5 + nAddrLen*2;
                break;
            }
        case REG_TYPE_D:
        case REG_TYPE_TV:
        case REG_TYPE_CV:
            {
                /*�ڶ��η��Ͱ�*/
                mSendPackge.sendDataList.clear();

                nTmpValue = 0x02;//ǰ��4���̶� 0 1 2 3
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x67;
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x46;
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x43;
                mSendPackge.sendDataList.push_back(nTmpValue);

                switch (eRegType)
                {
                case REG_TYPE_D:
                    iOffsetAdd = 0x80;
                    break;
                case REG_TYPE_TV:
                    iOffsetAdd = 0x00;
                    break;
                case REG_TYPE_CV:
                    iOffsetAdd = 0x60;
                    break;
                default:
                    iOffsetAdd = 0x80;
                    break;
                }

                nAddrValue = mAddrInfo.nAddrValue*2 + iOffsetAdd;//��ʼ��ַ 4 5

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nAddrLen = mAddrInfo.nAddrLen*2;

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x04;//���� 8
                mSendPackge.sendDataList.push_back(nTmpValue);

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.nReturnLen = 5 + mAddrInfo.nAddrLen*4;
                break;
            }
        default:
            {
                return false;
                break;
            }
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
    /*��ʼ��*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 2 ;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    uchar nTmpValue = 0;
    if(mSendPackge.nPkgIndex == 1)
    {
        nTmpValue = 0x05;
        mSendPackge.sendDataList.push_back(nTmpValue);
        mSendPackge.nReturnLen = 1;
    }
    else if(mSendPackge.nPkgIndex == 2)
    {
        bool bSuccess = false;
        uint nAddrValue = 0;
        int nAddrLen = 1;
        int iOffsetAdd = 0;
        REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
        switch(eRegType)
        {
        case REG_TYPE_P:
        case REG_TYPE_M:
        case REG_TYPE_K:
        case REG_TYPE_L:
        case REG_TYPE_F:
        case REG_TYPE_T:
        case REG_TYPE_C:
            {
                /*�ڶ��η��Ͱ�*/
                mSendPackge.sendDataList.clear();

                mSendPackge.sendDataList.push_back(0x02);//ǰ��4���̶� 0 1 2 3

                if (mAddrInfo.nValueList.size() > 0)
                {
                    nTmpValue = mAddrInfo.nValueList.at(0);
                    if(nTmpValue == 1)
                    {
                        mSendPackge.sendDataList.push_back(0x6F);
                    }
                    else
                    {
                        mSendPackge.sendDataList.push_back(0x70);
                    }
                }

                mSendPackge.sendDataList.push_back(0x46);
                mSendPackge.sendDataList.push_back(0x42);

                switch (eRegType)
                {
                case REG_TYPE_P:
                    iOffsetAdd = 0xA0;
                    break;
                case REG_TYPE_M:
                    iOffsetAdd = 0x80;
                    break;
                case REG_TYPE_K:
                    iOffsetAdd = 0xA4;
                    break;
                case REG_TYPE_L:
                    iOffsetAdd = 0xB4;
                    break;
                case REG_TYPE_F:
                    iOffsetAdd = 0xC4;
                    break;
                case REG_TYPE_T:
                    iOffsetAdd = 0xE4;
                    break;
                case REG_TYPE_C:
                    iOffsetAdd = 0xEA;
                    break;
                default:
                    iOffsetAdd = 0xA0;
                    break;
                }

                nAddrValue = mAddrInfo.mAddrProp.nAddrValue/8 + iOffsetAdd;//��ʼ��ַ 4 5

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nAddrLen = mAddrInfo.mAddrProp.nAddrValue%8;
                if (mAddrInfo.nValueList.size() > 0)
                {
                    nTmpValue = mAddrInfo.nValueList.at(0);
                    if(nTmpValue == 1)
                    {
                        nAddrLen = (0x01 << nAddrLen) & 0xff;
                    }
                    else
                    {
                        nAddrLen = ~(0x01 << nAddrLen) & 0xff;
                    }
                }

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);//��λ�͸�λ��λ��
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 0x04;//���� 8
                mSendPackge.sendDataList.push_back(nTmpValue);

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.nReturnLen = 6;
                bSuccess = true;
                break;
            }
        case REG_TYPE_D:
        case REG_TYPE_TV:
        case REG_TYPE_CV:
            {
                /*�ڶ��η��Ͱ�*/
                mSendPackge.sendDataList.clear();

                mSendPackge.sendDataList.push_back(0x02);//ǰ��4���̶� 0 1 2 3
                mSendPackge.sendDataList.push_back(0x77);
                mSendPackge.sendDataList.push_back(0x46);
                mSendPackge.sendDataList.push_back(0x43);

                switch (eRegType)
                {
                case REG_TYPE_D:
                    iOffsetAdd = 0x80;
                    break;
                case REG_TYPE_TV:
                    iOffsetAdd = 0x00;
                    break;
                case REG_TYPE_CV:
                    iOffsetAdd = 0x60;
                    break;
                default:
                    iOffsetAdd = 0x80;
                    break;
                }

                nAddrValue = mAddrInfo.mAddrProp.nAddrValue*2 + iOffsetAdd;//��ʼ��ַ 4 5

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrValue >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nAddrLen = mAddrInfo.mAddrProp.nAddrLen*2;

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((nAddrLen >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                int size = mAddrInfo.nValueList.size();
                for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)
                {
                    if ((i*2 + 1) < size)
                    {
                        nTmpValue = mAddrInfo.nValueList.at(i*2 + 0) & 0xff;//��8λ

                        uchar ucTmp = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                        mSendPackge.sendDataList.push_back(ucTmp);

                        ucTmp = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                        mSendPackge.sendDataList.push_back(ucTmp);

                        nTmpValue = mAddrInfo.nValueList.at(i*2 + 1) & 0xff;//��8λ

                        ucTmp = PlcMathMethod::intToAsciiValue((nTmpValue >> 4) & 0xf);
                        mSendPackge.sendDataList.push_back(ucTmp);

                        ucTmp = PlcMathMethod::intToAsciiValue((nTmpValue >> 0) & 0xf);
                        mSendPackge.sendDataList.push_back(ucTmp);
                    }
                }

                mSendPackge.sendDataList.push_back(0x04);//����

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.nReturnLen = 6;
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

    /*��վ�ŷ֣�����һ��map�У�վ��Ϊkey*/
    int nTotalSize = mSourceAddrList.size();

    /*��������д���� ���������д����*/
    if(nMaxRWlen > MAX_RW_LEN)
    {
        nMaxRWlen = MAX_RW_LEN;
    }

    ADDR_TARGET_PROP mTmpAddr;

    /*���Ĵ���������*/
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

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_P:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_M:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_K:
            {
                mTmpListBit2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_L:
            {
                mTmpListBit3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_F:
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
        case REG_TYPE_D:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TV:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CV:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*���nMaxRWlen���û����õ�����д���ȣ����ܻ�����Э���е����֧�ֳ��ȱȽ�*/
    mTmpListBit0 = PlcMathMethod::addrBubbleSort(mTmpListBit0);
    if(bWriteAddr)
    {
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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
        /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
        mTmpListBit6 = PlcMathMethod::addrSortOut(mTmpListBit6, 1, 1);
    }
    else
    {
        /*����ַ����֧�ֶ������*/
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

    return true;
}

/******************************************************************
 * Function: ���Ĭ�����Ķ�д����
 * Parameters:
 * Return: ��󳤶�
 ******************************************************************/
int PlcProtocol::getMaxRwLen()
{
    int nMaxRWlen = 64;
    return nMaxRWlen;
}

/******************************************************************
 * Function: У���ַ����ĵ�ַ
 * Parameters: null
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp)
{
    /*ȡ�Ĵ����������б�*/
    int nStartPos = -1;
    vector<string > regNameList = getRegNameList();
    int nSize = regNameList.size();

    /*һ�����ж�*/
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

        /*��������Ĵ�������*/
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

    /*ȡ��һ����ʣ�µıȽ�*/
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

    /*ȡ�Ĵ������ֺ͵�ֵַ�ĳ���*/
    int nNameLen = sRegName.length();
    int nValueLen = sAddrStr.length() - (nStartPos + nNameLen);
    if(nValueLen > 0)
    {
        /*ȡ��ַ��ֵ*/
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
 * Function: ��õ�ǰPLC������
 * Parameters: null
 * Return: null
 ******************************************************************/
PLC_FUN_TYPE PlcProtocol::getPlcType()
{
    return PLC_MASTER;
}

/******************************************************************
 * Function: ���Ĭ��վ��
 * Parameters: null
 * Return: Ĭ��վ��
 ******************************************************************/
int PlcProtocol::getDefaultStation()
{
    int nStation = 0;
    return nStation;
}

/******************************************************************
 * Function: ���Ĭ�ϱ�Э���Ĭ�ϴ�����Ϣ
 * Parameters: null
 * Return: SERIAL_PORT_PROP�ṹ�壬������Ϣ
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
 * Function: ���Ĭ�ϳ�ʱʱ��
 * Parameters: null
 * Return: Ĭ�ϳ�ʱʱ��,��λ��ms
 ******************************************************************/
int PlcProtocol::getDefaultTimeout()
{
    int nTimeout = 1000;
    return nTimeout;
}

/******************************************************************
 * Function: ���Ĭ����С�ɼ�����
 * Parameters: null
 * Return: Ĭ����С�ɼ�����,��λ������
 ******************************************************************/
int PlcProtocol::getDefaultCollectRate()
{
    int nCollectRate = 1;
    return nCollectRate;
}

/******************************************************************
 * Function: ���֡���ʱ��
 * Parameters: null
 * Return: ���֡���ʱ��,��λ������
 ******************************************************************/
int PlcProtocol::getDefaultIntervalTime()
{
    int nIntervalTime = 10;
    return nIntervalTime;
}

/******************************************************************
 * Function: ��õ�ַ�Ƿ�ɶ�
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_P:
    case REG_TYPE_M:
    case REG_TYPE_K:
    case REG_TYPE_L:
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
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
 * Function: ��õ�ַ�Ƿ��д
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_P:
    case REG_TYPE_M:
    case REG_TYPE_K:
    case REG_TYPE_L:
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
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
 * Funciton: int ����ת���ɼĴ�����ö������
 * Parameters:
 * Return: ת���Ľ��
*************************************************************************/
REG_TYPE_INDEX PlcProtocol::getRegTypeFromInt(int nRegIndex)
{
    REG_TYPE_INDEX eRegType = REG_TYPE_NULL;
    switch(nRegIndex)
    {
    case 0:
        {
            eRegType = REG_TYPE_P;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_M;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_K;
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
            eRegType = REG_TYPE_D;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 9:
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
 * Funciton: ͨ�����ֻ�üĴ������±��
 * Parameters:
 * Return: ת���Ľ��
*************************************************************************/
bool PlcProtocol::getRegIndexFromName(string sRegName, int &nRegIndex)
{
    if(sRegName.empty()) return false;

    if(DealWithString::compare("P", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("M", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("K", sRegName)))
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
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
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
 * Funciton: ��ȡЭ�����������
 * Parameters:
 * Return: Э������
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return SERIAL_PORT;
}


