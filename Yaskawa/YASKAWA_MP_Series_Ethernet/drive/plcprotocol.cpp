#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*д���ݷ��صĳ���*/
#define WRITE_RETURN_LEN      8

/*�����ݷ��ص�����ͷ������*/
#define READ_HEAD_LEN         30

/*�����ݷ��ص�����У��ĳ���*/
#define CHECK_LEN             20

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
    case REG_TYPE_IB:
    case REG_TYPE_OB:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536 * 16)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_OW:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536)
            {
                return false;
            }
            return bSuccess;
            break;
        }
    case REG_TYPE_MB:
    case REG_TYPE_SB:
        {
            if (mAddrSource.sAddrValue.size() == 1)
            {
                mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);

                /*�жϳ����Ƿ���*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536)
                {
                    return false;
                }
                return bSuccess;
            }
            else if (mAddrSource.sAddrValue.size() > 1)
            {
                string IntPass = DealWithString::mid(mAddrSource.sAddrValue,0,mAddrSource.sAddrValue.size() - 1);//mAddrSource.sAddrValue.left(mAddrSource.sAddrValue.size() - 1);
                string DecPass = DealWithString::mid(mAddrSource.sAddrValue,mAddrSource.sAddrValue.size() -1,1);//mAddrSource.sAddrValue.mid(IntPass.size(),1);

                bool bRet = false;

                int IntTemp = DealWithString::stringToInt(IntPass,bRet);//IntPass.toInt();
                if(!bRet)
                {
                    return false;
                }
                int DecTemp = DealWithString::stringToInt(DecPass,bRet,16);//DecPass.toInt(&bRet,16);
                if(!bRet)
                {
                    return false;
                }

                mAddrTarget.nAddrValue = IntTemp*16 + DecTemp;

                /*�жϳ����Ƿ���*/
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536 * 16)
                {
                    return false;
                }
                bSuccess = true;
                return bSuccess;
            }
            break;
        }
    default:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*�жϳ����Ƿ���*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536)
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
            sRegNameList.push_back("IB");
            sRegNameList.push_back("OB");
            sRegNameList.push_back("MB");
            sRegNameList.push_back("SB");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IW");
            sRegNameList.push_back("OW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
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
            sRegNameList.push_back("IB");
            sRegNameList.push_back("OB");
            sRegNameList.push_back("MB");
            sRegNameList.push_back("SB");
            sRegNameList.push_back("IW");
            sRegNameList.push_back("OW");
            sRegNameList.push_back("MW");
            sRegNameList.push_back("SW");
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
    case REG_TYPE_IB:
    case REG_TYPE_OB:
    case REG_TYPE_MB:
    case REG_TYPE_SB:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_OW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
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
    case REG_TYPE_IB:
    case REG_TYPE_OB:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess,16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_65536 * 16)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536 * 16)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_IW:
    case REG_TYPE_OW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess,16);
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

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            return ADDR_CORRECT;
            break;
        }
    case REG_TYPE_MB:
    case REG_TYPE_SB:
        {
            if(mStartAddr.sAddrValue.size() > 1)
            {
                string sIntStr = DealWithString::mid(mStartAddr.sAddrValue,0,mStartAddr.sAddrValue.size() - 1);//mAddrSource.sAddrValue.left(mAddrSource.sAddrValue.size() - 1);
                string sDecStr = DealWithString::mid(mStartAddr.sAddrValue,mStartAddr.sAddrValue.size() -1,1);//mAddrSource.sAddrValue.mid(IntPass.size(),1);
                //string sIntStr = mStartAddr.sAddrValue.left(mStartAddr.sAddrValue.size() - 1);
                //string sDecStr = mStartAddr.sAddrValue.mid(sIntStr.size(),1);

                /*��λ���ֵ�ת��*/
                int nIntValue = DealWithString::stringToInt(sIntStr,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                if(nIntValue < 0 || nIntValue >= ADDR_MAX_VALUE_65536 * 16)
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
                if(nIntValue < 0 || nDecValue < 0 || nDecValue >= 16)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nDecValue + nInterval >= 16)
                    {
                        nIntValue += (nDecValue + nInterval)/16;
                        nDecValue = (nDecValue + nInterval)%16;
                    }
                    else
                    {
                        nDecValue += nInterval;
                    }

                    if(nIntValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536 * 16)
                    {
                        /*�����������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;

                        /*���С������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%x", nDecValue);
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
                /*��λ���ֵ�ת��*/
                int nIntValue = 0;

                /*��λ���ֵ�ת��*/
                int nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess,16);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }

                /*С�����ֲ��ܳ���16��Ҳ����Ϊ����*/
                if(nIntValue < 0 || nDecValue < 0 || nDecValue >= 16)
                {
                    return ILLEGAL_VALUE;
                }

                char buf[10] = {0};
                string sTmpStr;
                for(int i = 0; i < nCopyNum; i++)
                {
                    if(nDecValue + nInterval >= 16)
                    {
                        nIntValue += (nDecValue + nInterval)/16;
                        nDecValue = (nDecValue + nInterval)%16;
                    }
                    else
                    {
                        nDecValue += nInterval;
                    }

                    if(nIntValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536 * 16)
                    {
                        /*�����������*/
                        if(nIntValue > 0)
                        {
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nIntValue);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr;
                        }
                        else
                        {
                            mTmpAddr.sAddrValue = "";
                        }

                        /*���С������*/
                        memset(buf, 0, 10);
                        sprintf(buf, "%x", nDecValue);
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
            break;
        }
    case REG_TYPE_MW:
    case REG_TYPE_SW:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess,16);
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

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536)
                {
                    /*�����������*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
            }
            return ADDR_CORRECT;
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
        targetList.clear();
        for(int i = READ_HEAD_LEN; i < nSize; i++)
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
    if(nRcvSize < 1)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    mUnpackProp.bNeedSetData = false;

    if (CHECK_READ == eCheckType)
    {
        mUnpackProp.bNeedSetData = true;//�����д���
        eCmnInfo = CMN_NOMAL_CODE;
    }
    else if (CHECK_WRITE == eCheckType)
    {
        mUnpackProp.bNeedSetData = true;//д������
        eCmnInfo = CMN_NOMAL_CODE;
    }
    return eCmnInfo;
}

/******************************************************************
 * Function: �������Э��İ�
 * Parameters: address list
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
    uchar nStationId = mAddrInfo.nPlcStationIndex & 0xff;
    ushort nCrcResult;
    uchar nCrcResultL;

    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x7f);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x1a);//lenght,after this char

    mSendPackge.sendDataList.push_back(0x14);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x07);
    mSendPackge.sendDataList.push_back(0x10);

    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);

    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);

    mSendPackge.sendDataList.push_back(0x01);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    switch (eRegType)//regester code
    {
    case REG_TYPE_SB:
    case REG_TYPE_SW:
        mSendPackge.sendDataList.push_back(0x00);
        break;
    case REG_TYPE_IB:
    case REG_TYPE_IW:
        mSendPackge.sendDataList.push_back(0x01);
        break;
    case REG_TYPE_OB:
    case REG_TYPE_OW:
        mSendPackge.sendDataList.push_back(0x02);
        break;
    case REG_TYPE_MB:
    case REG_TYPE_MW:
        mSendPackge.sendDataList.push_back(0x03);
        break;
    }

    mSendPackge.sendDataList.push_back(0x10);

    unsigned char cTmp1 = (mAddrInfo.nAddrValue >> 8) & 0xff;
    unsigned char cTmp2 = (mAddrInfo.nAddrValue >> 0) & 0xff;
    mSendPackge.sendDataList.push_back(cTmp2);
    mSendPackge.sendDataList.push_back(cTmp1);

    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);
    mSendPackge.sendDataList.push_back(0x00);

    cTmp1 = (mAddrInfo.nAddrLen >> 8) & 0xff;
    cTmp2 = (mAddrInfo.nAddrLen >> 0) & 0xff;
    mSendPackge.sendDataList.push_back(cTmp2);
    mSendPackge.sendDataList.push_back(cTmp1);


    mSendPackge.nReturnLen = 30 + mAddrInfo.nAddrLen*2;                //�������ֽ���
    return true;
    //PRO_TEMP.iRet_ValidStar = 30;                                   //�������ÿ�ʼ�ֽ���
    //PRO_TEMP.iRet_ValidLen = RW_Var->i_SOURCE_LEN*2;                //���������ֽڳ���
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

    vector<uchar > nTmpWriteList;
    nTmpWriteList.clear();
    uchar nStationId = mAddrInfo.mAddrProp.nPlcStationIndex & 0xff;
    ushort nCrcResult;
    uchar nCrcResultL;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_IB:
    case REG_TYPE_OB:
    case REG_TYPE_MB:
    case REG_TYPE_SB:
        {
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x7f);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x1c);//lenght,after this char

            mSendPackge.sendDataList.push_back(0x16);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x07);
            mSendPackge.sendDataList.push_back(0x10);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x01);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            switch (eRegType)//regester code
            {
            case REG_TYPE_SB:
            case REG_TYPE_SW:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            case REG_TYPE_IB:
            case REG_TYPE_IW:
                mSendPackge.sendDataList.push_back(0x01);
                break;
            case REG_TYPE_OB:
            case REG_TYPE_OW:
                mSendPackge.sendDataList.push_back(0x02);
                break;
            case REG_TYPE_MB:
            case REG_TYPE_MW:
                mSendPackge.sendDataList.push_back(0x03);
                break;
            }

            mSendPackge.sendDataList.push_back(0x10);

            unsigned char cTmp1 = ((mAddrInfo.mAddrProp.nAddrValue / 16) >> 8) & 0xff;
            unsigned char cTmp2 = ((mAddrInfo.mAddrProp.nAddrValue / 16) >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(cTmp2);
            mSendPackge.sendDataList.push_back(cTmp1);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            int nTmp1 = (mAddrInfo.mAddrProp.nAddrValue % 16);
            int nTmp2 = (1 << nTmp1) & 0xffff;

            mSendPackge.sendDataList.push_back(((~nTmp2) >> 0) & 0xff);
            mSendPackge.sendDataList.push_back(((~nTmp2) >> 8) & 0xff);

            if (!mAddrInfo.nValueList.empty())
            {
                if (mAddrInfo.nValueList.at(0) == 1)//set bit
                {
                    mSendPackge.sendDataList.push_back((nTmp2 >> 0) & 0xff);
                    mSendPackge.sendDataList.push_back((nTmp2 >> 8) & 0xff);
                }
                else//reset bit
                {
                    mSendPackge.sendDataList.push_back(0x00);
                    mSendPackge.sendDataList.push_back(0x00);//index 31
                }
            }

            mSendPackge.nReturnLen = 20;
        }
        break;
    case REG_TYPE_IW:
    case REG_TYPE_OW:
    case REG_TYPE_MW:
    case REG_TYPE_SW:
        {
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x7f);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x1a + mAddrInfo.mAddrProp.nAddrLen * 2);//lenght,after this char

            mSendPackge.sendDataList.push_back(0x15);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x07);
            mSendPackge.sendDataList.push_back(0x10);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x01);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            switch (eRegType)//regester code
            {
            case REG_TYPE_SB:
            case REG_TYPE_SW:
                mSendPackge.sendDataList.push_back(0x00);
                break;
            case REG_TYPE_IB:
            case REG_TYPE_IW:
                mSendPackge.sendDataList.push_back(0x01);
                break;
            case REG_TYPE_OB:
            case REG_TYPE_OW:
                mSendPackge.sendDataList.push_back(0x02);
                break;
            case REG_TYPE_MB:
            case REG_TYPE_MW:
                mSendPackge.sendDataList.push_back(0x03);//21
                break;
            }

            mSendPackge.sendDataList.push_back(0x10);

            unsigned char cTmp1 = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xff;
            unsigned char cTmp2 = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(cTmp2);
            mSendPackge.sendDataList.push_back(cTmp1);

            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);
            mSendPackge.sendDataList.push_back(0x00);

            cTmp1 = (mAddrInfo.mAddrProp.nAddrLen >> 8) & 0xff;
            cTmp2 = (mAddrInfo.mAddrProp.nAddrLen >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(cTmp2);
            mSendPackge.sendDataList.push_back(cTmp1);

            for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)
            {
                if ((1 + i*2) < mAddrInfo.nValueList.size())
                {
                    cTmp1 = mAddrInfo.nValueList.at(0 + i*2) & 0xff;
                    cTmp2 = mAddrInfo.nValueList.at(1 + i*2) & 0xff;
                    mSendPackge.sendDataList.push_back(cTmp1);//30 + i*2
                    mSendPackge.sendDataList.push_back(cTmp2);//31 + i*2
                }
            }

            mSendPackge.nReturnLen = 20;
        }
        break;
    default:
        {
            return false;
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

    /*���Ĵ��������� λ�Ļ���*/
    vector<ADDR_TARGET_PROP > mTmpList0;
    vector<ADDR_TARGET_PROP > mTmpList1;
    vector<ADDR_TARGET_PROP > mTmpList2;
    vector<ADDR_TARGET_PROP > mTmpList3;

    /*�ֵĻ���*/
//    vector<ADDR_TARGET_PROP > mTmpList4;
//    vector<ADDR_TARGET_PROP > mTmpList5;
//    vector<ADDR_TARGET_PROP > mTmpList6;
//    vector<ADDR_TARGET_PROP > mTmpList7;


    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);


        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_IB:
            {

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/16;

                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_OB:
            {

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/16;

                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MB:
            {

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/16;

                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SB:
            {

                /*��ʼλ��Ϊ�����ֵ���ʼλ��*/
                int tmpStartValue = mTmpAddr.nAddrValue;
                mTmpAddr.nAddrValue = tmpStartValue/16;

                if((mTmpAddr.nAddrLen + tmpStartValue%16)%16 == 0)
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16;
                }
                else
                {
                    mTmpAddr.nAddrLen = (mTmpAddr.nAddrLen + tmpStartValue%16)/16 +1;
                }

                mTmpList3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_IW:
            {
                mTmpList0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_OW:
            {
                mTmpList1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MW:
            {
                mTmpList2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SW:
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
    mTmpList0 = PlcMathMethod::addrSortOut(mTmpList0, nMaxRWlen/2, 1);
    int tmpSize = mTmpList0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList0.at(i));
    }

    mTmpList1 = PlcMathMethod::addrBubbleSort(mTmpList1);
    mTmpList1 = PlcMathMethod::addrSortOut(mTmpList1, nMaxRWlen/2, 1);
    tmpSize = mTmpList1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList1.at(i));
    }

    mTmpList2 = PlcMathMethod::addrBubbleSort(mTmpList2);
    mTmpList2 = PlcMathMethod::addrSortOut(mTmpList2, nMaxRWlen/2, 1);
    tmpSize = mTmpList2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList2.at(i));
    }

    mTmpList3 = PlcMathMethod::addrBubbleSort(mTmpList3);
    mTmpList3 = PlcMathMethod::addrSortOut(mTmpList3, nMaxRWlen/2, 1);
    tmpSize = mTmpList3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpList3.at(i));
    }

//    mTmpList4 = PlcMathMethod::addrBubbleSort(mTmpList4);
//    mTmpList4 = PlcMathMethod::addrSortOut(mTmpList4, nMaxRWlen/2, 1);
//    tmpSize = mTmpList4.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpList4.at(i));
//    }

//    mTmpList5 = PlcMathMethod::addrBubbleSort(mTmpList5);
//    mTmpList5 = PlcMathMethod::addrSortOut(mTmpList5, nMaxRWlen/2, 1);
//    tmpSize = mTmpList5.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpList5.at(i));
//    }

//    mTmpList6 = PlcMathMethod::addrBubbleSort(mTmpList6);
//    mTmpList6 = PlcMathMethod::addrSortOut(mTmpList6, nMaxRWlen/2, 1);
//    tmpSize = mTmpList6.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpList6.at(i));
//    }

//    mTmpList7 = PlcMathMethod::addrBubbleSort(mTmpList7);
//    mTmpList7 = PlcMathMethod::addrSortOut(mTmpList7, nMaxRWlen/2, 1);
//    tmpSize = mTmpList7.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpList7.at(i));
//    }
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
    mSerialInfo.BAUD_RATE = 9600;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 2;
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
            eRegType = REG_TYPE_IB;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_OB;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_MB;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_SB;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_IW;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_OW;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_MW;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_SW;
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

    if(DealWithString::compare("IB", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("OB", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("MB", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("SB", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("IW", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("OW", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("MW", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("SW", sRegName)))
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
    return NET_PORT;
}


