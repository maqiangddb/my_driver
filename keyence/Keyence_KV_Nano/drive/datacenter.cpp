#include "datacenter.h"
#include "plcprotocol.h"
#include "../tools/plcmathmethod.h"

DataCenter::DataCenter()
{
}

/******************************************************************
* Function: ����ֵ��plc������
* Parameters: address list
* bUserData Ϊtrue��ʾ��Ӧ�ó���ȡ�õ����ݣ���Ҫ�ߵ�λת����Ϊfalse����ʾ��Э���ȡ�������ݣ�
* Return: null
******************************************************************/
bool DataCenter::setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess)
{
    /* ����Ĵ���û��ӣ����ʼ���Ĵ������ݴ洢 */
    while(readDataList.regBitList.size() < MAX_BIT_REG_NUN)
    {
        /* REG_TYPE_RLY����readDataList.regBitList.at(0) */
        /* REG_TYPE_T����readDataList.regBitList.at(1) */
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        /* REG_TYPE_DM����readDataList.regWordList.at(0) */
        /* REG_TYPE_TM����readDataList.regWordList.at(1) */
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*д������Ϊ���򷵻�*/
    int nListSize = valueList.size();
    //if(nListSize > 200) return false;
    //    if(nListSize <= 0) return ;

    /*�����ݱ��浽��Ӧ��������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            if(bUserData)       //������û�д������
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*��ֵ*/
                for(int i = 0; i < nSetLen; i++)
                {
                    readDataList.regBitList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regBitList[eRegType].size();
                if(nSize < mAddr.nAddrValue + nListSize)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize, emptyBitValue);
                }

                /*��ֵ*/
                int nStartAddr = mAddr.nAddrValue;
                for(int i = 0; i < nListSize; i++)
                {
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    uchar nTmpValue = valueList.at(i);
                    nTmpValue = PlcMathMethod::asciiToIntValue(nTmpValue);
                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/4)
            {
                nSetLen = nListSize/4;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    unsigned short usTemp1 = valueList.at(i*4 + 0);
                    unsigned short usTemp2 = valueList.at(i*4 + 1);
                    unsigned short usTemp3 = valueList.at(i*4 + 2);
                    unsigned short usTemp4 = valueList.at(i*4 + 3);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    unsigned short usTemp1 = valueList.at(i*11 + 2);
                    unsigned short usTemp2 = valueList.at(i*11 + 3);
                    unsigned short usTemp3 = valueList.at(i*11 + 4);
                    unsigned short usTemp4 = valueList.at(i*11 + 5);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CTC:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    unsigned short usTemp1 = valueList.at(i*11 + 7);
                    unsigned short usTemp2 = valueList.at(i*11 + 8);
                    unsigned short usTemp3 = valueList.at(i*11 + 9);
                    unsigned short usTemp4 = valueList.at(i*11 + 10);

                    usTemp1 = (PlcMathMethod::asciiToIntValue(usTemp1) << 12) & 0Xf000;
                    usTemp2 = (PlcMathMethod::asciiToIntValue(usTemp2) << 8 ) & 0x0f00;
                    usTemp3 = (PlcMathMethod::asciiToIntValue(usTemp3) << 4 ) & 0x00f0;
                    usTemp4 = (PlcMathMethod::asciiToIntValue(usTemp4) << 0 ) & 0x000f;

                    unsigned short nTmpValue = usTemp1 + usTemp2 + usTemp3 + usTemp4;
                    readDataList.regWordList[eRegType - 8][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_C:
    case REG_TYPE_T:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            if(bUserData)       //������û�д������
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*��ֵ*/
                for(int i = 0; i < nSetLen; i++)
                {
                    readDataList.regBitList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[eRegType][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regBitList[eRegType].size();
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/11)
                {
                    nSetLen = nListSize/11;
                }

                if(nSize < mAddr.nAddrValue + nSetLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize, emptyBitValue);
                }

                int nStartAddr = mAddr.nAddrValue;
                for(int i = 0; i < nSetLen; i++)
                {
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    uchar nTmpValue = valueList.at(i*11 + 0);
                    nTmpValue = PlcMathMethod::asciiToIntValue(nTmpValue);

                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    default:
        {
            break;
        }
    }//end switch

    return true;
}

/******************************************************************
* Function: ��ð���վ�ŵ�STATION_VALUE_PROP��stationList�����е��±�
* Parameters: stationList��nStationId
* Return: null
******************************************************************/
bool DataCenter::getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList)
{
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

    nResultList.clear();
    //if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return nResultList;

    /*��������ʱ������������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*�����ݱ��浽��Ӧ��������*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[eRegType].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
    case REG_TYPE_CTH:
    case REG_TYPE_CTC:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 8].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 8].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 8].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 8].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

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

#if 0
/******************************************************************
* Function: ��ð���վ�ŵ�STATION_VALUE_PROP��stationList�����е��±�
* Parameters: stationList��nStationId
* Return: null
******************************************************************/
int DataCenter::getVectorIndex(vector<STATION_VALUE_PROP> stationList, int nStationId)
{
    int index = -1;
    if(stationList.empty()) return index;

    int nSize = stationList.size();
    STATION_VALUE_PROP mTmpValue;
    for(int i = 0; i < nSize; i++)
    {
        mTmpValue = stationList.at(i);
        if(mTmpValue.nStationId == nStationId)
        {
            index = i;
            return index;
        }
    }

    return index;
}
#endif
