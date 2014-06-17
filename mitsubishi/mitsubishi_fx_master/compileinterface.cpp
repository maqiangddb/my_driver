/************************************************************************
 *                        SAMKOON TECHNOLOGY CORP.
 *                          COPYRIGHT (C) 2012
 *
 * Description: ����λ����͵�ַ�����õ�һЩ�����ӿ�
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
 * Function: ���Э������мĴ���������
 * Parameters:
 * Return: ���еļĴ�������
 ******************************************************************/
DLL_EXPORT void getAllRegNames(vector<string > &sNameList, ADDR_REG_TYPE eAddrType)
{
    sNameList = g_plcProtocolObj.getRegNameList(eAddrType);
}

/******************************************************************
 * Function: ͨ���Ĵ��������ֻ���±�
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT bool getRegIndexFromName(string sRegName, int &nRegIndex)
{
    return g_plcProtocolObj.getRegIndexFromName(sRegName, nRegIndex);
}

/******************************************************************
 * Function: ��ַ�ַ�������ת��Ϊint����
 * Parameters: mAddrSource��mAddrTarget
 * mAddrSource: Ҫת���ĵ�ַ�ṹ��
 * mAddrTarget�� ת����ĵ�ַ�ṹ��
 * Return: null
 ******************************************************************/
DLL_EXPORT bool addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget)
{
    return g_plcProtocolObj.addrStringToInt(mAddrSource, mAddrTarget);
}

/******************************************************************
 * Function: ����ַ�����Ƿ���ȷ
 * Parameters: mAddrProp�� eAddrRegType
 * mAddrProp: Ҫ���ĵ�ַ�ṹ��
 * eAddrRegType�� ��ַ�������ͣ���������λ��ַ�����ֵ�ַ
 * Return: null
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType)
{
    return g_plcProtocolObj.checkAddrCorrect(mAddrProp, eAddrRegType);
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
 * Function: �ַ�����ַ�ļ��
 * Parameters: null
 * Return: У���Ƿ���ȷ
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp)
{
    return g_plcProtocolObj.checkStrAddr(sAddrStr, eAddrRegType, sAddrProp);
}

/******************************************************************
 * Function: ��ַ�Ķ��ظ��ƵĽӿ�
 * Parameters: null
 * Return: ���Ƴɹ�
 ******************************************************************/
DLL_EXPORT ADDR_CHECK_ERROR_INFO addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP > &mSourceAddrList, int nInterval, int nCopyNum)
{
    return g_plcProtocolObj.addrsMutilCopy(mStartAddr, mSourceAddrList, nInterval, nCopyNum);
}

/******************************************************************
 * Function: ���Ĭ��վ��
 * Parameters: null
 * Return: Ĭ��վ��
 ******************************************************************/
DLL_EXPORT int getDefaultStation()
{
    return g_plcProtocolObj.getDefaultStation();
}

/******************************************************************
 * Function: ���Ĭ�ϱ�Э���Ĭ�ϴ�����Ϣ
 * Parameters: null
 * Return: SERIAL_PORT_PROP�ṹ�壬������Ϣ
 ******************************************************************/
DLL_EXPORT SERIAL_PORT_PROP getDefaultSerialInfo()
{
    return g_plcProtocolObj.getDefaultSerialInfo();
}

/******************************************************************
 * Function: ���Ĭ�ϳ�ʱʱ��
 * Parameters: null
 * Return: Ĭ�ϳ�ʱʱ��,��λ��ms
 ******************************************************************/
DLL_EXPORT int getDefaultTimeout()
{
    return g_plcProtocolObj.getDefaultTimeout();
}

/******************************************************************
 * Function: ���Ĭ������д����
 * Parameters: null
 * Return: Ĭ������д����,��λ��byte
 ******************************************************************/
DLL_EXPORT int getDefaultMaxRwLen()
{
    return g_plcProtocolObj.getMaxRwLen();
}

/******************************************************************
 * Function: ���Ĭ�����Դ���
 * Parameters: null
 * Return: Ĭ�����Դ���,��λ����
 ******************************************************************/
DLL_EXPORT int getDefaultRetryTimes()
{
    int nRetryTimes = 3;
    return nRetryTimes;
}

/******************************************************************
 * Function: ���Ĭ����С�ɼ�����
 * Parameters: null
 * Return: Ĭ����С�ɼ�����,��λ������
 ******************************************************************/
DLL_EXPORT int getDefaultCollectRate()
{
    return g_plcProtocolObj.getDefaultCollectRate();
}

/******************************************************************
 * Function: ���֡���ʱ��
 * Parameters: null
 * Return: ���֡���ʱ��,��λ������
 ******************************************************************/
DLL_EXPORT int getDefaultIntervalTime()
{
    return g_plcProtocolObj.getDefaultIntervalTime();
}

/*************************************************************************
 * Funciton: ��ȡЭ�����������
 * Parameters:
 * Return: Э������
*************************************************************************/
DLL_EXPORT int getProType()
{
    return g_plcProtocolObj.getProType();
}

#endif
