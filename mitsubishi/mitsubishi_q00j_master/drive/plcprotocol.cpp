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
    case REG_TYPE_B:
    case REG_TYPE_W:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess, 16);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_SB:
    case REG_TYPE_SW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess, 16);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1024)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_M:
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
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_2048)
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
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 11135)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_Z:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 10)
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
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("V");
            sRegNameList.push_back("S");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("W");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("Z");
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
            sRegNameList.push_back("L");
            sRegNameList.push_back("F");
            sRegNameList.push_back("V");
            sRegNameList.push_back("S");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D");
            sRegNameList.push_back("W");
            sRegNameList.push_back("SW");
            sRegNameList.push_back("Z");
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
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
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
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
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
    case REG_TYPE_B:
    case REG_TYPE_W:
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
    case REG_TYPE_SB:
    case REG_TYPE_SW:
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
    case REG_TYPE_M:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= 8191)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 8191)
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
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
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
    case REG_TYPE_CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_512)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_512)
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
    case REG_TYPE_Z:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= 10)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 10)
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
            if(nStartValue < 0 || nStartValue >= 11136)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 11136)
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
CMN_RW_ERROR_CODE PlcProtocol::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = checkRcvData(mUnpackProp, rcvList, CHECK_READ);
    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    if(mUnpackProp.bNeedSetData)
    {
        /*ȡ��Ч����*/
        int nSize = rcvList.size();
        vector<uchar> nTmpRcvList ;
        nTmpRcvList.clear();
        for(int i = 0; i < nSize; i++)
        {
            nTmpRcvList.push_back(rcvList.at(i));
            if(i < nSize -1)
            {
                if(rcvList.at(i) == 0x10 && rcvList.at(i+1) == 0x10)
                {
                    i++;
                }
            }
        }

        targetList.clear();
        nSize = nTmpRcvList.size();
        for(int i = 40; i < nSize - 4; i++)
        {
            targetList.push_back(nTmpRcvList.at(i));
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
    if(nRcvSize < 44)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    mUnpackProp.bNeedSetData = false;

    /*�����������ǽ�����0x03*/
    if(rcvList.at(nRcvSize -3) != 0x03)
    {
        eCmnInfo = DATA_CHECK_ERROR;
        return eCmnInfo;
    }

    uchar nRcvResultL = rcvList.at(nRcvSize -1);
    uchar nRcvResultH = rcvList.at(nRcvSize -2);

    int nSize = rcvList.size();
    vector<uchar> nTmpRcvList ;
    nTmpRcvList.clear();
    for(int i = 2; i < nSize -4; i++)
    {
        nTmpRcvList.push_back(rcvList.at(i));
        if(rcvList.at(i) == 0x10 && rcvList.at(i+1) == 0x10)
        {
            i++;
        }
    }

    /*����У����*/
    ushort nCrcResult = PlcMathMethod::getSumCheck(nTmpRcvList) & 0xff;
    uchar nCheckL = nCrcResult & 0x0f;
    uchar nCheckH = (nCrcResult >> 4) & 0x0f;
    nCheckL = PlcMathMethod::intToAsciiValue(nCheckL);
    nCheckH = PlcMathMethod::intToAsciiValue(nCheckH);

    if(nRcvResultL == nCheckL && nRcvResultH == nCheckH)
    {
        mUnpackProp.bNeedSetData = true;
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else
    {
        mUnpackProp.bNeedSetData = false;
        eCmnInfo = DATA_CHECK_ERROR;
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: �������Э��İ�
 * Parameters: mHandShakeList, nPkgIndex:��1��ʼ
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    switch(nPkgIndex)
    {
    case 1:
        {
            /*��ջ���*/
            m_nTmpRcvList.clear();

            /*******************��ӵ�һ�����ְ�************************
              * send(9)       00 10 02 ff ff fc 05 10 03
              * return(27)    00 10 02 ff ff fc 0f 50 02 10 03 00 10 02 01 41 f7 4f 58 c6 88 35 2a d5 d9 10 03
              ******************************************************/
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0xff);

            mHandShake.sendDataList.push_back(0xfc);

            /*01��ʾ9600 02��ʾ19200 03��ʾ38400 04��ʾ57600 05��ʾ115200*/
            switch(nBaudRate)
            {
            case 0:
                {
                    mHandShake.sendDataList.push_back(0x01);
                    break;
                }
            case 9600:
                {
                    mHandShake.sendDataList.push_back(0x01);
                    break;
                }
            case 19200:
                {
                    mHandShake.sendDataList.push_back(0x02);
                    break;
                }
            case 38400:
                {
                    mHandShake.sendDataList.push_back(0x03);
                    break;
                }
            case 57600:
                {
                    mHandShake.sendDataList.push_back(0x04);
                    break;
                }
            case 115200:
                {
                    mHandShake.sendDataList.push_back(0x05);
                    break;
                }
            default:
                {
                    mHandShake.sendDataList.push_back(0x01);
                    break;
                }
            }

            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);

            mHandShake.nReturnLen = -1;
            nCmnTimes = 1;
            break;
        }
    case 2:
        {
            /*******************��ӵڶ������ְ�************************
              * send(60)       10 02 00 00  fc 00 00 00  11 11 00 ff  ff 03 00 00  00 00 24 00  1c 00 0a 08  00 00 00 00  00 00 00 00
                               01 13 01 00  00 00 01 00  68 c9 a3 79  9f 45 4c 4d  ad 45 e0 41  24 3e 5a 43  10 03 36 33
              * return(62)     10 02 00 00  fc 00 00 00  11 11 00 00  e4 03 00 ff  ff 03 26 00  9c 00 0c 08  00 00 00 00  00 00 00 00
                               00 00 01 13  01 00 00 00  01 00 4a 00  14 12 84 75  d1 22 bc 20  4d 5b 61 e5  e9 41 10 03  34 32
                               ���з��ʹ�����68 c9 a3 79  9f 45 4c 4d  ad 45 e0 41  24 3e 5a 43Ϊ�����㷨���ɴ��룬�ڸú�������д0�����յ�PLC�����������и�ֵ�޸�
                               ���ش�����4a 00  14 12 84 75  d1 22 bc 20  4d 5b 61 e5  e9 41ΪPLC�˶��㷨׼ȷ�ԣ����Բ������
              ******************************************************/
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xfc);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x11);
            mHandShake.sendDataList.push_back(0x11);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x24);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x1c);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0a);
            mHandShake.sendDataList.push_back(0x08);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x13);
            mHandShake.sendDataList.push_back(0x01);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);

            /*�����㷨�����������д0���ڵ�һ�ֽ��յ�PLC���������и�ֵ*/
            vector<uchar > nRamdonList;
            int nXorList[10] = {0};
            int nResultValue[4] = {0};
            int nSize = m_nTmpRcvList.size();
            uchar nTmpUCvalue = 0;
            for(int i = 0; i < nSize - 17; i++)
            {
                nTmpUCvalue = m_nTmpRcvList.at(15 + i);
                nRamdonList.push_back(nTmpUCvalue);
                if(nTmpUCvalue == 0x10)
                {
                    i++;
                }
            }

            if(nRamdonList.size() < 10)
            {
                nRamdonList.resize(10, 0);
            }

            /*�����*/
            if(nSize >= 14 && m_nTmpRcvList.at(14) == 0x01)
            {
                nXorList[0] = (nRamdonList[7] ^ 0x51) & 0xff;
                nXorList[1] = (nRamdonList[3] ^ 0x53) & 0xff;
                nXorList[2] = (nRamdonList[0] ^ 0x4d) & 0xff;
                nXorList[3] = (nRamdonList[6] ^ 0x2d) & 0xff;
                nXorList[4] = (nRamdonList[5] ^ 0x43) & 0xff;
                nXorList[5] = (nRamdonList[2] ^ 0x4c) & 0xff;
                nXorList[6] = (nRamdonList[4] ^ 0x45) & 0xff;
                nXorList[7] = (nRamdonList[1] ^ 0x45) & 0xff;
                nXorList[8] = nRamdonList[8] & 0xff;
                nXorList[9] = nRamdonList[9] & 0xff;

                nResultValue[0] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[3] << 8) + nXorList[2]);
                nResultValue[1] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[1] << 8) + nXorList[0]);
                nResultValue[2] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[5] << 8) + nXorList[4]);
                nResultValue[3] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[7] << 8) + nXorList[6]);
            }
            else
            {
                nXorList[0] = (nRamdonList[0] ^ 0x4d) & 0xff;
                nXorList[1] = (nRamdonList[1] ^ 0x45) & 0xff;
                nXorList[2] = (nRamdonList[2] ^ 0x4c) & 0xff;
                nXorList[3] = (nRamdonList[3] ^ 0x53) & 0xff;
                nXorList[4] = (nRamdonList[4] ^ 0x45) & 0xff;
                nXorList[5] = (nRamdonList[5] ^ 0x43) & 0xff;
                nXorList[6] = (nRamdonList[6] ^ 0x2d) & 0xff;
                nXorList[7] = (nRamdonList[7] ^ 0x51) & 0xff;
                nXorList[8] = nRamdonList[8] & 0xff;
                nXorList[9] = nRamdonList[9] & 0xff;

                for (int i=0; i<8; i++)
                {
                    if (nXorList[i] >= 0x41 && nXorList[i] <= 0x5a)
                    {
                        nXorList[i] = nXorList[i] + 0x20;
                    }
                    else if (nXorList[i] >= 0x61 && nXorList[i] <= 0x78)
                    {
                        nXorList[i] = nXorList[i] - 0x20;
                    }
                }
                nResultValue[0] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[3] << 8) + nXorList[2]);
                nResultValue[1] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[1] << 8) + nXorList[0]);
                nResultValue[2] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[5] << 8) + nXorList[4]);
                nResultValue[3] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[7] << 8) + nXorList[6]);
            }

            int nRepeatNum = 0;

            /*index = 40*/
            nTmpUCvalue = (nResultValue[0] & 0xff) ^ 0x4c;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 41*/
            nTmpUCvalue = ((nResultValue[0] >> 8) & 0xff) ^ 0x6f;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 42*/
            nTmpUCvalue = ((nResultValue[0] >> 16) & 0xff) ^ 0x67;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 43*/
            nTmpUCvalue = ((nResultValue[0] >> 24) & 0xff) ^ 0x69;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 44*/
            nTmpUCvalue = (nResultValue[1] & 0xff) ^ 0x6e;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 45*/
            nTmpUCvalue = ((nResultValue[1] >> 8) & 0xff) ^ 0x20;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 46*/
            nTmpUCvalue = ((nResultValue[1] >> 16) & 0xff) ^ 0x4d;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 47*/
            nTmpUCvalue = ((nResultValue[1] >> 24) & 0xff) ^ 0x45;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 48*/
            nTmpUCvalue = (nResultValue[2] & 0xff) ^ 0x4c;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 49*/
            nTmpUCvalue = ((nResultValue[2] >> 8) & 0xff) ^ 0x53;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 50*/
            nTmpUCvalue = ((nResultValue[2] >> 16) & 0xff) ^ 0x45;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 51*/
            nTmpUCvalue = ((nResultValue[2] >> 24) & 0xff) ^ 0x43;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 52*/
            nTmpUCvalue = (nResultValue[3] & 0xff) ^ 0x2d;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 53*/
            nTmpUCvalue = ((nResultValue[3] >> 8) & 0xff) ^ 0x51;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 54*/
            nTmpUCvalue = ((nResultValue[3] >> 16) & 0xff) ^ 0x20;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 55*/
            nTmpUCvalue = ((nResultValue[3] >> 24) & 0xff) ^ 0x3f;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }
            /*end--------------------------------*/

            /*����У����*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mHandShake.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);

            /*��У��*/
            mHandShake.sendDataList.push_back(nCrcResultH);
            mHandShake.sendDataList.push_back(nCrcResultL);

            mHandShake.nReturnLen = 62;
            nCmnTimes = 2;
            break;
        }
    case 3:
        {
            /*******************��ӵ��������ְ�************************
              * send(60)      10 02 00 00  fc 00 00 00  11 11 00 ff  ff 03 00 00  00 00 24 00  1c 08 0a 08  00 00 00 00  00 00 00 00
                              01 13 02 00  00 00 02 00  6c c3 a8 7c  9e 45 4c 4d  ad 45 e0 41  24 3e 5a 5d  10 03 38 43
              * return(46)    10 02 00 00  fc 00 00 00  11 11 00 00  e4 03 00 ff  ff 03 16 00  9c 08 0c 08  00 00 00 00  00 00 00 00
                              00 00 01 13  02 00 00 00  02 00 10 03  45 43
              ******************************************************/
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xfc);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x11);
            mHandShake.sendDataList.push_back(0x11);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x24);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x1c);
            mHandShake.sendDataList.push_back(0x08);      //�ڶ���00
            mHandShake.sendDataList.push_back(0x0a);
            mHandShake.sendDataList.push_back(0x08);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x13);
            mHandShake.sendDataList.push_back(0x02);      //�ڶ���01

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x02);      //�ڶ���01
            mHandShake.sendDataList.push_back(0x00);

            /*�����㷨�����������д0���ڵ�һ�ֽ��յ�PLC���������и�ֵ*/
            vector<uchar > nRamdonList;
            int nXorList[10] = {0};
            int nResultValue[4] = {0};
            int nSize = m_nTmpRcvList.size();
            uchar nTmpUCvalue = 0;
            for(int i = 0; i < nSize - 17; i++)
            {
                nTmpUCvalue = m_nTmpRcvList.at(15 + i);
                nRamdonList.push_back(nTmpUCvalue);
                if(nTmpUCvalue == 0x10)
                {
                    i++;
                }
            }

            if(nRamdonList.size() < 10)
            {
                nRamdonList.resize(10, 0);
            }

            /*�����*/
            if(nSize >= 14 && m_nTmpRcvList.at(14) == 0x01)
            {
                nXorList[0] = (nRamdonList[7] ^ 0x51) & 0xff;
                nXorList[1] = (nRamdonList[3] ^ 0x53) & 0xff;
                nXorList[2] = (nRamdonList[0] ^ 0x4d) & 0xff;
                nXorList[3] = (nRamdonList[6] ^ 0x2d) & 0xff;
                nXorList[4] = (nRamdonList[5] ^ 0x43) & 0xff;
                nXorList[5] = (nRamdonList[2] ^ 0x4c) & 0xff;
                nXorList[6] = (nRamdonList[4] ^ 0x45) & 0xff;
                nXorList[7] = (nRamdonList[1] ^ 0x45) & 0xff;
                nXorList[8] = nRamdonList[8] & 0xff;
                nXorList[9] = nRamdonList[9] & 0xff;

                nResultValue[0] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[3] << 8) + nXorList[2]);
                nResultValue[1] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[1] << 8) + nXorList[0]);
                nResultValue[2] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[5] << 8) + nXorList[4]);
                nResultValue[3] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[7] << 8) + nXorList[6]);
            }
            else
            {
                nXorList[0] = (nRamdonList[0] ^ 0x4d) & 0xff;
                nXorList[1] = (nRamdonList[1] ^ 0x45) & 0xff;
                nXorList[2] = (nRamdonList[2] ^ 0x4c) & 0xff;
                nXorList[3] = (nRamdonList[3] ^ 0x53) & 0xff;
                nXorList[4] = (nRamdonList[4] ^ 0x45) & 0xff;
                nXorList[5] = (nRamdonList[5] ^ 0x43) & 0xff;
                nXorList[6] = (nRamdonList[6] ^ 0x2d) & 0xff;
                nXorList[7] = (nRamdonList[7] ^ 0x51) & 0xff;
                nXorList[8] = nRamdonList[8] & 0xff;
                nXorList[9] = nRamdonList[9] & 0xff;

                for (int i=0; i<8; i++)
                {
                    if (nXorList[i] >= 0x41 && nXorList[i] <= 0x5a)
                    {
                        nXorList[i] = nXorList[i] + 0x20;
                    }
                    else if (nXorList[i] >= 0x61 && nXorList[i] <= 0x78)
                    {
                        nXorList[i] = nXorList[i] - 0x20;
                    }
                }
                nResultValue[0] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[3] << 8) + nXorList[2]);
                nResultValue[1] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[1] << 8) + nXorList[0]);
                nResultValue[2] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[5] << 8) + nXorList[4]);
                nResultValue[3] = ((nXorList[7] << 8) + nXorList[6]) * ((nXorList[7] << 8) + nXorList[6]);
            }

            int nRepeatNum = 0;

            /*index = 40*/
            nTmpUCvalue = (nResultValue[0] & 0xff) ^ 0x48;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 41*/
            nTmpUCvalue = ((nResultValue[0] >> 8) & 0xff) ^ 0x65;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 42*/
            nTmpUCvalue = ((nResultValue[0] >> 16) & 0xff) ^ 0x6c;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 43*/
            nTmpUCvalue = ((nResultValue[0] >> 24) & 0xff) ^ 0x6c;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 44*/
            nTmpUCvalue = (nResultValue[1] & 0xff) ^ 0x6f;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 45*/
            nTmpUCvalue = ((nResultValue[1] >> 8) & 0xff) ^ 0x20;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 46*/
            nTmpUCvalue = ((nResultValue[1] >> 16) & 0xff) ^ 0x4d;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 47*/
            nTmpUCvalue = ((nResultValue[1] >> 24) & 0xff) ^ 0x45;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 48*/
            nTmpUCvalue = (nResultValue[2] & 0xff) ^ 0x4c;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 49*/
            nTmpUCvalue = ((nResultValue[2] >> 8) & 0xff) ^ 0x53;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 50*/
            nTmpUCvalue = ((nResultValue[2] >> 16) & 0xff) ^ 0x45;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 51*/
            nTmpUCvalue = ((nResultValue[2] >> 24) & 0xff) ^ 0x43;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 52*/
            nTmpUCvalue = (nResultValue[3] & 0xff) ^ 0x2d;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 53*/
            nTmpUCvalue = ((nResultValue[3] >> 8) & 0xff) ^ 0x51;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 54*/
            nTmpUCvalue = ((nResultValue[3] >> 16) & 0xff) ^ 0x20;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }

            /*index = 55*/
            nTmpUCvalue = ((nResultValue[3] >> 24) & 0xff) ^ 0x21;
            mHandShake.sendDataList.push_back(nTmpUCvalue);
            if(nTmpUCvalue == 0x10)
            {
                nRepeatNum++;
                mHandShake.sendDataList.push_back(nTmpUCvalue);
            }
            /*end--------------------------------*/

            /*����У����*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mHandShake.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);

            /*��У��*/
            mHandShake.sendDataList.push_back(nCrcResultH);
            mHandShake.sendDataList.push_back(nCrcResultL);

            mHandShake.nReturnLen = 46;

            /*��ջ���*/
            m_nTmpRcvList.clear();
            nCmnTimes = 3;
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
 * Function: ������Э��İ������н��
 * Parameters: rcvList nCurrTimes: ������1��ʼ
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    int nRcvSize = rcvList.size();
    switch(nCurrTimes)
    {
    case 1:                 //
        {
            if(nRcvSize <= 0)
            {
                eCmnInfo = CHANGE_BAUD_RATE_9600;
                return eCmnInfo;
            }

            if(nRcvSize < 27)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize -2) == 0x10 && rcvList.at(nRcvSize -1) == 0x03)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                m_nTmpRcvList = rcvList;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 2:                 //
        {
            if(nRcvSize != 62)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(58) == 0x10 && rcvList.at(59) == 0x03)
            {
                uchar nCheckValueH = rcvList.at(60);
                uchar nCheckValueL = rcvList.at(61);
                rcvList.pop_back();
                rcvList.pop_back();
                rcvList.pop_back();
                rcvList.pop_back();

                ushort nCrcResult = PlcMathMethod::getSumCheck(rcvList, 2);
                uchar nCrcResultL = nCrcResult & 0x0f;
                uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;

                nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
                nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

                if(nCheckValueH == nCrcResultH && nCheckValueL == nCrcResultL)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 3:                 //
        {
            if(nRcvSize != 46)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(42) == 0x10 && rcvList.at(43) == 0x03)
            {
                uchar nCheckValueH = rcvList.at(44);
                uchar nCheckValueL = rcvList.at(45);
                rcvList.pop_back();
                rcvList.pop_back();
                rcvList.pop_back();
                rcvList.pop_back();

                ushort nCrcResult = PlcMathMethod::getSumCheck(rcvList, 2);
                uchar nCrcResultL = nCrcResult & 0x0f;
                uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;

                nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
                nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

                if(nCheckValueH == nCrcResultH && nCheckValueL == nCrcResultL)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    default:
        {
            break;
        }
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
    /*�Ե�ǰ����ĵ�ַ��ֵ*/
    mCurrAddr.mAddrProp = mCmnProp.mAddrInfo.mAddrProp;

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
            /*���д����Ϊ�գ��򷵻�*/
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
    /*��ʼ��*/
    ADDR_TARGET_PROP mAddrInfo = mCmnProp.mAddrInfo.mAddrProp;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*�ֲ������Ķ���*/
    int nWordValue = 0;
    int nRepeatNum = 0;
    ushort nRwLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nWordValue = (mAddrInfo.nAddrValue/8)*8;

            /*�㷵�ص��ֽڳ���*/
            if((mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)%8 == 0)
            {
                nRwLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8;
            }
            else
            {
                nRwLen = (mAddrInfo.nAddrValue%8 + mAddrInfo.nAddrLen)/8 +1;
            }

            /*���֧��һ���ֵĳ��ȶ�д��16λ��*/
            nRwLen = (nRwLen/2) + (nRwLen%2);
            nRwLen *= 16;
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            nWordValue = mAddrInfo.nAddrValue;

            /*��д�ֽڳ���*/
            nRwLen = mAddrInfo.nAddrLen;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    switch (eRegType)
    {
    case REG_TYPE_X:
        {
            /* ������*/
            nFunNum = 0x9c;
            break;
        }
    case REG_TYPE_Y:
        {
            /* ������*/
            nFunNum = 0x9d;
            break;
        }
    case REG_TYPE_M:
        {
            /* ������*/
            nFunNum = 0x90;
            break;
        }
    case REG_TYPE_L:
        {
            /* ������*/
            nFunNum = 0x92;
            break;
        }
    case REG_TYPE_F:
        {
            /* ������*/
            nFunNum = 0x93;
            break;
        }
    case REG_TYPE_V:
        {
            /* ������*/
            nFunNum = 0x94;
            break;
        }
    case REG_TYPE_S:
        {
            /* ������*/
            nFunNum = 0x98;
            break;
        }
    case REG_TYPE_B:
        {
            /* ������*/
            nFunNum = 0xa0;
            break;
        }
    case REG_TYPE_SB:
        {
            /* ������*/
            nFunNum = 0xa1;
            break;
        }
    case REG_TYPE_T:
        {
            /* ������, ��Ȧc0������c1*/
            nFunNum = 0xc1;
            break;
        }
    case REG_TYPE_C:
        {
            /* ������, ��Ȧc3������c4*/
            nFunNum = 0xc4;
            break;
        }
    case REG_TYPE_D:
        {
            /* ������*/
            nFunNum = 0xa8;
            break;
        }
    case REG_TYPE_W:
        {
            /* ������*/
            nFunNum = 0xb4;
            break;
        }
    case REG_TYPE_SW:
        {
            /* ������*/
            nFunNum = 0xb5;
            break;
        }
    case REG_TYPE_TV:
        {
            /* ������*/
            nFunNum = 0xc2;
            break;
        }
    case REG_TYPE_CV:
        {
            /* ������*/
            nFunNum = 0xc5;
            break;
        }
    case REG_TYPE_Z:
        {
            /* ������*/
            nFunNum = 0x94;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    /***************** ���ɵ�һ�η��͵İ� ***********************/
    /* ���ͣ�02
     * ���أ�10
     */
    mSendPackge.sendDataList.clear();

    /* index = 0*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 1*/
    mSendPackge.sendDataList.push_back(0x02);

    /* index = 2*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 3*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 4*/
    mSendPackge.sendDataList.push_back(0xfc);

    /* index = 5*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 6*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 7*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 8*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 9*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 10*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 11*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 12*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 13*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 14*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 15*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 16*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 17*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 18*/
    mSendPackge.sendDataList.push_back(0x1a);

    /* index = 19*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 20*/
    mSendPackge.sendDataList.push_back(0x1c);

    /* index = 21*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 22*/
    mSendPackge.sendDataList.push_back(0x0a);

    /* index = 23*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 24*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 25*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 26*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 27*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 28*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 29*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 30*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 31*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 32*/
    mSendPackge.sendDataList.push_back(0x04);

    /* index = 33*/
    mSendPackge.sendDataList.push_back(0x01);

    /* index = 34*/
    mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
    if(nCmnTimes == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
    }

    /* index = 35*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 36*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 37*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 38*/
    mSendPackge.sendDataList.push_back(nFunNum);

    /* index = 39*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 40*/
    nTmpValue = (nWordValue) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 41*/
    nTmpValue = (nWordValue >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 42*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 43*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 44*/
    nTmpValue = (nRwLen) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /* index = 45*/
    nTmpValue = (nRwLen >> 8) & 0xff;
    mSendPackge.sendDataList.push_back(nTmpValue);
    if(nTmpValue == 0x10)
    {
        nRepeatNum++;
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /*����У����*/
    ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
    uchar nCrcResultL = nCrcResult & 0x0f;
    uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
    nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
    nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

    /* index = 46*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 47*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 48*/
    mSendPackge.sendDataList.push_back(nCrcResultH);

    /* index = 49*/
    mSendPackge.sendDataList.push_back(nCrcResultL);

    mSendPackge.nReturnLen = -1;

    /*ͨ�Ŵ����ĵݼ�*/
    if(nCmnTimes >= 255 || nCmnTimes < 3)
    {
        nCmnTimes = 3;
    }
    else
    {
        nCmnTimes++;
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

    /*д����ֵ*/
    if(mAddrInfo.nValueList.empty()) return false;

    /***************** ���ɵ�һ�η��͵İ� ***********************/
    /* ���ͣ�02
     * ���أ�10
     */
    /*�ֲ������Ķ���*/
    int nRwLen = 1;
    int nWordValue = 0;
    int nRepeatNum = 0;
    ushort nOffsetLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;
    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();

    mSendPackge.sendDataList.clear();

    /* index = 0*/
    mSendPackge.sendDataList.push_back(0x10);

    /* index = 1*/
    mSendPackge.sendDataList.push_back(0x02);

    /* index = 2*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 3*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 4*/
    mSendPackge.sendDataList.push_back(0xfc);

    /* index = 5*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 6*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 7*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 8*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 9*/
    mSendPackge.sendDataList.push_back(0x11);

    /* index = 10*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 11*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 12*/
    mSendPackge.sendDataList.push_back(0xff);

    /* index = 13*/
    mSendPackge.sendDataList.push_back(0x03);

    /* index = 14*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 15*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 16*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 17*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 18*/
    mSendPackge.sendDataList.push_back(0x22);     //��1a

    /* index = 19*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 20*/
    mSendPackge.sendDataList.push_back(0x1c);

    /* index = 21*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 22*/
    mSendPackge.sendDataList.push_back(0x0a);

    /* index = 23*/
    mSendPackge.sendDataList.push_back(0x08);

    /* index = 24*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 25*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 26*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 27*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 28*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 29*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 30*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 31*/
    mSendPackge.sendDataList.push_back(0x00);

    /* index = 32*/
    mSendPackge.sendDataList.push_back(0x14);         //��04

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch (eRegType)
    {
    case REG_TYPE_Y:
        {
            /* ������*/
            nFunNum = 0x9d;
            break;
        }
    case REG_TYPE_M:
        {
            /* ������*/
            nFunNum = 0x90;
            break;
        }
    case REG_TYPE_L:
        {
            /* ������*/
            nFunNum = 0x92;
            break;
        }
    case REG_TYPE_F:
        {
            /* ������*/
            nFunNum = 0x93;
            break;
        }
    case REG_TYPE_V:
        {
            /* ������*/
            nFunNum = 0x94;
            break;
        }
    case REG_TYPE_S:
        {
            /* ������*/
            nFunNum = 0x98;
            break;
        }
    case REG_TYPE_B:
        {
            /* ������*/
            nFunNum = 0xa0;
            break;
        }
    case REG_TYPE_SB:
        {
            /* ������*/
            nFunNum = 0xa1;
            break;
        }
    case REG_TYPE_T:
        {
            /* ������, ��Ȧc0������c1*/
            nFunNum = 0xc1;
            break;
        }
    case REG_TYPE_C:
        {
            /* ������, ��Ȧc3������c4*/
            nFunNum = 0xc4;
            break;
        }
    case REG_TYPE_D:
        {
            /* ������*/
            nFunNum = 0xa8;
            break;
        }
    case REG_TYPE_W:
        {
            /* ������*/
            nFunNum = 0xb4;
            break;
        }
    case REG_TYPE_SW:
        {
            /* ������*/
            nFunNum = 0xb5;
            break;
        }
    case REG_TYPE_TV:
        {
            /* ������*/
            nFunNum = 0xc2;
            break;
        }
    case REG_TYPE_CV:
        {
            /* ������*/
            nFunNum = 0xc5;
            break;
        }
    case REG_TYPE_Z:
        {
            /* ������*/
            nFunNum = 0x94;
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
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            nWordValue = mAddrInfo.mAddrProp.nAddrValue;

            /*д�볤��, 15*/
            nRwLen = 1;

            /*ƫ�Ƴ���*/
            nOffsetLen = 0x22;

            /*�������*/
            nTmpWriteList.clear();
            if(!mAddrInfo.nValueList.empty())
            {
                if(mAddrInfo.nValueList.at(0) == 1)
                {
                    nTmpWriteList.push_back(1);
                }
                else
                {
                    nTmpWriteList.push_back(0);
                }
            }
            else
            {
                mSendPackge.sendDataList.clear();
                return false;
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
            {
                mSendPackge.sendDataList.clear();
                return false;
            }

            /* index = 18, �޸�ƫ�Ƴ���*/
            mSendPackge.sendDataList[18] = nOffsetLen;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(0x02);         //��01

            /* index = 34*/
            mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            if(nCmnTimes == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 35*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 38*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 39*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 40*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 41*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 42*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 43*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 44*/
            nTmpValue = (nWordValue) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 45*/
            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 46*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 47*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 48*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 49*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 50*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 51*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 52, ���õ�ֵ*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /* index = 53*/
            mSendPackge.sendDataList.push_back(0x00);

            /*����У����*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            /* index = 54*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 55*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 56*/
            mSendPackge.sendDataList.push_back(nCrcResultH);

            /* index = 57*/
            mSendPackge.sendDataList.push_back(nCrcResultL);
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            nWordValue = mAddrInfo.mAddrProp.nAddrValue;

            /*д�볤��, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen;

            /*ƫ����*/
            nOffsetLen = 0x1a + nRwLen*2;

            /*�������, ��λ�ȴ棬��λ���*/
            int nSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSize/2)
            {
                nRwLen = nSize/2;
            }
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen; i++ )
            {
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);

                /*���ֽں��*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nRepeatNum++;
                    nTmpWriteList.push_back(nLowByte);
                }

                /*���ֽ��ȴ�*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nRepeatNum++;
                    nTmpWriteList.push_back(nHighByte);
                }
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
            {
                mSendPackge.sendDataList.clear();
                return false;
            }

            /* index = 18, �޸�ƫ�Ƴ���*/
            mSendPackge.sendDataList[18] = nOffsetLen;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(0x01);         //��01

            /* index = 34*/
            mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            if(nCmnTimes == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 35*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 38*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 39*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 40*/
            nTmpValue = (nWordValue) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 41*/
            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 42*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 43*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 44*/
            mSendPackge.sendDataList.push_back(mAddrInfo.mAddrProp.nAddrLen);
            if(mAddrInfo.mAddrProp.nAddrLen == 0x10)
            {
                nRepeatNum++;
                mSendPackge.sendDataList.push_back(mAddrInfo.mAddrProp.nAddrLen);
            }

            /* index = 45*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 46, ���õ�ֵ*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /*����У����*/
            ushort nCrcResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList, 2) - nRepeatNum*0x10;
            uchar nCrcResultL = nCrcResult & 0x0f;
            uchar nCrcResultH = (nCrcResult >> 4) & 0x0f;
            nCrcResultL = PlcMathMethod::intToAsciiValue(nCrcResultL);
            nCrcResultH = PlcMathMethod::intToAsciiValue(nCrcResultH);

            /* index = 47*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 48*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 49*/
            mSendPackge.sendDataList.push_back(nCrcResultH);

            /* index = 50*/
            mSendPackge.sendDataList.push_back(nCrcResultL);
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    mSendPackge.nReturnLen = -1;

    /*ͨ�Ŵ����ĵݼ�*/
    if(nCmnTimes >= 255 || nCmnTimes < 3)
    {
        nCmnTimes = 3;
    }
    else
    {
        nCmnTimes++;
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
    targetList.clear();

    if(mSourceAddrList.empty()) return false;

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
    mTmpList.resize(17, mTmpList1);

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_X:
        case REG_TYPE_Y:
        case REG_TYPE_M:
        case REG_TYPE_L:
        case REG_TYPE_F:
        case REG_TYPE_V:
        case REG_TYPE_S:
        case REG_TYPE_B:
        case REG_TYPE_SB:
        case REG_TYPE_T:
        case REG_TYPE_C:
        case REG_TYPE_D:
        case REG_TYPE_W:
        case REG_TYPE_SW:
        case REG_TYPE_Z:
        case REG_TYPE_TV:
        case REG_TYPE_CV:
           {
                mTmpList[eRegIndex].push_back(mTmpAddr);
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
    for(int i = 0; i < 11; i++)
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

    /*REG_TYPE_D ~ REG_TYPE_CV*/
    for(int i = 11; i < 17; i++)
    {
        mTmpList[i] = PlcMathMethod::addrBubbleSort(mTmpList[i]);
        mTmpList[i] = PlcMathMethod::addrSortOut(mTmpList[i], nMaxRWlen/2, 1);

        int tmpSize = mTmpList[i].size();
        for(int k = 0; k < tmpSize; k++)
        {
            targetList.push_back(mTmpList[i].at(k));
        }
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
    mSerialInfo.BAUD_RATE = 115200;
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
    case REG_TYPE_X:
        {
            return false;
            break;
        }
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            return true;
            break;
        }
    default:
        return false;
    }
    return true;
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
            eRegType = REG_TYPE_V;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_S;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_B;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_SB;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_W;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_SW;
            break;
        }
    case 14:
        {
            eRegType = REG_TYPE_Z;
            break;
        }
    case 15:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 16:
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
    else if((DealWithString::compare("V", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("B", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("SB", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("W", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("SW", sRegName)))
    {
        nRegIndex = 13;
        return true;
    }
    else if((DealWithString::compare("Z", sRegName)))
    {
        nRegIndex = 14;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 15;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 16;
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


