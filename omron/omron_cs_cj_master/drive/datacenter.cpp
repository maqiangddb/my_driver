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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
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
            int nSize = readDataList.regBitList[eRegType - 5].size();
            if(bUserData)//������û�д������
            {
                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[eRegType - 5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

                if(mAddr.nAddrLen != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen, 0);
                }

                /*��ֵ*/
                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    readDataList.regBitList[eRegType - 5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[eRegType - 5][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[eRegType - 5][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*��ֵ*/
                int nSetLen = mAddr.nAddrLen/8;
                if(mAddr.nAddrLen%8 != 0)
                {
                    nSetLen += 1;
                }

                if(nSetLen > nListSize/16)
                {
                    nSetLen = nListSize/16;
                }

                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < mAddr.nAddrValue + nSetLen*8)
                {
                    readDataList.regBitList[eRegType - 5].resize(mAddr.nAddrValue + nSetLen*8, emptyBitValue);
                }

                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < nSetLen; i++)
                {
                    ushort nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 1))) & 0x0f;
                    ushort nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 3))) & 0x0f;
                    ushort nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 5))) & 0x0f;
                    ushort nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 7))) & 0x0f;
                    ushort nTmpValue5 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 9))) & 0x0f;
                    ushort nTmpValue6 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 11))) & 0x0f;
                    ushort nTmpValue7 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 13))) & 0x0f;
                    ushort nTmpValue8 = (PlcMathMethod::asciiToIntValue(valueList.at(16*i + 15))) & 0x0f;

                    ushort nTmpValue = (nTmpValue1 << 0) + (nTmpValue2 << 1) + (nTmpValue3 << 2) + (nTmpValue4 << 3)
                                       + (nTmpValue5 << 4) + (nTmpValue6 << 5) + (nTmpValue7 << 6) + (nTmpValue8 << 7);

                    for(int j = 0; j < 8; j++)
                    {
                        readDataList.regBitList[eRegType - 5][nStartAddr + i*8 + j].eErrorStatus = eSuccess;
                        if((nTmpValue & (1 << j)) > 0)
                        {
                            readDataList.regBitList[eRegType - 5][nStartAddr + i*8 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType - 5][nStartAddr + i*8 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 7].size();

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType - 7].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
                }

                if(mAddr.nAddrLen*2 != nListSize)
                {
                    valueList.resize(mAddr.nAddrLen*2, 0);
                }

                for(int i = 0; i < mAddr.nAddrLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 7][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 7][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else//����Ǵ�PLC��ȡ������
            {
                if(nSize < mAddr.nAddrValue + nListSize/4)
                {
                    readDataList.regWordList[eRegType - 7].resize(mAddr.nAddrValue + nListSize/4, emptyWordValue);
                }

                for(int i = 0; i < nListSize/4; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 7][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    uint nTmpValue1 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 0)) << 12) & 0xf000;
                    uint nTmpValue2 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 1)) << 8) & 0x0f00;
                    uint nTmpValue3 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 2)) << 4) & 0x00f0;
                    uint nTmpValue4 = (PlcMathMethod::asciiToIntValue(valueList.at(4*i + 3)) << 0) & 0x000f;

                    uint nTmpValue = nTmpValue1 + nTmpValue2 + nTmpValue3 + nTmpValue4;
                    readDataList.regWordList[eRegType - 7][mAddr.nAddrValue + i].nValue = nTmpValue;
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
    case REG_TYPE_CIO_BIT:
    case REG_TYPE_W_BIT:
    case REG_TYPE_H_BIT:
    case REG_TYPE_A_BIT:
    case REG_TYPE_D_BIT:
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
            int nSize = readDataList.regBitList[eRegType - 5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[eRegType - 5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[eRegType - 5].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_CIO:
    case REG_TYPE_W:
    case REG_TYPE_H:
    case REG_TYPE_A:
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 7].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 7].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 7].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 7].at(mAddr.nAddrValue + i).nValue;

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
