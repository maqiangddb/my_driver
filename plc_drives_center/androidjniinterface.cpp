#include "androidjniinterface.h"

#ifdef ANDROID_PLC_DLL

#include <android/log.h>
#define LOG_TAG "debug"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


/******************************************************************
 * Function: ���д���ݵĴ������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getWriteDataJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*�Է��ͽṹ��ת��*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*��÷��ͷ�ʽ*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô�����д������Ϣ*/
    jboolean bSuccess = manageObj.getWriteDataStr(mSendPkgList, mPlcRateInfo, eCmnType);

    /*���ʧ�ܣ����߷�������Ϊ���򷵻�*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*��ֵ����λ*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    /*�ͷ��ڴ�*/
    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: ��ö����ݵĴ������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getReadDataJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*�Է��ͽṹ��ת��*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*��÷��ͷ�ʽ*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô����Ķ�������Ϣ*/
    jboolean bSuccess = manageObj.getReadDataStr(mSendPkgList, mPlcRateInfo, eCmnType);

    /*���ʧ�ܣ����߷�������Ϊ���򷵻�*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*��ֵ����λ*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    /*�ͷ��ڴ�*/
    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: �������Э��Ĵ������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getHandshakePkgJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jobject joPlcInfo, jint jnPkgIndex, jint jnStationId, jint jnBaudRate)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*�Է��ͽṹ��ת��*/
    SEND_PACKAGE mSendPkg;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô����Ķ�������Ϣ*/
    jboolean bSuccess = manageObj.getHandshakePkg(mPlcRateInfo, mSendPkg, jnPkgIndex, jnStationId, jnBaudRate);

    /*���ʧ�ܣ����߷�������Ϊ���򷵻�*/
    if(!bSuccess) return false;

    /*��ֵ����λ*/
    dealJniObj.setSendPropToJni(env, joSendData, mSendPkg);

    return bSuccess;
}

/******************************************************************
 * Function: ��õ�ǰͨ�ŵĵ�ַ
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getCurrAddrJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo, jobject joAddr)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joPlcInfo || 0 == joAddr) return false;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô����Ķ�������Ϣ*/
    ADDR_TARGET_PROP mAddr;
    bool bSuccess = manageObj.getCurrAddr(mPlcRateInfo, mAddr);

    /*��ֵ����λ*/
    if(bSuccess)
    {
        dealJniObj.setAddrProp(env, joAddr, mAddr);
    }

    return bSuccess;
}

/******************************************************************
 * Function: ���ö��õ�������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setReadDataJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jint jeCmnTyoe, jobject joPlcInfo, jobject joNoticInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*��ȡ�������ݼ���*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*��÷��ͷ�ʽ*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*���ö��õ�������*/
    bool bChange = false;
    jint nResultInfo = manageObj.checkReadData(pszStr, len, jeReturnLen, nCurrTimes, eCmnType, mPlcRateInfo, bChange);

    /*ֵ�ı䣬֪ͨ��λ*/
    if(bChange)
    {
        dealJniObj.setPlcNoticValue(env, joNoticInfo, true);
    }

    return nResultInfo;
}

/******************************************************************
 * Function: У��д�����Ƿ�ɹ�
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_checkWriteDataJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jobject joPlcInfo, jobject joNoticInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*��ȡ�������ݼ���*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*У��д�����Ƿ�ɹ�*/
    bool bChange = false;
    jint nResultInfo = manageObj.checkWriteData(pszStr, len, jeReturnLen, nCurrTimes, mPlcRateInfo, bChange);

    /*ֵ�ı䣬֪ͨ��λ*/
    if(bChange)
    {
        dealJniObj.setPlcNoticValue(env, joNoticInfo, true);
    }

    return nResultInfo;
}

/******************************************************************
 * Function: ������Э��ķ�������У��
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_checkHandshakePkgJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*��ȡ�������ݼ���*/
    char *pszStr = 0;
    int len = 0;

    if(joReadStr != 0)
    {
        pszStr = (char*)env->GetByteArrayElements(joReadStr,0);
        if(0 == pszStr) return OTHER_ERROR_CODE;

        len = env->GetArrayLength(joReadStr);
        if(len > jnSetSize)
        {
            len = jnSetSize;
        }
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*У��д�����Ƿ�ɹ�*/
    jint nResultInfo = manageObj.checkHandshakePkg(pszStr,len, jeReturnLen, nCurrTimes, mPlcRateInfo);

    /*�ͷ��ڴ�*/
    if(joReadStr != 0 && pszStr != 0)
    {
        env->ReleaseByteArrayElements(joReadStr, (jbyte *)pszStr,0);
    }
    return nResultInfo;
}

/******************************************************************
 * Function: ����ͨ��ʧ�ܱ�־
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setCmnInfoJni
        (JNIEnv *env, jobject thiz, jint jnCmnInfo, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joPlcInfo) return false;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*У��д�����Ƿ�ɹ�*/
    bool bSuccess = manageObj.setCmnInfo(jnCmnInfo, mPlcRateInfo);

    /*�ͷ��ڴ�*/
    return bSuccess;
}

/******************************************************************
 * Function: ���մӴ��ڽ��յ������ݵ���վ����ֻ�ʺϴ�վ
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_rcvStrForSlaveJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jnStationId, jobject joSendData, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo || 0 == joSendData) return OTHER_ERROR_CODE;

    /*��ȡ�������ݼ���*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*�Է��ͽṹ��ת��*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*�����վ���յ�������*/
    jint nResultInfo = manageObj.rcvStrForSlave(pszStr, len, jnStationId, mPlcRateInfo, mSendPkgList);

    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);
    return nResultInfo;
}

/******************************************************************
 * Function: ���ö���ַ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setOneSceneAddrsJni
        (JNIEnv *env, jobject thiz, jobjectArray joAddrList, jboolean jbCover, jobject joPlcInfo,jint jMaxRWlen)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddrList || 0 == joPlcInfo) return false;

    /*��ʼ��װ��ַ������*/
    vector<ADDR_TARGET_PROP> addrList;
    addrList.clear();

    /*һ��������ַ*/
    ADDR_TARGET_PROP mTmpAddr;
    int len = env->GetArrayLength(joAddrList);
    for(int i = 0; i < len; i++)
    {
        jobject joAddr = env->GetObjectArrayElement(joAddrList, i);

        /*�Ե�ַ����ת��*/
        mTmpAddr = dealJniObj.getAddrPropFromJni(env,joAddr);
        addrList.push_back(mTmpAddr);

        /*ɾ������*/
        env->DeleteLocalRef(joAddr);
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*���ö���ַ����*/
    manageObj.setAllReadAddrs(addrList, jbCover, mPlcRateInfo,jMaxRWlen);
    return true;
}

/******************************************************************
 * Function: ������еĶ���ַ
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_clearAllReadAddrJni
        (JNIEnv *env, jobject thiz)
{
    manageObj.clearAllReadAddr();
    return true;
}

/******************************************************************
 * Function: �Ե�ַ��������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_sortOutAddrListJni
        (JNIEnv *env, jobject thiz, jobjectArray joAddrList, jobject joAddrListObj, jobject joPlcInfo, jint jnMaxRWlen, jboolean jbWriteAddr)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddrList || 0 == joPlcInfo) return false;

    /*��ʼ��װ��ַ������*/
    vector<ADDR_TARGET_PROP> addrList;
    addrList.clear();

    vector<ADDR_TARGET_PROP> mSortAddrList;
    mSortAddrList.clear();

    /*һ��������ַ*/
    ADDR_TARGET_PROP mTmpAddr;
    int len = env->GetArrayLength(joAddrList);
    for(int i = 0; i < len; i++)
    {
        jobject joAddr = env->GetObjectArrayElement(joAddrList, i);

        /*�Ե�ַ����ת��*/
        mTmpAddr = dealJniObj.getAddrPropFromJni(env,joAddr);
        addrList.push_back(mTmpAddr);

        /*ɾ������*/
        env->DeleteLocalRef(joAddr);
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*���ö���ַ����*/
    bool bSuccess = manageObj.sortOutAddrList(addrList, mSortAddrList, mPlcRateInfo, jnMaxRWlen, jbWriteAddr);

    if(bSuccess)
    {
        dealJniObj.setAddrListToJni(env, joAddrListObj, mSortAddrList);
    }
    return bSuccess;
}

/******************************************************************
 * Function: ��д��ַ���д��
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_makeWritePackageJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jint jnSetSize, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joSendData || 0 == jeCmnTyoe || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    /*��ȡ�������ݼ���*/
    int len = env->GetArrayLength(jDataList);
    if(len <= 0) return false;

    char pDataList[len];
    env->GetByteArrayRegion(jDataList, 0, len, (jbyte *)pDataList);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*�Ȱ�ֵ�浽tmpList��*/
    vector<uchar > tmpList;
    tmpList.clear();
    uchar nTmpValue = 0;
    for(int i = 0; i < len; i++)
    {
        nTmpValue = pDataList[i];
        tmpList.push_back(nTmpValue);
    }

    /*�Է��ͽṹ��ת��*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*��÷��ͷ�ʽ*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô�����д������Ϣ*/
    jboolean bSuccess = manageObj.makeWritePackage(mAddrProp, tmpList, mSendPkgList, mPlcRateInfo, eCmnType);

    /*���ʧ�ܣ����߷�������Ϊ���򷵻�*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*��ֵ����λ*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: �Զ���ַ���д��
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_makeReadPackageJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == joSendData || 0 == jeCmnTyoe || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    /*�Է��ͽṹ��ת��*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*��÷��ͷ�ʽ*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ô�����д������Ϣ*/
    jboolean bSuccess = manageObj.makeReadPackage(mAddrProp, mSendPkgList, mPlcRateInfo, eCmnType);

    /*���ʧ�ܣ����߷�������Ϊ���򷵻�*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*��ֵ����λ*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    return bSuccess;
}


/******************************************************************
 * Function: ���Bytes���͵����ݽӿ�
 * Parameters:
 * Return: ���еļĴ�������
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getRegBytesDataJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jobject joPlcInfo, jobject joRcvProp)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joRcvProp || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("getRegBytesDataJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*��ȡֵ*/
    vector<ADDR_BIT_VALUE_PROP> tmpList;
    bool bSuccess = manageObj.getReadRegDataList(mAddrProp, tmpList, mPlcRateInfo);
    if(!bSuccess)
    {
        /*�ڴ��ͷ�*/
        tmpList.clear();
        vector<ADDR_BIT_VALUE_PROP>().swap(tmpList);
        return false;
    }

    int nListSize = tmpList.size();
    int len = env->GetArrayLength(jDataList);
    if(nListSize > len)
    {
        nListSize = len;
    }

    CMN_RW_ERROR_CODE eRrror;
    jbyte jbBuff[nListSize];

    for(int i = 0; i < nListSize; i++)
    {
        jbBuff[i] = tmpList.at(i).nValue;
        eRrror = tmpList.at(i).eErrorStatus;
    }

    /*����״̬��־��*/
    dealJniObj.setErrorInfo(env, joRcvProp, eRrror);
    env->SetByteArrayRegion(jDataList, 0, nListSize, jbBuff);

    /*�ڴ��ͷ�*/
    tmpList.clear();
    vector<ADDR_BIT_VALUE_PROP>().swap(tmpList);

    return true;
}


/******************************************************************
 * Function: ���� bytes ���͵����ݽӿ�
 * Parameters:
 * Return: ���еļĴ�������
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setRegBytesDataJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jobject joPlcInfo, jint jnRwType, jint jnSimulateType)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env, joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("setRegBytesDataJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    /*��ȡ�������ݼ���*/
    int len = env->GetArrayLength(jDataList);
    if(len <= 0) return false;

    char pDataList[len];
    env->GetByteArrayRegion(jDataList, 0, len, (jbyte *)pDataList);

    /*�Ȱ�ֵ�浽tmpList��*/
    vector<uchar > tmpList;
    tmpList.clear();
    uchar nTmpChar = 0;

    for(int i = 0; i < len; i++)
    {
        nTmpChar = pDataList[i];
        tmpList.push_back(nTmpChar);
    }

    if(!tmpList.empty())
    {
        /*��PLC��Ϣת��*/
        PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
        mAddrProp.eAddrRWprop = (READ_WRITE_COM_TYPE)jnRwType;
        manageObj.setWriteRegDataList(mAddrProp, tmpList, mPlcRateInfo, jnSimulateType);
    }

    return true;
}

/******************************************************************
 * Function: ���string���͵����ݽӿ�
 * Parameters:
 * Return: ���еļĴ�������
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getLocalStrJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobject jStrList, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == jStrList || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("getLocalStrJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    vector<string > tmpList ;
    tmpList.clear();

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    bool bSuccess = manageObj.getReadRegStrList(mAddrProp, tmpList, mPlcRateInfo);

    if(bSuccess)
    {
        dealJniObj.setStrListToObjArray(env, jStrList, tmpList);
    }

    /*�ͷ��ڴ�*/
    tmpList.clear();
    vector<string >().swap(tmpList);

    return bSuccess;
}

/******************************************************************
 * Function: ����string���͵����ݽӿ�
 * Parameters:
 * Return: ���еļĴ�������
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setLocalStrJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobjectArray jStrList, jint jnSetSize, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joAddr || 0 == jStrList || 0 == joPlcInfo) return false;

    /*��ȡ��ַ�ṹ��*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("setLocalStrJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    vector<string > tmpList = dealJniObj.getStrListFromObjArray(env, jStrList, jnSetSize);
    if(tmpList.empty()) return false;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    bool bSuccess = manageObj.setWriteRegStrList(mAddrProp, tmpList, mPlcRateInfo);

    return bSuccess;
}

/******************************************************************
 * Function: �������������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getProtocolFunTypeJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    return manageObj.getProtocolFunType(mPlcRateInfo);
}

/******************************************************************
 * Function: �ر�һ������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_closeProtocolJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == joPlcInfo) return false;

    /*��PLC��Ϣת��*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    return manageObj.closeProtocolSo(mPlcRateInfo);
}

/******************************************************************
 * Function: �ر���������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_closeAllProtocolJni
        (JNIEnv *env, jobject thiz)
{
    return manageObj.closeAllSo();
}

/******************************************************************
 * Function: bytes תshort��������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToShortsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jshortArray jshorts)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jshorts) return false;

    /*��ȡ�������ݼ���*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(jshorts);
    if(nTargetLen > nByteLen/2)
    {
        nTargetLen = nByteLen/2;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*������ֵ*/
    short tmpValue[nTargetLen];
    memset(tmpValue,0,nTargetLen*2);
    memcpy(tmpValue, pValue, nTargetLen*2);

//    jshort jniArray[nTargetLen];
//    for(int i = 0; i < nTargetLen; i++)
//    {
//        jniArray[i] = tmpValue[i];
//    }

    env->SetShortArrayRegion(jshorts, 0, nTargetLen, tmpValue);

    return true;
}

/******************************************************************
 * Function: bytes ת unsigned short ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToUShortsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jintArray jushorts)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jushorts) return false;

    /*��ȡ�������ݼ���*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(jushorts);
    if(nTargetLen > nByteLen/2)
    {
        nTargetLen = nByteLen/2;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*������ֵ*/
    ushort tmpValue[nTargetLen];
    memset(tmpValue,0,nTargetLen*2);
    memcpy(tmpValue, pValue, nTargetLen*2);

    jint jniArray[nTargetLen];
    for(int i = 0; i < nTargetLen; i++)
    {
        jniArray[i] = tmpValue[i];
    }

    env->SetIntArrayRegion(jushorts, 0, nTargetLen, jniArray);

    return true;
}

/******************************************************************
 * Function: bytes ת int ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToIntsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize,jintArray jints)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jints) return false;

    /*��ȡ�������ݼ���*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(jints);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*������ֵ*/
    int tmpValue[nTargetLen];
    memset(tmpValue,0,nTargetLen*4);
    memcpy(tmpValue, pValue, nTargetLen*4);

//    jint jniArray[nTargetLen];
//    for(int i = 0; i < nTargetLen; i++)
//    {
//        jniArray[i] = tmpValue[i];
//    }

    env->SetIntArrayRegion(jints, 0, nTargetLen, tmpValue);

    return true;
}

/******************************************************************
 * Function: bytes ת unsigned int ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToUIntsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jlongArray juints)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == juints) return false;

    /*��ȡ�������ݼ���*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(juints);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*������ֵ*/
    uint tmpValue[nTargetLen];
    memset(tmpValue,0,nTargetLen*4);
    memcpy(tmpValue, pValue, nTargetLen*4);

    jlong jniArray[nTargetLen];
    for(int i = 0; i < nTargetLen; i++)
    {
        jniArray[i] = tmpValue[i];
    }

    env->SetLongArrayRegion(juints, 0, nTargetLen, jniArray);

    return true;
}

/******************************************************************
 * Function: bytes ת float ��������
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToFloatsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jfloatArray jfloats)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jfloats) return false;

    /*��ȡ�������ݼ���*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(jfloats);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*������ֵ*/
    float tmpValue[nTargetLen];
    memset(tmpValue,0,nTargetLen*4);
    memcpy(tmpValue, pValue, nTargetLen*4);

//    jfloat jfValue[nTargetLen];
//    for(int i = 0; i < nTargetLen; i++)
//    {
//        jfValue[i] = tmpValue[i];
//    }

    env->SetFloatArrayRegion(jfloats, 0, nTargetLen, tmpValue);

    return true;
}


/******************************************************************
 * Function: ascii ת bytes ��������
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * jInterval:ת�������һ��asciiʹ��1��unicodeʹ��2
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_asciiToBytesJni
        (JNIEnv *env, jobject thiz, jbyteArray jbytes, jint jInterval, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes) return false;

    /*���short��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nTargetLen <= 0) return false;

    char pValue[nTargetLen];
    env->GetByteArrayRegion(jbytes, 0, nTargetLen, (jbyte *)pValue);
    if(nTargetLen <= 0)
    {
        return false;
    }

    if(jnByteHLPos == 2)
    {
        /*����ֵ*/
        switch(jInterval)
        {
        case 1:
            {
                char nTmpValue = 0;
                int nSetLen = nTargetLen/2;
                for(int i = 0; i < nSetLen; i++)
                {
                    nTmpValue = pValue[2*i];
                    pValue[2*i] = pValue[2*i + 1];
                    pValue[2*i + 1] = nTmpValue;
                }
                break;
            }
        case 2:
            {
                char nTmpValueL = 0;
                char nTmpValueH = 0;
                int nSetLen = nTargetLen/4;
                for(int i = 0; i < nSetLen; i++)
                {
                    nTmpValueL = pValue[4*i + 0];
                    nTmpValueH = pValue[4*i + 1];

                    pValue[4*i + 0] = pValue[4*i + 2];
                    pValue[4*i + 1] = pValue[4*i + 3];

                    pValue[4*i + 2] = nTmpValueL;
                    pValue[4*i + 3] = nTmpValueH;
                }
                break;
            }
        default:
            break;
        }
    }

    env->SetByteArrayRegion(jbytes, 0, nTargetLen, (jbyte *)pValue);
    return true;
}

/******************************************************************
 * Function: short ת bytes ��������
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_shortsToBytesJni
        (JNIEnv *env, jobject thiz, jshortArray jshorts, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jshorts) return false;

    /*��ȡ short ���鳤��*/
    int nSourceLen = env->GetArrayLength(jshorts);
    jshort pValue[nSourceLen];
    env->GetShortArrayRegion(jshorts, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*��� byte ��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *2 > nTargetLen )
    {
        nSourceLen = nTargetLen/2;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*����ֵ*/
    short nTmpValue[nSourceLen];
    for(int i = 0; i < nSourceLen; i++)
    {
        nTmpValue[i] = pValue[i];
    }

    char pTarge[nSourceLen*2];
    memset(pTarge,0,nSourceLen*2);
    memcpy(pTarge, (char *)nTmpValue, nSourceLen*2);

    env->SetByteArrayRegion(jbytes, 0, nSourceLen*2, (jbyte *)pTarge);

    return true;
}

/******************************************************************
 * Function: unsigned short ת bytes ����
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_uShortsToBytesJni
        (JNIEnv *env, jobject thiz, jintArray jushorts, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jushorts) return false;

    /*��ȡ short ���鳤��*/
    int nSourceLen = env->GetArrayLength(jushorts);
    jint pValue[nSourceLen];
    env->GetIntArrayRegion(jushorts, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*��� byte ��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *2 > nTargetLen )
    {
        nSourceLen = nTargetLen/2;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*����ֵ*/
    ushort nTmpValue[nSourceLen];
    for(int i = 0; i < nSourceLen; i++)
    {
        nTmpValue[i] = pValue[i];
    }

    char pTarge[nSourceLen*2];
    memset(pTarge,0,nSourceLen*2);
    memcpy(pTarge, (char *)nTmpValue, nSourceLen*2);

    env->SetByteArrayRegion(jbytes, 0, nSourceLen*2, (jbyte *)pTarge);

    return true;
}

/******************************************************************
 * Function: int ת bytes ����
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intsToBytesJni
        (JNIEnv *env, jobject thiz, jintArray jints, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jints) return false;

    /*��ȡ short ���鳤��*/
    int nSourceLen = env->GetArrayLength(jints);
    jint pValue[nSourceLen];
    env->GetIntArrayRegion(jints, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*��� byte ��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*����ֵ*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*���ֽ���ǰ*/
        int nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*���ֽ���ǰ*/
        int nTmpValue[nSourceLen];
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue[i] = pValue[i];
        }
        memset(pTarge,0, nSourceLen*4);
        memcpy(pTarge, (char *)nTmpValue, nSourceLen*4);
    }

    env->SetByteArrayRegion(jbytes, 0, nSourceLen*4, (jbyte *)pTarge);

    return true;
}

/******************************************************************
 * Function: unsigned int ת bytes ����
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_uIntsToBytesJni
        (JNIEnv *env, jobject thiz, jlongArray juints, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == juints) return false;

    /*��ȡ long ���鳤��*/
    int nSourceLen = env->GetArrayLength(juints);
    jlong pValue[nSourceLen];
    env->GetLongArrayRegion(juints, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*��� byte ��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*����ֵ*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*���ֽ���ǰ*/
        uint nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*���ֽ���ǰ*/
        uint nTmpValue[nSourceLen];
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue[i] = pValue[i];
        }
        memset(pTarge,0, nSourceLen*4);
        memcpy(pTarge, (char *)nTmpValue, nSourceLen*4);
    }

    env->SetByteArrayRegion(jbytes, 0, nSourceLen*4, (jbyte *)pTarge);

    return true;
}

/******************************************************************
 * Function: float ת bytes ��������
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_floatsToBytesJni
        (JNIEnv *env, jobject thiz, jfloatArray jfloats, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jbytes || 0 == jfloats) return false;

    /*��ȡ long ���鳤��*/
    int nSourceLen = env->GetArrayLength(jfloats);
    jfloat pValue[nSourceLen];
    env->GetFloatArrayRegion(jfloats, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*��� byte ��������*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*����ֵ*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*���ֽ���ǰ*/
        float nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*�ȴ��8λ*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*����8λ*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*���ֽ���ǰ*/
        float nTmpValue[nSourceLen];
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue[i] = pValue[i];
        }
        memset(pTarge,0, nSourceLen*4);
        memcpy(pTarge, (char *)nTmpValue, nSourceLen*4);
    }

    env->SetByteArrayRegion(jbytes, 0, nSourceLen*4, (jbyte *)pTarge);

    return true;
}

/******************************************************************
 * Function: double ת bytes ��������
 * Parameters:jnByteHLPos : �ߵ�λ���ã�1�����ֽ���ǰ��2�����ֽ���ǰ
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jbyteArray JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_doublesToBytesJni
        (JNIEnv *env, jobject thiz, jdoubleArray jdoubles, jobject joSendProp)
{
    /*�������Ƿ�Ϊ�գ�Ϊ�շ���ʧ��*/
    if(0 == env || 0 == jdoubles || 0 == joSendProp) return 0;

    /*��ȡ���ͽṹ��*/
    SEND_DATA_STRUCT mSendProp = dealJniObj.getSendStructFromJni(env,joSendProp);

    /*��ȡbyte���鳤��*/
    int nSourceLen = env->GetArrayLength(jdoubles);
    if(nSourceLen <= 0)
    {
        return 0;
    }

    jdouble pValue[nSourceLen];
    env->GetDoubleArrayRegion(jdoubles, 0, nSourceLen, pValue);

    /*����ֵ*/
    char nTmpChar = 0;
    switch(mSendProp.nDataType)
    {
    case BIT_1:                      // λ
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen);
            jbyte bytes[nSourceLen];

            char nTmpValue = 0;
            for(int i = 0; i < nSourceLen; i++)
            {
                nTmpValue = pValue[i];
                bytes[i] = nTmpValue;
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen, bytes);

            return nResultList;
            break;
        }
    case INT_16:                     // 16λ������
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*2);
            jbyte bytes[nSourceLen *2];

            /*����ֵ*/
            short nTmpValue = 0;
            for(int i = 0; i < nSourceLen; i++)
            {
                nTmpValue = pValue[i];

                /*�ȴ��8λ*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                bytes[2*i + 0] = nTmpChar;

                /*����8λ*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                bytes[2*i + 1] = nTmpChar;

            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *2, bytes);

            return nResultList;
            break;
        }
    case POSITIVE_INT_16:            // 16λ����
    case BCD_16:                     // 16λBCD��
    case HEX_16:                     // 16λ16����
    case OTC_16:                     // 16λ�˽���
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*2);
            jbyte bytes[nSourceLen *2];

            /*����ֵ*/
            ushort nTmpValue = 0;
            for(int i = 0; i < nSourceLen; i++)
            {
                nTmpValue = pValue[i];

                /*�ȴ��8λ*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                bytes[2*i + 0] = nTmpChar;

                /*����8λ*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                bytes[2*i + 1] = nTmpChar;

            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *2, bytes);

            return nResultList;
            break;
        }
    case INT_32:                     // 32λ����
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*����ֵ*/
            int nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *4, bytes);

            return nResultList;
            break;
        }
    case POSITIVE_INT_32:            // 32λ������
    case BCD_32:                     // 32λBCD��
    case HEX_32:                     //32λ16����
    case OTC_32:                     //32λ�˽���
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*����ֵ*/
            uint nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *4, bytes);

            return nResultList;
            break;
        }
    case FLOAT_32:                   // 32λ������
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*����ֵ*/
            float nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*���ֽ���ǰ*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = *(pValue + i);

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*�ȴ��8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*����8λ*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *4, bytes);

            return nResultList;
            break;
        }
    case OTHER_DATA_TYPE:
    default:
        {
            break;
        }
    }

    return 0;
}

/******************************************************************
 * Function: int תshort����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jshort JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intToShortJni
        (JNIEnv *env, jobject thiz,jint ji)
{
    int nSource = ji;
    short result = 0;
    memcpy(&result, &nSource, 2);
    jshort jresult = result;
    return jresult;
}

/******************************************************************
 * Function: int ת float ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jfloat JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intToFloatJni
        (JNIEnv *env, jobject thiz,jint ji)
{
    int nSource = ji;
    float result = 0;
    memcpy(&result, &nSource, 4);
    jfloat jresult = result;
    return jresult;
}

/******************************************************************
 * Function: int ת unsigned short ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intToUShortJni
        (JNIEnv *env, jobject thiz,jint ji)
{
    int nSource = ji;
    unsigned short result = 0;
    memcpy(&result, &nSource, 2);
    jint jresult = result;
    return jresult;
}

/******************************************************************
 * Function: int ת unsigned int ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jlong JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intToUIntJni
        (JNIEnv *env, jobject thiz,jint ji)
{
    int nSource = ji;
    unsigned int result = 0;
    memcpy(&result, &nSource, 4);
    jlong jresult = result;
    return jresult;
}

#endif
