#ifndef PLCPROTOCOL_H
#define PLCPROTOCOL_H

#include "../tools/stdafx.h"
#include "../tools/structHead.h"

class PlcProtocol
{
public:
    PlcProtocol();

    /*字符串类型的地址值转换为整数类型,供上位编译时用*/
    bool addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget);

    /*对地址进行排序和整理,供上位编译时用*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr = false);

    /*地址的多重复制*/
    ADDR_CHECK_ERROR_INFO addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP > &mSourceAddrList, int nInterval, int nCopyNum);

    /*获得本协议的寄存器名称,供上位用*/
    vector<string > getRegNameList(ADDR_REG_TYPE eAddrType = ALL_ADDR_REG);

    /*检查地址值是否正确,供上位用*/
    ADDR_CHECK_ERROR_INFO checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType);

    /*检查传进来的字符串是否合乎地址规范,供上位地址检查用*/
    ADDR_CHECK_ERROR_INFO checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp);

    /*生成需要发送的包*/
    bool makePackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr);

    /*解包，解释收到的数据，只指读取的数据*/
    CMN_RW_ERROR_CODE unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, vector<uchar > &targetList);

    /*校验接收的结果*/
    CMN_RW_ERROR_CODE checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType = CHECK_WRITE);

    /*获得握手协议的发送包*/
    bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*握手协议解包*/
    CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes = 0);

    /*获得最大读写长度*/
    int getMaxRwLen();

    /*返回PLC的类型，是主站还是从站,1是主站，2是从站*/
    PLC_FUN_TYPE getPlcType();

    /*获得默认站号*/
    int getDefaultStation();

    /*获得默认本协议的默认串口信息*/
    SERIAL_PORT_PROP getDefaultSerialInfo();

    /*获得默认超时时间*/
    int getDefaultTimeout();

    /*获得默认最小采集周期*/
    int getDefaultCollectRate();

    /*获得帧间隔时间*/
    int getDefaultIntervalTime();

    /*获得这种地址寄存器是否可读*/
    bool getAddrCanRead(ADDR_TARGET_PROP mAddr);

    /*获得这种地址寄存器是否可写*/
    bool getAddrCanWrite(ADDR_TARGET_PROP mAddr);

    /*根据寄存器的下标值获得寄存器的枚举*/
    static REG_TYPE_INDEX getRegTypeFromInt(int nRegIndex);

    /*根据寄存器的名字获得寄存器的下标*/
    bool getRegIndexFromName(string sRegName, int &nRegIndex);

    /*获取协议使用通讯方式，串口or网口*/
    PROTOCOL_TYPE getProType();

private:
    bool makeReadPackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge);
    bool makeWritePackage(CMN_NEED_PROP mCmnProp, uint &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr);
};

#endif // PLCPROTOCOL_H
