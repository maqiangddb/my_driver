#ifndef MANAGECENTER_H
#define MANAGECENTER_H

#include "addrmanage.h"
#include "openlibinterface.h"
#include "localaddrs.h"
#include <pthread.h>

/*һ��PLC���ӵ�ͨ������ͬͨ��������Э�������ͬ*/
typedef struct plcChannel{
    bool bNeedHandShake;                                       //�Ƿ���Ҫ����Э��
    uint nCmnTimes;                                             //ͨ�ŵĴ���
    ADDR_RW_STRUCT mCurrAddr;                                   //��ǰ��ַ
    AddrManage addrManageObj;                                 //��ַ�������ĵĶ����������PLC�����ж�д��ַ
    PLC_RATE_INFO mPlcRateInfo;                                 //�ܾ������PLC�Ƿ�Ψһ������
    vector<uchar > nRcvValueList;                               //���PLC�Ľ������ݵĻ��棬������յ�������
    STATION_VALUE_PROP readDataList;                            //���PLC���ж�д���ݵ��ڲ�����
}PlC_CHANNEL;

/*һ��Э���Ӧ������PLC���ݺʹ���*/
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

    /*���д���ݵĴ������*/
    bool getWriteDataStr(vector<SEND_PACKAGE > &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_WRITE);

    /*��ö����ݵĴ������*/
    bool getReadDataStr(vector<SEND_PACKAGE > &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_READ);

    /*�������Э��İ�*/
    bool getHandshakePkg(PLC_RATE_INFO mPlcRateInfo, SEND_PACKAGE &mHandShake, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*��ȡ��ǰͨ�ŵĵ�ַ*/
    bool getCurrAddr(PLC_RATE_INFO mPlcRateInfo, ADDR_TARGET_PROP &mCurrAddr);

    /*���ö����ݵĽ�������*/
    int checkReadData(char *pRrcvData, int nLen, int nReturnLen, int nCurrTimes, CMN_SEND_TYPE eSendType, PLC_RATE_INFO mPlcRateInfo, bool &bChange);

    /*У��д���ݵķ����룬ȷ��д�����Ƿ�ɹ�*/
    int checkWriteData(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo, bool &bChange);

    /*У������Э��İ�*/
    int checkHandshakePkg(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo);

    /*����ͨ�ű�־*/
    bool setCmnInfo(int nCmnInfo, PLC_RATE_INFO mPlcRateInfo);

    /*���õ�ֵַ��ͨ��״̬*/
    bool setDataListErrorCode(STATION_VALUE_PROP &readDataList, CMN_RW_ERROR_CODE eErrorCode);

    /*��ȡЭ��Ĺ������ͣ���Ҫ����վ���Ǵ�վ*/
    int getProtocolFunType(PLC_RATE_INFO mPlcRateInfo);

    /*�������ж���ַ*/
    void setAllReadAddrs(vector<ADDR_TARGET_PROP > addrList, bool bCover, PLC_RATE_INFO mPlcRateInfo,int nMaxRWlen);

    /*������ж���ַ*/
    void clearAllReadAddr();

    /*�����ַ*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen, bool bWriteAddr);

    /*��д��ַ���*/
    bool makeWritePackage(ADDR_TARGET_PROP mAddrProp, vector<uchar > dataList, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_WRITE);

    /*�Զ���ַ���*/
    bool makeReadPackage(ADDR_TARGET_PROP mAddrProp, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType = CMN_READ);

    /*����д���ݵĵ�ַ*/
    void setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<uchar > dataList, PLC_RATE_INFO mPlcRateInfo, int nSimulateType);

    /*��ö����ݵ�ֵ*/
    bool getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP > &valueList, PLC_RATE_INFO mPlcRateInfo);

    /*����д�ַ����ĵ�ַ*/
    bool setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string > dataList, PLC_RATE_INFO mPlcRateInfo);

    /*��ö��ַ�����ֵ*/
    bool getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string > &valueList, PLC_RATE_INFO mPlcRateInfo);

    /*�ر�Э��*/
    bool closeProtocolSo(PLC_RATE_INFO mPlcRateInfo);

    /*�ر�����Э��*/
    bool closeAllSo();

    /*����*/
    bool threadLock();

    /*����*/
    bool threadUnlock();

    /*���Լ��������ɹ��򷵻�*/
    bool threadTryLock();

    /*���մӴ��ڽ��յ������ݵ���վ����ֻ�ʺϴ�վ*/
    int rcvStrForSlave(char *pRrcvData, int nLen, int nStationId, PLC_RATE_INFO mPlcRateInfo, vector<SEND_PACKAGE> &eSendPkg);

private:
    bool getProtocolLinkInfo(PLC_RATE_INFO mPlcRateInfo, int &nLinkIndex, int &nPlcIndex);

private:
    LocalAddrs m_localAddrObj;
    vector<PROTOCOL_LINK_INFO > m_protocolLinkList;

    /*���߳�ͬ������*/
    pthread_mutex_t m_mutex;
};

#endif // MANAGECENTER_H
