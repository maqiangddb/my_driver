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
    case REG_TYPE_3P:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/2)
                {
                    nSetLen = nListSize/2;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                nSetLen = mAddr.nAddrLen;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    ushort temp1 = PlcMathMethod::asciiToIntValue(valueList.at(4*i));
                    ushort temp2 = PlcMathMethod::asciiToIntValue(valueList.at(4*i +1));
                    ushort temp3 = PlcMathMethod::asciiToIntValue(valueList.at(4*i +2));
                    ushort temp4 = PlcMathMethod::asciiToIntValue((valueList.at(4*i +3)));
                    nTmpValue = (temp1 << 12) + (temp2 << 8) + (temp3 << 4) + (temp4 << 0);

                    //LOGE("setReadData ...nTmpValue = %d.",nTmpValue);
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_2P:
    case REG_TYPE_7P:
    case REG_TYPE_4P:
    case REG_TYPE_6P:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regDoubleWordList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDWordValue);
            }

            /*��ֵ*/
            uint nTmpValue = 0;
            if(bUserData)  //���û����õ�ֵ
            {
                int nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/4)
                {
                    nSetLen = nListSize/4;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uint nLowByte = valueList.at(4*i) & 0xff;
                    uint nHighByte = valueList.at(4*i + 1) & 0xff;
                    uint nLowByte1 = valueList.at(4*i + 2) & 0xff;
                    uint nHighByte1 = valueList.at(4*i + 3) & 0xff;

                    nTmpValue = (nHighByte1 << 24) + (nLowByte1 << 16) + (nHighByte << 8) + nLowByte;
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                int nSetLen = mAddr.nAddrLen/2;
                if(nSetLen > nListSize/8)
                {
                    nSetLen = nListSize/8;
                }

                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ 12 8 4 0*/
                    uint nChar1 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 0)) << 12) & 0xf000;
                    uint nChar2 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 1)) << 8) & 0x0f00;
                    uint nChar3 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 2)) << 4) & 0x00f0;
                    uint nChar4 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 3)) << 0) & 0x000f;

                    uint nChar5 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 4)) << 12) & 0xf000;
                    uint nChar6 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 5)) << 8) & 0x0f00;
                    uint nChar7 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 6)) << 4) & 0x00f0;
                    uint nChar8 = (PlcMathMethod::asciiToIntValue(valueList.at(8*i + 7)) << 0) & 0x000f;

                    uint nHighValue = nChar1 + nChar2 + nChar3 + nChar4;
                    uint nLowValue = nChar5 + nChar6 + nChar7 + nChar8;
                    nTmpValue = (nHighValue << 16) + nLowValue;
//                    float nValue = nTmpValue/100.00;
//                    memcpy(&nTmpValue,&nValue,4);
                    readDataList.regDoubleWordList[eRegType][mAddr.nAddrValue + i].nValue = nTmpValue;
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

    addrDoubleWordProp emptyDWordValue;
    emptyDWordValue.nValue = 0;
    emptyDWordValue.eErrorStatus = READ_DATA_FAIL;

    ADDR_BIT_VALUE_PROP mTmpValueProp;

    /*�����ݱ��浽��Ӧ��������*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_3P:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[0].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_2P:
    case REG_TYPE_7P:
    case REG_TYPE_4P:
    case REG_TYPE_6P:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regDoubleWordList[eRegType].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regDoubleWordList[eRegType].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyDWordValue);
            }

            /*��ֵ*/
            unsigned int nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regDoubleWordList[eRegType].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regDoubleWordList[eRegType].at(mAddr.nAddrValue + i).nValue;

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
