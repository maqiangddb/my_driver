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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_4096 * 16)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TC:
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
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
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
    case REG_TYPE_DM:
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
            sRegNameList.push_back("IR_BIT");
            sRegNameList.push_back("HR_BIT");
            sRegNameList.push_back("LR_BIT");
            sRegNameList.push_back("AR_BIT");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("IR");
            sRegNameList.push_back("HR");
            sRegNameList.push_back("LR");
            sRegNameList.push_back("AR");
            sRegNameList.push_back("DM");
            sRegNameList.push_back("TC");
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
            sRegNameList.push_back("IR_BIT");
            sRegNameList.push_back("HR_BIT");
            sRegNameList.push_back("LR_BIT");
            sRegNameList.push_back("AR_BIT");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            sRegNameList.push_back("IR");
            sRegNameList.push_back("HR");
            sRegNameList.push_back("LR");
            sRegNameList.push_back("AR");
            sRegNameList.push_back("DM");
            sRegNameList.push_back("TC");
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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
    case REG_TYPE_DM:
    case REG_TYPE_TC:
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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
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
                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_4096 * 16)
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
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TC:
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
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
        {
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
    case REG_TYPE_DM:
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
        int nSize = rcvList.size() - 4;
        targetList.clear();
        for(int i = 7; i < nSize; i++)
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
    if(nRcvSize < 11)
    {
        eCmnInfo = DATA_LEN_ERROR;
        return eCmnInfo;
    }

    uchar nRcvHighByte = rcvList.at(nRcvSize - 4);
    uchar nRcvLowByte = rcvList.at(nRcvSize - 3);

    vector<uchar> templist;
    templist.clear();
    int size = nRcvSize - 4;
    for (int i=0; i<size; i++)
    {
        templist.push_back(rcvList.at(i));
    }

    ushort nCheckResult = PlcMathMethod::getXORcheck(templist);
    uchar highByte = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
    uchar lowByte = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);

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
    switch (nPkgIndex)
    {
    case 1:
        {
            mHandShake.sendDataList.clear();
            mHandShake.sendDataList.push_back(0x40);//��ʼ�ַ�@

            uchar ucTmp1 = PlcMathMethod::intToAsciiValue((nStationId >> 4) & 0xf);
            uchar ucTmp2 = PlcMathMethod::intToAsciiValue((nStationId >> 0) & 0xf);

            mHandShake.sendDataList.push_back(ucTmp1);//վ��
            mHandShake.sendDataList.push_back(ucTmp2);

            mHandShake.sendDataList.push_back(0x53);//������SC���޸�PLC״̬
            mHandShake.sendDataList.push_back(0x43);
            mHandShake.sendDataList.push_back(0x30);//�޸�PLC״̬Ϊ����ģʽ��0��̣�2���ӣ�3����
            mHandShake.sendDataList.push_back(0x32);

            ushort nCheckResult = PlcMathMethod::getXORcheck(mHandShake.sendDataList);//����У��

            ucTmp1 = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            ucTmp2 = PlcMathMethod::intToAsciiValue((nCheckResult >> 0) & 0xf);

            mHandShake.sendDataList.push_back(ucTmp1);
            mHandShake.sendDataList.push_back(ucTmp2);

            mHandShake.sendDataList.push_back(0x2a);//�����ַ�*CR
            mHandShake.sendDataList.push_back(0x0d);

            mHandShake.nReturnLen = 11;
            nCmnTimes = 1;
            return true;
        }
        break;
    default:
        return false;
        break;
    }

    return false;
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
    case 1:
        {
            if (nRcvSize == 11)
            {
                if (rcvList.at(0) == 0x40 && rcvList.at(5) == 0x30 && rcvList.at(6) == 0x30 &&
                    rcvList.at(9) == 0x2a && rcvList.at(10) == 0x0d)
                {
                    return CMN_NOMAL_CODE;
                }
                else
                {
                    return DATA_CHECK_ERROR;
                }
            }
            else
            {
                return DATA_LEN_ERROR;
            }
        }
        break;
    default:
        break;
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
    /*��ʼ��*/
    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

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
            mCurrAddr.nValueList = mAddrInfo.nValueList;

            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch (eRegType)
            {
            case REG_TYPE_IR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('R');
                }
                break;
            case REG_TYPE_HR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('H');
                }
                break;
            case REG_TYPE_LR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('L');
                }
                break;
            case REG_TYPE_AR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('J');
                }
                break;
            default :
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }

            uint nAddrValue = mAddrInfo.mAddrProp.nAddrValue/16;

            uchar uctmp1 = (nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 15;
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            if(mAddrInfo.nValueList.empty() || mCurrAddr.nValueList.empty()) break;

            mAddrInfo.nValueList.resize(4);
            uchar uctmp1 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(0));//12
            uchar uctmp2 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(1));//8
            uchar uctmp3 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(2));//4
            uchar uctmp4 = PlcMathMethod::asciiToIntValue(mAddrInfo.nValueList.at(3));//0

            uint nWriteValue = ((uctmp1 << 12) & 0xf000) + ((uctmp2 << 8) & 0x0f00) +
                               ((uctmp3 << 4) & 0x00f0) + (uctmp4& 0x000f);

            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch (eRegType)
            {
            case REG_TYPE_IR_BIT:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('R');
                }
                break;
            case REG_TYPE_HR_BIT:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('H');
                }
                break;
            case REG_TYPE_LR_BIT:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('L');
                }
                break;
            case REG_TYPE_AR_BIT:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('J');
                }
                break;
            default :
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }

            uint nAddrValue = mAddrInfo.mAddrProp.nAddrValue/16;

            uctmp1 = (nAddrValue/1000) & 0xf;
            uctmp2 = ((nAddrValue%1000)/100) & 0xf;
            uctmp3 = ((nAddrValue%100)/10) & 0xf;
            uctmp4 = (nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            uint nBitPos = mAddrInfo.mAddrProp.nAddrValue%16;

            if(mCurrAddr.nValueList.at(0) > 0)
            {
                nWriteValue |= 1 << nBitPos;
            }
            else
            {
                nWriteValue &= ~(1 << nBitPos);
            }

            uctmp1 = (nWriteValue >> 12) & 0xf;
            uctmp2 = (nWriteValue >> 8) & 0xf;
            uctmp3 = (nWriteValue >> 4) & 0xf;
            uctmp4 = (nWriteValue >> 0) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 11;
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

    uint nAddrValue = 0;
    uint nAddrLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch (eRegType)
            {
            case REG_TYPE_IR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('R');
                }
                break;
            case REG_TYPE_HR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('H');
                }
                break;
            case REG_TYPE_LR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('L');
                }
                break;
            case REG_TYPE_AR_BIT:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('J');
                }
                break;
            default :
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }

            nAddrValue = mAddrInfo.nAddrValue/16;//��ʼ��ַ
            uchar uctmp1 = (nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            if((mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)%16 == 0)//��ȡ����
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16;
            }
            else
            {
                nAddrLen = (mAddrInfo.nAddrValue%16 + mAddrInfo.nAddrLen)/16 +1;
            }

            uctmp1 = (nAddrLen/1000) & 0xf;
            uctmp2 = ((nAddrLen%1000)/100) & 0xf;
            uctmp3 = ((nAddrLen%100)/10) & 0xf;
            uctmp4 = (nAddrLen%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 11 + nAddrLen*4;
            break;
        }
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
    case REG_TYPE_DM:
    case REG_TYPE_TC:
        {
            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch (eRegType)
            {
            case REG_TYPE_IR:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('R');
                }
                break;
            case REG_TYPE_HR:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('H');
                }
                break;
            case REG_TYPE_LR:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('L');
                }
                break;
            case REG_TYPE_AR:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('J');
                }
                break;
            case REG_TYPE_DM:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('D');
                }
                break;
            case REG_TYPE_TC:
                {
                    mSendPackge.sendDataList.push_back('R');
                    mSendPackge.sendDataList.push_back('C');
                }
                break;
            default :
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }

            uchar uctmp1 = (mAddrInfo.nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((mAddrInfo.nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((mAddrInfo.nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (mAddrInfo.nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            uctmp1 = (mAddrInfo.nAddrLen/1000) & 0xf;
            uctmp2 = ((mAddrInfo.nAddrLen%1000)/100) & 0xf;
            uctmp3 = ((mAddrInfo.nAddrLen%100)/10) & 0xf;
            uctmp4 = (mAddrInfo.nAddrLen%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 11 + mAddrInfo.nAddrLen*4;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back('R');
            mSendPackge.sendDataList.push_back('G');

            uchar uctmp1 = (mAddrInfo.nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((mAddrInfo.nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((mAddrInfo.nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (mAddrInfo.nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x30);
            mSendPackge.sendDataList.push_back(0x31);

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 12;
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

    bool bSuccess = false;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            mCmnProp.eSendType = CMN_READ_BEFORE_WRITE;
            return makePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
    case REG_TYPE_DM:
    case REG_TYPE_TC:
        {
            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch (eRegType)
            {
            case REG_TYPE_IR:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('R');
                }
                break;
            case REG_TYPE_HR:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('H');
                }
                break;
            case REG_TYPE_LR:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('L');
                }
                break;
            case REG_TYPE_AR:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('J');
                }
                break;
            case REG_TYPE_DM:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('D');
                }
                break;
            case REG_TYPE_TC:
                {
                    mSendPackge.sendDataList.push_back('W');
                    mSendPackge.sendDataList.push_back('C');
                }
                break;
            default :
                {
                    mSendPackge.sendDataList.clear();
                    return false;
                }
                break;
            }

            uchar uctmp1 = (mAddrInfo.mAddrProp.nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((mAddrInfo.mAddrProp.nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((mAddrInfo.mAddrProp.nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (mAddrInfo.mAddrProp.nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            mAddrInfo.nValueList.resize(mAddrInfo.mAddrProp.nAddrLen * 2);

            for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)
            {
                uctmp1 = mAddrInfo.nValueList.at(i*2 + 1);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                uctmp1 = mAddrInfo.nValueList.at(i*2 + 0);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 4) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = PlcMathMethod::intToAsciiValue((uctmp1 >> 0) & 0xf);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 11;
            bSuccess = true;
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            mSendPackge.sendDataList.push_back(0x40);//��ʼ��

            uchar nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 4) & 0xf);//վ��
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue((mAddrInfo.mAddrProp.nPlcStationIndex >> 0) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back('W');
            mSendPackge.sendDataList.push_back('G');

            uchar uctmp1 = (mAddrInfo.mAddrProp.nAddrValue/1000) & 0xf;
            uchar uctmp2 = ((mAddrInfo.mAddrProp.nAddrValue%1000)/100) & 0xf;
            uchar uctmp3 = ((mAddrInfo.mAddrProp.nAddrValue%100)/10) & 0xf;
            uchar uctmp4 = (mAddrInfo.mAddrProp.nAddrValue%10) & 0xf;

            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp1));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp2));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp3));
            mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(uctmp4));

            if (mAddrInfo.nValueList.size() > 0)
            {
                nTmpValue = mAddrInfo.nValueList.at(0);
                if (nTmpValue)//��λ
                {
                    mSendPackge.sendDataList.push_back(0x31);
                }
                else//��λ
                {
                    mSendPackge.sendDataList.push_back(0x30);
                }
            }

            ushort nCheckResult = PlcMathMethod::getXORcheck(mSendPackge.sendDataList);

            nTmpValue = PlcMathMethod::intToAsciiValue((nCheckResult >> 4) & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = PlcMathMethod::intToAsciiValue(nCheckResult & 0xf);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x2a);
            mSendPackge.sendDataList.push_back(0x0d);

            mSendPackge.nReturnLen = 11;
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

    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;
    vector<ADDR_TARGET_PROP > mTmpListWord5;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_T:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_IR_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_IR;

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

                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_HR_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_HR;

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

                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_LR_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_LR;

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
        case REG_TYPE_AR_BIT:
            {
                mTmpAddr.nRegIndex = REG_TYPE_AR;

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
        case REG_TYPE_IR:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_HR:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_LR:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_AR:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DM:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TC:
            {
                mTmpListWord5.push_back(mTmpAddr);
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
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, 1, 8);
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
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, 1, 8);
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

    mTmpListWord5 = PlcMathMethod::addrBubbleSort(mTmpListWord5);
    mTmpListWord5 = PlcMathMethod::addrSortOut(mTmpListWord5, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord5.at(i));
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
    int nMaxRWlen = 32;
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
    mSerialInfo.DATA_BIT = 7;
    mSerialInfo.PARITY_TYPE = 2;
    mSerialInfo.STOP_BIT = 2;

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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096 * 16)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TC:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
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

    return true;
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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096 * 16)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
    case REG_TYPE_TC:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_IR:
    case REG_TYPE_HR:
    case REG_TYPE_LR:
    case REG_TYPE_AR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4096)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
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
            eRegType = REG_TYPE_IR_BIT;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_HR_BIT;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_LR_BIT;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_AR_BIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_IR;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_HR;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_LR;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_AR;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_DM;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_TC;
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

    if(DealWithString::compare("IR_BIT", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("HR_BIT", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("LR_BIT", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("AR_BIT", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("IR", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("HR", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("LR", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("AR", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("DM", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("TC", sRegName)))
    {
        nRegIndex = 11;
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
