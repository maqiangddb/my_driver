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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_N:
    case REG_TYPE_SW:
    case REG_TYPE_NW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048 + 1)
            {
                return false;
            }

            mAddrTarget.nAddrValue -= 1;
            if (mAddrTarget.nAddrValue < 0)
            {
                return false;
            }

            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_8192 + 1)
            {
                return false;
            }

            mAddrTarget.nAddrValue -= 1;
            if (mAddrTarget.nAddrValue < 0)
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

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096 + 1)
            {
                return false;
            }

            mAddrTarget.nAddrValue -= 1;
            if (mAddrTarget.nAddrValue < 0)
            {
                return false;
            }

            return true;
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512 + 1)
            {
                return false;
            }

            mAddrTarget.nAddrValue -= 1;
            if (mAddrTarget.nAddrValue < 0)
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
            sRegNameList.push_back("I");
            sRegNameList.push_back("Q");
            sRegNameList.push_back("M");
            sRegNameList.push_back("S");
            sRegNameList.push_back("N");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("NW");
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
            sRegNameList.push_back("I");
            sRegNameList.push_back("Q");
            sRegNameList.push_back("M");
            sRegNameList.push_back("S");
            sRegNameList.push_back("N");
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("NW");
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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_N:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
    case REG_TYPE_NW:
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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_N:
    case REG_TYPE_SW:
    case REG_TYPE_NW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
            if(nStartValue < 1 || nStartValue > ADDR_MAX_VALUE_2048)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_2048 + 1)
                {
                    /*������������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
            if(nStartValue < 1 || nStartValue > ADDR_MAX_VALUE_8192)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_8192 + 1)
                {
                    /*������������*/
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

            /*����Ϊ����*/
            if(nStartValue < 1 || nStartValue > ADDR_MAX_VALUE_32)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096 + 1)
                {
                    /*������������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
            if(nStartValue < 1 || nStartValue > ADDR_MAX_VALUE_512)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_1024 + 1)
                {
                    /*������������*/
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
        /*ȡ��Ч����*/
        targetList.clear();

        int nRcvSize = rcvList.size() - 2;
        for(int i = 3; i < nRcvSize; i++)
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
    if(nRcvSize < 5)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvHighByte = rcvList.at(nRcvSize - 1);
    uchar nRcvLowByte = rcvList.at(nRcvSize - 2);

    vector<uchar> templist;
    templist.clear();
    for (int i=0; i<nRcvSize - 2; i++)
    {
        templist.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getCRCcheck(templist);

    uchar highByte = (nCheckResult >> 8) & 0xff;
    uchar lowByte = nCheckResult & 0xff;

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
    mSendPackge.nTotalPkgNum = 1;
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

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    uchar nFumNum = 0;
    ushort nAddrValue = 0;
    int nAddrLen = 1;
    int nReturnLen = 1;
    int iOffsetAdd = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_S:
    case REG_TYPE_N:
        {
            /*������*/
            nFumNum = 0x01;
            if (eRegType == REG_TYPE_I || eRegType == REG_TYPE_S)
            {
                nFumNum = 0x02;
            }

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            switch (eRegType)
            {
            case REG_TYPE_I:
                iOffsetAdd = 0x0000;                                        //0001��2048 00000��02047(ʮ����)
                break;
            case REG_TYPE_Q:
                iOffsetAdd = 0x0000;                                        //0001��2048 00000��02047
                break;
            case REG_TYPE_M:
                iOffsetAdd = 0x2710;                                        //0001��8192 10000��18191
                break;
            case REG_TYPE_S:
                iOffsetAdd = 0x2710;                                        //0001��4096 10000��14095
                break;
            case REG_TYPE_N:
                iOffsetAdd = 0x4E20;                                        //0001��2048 20000��22047
                break;
            default:
                return false;
                break;
            }

            nAddrValue = (mAddrInfo.nAddrValue/8)*8;
            nAddrValue += iOffsetAdd;

            /*�㷵�ص��ֽڳ���*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nReturnLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nReturnLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*��ȡ�õ�ַ����*/
            nAddrLen = mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen;
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
    case REG_TYPE_NW:
        {
            /*������*/
            nFumNum = 0x03;
            if (eRegType == REG_TYPE_IW || eRegType == REG_TYPE_SW)
            {
                nFumNum = 0x04;
            }

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            switch (eRegType)
            {
            case REG_TYPE_IW:
                iOffsetAdd = 0x0000;                                        //0001��0512 00000��00031(ʮ����)
                break;
            case REG_TYPE_QW:
                iOffsetAdd = 0x4E20;                                        //0001��0512 20000��20511
                break;
            case REG_TYPE_MW:
                iOffsetAdd = 0x0000;                                        //0001��8192 00000��08191
                break;
            case REG_TYPE_SW:
                iOffsetAdd = 0x1388;                                        //0001��2048 05000��07047
                break;
            case REG_TYPE_NW:
                iOffsetAdd = 0x2710;                                        //0001��2048 10000��12047
                break;
            default:
                return false;
                break;
            }

            nAddrValue = mAddrInfo.nAddrValue + iOffsetAdd;

            /*��ȡ�õ�ַ����*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*�㷵�ص��ֽڳ���*/
            nReturnLen = nAddrLen *2;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    /*վ�� vector index = 0*/
    uchar nTmpValue = 0;
    memcpy(&nTmpValue, &mAddrInfo.nPlcStationIndex, 1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*�Թ����븳ֵ vector index = 1*/
    mSendPackge.sendDataList.push_back(nFumNum);

    /*��ʼ��ַ vector index = 2*/
    nTmpValue = (nAddrValue >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 3*/
    nTmpValue = nAddrValue & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*��ȡ����*/
    /*vector index = 4*/
    nTmpValue = (nAddrLen >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 5*/
    nTmpValue = (nAddrLen) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*����У����*/
    ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
    uchar nCrcResultL = nCrcResult & 0xff;
    uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

    /*vector index = 6*/
    mSendPackge.sendDataList.push_back(nCrcResultL);
    /*vector index = 7*/
    mSendPackge.sendDataList.push_back(nCrcResultH);

    /*�������ݵĳ���*/
    if(mAddrInfo.nPlcStationIndex == 0)
    {
        mSendPackge.nReturnLen = 0;
    }
    else
    {
        mSendPackge.nReturnLen = nReturnLen + 5;
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

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    /*վ�� vector index = 0*/
    uchar nTmpValue = 0;
    memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    if(nTmpValue == 0)
    {
        mSendPackge.nReturnLen = 0;
    }
    else
    {
        mSendPackge.nReturnLen = 8;
    }

    uchar nFumNum = 0;
    bool bSuccess = false;
    int iOffsetAdd = 0;
    int iSendAdd = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_N:
        {
            /*�Թ����븳ֵ vector index = 1*/
            nFumNum = 0x05;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*��ʼ��ַ vector index = 2*/
            switch (eRegType)
            {
            case REG_TYPE_Q:
                iOffsetAdd = 0x0000;                                        //0001��2048 00000��02047
                break;
            case REG_TYPE_M:
                iOffsetAdd = 0x2710;                                        //0001��8192 10000��18191
                break;
            case REG_TYPE_N:
                iOffsetAdd = 0x4E20;                                        //0001��2048 20000��22047
                break;
            default:
                return false;
                break;
            }

            iSendAdd = mAddrInfo.mAddrProp.nAddrValue + iOffsetAdd;

            nTmpValue = (iSendAdd >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = iSendAdd & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*��ֵ*/
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
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_NW:
        {
            switch (eRegType)
            {
            case REG_TYPE_QW:
                iOffsetAdd = 0x4E20;                                        //0001��0512 20000��20511
                break;
            case REG_TYPE_MW:
                iOffsetAdd = 0x0000;                                        //0001��8192 00000��08191
                break;
            case REG_TYPE_NW:
                iOffsetAdd = 0x2710;                                        //0001��2048 10000��12047
                break;
            default:
                return false;
                break;
            }

            iSendAdd = mAddrInfo.mAddrProp.nAddrValue + iOffsetAdd;

            if(1 == mAddrInfo.mAddrProp.nAddrLen)
            {
                /*�Թ����븳ֵ vector index = 1*/
                nFumNum = 0x06;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*��ʼ��ַ vector index = 2*/
                nTmpValue = (iSendAdd >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = iSendAdd & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*��ֵ*/
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
                /*�Թ����븳ֵ vector index = 1*/
                nFumNum = 0x10;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*��ʼ��ַ vector index = 2*/
                nTmpValue = (iSendAdd >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = iSendAdd & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*��ַ����*/
                /*vector index = 4*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 5*/
                nTmpValue = mAddrInfo.mAddrProp.nAddrLen & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 6*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen*2) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                mAddrInfo.nValueList.resize(mAddrInfo.mAddrProp.nAddrLen * 2);

                for(int i = 0; i < mAddrInfo.mAddrProp.nAddrLen; i++)
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
    default:
        {
            break;
        }
    }

    /*����У����*/
    ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
    uchar nCrcResultL = nCrcResult & 0xff;
    uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

    mSendPackge.sendDataList.push_back(nCrcResultL);
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

    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_I:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Q:
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
        case REG_TYPE_N:
            {
                mTmpListBit4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_IW:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_QW:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MW:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SW:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_NW:
            {
                mTmpListWord4.push_back(mTmpAddr);
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
    int nStation = 1;
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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_N:
    case REG_TYPE_SW:
    case REG_TYPE_NW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8192)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
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
    case REG_TYPE_Q:
    case REG_TYPE_N:
    case REG_TYPE_NW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8192)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_QW:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
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
            eRegType = REG_TYPE_I;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_Q;
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
            eRegType = REG_TYPE_N;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_IW;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_QW;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_MW;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_SW;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_NW;
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

    if(DealWithString::compare("I", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("Q", sRegName)))
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
    else if((DealWithString::compare("N", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("IW", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("QW", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("MW", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("SW", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("NW", sRegName)))
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