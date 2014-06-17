#include "androidjniinterface.h"

#ifdef ANDROID_PLC_DLL

#include <android/log.h>
#define LOG_TAG "debug"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)


/******************************************************************
 * Function: 获得写数据的打包数据
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getWriteDataJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*对发送结构体转换*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*获得发送方式*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的写数据信息*/
    jboolean bSuccess = manageObj.getWriteDataStr(mSendPkgList, mPlcRateInfo, eCmnType);

    /*如果失败，或者返回数据为空则返回*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*传值给上位*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    /*释放内存*/
    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: 获得读数据的打包数据
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getReadDataJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*对发送结构体转换*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*获得发送方式*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的读数据信息*/
    jboolean bSuccess = manageObj.getReadDataStr(mSendPkgList, mPlcRateInfo, eCmnType);

    /*如果失败，或者返回数据为空则返回*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*传值给上位*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    /*释放内存*/
    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: 获得握手协议的打包数据
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getHandshakePkgJni
        (JNIEnv *env, jobject thiz, jobject joSendData, jobject joPlcInfo, jint jnPkgIndex, jint jnStationId, jint jnBaudRate)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joSendData || 0 == joPlcInfo) return false;

    /*对发送结构体转换*/
    SEND_PACKAGE mSendPkg;

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的读数据信息*/
    jboolean bSuccess = manageObj.getHandshakePkg(mPlcRateInfo, mSendPkg, jnPkgIndex, jnStationId, jnBaudRate);

    /*如果失败，或者返回数据为空则返回*/
    if(!bSuccess) return false;

    /*传值给上位*/
    dealJniObj.setSendPropToJni(env, joSendData, mSendPkg);

    return bSuccess;
}

/******************************************************************
 * Function: 获得当前通信的地址
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getCurrAddrJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo, jobject joAddr)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joPlcInfo || 0 == joAddr) return false;

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的读数据信息*/
    ADDR_TARGET_PROP mAddr;
    bool bSuccess = manageObj.getCurrAddr(mPlcRateInfo, mAddr);

    /*传值给上位*/
    if(bSuccess)
    {
        dealJniObj.setAddrProp(env, joAddr, mAddr);
    }

    return bSuccess;
}

/******************************************************************
 * Function: 设置读得到的数据
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setReadDataJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jint jeCmnTyoe, jobject joPlcInfo, jobject joNoticInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*获取发送数据集合*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*获得发送方式*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*设置读得到的数据*/
    bool bChange = false;
    jint nResultInfo = manageObj.checkReadData(pszStr, len, jeReturnLen, nCurrTimes, eCmnType, mPlcRateInfo, bChange);

    /*值改变，通知上位*/
    if(bChange)
    {
        dealJniObj.setPlcNoticValue(env, joNoticInfo, true);
    }

    return nResultInfo;
}

/******************************************************************
 * Function: 校验写数据是否成功
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_checkWriteDataJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jobject joPlcInfo, jobject joNoticInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*获取发送数据集合*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*校验写数据是否成功*/
    bool bChange = false;
    jint nResultInfo = manageObj.checkWriteData(pszStr, len, jeReturnLen, nCurrTimes, mPlcRateInfo, bChange);

    /*值改变，通知上位*/
    if(bChange)
    {
        dealJniObj.setPlcNoticValue(env, joNoticInfo, true);
    }

    return nResultInfo;
}

/******************************************************************
 * Function: 对握手协议的返回数据校验
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_checkHandshakePkgJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jeReturnLen, jint nCurrTimes, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*获取发送数据集合*/
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

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*校验写数据是否成功*/
    jint nResultInfo = manageObj.checkHandshakePkg(pszStr,len, jeReturnLen, nCurrTimes, mPlcRateInfo);

    /*释放内存*/
    if(joReadStr != 0 && pszStr != 0)
    {
        env->ReleaseByteArrayElements(joReadStr, (jbyte *)pszStr,0);
    }
    return nResultInfo;
}

/******************************************************************
 * Function: 设置通信失败标志
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setCmnInfoJni
        (JNIEnv *env, jobject thiz, jint jnCmnInfo, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joPlcInfo) return false;

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*校验写数据是否成功*/
    bool bSuccess = manageObj.setCmnInfo(jnCmnInfo, mPlcRateInfo);

    /*释放内存*/
    return bSuccess;
}

/******************************************************************
 * Function: 接收从串口接收到的数据到从站处理，只适合从站
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_rcvStrForSlaveJni
        (JNIEnv *env, jobject thiz, jbyteArray joReadStr, jint jnSetSize, jint jnStationId, jobject joSendData, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joReadStr || 0 == joPlcInfo || 0 == joSendData) return OTHER_ERROR_CODE;

    /*获取发送数据集合*/
    int len = env->GetArrayLength(joReadStr);
    if(len <= 0) return RCV_DATA_FAILED;

    char pszStr[len];
    env->GetByteArrayRegion(joReadStr, 0, len, (jbyte *)pszStr);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*对发送结构体转换*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*处理从站接收到的数据*/
    jint nResultInfo = manageObj.rcvStrForSlave(pszStr, len, jnStationId, mPlcRateInfo, mSendPkgList);

    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    mSendPkgList.clear();
    vector<SEND_PACKAGE >().swap(mSendPkgList);
    return nResultInfo;
}

/******************************************************************
 * Function: 设置读地址集合
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setOneSceneAddrsJni
        (JNIEnv *env, jobject thiz, jobjectArray joAddrList, jboolean jbCover, jobject joPlcInfo,jint jMaxRWlen)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddrList || 0 == joPlcInfo) return false;

    /*初始化装地址的容器*/
    vector<ADDR_TARGET_PROP> addrList;
    addrList.clear();

    /*一个个读地址*/
    ADDR_TARGET_PROP mTmpAddr;
    int len = env->GetArrayLength(joAddrList);
    for(int i = 0; i < len; i++)
    {
        jobject joAddr = env->GetObjectArrayElement(joAddrList, i);

        /*对地址进行转换*/
        mTmpAddr = dealJniObj.getAddrPropFromJni(env,joAddr);
        addrList.push_back(mTmpAddr);

        /*删除缓存*/
        env->DeleteLocalRef(joAddr);
    }

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*设置读地址集合*/
    manageObj.setAllReadAddrs(addrList, jbCover, mPlcRateInfo,jMaxRWlen);
    return true;
}

/******************************************************************
 * Function: 清除所有的读地址
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
 * Function: 对地址进行整合
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_sortOutAddrListJni
        (JNIEnv *env, jobject thiz, jobjectArray joAddrList, jobject joAddrListObj, jobject joPlcInfo, jint jnMaxRWlen, jboolean jbWriteAddr)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddrList || 0 == joPlcInfo) return false;

    /*初始化装地址的容器*/
    vector<ADDR_TARGET_PROP> addrList;
    addrList.clear();

    vector<ADDR_TARGET_PROP> mSortAddrList;
    mSortAddrList.clear();

    /*一个个读地址*/
    ADDR_TARGET_PROP mTmpAddr;
    int len = env->GetArrayLength(joAddrList);
    for(int i = 0; i < len; i++)
    {
        jobject joAddr = env->GetObjectArrayElement(joAddrList, i);

        /*对地址进行转换*/
        mTmpAddr = dealJniObj.getAddrPropFromJni(env,joAddr);
        addrList.push_back(mTmpAddr);

        /*删除缓存*/
        env->DeleteLocalRef(joAddr);
    }

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*设置读地址集合*/
    bool bSuccess = manageObj.sortOutAddrList(addrList, mSortAddrList, mPlcRateInfo, jnMaxRWlen, jbWriteAddr);

    if(bSuccess)
    {
        dealJniObj.setAddrListToJni(env, joAddrListObj, mSortAddrList);
    }
    return bSuccess;
}

/******************************************************************
 * Function: 对写地址进行打包
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_makeWritePackageJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jint jnSetSize, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joSendData || 0 == jeCmnTyoe || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    /*获取发送数据集合*/
    int len = env->GetArrayLength(jDataList);
    if(len <= 0) return false;

    char pDataList[len];
    env->GetByteArrayRegion(jDataList, 0, len, (jbyte *)pDataList);
    if(len > jnSetSize)
    {
        len = jnSetSize;
    }

    /*先把值存到tmpList中*/
    vector<uchar > tmpList;
    tmpList.clear();
    uchar nTmpValue = 0;
    for(int i = 0; i < len; i++)
    {
        nTmpValue = pDataList[i];
        tmpList.push_back(nTmpValue);
    }

    /*对发送结构体转换*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*获得发送方式*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的写数据信息*/
    jboolean bSuccess = manageObj.makeWritePackage(mAddrProp, tmpList, mSendPkgList, mPlcRateInfo, eCmnType);

    /*如果失败，或者返回数据为空则返回*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*传值给上位*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    return bSuccess;
}

/******************************************************************
 * Function: 对读地址进行打包
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_makeReadPackageJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobject joSendData, jint jeCmnTyoe, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == joSendData || 0 == jeCmnTyoe || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    /*对发送结构体转换*/
    vector<SEND_PACKAGE > mSendPkgList;
    mSendPkgList.clear();

    /*获得发送方式*/
    CMN_SEND_TYPE eCmnType = dealJniObj.getCmnSendType(jeCmnTyoe);

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*获得打包后的写数据信息*/
    jboolean bSuccess = manageObj.makeReadPackage(mAddrProp, mSendPkgList, mPlcRateInfo, eCmnType);

    /*如果失败，或者返回数据为空则返回*/
    if(!bSuccess || mSendPkgList.empty()) return false;

    /*传值给上位*/
    dealJniObj.setSendPkgList(env, joSendData, mSendPkgList);

    return bSuccess;
}


/******************************************************************
 * Function: 获得Bytes类型的数据接口
 * Parameters:
 * Return: 所有的寄存器名字
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getRegBytesDataJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jobject joPlcInfo, jobject joRcvProp)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joRcvProp || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env,joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("getRegBytesDataJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);

    /*读取值*/
    vector<ADDR_BIT_VALUE_PROP> tmpList;
    bool bSuccess = manageObj.getReadRegDataList(mAddrProp, tmpList, mPlcRateInfo);
    if(!bSuccess)
    {
        /*内存释放*/
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

    /*设置状态标志符*/
    dealJniObj.setErrorInfo(env, joRcvProp, eRrror);
    env->SetByteArrayRegion(jDataList, 0, nListSize, jbBuff);

    /*内存释放*/
    tmpList.clear();
    vector<ADDR_BIT_VALUE_PROP>().swap(tmpList);

    return true;
}


/******************************************************************
 * Function: 设置 bytes 类型的数据接口
 * Parameters:
 * Return: 所有的寄存器名字
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setRegBytesDataJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jbyteArray jDataList, jobject joPlcInfo, jint jnRwType, jint jnSimulateType)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == jDataList || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
    ADDR_TARGET_PROP mAddrProp = dealJniObj.getAddrPropFromJni(env, joAddr);

    if(mAddrProp.nAddrValue + mAddrProp.nAddrLen > MAXADDR
       || mAddrProp.nAddrValue < 0)
    {
        LOGE("setRegBytesDataJni nAddrValue value is: %d, nAddrlen is: %d",
             mAddrProp.nAddrValue,mAddrProp.nAddrLen);
        return false;
    }

    /*获取发送数据集合*/
    int len = env->GetArrayLength(jDataList);
    if(len <= 0) return false;

    char pDataList[len];
    env->GetByteArrayRegion(jDataList, 0, len, (jbyte *)pDataList);

    /*先把值存到tmpList中*/
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
        /*对PLC信息转换*/
        PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
        mAddrProp.eAddrRWprop = (READ_WRITE_COM_TYPE)jnRwType;
        manageObj.setWriteRegDataList(mAddrProp, tmpList, mPlcRateInfo, jnSimulateType);
    }

    return true;
}

/******************************************************************
 * Function: 获得string类型的数据接口
 * Parameters:
 * Return: 所有的寄存器名字
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getLocalStrJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobject jStrList, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == jStrList || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
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

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    bool bSuccess = manageObj.getReadRegStrList(mAddrProp, tmpList, mPlcRateInfo);

    if(bSuccess)
    {
        dealJniObj.setStrListToObjArray(env, jStrList, tmpList);
    }

    /*释放内存*/
    tmpList.clear();
    vector<string >().swap(tmpList);

    return bSuccess;
}

/******************************************************************
 * Function: 设置string类型的数据接口
 * Parameters:
 * Return: 所有的寄存器名字
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_setLocalStrJni
        (JNIEnv *env, jobject thiz, jobject joAddr, jobjectArray jStrList, jint jnSetSize, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joAddr || 0 == jStrList || 0 == joPlcInfo) return false;

    /*获取地址结构体*/
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

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    bool bSuccess = manageObj.setWriteRegStrList(mAddrProp, tmpList, mPlcRateInfo);

    return bSuccess;
}

/******************************************************************
 * Function: 获得驱动的类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jint JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_getProtocolFunTypeJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joPlcInfo) return OTHER_ERROR_CODE;

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    return manageObj.getProtocolFunType(mPlcRateInfo);
}

/******************************************************************
 * Function: 关闭一个驱动
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_closeProtocolJni
        (JNIEnv *env, jobject thiz, jobject joPlcInfo)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == joPlcInfo) return false;

    /*对PLC信息转换*/
    PLC_RATE_INFO mPlcRateInfo = dealJniObj.getPlcRateInfo(env, joPlcInfo);
    return manageObj.closeProtocolSo(mPlcRateInfo);
}

/******************************************************************
 * Function: 关闭所有驱动
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_closeAllProtocolJni
        (JNIEnv *env, jobject thiz)
{
    return manageObj.closeAllSo();
}

/******************************************************************
 * Function: bytes 转short数组类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToShortsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jshortArray jshorts)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jshorts) return false;

    /*获取发送数据集合*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*获得short类型数组*/
    int nTargetLen = env->GetArrayLength(jshorts);
    if(nTargetLen > nByteLen/2)
    {
        nTargetLen = nByteLen/2;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*设置数值*/
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
 * Function: bytes 转 unsigned short 类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToUShortsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jintArray jushorts)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jushorts) return false;

    /*获取发送数据集合*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*获得short类型数组*/
    int nTargetLen = env->GetArrayLength(jushorts);
    if(nTargetLen > nByteLen/2)
    {
        nTargetLen = nByteLen/2;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*设置数值*/
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
 * Function: bytes 转 int 类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToIntsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize,jintArray jints)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jints) return false;

    /*获取发送数据集合*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*获得short类型数组*/
    int nTargetLen = env->GetArrayLength(jints);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*设置数值*/
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
 * Function: bytes 转 unsigned int 类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToUIntsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jlongArray juints)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == juints) return false;

    /*获取发送数据集合*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*获得short类型数组*/
    int nTargetLen = env->GetArrayLength(juints);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*设置数值*/
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
 * Function: bytes 转 float 数组类型
 * Parameters:
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_bytesToFloatsJni
        (JNIEnv *env, jobject thiz,jbyteArray jbytes, jint jnSetSize, jfloatArray jfloats)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jfloats) return false;

    /*获取发送数据集合*/
    int nByteLen = env->GetArrayLength(jbytes);
    if(nByteLen <= 0) return false;

    char pValue[nByteLen];
    env->GetByteArrayRegion(jbytes, 0, nByteLen, (jbyte *)pValue);
    if(nByteLen > jnSetSize)
    {
        nByteLen = jnSetSize;
    }

    /*获得short类型数组*/
    int nTargetLen = env->GetArrayLength(jfloats);
    if(nTargetLen > nByteLen/4)
    {
        nTargetLen = nByteLen/4;
    }

    if(nTargetLen <= 0)
    {
        return false;
    }

    /*设置数值*/
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
 * Function: ascii 转 bytes 数组类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * jInterval:转换间隔，一般ascii使用1，unicode使用2
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_asciiToBytesJni
        (JNIEnv *env, jobject thiz, jbyteArray jbytes, jint jInterval, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes) return false;

    /*获得short类型数组*/
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
        /*拷贝值*/
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
 * Function: short 转 bytes 数组类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_shortsToBytesJni
        (JNIEnv *env, jobject thiz, jshortArray jshorts, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jshorts) return false;

    /*获取 short 数组长度*/
    int nSourceLen = env->GetArrayLength(jshorts);
    jshort pValue[nSourceLen];
    env->GetShortArrayRegion(jshorts, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*获得 byte 类型数组*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *2 > nTargetLen )
    {
        nSourceLen = nTargetLen/2;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*拷贝值*/
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
 * Function: unsigned short 转 bytes 类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_uShortsToBytesJni
        (JNIEnv *env, jobject thiz, jintArray jushorts, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jushorts) return false;

    /*获取 short 数组长度*/
    int nSourceLen = env->GetArrayLength(jushorts);
    jint pValue[nSourceLen];
    env->GetIntArrayRegion(jushorts, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*获得 byte 类型数组*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *2 > nTargetLen )
    {
        nSourceLen = nTargetLen/2;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*拷贝值*/
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
 * Function: int 转 bytes 类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_intsToBytesJni
        (JNIEnv *env, jobject thiz, jintArray jints, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jints) return false;

    /*获取 short 数组长度*/
    int nSourceLen = env->GetArrayLength(jints);
    jint pValue[nSourceLen];
    env->GetIntArrayRegion(jints, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*获得 byte 类型数组*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*拷贝值*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*高字节在前*/
        int nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*先存低8位*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*先存低8位*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*低字节在前*/
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
 * Function: unsigned int 转 bytes 类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_uIntsToBytesJni
        (JNIEnv *env, jobject thiz, jlongArray juints, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == juints) return false;

    /*获取 long 数组长度*/
    int nSourceLen = env->GetArrayLength(juints);
    jlong pValue[nSourceLen];
    env->GetLongArrayRegion(juints, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*获得 byte 类型数组*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*拷贝值*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*高字节在前*/
        uint nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*先存低8位*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*先存低8位*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*低字节在前*/
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
 * Function: float 转 bytes 数组类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jboolean JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_floatsToBytesJni
        (JNIEnv *env, jobject thiz, jfloatArray jfloats, jint jnSetSize, jbyteArray jbytes, jint jnByteHLPos)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jbytes || 0 == jfloats) return false;

    /*获取 long 数组长度*/
    int nSourceLen = env->GetArrayLength(jfloats);
    jfloat pValue[nSourceLen];
    env->GetFloatArrayRegion(jfloats, 0, nSourceLen, pValue);
    if(nSourceLen > jnSetSize)
    {
        nSourceLen = jnSetSize;
    }

    /*获得 byte 类型数组*/
    int nTargetLen = env->GetArrayLength(jbytes);
    if(nSourceLen *4 > nTargetLen )
    {
        nSourceLen = nTargetLen/4;
    }

    if(nSourceLen <= 0)
    {
        return false;
    }

    /*拷贝值*/
    char pTarge[nSourceLen*4];
    if(jnByteHLPos == 2)
    {
        /*高字节在前*/
        float nTmpValue = 0;
        memset(pTarge,0, nSourceLen*4);
        for(int i = 0; i < nSourceLen; i++)
        {
            nTmpValue = pValue[i];

            /*先存低8位*/
            memcpy((pTarge + 4*i + 0), (uchar *)(&nTmpValue) + 2, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 1), (uchar *)(&nTmpValue) + 3, 1);

            /*先存低8位*/
            memcpy((pTarge + 4*i + 2), (uchar *)(&nTmpValue) + 0, 1);

            /*后存高8位*/
            memcpy((pTarge + 4*i + 3), (uchar *)(&nTmpValue) + 1, 1);
        }
    }
    else
    {
        /*低字节在前*/
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
 * Function: double 转 bytes 数组类型
 * Parameters:jnByteHLPos : 高地位设置，1：低字节在前，2：高字节在前
 * Return: true or false
 ******************************************************************/
DLL_EXPORT jbyteArray JNICALL Java_com_android_Samkoonhmi_plccommunicate_ProtocolInterfaces_doublesToBytesJni
        (JNIEnv *env, jobject thiz, jdoubleArray jdoubles, jobject joSendProp)
{
    /*检查对象是否为空，为空返回失败*/
    if(0 == env || 0 == jdoubles || 0 == joSendProp) return 0;

    /*获取发送结构体*/
    SEND_DATA_STRUCT mSendProp = dealJniObj.getSendStructFromJni(env,joSendProp);

    /*获取byte数组长度*/
    int nSourceLen = env->GetArrayLength(jdoubles);
    if(nSourceLen <= 0)
    {
        return 0;
    }

    jdouble pValue[nSourceLen];
    env->GetDoubleArrayRegion(jdoubles, 0, nSourceLen, pValue);

    /*拷贝值*/
    char nTmpChar = 0;
    switch(mSendProp.nDataType)
    {
    case BIT_1:                      // 位
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
    case INT_16:                     // 16位正整数
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*2);
            jbyte bytes[nSourceLen *2];

            /*拷贝值*/
            short nTmpValue = 0;
            for(int i = 0; i < nSourceLen; i++)
            {
                nTmpValue = pValue[i];

                /*先存低8位*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                bytes[2*i + 0] = nTmpChar;

                /*后存高8位*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                bytes[2*i + 1] = nTmpChar;

            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *2, bytes);

            return nResultList;
            break;
        }
    case POSITIVE_INT_16:            // 16位整数
    case BCD_16:                     // 16位BCD码
    case HEX_16:                     // 16位16进制
    case OTC_16:                     // 16位八进制
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*2);
            jbyte bytes[nSourceLen *2];

            /*拷贝值*/
            ushort nTmpValue = 0;
            for(int i = 0; i < nSourceLen; i++)
            {
                nTmpValue = pValue[i];

                /*先存低8位*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                bytes[2*i + 0] = nTmpChar;

                /*后存高8位*/
                memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                bytes[2*i + 1] = nTmpChar;

            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *2, bytes);

            return nResultList;
            break;
        }
    case INT_32:                     // 32位整数
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*拷贝值*/
            int nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*高字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*低字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *4, bytes);

            return nResultList;
            break;
        }
    case POSITIVE_INT_32:            // 32位正整数
    case BCD_32:                     // 32位BCD码
    case HEX_32:                     //32位16进制
    case OTC_32:                     //32位八进制
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*拷贝值*/
            uint nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*高字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*低字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }

            env->SetByteArrayRegion(nResultList, 0, nSourceLen *4, bytes);

            return nResultList;
            break;
        }
    case FLOAT_32:                   // 32位浮点数
        {
            jbyteArray nResultList = env->NewByteArray(nSourceLen*4);
            jbyte bytes[nSourceLen *4];

            /*拷贝值*/
            float nTmpValue = 0;
            if(mSendProp.nByteHLPos == H_BYTE_FIRST)
            {
                /*高字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = pValue[i];

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 3, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 3] = nTmpChar;
                }
            }
            else
            {
                /*低字节在前*/
                for(int i = 0; i < nSourceLen; i++)
                {
                    nTmpValue = *(pValue + i);

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 0, 1);
                    bytes[4*i + 0] = nTmpChar;

                    /*后存高8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 1, 1);
                    bytes[4*i + 1] = nTmpChar;

                    /*先存低8位*/
                    memcpy(&nTmpChar, (uchar *)(&nTmpValue) + 2, 1);
                    bytes[4*i + 2] = nTmpChar;

                    /*后存高8位*/
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
 * Function: int 转short类型
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
 * Function: int 转 float 类型
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
 * Function: int 转 unsigned short 类型
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
 * Function: int 转 unsigned int 类型
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
