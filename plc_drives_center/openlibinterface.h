#ifndef OPENLIBINTERFACE_H
#define OPENLIBINTERFACE_H

#include "dlfcn.h"
#include "tools/structHead.h"

class OpenLibInterface
{
public:
    OpenLibInterface();

    /*打开库文件*/
    bool openLib(string sFilePath);

    /*关闭库文件*/
    bool closeLib();

    /*获得错误信息*/
    string getLastErrorInfo();

private:
    /*获得函数句柄*/
    void *getFunHand(string sFunName);

private:
    string m_sFilePath;
    void *m_pLibFileHand;

public:
    /*获得打包数据*/
    bool makePackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, vector<SEND_PACKAGE> &mSendPkgList, ADDR_RW_STRUCT &mCurrAddr);

    /*对读到数据进行解包*/
    CMN_RW_ERROR_CODE unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar > &targetList);

    /*检查写数据或读取的数据是否检验正确*/
    CMN_RW_ERROR_CODE checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType);

    /*获得握手协议的包*/
    bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*对握手协议的进行解包*/
    CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes);

    /*获取PLC驱动的类型，主站还是从站*/
    PLC_FUN_TYPE getPlcType();

    /*获得这种地址寄存器是否可读*/
    bool getAddrCanRead(ADDR_TARGET_PROP mAddr);

    /*获得这种地址寄存器是否可写*/
    bool getAddrCanWrite(ADDR_TARGET_PROP mAddr);

    /*整理地址*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr);

    /*设置读取到的数据*/
    bool setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData = true, CMN_RW_ERROR_CODE eSuccess = CMN_NOMAL_CODE);

    /*获得地址值*/
    bool getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList);

    /*从站函数，从串口接收到的数据到驱动处理，成功后返回打包好的发送数据*/
    CMN_RW_ERROR_CODE getSlaveDataStr(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, vector<SEND_PACKAGE> &mSendPkg);

    /**************************以下是一些函数库的接口***********************/
private:
    /*获得打包数据*/
    typedef bool (*pMakePackage)(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr);

    /*对读到数据进行解包*/
    typedef CMN_RW_ERROR_CODE (*pUnpackReadData)(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar > &targetList);

    /*检查写数据或读取的数据是否检验正确*/
    typedef CMN_RW_ERROR_CODE (*pCheckRcvData)(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType);

    /*获得握手协议的包*/
    typedef bool (*pGetHandshakePkg)(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate);

    /*对握手协议的进行解包*/
    typedef CMN_RW_ERROR_CODE (*pUnpackHandshakePkg)(vector<uchar > rcvList, int nCurrTimes);

    /*获取PLC驱动的类型，主站还是从站*/
    typedef PLC_FUN_TYPE (*pGetPlcType)();

    /*获得这种地址寄存器是否可读*/
    typedef bool (*pGetAddrCanRead)(ADDR_TARGET_PROP mAddr);

    /*获得这种地址寄存器是否可写*/
    typedef bool (*pGetAddrCanWrite)(ADDR_TARGET_PROP mAddr);

    /*设置读取到的数据*/
    typedef bool (*pSetReadData)(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess);

    /*获得地址值*/
    typedef bool (*pGetReadData)(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList);

    /*从站函数，从串口接收到的数据到驱动处理，成功后返回打包好的发送数据*/
    typedef CMN_RW_ERROR_CODE (*pGetSlaveDataStr)(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg);

    /*整理地址*/
    typedef bool (*pSortOutAddrList)(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr);

private:
    /*获得打包数据*/
    pMakePackage m_makePackage;

    /*对读到数据进行解包*/
    pUnpackReadData m_unpackReadData;

    /*检查写数据或读取的数据是否检验正确*/
    pCheckRcvData m_checkRcvData;

    /*获得握手协议的包*/
    pGetHandshakePkg m_getHandshakePkg;

    /*对握手协议的进行解包*/
    pUnpackHandshakePkg m_unpackHandshakePkg;

    /*获取PLC驱动的类型，主站还是从站*/
    pGetPlcType m_getPlcType;

    /*获得这种地址寄存器是否可读*/
    pGetAddrCanRead m_getAddrCanRead;

    /*获得这种地址寄存器是否可写*/
    pGetAddrCanWrite m_getAddrCanWrite;

    /*设置读取到的数据*/
    pSetReadData m_setReadData;

    /*获得地址值*/
    pGetReadData m_getReadData;

    /*从站函数，从串口接收到的数据到驱动处理，成功后返回打包好的发送数据*/
    pGetSlaveDataStr m_getSlaveDataStr;

    /*整理地址*/
    pSortOutAddrList m_sortOutAddrList;
};

#endif // OPENLIBINTERFACE_H
