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

    ADDR_DOUBLE_WORD_PROP emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_T:
    case REG_TYPE_C:
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
                if(nSize < mAddr.nAddrValue + nListSize *8)
                {
                    readDataList.regBitList[eRegType].resize(mAddr.nAddrValue + nListSize *8, emptyBitValue);
                }

                /*��ֵ*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < nListSize; i++)
                {
                    uchar nTmpValue = valueList.at(i);
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
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType -5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
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
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_CV:
        {
            /*���ݴ洢�����������������·���ռ�*/
            if(mAddr.nAddrValue < 200)
            {
                int nSize = readDataList.regWordList[eRegType -5].size();
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
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
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                        /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                        uchar nLowByte = valueList.at(2*i);
                        uchar nHighByte = valueList.at(2*i +1);
                        nTmpValue = nHighByte & 0xff;
                        nTmpValue = (nTmpValue << 8);
                        nTmpValue += nLowByte;
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                    }
                }
                else      //����Ǵ�PLC��ȡ������
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*����״̬*/
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                        /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                        uchar nHighByte = valueList.at(2*i);
                        uchar nLowByte = valueList.at(2*i +1);
                        nTmpValue = nHighByte & 0xff;
                        nTmpValue = nTmpValue << 8;
                        nTmpValue = nTmpValue + (nLowByte & 0xff);
                        readDataList.regWordList[eRegType -5][mAddr.nAddrValue + i].nValue = nTmpValue;
                    }
                }
            }
            else
            {
                int nSize = readDataList.regDoubleWordList[0].size();
                int nAddrValue = mAddr.nAddrValue - 200;
                if(nSize < nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regDoubleWordList[0].resize(nAddrValue + mAddr.nAddrLen, emptyDWordValue);
                }

                nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                /*��ֵ*/
                uint nTmpValue = 0;
                if(bUserData)  //���û����õ�ֵ
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*����״̬*/
                        readDataList.regDoubleWordList[0][nAddrValue + i].eErrorStatus = eSuccess;

                        /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                        uint nLowByte = valueList.at(4*i );
                        uint nHighByte = valueList.at(4*i +1);
                        uint nLowByte1 = valueList.at(4*i +2);
                        uint nHighByte1 = valueList.at(4*i +3);
                        nTmpValue = (nHighByte1 << 24) + (nLowByte1 << 16) + (nHighByte << 8) + nLowByte;
                        readDataList.regDoubleWordList[0][nAddrValue + i].nValue = nTmpValue;
                    }
                }
                else      //����Ǵ�PLC��ȡ������
                {
                    for(int i = 0; i < nSetLen; i++)
                    {
                        /*����״̬*/
                        readDataList.regDoubleWordList[0][nAddrValue + i].eErrorStatus = eSuccess;

                        /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                        uint nLowByte = valueList.at(4*i +3);
                        uint nHighByte = valueList.at(4*i +2);
                        uint nLowByte1 = valueList.at(4*i +1);
                        uint nHighByte1 = valueList.at(4*i +0);
                        nTmpValue = (nHighByte1 << 8) + (nLowByte1 << 0) + (nHighByte << 24) + (nLowByte << 16);
                        readDataList.regDoubleWordList[0][nAddrValue + i].nValue = nTmpValue;
                    }
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

    while(readDataList.regDoubleWordList.size() < MAX_DWORD_REG_NUN)
    {
        vector<ADDR_DOUBLE_WORD_PROP > mTmpValueList;
        readDataList.regDoubleWordList.push_back(mTmpValueList);
    }
    /* ��ʼ������ */

    nResultList.clear();
    if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return false;

    /*��������ʱ������������*/
    ADDR_BIT_VALUE_PROP emptyBitValue;
    emptyBitValue.nValue = 0;
    emptyBitValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_WORD_VALUE_PROP emptyWordValue;
    emptyWordValue.nValue = 0;
    emptyWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_DOUBLE_WORD_PROP emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*�����ݱ��浽��Ӧ��������*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
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
    case REG_TYPE_D:
    case REG_TYPE_TV:
    case REG_TYPE_AI:
    case REG_TYPE_AO:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType -5].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_CV:
        {
            if(mAddr.nAddrValue < 200)
            {
                /*���ݴ洢�����������������·���ռ�*/
                int nSize = readDataList.regWordList[eRegType -5].size();
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regWordList[eRegType -5].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
                }

                /*��ֵ*/
                ushort nTmpValue = 0;
                for(int i = 0; i <  mAddr.nAddrLen; i++)
                {
                    /*����״̬*/
                    mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).eErrorStatus;

                    /*����ֵ*/
                    nTmpValue = readDataList.regWordList[eRegType -5].at(mAddr.nAddrValue + i).nValue;

                    mTmpValueProp.nValue = nTmpValue & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                    nResultList.push_back(mTmpValueProp);
                }
            }
            else
            {
                /*���ݴ洢�����������������·���ռ�*/
                int nSize = readDataList.regDoubleWordList[0].size();
                int nAddrValue = mAddr.nAddrValue - 200;
                if(nSize < nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regDoubleWordList[0].resize(nAddrValue + mAddr.nAddrLen, emptyDWordValue);
                }

                /*��ֵ*/
                uint nTmpValue = 0;
                for(int i = 0; i <  mAddr.nAddrLen; i++)
                {
                    /*����״̬*/
                    mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[0].at(nAddrValue + i).eErrorStatus;

                    /*����ֵ*/
                    nTmpValue = readDataList.regDoubleWordList[0].at(nAddrValue + i).nValue;

                    mTmpValueProp.nValue = nTmpValue & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 16) & 0xff;
                    nResultList.push_back(mTmpValueProp);

                    mTmpValueProp.nValue = (nTmpValue >> 24) & 0xff;
                    nResultList.push_back(mTmpValueProp);
                }
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
