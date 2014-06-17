#ifndef DEALJNIMETHOD_H
#define DEALJNIMETHOD_H

#include "jni.h"
#include <stdlib.h>
#include "tools/structHead.h"

class DealJniMethod
{
private:
    /*��ַ�ṹ��*/
    jfieldID m_jfnUserPlcId;
    jfieldID m_jfnRegIndex;
    jfieldID m_jfnAddrLen;
    jfieldID m_jfnPlcStationIndex;
    jfieldID m_jfnAddrValue;
    jfieldID m_jfeAddrRWprop;

    /*�������ݽṹ��*/
    jfieldID m_jfnDataType;
    jfieldID m_jfnRWCtlType;
    jfieldID m_jfnByteHLPos;

    /*���Ͱ��Ľṹ��*/
    jfieldID m_jfnReturnLen;
    jfieldID m_jfeSendType;
    jfieldID m_jfsSendDataList;

    /*PLC��Ϣ����*/
    jfieldID m_jfeConnectType;
    jfieldID m_jfnProtocolIndex;
    jfieldID m_jfnSampRate;
    jfieldID m_jfsProtocolName;

public:
    DealJniMethod();

    /*ת����ַ�ṹ��*/
    ADDR_TARGET_PROP getAddrPropFromJni(JNIEnv *env, jobject jAddr);

    /*ת���������ݽṹ��*/
    SEND_DATA_STRUCT getSendStructFromJni(JNIEnv *env,jobject jSend);

    /*ת�������ַ����������ڵģ��ṹ��*/
    SEND_PACKAGE getSendPropFromJni(JNIEnv *env, jobject jSend);

    /*���PLC�ɼ���Ψһ��Ϣ*/
    PLC_RATE_INFO getPlcRateInfo(JNIEnv *env, jobject joPlcRate);

    /*ת����ַ�ṹ�嵽java*/
    bool setAddrProp(JNIEnv *env, jobject jAddr, ADDR_TARGET_PROP mAddrProp);

    /*���ݵ�ַ���ϵ�java*/
    bool setAddrListToJni(JNIEnv *env, jobject joAddrListObj, vector<ADDR_TARGET_PROP > sourceList);

    /*���÷����ַ����������ڵģ��ṹ��*/
    bool setSendPropToJni(JNIEnv *env, jobject joSend, SEND_PACKAGE source);

    /*����һ�鷢�����ݰ�*/
    bool setSendPkgList(JNIEnv *env, jobject joSendData, vector<SEND_PACKAGE > sourceList);

    /*����ֵ֪ͨ��Ӧ�ò�Ľṹ��*/
    bool setPlcNoticValue(JNIEnv *env, jobject joNoticInfo, bool bRefreash);

    /*���ö�ȡ��������Ϣ*/
    bool setErrorInfo(JNIEnv *env, jobject joErrors, CMN_RW_ERROR_CODE eError);

    /*string �����javaת����c++*/
    vector<string > getStrListFromObjArray(JNIEnv *env, jobjectArray joStrList, int nSetSize);

    /*string �����c++ת����java*/
    bool setStrListToObjArray(JNIEnv *env, jobject joStrList, vector<string > sStrList);

    /*ת������ͨ�ŵ�ö��*/
    CMN_SEND_TYPE getCmnSendType(int nCmnType);

    /*����ͨ�ŵ�ö��ת����intֵ��java��*/
    int getCmnSendFromType(CMN_SEND_TYPE eCmnType);
};

#endif // DEALJNIMETHOD_H
