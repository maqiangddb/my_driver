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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue, bSuccess, 16);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_L:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue, bSuccess, 16);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue, bSuccess, 16);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 33280)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue, bSuccess, 16);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1024)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue, bSuccess, 16);
            if (!bSuccess)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 8192)
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
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("M");
            sRegNameList.push_back("SM");
            sRegNameList.push_back("L");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
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
            sRegNameList.push_back("X");
            sRegNameList.push_back("Y");
            sRegNameList.push_back("M");
            sRegNameList.push_back("SM");
            sRegNameList.push_back("L");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
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
    case REG_TYPE_D:
    case REG_TYPE_SD:
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_2048)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_2048)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
    case REG_TYPE_L:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= 33280)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 33280)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_1024)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_1024)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
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
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 16);
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
                    sprintf(buf, "%x", nStartValue);
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
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    if(nRcvSize < 3)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvSUMByte = rcvList.at(nRcvSize - 1);//the last
    uchar nRcvLRCByte = rcvList.at(nRcvSize - 2);

    vector<unsigned char> tmprcvList;
    for (int i=1; i<(nRcvSize - 2); i++)
    {
        tmprcvList.push_back(rcvList.at(i));
    }

    ushort nCheckLRCResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;
    ushort nCheckSUMResult = PlcMathMethod::getSumCheck(tmprcvList) & 0xff;

    if(nRcvLRCByte == nCheckLRCResult && nRcvSUMByte == nCheckSUMResult)
    {
        /*ȡ��Ч����*/
        targetList.clear();

        int size = nRcvSize - 2;
        for(int i = 7; i < size; i++)
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
    if(nRcvSize < 3)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvSUMByte = rcvList.at(nRcvSize - 1);//the last
    uchar nRcvLRCByte = rcvList.at(nRcvSize - 2);

    vector<unsigned char> tmprcvList;
    for (int i=1; i<(nRcvSize - 2); i++)
    {
        tmprcvList.push_back(rcvList.at(i));
    }

    ushort nCheckLRCResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;
    ushort nCheckSUMResult = PlcMathMethod::getSumCheck(tmprcvList) & 0xff;

    if(nRcvLRCByte == nCheckLRCResult && nRcvSUMByte == nCheckSUMResult)
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

    ushort nAddrValue = 0;
    int nAddrLen = 1;

    /*�̶� index = 0 1 2 3 4 5*/
    uchar nTmpValue = 0;
    mSendPackge.sendDataList.push_back(0x7e);
    mSendPackge.sendDataList.push_back(0x0b);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_L:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*�Ĵ������� index = 6*/
            switch (eRegType)
            {
            case REG_TYPE_X:
                mSendPackge.sendDataList.push_back(0x01);
                break;
            case REG_TYPE_Y:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            case REG_TYPE_M:
                mSendPackge.sendDataList.push_back(0x02);
                break;
            case REG_TYPE_SM:
                mSendPackge.sendDataList.push_back(0x06);
                break;
            case REG_TYPE_L:
                mSendPackge.sendDataList.push_back(0x03);
                break;
            case REG_TYPE_T:
                mSendPackge.sendDataList.push_back(0x04);
                break;
            case REG_TYPE_C:
                mSendPackge.sendDataList.push_back(0x05);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            }

            /*��ַ index = 7 8 */
            nAddrValue = (mAddrInfo.nAddrValue/8)*8;
            nAddrValue = nAddrValue/16;

            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nAddrValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*���� index = 9 10 */
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 + 1;
            }

            if (nAddrLen % 2 == 0)
            {
                nAddrLen = nAddrLen/2;

            }
            else
            {
                nAddrLen = nAddrLen/2 + 1;
            }
            nTmpValue = (nAddrLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nAddrLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*�Ĵ������� index = 6*/
            switch (eRegType)
            {
            case REG_TYPE_D:
                mSendPackge.sendDataList.push_back(0x0c);
                break;
            case REG_TYPE_SD:
                mSendPackge.sendDataList.push_back(0x0f);
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back(0x0a);
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back(0x0b);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x0c);
                break;
            }

            /*��ַ index = 7 8 */
            nAddrValue = mAddrInfo.nAddrValue;
            nTmpValue = (nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nAddrValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*���� index = 9 10 */
            nAddrLen = mAddrInfo.nAddrLen;
            nTmpValue = (nAddrLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (nAddrLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    /*У�� index = 11 12 */
    vector<unsigned char> tmprcvList;
    int size = mSendPackge.sendDataList.size();
    for (int i=1; i<size; i++)
    {
        tmprcvList.push_back(mSendPackge.sendDataList.at(i));
    }

    ushort nCheckLRCResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;
    mSendPackge.sendDataList.push_back(nCheckLRCResult);

    ushort nCheckSUMResult = (PlcMathMethod::getSumCheck(tmprcvList) & 0xff) - 1;
    mSendPackge.sendDataList.push_back(nCheckSUMResult);

    mSendPackge.nReturnLen = 9 + nAddrLen*2;

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

    bool bSuccess = false;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_L:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*�̶� index = 0 1 2 3 4 5*/
            uchar nTmpValue = 0;
            mSendPackge.sendDataList.push_back(0x7e);
            mSendPackge.sendDataList.push_back(0x0b);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x01);
            mSendPackge.sendDataList.push_back(0x02);

            /*�Ĵ������� index = 6*/
            switch (eRegType)
            {
            case REG_TYPE_X:
                mSendPackge.sendDataList.push_back(0x01);
                break;
            case REG_TYPE_Y:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            case REG_TYPE_M:
                mSendPackge.sendDataList.push_back(0x02);
                break;
            case REG_TYPE_SM:
                mSendPackge.sendDataList.push_back(0x06);
                break;
            case REG_TYPE_L:
                mSendPackge.sendDataList.push_back(0x03);
                break;
            case REG_TYPE_T:
                mSendPackge.sendDataList.push_back(0x04);
                break;
            case REG_TYPE_C:
                mSendPackge.sendDataList.push_back(0x05);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            }

            /*��ַ index = 7 8 9*/
            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 6) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 3) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = mAddrInfo.mAddrProp.nAddrValue & 0X07;

            /*��ֵ*/
            nTmpValue = mAddrInfo.nValueList.front();
            if(nTmpValue == 1)
            {
                mSendPackge.sendDataList.push_back(0x01);
            }
            else
            {
                mSendPackge.sendDataList.push_back(0x00);
            }

            /*У�� index = 11 12 */
            vector<unsigned char> tmprcvList;
            int size = mSendPackge.sendDataList.size();
            for (int i=1; i<size; i++)
            {
                tmprcvList.push_back(mSendPackge.sendDataList.at(i));
            }

            ushort nCheckLRCResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;
            mSendPackge.sendDataList.push_back(nCheckLRCResult);

            ushort nCheckSUMResult = (PlcMathMethod::getSumCheck(tmprcvList) & 0xff) - 1;
            mSendPackge.sendDataList.push_back(nCheckSUMResult);

            mSendPackge.nReturnLen = 7;
            bSuccess = true;
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*�̶� index = 0 1 2 3 4 5*/
            uchar nTmpValue = 0;
            mSendPackge.sendDataList.push_back(0x7e);
            mSendPackge.sendDataList.push_back((mAddrInfo.mAddrProp.nAddrLen*2) & 0xff);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x01);
            mSendPackge.sendDataList.push_back(0x00);

            /*�Ĵ������� index = 6*/
            switch (eRegType)
            {
            case REG_TYPE_D:
                mSendPackge.sendDataList.push_back(0x0c);
                break;
            case REG_TYPE_SD:
                mSendPackge.sendDataList.push_back(0x0f);
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back(0x0a);
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back(0x0b);
                break;
            default:
                mSendPackge.sendDataList.push_back(0x0c);
                break;
            }

            /*��ַ index = 7 8 */
            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*���� index = 9 10 */
            nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = (mAddrInfo.mAddrProp.nAddrLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*��ֵ index = 11 12... */
            for(int i = 0; i < nTmpValue; i++)
            {
                int size = mAddrInfo.nValueList.size();
                if ((i*2 + 1) < size)
                {
                    nTmpValue = mAddrInfo.nValueList.at(2*i + 0);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(2*i + 1);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
            }
            /*У�� index = 13+len*2 14+len*2 */
            vector<unsigned char> tmprcvList;
            int size = mSendPackge.sendDataList.size();
            for (int i=1; i<size; i++)
            {
                tmprcvList.push_back(mSendPackge.sendDataList.at(i));
            }

            ushort nCheckLRCResult = PlcMathMethod::getLRCcheck(tmprcvList) & 0xff;
            mSendPackge.sendDataList.push_back(nCheckLRCResult);

            ushort nCheckSUMResult = (PlcMathMethod::getSumCheck(tmprcvList) & 0xff) - 1;
            mSendPackge.sendDataList.push_back(nCheckSUMResult);

            mSendPackge.nReturnLen = 7;
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
    vector<ADDR_TARGET_PROP > mTmpListWord3;

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
        case REG_TYPE_L:
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

    mTmpListWord3 = PlcMathMethod::addrBubbleSort(mTmpListWord3);
    mTmpListWord3 = PlcMathMethod::addrSortOut(mTmpListWord3, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord3.at(i));
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
    mSerialInfo.BAUD_RATE = 19200;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 3;
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_L:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 33280)
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1024)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 8192)
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
    case REG_TYPE_Y:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_L:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 33280)
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1024)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 8192)
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
            eRegType = REG_TYPE_L;
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
            eRegType = REG_TYPE_SD;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 10:
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
    else if((DealWithString::compare("L", sRegName)))
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
    else if((DealWithString::compare("SD", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 10;
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


