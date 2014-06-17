#ifndef MANAGECENTER_H
#define MANAGECENTER_H

#include "addrmanage.h"
#include "openlibinterface.h"
#include "localaddrs.h"
#include <pthread.h>

/*一个PLC连接的通道，不同通道所属的协议可以相同*/
typedef struct plcChannel{
    bool bNeedHandShake;                                       //是否需要握手协议
    uint nCmnTimes;                                             //通信的次数
    ADDR_RW_STRUCT mCurrAddr;                                   //当前地址
    AddrManage addrManageObj;                                 //地址管理中心的对象，里面包含PLC的所有读写地址
    PLC_RATE_INFO mPlcRateInfo;                                 //能决定这个PLC是否唯一的属性
    vector<uchar > nRcvValueList;                               //这个PLC的接收数据的缓存，保存接收到的数据
    STATION_VALUE_PROP readDataList;                            //这个PLC所有读写数据的内部缓存
}PlC_CHANNEL;

/*一个协议对应的所有PLC数据和处理*/
typedef struct protocolLinkInfo{
    string sLibName;
    OpenLibInterface mLibObj;
    vector<PlC_CHANNEL > mPlcChannelList;
}PROTOCOL_LINK_INFO;

class ManageCenter
{
public:
    ManageCenter();

    ~ManageCenter();

    /*获得写数据的打包数据*/
    bool getWriteDataStr(vector<SEND_PACKAGE > &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_WRITE);

    /*获得读数据的打包数据*/
    bool getReadDataStr(vector<SEND_PACKAGE > &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_READ);

    /*获得握手协议的包*/
    bool getHandshakePkg(PLC_RATE_INFO mPlcRateInfo, SEND_PACKAGE &mHandShake, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*获取当前通信的地址*/
    bool getCurrAddr(PLC_RATE_INFO mPlcRateInfo, ADDR_TARGET_PROP &mCurrAddr);

    /*设置读数据的接收数据*/
    int checkReadData(char *pRrcvData, int nLen, int nReturnLen, int nCurrTimes, CMN_SEND_TYPE eSendType, PLC_RATE_INFO mPlcRateInfo, bool &bChange);

    /*校验写数据的返回码，确定写数据是否成功*/
    int checkWriteData(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo, bool &bChange);

    /*校验握手协议的包*/
    int checkHandshakePkg(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo);

    /*设置通信标志*/
    bool setCmnInfo(int nCmnInfo, PLC_RATE_INFO mPlcRateInfo);

    /*设置地址值的通信状态*/
    bool setDataListErrorCode(STATION_VALUE_PROP &readDataList, CMN_RW_ERROR_CODE eErrorCode);

    /*获取协议的功能类型，主要是主站还是从站*/
    int getProtocolFunType(PLC_RATE_INFO mPlcRateInfo);

    /*设置所有读地址*/
    void setAllReadAddrs(vector<ADDR_TARGET_PROP > addrList, bool bCover, PLC_RATE_INFO mPlcRateInfo,int nMaxRWlen);

    /*清除所有读地址*/
    void clearAllReadAddr();

    /*整理地址*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen, bool bWriteAddr);

    /*对写地址打包*/
    bool makeWritePackage(ADDR_TARGET_PROP mAddrProp, vector<uchar > dataList, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_WRITE);

    /*对读地址打包*/
    bool makeReadPackage(ADDR_TARGET_PROP mAddrProp, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_READ);

    /*设置写数据的地址*/
    void setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<uchar > dataList, PLC_RATE_INFO mPlcRateInfo, int nSimulateType);

    /*获得读数据的值*/
    bool getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP > &valueList, PLC_RATE_INFO mPlcRateInfo);

    /*设置写字符串的地址*/
    bool setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string > dataList, PLC_RATE_INFO mPlcRateInfo);

    /*获得读字符串的值*/
    bool getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string > &valueList, PLC_RATE_INFO mPlcRateInfo);

    /*关闭协议*/
    bool closeProtocolSo(PLC_RATE_INFO mPlcRateInfo);

    /*关闭所有协议*/
    bool closeAllSo();

    /*加锁*/
    bool threadLock();

    /*解锁*/
    bool threadUnlock();

    /*尝试加锁，不成功则返回*/
    bool threadTryLock();

    /*接收从串口接收到的数据到从站处理，只适合从站*/
    int rcvStrForSlave(char *pRrcvData, int nLen, int nStationId, PLC_RATE_INFO mPlcRateInfo, vector<SEND_PACKAGE> &eSendPkg);

private:
    bool getProtocolLinkInfo(PLC_RATE_INFO mPlcRateInfo, int &nLinkIndex, int &nPlcIndex);

private:
    LocalAddrs m_localAddrObj;
    vector<PROTOCOL_LINK_INFO > m_protocolLinkList;

    /*多线程同步解锁*/
    pthread_mutex_t m_mutex;
};

#endif // MANAGECENTER_H
