#ifndef ADDRMANAGE_H
#define ADDRMANAGE_H

#include "tools/dealwithstring.h"
#include "tools/structHead.h"

class AddrManage
{
public:
    AddrManage();

    /*������Ҫͨ�ŵĶ���ַ�б�*/
    void setReadAddrList(vector<ADDR_RW_STRUCT > mSourceAddrList, bool bCover = true);

    /*ȡ�ö���ַ*/
    bool getNextReadAddr(ADDR_RW_STRUCT &mReadAddr);

    /*ȡ��д��ַ*/
    bool getNextWriteAddr(ADDR_RW_STRUCT &mWriteAddr);

    /*���д��ַ*/
    void setWriteAddrList(ADDR_RW_STRUCT mSourceAddr);

    /*�������ַ�Ķ�������ֵ*/
    bool setAddrValue(ADDR_TARGET_PROP mAddr, vector<uchar > valueList);

    /*�Ե�ַ��������*/
    void reCollateAddrpriority();

    /*ͨ��ʧ�ܣ����д��ַ*/
    void clearWriteAddr();

    /*�л����棬����ɻ����ͨ�ŵ�ַ*/
    void clearReadAddr();

    /*�л����棬��¼��ַ*/
    void setAddrListBuf(vector<ADDR_TARGET_PROP> addrList);

    /*�л����棬��ַ*/
    vector<ADDR_TARGET_PROP> getAddrListBuf();

private:
    void setAddrToList(ADDR_RW_STRUCT mAddr);

private:
    int m_nReadLevel;
    int m_nWriteLevel;
    int m_nRLevel1UseTimes;
    int m_nWLevel1UseTimes;
    int m_nGetAddrTimes;
    int m_nMaxTimes;

    int m_nOldReadLevel;
    int m_nOldReadIndex;

    int m_nReadIndex1;
    int m_nReadIndex2;
    int m_nReadIndex3;
    int m_nReadIndex4;

    int m_nReadIndex5;
    int m_nReadIndex6;
    int m_nReadIndex7;
    int m_nReadIndex8;

    vector<ADDR_TARGET_PROP > m_readAddrListbuf;

    vector<ADDR_RW_STRUCT > m_readAddrListLevel1;
    vector<ADDR_RW_STRUCT > m_readAddrListLevel2;
    vector<ADDR_RW_STRUCT > m_readAddrListLevel3;
    vector<ADDR_RW_STRUCT > m_readAddrListLevel4;

    vector<ADDR_RW_STRUCT > m_readAddrListHightLevel1;
    vector<ADDR_RW_STRUCT > m_readAddrListHightLevel2;
    vector<ADDR_RW_STRUCT > m_readAddrListHightLevel3;
    vector<ADDR_RW_STRUCT > m_readAddrListHightLevel4;

    deque<ADDR_RW_STRUCT > m_writeAddrListLevel1;
    deque<ADDR_RW_STRUCT > m_writeAddrListLevel2;
    deque<ADDR_RW_STRUCT > m_writeAddrListLevel3;
};

#endif // ADDRMANAGE_H
