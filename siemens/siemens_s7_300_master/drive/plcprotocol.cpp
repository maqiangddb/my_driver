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
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
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
                if(nIntValue < 0 || nIntValue >= ADDR_MAX_VALUE_4096 || nDecValue < 0 || nDecValue >= 8)
                {
                    return false;
                }

                /*���Ϻ�Ľ��*/
                mAddrTarget.nAddrValue = nIntValue*8 + nDecValue;

                /*�жϳ����Ƿ���*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096 * 8)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_DB_BIT:
        {
            /*��"."���*/
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                strList = DealWithString::split(sElsePass, ".");

                if(strList.size() == 2)
                {
                    string sWordStr = strList.at(0);
                    string sBitStr = strList.at(1);

                    /*�鲿�ֵ�ת��*/
                    int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*��λ���ֵ�ת��*/
                    int nWordValue = DealWithString::stringToInt(sWordStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*��λ���ֵ�ת��*/
                    int nBitValue = DealWithString::stringToInt(sBitStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return false;
                    }

                    /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
                    if(nBlockValue < 0 || nBlockValue >= 513 || nWordValue < 0 || nWordValue >= 8193 || nBitValue < 0 || nBitValue >= 8)
                    {
                        return false;
                    }

                    /*���Ϻ�Ľ��*/
                    mAddrTarget.nAddrValue = (nBlockValue * 8193 + nWordValue)*8 + nBitValue;

                    /*�жϳ����Ƿ���*/
                    if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 513*8193*8)
                    {
                        return false;
                    }
                    return true;
                }
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                /*�鲿�ֵ�ת��*/
                int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                /*��λ���ֵ�ת��*/
                int nWordValue = DealWithString::stringToInt(sElsePass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }

                mAddrTarget.nAddrValue = nBlockValue * 8193 + nWordValue;

                /*�жϳ����Ƿ���*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 65535 * 8193)
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
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096)
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
            sRegNameList.push_back("I");
            sRegNameList.push_back("Q");
            sRegNameList.push_back("M");
            sRegNameList.push_back("DB_BIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("DB_W");
            sRegNameList.push_back("DB_D");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("DB_D");
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
            sRegNameList.push_back("DB_BIT");
            sRegNameList.push_back("IW");
            sRegNameList.push_back("QW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("DB_W");
            sRegNameList.push_back("DB_D");
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
    case REG_TYPE_DB_BIT:
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
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
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
    case REG_TYPE_M:
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
                if(nIntValue < 0 || nIntValue >= ADDR_MAX_VALUE_4096 || nDecValue < 0 || nDecValue >= 8)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nDecValue + nInterval >= 8)
                    {
                        nIntValue += (nDecValue + nInterval)/8;
                        nDecValue = (nDecValue + nInterval)%8;
                    }
                    else
                    {
                        nDecValue += nInterval;
                    }

                    /*���Ϻ�Ľ��*/
                    int nStartValue = nIntValue*8 + nDecValue;

                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096*8)
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
    case REG_TYPE_DB_BIT:
        {
            /*��"."���*/
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                strList = DealWithString::split(sElsePass, ".");

                if(strList.size() == 2)
                {
                    string sWordStr = strList.at(0);
                    string sBitStr = strList.at(1);

                    /*�鲿�ֵ�ת��*/
                    int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*��λ���ֵ�ת��*/
                    int nWordValue = DealWithString::stringToInt(sWordStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*��λ���ֵ�ת��*/
                    int nBitValue = DealWithString::stringToInt(sBitStr,bSuccess);
                    if(bSuccess == false)
                    {
                        return ILLEGAL_VALUE;
                    }

                    /*�ж����ݺϷ���*/
                    if(nBlockValue < 0 || nBlockValue >= 513 || nWordValue < 0 || nWordValue >= 8193 || nBitValue < 0 || nBitValue >= 8)
                    {
                        return ADDR_OUT_BOUND;
                    }

                    char buf[10] = {0};
                    string sTmpStr;
                    for(int i = 0; i < nCopyNum; i++)
                    {
                        if(nBitValue + nInterval >= 8)
                        {
                            nWordValue += (nBitValue + nInterval)/8;
                            if(nWordValue >= 8193)
                            {
                                nBlockValue += nWordValue/8193;
                                nWordValue = nWordValue%8193;
                            }
                            nBitValue = (nBitValue + nInterval)%8;
                        }
                        else
                        {
                            nBitValue += nInterval;
                        }

                        /*���Ϻ�Ľ��*/
                        int nStartValue = (nBlockValue*8193 + nWordValue)*8 + nBitValue;

                        if(nStartValue + mTmpAddr.nAddrLen <= 513*8193*8)
                        {
                            /*��ӿ鲿��*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockValue);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*�����������*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordValue);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr + ".";

                            /*���С������*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBitValue);
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
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            nInterval = nInterval*2;
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string sBlockStr = strList.at(0);
                string sElsePass = strList.at(1);

                /*�鲿�ֵ�ת��*/
                int nBlockValue = DealWithString::stringToInt(sBlockStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*��λ���ֵ�ת��*/
                int nWordValue = DealWithString::stringToInt(sElsePass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nWordValue + nInterval >= 8193)
                    {
                        nBlockValue += (nWordValue + nInterval)/8193;
                        nWordValue = (nWordValue + nInterval)%8193;
                    }
                    else
                    {
                        nWordValue += nInterval;
                    }

                    /*���Ϻ�Ľ��*/
                    int nStartValue = nBlockValue*8193 + nWordValue;

                    if(nStartValue + mTmpAddr.nAddrLen <= 100*8193)
                    {
                        /*��ӿ鲿��*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nBlockValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr + ":";

                        /*�����������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nWordValue);
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
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nInterval = nInterval*2;
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
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
        for(int i = 24; i < nSize - 3; i++)
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
    if(nRcvSize < 1)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    mUnpackProp.bNeedSetData = false;
    switch(mUnpackProp.nCurrTimes)
    {
    case 1:                               //��һ��У��
    case 6:                               //������У��
    case 7:                               //���ߴ�У��
        {
            if(nRcvSize == 1)
            {
                if(rcvList.front() == 0x10)
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
            break;
        }
    case 2:                               //�ڶ���У��
        {
            if(nRcvSize == 2)
            {
                if(rcvList.at(0) == 0x10 && rcvList.at(1) == 0x02)
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
            break;
        }
    case 3:                               //������У��
        {
            if(nRcvSize >= 10)
            {
                if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                {
                    /*��дУ��һ��*/
                    uchar nCheckByte = rcvList.at(nRcvSize -1);
                    rcvList.pop_back();
                    ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                    if(nCheckResult == nCheckByte)
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
            }
            break;
        }
    case 4:                               //���Ĵ�У��
        {
            if(nRcvSize == 1)
            {
                if(rcvList.front() == 0x02)
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
            break;
        }
    case 5:                               //�����У��
        {
            if(CHECK_READ == eCheckType)            //��У��
            {
                /*���ȴ��ڵ���29���ҽ�����Ϊ10 03����У����ȷ*/
                if(nRcvSize >= 29)
                {
                    if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                    {
                        /*��дУ��һ��*/
                        uchar nCheckByte = rcvList.at(nRcvSize -1);
                        rcvList.pop_back();
                        ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                        if(nCheckResult == nCheckByte)
                        {
                            eCmnInfo = CMN_NOMAL_CODE;
                            mUnpackProp.bNeedSetData = true;
                            return eCmnInfo;
                        }
                        else
                        {
                            eCmnInfo = DATA_CHECK_ERROR;
                            return eCmnInfo;
                        }
                    }
                }
            }
            else if(CHECK_WRITE == eCheckType)            //дУ��
            {
                /*���ȴ��ڵ���24���ҽ�����Ϊ10 03����У����ȷ*/
                if(nRcvSize >= 24)
                {
                    if(rcvList.at(nRcvSize -3) == 0x10 && rcvList.at(nRcvSize -2) == 0x03)
                    {
                        /*��дУ��һ��*/
                        uchar nCheckByte = rcvList.at(nRcvSize -1);
                        rcvList.pop_back();
                        ushort nCheckResult = PlcMathMethod::getXORcheck(rcvList);
                        if(nCheckResult == nCheckByte)
                        {
                            eCmnInfo = CMN_NOMAL_CODE;
                            mUnpackProp.bNeedSetData = true;
                            return eCmnInfo;
                        }
                        else
                        {
                            eCmnInfo = DATA_CHECK_ERROR;
                            return eCmnInfo;
                        }
                    }
                }
            }
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
 * Function: �������Э��İ�
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    switch(nPkgIndex)
    {
    case 1:
        {
            /*******************��ӵ�һ�����ְ�************************
              * send(1)       02
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /*******************��ӵڶ������ְ�************************
              * send(26)       01 03 02 17  00 9f 01 3c  00 90 01 14  00 00 14 02  00 0f 05 01  00 03 00 10  03 3d
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x17);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x9f);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x3c);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x90);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x14);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x14);

            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0f);
            mHandShake.sendDataList.push_back(0x05);
            mHandShake.sendDataList.push_back(0x01);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);

            mHandShake.sendDataList.push_back(0x3d);
            mHandShake.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /*******************��ӵ��������ְ�************************
              * send(1)       10
              * return(12)    01 03 20 56  30 30 2e 36  30 10 03 4f
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 12;
            break;
        }
    case 4:
        {
            /*******************��ӵ��Ĵ����ְ�************************
              * send(2)       10 02
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /*******************��ӵ�������ְ�************************
              * send(23)       00 0d 00 03 e0   04 00 80 00 02   01 06 01 00 00    01 02 02 01 00   10 03 7d
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0d);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xe0);

            mHandShake.sendDataList.push_back(0x04);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x80);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x06);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x02);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x7d);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 6:
        {
            /*******************��ӵ��������ְ�************************
              * send(1)        10
              * return(23)     00 0c 03 03  d0 04 00 80  01 06 00 02  00 01 02 02  01 00 01 00  10 03 4f
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 23;
            break;
        }
    case 7:
        {
            /*******************��ӵ��ߴ����ְ�************************
              * send(2)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 8:
        {
            /*******************��ӵڰ˴����ְ�************************
              * send(2)        00 0c 03 03 05   01 10 03 1b
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x05);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x1b);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 9:
        {
            /*******************��ӵھŴ����ְ�************************
              * send(1)        10
              * return(9)      00 0c 03 03 05   01 10 03 1b
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 9;
            break;
        }
    case 10:
        {
            /*******************��ӵ�ʮ�����ְ�************************
              * send(2)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 11:
        {
            /*******************��ӵ�ʮһ�����ְ�************************
              * send(27)       00 0c 03 03 f1   00 32 01 00 00   ff ff 00 08 00   00 f0 00 00 03   00 03 01 00 10   03 24
              * return(2)      10 02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xf1);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x32);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0xff);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x08);
            mHandShake.sendDataList.push_back(0x00);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0xf0);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);

            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x24);

            mHandShake.nReturnLen = 2;
            break;
        }
    case 12:
        {
            /*******************��ӵ�ʮ�������ְ�************************
              * send(1)        10
              * return(10)     00 0c 03 03  b0 01 00 10  03 ae
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 10;
            break;
        }
    case 13:
        {
            /*******************��ӵ�ʮ�������ְ�************************
              * send(1)        10
              * return(1)      02
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 14:
        {
            /*******************��ӵ�ʮ�Ĵ����ְ�************************
              * send(1)        10
              * return(29)     00 0c 03 03  f1 00 32 03  00 00 ff ff  00 08 00 00  00 00 f0 00  00 02 00 02  00 f0 10 03  d7
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);

            mHandShake.nReturnLen = 29;
            break;
        }
    case 15:
        {
            /*******************��ӵ�ʮ������ְ�************************
              * send(1)        10 02
              * return(1)      10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x02);

            mHandShake.nReturnLen = 1;
            break;
        }
    case 16:
        {
            /*******************��ӵ�ʮ�������ְ�************************
              * send(10)      00 0c 03 03 b0   01 00 10 03 ae
              * return(1)     10
              ******************************************************/
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x0c);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xb0);

            mHandShake.sendDataList.push_back(0x01);
            mHandShake.sendDataList.push_back(0x00);
            mHandShake.sendDataList.push_back(0x10);
            mHandShake.sendDataList.push_back(0x03);
            mHandShake.sendDataList.push_back(0xae);

            mHandShake.nReturnLen = 1;
            nCmnTimes = 1;
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
    case 1:                 //10
    case 4:
    case 7:
    case 10:
    case 15:
    case 16:
        {
            if(nRcvSize != 1)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x10)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 2:                 //10 02
    case 5:
    case 8:
    case 11:
        {
            if(nRcvSize != 2)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x10 && rcvList.at(1) == 0x02)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 3:                 //01 03 20 56  30 30 2e 36  30 10 03 4f
        {
            if(nRcvSize != 12)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 6:                 // 00 0c 03 03  d0 04 00 80  01 06 00 02  00 01 02 02  01 00 01 00  10 03 4f
        {
            if(nRcvSize != 23)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 9:                 // 00 0c 03 03 05   01 10 03 1b
        {
            if(nRcvSize != 9)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 12:                 // 00 0c 03 03  b0 01 00 10  03 ae
        {
            if(nRcvSize != 10)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
                {
                    eCmnInfo = CMN_NOMAL_CODE;
                    return eCmnInfo;
                }
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 13:                 // 02
        {
            if(nRcvSize != 1)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(0) == 0x02)
            {
                eCmnInfo = CMN_NOMAL_CODE;
                return eCmnInfo;
            }

            eCmnInfo = DATA_CHECK_ERROR;
            break;
        }
    case 14:                 // 00 0c 03 03  f1 00 32 03  00 00 ff ff  00 08 00 00  00 00 f0 00  00 02 00 02  00 f0 10 03  d7
        {
            if(nRcvSize != 29)
            {
                eCmnInfo = DATA_LEN_ERROR;
                return eCmnInfo;
            }

            if(rcvList.at(nRcvSize - 3) == 0x10 && rcvList.at(nRcvSize - 2) == 0x03)
            {
                uchar nLastData = rcvList.at(nRcvSize - 1);
                rcvList.pop_back();

                ushort nCheck = PlcMathMethod::getXORcheck(rcvList);
                nCheck &= 0xff;
                if(nCheck == nLastData)
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

    mSendPackge.nTotalPkgNum = 7;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*�鲿�ֵ�ת��*/
    int nBlockValue = 0;
    int nWordValue = 0;
    uchar nRwLen = 0;
    uchar nFunNum = 0;
    uchar nTmpValue = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_Q:
    case REG_TYPE_M:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.nAddrValue/8;
            nWordValue = nWordValue*8;

            /*��д�ֽڳ���*/
            nRwLen = mAddrInfo.nAddrLen/8;
            if(mAddrInfo.nAddrLen%8 != 0)
            {
                nRwLen++;
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.nAddrValue*8;

            /*��д�ֽڳ���*/
            nRwLen = mAddrInfo.nAddrLen*2;
            break;
        }
    case REG_TYPE_DB_BIT:
        {
            nBlockValue = mAddrInfo.nAddrValue/(8193*8);
            nWordValue = (mAddrInfo.nAddrValue/8)%8193;
            nWordValue = nWordValue*8;

            /*��д�ֽڳ���*/
            nRwLen = mAddrInfo.nAddrLen/8;
            if(mAddrInfo.nAddrLen%8 != 0)
            {
                nRwLen++;
            }
            break;
        }
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            nBlockValue = mAddrInfo.nAddrValue/8193;
            nWordValue = mAddrInfo.nAddrValue%8193;
            nWordValue = nWordValue*8;

            /*��д�ֽڳ���*/
            nRwLen = mAddrInfo.nAddrLen*2;
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
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            /* ������*/
            nFunNum = 0x81;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_QW:
        {
            /* ������*/
            nFunNum = 0x82;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            /* ������*/
            nFunNum = 0x83;
            break;
        }
    case REG_TYPE_DB_BIT:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            /* ������*/
            nFunNum = 0x84;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    ushort nCrcResult;
    uchar nCrcResultL;
    switch(mSendPackge.nPkgIndex)
    {
    case 1:
        {
            /***************** ���ɵ�һ�η��͵İ� ***********************/
            /* ���ͣ�02
             * ���أ�10
             */
            mSendPackge.sendDataList.clear();
            mSendPackge.sendDataList.push_back(0x02);
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /***************** ���ɵڶ��η��͵İ� ***********************/
            /* ���ͣ�00 0c 03 03  f1 ���� 32 01  00 00 33 01  00 0e 00 00  04 01 12 0a  10 10 02 00  02 00 00 81  00 00 08 10 03 77

             * ���أ�10 02
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xf1);

            if(nCmnTimes == 0x10)
            {
                /* index = 5*/
                mSendPackge.sendDataList.push_back(0x10);

                /* index = 6*/
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                /* index = 5*/
                mSendPackge.sendDataList.push_back(nCmnTimes & 0xff);
            }

            /* index = 6*/
            mSendPackge.sendDataList.push_back(0x32);

            /* index = 7 */
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 9*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 10, д������ͬ�ĵط��� ����0x33,д��0x44 */
            mSendPackge.sendDataList.push_back(0x33);

            /* index = 11 */
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 12 */
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 13 */
            mSendPackge.sendDataList.push_back(0x0e);

            /* index = 14*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 15, д������ͬ�ĵط��� ����0x00,д��0x05*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 16, д������ͬ�ĵط��� ����0x04,д��0x05*/
            mSendPackge.sendDataList.push_back(0x04);

            /* index = 17*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 18*/
            mSendPackge.sendDataList.push_back(0x12);

            /* index = 19*/
            mSendPackge.sendDataList.push_back(0x0a);

            /* index = 20*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 21*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 22*/
            mSendPackge.sendDataList.push_back(0x02);

            /* index = 23*/
            mSendPackge.sendDataList.push_back(0x00);

            if(nRwLen == 0x10)
            {
                /* index = 24*/
                mSendPackge.sendDataList.push_back(nRwLen);

                /* index = 25*/
                mSendPackge.sendDataList.push_back(nRwLen);
            }
            else
            {
                /* index = 24*/
                mSendPackge.sendDataList.push_back(nRwLen);
            }

            /*����ֵ, ��ȡ����*/
            uchar nLowByte = (nBlockValue) & 0xff;
            uchar nHighByte = (nBlockValue >> 8) & 0xff;

            /*���ֽ��ȴ�*/
            if(nHighByte == 0x10)
            {
                /* index = 25*/
                mSendPackge.sendDataList.push_back(nHighByte);

                /* index = 26*/
                mSendPackge.sendDataList.push_back(nHighByte);
            }
            else
            {
                /* index = 25*/
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /*���ֽں��*/
            if(nLowByte == 0x10)
            {
                /* index = 6*/
                mSendPackge.sendDataList.push_back(nLowByte);

                /* index = 27*/
                mSendPackge.sendDataList.push_back(nLowByte);
            }
            else
            {
                /* index = 26*/
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 27*/
            mSendPackge.sendDataList.push_back(nFunNum);

            /* index = 28, 29, 30, ��ȡ�ĵ�ֵַ, ���ֽ���ǰ*/
            nTmpValue = (nWordValue >> 16) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = (nWordValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nTmpValue);
            if(nTmpValue == 0x10)
            {
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /* index = 31*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 32*/
            mSendPackge.sendDataList.push_back(0x03);

            /*����У����*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 33*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /***************** ���ɵ����η��͵İ� ***********************/
            /* ���ͣ�10
             * ���أ�00 0c 03 03  b0 01 00 10  03 ae
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 11;
            }
            else
            {
                mSendPackge.nReturnLen = 10;
            }
            break;
        }
    case 4:
        {
            /***************** ���ɵ��Ĵη��͵İ� ***********************/
            /* ���ͣ�10
             * ���أ�02
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /***************** ���ɵ���η��͵İ� ***********************/
            /* ���ͣ�10
             * ���أ�00 0c 03 03  f1 01 32 01  00 00 33 01  00 0e 00 00  04 01 12 0a  10 10 02 00  02 00 00 81  00 00 08 10 03 74
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = -1;
            break;
        }
    case 6:
        {
            /***************** ���ɵ����η��͵İ� ***********************/
            /* ���ͣ�10 02
             * ���أ�10
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x02);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 7:
        {
            /***************** ���ɵ��ߴη��͵İ� ***********************/
            /* ���ͣ�00 0c 03 03  b0 01 01 10 03 af
             * ���أ�10
             */
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xb0);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 6*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x03);

            /*����У����*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 9*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;

            /*ͨ�Ŵ����ĵݼ�*/
            if(nCmnTimes >= 255 || nCmnTimes < 1)
            {
                nCmnTimes = 1;
            }
            else
            {
                nCmnTimes++;
            }
            break;
        }
    default:
        {
            return false;
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
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 7;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    /*д����ֵ*/
    if(mAddrInfo.nValueList.empty()) return false;

    /*�鲿�ֵ�ת��*/
    int nBlockValue = 0;
    int nWordValue = 0;
    uchar nBitValue = 0;
    ushort nWriteType = 0;
    ushort nOffsetLen = 0;
    ushort nFunNum = 0;
    ushort nRwUnit = 0;
    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();

    int nRwLen = 1;
    uchar nAddrRwLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
//    case REG_TYPE_I:     ����д
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_DB_BIT:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.mAddrProp.nAddrValue/8;
            nBitValue = mAddrInfo.mAddrProp.nAddrValue%8;

            /*�����VB��ƫ�Ƶ�ַ ���� ����8 */
            if(REG_TYPE_DB_BIT == eRegType)
            {
                nBlockValue = (mAddrInfo.mAddrProp.nAddrValue/8)/8193;
                nWordValue = (mAddrInfo.mAddrProp.nAddrValue/8)%8193;
                nBitValue = mAddrInfo.mAddrProp.nAddrValue%8;
            }

            nWordValue = nWordValue*8 + nBitValue;

            /*д�볤��, 15*/
            nRwLen = 1;
            nAddrRwLen = 0x04 + nRwLen;

            /*д�����λ,31��32*/
            nWriteType = 0x0003;

            /*ƫ����,33��34*/
            nOffsetLen = 0x0001;

            /*��д��λ, 01Ϊ1 bit�� 02Ϊ1 byte�� 04Ϊ1 word�� 06Ϊ1 double word*/
            nRwUnit = 0x01;

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
                return false;
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
//    case REG_TYPE_IW:   ����д
    case REG_TYPE_QW:
    case REG_TYPE_MW:
        {
            nBlockValue = 0;
            nWordValue = mAddrInfo.mAddrProp.nAddrValue*8;

            /*д�볤��, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;

            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*д�����λ,31��32*/
            nWriteType = 0x3004;

            /*ƫ����,33��34*/
            nOffsetLen = nRwLen *8;

            /*��д��λ, 01Ϊ1 bit�� 02Ϊ1 byte�� 04Ϊ1 word�� 06Ϊ1 double word*/
            nRwUnit = 0x02;

            /*�������*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/2; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                /*���ֽ��ȴ�*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*���ֽں��*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DB_W:
        {
            nBlockValue = (mAddrInfo.mAddrProp.nAddrValue)/8193;
            nWordValue = (mAddrInfo.mAddrProp.nAddrValue)%8193;
            nWordValue = nWordValue*8;

            /*д�볤��, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;
            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*д�����λ,31��32*/
            nWriteType = 0x3004;

            /*ƫ����,33��34*/
            nOffsetLen = nRwLen *8;

            /*��д��λ, 01Ϊ1 bit�� 02Ϊ1 byte�� 04Ϊ1 word�� 06Ϊ1 double word*/
            nRwUnit = 0x02;

            /*�������*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/2; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                /*���ֽ��ȴ�*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*���ֽں��*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DB_D:
        {
            nBlockValue = (mAddrInfo.mAddrProp.nAddrValue)/8193;
            nWordValue = (mAddrInfo.mAddrProp.nAddrValue)%8193;
            nWordValue = nWordValue*8;

            /*д�볤��, 15*/
            nRwLen = mAddrInfo.mAddrProp.nAddrLen *2;
            int nSetListSize = mAddrInfo.nValueList.size();
            if(nRwLen > nSetListSize)
            {
                nRwLen = (nSetListSize/2) *2;
            }

            nAddrRwLen = 0x04 + nRwLen;

            /*д�����λ,31��32*/
            nWriteType = 0x3004;

            /*ƫ����,33��34*/
            nOffsetLen = nRwLen *8;

            /*��д��λ, 01Ϊ1 bit�� 02Ϊ1 byte�� 04Ϊ1 word�� 06Ϊ1 double word*/
            nRwUnit = 0x02;

            /*�������*/
            nTmpWriteList.clear();
            for(int i = 0; i < nRwLen/4; i++ )
            {
                uchar nHighByte = mAddrInfo.nValueList.at(i*4 + 3);
                uchar nLowByte = mAddrInfo.nValueList.at(i*4 + 2);

                /*���ֽ��ȴ�*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*���ֽں��*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }

                nHighByte = mAddrInfo.nValueList.at(i*4 + 1);
                nLowByte = mAddrInfo.nValueList.at(i*4);

                /*���ֽ��ȴ�*/
                nTmpWriteList.push_back(nHighByte);
                if(nHighByte == 0x10)
                {
                    nTmpWriteList.push_back(nHighByte);
                }

                /*���ֽں��*/
                nTmpWriteList.push_back(nLowByte);
                if(nLowByte == 0x10)
                {
                    nTmpWriteList.push_back(nLowByte);
                }
            }

            if(nRwLen%4 == 2)
            {
                for(int i = (nRwLen/4)*2; i < nRwLen/2; i++ )
                {
                    uchar nHighByte = mAddrInfo.nValueList.at(i*2 + 1);
                    uchar nLowByte = mAddrInfo.nValueList.at(i*2);

                    /*���ֽ��ȴ�*/
                    nTmpWriteList.push_back(nHighByte);
                    if(nHighByte == 0x10)
                    {
                        nTmpWriteList.push_back(nHighByte);
                    }

                    /*���ֽں��*/
                    nTmpWriteList.push_back(nLowByte);
                    if(nLowByte == 0x10)
                    {
                        nTmpWriteList.push_back(nLowByte);
                    }
                }
            }

            /*���Ϊ�գ��򷵻ؿ�*/
            if(nTmpWriteList.empty())
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
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            /* ������*/
            nFunNum = 0x0081;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_QW:
        {
            /* ������*/
            nFunNum = 0x0082;
            break;
        }
    case REG_TYPE_M:
    case REG_TYPE_MW:
        {
            /* ������*/
            nFunNum = 0x0083;
            break;
        }
    case REG_TYPE_DB_BIT:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
        {
            /* ������*/
            nFunNum = 0x0084;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    ushort nCrcResult;
    uchar nCrcResultL;
    switch(mSendPackge.nPkgIndex)
    {
    case 1:
        {
            /***************** ���ɵ�һ�η��͵İ� ***********************/
            mSendPackge.sendDataList.clear();
            mSendPackge.sendDataList.push_back(0x02);
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 2:
        {
            /***************** ���ɵڶ��η��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xf1);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 6*/
            mSendPackge.sendDataList.push_back(0x32);

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 9*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 10, д������ͬ�ĵط��� ����0x33,д��0x44*/
            mSendPackge.sendDataList.push_back(0x44);

            /* index = 11*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 12*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 13*/
            mSendPackge.sendDataList.push_back(0x0e);

            /* index = 14*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 15, д�ĳ���*/
            mSendPackge.sendDataList.push_back(nAddrRwLen);
            if(nAddrRwLen == 0x10)
            {
                mSendPackge.sendDataList.push_back(nAddrRwLen);
            }

            /* index = 16, д������ͬ�ĵط��� ����0x04,д��0x05*/
            mSendPackge.sendDataList.push_back(0x05);

            /* index = 17*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 18*/
            mSendPackge.sendDataList.push_back(0x12);

            /* index = 19*/
            mSendPackge.sendDataList.push_back(0x0a);

            /* index = 20*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 21*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 22*/
            mSendPackge.sendDataList.push_back(nRwUnit);

            /*����ֵ, д�볤��*/
            uchar nLowByte = (nRwLen) & 0xff;
            uchar nHighByte = (nRwLen >> 8) & 0xff;

            /* index = 23, ���ֽ��ȴ�*/
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 24, ���ֽں��*/
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /*����ֵ, ���λ��*/
            nLowByte = (nBlockValue) & 0xff;
            nHighByte = (nBlockValue >> 8) & 0xff;

            /* index = 25, ���ֽ��ȴ�*/
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 26, ���ֽں��*/
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 27*/
            mSendPackge.sendDataList.push_back(nFunNum & 0xff);

            /* index = 28, 29, 30, ��ȡ�ĵ�ֵַ, ���ֽ���ǰ*/
            nLowByte = (nWordValue >> 16) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            nLowByte = (nWordValue >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            nLowByte = (nWordValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 31*/
            nHighByte = (nWriteType >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nHighByte);

            /* index = 32*/
            nLowByte = (nWriteType >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);

            /* index = 33*/
            nHighByte = (nOffsetLen >> 8) & 0xff;
            mSendPackge.sendDataList.push_back(nHighByte);
            if(nHighByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nHighByte);
            }

            /* index = 34*/
            nLowByte = (nOffsetLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(nLowByte);
            if(nLowByte == 0x10)
            {
                mSendPackge.sendDataList.push_back(nLowByte);
            }

            /* index = 35, ���õ�ֵ*/
            int nWriteLen = nTmpWriteList.size();
            for(int i = 0; i < nWriteLen; i++)
            {
                mSendPackge.sendDataList.push_back(nTmpWriteList.at(i));
            }

            /* index = 36*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 37*/
            mSendPackge.sendDataList.push_back(0x03);

            /*����У����*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 38*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 2;
            break;
        }
    case 3:
        {
            /***************** ���ɵ����η��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 11;
            }
            else
            {
                mSendPackge.nReturnLen = 10;
            }
            break;
        }
    case 4:
        {
            /***************** ���ɵ��Ĵη��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 5:
        {
            /***************** ���ɵ���η��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /*�������ݵĳ���*/
            if(nCmnTimes == 0x10)
            {
                mSendPackge.nReturnLen = 25;
            }
            else
            {
                mSendPackge.nReturnLen = 24;
            }
            break;
        }
    case 6:
        {
            /***************** ���ɵ����η��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x02);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;
            break;
        }
    case 7:
        {
            /***************** ���ɵ��ߴη��͵İ� ***********************/
            mSendPackge.sendDataList.clear();

            /* index = 0*/
            mSendPackge.sendDataList.push_back(0x00);

            /* index = 1*/
            mSendPackge.sendDataList.push_back(0x0c);

            /* index = 2*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 3*/
            mSendPackge.sendDataList.push_back(0x03);

            /* index = 4*/
            mSendPackge.sendDataList.push_back(0xb0);

            /* index = 5*/
            mSendPackge.sendDataList.push_back(0x01);

            /* index = 6*/
            mSendPackge.sendDataList.push_back(nCmnTimes);
            if(nCmnTimes == 0x10)
            {
                mSendPackge.sendDataList.push_back(nCmnTimes);
            }

            /* index = 7*/
            mSendPackge.sendDataList.push_back(0x10);

            /* index = 8*/
            mSendPackge.sendDataList.push_back(0x03);

            /*����У����*/
            nCrcResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);
            nCrcResultL = nCrcResult & 0xff;

            /* index = 9*/
            mSendPackge.sendDataList.push_back(nCrcResultL);

            /*�������ݵĳ���*/
            mSendPackge.nReturnLen = 1;

            /*ͨ�Ŵ����ĵݼ�*/
            if(nCmnTimes >= 255 || nCmnTimes < 1)
            {
                nCmnTimes = 1;
            }
            else
            {
                nCmnTimes++;
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
    vector<ADDR_TARGET_PROP > mTmpList0;
    vector<ADDR_TARGET_PROP > mTmpList1;
    vector<ADDR_TARGET_PROP > mTmpList2;
    vector<ADDR_TARGET_PROP > mTmpList3;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);


        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_I:
            {
                /*ת���ɶ�Ӧ���ֵ�ַ���ͼĴ���*/
                mTmpAddr.nRegIndex = REG_TYPE_IW;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_IW:
            {
                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Q:
            {
                /*ת���ɶ�Ӧ���ֵ�ַ���ͼĴ���*/
                mTmpAddr.nRegIndex = REG_TYPE_QW;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_QW:
            {
                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_M:
            {
                /*ת���ɶ�Ӧ���ֵ�ַ���ͼĴ���*/
                mTmpAddr.nRegIndex = REG_TYPE_MW;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MW:
            {
                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DB_BIT:
            {
                /*ת���ɶ�Ӧ���ֵ�ַ���ͼĴ���*/
                mTmpAddr.nRegIndex = REG_TYPE_DB_W;

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/8;

                if((mTmpAddr.nAddrLen + tmpStartValue%8)%8 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%8)/8 +1;
                }

                mTmpList3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DB_W:
        case REG_TYPE_DB_D:
            {
                mTmpList3.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*���nMaxRWlen���û����õ�����д���ȣ����ܻ�����Э���е����֧�ֳ��ȱȽ�*/
    mTmpList0 = PlcMathMethod::addrBubbleSort(mTmpList0);
    mTmpList0 = PlcMathMethod::addrSortOut(mTmpList0, nMaxRWlen, 1, true);
    int tmpSize = mTmpList0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList0.at(i));
    }

    mTmpList1 = PlcMathMethod::addrBubbleSort(mTmpList1);
    mTmpList1 = PlcMathMethod::addrSortOut(mTmpList1, nMaxRWlen, 1, true);
    tmpSize = mTmpList1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList1.at(i));
    }

    mTmpList2 = PlcMathMethod::addrBubbleSort(mTmpList2);
    mTmpList2 = PlcMathMethod::addrSortOut(mTmpList2, nMaxRWlen, 1, true);
    tmpSize = mTmpList2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList2.at(i));
    }

    mTmpList3 = PlcMathMethod::addrBubbleSort(mTmpList3);
    mTmpList3 = PlcMathMethod::addrSortOut(mTmpList3, nMaxRWlen, 1, true);
    tmpSize = mTmpList3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList3.at(i));
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
    int nStation = 2;
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
    case REG_TYPE_I:
    case REG_TYPE_IW:
        {
            return false;
            break;
        }
    case REG_TYPE_Q:
    case REG_TYPE_M:
    case REG_TYPE_DB_BIT:
    case REG_TYPE_QW:
    case REG_TYPE_MW:
    case REG_TYPE_DB_W:
    case REG_TYPE_DB_D:
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
            eRegType = REG_TYPE_DB_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_IW;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_QW;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_MW;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_DB_W;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_DB_D;
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
    else if((DealWithString::compare("DB_BIT", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("IW", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("QW", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("MW", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("DB_W", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("DB_D", sRegName)))
    {
        nRegIndex = 8;
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
