#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*д���ݷ��صĳ���*/
#define WRITE_RETURN_LEN      8

/*�����ݷ��ص�����ͷ������*/
#define READ_HEAD_LEN         3

/*�����ݷ��ص�����У��ĳ���*/
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,8);

            /*�жϳ����Ƿ���*/
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

            /*�жϳ����Ƿ���*/
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

            /*�жϳ����Ƿ���*/
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

            /*�жϳ����Ƿ���*/
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

            /*�жϳ����Ƿ���*/
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 8);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
                    /*�����������*/
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

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
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

                if(nStartValue < ADDR_MAX_VALUE_200)
                {
                    /*������Ȳ�����������һ��*/
                    if(nStartValue + mTmpAddr.nAddrLen > ADDR_MAX_VALUE_200)
                    {
                        nCopyNum++;
                        continue;
                    }

                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else if(nStartValue <= 255 && nStartValue + mTmpAddr.nAddrLen <= 257)
                {
                    /*�����������*/
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

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
        /*��������*/
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
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*�Ե�ǰ����ĵ�ַ��ֵ*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;
    mCurrAddr.mAddrProp = mAddrInfo.mAddrProp;

    /*������*/
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*������*/
            nFumNum = 0x01;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = (mAddrInfo.nAddrValue/8)*8;

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
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*������*/
            nFumNum = 0x03;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            /*��ȡ�õ�ַ����*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*�㷵�ص��ֽڳ���*/
            nReturnLen = nAddrLen *2;
            break;
        }
    case REG_TYPE_CV:
        {
            /*������*/
            nFumNum = 0x03;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            if(nAddrValue < 200)
            {
                /*��ȡ�õ�ַ����*/
                nAddrLen = mAddrInfo.nAddrLen;

                nReturnLen = nAddrLen *2;

                /*�㷵�ص��ֽڳ���*/
                if(nAddrValue + nAddrLen > 200)
                {
                    return false;
                }
            }
            else if(nAddrValue < 256)
            {
                /*��ȡ�õ�ַ����*/
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
            /*ƫ��10000*/
            nAddrValue += 10000;
            break;
        }
    case REG_TYPE_M:
        {
            /*ƫ��30000*/
            nAddrValue += 30000;
            break;
        }
    case REG_TYPE_T:
        {
            /*ƫ�� 60768*/
            nAddrValue += 60768;
            break;
        }
    case REG_TYPE_C:
        {
            /*ƫ�� 60512*/
            nAddrValue += 60512;
            break;
        }
    case REG_TYPE_D:
        {
            /*ƫ�� 40000*/
            nAddrValue += 40000;
            break;
        }
    case REG_TYPE_TV:
        {
            /*ƫ�� 60256*/
            nAddrValue += 60256;
            break;
        }
    case REG_TYPE_CV:
        {
            /*ƫ�� 60000*/
            nAddrValue += 60000;
            break;
        }
    case REG_TYPE_AI:
        {
            /*ƫ�� 20000*/
            nAddrValue += 20000;
            break;
        }
    case REG_TYPE_AO:
        {
            /*ƫ�� 20512*/
            nAddrValue += 20512;
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
            /*ƫ��10000*/
            nAddrValue += 10000;
            break;
        }
    case REG_TYPE_M:
        {
            /*ƫ��30000*/
            nAddrValue += 30000;
            break;
        }
    case REG_TYPE_T:
        {
            /*ƫ�� 60768*/
            nAddrValue += 60768;
            break;
        }
    case REG_TYPE_C:
        {
            /*ƫ�� 60512*/
            nAddrValue += 60512;
            break;
        }
    case REG_TYPE_D:
        {
            /*ƫ�� 40000*/
            nAddrValue += 40000;
            break;
        }
    case REG_TYPE_TV:
        {
            /*ƫ�� 60256*/
            nAddrValue += 60256;
            break;
        }
    case REG_TYPE_CV:
        {
            /*ƫ�� 60000*/
            nAddrValue += 60000;
            break;
        }
    case REG_TYPE_AI:
        {
            /*ƫ�� 20000*/
            nAddrValue += 20000;
            break;
        }
    case REG_TYPE_AO:
        {
            /*ƫ�� 20512*/
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
            /*�Թ����븳ֵ vector index = 1*/
            nFumNum = 0x05;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*��ʼ��ַ vector index = 2*/
            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = nAddrValue & 0xff;
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
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            if(1 == mAddrInfo.mAddrProp.nAddrLen)
            {
                /*�Թ����븳ֵ vector index = 1*/
                nFumNum = 0x06;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*��ʼ��ַ vector index = 2*/
                nTmpValue = (nAddrValue >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = nAddrValue & 0xff;
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
                nTmpValue = (nAddrValue >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = nAddrValue & 0xff;
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

                int nLen = 0;
                int nValueSize = mAddrInfo.nValueList.size();

                /*ȡ���ͳ���*/
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
            /*�Թ����븳ֵ vector index = 1*/
            nFumNum = 0x10;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*��ʼ��ַ vector index = 2*/
            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = nAddrValue & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*��ַ����*/
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

                /*ȡ���ͳ���*/
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

                /*ȡ���ͳ���*/
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

    /*����У����*/
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

    /*��վ�ŷ֣�����һ��map�У�վ��Ϊkey*/
    int nTotalSize = mSourceAddrList.size();

    /*��������д���� ���������д����*/
    if(nMaxRWlen > MAX_RW_LEN)
    {
        nMaxRWlen = MAX_RW_LEN;
    }

    ADDR_TARGET_PROP mTmpAddr;

    /*���Ĵ���������*/
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

    /*���nMaxRWlen���û����õ�����д���ȣ����ܻ�����Э���е����֧�ֳ��ȱȽ�*/
    /*REG_TYPE_X ~ REG_TYPE_C*/
    for(int i = 0; i < 5; i++)
    {
        mTmpList[i] = PlcMathMethod::addrBubbleSort(mTmpList[i]);
        if(bWriteAddr)
        {
            /*д��ַ����ֻ֧�ֳ���Ϊ1��д*/
            mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], 1, 1);
        }
        else
        {
            /*����ַ����֧�ֶ������*/
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

    /*CV�Ĵ�����56��*/
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
    /*ֻ���������ּĴ���֧��д*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
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
 * Function: ��õ�ַ�Ƿ��д
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    /*ֻ���������ּĴ���֧��д*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
        {
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
            if(mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            /*�жϳ����Ƿ���*/
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
            /*�жϳ����Ƿ���*/
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
 * Funciton: ͨ�����ֻ�üĴ������±��
 * Parameters:
 * Return: ת���Ľ��
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
 * Funciton: ��ȡЭ�����������
 * Parameters:
 * Return: Э������
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return SERIAL_PORT;
}
