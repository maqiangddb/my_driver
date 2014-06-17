#include "openlibinterface.h"
#include <stdio.h>

OpenLibInterface::OpenLibInterface()
{
    m_pLibFileHand = NULL;
    m_makePackage = NULL;
    m_unpackReadData = NULL;
    m_checkRcvData = NULL;
    m_getHandshakePkg = NULL;
    m_unpackHandshakePkg = NULL;
    m_getPlcType = NULL;
    m_getAddrCanRead = NULL;
    m_getAddrCanWrite = NULL;
    m_setReadData = NULL;
    m_getReadData = NULL;
    m_getSlaveDataStr = NULL;
    m_sortOutAddrList = NULL;
}

/******************************************************************
 * Function: �򿪿��ļ�
 * Parameters: sFilePath
 * Return: null
 ******************************************************************/
bool OpenLibInterface::openLib(string sFilePath)
{
    /*��֪��Ϊʲô���򿪱�Ŀ⣬m_pLibFileHand����ָ�����*/
//    if(NULL != m_pLibFileHand)
//    {
//        return true;
//    }

//#ifdef ANDROID_PLC_DLL
//        LOGD("open %s lib", sFilePath.data());
//#endif
    FILE *f = fopen("/data/sumsung.phone","r");

    m_sFilePath = sFilePath;
    if(f!=NULL){
        sFilePath = "/data/data/com.android.Samkoonhmi/drv-lib/lib" + sFilePath + ".so";
        fclose(f);
        f=NULL;
    }else{
        sFilePath = "/data/data/com.android.Samkoonhmi/lib/lib" + sFilePath + ".so";
    }

    m_pLibFileHand = dlopen(sFilePath.data(), RTLD_NOW);
    if(NULL == m_pLibFileHand)
    {
        const char *pErrorInfo = dlerror();
//        cout << "open " << sFilePath << " lib error,msg is:" << pErrorInfo << endl;
#ifdef ANDROID_PLC_DLL
        LOGE("open %s lib error,msg is: %s", sFilePath.data(), pErrorInfo);
#endif
        return false;
    }

    /*�򿪿��ͬʱ�򿪺����ӿ�*/
    *(void **)(&m_makePackage) = getFunHand("makePackage");
    *(void **)(&m_unpackReadData) = getFunHand("unpackReadData");
    *(void **)(&m_checkRcvData) = getFunHand("checkRcvData");
    *(void **)(&m_getHandshakePkg) = getFunHand("getHandshakePkg");
    *(void **)(&m_unpackHandshakePkg) = getFunHand("unpackHandshakePkg");
    *(void **)(&m_getPlcType) = getFunHand("getPlcType");
    *(void **)(&m_getAddrCanRead) = getFunHand("getAddrCanRead");
    *(void **)(&m_getAddrCanWrite) = getFunHand("getAddrCanWrite");
    *(void **)(&m_setReadData) = getFunHand("setReadData");
    *(void **)(&m_getReadData) = getFunHand("getReadData");
    *(void **)(&m_getSlaveDataStr) = getFunHand("getSlaveDataStr");
    *(void **)(&m_sortOutAddrList) = getFunHand("sortOutAddrList");

    return true;
}

/******************************************************************
 * Function: �رտ��ļ�
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::closeLib()
{
    if(NULL == m_pLibFileHand)  return true;

    int nErrorInfo = dlclose(m_pLibFileHand);
    if(nErrorInfo != 0)
    {
        const char *pErrorInfo = dlerror();
//        cout << "close lib error,msg is:" << pErrorInfo << endl;
#ifdef ANDROID_PLC_DLL
        LOGE("close lib error,msg is: %s", pErrorInfo);
#endif
        return false;
    }

    m_pLibFileHand = NULL;
    return true;
}

/******************************************************************
 * Function: ��ô�����Ϣ
 * Parameters: null
 * Return: null
 ******************************************************************/
string OpenLibInterface::getLastErrorInfo()
{
    string sError = "no error";

    const char *pInfo = dlerror();
    if(pInfo != 0)
    {
        sError = pInfo;
    }

    return sError;
}

/******************************************************************
 * Function: ��ú����ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
void *OpenLibInterface::getFunHand(string sFunName)
{
//#ifdef ANDROID_PLC_DLL
//        LOGD("open %s fun", sFunName.data());
//#endif

    if(sFunName.empty())
    {
//        cout << "function name is empty" << endl;
//#ifdef ANDROID_PLC_DLL
//        LOGE("function name is empty");
//#endif
        return NULL;
    }
    if(NULL == m_pLibFileHand)
    {
//        LOGD("open %s function but m_pLibFileHand is null, m_sFilePath = %s",sFunName.data(), m_sFilePath.data());
        bool bSuccess = openLib(m_sFilePath);
        if(!bSuccess) return NULL;
    }

    /*ʵ��ȡ�������*/
    void *pFunHand = dlsym(m_pLibFileHand, sFunName.data());
    const char *pErrorInfo = dlerror();
    if(NULL != pErrorInfo)
    {
//        LOGD("first open %s function error,msg is: %s", sFunName.data(), pErrorInfo);
//        LOGD("try second open");
        pFunHand = dlsym(m_pLibFileHand, sFunName.data());
        pErrorInfo = dlerror();
        if(NULL != pErrorInfo)
        {
//            cout << "open " << sFunName << "function error,msg is:" << pErrorInfo << endl;
//            LOGE("open %s function error,msg is: %s", sFunName.data(), pErrorInfo);
            return NULL;
        }
    }

//    LOGD("open %s function success", sFunName.data());

    return pFunHand;
}

/******************************************************************
 * Function: ��ô������ makePackage �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::makePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, vector<SEND_PACKAGE> &mSendPkgList, ADDR_RW_STRUCT &mCurrAddr)
{
    SEND_PACKAGE mSendPkg;
    mSendPkg.sendDataList.clear();

    if(NULL == m_makePackage)
    {
        *(void **)(&m_makePackage) = getFunHand("makePackage");
    }

    if(NULL == m_makePackage)
    {
        return false;
    }

    mSendPkgList.clear();
    bool bSuccess = false;
    SEND_PACKAGE mFirstPkg;
    mFirstPkg.nPkgIndex = 1;
    mFirstPkg.sendDataList.resize(1000);
    mFirstPkg.sendDataList.clear();
    bSuccess = m_makePackage(mCmnProp, nCmnTimes, mFirstPkg, mCurrAddr);
    if(bSuccess == false)
    {
        return false;
    }
    mSendPkgList.push_back(mFirstPkg);
    if(mFirstPkg.nTotalPkgNum > 1)
    {
        for(int i = 2; i <= mFirstPkg.nTotalPkgNum; i++)
        {
            mFirstPkg.nPkgIndex = i;
            mFirstPkg.sendDataList.clear();
            bSuccess = m_makePackage(mCmnProp, nCmnTimes, mFirstPkg, mCurrAddr);
            if(bSuccess == false)
            {
                mSendPkgList.clear();
                return false;
            }
            mSendPkgList.push_back(mFirstPkg);
        }
    }

    return bSuccess;
}

/******************************************************************
 * Function: ��ý������ unpackReadData �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE OpenLibInterface::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = CALL_PROTO_ERROR;
    if(NULL == m_unpackReadData)
    {
        *(void **)(&m_unpackReadData) = getFunHand("unpackReadData");
    }

    if(NULL == m_unpackReadData)
    {
        return eCmnInfo;
    }

    targetList.resize(1000);
    targetList.clear();
    eCmnInfo = m_unpackReadData(mUnpackProp, rcvList, targetList);
    return eCmnInfo;
}

/******************************************************************
 * Function: ���"���д���ݻ��ȡ�������Ƿ������ȷ" checkRcvData �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE OpenLibInterface::checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType)
{
    CMN_RW_ERROR_CODE eCmnInfo = CALL_PROTO_ERROR;
    if(NULL == m_checkRcvData)
    {
        *(void **)(&m_checkRcvData) = getFunHand("checkRcvData");
    }

    if(NULL == m_checkRcvData)
    {
        return eCmnInfo;
    }

    eCmnInfo = m_checkRcvData(mUnpackProp, rcvList, eCheckType);
    return eCmnInfo;
}

/******************************************************************
 * Function: ���" �������Э��İ� " getHandshakePkg �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    if(NULL == m_getHandshakePkg)
    {
        *(void **)(&m_getHandshakePkg) = getFunHand("getHandshakePkg");
    }

    if(NULL == m_getHandshakePkg)
    {
        return false;
    }

    bool bSuccess = m_getHandshakePkg(mHandShake, nCmnTimes, nPkgIndex, nStationId, nBaudRate);
    return bSuccess;
}

/******************************************************************
 * Function: ���"������Э��İ����н��" unpackHandshakePkg �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE OpenLibInterface::unpackHandshakePkg(vector<uchar> rcvList, int nCurrTimes)
{
    CMN_RW_ERROR_CODE eCmnInfo = CALL_PROTO_ERROR;
    if(NULL == m_unpackHandshakePkg)
    {
        *(void **)(&m_unpackHandshakePkg) = getFunHand("unpackHandshakePkg");
    }

    if(NULL == m_unpackHandshakePkg)
    {
        return eCmnInfo;
    }

    eCmnInfo = m_unpackHandshakePkg(rcvList, nCurrTimes);
    return eCmnInfo;
}

/******************************************************************
 * Function: ���"��ȡPLC���������ͣ���վ���Ǵ�վ" getPlcType �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
PLC_FUN_TYPE OpenLibInterface::getPlcType()
{
    if(NULL == m_getPlcType)
    {
        *(void **)(&m_getPlcType) = getFunHand("getPlcType");
    }

    if(NULL == m_getPlcType)
    {
        return PLC_OTHER_TYPE;
    }

    PLC_FUN_TYPE ePlcType = m_getPlcType();
    return ePlcType;
}

/******************************************************************
 * Function: ���"������ֵ�ַ�Ĵ����Ƿ�ɶ�" getAddrCanRead �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    if(NULL == m_getAddrCanRead)
    {
        *(void **)(&m_getAddrCanRead) = getFunHand("getAddrCanRead");
    }

    if(NULL == m_getAddrCanRead)
    {
        return false;
    }

    bool bSuccess = m_getAddrCanRead(mAddr);

    return bSuccess;
}

/******************************************************************
 * Function: ���"������ֵ�ַ�Ĵ����Ƿ��д" getAddrCanWrite �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    if(NULL == m_getAddrCanWrite)
    {
        *(void **)(&m_getAddrCanWrite) = getFunHand("getAddrCanWrite");
    }

    if(NULL == m_getAddrCanWrite)
    {
        return false;
    }

    bool bSuccess = m_getAddrCanWrite(mAddr);

    return bSuccess;
}

/******************************************************************
 * Function: ���"���ö�ȡ��������" setReadData �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData, CMN_RW_ERROR_CODE eSuccess)
{
    if(NULL == m_setReadData)
    {
        *(void **)(&m_setReadData) = getFunHand("setReadData");
    }

    if(NULL == m_setReadData)
    {
        return false;
    }

    bool bSuccess =  m_setReadData(mAddrInfo, readDataList, bUserData, eSuccess);
    return bSuccess;
}

/******************************************************************
 * Function: ���"��õ�ֵַ" getReadData �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList)
{
    if(NULL == m_getReadData)
    {
        *(void **)(&m_getReadData) = getFunHand("getReadData");
    }

    if(NULL == m_getReadData)
    {
        return false;
    }

    nResultList.resize(1000);
    nResultList.clear();
    bool bSuccess = m_getReadData(mAddr, readDataList, nResultList);
    return bSuccess;
}

/******************************************************************
 * Function: ���"��վ�������Ӵ��ڽ��յ������ݵ����������ɹ��󷵻ش���õķ�������" getSlaveDataStr �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE OpenLibInterface::getSlaveDataStr(vector<unsigned char> rcvList, int nStationId, STATION_VALUE_PROP &readDataList, vector<SEND_PACKAGE> &mSendPkg)
{
    CMN_RW_ERROR_CODE eCmnInfo = CALL_PROTO_ERROR;
    if(NULL == m_getSlaveDataStr)
    {
        *(void **)(&m_getSlaveDataStr) = getFunHand("getSlaveDataStr");
    }

    if(NULL == m_getSlaveDataStr)
    {
        return eCmnInfo;
    }

    mSendPkg.clear();
    SEND_PACKAGE mFirstPkg;
    mFirstPkg.nTotalPkgNum = 1;
    mFirstPkg.nPkgIndex = 1;
    mFirstPkg.sendDataList.resize(1000);
    mFirstPkg.sendDataList.clear();
    eCmnInfo = m_getSlaveDataStr(rcvList, nStationId, readDataList, mFirstPkg);
    mSendPkg.push_back(mFirstPkg);
    if(mFirstPkg.nTotalPkgNum == 1)
    {
        return eCmnInfo;
    }
    else if(mFirstPkg.nTotalPkgNum > 1)
    {
        if(eCmnInfo != CMN_NOMAL_CODE)
        {
            return eCmnInfo;
        }
        for(int i = 2; i <= mFirstPkg.nTotalPkgNum; i++)
        {
            mFirstPkg.nPkgIndex = i;
            mFirstPkg.sendDataList.clear();
            m_getSlaveDataStr(rcvList, nStationId, readDataList, mFirstPkg);
            mSendPkg.push_back(mFirstPkg);
        }
    }
    return eCmnInfo;
}

/******************************************************************
 * Function: ���"�����ַ" sortOutAddrList �ľ��
 * Parameters: null
 * Return: null
 ******************************************************************/
bool OpenLibInterface::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    if(NULL == m_sortOutAddrList)
    {
        *(void **)(&m_sortOutAddrList) = getFunHand("sortOutAddrList");
    }

    if(NULL == m_sortOutAddrList)
    {
        return false;
    }

    targetList.resize(1000);
    targetList.clear();
    bool bSuccess = m_sortOutAddrList(mSourceAddrList, targetList, nMaxRWlen, bWriteAddr);

    return bSuccess;
}
