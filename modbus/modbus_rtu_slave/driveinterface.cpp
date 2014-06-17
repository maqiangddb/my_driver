/************************************************************************
 *                        SAMKOON TECHNOLOGY CORP.
 *                          COPYRIGHT (C) 2012
 *
 * Description: 供下位android调用的一些函数接口
 *
 * Functions: None.
 *
 * Modification Log:
 * Date             Name             Description Of Change
 * 2012-7-27        latory           Creation.
 ***********************************************************************/
#include "drive/datacenter.h"
#include "drive/plcprotocol.h"

#ifdef ANDROID_PLC_DLL

DataCenter g_dataCenterObj;
PlcProtocol g_plcProtocolObj;

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: address list
 * Return: null
 ******************************************************************/
DLL_EXPORT bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    return g_plcProtocolObj.getHandshakePkg(mHandShake, nCmnTimes, nPkgIndex, nStationId, nBaudRate);
}

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes
 * Return: true or false
 ******************************************************************/
DLL_EXPORT CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes)
{
    return g_plcProtocolObj.unpackHandshakePkg(rcvList, nCurrTimes);
}

/******************************************************************
 * Function: 获取PLC驱动的类型，主站还是从站
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT PLC_FUN_TYPE getPlcType()
{
    return g_plcProtocolObj.getPlcType();
}

/******************************************************************
 * Function: 获得这种地址寄存器是否可读
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    return g_plcProtocolObj.getAddrCanRead(mAddr);
}

/******************************************************************
 * Function: 获得这种地址寄存器是否可写
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    return g_plcProtocolObj.getAddrCanWrite(mAddr);
}

/******************************************************************
 * Function: 对地址整理排序
 * Parameters: mSourceAddrList， targetList，nMaxRWlen
 * mSourceAddrList: 要整理的地址结构体集合
 * targetList: 整理后的地址结构体集合
 * nMaxRWlen: 最大读写长度
 * Return: null
 ******************************************************************/
DLL_EXPORT bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr)
{
    return g_plcProtocolObj.sortOutAddrList(mSourceAddrList, targetList, nMaxRWlen, bWriteAddr);
}

/******************************************************************
 * Function: 设置读取到的数据
 * Parameters: null
 * Return: null
 ******************************************************************/
DLL_EXPORT bool setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess)
{
    return g_dataCenterObj.setReadData(mAddrInfo, readDataList, bUserData, eSuccess);
}

/******************************************************************
 * Function: 获得地址值
 * Parameters: null
 * Return: 地址值的集合体
 ******************************************************************/
DLL_EXPORT bool getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList)
{
    return g_dataCenterObj.getReadData(mAddr, readDataList, nResultList);
}

/******************************************************************
 * Function: 从站函数，从串口接收到的数据到驱动处理，成功后返回打包好的发送数据
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT CMN_RW_ERROR_CODE getSlaveDataStr(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg)
{
    return g_plcProtocolObj.getSlaveDataStr(rcvList, nStationId, readDataList, mSendPkg);
}

#endif
