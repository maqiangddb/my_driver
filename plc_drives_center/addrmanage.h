#ifndef ADDRMANAGE_H
#define ADDRMANAGE_H

#include "tools/dealwithstring.h"
#include "tools/structHead.h"

class AddrManage
{
public:
    AddrManage();

    /*设置需要通信的读地址列表*/
    void setReadAddrList(vector<ADDR_RW_STRUCT > mSourceAddrList, bool bCover = true);

    /*取得读地址*/
    bool getNextReadAddr(ADDR_RW_STRUCT &mReadAddr);

    /*取得写地址*/
    bool getNextWriteAddr(ADDR_RW_STRUCT &mWriteAddr);

    /*添加写地址*/
    void setWriteAddrList(ADDR_RW_STRUCT mSourceAddr);

    /*保存读地址的读回来的值*/
    bool setAddrValue(ADDR_TARGET_PROP mAddr, vector<uchar > valueList);

    /*对地址重新整理*/
    void reCollateAddrpriority();

    /*通信失败，清除写地址*/
    void clearWriteAddr();

    /*切换画面，清除旧画面的通信地址*/
    void clearReadAddr();

    /*切换画面，记录地址*/
    void setAddrListBuf(vector<ADDR_TARGET_PROP> addrList);

    /*切换画面，地址*/
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
