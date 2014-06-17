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
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*д������Ϊ���򷵻�*/
    int nListSize = valueList.size();
    if(nListSize > 200) return false;

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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            int nSize = readDataList.regWordList[eRegType].size();

            if(bUserData)  //���û����õ�ֵ
            {
                int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 + 1;

                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < nValueListSize)
                {
                    readDataList.regWordList[eRegType].resize(nValueListSize, emptyWordValue);
                }

                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize)
                {
                    nSetLen = nListSize;
                }

                /*��ֵ*/
                int nStartPos = 0;
                int nBitPost = 0;
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetLen; i++)
                {
                    nStartPos = (mAddr.nAddrValue + i)/16;
                    nBitPost = (mAddr.nAddrValue + i)%16;

                    /*����״̬*/
                    readDataList.regWordList[eRegType][nStartPos].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    nTmpValue = readDataList.regWordList[eRegType].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= 1 << nBitPost;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regWordList[eRegType][nStartPos].nValue = nTmpValue;
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regWordList[eRegType].size();
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/2;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regWordList[eRegType].resize(nStartAddr + nSetDataLen, emptyWordValue);
                }

                /*��ֵ*/
                ushort nTmpValue = 0;
                for(int i = 0; i < nListSize/4; i++)
                {
                    /*����״̬*/
                    ushort nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    ushort nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    ushort nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    ushort nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;

                    nTmpValue = nTmpValue1 + nTmpValue2 + nTmpValue3 + nTmpValue4;

                    readDataList.regWordList[eRegType][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            int nSize = readDataList.regBitList[eRegType - 4].size();
            if(bUserData)//������û�д������
            {
                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType - 4].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
                }

                /*��ֵ*/
                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    readDataList.regBitList[eRegType - 4][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[eRegType - 4][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[eRegType - 4][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + nListSize*4)//4�������ֽ�һ����
                {
                    readDataList.regBitList[eRegType - 4].resize(mAddr.nAddrValue + nListSize*4, emptyBitValue);
                }

                valueList.resize(2);
                uchar uctmp = PlcMathMethod::asciiToIntValue(valueList.at(0));

                if(uctmp)
                {
                    readDataList.regBitList[eRegType - 4][mAddr.nAddrValue].nValue = 1;
                }
                else
                {
                    readDataList.regBitList[eRegType - 4][mAddr.nAddrValue].nValue = 0;
                }
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
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
                }

                if(mAddr.nAddrLen*2 != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen*2, 0);
                }

                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else//����Ǵ�PLC��ȡ������
            {
                if(nSize < mAddr.nAddrValue + nListSize/4)
                {
                    readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + nListSize/4, emptyWordValue);
                }

                for(int i = 0; i < nListSize/4; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    uint nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    uint nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    uint nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    uint nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;

                    uint nTmpValue = nTmpValue1 + nTmpValue2 + nTmpValue3 + nTmpValue4;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;
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
        vector<ADDR_BIT_VALUE_PROP > mTmpValueList;
        readDataList.regBitList.push_back(mTmpValueList);
    }

    while(readDataList.regWordList.size() < MAX_WORD_REG_NUN)
    {
        vector<ADDR_WORD_VALUE_PROP > mTmpValueList;
        readDataList.regWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    nResultList.clear();

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
    case REG_TYPE_IR_BIT:
    case REG_TYPE_HR_BIT:
    case REG_TYPE_LR_BIT:
    case REG_TYPE_AR_BIT:
        {
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType].size();
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[eRegType].resize(nValueListSize, emptyWordValue);
            }

            /*��ֵ*/
            int nStartPos = 0;
            int nBitPost = 0;
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                nStartPos = (mAddr.nAddrValue + i)/16;
                nBitPost = (mAddr.nAddrValue + i)%16;

                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType].at(nStartPos).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType].at(nStartPos).nValue;
                if(nTmpValue & (1 << nBitPost))
                {
                    mTmpValueProp.nValue = 1;
                }
                else
                {
                    mTmpValueProp.nValue = 0;
                }
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[eRegType - 4].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType - 4].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[eRegType - 4].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
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
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i).nValue;

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
