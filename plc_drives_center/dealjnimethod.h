#ifndef DEALJNIMETHOD_H
#define DEALJNIMETHOD_H

#include "jni.h"
#include <stdlib.h>
#include "tools/structHead.h"

class DealJniMethod
{
private:
    /*地址结构体*/
    jfieldID m_jfnUserPlcId;
    jfieldID m_jfnRegIndex;
    jfieldID m_jfnAddrLen;
    jfieldID m_jfnPlcStationIndex;
    jfieldID m_jfnAddrValue;
    jfieldID m_jfeAddrRWprop;

    /*发送数据结构体*/
    jfieldID m_jfnDataType;
    jfieldID m_jfnRWCtlType;
    jfieldID m_jfnByteHLPos;

    /*发送包的结构体*/
    jfieldID m_jfnReturnLen;
    jfieldID m_jfeSendType;
    jfieldID m_jfsSendDataList;

    /*PLC信息属性*/
    jfieldID m_jfeConnectType;
    jfieldID m_jfnProtocolIndex;
    jfieldID m_jfnSampRate;
    jfieldID m_jfsProtocolName;

public:
    DealJniMethod();

    /*转换地址结构体*/
    ADDR_TARGET_PROP getAddrPropFromJni(JNIEnv *env, jobject jAddr);

    /*转换发送数据结构体*/
    SEND_DATA_STRUCT getSendStructFromJni(JNIEnv *env,jobject jSend);

    /*转换发送字符串（到串口的）结构体*/
    SEND_PACKAGE getSendPropFromJni(JNIEnv *env, jobject jSend);

    /*获得PLC采集的唯一信息*/
    PLC_RATE_INFO getPlcRateInfo(JNIEnv *env, jobject joPlcRate);

    /*转换地址结构体到java*/
    bool setAddrProp(JNIEnv *env, jobject jAddr, ADDR_TARGET_PROP mAddrProp);

    /*传递地址集合到java*/
    bool setAddrListToJni(JNIEnv *env, jobject joAddrListObj, vector<ADDR_TARGET_PROP > sourceList);

    /*设置发送字符串（到串口的）结构体*/
    bool setSendPropToJni(JNIEnv *env, jobject joSend, SEND_PACKAGE source);

    /*设置一组发送数据包*/
    bool setSendPkgList(JNIEnv *env, jobject joSendData, vector<SEND_PACKAGE > sourceList);

    /*设置通知值到应用层的结构体*/
    bool setPlcNoticValue(JNIEnv *env, jobject joNoticInfo, bool bRefreash);

    /*设置读取的数据信息*/
    bool setErrorInfo(JNIEnv *env, jobject joErrors, CMN_RW_ERROR_CODE eError);

    /*string 数组从java转换到c++*/
    vector<string > getStrListFromObjArray(JNIEnv *env, jobjectArray joStrList, int nSetSize);

    /*string 数组从c++转换到java*/
    bool setStrListToObjArray(JNIEnv *env, jobject joStrList, vector<string > sStrList);

    /*转换发送通信的枚举*/
    CMN_SEND_TYPE getCmnSendType(int nCmnType);

    /*发送通信的枚举转换成int值到java层*/
    int getCmnSendFromType(CMN_SEND_TYPE eCmnType);
};

#endif // DEALJNIMETHOD_H
