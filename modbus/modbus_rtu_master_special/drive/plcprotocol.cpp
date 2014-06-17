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
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            /*��"."���*/
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,".");
            if(strList.size() == 2)
            {
                string sIntStr = strList.at(0);
                string sDecStr = strList.at(1);

                /*��λ���ֵ�ת��*/
                int nIntValue = DealWithString::stringToInt(sIntStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*��λ���ֵ�ת��*/
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
                if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
                {
                    return false;
                }

                /*���Ϻ�Ľ��*/
                mAddrTarget.nAddrValue = nIntValue*16 + nDecValue;

                /*�жϳ����Ƿ���*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen >= ADDR_MAX_VALUE_65536 * 16)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    default:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen >= ADDR_MAX_VALUE_65536)
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
    case REG_TYPE_0X:                   //REG_TYPE_0X:
    case REG_TYPE_1X:                   //REG_TYPE_1X:
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_3X:                   //REG_TYPE_3X:
    case REG_TYPE_4X:                   //REG_TYPE_4X:
    case REG_TYPE_3X_DB:                   //REG_TYPE_3X_DB:
    case REG_TYPE_4X_DB:                   //REG_TYPE_4X_DB:
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
    case REG_TYPE_3X_BIT:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:                   //REG_TYPE_4X_BIT:
        {
            /*����.�����*/
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,".");
            if(strList.size() == 2)
            {
                string sIntStr = strList.at(0);
                string sDecStr = strList.at(1);

                /*��λ���ֵ�ת��*/
                int nIntValue = DealWithString::stringToInt(sIntStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*��λ���ֵ�ת��*/
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
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

                    /*���Ϻ�Ľ��*/
                    int nStartValue = nIntValue*16 + nDecValue;
                    if(nStartValue + mTmpAddr.nAddrLen < ADDR_MAX_VALUE_65536 * 16)
                    {
                        /*�����������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr + ".";

                        /*���С������*/
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
    case REG_TYPE_0X:                   //REG_TYPE_0X:
    case REG_TYPE_1X:                   //REG_TYPE_1X:
    case REG_TYPE_3X:                   //REG_TYPE_3X:
    case REG_TYPE_4X:                   //REG_TYPE_4X:
    case REG_TYPE_3X_DB:                //REG_TYPE_3X_DB:
    case REG_TYPE_4X_DB:                //REG_TYPE_4X_DB:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_65536)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen < ADDR_MAX_VALUE_65536)
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
    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    //    if(nValueList.empty()) return mSendPackge;

    /*�Ե�ǰ����ĵ�ַ��ֵ*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;
    mCurrAddr.mAddrProp = mAddrInfo.mAddrProp;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);

    /*������*/
    char nFumNum = 0;
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

                /*վ�� vector index = 0*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*�Թ����븳ֵ vector index = 1*/
                nFumNum = 0x03;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*��ʼ��ַ vector index = 2*/
                nTmpValue = ((mAddrInfo.mAddrProp.nAddrValue/16) >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue/16) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*��ȡ����*/
                /*�㷵�ص��ֽڳ���*/
                int nAddrLen = 0;
                if((mAddrInfo.mAddrProp.nAddrValue%16 + mAddrInfo.mAddrProp.nAddrLen)%16 == 0)
                {
                    nAddrLen = (mAddrInfo.mAddrProp.nAddrValue%16 + mAddrInfo.mAddrProp.nAddrLen)/16;
                }
                else
                {
                    nAddrLen = (mAddrInfo.mAddrProp.nAddrValue%16 + mAddrInfo.mAddrProp.nAddrLen)/16 +1;
                }

                /*vector index = 4*/
                mSendPackge.sendDataList.push_back((nAddrLen >> 8) & 0xff);

                /*vector index = 5*/
                mSendPackge.sendDataList.push_back(nAddrLen & 0xff);

                /*����У����*/
                ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
                uchar nCrcResultL = nCrcResult & 0xff;
                uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

                /*vector index = 6*/
                mSendPackge.sendDataList.push_back(nCrcResultL);
                /*vector index = 7*/
                mSendPackge.sendDataList.push_back(nCrcResultH);

                /*�������ݵĳ���*/
                mSendPackge.nReturnLen = nAddrLen *2 + READ_HEAD_LEN + CHECK_LEN;
            }
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            if(mAddrInfo.nValueList.empty() || mCurrAddr.nValueList.empty()) break;
            if(mAddrInfo.nValueList.size() < 2)
            {
                mAddrInfo.nValueList.push_back(0);
            }

            if(REG_TYPE_4X_BIT == eRegType)
            {
                /*վ�� vector index = 0*/
                uchar nTmpValue = 0;
                memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*�Թ����븳ֵ vector index = 1*/
                nFumNum = 0x10;
                mSendPackge.sendDataList.push_back(nFumNum);

                /*��ʼ��ַ vector index = 2*/
                nTmpValue = ((mAddrInfo.mAddrProp.nAddrValue/16) >> 8) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = (mAddrInfo.mAddrProp.nAddrValue/16) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*��ַ����*/
                /*vector index = 4*/
                mSendPackge.sendDataList.push_back(0x00);

                /*vector index = 5*/
                mSendPackge.sendDataList.push_back(0x01);

                /*vector index = 6*/
                mSendPackge.sendDataList.push_back(0x02);

                /*����ֵ ��λ��ǰ*/
                int nBitPos = mAddrInfo.mAddrProp.nAddrValue%16;
                if(nBitPos >=0 && nBitPos < 8)
                {
                    /*����ڵ�λ����ֵ�����λ�����ȴ�*/
                    nTmpValue = mAddrInfo.nValueList.at(0);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    /*����ڵ�λ����ֵ�����λ����ֵ*/
                    nTmpValue = mAddrInfo.nValueList.at(1);
                    if(mCurrAddr.nValueList.front() > 0)
                    {
                        nTmpValue |= 1 << nBitPos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPos);
                    }
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
                else if(nBitPos >= 8 && nBitPos < 16)
                {
                    nBitPos = nBitPos -8;

                    /*����ڸ�λ����ֵ�����λ����ֵ*/
                    nTmpValue = mAddrInfo.nValueList.at(0);
                    if(mCurrAddr.nValueList.front() > 0)
                    {
                        nTmpValue |= 1 << nBitPos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPos);
                    }
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    /*����ڸ�λ����ֵ�����λ����*/
                    nTmpValue = mAddrInfo.nValueList.at(1);
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }
                else
                {
                    mSendPackge.sendDataList.clear();
                    break;
                }

                /*����У����*/
                ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPackge.sendDataList);
                uchar nCrcResultL = nCrcResult & 0xff;
                uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

                /*vector index = 6*/
                mSendPackge.sendDataList.push_back(nCrcResultL);
                /*vector index = 7*/
                mSendPackge.sendDataList.push_back(nCrcResultH);

                /*�������ݵĳ���*/
                if(mAddrInfo.mAddrProp.nPlcStationIndex == 0)
                {
                    mSendPackge.nReturnLen = 0;
                }
                else
                {
                    mSendPackge.nReturnLen = WRITE_RETURN_LEN;
                }
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
    case REG_TYPE_0X:
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
    case REG_TYPE_1X:
        {
            /*������*/
            nFumNum = 0x02;

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
    case REG_TYPE_3X_BIT:
        {
            /*������*/
            nFumNum = 0x04;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue/16;

            /*��ȡ�õ�ַ����*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            /*�㷵�ص��ֽڳ���*/
            nReturnLen = nAddrLen *2;
            break;
        }
    case REG_TYPE_3X:
    case REG_TYPE_3X_DB:
        {
            /*������*/
            nFumNum = 0x04;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue;

            /*��ȡ�õ�ַ����*/
            nAddrLen = mAddrInfo.nAddrLen;

            /*�㷵�ص��ֽڳ���*/
            nReturnLen = nAddrLen *2;
            break;
        }
    case REG_TYPE_4X_BIT:
        {
            /*������*/
            nFumNum = 0x03;

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue/16;

            /*��ȡ�õ�ַ����*/
            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            /*�㷵�ص��ֽڳ���*/
            nReturnLen = nAddrLen *2;
            break;
        }
    case REG_TYPE_4X:
    case REG_TYPE_4X_DB:
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
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:
        {
            /*�Թ����븳ֵ vector index = 1*/
            nFumNum = 0x05;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*��ʼ��ַ vector index = 2*/
            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = mAddrInfo.mAddrProp.nAddrValue & 0xff;
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
    case REG_TYPE_4X_BIT:
        {
            mCmnProp.eSendType = CMN_READ_BEFORE_WRITE;
            return makePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case REG_TYPE_4X:
    case REG_TYPE_4X_DB:
        {
            /*�Թ����븳ֵ vector index = 1*/
            nFumNum = 0x10;
            mSendPackge.sendDataList.push_back(nFumNum);

            /*��ʼ��ַ vector index = 2*/
            nTmpValue = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 3*/
            nTmpValue = mAddrInfo.mAddrProp.nAddrValue & 0xff;
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
            if(REG_TYPE_4X == eRegType)
            {
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
            else if(REG_TYPE_4X_DB == eRegType)
            {
                /*ȡ���ͳ���*/
                nLen = mAddrInfo.mAddrProp.nAddrLen / 2;
                if(4*nLen > nValueSize)
                {
                    nLen = nValueSize/4;
                }

                for(int i = 0; i < nLen; i++)
                {
                    nTmpValue = mAddrInfo.nValueList.at(4*i + 3);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 2);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 1);
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    nTmpValue = mAddrInfo.nValueList.at(4*i + 0);
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
    vector<ADDR_TARGET_PROP > mTmpListBit0;
    vector<ADDR_TARGET_PROP > mTmpListBit1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;

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
                /*ת���Ĵ������*/
                mTmpAddr.nRegIndex = REG_TYPE_3X;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
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
        case REG_TYPE_4X_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_4X;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
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
        case REG_TYPE_3X:
        case REG_TYPE_3X_DB:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_4X:
        case REG_TYPE_4X_DB:
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
    mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 8);
    tmpSize = mTmpListBit1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit1.at(i));
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
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536 * 16)
            {
                return false;
            }
            break;
        }
    default:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            break;
        }
    }

    return true;
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
    case REG_TYPE_4X_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536 * 16)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_0X:
    case REG_TYPE_4X:
    case REG_TYPE_4X_DB:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen >= ADDR_MAX_VALUE_65536)
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
            eRegType = REG_TYPE_3X_DB;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_4X_DB;
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
 * Funciton: ��ȡЭ�����������
 * Parameters:
 * Return: Э������
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return SERIAL_PORT;
}

