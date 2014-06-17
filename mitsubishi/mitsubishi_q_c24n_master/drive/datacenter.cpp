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
    /* ��ʼ������ */

    ADDR_TARGET_PROP mAddr = mAddrInfo.mAddrProp;
    vector<uchar > valueList = mAddrInfo.nValueList;

    /*���Ȳ����򷵻�*/
    if(mAddr.nAddrLen < 1) return false;

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

    /*����ֵ�ĳ���*/
    int nSetDataLen = 0;
    uchar nTmpValue = 0;

    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            int nSize = readDataList.regBitList.at(eRegType).size();
            if(bUserData)       //������û�д������
            {
                int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/8 +1;

                /*���ݴ洢�����������������·���ռ�*/
                if(nSize < nValueListSize)
                {
                    readDataList.regBitList[eRegType].resize(nValueListSize, emptyBitValue);
                }

                /*���ó���*/
                nSetDataLen = mAddr.nAddrLen;
                if(nSetDataLen > nListSize)
                {
                    nSetDataLen = nListSize;
                }

                /*��ֵ*/
                int nStartPos = 0;
                int nBitPost = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    nStartPos = (mAddr.nAddrValue + i)/8;
                    nBitPost = (mAddr.nAddrValue + i)%8;

                    /*����״̬*/
                    readDataList.regBitList[eRegType][nStartPos].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    nTmpValue = readDataList.regBitList[eRegType].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= (1 << nBitPost);
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regBitList[eRegType][nStartPos].nValue = nTmpValue;
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ó��ȣ���ͽ���2���ֽ�*/
                nSetDataLen = nListSize;

                /*��ַ���Ȱ���һ����(�����ֽڣ�������*/
                int nTmpWordLen = mAddr.nAddrLen/16;
                if(mAddr.nAddrLen%16 != 0)
                {
                    nTmpWordLen++;
                }

                if(nSetDataLen > nTmpWordLen*2)
                {
                    nSetDataLen = nTmpWordLen*2;
                }

                /*���ݴ洢�����������������·���ռ�*/
                int nStartAddr = mAddr.nAddrValue/8;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regBitList[eRegType].resize(nStartAddr + nSetDataLen, emptyBitValue);
                }

                /*��ֵ*/
                for(int i = 0; i < nSetDataLen; i++)
                {
                    nTmpValue = valueList.at(i);
                    readDataList.regBitList[eRegType][nStartAddr + i].eErrorStatus = eSuccess;
                    readDataList.regBitList[eRegType][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType - 11].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType - 11].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            nSetDataLen = mAddr.nAddrLen;
            if(nSetDataLen > nListSize/2)
            {
                nSetDataLen = nListSize/2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 11][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[eRegType - 11][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[eRegType - 11][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[eRegType - 11][mAddr.nAddrValue + i].nValue = nTmpValue;
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
    if(mAddr.nAddrLen < 1 || mAddr.nAddrLen > 65535) return false;

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
    case REG_TYPE_X:
    case REG_TYPE_Y:
    case REG_TYPE_M:
    case REG_TYPE_L:
    case REG_TYPE_F:
    case REG_TYPE_V:
    case REG_TYPE_S:
    case REG_TYPE_B:
    case REG_TYPE_SB:
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
            int nStartPos = 0;
            int nBitPost = 0;
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                nStartPos = (mAddr.nAddrValue + i)/8;
                nBitPost = (mAddr.nAddrValue + i)%8;

                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regBitList[eRegType].at(nStartPos).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regBitList[eRegType].at(nStartPos).nValue;
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
    case REG_TYPE_D:
    case REG_TYPE_W:
    case REG_TYPE_SW:
    case REG_TYPE_Z:
    case REG_TYPE_TV:
    case REG_TYPE_CV:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[eRegType -11].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[eRegType -11].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[eRegType -11].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[eRegType -11].at(mAddr.nAddrValue + i).nValue;

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
