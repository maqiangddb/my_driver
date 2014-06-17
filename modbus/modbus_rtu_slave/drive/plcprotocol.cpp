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
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_65536 * 16)
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
            sRegNameList.push_back("LB");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
            break;
        }
    case DWORT_ADDR_REG:
        {
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
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
            sRegNameList.push_back("LB");
            sRegNameList.push_back("LW");
            sRegNameList.push_back("RWI");
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

                    if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536 * 16)
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

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_65536)
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
        eCmnInfo = UNRCV_ALL_DATA;
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
 * Function: ���"��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������" getSlaveDataStr �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::getSlaveDataStr(vector<unsigned char> rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg)
{
    /*���������ֵ*/
    mSendPkg.sendDataList.clear();

    UNPACK_NEED_PROP mCheckProp;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;
    if(rcvList.empty())
    {
        eCmnInfo = NO_RCV_DATA;
        return eCmnInfo;
    }

    /*���վ���Ƿ���ȷ��վ��Ҳ��PLC�ĵ�ֵַ*/
    int nRcvSize = rcvList.size();

    /*���Ȳ����򷵻�*/
    if(nRcvSize < 8)
    {
        eCmnInfo = checkRcvData(mCheckProp, rcvList, CHECK_READ);
        if(eCmnInfo == CMN_NOMAL_CODE)
        {
            eCmnInfo = CHECK_OK_UNRCV_ALL;
        }
        else
        {
            eCmnInfo = UNRCV_ALL_DATA;
        }
        return eCmnInfo;
    }

    /*վ�Ų�ƥ�䣬����*/
    uchar nList0 = rcvList.at(0);
    if(nList0 != 0 && nList0 != nStationId)
    {
        eCmnInfo = STATION_ERROR;
        return eCmnInfo;
    }

    /* ����Ĵ���û��ӣ����ʼ���Ĵ������ݴ洢 */
    while(readDataList.regBitList.size() < MAX_BIT_REG_NUN)
    {
        /* 0x����readDataList.regBitList.at(0) */
        /* 1x����readDataList.regBitList.at(1) */
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        /* 3x����readDataList.regWordList.at(0) */
        /* 4x����readDataList.regWordList.at(1) */
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    ushort nStartAddr = 0;
    ushort nAddrLen = 0;
    int nRegSize = 0;
    uchar nByteLen = 0x01;

    /*������*/
    uchar nFumNum = rcvList.at(1);

    /*վ�� vector index = 0*/
    mSendPkg.sendDataList.push_back(nList0);

    /*�Թ����븳ֵ vector index = 1*/
    mSendPkg.sendDataList.push_back(nFumNum);

    /*�����յ����ݶ���*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    /*У����󣬷���*/
    bool bDataCorrect = true;
    eCmnInfo = checkRcvData(mCheckProp, rcvList, CHECK_READ);
    if(eCmnInfo != CMN_NOMAL_CODE)
    {
        bDataCorrect = false;
        mSendPkg.sendDataList.push_back(eCmnInfo);
    }

    /*�Ƿ���Ҫ���У����*/
    bool bAddCheckCode = true;
    if(bDataCorrect)
    {
        switch(nFumNum)
        {
        case 0x01:          //��REG_TYPE_0X����
            {
                if(nRcvSize < 8)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*��ȡ����*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*ȡ����*/
                nRegSize = readDataList.regBitList[0].size();
                if(nRegSize < nStartAddr + nAddrLen + 8)
                {
                    readDataList.regBitList[0].resize(nStartAddr + nAddrLen + 8, emptyBitValue);
                }

                /*�Զ�ȡ�ֽ�����ʼ��1,��ʱ��Ҫ�޸�*/
                mSendPkg.sendDataList.push_back(nByteLen);

                int pos = 0;
                uchar nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    if(pos >= 8)
                    {
                        pos = 0;
                        mSendPkg.sendDataList.push_back(nTmpValue);
                        nTmpValue = 0;
                    }

                    if(1 == readDataList.regBitList[0].at(nStartAddr + i).nValue)
                    {
                        nTmpValue |= 1 << pos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << pos);
                    }

                    pos++;
                }

                /*��ʣ�µ�λ����ȫ*/
                int nIndex = 0;
                while(pos < 8)
                {
                    if(1 == readDataList.regBitList[0].at(nStartAddr + nAddrLen + nIndex).nValue)
                    {
                        nTmpValue |= 1 << pos;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << pos);
                    }

                    pos++;
                    nIndex++;
                }
                mSendPkg.sendDataList.push_back(nTmpValue);

                /*�Զ�ȡ�ֽ����޸�*/
                nByteLen = mSendPkg.sendDataList.size() - 3;
                mSendPkg.sendDataList[2] = nByteLen;
                break;
            }
        case 0x02:          //��REG_TYPE_1X����
            {
                /*��֧��02������*/
                mSendPkg.sendDataList.push_back(FUN_ERROR_CODE);
                eCmnInfo = FUN_ERROR_CODE;
                break;
            }
        case 0x03:          //��REG_TYPE_4X����
            {
                if(nRcvSize < 8)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*��ȡ����*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*ȡ����*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                /*�Զ�ȡ�ֽ�����ʼ��1,��ʱ��Ҫ�޸�*/
                mSendPkg.sendDataList.push_back(nByteLen);

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = readDataList.regWordList[0].at(nStartAddr + i).nValue;

                    /*����ֵ*/
                    uchar nHighByte = (nTmpValue >> 8) & 0xff;
                    uchar nLowByte = nTmpValue & 0xff;

                    /*����Ӹ��ֽڣ�����ӵ��ֽ�*/
                    mSendPkg.sendDataList.push_back(nHighByte);
                    mSendPkg.sendDataList.push_back(nLowByte);
                }

                /*�Զ�ȡ�ֽ����޸�*/
                nByteLen = mSendPkg.sendDataList.size() - 3;
                mSendPkg.sendDataList[2] = nByteLen;
                break;
            }
        case 0x04:          //��REG_TYPE_3X����
            {
                if(nRcvSize < 8)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*��ȡ����*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*ȡ����*/
                nRegSize = readDataList.regWordList[1].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[1].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                /*�Զ�ȡ�ֽ�����ʼ��1,��ʱ��Ҫ�޸�*/
                mSendPkg.sendDataList.push_back(nByteLen);

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = readDataList.regWordList[1].at(nStartAddr + i).nValue;

                    /*����ֵ*/
                    uchar nHighByte = (nTmpValue >> 8) & 0xff;
                    uchar nLowByte = nTmpValue & 0xff;

                    /*����Ӹ��ֽڣ�����ӵ��ֽ�*/
                    mSendPkg.sendDataList.push_back(nHighByte);
                    mSendPkg.sendDataList.push_back(nLowByte);
                }

                /*�Զ�ȡ�ֽ����޸�*/
                nByteLen = mSendPkg.sendDataList.size() - 3;
                mSendPkg.sendDataList[2] = nByteLen;
                break;
            }
        case 0x05:          //дREG_TYPE_0X����, д��������
            {
                if(nRcvSize != 8)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = DATA_LEN_ERROR;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + 1 >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*�����������������*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*ȡ����*/
                nRegSize = readDataList.regBitList[0].size();
                if(nRegSize < nStartAddr + 1)
                {
                    readDataList.regBitList[0].resize(nStartAddr + 1, emptyBitValue);
                }

                if(0xff00 == nAddrLen)
                {
                    readDataList.regBitList[0][nStartAddr].nValue = 1;
                }
                else if(0x0000 == nAddrLen)
                {
                    readDataList.regBitList[0][nStartAddr].nValue = 0;
                }
                else   //���󣬷��ش�����
                {
                    mSendPkg.sendDataList.push_back(DATA_VALUE_ERROR);
                    eCmnInfo = ADDR_LEN_ERROR;
                    break;
                }

                mSendPkg.sendDataList.clear();
                mSendPkg.sendDataList = rcvList;

                /*����д������Ҫ���У��*/
                bAddCheckCode = false;
                break;
            }
        case 0x0f:          //дREG_TYPE_0X����, д��������
            {
                if(nRcvSize < 10)   //���󣬷��ش�����
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*���ó���*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*�ֽڸ���*/
                nByteLen = rcvList.at(6);

                /*����д����ȫ*/
                nByteLen = rcvList.size() - 9;

                /*д����*/
                nRegSize = readDataList.regBitList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regBitList[0].resize(nStartAddr + nAddrLen, emptyBitValue);
                }

                if(nByteLen*8 < nAddrLen)
                {
                    nAddrLen = nByteLen*8;
                }

                uchar nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    nTmpValue = rcvList.at(7 + i/8);

                    if(nTmpValue & (1 << i%8))
                    {
                        readDataList.regBitList[0][nStartAddr + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[0][nStartAddr + i].nValue = 0;
                    }
                }

                /*��ӷ�������*/
                mSendPkg.sendDataList.clear();
                for(int k = 0; k < 6; k++)
                {
                    mSendPkg.sendDataList.push_back(rcvList.at(k));
                }
                break;
            }
        case 0x06:          //дREG_TYPE_4X���ݣ� д��������
            {
                if(nRcvSize != 8)
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = DATA_LEN_ERROR;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + 1 >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*�����������������*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*д����*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + 1)
                {
                    readDataList.regWordList[0].resize(nStartAddr + 1, emptyWordValue);
                }

                readDataList.regWordList[0][nStartAddr].nValue = nAddrLen;

                mSendPkg.sendDataList.clear();
                mSendPkg.sendDataList = rcvList;

                /*����д������Ҫ���У��*/
                bAddCheckCode = false;
                break;
            }
        case 0x10:          //дREG_TYPE_4X����, д��������
            {
                if(nRcvSize < 11)   //���󣬷��ش�����
                {
                    mSendPkg.sendDataList.push_back(DATA_LEN_ERROR);
                    eCmnInfo = UNRCV_ALL_DATA;
                    break;
                }

                /*��ʼ��ַ vector index = 2*/
                nStartAddr = rcvList.at(2);
                nStartAddr = (nStartAddr << 8) & 0xff00;
                nStartAddr += rcvList.at(3);

                /*���ó���*/
                /*vector index = 3*/
                nAddrLen = rcvList.at(4);
                nAddrLen = (nAddrLen << 8) & 0xff00;
                nAddrLen += rcvList.at(5);

                /*����ַ�ķ�Χ�Ƿ���ȷ*/
                if(nStartAddr < 0 || nStartAddr + nAddrLen >= ADDR_MAX_VALUE_65536)
                {
                    mSendPkg.sendDataList.push_back(ADDR_OUT_RANG);
                    eCmnInfo = ADDR_OUT_RANG;
                    break;
                }

                /*�ֽڸ���*/
                nByteLen = rcvList.at(6);

                /*����д����ȫ*/
                nByteLen = rcvList.size() - 9;

                /*ȡ����*/
                nRegSize = readDataList.regWordList[0].size();
                if(nRegSize < nStartAddr + nAddrLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nAddrLen, emptyWordValue);
                }

                if(nByteLen < nAddrLen *2)
                {
                    nAddrLen = nByteLen/2;
                }

                ushort nTmpValue = 0;
                for(int i = 0; i < nAddrLen; i++)
                {
                    /*����ֵ*/
                    uchar nHighByte = rcvList.at(7 + 2*i);
                    uchar nLowByte = rcvList.at(7 + 2*i + 1);

                    nTmpValue = nHighByte;
                    nTmpValue = (nTmpValue << 8) & 0xff00;
                    nTmpValue += nLowByte;

                    readDataList.regWordList[0][nStartAddr + i].nValue = nTmpValue;
                }

                /*��ӷ�������*/
                mSendPkg.sendDataList.clear();
                for(int k = 0; k < 6; k++)
                {
                    mSendPkg.sendDataList.push_back(rcvList.at(k));
                }
                break;
            }
        default:
            {
                mSendPkg.sendDataList.push_back(FUN_ERROR_CODE);
                eCmnInfo = FUN_ERROR_CODE;
                break;
            }
        }
    }

    /*��������򷵻ش����룬��У��*/
    if(bAddCheckCode)
    {
        /*����У����*/
        ushort nCrcResult = PlcMathMethod::getCRCcheck(mSendPkg.sendDataList);
        uchar nCrcResultL = nCrcResult & 0xff;
        uchar nCrcResultH = (nCrcResult >> 8) & 0xff;

        /*vector index = 6*/
        mSendPkg.sendDataList.push_back(nCrcResultL);
        /*vector index = 7*/
        mSendPkg.sendDataList.push_back(nCrcResultH);
    }

    if(nList0 == 0)
    {
        mSendPkg.sendDataList.clear();
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: merge global address to m_globalReadPlcAddrList
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    targetList.clear();
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
 * Function: ���õ�ǰ��ַ
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

    /*����У��ṹ��*/

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
    return PLC_SLAVE;
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

    if(DealWithString::compare("LB", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("RWI", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("LW", sRegName)))
    {
        nRegIndex = 5;
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
    return NET_SERIAL_PORT;
}

