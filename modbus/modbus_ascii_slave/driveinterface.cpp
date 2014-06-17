/************************************************************************
 *                        SAMKOON TECHNOLOGY CORP.
 *                          COPYRIGHT (C) 2012
 *
 * Description: ����λandroid���õ�һЩ�����ӿ�
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
 * Function: �������Э��İ�
 * Parameters: address list
 * Return: null
 ******************************************************************/
DLL_EXPORT bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    return g_plcProtocolObj.getHandshakePkg(mHandShake, nCmnTimes, nPkgIndex, nStationId, nBaudRate);
}

/******************************************************************
 * Function: ������Э��İ������н��
 * Parameters: rcvList nCurrTimes
 * Return: true or false
 ******************************************************************/
DLL_EXPORT CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes)
{
    return g_plcProtocolObj.unpackHandshakePkg(rcvList, nCurrTimes);
}

/******************************************************************
 * Function: ��ȡPLC���������ͣ���վ���Ǵ�վ
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT PLC_FUN_TYPE getPlcType()
{
    return g_plcProtocolObj.getPlcType();
}

/******************************************************************
 * Function: ������ֵ�ַ�Ĵ����Ƿ�ɶ�
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    return g_plcProtocolObj.getAddrCanRead(mAddr);
}

/******************************************************************
 * Function: ������ֵ�ַ�Ĵ����Ƿ��д
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    return g_plcProtocolObj.getAddrCanWrite(mAddr);
}

/******************************************************************
 * Function: �Ե�ַ��������
 * Parameters: mSourceAddrList�� targetList��nMaxRWlen
 * mSourceAddrList: Ҫ����ĵ�ַ�ṹ�弯��
 * targetList: �����ĵ�ַ�ṹ�弯��
 * nMaxRWlen: ����д����
 * Return: null
 ******************************************************************/
DLL_EXPORT bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr)
{
    return g_plcProtocolObj.sortOutAddrList(mSourceAddrList, targetList, nMaxRWlen, bWriteAddr);
}

/******************************************************************
 * Function: ���ö�ȡ��������
 * Parameters: null
 * Return: null
 ******************************************************************/
DLL_EXPORT bool setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess)
{
    return g_dataCenterObj.setReadData(mAddrInfo, readDataList, bUserData, eSuccess);
}

/******************************************************************
 * Function: ��õ�ֵַ
 * Parameters: null
 * Return: ��ֵַ�ļ�����
 ******************************************************************/
DLL_EXPORT bool getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList)
{
    return g_dataCenterObj.getReadData(mAddr, readDataList, nResultList);
}

/******************************************************************
 * Function: ��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������
 * Parameters: null
 * Return: true or false
 ******************************************************************/
DLL_EXPORT CMN_RW_ERROR_CODE getSlaveDataStr(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg)
{
    return g_plcProtocolObj.getSlaveDataStr(rcvList, nStationId, readDataList, mSendPkg);
}

#endif
