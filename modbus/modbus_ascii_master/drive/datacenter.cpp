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
    /* ����Ĵ���û���ӣ����ʼ���Ĵ������ݴ洢 */
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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        vector<ADDR_DOUBLE_WORD_PROP > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
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

    ADDR_DOUBLE_WORD_PROP emptyDoubleWordValue;
    emptyDoubleWordValue.nValue = 0;
    emptyDoubleWordValue.eErrorStatus = READ_DATA_FAIL;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:
    case REG_TYPE_1X:
        {
            int nSize = readDataList.regBitList[eRegType].size();
            if(bUserData)//������û�д������
            {
                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
                }

                /*��ֵ*/
                for(int i = 0; i < mAddr.nAddrLen; i++)
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
            else//����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + nListSize *8)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize *8, emptyBitValue);
                }

                /*��ֵ*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < nListSize/2; i++)
                {
                    uchar nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(2*i + 0)) << 4) & 0xf0;
                    uchar nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(2*i + 1)) << 0) & 0x0f;
                    uchar nTmpValue = nTmpValue1 + nTmpValue2;

                    for(int j = 0; j < 8; j++)
                    {
                        readDataList.regBitList[eRegType][nStartAddr + i*8 + j].eErrorStatus = eSuccess;
                        if(nTmpValue & (1 << j))
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*8 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*8 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            int nSize = readDataList.regWordList[eRegType - 2].size();
            if(bUserData)//���û����õ�ֵ
            {
                int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < nValueListSize)
                {
                    readDataList.regWordList[eRegType - 2].resize(nValueListSize, emptyWordValue);
                }

                /*���Ȳ���,��0*/
                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
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
                    readDataList.regWordList[eRegType - 2][nStartPos].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    nTmpValue = readDataList.regWordList[eRegType - 2].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= 1 << nBitPost;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regWordList[eRegType - 2][nStartPos].nValue = nTmpValue;
                }
            }
            else//����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/4;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regWordList[eRegType - 2].resize(nStartAddr + nSetDataLen, emptyWordValue);
                }

                /*��ֵ*/
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 2][nStartAddr + i].eErrorStatus = eSuccess;

                    /*����ֵ 12 8 4 0*/
                    ushort tmp1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    ushort tmp2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    ushort tmp3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    ushort tmp4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;
                    nTmpValue = tmp1 + tmp2 + tmp3 + tmp4;

                    readDataList.regWordList[eRegType - 2][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_3X:
    case REG_TYPE_4X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 4].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 4].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                if(mAddr.nAddrLen*2 != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen*2, 0);
                }

                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 4][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 4][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else//����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nListSize/4; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 4][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ 12 8 4 0*/
                    ushort tmp1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    ushort tmp2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    ushort tmp3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    ushort tmp4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;
                    nTmpValue = tmp1 + tmp2 + tmp3 + tmp4;
                    readDataList.regWordList[eRegType - 4][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
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
                for(int i = 0; i < nListSize/4; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ 12 8 4 0*/
                    ushort tmp1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    ushort tmp2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    ushort tmp3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    ushort tmp4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;
                    nTmpValue = tmp1 + tmp2 + tmp3 + tmp4;
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
    /* ����Ĵ���û���ӣ����ʼ���Ĵ������ݴ洢 */
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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        vector<ADDR_DOUBLE_WORD_PROP > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    nResultList.clear();

    /*��������ʱ�������������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDoubleWordValue;
    emptyDoubleWordValue.nValue = 0;
    emptyDoubleWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*�����ݱ��浽��Ӧ��������*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:
    case REG_TYPE_1X:
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
    case REG_TYPE_3X_BIT:
    case REG_TYPE_4X_BIT:
        {
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 2].size();
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[eRegType - 2].resize(nValueListSize, emptyWordValue);
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
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 2].at(nStartPos).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 2].at(nStartPos).nValue;
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
    case REG_TYPE_3X:
    case REG_TYPE_4X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 4].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 4].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 4].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 4].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_3X_DOUBLE:
    case REG_TYPE_4X_DOUBLE:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen/2; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i*2 + 1).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                nTmpValue = readDataList.regWordList[eRegType - 6].at(mAddr.nAddrValue + i*2 + 0).nValue;

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