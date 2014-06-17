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
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,8);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue > ADDR_MAX_VALUE_32)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
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
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess,16);
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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_128000)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_M:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_8000)
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
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if (mAddrTarget.nAddrValue < ADDR_MAX_VALUE_8000 || mAddrTarget.nAddrValue > 8126)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 8126)
            {
                return false;
            }

            /*��8000��ʼ��ƫ�Ƶ���0��ʼ*/
            mAddrTarget.nAddrValue -= 8000;
            return true;
            break;
        }
    case REG_TYPE_SD:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if (mAddrTarget.nAddrValue < ADDR_MAX_VALUE_8000 || mAddrTarget.nAddrValue > 9000)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 9000)
            {
                return false;
            }

            /*��8000��ʼ��ƫ�Ƶ���0��ʼ*/
            mAddrTarget.nAddrValue -= 8000;
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_800)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_R:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_32768)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_TV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
            {
                return false;
            }

            /*�жϳ����Ƿ���*/
            if (mAddrTarget.nAddrValue < ADDR_MAX_VALUE_200 || mAddrTarget.nAddrValue > 280)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen/2 > 280)
            {
                return false;
            }

            /*��200��ʼ��ƫ�Ƶ���0��ʼ*/
            mAddrTarget.nAddrValue -= 200;
            return true;
            break;
        }
    default:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            if(bSuccess == false)
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
            sRegNameList.push_back("SM");
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D_BIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
            sRegNameList.push_back("R");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("32CV");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("32CV");
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
            sRegNameList.push_back("S");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("D_BIT");
            sRegNameList.push_back("D");
            sRegNameList.push_back("SD");
            sRegNameList.push_back("R");
            sRegNameList.push_back("TV");
            sRegNameList.push_back("CV");
            sRegNameList.push_back("32CV");
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
    case REG_TYPE_D_BIT:
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_S:
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
    case REG_TYPE_R:
    case REG_TYPE_CV:
    case REG_TYPE_32CV:
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
    case REG_TYPE_D_BIT:
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
                int nDecValue = DealWithString::stringToInt(sDecStr,bSuccess,16);
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

                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_128000)
                    {
                        /*�����������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr + ".";

                        /*���С������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%x", nDecValue);
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
    case REG_TYPE_X:
    case REG_TYPE_Y:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess, 8);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue > ADDR_MAX_VALUE_32)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_32 + 1)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%o", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
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

            /*����Ϊ����*/
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
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
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

            /*����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_8000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_8000)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_SD:
    {
        int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
        if(false == bSuccess)
        {
            return ILLEGAL_VALUE;
        }

        /*����Ϊ����*/
        if(nStartValue < 0 || nStartValue >= 9000)
        {
            return ILLEGAL_VALUE;
        }

        char buf[10] = {0};
        string sTmpStr;
        for(int i = 0; i < nCopyNum; i++)
        {
            nStartValue += nInterval;

            if(nStartValue + mTmpAddr.nAddrLen <= 9000)
            {
                /*�����������*/
                memset(buf, 0, 10);
                sprintf(buf, "%d", nStartValue);
                sTmpStr = buf;
                mTmpAddr.sAddrValue = sTmpStr;

                mSourceAddrList.push_back(mTmpAddr);
            }
        }
        break;
    }
    case REG_TYPE_SM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= 8126)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 8126)
                {
                    /*�����������*/
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
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    {
        int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
        if(false == bSuccess)
        {
            return ILLEGAL_VALUE;
        }

        /*����Ϊ����*/
        if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_800)
        {
            return ILLEGAL_VALUE;
        }

        char buf[10] = {0};
        string sTmpStr;
        for(int i = 0; i < nCopyNum; i++)
        {
            nStartValue += nInterval;

            if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_800)
            {
                /*�����������*/
                memset(buf, 0, 10);
                sprintf(buf, "%d", nStartValue);
                sTmpStr = buf;
                mTmpAddr.sAddrValue = sTmpStr;

                mSourceAddrList.push_back(mTmpAddr);
            }
        }
        break;
    }
    case REG_TYPE_TV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
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
            }
            break;
        }
    case REG_TYPE_R:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*����Ϊ����*/
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
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

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

            /*����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_200)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_200)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            break;
        }
    case REG_TYPE_32CV:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= 280)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen/2 <= 280)
                {
                    /*�����������*/
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

    int nRcvSize = rcvList.size();
    if(nRcvSize < 6)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvLowByte = rcvList.at(nRcvSize - 1);
    uchar nRcvHighByte = rcvList.at(nRcvSize - 2);

    vector<unsigned char> tmprcvList;
    for (int i=1; i<(nRcvSize - 2); i++)
    {
        tmprcvList.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getSumCheck(tmprcvList) & 0xff;

    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

    if(lowByte == nRcvLowByte && highByte == nRcvHighByte)
    {
        /*ȡ��Ч����*/
        targetList.clear();
        for(int i = 5; i < nRcvSize - 3; i++)
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
    unsigned short SUM;
    unsigned char SUM_low_cal;
    unsigned char SUM_high_cal;
    unsigned char SUM_low_return;
    unsigned char SUM_high_return;

    if (nRcvSize == 5)//д����ֻ����5������
    {
        if (rcvList.at(0) == 0x06 && rcvList.at(3) == 0x46 && rcvList.at(4) == 0x46)//����06����ȷ��
        {
            eCmnInfo = CMN_NOMAL_CODE;
        }
        else//����15�Ǵ����
        {
            eCmnInfo = OTHER_ERROR_CODE;
        }
    }
    else if (nRcvSize >= 6)//�����ܷ��ش���
    {
        if (rcvList.at(0) == 0x02  &&  rcvList.at(nRcvSize - 3) == 0x03)//��ʼ���ͽ���λ����ȷ
        {
            SUM_low_return = rcvList.at(nRcvSize-1);//ȡ���ص����һ������
            SUM_high_return = rcvList.at(nRcvSize-2);//ȡ���صĵ����ڶ�������

            rcvList.pop_back();
            rcvList.pop_back();
            SUM = PlcMathMethod::getSumCheck(rcvList,1) & 0xff;
            SUM_low_cal = SUM&0xf;
            SUM_high_cal = (SUM>>4)&0xf;
            SUM_low_cal = PlcMathMethod::intToAsciiValue(SUM_low_cal);//ת����ASC����ʽ
            SUM_high_cal = PlcMathMethod::intToAsciiValue(SUM_high_cal);

            if (SUM_high_return == SUM_high_cal && SUM_low_cal == SUM_low_return)//SUMУ�����㣬������ȷ����1������ȷ����0
            {
                mUnpackProp.bNeedSetData = true;
                eCmnInfo = CMN_NOMAL_CODE;
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
    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    //    if(nValueList.empty()) return mSendPackge;

    /*�Ե�ǰ����ĵ�ַ��ֵ*/
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
            mSendPackge.nTotalPkgNum = 1;
            if(REG_TYPE_D_BIT == eRegType)
            {
                /*��ʼ���̶�*/
                uchar nTmpValue = 0x05;
                mSendPackge.sendDataList.push_back(nTmpValue);

                //վ��
                nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf;
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

                nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf;
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

                mSendPackge.sendDataList.push_back('F');
                mSendPackge.sendDataList.push_back('F');
                mSendPackge.sendDataList.push_back('W');
                mSendPackge.sendDataList.push_back('R');
                mSendPackge.sendDataList.push_back('0');

                int nSendAddr = mAddrInfo.mAddrProp.nAddrValue/16;

                int nTmp1 = nSendAddr/ 1000;
                int nTmp2 = (nSendAddr % 1000) / 100;
                int nTmp3 = (nSendAddr % 100) / 10;
                int nTmp4 = nSendAddr % 10;

                mSendPackge.sendDataList.push_back('D');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));

                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x31);

                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(highByte);

                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(lowByte);

                mSendPackge.nReturnLen = 8 + 4;
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

            if(REG_TYPE_D_BIT == eRegType)
            {
                uchar nTmpValue = 0x05;
                mSendPackge.sendDataList.push_back(nTmpValue);

                //վ��
                nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf;
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

                nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf;
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

                mSendPackge.sendDataList.push_back('F');
                mSendPackge.sendDataList.push_back('F');
                mSendPackge.sendDataList.push_back('W');
                mSendPackge.sendDataList.push_back('W');
                mSendPackge.sendDataList.push_back('0');

                int nSendAddr = mAddrInfo.mAddrProp.nAddrValue/16;

                int nTmp1 = nSendAddr/ 1000;
                int nTmp2 = (nSendAddr % 1000) / 100;
                int nTmp3 = (nSendAddr % 100) / 10;
                int nTmp4 = nSendAddr % 10;

                mSendPackge.sendDataList.push_back('D');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));

                mSendPackge.sendDataList.push_back(0x30);
                mSendPackge.sendDataList.push_back(0x31);


                if(mAddrInfo.nValueList.size() == 4)
                {
                    ushort nValue = 0;
                    ushort n4Bit = (PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(0)) << 12) & 0xf000;
                    ushort n0Bit = (PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(1)) << 8) & 0x0f00;
                    ushort n12Bit = (PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(2)) << 4) & 0x00f0;
                    ushort n8Bit = (PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(3)) << 0) & 0x000f;
                    nValue = n12Bit + n8Bit + n4Bit + n0Bit;

                    int nBitPos = mAddrInfo.mAddrProp.nAddrValue%16;
                    /*�޸�д��ֵ, >0��λ*/
                    if(mCurrAddr.nValueList.front() > 0)
                    {
                        nValue |= 1 << nBitPos;
                    }
                    else
                    {
                        nValue &= ~(1 << nBitPos);
                    }


                    nTmp1 = nValue & 0xff00;
                    nTmp2 = (nTmp1 >> 12) & 0xf;
                    nTmp3 = (nTmp1 >> 8) & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

                    nTmp1 = nValue & 0x00ff;
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                }


                ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

                uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(highByte);

                uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
                mSendPackge.sendDataList.push_back(lowByte);

                mSendPackge.nReturnLen = 5;
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

    int nAddrValue = 0;
    int nAddrLen = 1;
    int nOffSetAddr = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_S:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('0');

            switch(eRegType)
            {
            case REG_TYPE_X:
                {
                    mSendPackge.sendDataList.push_back('X');
                    break;
                }
            case REG_TYPE_Y:
                {
                    mSendPackge.sendDataList.push_back('Y');
                    break;
                }
            case REG_TYPE_M:
            case REG_TYPE_SM:
                {
                    mSendPackge.sendDataList.push_back('M');
                    break;
                }
            case REG_TYPE_S:
                {
                    mSendPackge.sendDataList.push_back('S');
                    break;
                }
            default:
                break;
            }

            int nTmp1, nTmp2, nTmp3, nTmp4;
            if (eRegType == REG_TYPE_X || eRegType == REG_TYPE_Y)
            {
                nTmp1 = mAddrInfo.nAddrValue / 512;
                nTmp2 = (mAddrInfo.nAddrValue % 512) / 64;
                nTmp3 = (mAddrInfo.nAddrValue % 64) / 8;
                nTmp4 = mAddrInfo.nAddrValue % 8;
            }
            else
            {
                nTmp1 = mAddrInfo.nAddrValue / 1000;
                nTmp2 = (mAddrInfo.nAddrValue % 1000) / 100;
                nTmp3 = (mAddrInfo.nAddrValue % 100) / 10;
                nTmp4 = mAddrInfo.nAddrValue % 10;
            }

            if (eRegType == REG_TYPE_SM)
            {
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1 + 8));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
            }
            else
            {
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
            }

            if(mAddrInfo.nAddrLen%16 == 0)
            {
                nAddrLen = mAddrInfo.nAddrLen/16;
            }
            else
            {
                nAddrLen = mAddrInfo.nAddrLen/16 + 1;
            }
//            nAddrLen = 0;
//            if(mAddrInfo.nAddrLen %2 == 0)
//            {
//                nAddrLen = mAddrInfo.nAddrLen/2;
//            }
//            else
//            {
//                nAddrLen = mAddrInfo.nAddrLen/2 + 1;
//            }

            nTmp1 = nAddrLen & 0xf;
            nTmp2 = (nAddrLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 8 + nAddrLen*4;

            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('B');
            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('0');

            switch (eRegType)
            {
            case REG_TYPE_T:
                mSendPackge.sendDataList.push_back('T');
                mSendPackge.sendDataList.push_back('S');
                break;
            case REG_TYPE_C:
                mSendPackge.sendDataList.push_back('C');
                mSendPackge.sendDataList.push_back('S');
                break;
            default:
                break;
            }

            /*����ʼ��ַ��һ����Сȡһ��byte*/
            nAddrValue = mAddrInfo.nAddrValue;
            int nTmp1 = (nAddrValue % 1000) / 100;
            int nTmp2 = (nAddrValue % 100) / 10;
            int nTmp3 = nAddrValue % 10;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

            if(mAddrInfo.nAddrLen %8 == 0)
            {
                nAddrLen = mAddrInfo.nAddrLen/8;
            }
            else
            {
                nAddrLen = mAddrInfo.nAddrLen /8 +1;
            }
            nAddrLen = nAddrLen * 8;
//            nAddrLen = mAddrInfo.nAddrLen * 8;
            nTmp1 = nAddrLen & 0xf;
            nTmp2 = (nAddrLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 8 + nAddrLen;
            break;
        }
    case REG_TYPE_D_BIT:
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('0');

            int nTmp1 = mAddrInfo.nAddrValue / 1000;
            int nTmp2 = (mAddrInfo.nAddrValue % 1000) / 100;
            int nTmp3 = (mAddrInfo.nAddrValue % 100) / 10;
            int nTmp4 = mAddrInfo.nAddrValue % 10;

            switch (eRegType)
            {
            case REG_TYPE_D_BIT:
            case REG_TYPE_D:
                mSendPackge.sendDataList.push_back('D');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_SD:
                mSendPackge.sendDataList.push_back('D');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1 + 8));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back('T');
                mSendPackge.sendDataList.push_back('N');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back('C');
                mSendPackge.sendDataList.push_back('N');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            default:
                break;
            }

            nAddrLen = mAddrInfo.nAddrLen;
            nTmp1 = nAddrLen & 0xf;
            nTmp2 = (nAddrLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 8 + nAddrLen * 4;
            break;
        }
    case REG_TYPE_32CV:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('0');
            mSendPackge.sendDataList.push_back('C');
            mSendPackge.sendDataList.push_back('N');

            int nTmp1 = mAddrInfo.nAddrValue / 1000;
            int nTmp2 = (mAddrInfo.nAddrValue % 1000) / 100;
            int nTmp3 = (mAddrInfo.nAddrValue % 100) / 10;
            int nTmp4 = mAddrInfo.nAddrValue % 10;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2 + 2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));

            nAddrLen = mAddrInfo.nAddrLen;
            if(mAddrInfo.nAddrValue + mAddrInfo.nAddrLen > 56)
            {
                nAddrLen = 56 - mAddrInfo.nAddrValue;
            }

            nTmp1 = nAddrLen & 0xf;
            nTmp2 = (nAddrLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 8 + nAddrLen * 8;
            break;
        }
    case REG_TYPE_R:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('Q');
            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('0');
            mSendPackge.sendDataList.push_back('R');

            int nTmp1 = mAddrInfo.nAddrValue / 100000;
            int nTmp2 = (mAddrInfo.nAddrValue % 100000) / 10000;
            int nTmp3 = (mAddrInfo.nAddrValue % 10000) / 1000;
            int nTmp4 = (mAddrInfo.nAddrValue % 1000) / 100;
            int nTmp5 = (mAddrInfo.nAddrValue % 100) / 10;
            int nTmp6 = mAddrInfo.nAddrValue % 10;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp5));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp6));

            nAddrLen = mAddrInfo.nAddrLen;
            nTmp1 = nAddrLen & 0xf;
            nTmp2 = (nAddrLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 8 + nAddrLen * 4;
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
    /*��ʼ��*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    uchar nFumNum = 0;
    bool bSuccess = false;
    int nOffsetAddr = 0;
    int nAddrValue = 0;
    int nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_SM:
    case REG_TYPE_S:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('B');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('0');

            int nTmp1, nTmp2, nTmp3, nTmp4;
            if (eRegType == REG_TYPE_Y)
            {
                nTmp1 = mAddrInfo.mAddrProp.nAddrValue / 512;
                nTmp2 = (mAddrInfo.mAddrProp.nAddrValue % 512) / 64;
                nTmp3 = (mAddrInfo.mAddrProp.nAddrValue % 64) / 8;
                nTmp4 = mAddrInfo.mAddrProp.nAddrValue % 8;
            }
            else
            {
                nTmp1 = mAddrInfo.mAddrProp.nAddrValue / 1000;
                nTmp2 = (mAddrInfo.mAddrProp.nAddrValue % 1000) / 100;
                nTmp3 = (mAddrInfo.mAddrProp.nAddrValue % 100) / 10;
                nTmp4 = mAddrInfo.mAddrProp.nAddrValue % 10;
            }

            switch(eRegType)
            {
            case REG_TYPE_Y:
                mSendPackge.sendDataList.push_back('Y');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_M:
                mSendPackge.sendDataList.push_back('M');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_SM:
                mSendPackge.sendDataList.push_back('M');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1 + 8));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_S:
                mSendPackge.sendDataList.push_back('S');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_T:
                mSendPackge.sendDataList.push_back('T');
                mSendPackge.sendDataList.push_back('S');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_C:
                mSendPackge.sendDataList.push_back('C');
                mSendPackge.sendDataList.push_back('S');
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            }

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            if(mAddrInfo.nValueList.size() > 0)
            {
                if(mAddrInfo.nValueList.front() == 1)
                {
                    mSendPackge.sendDataList.push_back(0x31);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(0x30);
                }
            }

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 5;
            bSuccess = true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            mCmnProp.eSendType = CMN_READ_BEFORE_WRITE;
            return makePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_SD:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
    case REG_TYPE_R:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('0');

            int nTmp1 = mAddrInfo.mAddrProp.nAddrValue / 1000;
            int nTmp2 = (mAddrInfo.mAddrProp.nAddrValue % 1000) / 100;
            int nTmp3 = (mAddrInfo.mAddrProp.nAddrValue % 100) / 10;
            int nTmp4 = mAddrInfo.mAddrProp.nAddrValue % 10;

            switch(eRegType)
            {
            case REG_TYPE_D:
            case REG_TYPE_D_BIT:
                mSendPackge.sendDataList.push_back('D');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_SD:
                mSendPackge.sendDataList.push_back('D');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1 + 8));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_TV:
                mSendPackge.sendDataList.push_back('T');
                mSendPackge.sendDataList.push_back('N');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            case REG_TYPE_CV:
                mSendPackge.sendDataList.push_back('C');
                mSendPackge.sendDataList.push_back('N');

                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));
                break;
            }

            int nSendLen = mAddrInfo.mAddrProp.nAddrLen;
            nTmp1 = nSendLen & 0xf;
            nTmp2 = (nSendLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            for (int i=0; i<nSendLen*4; i++)
            {
                if ((i*2 + 1) < mAddrInfo.nValueList.size())
                {
                    nTmp1 = mAddrInfo.nValueList.at(i*2 + 1);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

                    nTmp1 = mAddrInfo.nValueList.at(i*2 + 0);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                }
            }

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 5;
            bSuccess = true;
            break;
        }
    case REG_TYPE_32CV:
        {
            /*��ʼ���̶�*/
            uchar nTmpValue = 0x05;
            mSendPackge.sendDataList.push_back(nTmpValue);

            //վ��
            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            nTmpValue = (mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmpValue));

            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('F');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('0');

            int nTmp1 = mAddrInfo.mAddrProp.nAddrValue / 1000;
            int nTmp2 = (mAddrInfo.mAddrProp.nAddrValue % 1000) / 100;
            int nTmp3 = (mAddrInfo.mAddrProp.nAddrValue % 100) / 10;
            int nTmp4 = mAddrInfo.mAddrProp.nAddrValue % 10;

            mSendPackge.sendDataList.push_back('C');
            mSendPackge.sendDataList.push_back('N');

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2 + 2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp4));

            int nSendLen = mAddrInfo.mAddrProp.nAddrLen/2;
            if(nSendLen == 0)
            {
                nSendLen = 1;
            }
            nTmp1 = nSendLen & 0xf;
            nTmp2 = (nSendLen >> 4) & 0xf;
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp1));

            for (int i=0; i<nSendLen; i++)
            {
                if ((i*4 + 3) < mAddrInfo.nValueList.size())
                {
                    nTmp1 = mAddrInfo.nValueList.at(i*4 + 3);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

                    nTmp1 = mAddrInfo.nValueList.at(i*4 + 2);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

                    nTmp1 = mAddrInfo.nValueList.at(i*4 + 1);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));

                    nTmp1 = mAddrInfo.nValueList.at(i*4 + 0);
                    nTmp2 = (nTmp1 >> 4) & 0xf;
                    nTmp3 = nTmp1 & 0xf;
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(nTmp3));
                }
            }

            ushort nCheckResult = PlcMathMethod::getSumCheck(mSendPackge.sendDataList,1) & 0xff;

            uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(highByte);

            uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(lowByte);

            mSendPackge.nReturnLen = 5;
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
    vector<ADDR_TARGET_PROP > mTmpListWord4;

    vector<ADDR_TARGET_PROP > mTmpListDWord0;

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
        case REG_TYPE_S:
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
        case REG_TYPE_D_BIT:
            {
                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                mTmpAddr.nRegIndex = REG_TYPE_D;

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
        case REG_TYPE_R:
            {
                mTmpListWord4.push_back(mTmpAddr);
            }
        case REG_TYPE_32CV:
            {
                mTmpListDWord0.push_back(mTmpAddr);
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

    mTmpListWord4 = PlcMathMethod::addrBubbleSort(mTmpListWord4);
    mTmpListWord4 = PlcMathMethod::addrSortOut(mTmpListWord4, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord4.at(i));
    }

    mTmpListDWord0 = PlcMathMethod::addrBubbleSort(mTmpListDWord0);
    mTmpListDWord0 = PlcMathMethod::addrSortOut(mTmpListDWord0, nMaxRWlen/2, 1);
    tmpSize = mTmpListDWord0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListDWord0.at(i));
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
    mSerialInfo.DATA_BIT = 7;
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue > ADDR_MAX_VALUE_32)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_800)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_TV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 8126)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 9000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_128000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen/2 > 280)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_R:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
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
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_800)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_TV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_M:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 8126)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_SD:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 9000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_D_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_128000)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_200)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_32CV:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen/2 > 280)
            {
                return false;
            }
            return true;
            break;
        }
    case REG_TYPE_R:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
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
            eRegType = REG_TYPE_S;
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
            eRegType = REG_TYPE_D_BIT;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_D;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_SD;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_R;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_TV;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_CV;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_32CV;
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
    else if((DealWithString::compare("S", sRegName)))
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
    else if((DealWithString::compare("D_BIT", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("D", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("SD", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("R", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("TV", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("CV", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("32CV", sRegName)))
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
 * Funciton: ��ȡЭ�����������
 * Parameters:
 * Return: Э������
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return SERIAL_PORT;
}


