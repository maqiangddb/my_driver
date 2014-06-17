#include "dealjnimethod.h"

DealJniMethod::DealJniMethod()
{
    m_jfnUserPlcId = 0;
    m_jfnRegIndex = 0;
    m_jfnAddrLen = 0;
    m_jfnPlcStationIndex = 0;
    m_jfnAddrValue = 0;
    m_jfeAddrRWprop = 0;

    m_jfnDataType = 0;
    m_jfnRWCtlType = 0;
    m_jfnByteHLPos = 0;

    m_jfnReturnLen = 0;
    m_jfeSendType = 0;

    m_jfeConnectType = 0;
    m_jfnProtocolIndex = 0;
    m_jfnSampRate = 0;
    m_jfsProtocolName = 0;
}

/*****************************以下是本地的一些方法****************************/
/******************************************************************
 * Function: 转换地址结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
ADDR_TARGET_PROP DealJniMethod::getAddrPropFromJni(JNIEnv *env, jobject jAddr)
{
    /*获取地址结构体*/
    ADDR_TARGET_PROP mAddrProp;
    mAddrProp.nUserPlcId = 0;
    mAddrProp.nAddrLen = 0;
    mAddrProp.nAddrValue = 0;
    mAddrProp.nPlcStationIndex = 0;
    mAddrProp.nRegIndex = 0;

    if(0 == env || 0 == jAddr) return mAddrProp;

    /*获取Java中的实例类AddrProp*/
    jclass m_jAddrClass = env->FindClass("com/android/Samkoonhmi/util/AddrProp");
    if(0 == m_jAddrClass) return mAddrProp;

    /*get short nUserPlcId;*/
    if(0 == m_jfnUserPlcId)
    {
        m_jfnUserPlcId = env->GetFieldID(m_jAddrClass, "nUserPlcId", "S");
    }
    mAddrProp.nUserPlcId = env->GetShortField(jAddr, m_jfnUserPlcId);

    /*get short nPlcRegIndex;*/
    if(0 == m_jfnRegIndex)
    {
        m_jfnRegIndex = env->GetFieldID(m_jAddrClass, "nRegIndex", "S");
    }
    mAddrProp.nRegIndex = env->GetShortField(jAddr, m_jfnRegIndex);

    /*get int nAddrLen;*/
    if(0 == m_jfnAddrLen)
    {
        m_jfnAddrLen = env->GetFieldID(m_jAddrClass, "nAddrLen", "I");
    }
    mAddrProp.nAddrLen = env->GetIntField(jAddr, m_jfnAddrLen);

    /*get int nPlcStationIndex;*/
    if(0 == m_jfnPlcStationIndex)
    {
        m_jfnPlcStationIndex = env->GetFieldID(m_jAddrClass, "nPlcStationIndex", "I");
    }
    mAddrProp.nPlcStationIndex = env->GetIntField(jAddr, m_jfnPlcStationIndex);

    /*get int nPlcStartAddr;*/
    if(0 == m_jfnAddrValue)
    {
        m_jfnAddrValue = env->GetFieldID(m_jAddrClass, "nAddrValue", "I");
    }
    mAddrProp.nAddrValue = env->GetIntField(jAddr, m_jfnAddrValue);

    /*get int nAddrLen;*/
    if(0 == m_jfeAddrRWprop)
    {
        m_jfeAddrRWprop = env->GetFieldID(m_jAddrClass, "eAddrRWprop", "I");
    }
    mAddrProp.eAddrRWprop = (READ_WRITE_COM_TYPE)env->GetIntField(jAddr, m_jfeAddrRWprop);

    env->DeleteLocalRef(m_jAddrClass);

    return mAddrProp;
}

/******************************************************************
 * Function: 转换发送数据结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
SEND_DATA_STRUCT DealJniMethod::getSendStructFromJni(JNIEnv *env,jobject jSend)
{
    /*获取地址结构体*/
    SEND_DATA_STRUCT mSendProp;
    mSendProp.nByteHLPos = OTHER_H_L_POS;
    mSendProp.nDataType = OTHER_DATA_TYPE;
    mSendProp.nReadWriteCtlType = OTHER_CONTROL_TYPE;

    if(env == 0 || jSend == 0) return mSendProp;

    /*获取Java中的实例类AddrProp*/
    jclass m_jSendDataClass = env->FindClass("com/android/Samkoonhmi/util/SEND_DATA_STRUCT_JNI");
    if(0 == m_jSendDataClass) return mSendProp;

    /*get short nDataType*/
    if(0 == m_jfnDataType)
    {
        m_jfnDataType = env->GetFieldID(m_jSendDataClass, "nDataType", "S");
    }
    mSendProp.nDataType = (DATA_TYPE)env->GetShortField(jSend, m_jfnDataType);

    /*get short nReadWriteCtlType;*/
    if(0 == m_jfnRWCtlType)
    {
        m_jfnRWCtlType = env->GetFieldID(m_jSendDataClass, "nReadWriteCtlType", "S");
    }
    mSendProp.nReadWriteCtlType = (READ_WRITE_COM_TYPE)env->GetShortField(jSend, m_jfnRWCtlType);

    /*get short nByteHLPos;*/
    if(0 == m_jfnByteHLPos)
    {
        m_jfnByteHLPos = env->GetFieldID(m_jSendDataClass, "nByteHLPos", "S");
    }
    mSendProp.nByteHLPos = (BYTE_HL_POS)env->GetShortField(jSend, m_jfnByteHLPos);

    env->DeleteLocalRef(m_jSendDataClass);
    return mSendProp;
}

/******************************************************************
 * Function: 转换发送字符串（到串口的）结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
SEND_PACKAGE DealJniMethod::getSendPropFromJni(JNIEnv *env, jobject jSend)
{
    /*获取地址结构体*/
    SEND_PACKAGE mSendProp;
    mSendProp.eSendType = CMN_OTHER_TYPE;
    mSendProp.nReturnLen = 0;
    mSendProp.sendDataList.clear();

    if(env == 0 || jSend == 0) return mSendProp;

    /*获取Java中的实例类SEND_PACKAGE_JNI*/
    jclass m_jSendPkgClass = env->FindClass("com/android/Samkoonhmi/util/SEND_PACKAGE_JNI");
    if(0 == m_jSendPkgClass) return mSendProp;

    /*get int nReturnLen;*/
    if(0 == m_jfnReturnLen)
    {
        m_jfnReturnLen = env->GetFieldID(m_jSendPkgClass, "nReturnLen", "I");
    }
    mSendProp.nReturnLen = env->GetIntField(jSend, m_jfnReturnLen);

    /*get int eSendType;*/
    if(0 == m_jfeSendType)
    {
        m_jfeSendType = env->GetFieldID(m_jSendPkgClass, "eSendType", "I");
    }
    mSendProp.eSendType = getCmnSendType(env->GetIntField(jSend, m_jfeSendType));

    /*获取发送数据集合*/
    mSendProp.sendDataList.clear();

#if 0
    /*get int sSendDataList;*/
    jfieldID josSendDataList = env->GetFieldID(m_jSendPkgClass, "sSendDataList", "[B");
    jbyteArray jDataList = (jbyteArray)env->GetObjectField(jSend, josSendDataList);

    /*获取发送数据集合*/
    if(jDataList != 0)
    {
        int nByteLen = env->GetArrayLength(jDataList);
        jbyte jbBuff[nByteLen];
        env->GetByteArrayRegion(jDataList, 0, nByteLen, jbBuff);

        for(int i = 0; i < nByteLen; i++)
        {
            mSendProp.sendDataList.push_back(jbBuff[i]);
        }
        env->DeleteLocalRef(jDataList);
    }
#endif

    env->DeleteLocalRef(m_jSendPkgClass);
    return mSendProp;
}

/******************************************************************
 * Function: 获得PLC采集的唯一信息
 * Parameters:
 * Return: true or false
 ******************************************************************/
PLC_RATE_INFO DealJniMethod::getPlcRateInfo(JNIEnv *env, jobject joPlcRate)
{
    PLC_RATE_INFO mPlcRateInfo;
    mPlcRateInfo.eConnectType = OTHER;
    mPlcRateInfo.nProtocolIndex = -1;
    mPlcRateInfo.nSampRate = 1000;

    if(env == 0 || joPlcRate == 0) return mPlcRateInfo;

    /*获取Java中的实例类PlcSampInfo*/
    jclass m_jPlcInfoClass = env->FindClass("com/android/Samkoonhmi/util/PlcSampInfo");
    if(0 == m_jPlcInfoClass) return mPlcRateInfo;

    /*get short eConnectType;*/
    if(0 == m_jfeConnectType)
    {
        m_jfeConnectType = env->GetFieldID(m_jPlcInfoClass, "eConnectType", "I");
    }
    mPlcRateInfo.eConnectType = (CONNECT_TYPE)env->GetIntField(joPlcRate, m_jfeConnectType);

    /*get int nProtocolIndex;*/
    if(0 == m_jfnProtocolIndex)
    {
        m_jfnProtocolIndex = env->GetFieldID(m_jPlcInfoClass, "nProtocolIndex", "I");
    }
    mPlcRateInfo.nProtocolIndex = env->GetIntField(joPlcRate, m_jfnProtocolIndex);

    /*get int nSampRate;*/
    if(0 == m_jfnSampRate)
    {
        m_jfnSampRate = env->GetFieldID(m_jPlcInfoClass, "nSampRate", "I");
    }
    mPlcRateInfo.nSampRate = env->GetIntField(joPlcRate, m_jfnSampRate);

    /*get int sProtocolName;*/
    if(0 == m_jfsProtocolName)
    {
        m_jfsProtocolName = env->GetFieldID(m_jPlcInfoClass, "sProtocolName", "Ljava/lang/String;");
    }
    jstring jstrn = (jstring)env->GetObjectField(joPlcRate, m_jfsProtocolName);
    if(jstrn != 0)
    {
        int nLen = env->GetStringLength(jstrn);
        if(nLen > 0)
        {
            char nTmpBuff[nLen];
            env->GetStringUTFRegion(jstrn, 0, nLen, nTmpBuff);
            mPlcRateInfo.sProtocolName = nTmpBuff;
        }
        else
        {
            mPlcRateInfo.sProtocolName = "";
        }
        env->DeleteLocalRef(jstrn);
    }

    env->DeleteLocalRef(m_jPlcInfoClass);

    return mPlcRateInfo;
}

/******************************************************************
 * Function: 转换地址结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setAddrProp(JNIEnv *env, jobject jAddr, ADDR_TARGET_PROP mAddrProp)
{
    if(env == 0 || jAddr == 0) return false;

    /*获取Java中的实例类AddrProp*/
    jclass m_jAddrClass = env->FindClass("com/android/Samkoonhmi/util/AddrProp");
    if(0 == m_jAddrClass) return false;

    /*get short nUserPlcId;*/
    if(0 == m_jfnUserPlcId)
    {
        m_jfnUserPlcId = env->GetFieldID(m_jAddrClass, "nUserPlcId", "S");
    }
    env->SetShortField(jAddr, m_jfnUserPlcId, mAddrProp.nUserPlcId);

    /*get short nPlcRegIndex;*/
    if(0 == m_jfnRegIndex)
    {
        m_jfnRegIndex = env->GetFieldID(m_jAddrClass, "nRegIndex", "S");
    }
    env->SetShortField(jAddr, m_jfnRegIndex, mAddrProp.nRegIndex);

    /*get int nAddrLen;*/
    if(0 == m_jfnAddrLen)
    {
        m_jfnAddrLen = env->GetFieldID(m_jAddrClass, "nAddrLen", "I");
    }
    env->SetIntField(jAddr, m_jfnAddrLen, mAddrProp.nAddrLen);

    /*get int nPlcStationIndex;*/
    if(0 == m_jfnPlcStationIndex)
    {
        m_jfnPlcStationIndex = env->GetFieldID(m_jAddrClass, "nPlcStationIndex", "I");
    }
    env->SetIntField(jAddr, m_jfnPlcStationIndex, mAddrProp.nPlcStationIndex);

    /*get int nPlcStartAddr;*/
    if(0 == m_jfnAddrValue)
    {
        m_jfnAddrValue = env->GetFieldID(m_jAddrClass, "nAddrValue", "I");
    }
    env->SetIntField(jAddr, m_jfnAddrValue, mAddrProp.nAddrValue);

    /*get int nAddrLen;*/
    if(0 == m_jfeAddrRWprop)
    {
        m_jfeAddrRWprop = env->GetFieldID(m_jAddrClass, "eAddrRWprop", "I");
    }
    env->SetIntField(jAddr, m_jfeAddrRWprop, (int)mAddrProp.eAddrRWprop);

    env->DeleteLocalRef(m_jAddrClass);
    return true;
}

/******************************************************************
 * Function: 传递地址集合到java
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setAddrListToJni(JNIEnv *env, jobject joAddrListObj, vector<ADDR_TARGET_PROP > sourceList)
{
    if(env == 0 || joAddrListObj == 0 || sourceList.empty()) return false;

    /*获得地址集合对象的类*/
    jclass m_joAddrArrayClass = env->FindClass("com/android/Samkoonhmi/util/AddrPropArray");
    if(0 == m_joAddrArrayClass) return false;

    jfieldID joSendArrayFiled = env->GetFieldID(m_joAddrArrayClass, "mSortAddrList", "[Lcom/android/Samkoonhmi/util/AddrProp;");

    /*获取Java中的实例类AddrProp*/
    jclass m_jAddrClass = env->FindClass("com/android/Samkoonhmi/util/AddrProp");
    if(0 == m_jAddrClass)
    {
        env->DeleteLocalRef(m_joAddrArrayClass);
        return false;
    }

    /*分配容器内存*/
    int nSize = sourceList.size();
    jobjectArray joArrayList = env->NewObjectArray(nSize, m_jAddrClass, 0);
    if(joArrayList != 0)
    {
        jobject joAddr = 0;
        for(int i = 0; i < nSize; i++)
        {
            /*创建对象*/
            joAddr = env->AllocObject(m_jAddrClass);
            if(joAddr != 0)
            {
                /*设置值*/
                setAddrProp(env, joAddr, sourceList.at(i));
                env->SetObjectArrayElement(joArrayList, i, joAddr);
                env->DeleteLocalRef(joAddr);
            }
        }

        /*设置值到对象去*/
        env->SetObjectField(joAddrListObj, joSendArrayFiled, joArrayList);
        env->DeleteLocalRef(joArrayList);
    }

    env->DeleteLocalRef(m_joAddrArrayClass);
    env->DeleteLocalRef(m_jAddrClass);

    return true;
}

/******************************************************************
 * Function: 设置发送字符串（到串口的）结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setSendPropToJni(JNIEnv *env, jobject joSend, SEND_PACKAGE source)
{
    if(env == 0 || joSend == 0 || source.sendDataList.empty()) return false;

    /*获取Java中的实例类SEND_PACKAGE_JNI*/
    jclass m_jSendPkgClass = env->FindClass("com/android/Samkoonhmi/util/SEND_PACKAGE_JNI");
    if(0 == m_jSendPkgClass) return false;

    /*get int nReturnLen;*/
    if(0 == m_jfnReturnLen)
    {
        m_jfnReturnLen = env->GetFieldID(m_jSendPkgClass, "nReturnLen", "I");
    }
    env->SetIntField(joSend, m_jfnReturnLen, source.nReturnLen);

    /*get int eSendType;*/
    if(0 == m_jfeSendType)
    {
        m_jfeSendType = env->GetFieldID(m_jSendPkgClass, "eSendType", "I");
    }
    env->SetIntField(joSend, m_jfeSendType, getCmnSendFromType(source.eSendType));

    /*get int sSendDataList;*/
    if(0 == m_jfsSendDataList)
    {
        m_jfsSendDataList = env->GetFieldID(m_jSendPkgClass, "sSendDataList", "[B");
    }

    /*转换成数值jbyteArray*/
    int nSize = source.sendDataList.size();
    if(nSize > 0)
    {
        jbyteArray sSendDataList = env->NewByteArray(nSize);
        if(sSendDataList != 0)
        {
            jbyte jbBuff[nSize];
            for(int i = 0; i < nSize; i++)
            {
                jbBuff[i] = source.sendDataList.at(i);
            }

            /*设置值到容器*/
            env->SetByteArrayRegion(sSendDataList, 0, nSize, jbBuff);
            env->SetObjectField(joSend, m_jfsSendDataList, sSendDataList);

            env->DeleteLocalRef(sSendDataList);
        }

        /*释放内存*/
        source.sendDataList.clear();
        vector<uchar >().swap(source.sendDataList);
    }

    env->DeleteLocalRef(m_jSendPkgClass);

    return true;
}

/******************************************************************
 * Function: 设置发送字符串（到串口的）结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setSendPkgList(JNIEnv *env, jobject joSendData, vector<SEND_PACKAGE > sourceList)
{
    if(env == 0 || joSendData == 0 || sourceList.empty()) return false;

    /*查找装载容器对象的类*/
    jclass m_jSendPkgArrayClass = env->FindClass("com/android/Samkoonhmi/util/SendPkgArray");
    if(0 == m_jSendPkgArrayClass) return false;

    jfieldID joSendArrayFiled = env->GetFieldID(m_jSendPkgArrayClass, "mSendPkgList", "[Lcom/android/Samkoonhmi/util/SEND_PACKAGE_JNI;");

    int nSize = sourceList.size();

    /*获取Java中的实例类SEND_PACKAGE_JNI*/
    jclass m_jSendPkgClass = env->FindClass("com/android/Samkoonhmi/util/SEND_PACKAGE_JNI");
    if(0 == m_jSendPkgClass)
    {
        env->DeleteLocalRef(m_jSendPkgArrayClass);
        return false;
    }

    /*分配容器内存*/
    jobjectArray joSendList = env->NewObjectArray(nSize, m_jSendPkgClass, 0);
    if(joSendList != 0)
    {
        jobject joSend = 0;
        for(int i = 0; i < nSize; i++)
        {
            /*创建对象*/
            joSend = env->AllocObject(m_jSendPkgClass);
            if(joSend != 0)
            {
                /*设置值*/
                setSendPropToJni(env, joSend, sourceList.at(i));
                env->SetObjectArrayElement(joSendList, i, joSend);
                env->DeleteLocalRef(joSend);
            }
        }

        /*设置值到对象去*/
        env->SetObjectField(joSendData, joSendArrayFiled, joSendList);
        env->DeleteLocalRef(joSendList);
    }
    env->DeleteLocalRef(m_jSendPkgArrayClass);
    env->DeleteLocalRef(m_jSendPkgClass);

    return true;
}

#if 0
/******************************************************************
 * Function: 设置通知值到应用层的结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setPlcNoticValue(JNIEnv *env, jobject joNoticInfo, ADDR_RW_STRUCT mAddrValueList)
{
    if(0 == env || 0 == joNoticInfo) return false;

    /*获取Java中的实例类AddrProp*/
    jclass jcPlcNoticClass = env->FindClass("com/android/Samkoonhmi/util/PlcNoticValue");
    if(0 == jcPlcNoticClass) return false;

    /*设置读写次数*/
    jfieldID jonUseTimes = env->GetFieldID(jcPlcNoticClass, "nUseTimes", "I");
    env->SetIntField(joNoticInfo, jonUseTimes, mAddrValueList.nUseTimes);

    /*转换成数值jbyteArray*/
    jfieldID josSendDataList = env->GetFieldID(jcPlcNoticClass, "nValueList", "[B");
    int nSize = mAddrValueList.nValueList.size();
    if(nSize > 0)
    {
        jbyteArray sSendDataList = env->NewByteArray(nSize);
        if(0 != sSendDataList)
        {
            jbyte jbBuff[nSize];
            for(int i = 0; i < nSize; i++)
            {
                jbBuff[i] = mAddrValueList.nValueList.at(i);
            }
            env->SetByteArrayRegion(sSendDataList, 0, nSize, jbBuff);
            env->SetObjectField(joNoticInfo, josSendDataList, sSendDataList);

            /*释放内存*/
            env->DeleteLocalRef(sSendDataList);
        }
    }

    /*存地址结构体*/
    jfieldID jomAddrPropField= env->GetFieldID(jcPlcNoticClass, "mAddrProp", "Lcom/android/Samkoonhmi/util/AddrProp;");

    /*获取Java中的实例类AddrProp*/
    jclass m_jAddrClass = env->FindClass("com/android/Samkoonhmi/util/AddrProp");

    /*创建对象*/
    jobject joAddr = env->AllocObject(m_jAddrClass);
    if(joAddr != 0)
    {
        /*设置地址结构体*/
        setAddrProp(env, joAddr, mAddrValueList.mAddrProp);
        env->SetObjectField(joNoticInfo, jomAddrPropField, joAddr);

        env->DeleteLocalRef(joAddr);
    }

    env->DeleteLocalRef(jcPlcNoticClass);
    env->DeleteLocalRef(m_jAddrClass);

    return true;
}
#else
/******************************************************************
 * Function: 设置通知值到应用层的结构体
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setPlcNoticValue(JNIEnv *env, jobject joNoticInfo, bool bRefreash)
{
    if(env == 0 || joNoticInfo == 0) return false;

    /*获取Java中的实例类RECEIVE_DATA_STRUCT*/
    jclass m_jcPlcNoticClass = env->FindClass("com/android/Samkoonhmi/util/PlcNoticValue");
    if(0 == m_jcPlcNoticClass) return false;

    /*get int nErrorCode;*/
    jfieldID jobRefreash = env->GetFieldID(m_jcPlcNoticClass, "bNeedRefreash", "Z");
    env->SetBooleanField(joNoticInfo, jobRefreash, bRefreash);

    env->DeleteLocalRef(m_jcPlcNoticClass);

    return true;
}
#endif

/******************************************************************
 * Function: 设置读取的数据信息
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setErrorInfo(JNIEnv *env, jobject joErrors, CMN_RW_ERROR_CODE eError)
{
    if(env == 0 || joErrors == 0) return false;

    /*获取Java中的实例类RECEIVE_DATA_STRUCT*/
    jclass m_jcErrorCodeClass = env->FindClass("com/android/Samkoonhmi/util/RECEIVE_DATA_STRUCT");
    if(0 == m_jcErrorCodeClass) return false;

    /*get int nErrorCode;*/
    jfieldID jonErrorCode = env->GetFieldID(m_jcErrorCodeClass, "nErrorCode", "I");
    env->SetIntField(joErrors, jonErrorCode, eError);

    env->DeleteLocalRef(m_jcErrorCodeClass);

    return true;
}

/******************************************************************
 * Function: string 数组从java转换到c++
 * Parameters:
 * Return: true or false
 ******************************************************************/
vector<string > DealJniMethod::getStrListFromObjArray(JNIEnv *env, jobjectArray joStrList, int nSetSize)
{
    vector<string > sStrList;
    sStrList.clear();
    if(0 == env || 0 == joStrList) return sStrList;

    int len = env->GetArrayLength(joStrList);
    if(len > nSetSize)
    {
        len = nSetSize;
    }

    for(int i = 0; i < len; i++)
    {
        jstring joStr = (jstring)env->GetObjectArrayElement(joStrList, i);
        if(joStr != 0)
        {
            int nLen = env->GetStringLength(joStr);
            if(nLen > 0)
            {
                char nTmpBuff[nLen];
                env->GetStringUTFRegion(joStr, 0, nLen, nTmpBuff);
     //         const char *pStr = nTmpBuff;
                sStrList.push_back(nTmpBuff);
            }

            /*删除对象*/
            env->DeleteLocalRef(joStr);
        }
    }

    return sStrList;
}

/******************************************************************
 * Function: string 数组从c++转换到java
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealJniMethod::setStrListToObjArray(JNIEnv *env, jobject joStrListObj, vector<string > sStrList)
{
    if(0 == env || 0 == joStrListObj || sStrList.empty()) return false;

    /*查找装载容器对象的类*/
    jclass m_jStrArrayClass = env->FindClass("com/android/Samkoonhmi/util/StrArray");
    if(0 == m_jStrArrayClass) return false;

    jfieldID joStrListFiled = env->GetFieldID(m_jStrArrayClass, "sStrList", "[Ljava/lang/String;");

    /*获取Java中的实例类SEND_PACKAGE_JNI*/
    jclass m_jStrClass = env->FindClass("java/lang/String");
    if(0 == m_jStrClass)
    {
        env->DeleteLocalRef(m_jStrArrayClass);
        return false;
    }

    /*分配容器内存*/
    int nSize = sStrList.size();
    jobjectArray joStrList = env->NewObjectArray(nSize, m_jStrClass, 0);
    if(joStrList != 0)
    {
        jstring sTmpStr = 0;
        for(int i = 0; i < nSize; i++)
        {
            /*创建对象*/
            sTmpStr = env->NewStringUTF(sStrList.at(i).data());
            if(sTmpStr != 0)
            {
                /*设置值*/
                env->SetObjectArrayElement(joStrList, i, sTmpStr);
                env->DeleteLocalRef(sTmpStr);
            }
        }

        /*设置地址结构体*/
        env->SetObjectField(joStrListObj, joStrListFiled, joStrList);
        env->DeleteLocalRef(joStrList);
    }

    env->DeleteLocalRef(m_jStrArrayClass);
    env->DeleteLocalRef(m_jStrClass);
    return true;
}


/******************************************************************
 * Function: 转换发送通信的枚举
 * Parameters:
 * Return: true or false
 ******************************************************************/
CMN_SEND_TYPE DealJniMethod::getCmnSendType(int nCmnType)
{
    CMN_SEND_TYPE eCmnSendType = CMN_OTHER_TYPE;
    switch(nCmnType)
    {
    case 1:
        {
            eCmnSendType = CMN_READ;
            break;
        }
    case 2:
        {
            eCmnSendType = CMN_WRITE;
            break;
        }
    case 3:
        {
            eCmnSendType = CMN_READ_BEFORE_WRITE;
            break;
        }
    case 4:
        {
            eCmnSendType = CMN_WRITE_AFTER_READ;
            break;
        }
    default:
        break;
    }

    return eCmnSendType;
}

/******************************************************************
 * Function: 发送通信的枚举转换成int值到java层
 * Parameters:
 * Return: true or false
 ******************************************************************/
int DealJniMethod::getCmnSendFromType(CMN_SEND_TYPE eCmnType)
{
    int eCmnSendType = 0;
    switch(eCmnType)
    {
    case CMN_READ:
        {
            eCmnSendType = 1;
            break;
        }
    case CMN_WRITE:
        {
            eCmnSendType = 2;
            break;
        }
    case CMN_READ_BEFORE_WRITE:
        {
            eCmnSendType = 3;
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            eCmnSendType = 4;
            break;
        }
    default:
        break;
    }

    return eCmnSendType;
}
