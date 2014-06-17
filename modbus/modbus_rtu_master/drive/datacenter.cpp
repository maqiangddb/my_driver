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

    int nSetLen = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_0X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList.at(0).size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
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
                    readDataList.regBitList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[0][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[0][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regBitList.at(0).size();

                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

//                LOGE("mAddr.nAddrValue=%d,mAddr.nAddrLen=%d",mAddr.nAddrValue,mAddr.nAddrLen);
//                LOGE("valueList.size=%d",valueList.size());
                //��Ч�ֽ���Ŀ
                int byteLen = mAddr.nAddrLen/8+(mAddr.nAddrLen%8==0?0:1);
                //��Ҫд���λ����Ŀ
                int bitLen = mAddr.nAddrLen + mAddr.nAddrValue%8;
//                LOGE("byteLen=%d",byteLen);
//                LOGE("bitLen=%d",bitLen);
                /*��ֵ*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < byteLen; i++)
                {
                    uchar nTmpValue = valueList.at(i);
                    for(int j = 0; j < 8; j++)
                    {
                        int index = i*8 + j;
                        if(index < bitLen){
                            readDataList.regBitList[0][nStartAddr + i*8 + j].eErrorStatus = eSuccess;
                            if(nTmpValue & (1 << j))
                            {
                                readDataList.regBitList[0][nStartAddr + i*8 + j].nValue = 1;
                            }
                            else
                            {
                                readDataList.regBitList[0][nStartAddr + i*8 + j].nValue = 0;
                            }
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_1X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
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
                    readDataList.regBitList[1][mAddr.nAddrValue + i].eErrorStatus = eSuccess;
                    if(valueList.at(i) > 0)
                    {
                        readDataList.regBitList[1][mAddr.nAddrValue + i].nValue = 1;
                    }
                    else
                    {
                        readDataList.regBitList[1][mAddr.nAddrValue + i].nValue = 0;
                    }
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regBitList[1].size();
                if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
                {
                    readDataList.regBitList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
                }

                //��Ч�ֽ���Ŀ
                int byteLen = mAddr.nAddrLen/8+(mAddr.nAddrLen%8==0?0:1);
                //��Ҫд���λ����Ŀ
                int bitLen = mAddr.nAddrLen + mAddr.nAddrValue%8;

                /*��ֵ*/
                int nStartAddr = (mAddr.nAddrValue/8)*8;
                for(int i = 0; i < byteLen; i++)
                {
                    uchar nTmpValue = valueList.at(i);
                    for(int j = 0; j < 8; j++)
                    {
                        int index = i*8 + j;
                        if(index < bitLen){
                            readDataList.regBitList[1][nStartAddr + i*8 + j].eErrorStatus = eSuccess;
                            if(nTmpValue & (1 << j))
                            {
                                readDataList.regBitList[1][nStartAddr + i*8 + j].nValue = 1;
                            }
                            else
                            {
                                readDataList.regBitList[1][nStartAddr + i*8 + j].nValue = 0;
                            }
                        }
                    }
                }
            }
            break;
        }
    case REG_TYPE_3X_BIT:
        {
            int nSize = readDataList.regWordList[0].size();
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[0].resize(nValueListSize, emptyWordValue);
            }

            if(bUserData)  //���û����õ�ֵ
            {
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
                    readDataList.regWordList[0][nStartPos].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    nTmpValue = readDataList.regWordList[0].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= 1 << nBitPost;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regWordList[0][nStartPos].nValue = nTmpValue;
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regWordList[0].size();
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/2;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regWordList[0].resize(nStartAddr + nSetDataLen, emptyWordValue);
                }

                /*��ֵ*/
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][nStartAddr + i].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[0][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_3X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
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
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[0][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_3X_DB:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen +1)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen +1, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/2)
            {
                nSetLen = nListSize/2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            int nSetDataLen = nSetLen/2;
            if(bUserData)
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;

                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(4*i +0);
                    uchar nHighByte = valueList.at(4*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i +1].nValue = nTmpValue;

                    /*���ø�8λ��ǰ*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    nLowByte = valueList.at(4*i +2);
                    nHighByte = valueList.at(4*i +3);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i].nValue = nTmpValue;
                }

                /*����ʣ������*/
                for(int i = (nSetLen/2)*2; i < nSetLen; i++)
                {
                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i +0);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + i +1].nValue = nTmpValue;
                    readDataList.regWordList[0][mAddr.nAddrValue + i +1].eErrorStatus = eSuccess;
                }
            }
            else
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;

                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ��ں� ���ֽ���ǰ*/
                    uchar nHighByte = valueList.at(4*i +2);
                    uchar nLowByte = valueList.at(4*i +3);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i +1].nValue = nTmpValue;

                    /*���ø�8λ��ǰ*/
                    /*����ֵ ���ֽ��ں� ���ֽ���ǰ*/
                    nHighByte = valueList.at(4*i +0);
                    nLowByte = valueList.at(4*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + 2*i].nValue = nTmpValue;
                }

                /*����ʣ������*/
                for(int i = (nSetLen/2)*2; i < nSetLen; i++)
                {
                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i +0);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[0][mAddr.nAddrValue + i +1].nValue = nTmpValue;
                    readDataList.regWordList[0][mAddr.nAddrValue + i +1].eErrorStatus = eSuccess;
                }
            }
            break;
        }
    case REG_TYPE_4X_BIT:
        {
            int nSize = readDataList.regWordList[1].size();
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[1].resize(nValueListSize, emptyWordValue);
            }

            if(bUserData)  //���û����õ�ֵ
            {
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
                    readDataList.regWordList[1][nStartPos].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    nTmpValue = readDataList.regWordList[1].at(nStartPos).nValue;
                    if(valueList.at(i) > 0)
                    {
                        nTmpValue |= 1 << nBitPost;
                    }
                    else
                    {
                        nTmpValue &= ~(1 << nBitPost);
                    }
                    readDataList.regWordList[1][nStartPos].nValue = nTmpValue;
                }
            }
            else       //����Ǵ�PLC��ȡ������
            {
                /*���ݴ洢�����������������·���ռ�*/
                nSize = readDataList.regWordList[1].size();
                int nStartAddr = mAddr.nAddrValue/16;
                int nSetDataLen = nListSize/2;
                if(nSize < nStartAddr + nSetDataLen)
                {
                    readDataList.regWordList[1].resize(nStartAddr + nSetDataLen, emptyWordValue);
                }

                /*��ֵ*/
                ushort nTmpValue = 0;
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[1][nStartAddr + i].eErrorStatus = eSuccess;

                    /*����ֵ*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[1][nStartAddr + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_4X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
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
                    readDataList.regWordList[1][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ,���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = (nTmpValue << 8);
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            else      //����Ǵ�PLC��ȡ������
            {
                for(int i = 0; i < nSetLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[1][mAddr.nAddrValue + i].eErrorStatus = eSuccess;

                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue = nTmpValue + (nLowByte & 0xff);
                    readDataList.regWordList[1][mAddr.nAddrValue + i].nValue = nTmpValue;
                }
            }
            break;
        }
    case REG_TYPE_4X_DB:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen +1)
            {
                readDataList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen +1, emptyWordValue);
            }

            nSetLen = mAddr.nAddrLen;
            if(nSetLen > nListSize/2)
            {
                nSetLen = nListSize/2;
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            int nSetDataLen = nSetLen/2;
            if(bUserData)
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;

                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(4*i +0);
                    uchar nHighByte = valueList.at(4*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i +1].nValue = nTmpValue;

                    /*���ø�8λ��ǰ*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    nLowByte = valueList.at(4*i +2);
                    nHighByte = valueList.at(4*i +3);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i].nValue = nTmpValue;
                }

                /*����ʣ������*/
                for(int i = (nSetLen/2)*2; i < nSetLen; i++)
                {
                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nLowByte = valueList.at(2*i +0);
                    uchar nHighByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + i +1].nValue = nTmpValue;
                    readDataList.regWordList[1][mAddr.nAddrValue + i +1].eErrorStatus = eSuccess;
                }
            }
            else
            {
                for(int i = 0; i < nSetDataLen; i++)
                {
                    /*����״̬*/
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i].eErrorStatus = eSuccess;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i +1].eErrorStatus = eSuccess;

                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ��ں� ���ֽ���ǰ*/
                    uchar nHighByte = valueList.at(4*i +2);
                    uchar nLowByte = valueList.at(4*i +3);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i +1].nValue = nTmpValue;

                    /*���ø�8λ��ǰ*/
                    /*����ֵ ���ֽ��ں� ���ֽ���ǰ*/
                    nHighByte = valueList.at(4*i +0);
                    nLowByte = valueList.at(4*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + 2*i].nValue = nTmpValue;
                }

                /*����ʣ������*/
                for(int i = (nSetLen/2)*2; i < nSetLen; i++)
                {
                    /*���õ�8λ�ں�*/
                    /*����ֵ ���ֽ���ǰ ���ֽ��ں�*/
                    uchar nHighByte = valueList.at(2*i +0);
                    uchar nLowByte = valueList.at(2*i +1);
                    nTmpValue = nHighByte & 0xff;
                    nTmpValue = nTmpValue << 8;
                    nTmpValue += nLowByte;
                    readDataList.regWordList[1][mAddr.nAddrValue + i +1].nValue = nTmpValue;
                    readDataList.regWordList[1][mAddr.nAddrValue + i +1].eErrorStatus = eSuccess;
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
    case REG_TYPE_0X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[0].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_1X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regBitList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regBitList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyBitValue);
            }

            /*��ֵ*/
            for(int i = 0; i < mAddr.nAddrLen; i++)
            {
                mTmpValueProp = readDataList.regBitList[1].at(mAddr.nAddrValue + i);
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_3X_BIT:
        {
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[0].resize(nValueListSize, emptyWordValue);
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
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(nStartPos).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[0].at(nStartPos).nValue;
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
    case REG_TYPE_4X_BIT:
        {
            int nValueListSize = (mAddr.nAddrValue + mAddr.nAddrLen)/16 +1;

            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[1].size();
            if(nSize < nValueListSize)
            {
                readDataList.regWordList[1].resize(nValueListSize, emptyWordValue);
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
                mTmpValueProp.eErrorStatus = readDataList.regWordList[1].at(nStartPos).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[1].at(nStartPos).nValue;
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
    case REG_TYPE_4X:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen)
            {
                readDataList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i <  mAddr.nAddrLen; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[1].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[1].at(mAddr.nAddrValue + i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }
            break;
        }
    case REG_TYPE_3X_DB:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[0].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen +1)
            {
                readDataList.regWordList[0].resize(mAddr.nAddrValue + mAddr.nAddrLen +1, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen/2; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(mAddr.nAddrValue + 2*i + 1).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[0].at(mAddr.nAddrValue + 2*i + 1).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[0].at(mAddr.nAddrValue + 2*i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[0].at(mAddr.nAddrValue + 2*i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }

            /*����ʣ������*/
            for(int i = (mAddr.nAddrLen/2)*2 +1; i < mAddr.nAddrLen +1; i++)
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
    case REG_TYPE_4X_DB:
        {
            /*���ݴ洢�����������������·���ռ�*/
            int nSize = readDataList.regWordList[1].size();
            if(nSize < mAddr.nAddrValue + mAddr.nAddrLen +1)
            {
                readDataList.regWordList[1].resize(mAddr.nAddrValue + mAddr.nAddrLen +1, emptyWordValue);
            }

            /*��ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < mAddr.nAddrLen/2; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[1].at(mAddr.nAddrValue + 2*i + 1).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[1].at(mAddr.nAddrValue + 2*i + 1).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);

                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[1].at(mAddr.nAddrValue + 2*i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[1].at(mAddr.nAddrValue + 2*i).nValue;

                mTmpValueProp.nValue = nTmpValue & 0xff;
                nResultList.push_back(mTmpValueProp);

                mTmpValueProp.nValue = (nTmpValue >> 8) & 0xff;
                nResultList.push_back(mTmpValueProp);
            }

            /*����ʣ������*/
            for(int i = (mAddr.nAddrLen/2)*2 +1; i < mAddr.nAddrLen +1; i++)
            {
                /*����״̬*/
                mTmpValueProp.eErrorStatus = readDataList.regWordList[1].at(mAddr.nAddrValue + i).eErrorStatus;

                /*����ֵ*/
                nTmpValue = readDataList.regWordList[1].at(mAddr.nAddrValue + i).nValue;

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
