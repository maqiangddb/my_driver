#include "datacenter.h"
#include "plcprotocol.h"

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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        /* 3x����readDataList.regWordList.at(0) */
        /* 4x����readDataList.regWordList.at(1) */
        vector<addrDoubleWordProp > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;


    /*���Ȳ����򷵻�*/
    if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return false;

    /*д������Ϊ���򷵻�*/
    int nListSize = valueList.size();
    if(nListSize > 200) return false;
    //    if(nListSize <= 0) return ;

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

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_O:
    case REG_TYPE_B:
    case REG_TYPE_SBIT:
    case REG_TYPE_NBIT:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList.at(eRegType).size();
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
                nSize = readDataList.regBitList.at(eRegType).size();
                if(nSize < mAddr.nAddrValue + nListSize *16)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize *16, emptyBitValue);
                }

                /*��ֵ*/
                int nTmpValue = 0;
                int iTemp1,iTemp2;
                int nStartAddr = (mAddr.nAddrValue/16)*16;
                for(int i = 0; i < nListSize/2; i++)
                {
                    iTemp1 = valueList.at(i*2 + 0);
                    iTemp2 = valueList.at(i*2 + 1);
                    iTemp1 = (iTemp1 << 0) & 0x00ff;
                    iTemp2 = (iTemp2 << 8) & 0xff00;
                    nTmpValue = iTemp1 + iTemp2;
                    for(int j = 0; j < 16; j++)
                    {
                        readDataList.regBitList[eRegType][nStartAddr + i*16 + j].eErrorStatus = eSuccess;
                        if(nTmpValue & (1 << j))
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 1;
                        }
                        else
                        {
                            readDataList.regBitList[eRegType][nStartAddr + i*16 + j].nValue = 0;
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_S:
    case REG_TYPE_N:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/2)
            {
                nSetLen = nListSize/2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i + 0);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 5][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i + 1);
                    uchar nLowByte = valueList.at(2*i +0);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType - 5][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            break;
        }
    case REG_TYPE_F:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/4)
            {
                nSetLen = nListSize/4;
            }

            /*��ֵ*/
            int nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(4*i + 0);
                    int iTemp2 = valueList.at(4*i +1);
                    int iTemp3 = valueList.at(4*i + 2);
                    int iTemp4 = valueList.at(4*i +3);
                    iTemp1 = (iTemp1 << 0) & 0x000000ff;
                    iTemp2 = (iTemp2 << 8) & 0x0000ff00;
                    iTemp3 = (iTemp3 << 16) & 0x00ff0000;
                    iTemp4 = (iTemp4 << 24) & 0xff000000;

                    nTmpValue = iTemp1 + iTemp2 + iTemp3 + iTemp4;
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(4*i + 0);
                    int iTemp2 = valueList.at(4*i +1);
                    int iTemp3 = valueList.at(4*i + 2);
                    int iTemp4 = valueList.at(4*i +3);
                    iTemp1 = (iTemp1 << 0) & 0x000000ff;
                    iTemp2 = (iTemp2 << 8) & 0x0000ff00;
                    iTemp3 = (iTemp3 << 16) & 0x00ff0000;
                    iTemp4 = (iTemp4 << 24) & 0xff000000;

                    nTmpValue = iTemp1 + iTemp2 + iTemp3 + iTemp4;

                    readDataList.regDoubleWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                    float fTemp;
                    memcpy((void *)&fTemp,(void *)&nTmpValue,4);
                }
            }
            break;
        }
    case REG_TYPE_TACC:
    case REG_TYPE_CACC:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/6)
            {
                nSetLen = nListSize/6;
            }

            /*��ֵ*/
            int nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(6*i + 4);
                    int iTemp2 = valueList.at(6*i + 5);
                    iTemp1 = (iTemp1 << 0) & 0x00ff;
                    iTemp2 = (iTemp2 << 8) & 0xff00;

                    nTmpValue = iTemp1 + iTemp2;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(6*i + 4);
                    int iTemp2 = valueList.at(6*i + 5);
                    iTemp1 = (iTemp1 << 0) & 0x00ff;
                    iTemp2 = (iTemp2 << 8) & 0xff00;

                    nTmpValue = iTemp1 + iTemp2;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            break;
        }
    case REG_TYPE_TPRE:
    case REG_TYPE_CPRE:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 6].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 6].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/6)
            {
                nSetLen = nListSize/6;
            }

            /*��ֵ*/
            int nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(6*i + 2);
                    int iTemp2 = valueList.at(6*i + 3);
                    iTemp1 = (iTemp1 << 0) & 0x00ff;
                    iTemp2 = (iTemp2 << 8) & 0xff00;

                    nTmpValue = iTemp1 + iTemp2;
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].nValue = nTmpValue;

                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 6][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    int iTemp1 = valueList.at(6*i + 2);
                    int iTemp2 = valueList.at(6*i + 3);
                    iTemp1 = (iTemp1 << 0) & 0x00ff;
                    iTemp2 = (iTemp2 << 8) & 0xff00;

                    nTmpValue = iTemp1 + iTemp2;
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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        /* 3x����readDataList.regWordList.at(0) */
        /* 4x����readDataList.regWordList.at(1) */
        vector<addrDoubleWordProp > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    //vector<ADDR_BIT_VALUE_PROP > resultList ;
    nResultList.clear();
    //if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return resultList;

    /*��������ʱ������������*/
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
    case REG_TYPE_I:
    case REG_TYPE_O:
    case REG_TYPE_B:
    case REG_TYPE_SBIT:
    case REG_TYPE_NBIT:
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
    case REG_TYPE_S:
    case REG_TYPE_N:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType - 5].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType - 5].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_F:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regDoubleWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDoubleWordValue);
            }

            /*��ֵ*/
            uint nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[0].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regDoubleWordList[0].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 16) & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 24) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
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
