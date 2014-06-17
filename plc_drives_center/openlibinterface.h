#ifndef OPENLIBINTERFACE_H
#define OPENLIBINTERFACE_H

#include "dlfcn.h"
#include "tools/structHead.h"

class OpenLibInterface
{
public:
    OpenLibInterface();

    /*�򿪿��ļ�*/
    bool openLib(string sFilePath);

    /*�رտ��ļ�*/
    bool closeLib();

    /*��ô�����Ϣ*/
    string getLastErrorInfo();

private:
    /*��ú������*/
    void *getFunHand(string sFunName);

private:
    string m_sFilePath;
    void *m_pLibFileHand;

public:
    /*��ô������*/
    bool makePackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, vector<SEND_PACKAGE> &mSendPkgList, ADDR_RW_STRUCT &mCurrAddr);

    /*�Զ������ݽ��н��*/
    CMN_RW_ERROR_CODE unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar > &targetList);

    /*���д���ݻ��ȡ�������Ƿ������ȷ*/
    CMN_RW_ERROR_CODE checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType);

    /*�������Э��İ�*/
    bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*������Э��Ľ��н��*/
    CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes);

    /*��ȡPLC���������ͣ���վ���Ǵ�վ*/
    PLC_FUN_TYPE getPlcType();

    /*������ֵ�ַ�Ĵ����Ƿ�ɶ�*/
    bool getAddrCanRead(ADDR_TARGET_PROP mAddr);

    /*������ֵ�ַ�Ĵ����Ƿ��д*/
    bool getAddrCanWrite(ADDR_TARGET_PROP mAddr);

    /*�����ַ*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr);

    /*���ö�ȡ��������*/
    bool setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData = true, CMN_RW_ERROR_CODE eSuccess = CMN_NOMAL_CODE);

    /*��õ�ֵַ*/
    bool getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList);

    /*��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������*/
    CMN_RW_ERROR_CODE getSlaveDataStr(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, vector<SEND_PACKAGE> &mSendPkg);

    /**************************������һЩ������Ľӿ�***********************/
private:
    /*��ô������*/
    typedef bool (*pMakePackage)(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr);

    /*�Զ������ݽ��н��*/
    typedef CMN_RW_ERROR_CODE (*pUnpackReadData)(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar > &targetList);

    /*���д���ݻ��ȡ�������Ƿ������ȷ*/
    typedef CMN_RW_ERROR_CODE (*pCheckRcvData)(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType);

    /*�������Э��İ�*/
    typedef bool (*pGetHandshakePkg)(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate);

    /*������Э��Ľ��н��*/
    typedef CMN_RW_ERROR_CODE (*pUnpackHandshakePkg)(vector<uchar > rcvList, int nCurrTimes);

    /*��ȡPLC���������ͣ���վ���Ǵ�վ*/
    typedef PLC_FUN_TYPE (*pGetPlcType)();

    /*������ֵ�ַ�Ĵ����Ƿ�ɶ�*/
    typedef bool (*pGetAddrCanRead)(ADDR_TARGET_PROP mAddr);

    /*������ֵ�ַ�Ĵ����Ƿ��д*/
    typedef bool (*pGetAddrCanWrite)(ADDR_TARGET_PROP mAddr);

    /*���ö�ȡ��������*/
    typedef bool (*pSetReadData)(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess);

    /*��õ�ֵַ*/
    typedef bool (*pGetReadData)(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList);

    /*��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������*/
    typedef CMN_RW_ERROR_CODE (*pGetSlaveDataStr)(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg);

    /*�����ַ*/
    typedef bool (*pSortOutAddrList)(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr);

private:
    /*��ô������*/
    pMakePackage m_makePackage;

    /*�Զ������ݽ��н��*/
    pUnpackReadData m_unpackReadData;

    /*���д���ݻ��ȡ�������Ƿ������ȷ*/
    pCheckRcvData m_checkRcvData;

    /*�������Э��İ�*/
    pGetHandshakePkg m_getHandshakePkg;

    /*������Э��Ľ��н��*/
    pUnpackHandshakePkg m_unpackHandshakePkg;

    /*��ȡPLC���������ͣ���վ���Ǵ�վ*/
    pGetPlcType m_getPlcType;

    /*������ֵ�ַ�Ĵ����Ƿ�ɶ�*/
    pGetAddrCanRead m_getAddrCanRead;

    /*������ֵ�ַ�Ĵ����Ƿ��д*/
    pGetAddrCanWrite m_getAddrCanWrite;

    /*���ö�ȡ��������*/
    pSetReadData m_setReadData;

    /*��õ�ֵַ*/
    pGetReadData m_getReadData;

    /*��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������*/
    pGetSlaveDataStr m_getSlaveDataStr;

    /*�����ַ*/
    pSortOutAddrList m_sortOutAddrList;
};

#endif // OPENLIBINTERFACE_H
