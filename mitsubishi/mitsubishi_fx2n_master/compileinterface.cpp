/************************************************************************
 *                        SAMKOON TECHNOLOGY CORP.
 *                          COPYRIGHT (C) 2012
 *
 * Description: 供上位编译和地址检查调用的一些函数接口
 *
 * Functions: None.
 *
 * Modification Log:
 * Date             Name             Description Of Change
 * 2012-7-27        latory           Creation.
 ***********************************************************************/

#include "drive/datacenter.h"
#include "drive/plcprotocol.h"

#ifndef ANDROID_PLC_DLL

DataCenter g_dataCenterObj;
PlcProtocol g_plcProtocolObj;

/******************************************************************
 * Function: 获得协议的所有寄存器的名字
 * Parameters:
 * Return: 所有的寄存器名字
 ******************************************************************/
DLL_EXPORT void getAllRegNames(vector<string > &sNameList, ADDR_REG_TYPE eAddrType)
{
    sNameList = g_plcProtocolObj.getRegNameList(eAddrType);
}

/******************************************************************
 * Function: 通过寄存器的名字获得下标
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getRegIndexFromName(string sRegName, int &nRegIndex)
{
    return g_plcProtocolObj.getRegIndexFromName(sRegName, nRegIndex);
}

/******************************************************************
 * Function: 地址字符串类型转化为int类型
 * Parameters: mAddrSource，mAddrTarget
 * mAddrSource: 要转化的地址结构体
 * mAddrTarget： 转化后的地址结构体
 * Return: null
 ******************************************************************/
DLL_EXPORT bool addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget)
{
    return g_plcProtocolObj.addrStringToInt(mAddrSource, mAddrTarget);
}

/******************************************************************
 * Function: 检查地址类型是否正确
 * Parameters: mAddrProp， eAddrRegType
 * mAddrProp: 要检查的地址结构体
 * eAddrRegType： 地址功能类型，功能属于位地址还是字地址
 * Return: null
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType)
{
    return g_plcProtocolObj.checkAddrCorrect(mAddrProp, eAddrRegType);
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
 * Function: 字符串地址的检查
 * Parameters: null
 * Return: 校验是否正确
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp)
{
    return g_plcProtocolObj.checkStrAddr(sAddrStr, eAddrRegType, sAddrProp);
}

/******************************************************************
 * Function: 地址的多重复制的接口
 * Parameters: null
 * Return: 复制成功
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP > &mSourceAddrList, int nInterval, int nCopyNum)
{
    return g_plcProtocolObj.addrsMutilCopy(mStartAddr, mSourceAddrList, nInterval, nCopyNum);
}

/******************************************************************
 * Function: 获得默认站号
 * Parameters: null
 * Return: 默认站号
 ******************************************************************/
DLL_EXPORT int getDefaultStation()
{
    return g_plcProtocolObj.getDefaultStation();
}

/******************************************************************
 * Function: 获得默认本协议的默认串口信息
 * Parameters: null
 * Return: SERIAL_PORT_PROP结构体，串口信息
 ******************************************************************/
DLL_EXPORT SERIAL_PORT_PROP getDefaultSerialInfo()
{
    return g_plcProtocolObj.getDefaultSerialInfo();
}

/******************************************************************
 * Function: 获得默认超时时间
 * Parameters: null
 * Return: 默认超时时间,单位：ms
 ******************************************************************/
DLL_EXPORT int getDefaultTimeout()
{
    return g_plcProtocolObj.getDefaultTimeout();
}

/******************************************************************
 * Function: 获得默认最大读写长度
 * Parameters: null
 * Return: 默认最大读写长度,单位：byte
 ******************************************************************/
DLL_EXPORT int getDefaultMaxRwLen()
{
    return g_plcProtocolObj.getMaxRwLen();
}

/******************************************************************
 * Function: 获得默认重试次数
 * Parameters: null
 * Return: 默认重试次数,单位：次
 ******************************************************************/
DLL_EXPORT int getDefaultRetryTimes()
{
    int nRetryTimes = 3;
    return nRetryTimes;
}

/******************************************************************
 * Function: 获得默认最小采集周期
 * Parameters: null
 * Return: 默认最小采集周期,单位：毫秒
 ******************************************************************/
DLL_EXPORT int getDefaultCollectRate()
{
    return g_plcProtocolObj.getDefaultCollectRate();
}

/******************************************************************
 * Function: 获得帧间隔时间
 * Parameters: null
 * Return: 获得帧间隔时间,单位：毫秒
 ******************************************************************/
DLL_EXPORT int getDefaultIntervalTime()
{
    return g_plcProtocolObj.getDefaultIntervalTime();
}

/*************************************************************************
 * Funciton: 获取协议的连接类型
 * Parameters:
 * Return: 协议类型
*************************************************************************/
DLL_EXPORT int getProType()
{
    return g_plcProtocolObj.getProType();
}

#endif
