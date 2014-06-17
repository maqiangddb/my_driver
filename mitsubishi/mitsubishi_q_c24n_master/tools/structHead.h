#ifndef STRUCTHEAD_H
#define STRUCTHEAD_H

#include "stdafx.h"
#include "enumhead.h"

/*检查地址的源地址*/
typedef struct addrCheckSourceProp{
    short nUserPlcId;                        // 用户定义的PLC号
    int nAddrLen;                            //地址的长度
    int nPlcStationIndex;                    //PLC的站号
    string sRegName;                         //寄存器的名字
    string sAddrValue;                       //地址值的字符串
    READ_WRITE_COM_TYPE eAddrRWprop;         //地址的读写权限
}ADDR_CHECK_SOURCE_PROP;

/*校验和转换成功后的地址结构体*/
typedef struct addrTargetProp{
    short nUserPlcId;                        // 用户定义的PLC号
    short nRegIndex;                         //寄存器的索引号
    int nAddrLen;                            //地址的长度
    int nPlcStationIndex;                    //PLC的站号
    int nAddrValue;                          //地址值
    READ_WRITE_COM_TYPE eAddrRWprop;         //地址的读写权限
}ADDR_TARGET_PROP;

/*地址读写结构体*/
typedef struct addrRWStruct{
    int nUseTimes;                           //读或写的次数
    ADDR_TARGET_PROP mAddrProp;              //地址结构体
    vector<uchar > nValueList;               //地址的值
}ADDR_RW_STRUCT;

/*打包后的数据结构体，要发送到通信口的包*/
typedef struct sendPackage{
    int nPkgIndex;                           //包的下标，从1开始
    int nTotalPkgNum;                        //包的总数
    int nReturnLen;                          //要返回的长度，<0:表示不定长，=0:表示不返回，>0:表示返回的长度。
    CMN_SEND_TYPE eSendType;                 //发送的类型，读，还是写，还是写前读
    vector<uchar > sendDataList;             //要发送的数据包
}SEND_PACKAGE;

/*位地址的结构体，包含值和状态信息*/
typedef struct addrBitValueProp{
    uchar nValue;                            //地址的值
    CMN_RW_ERROR_CODE eErrorStatus;           //值的状态，是否正常，是否出错
}ADDR_BIT_VALUE_PROP;

/*字地址的结构体，包含值和状态信息*/
typedef struct addrWordValueProp{
    ushort nValue;                            //地址的值
    CMN_RW_ERROR_CODE eErrorStatus;            //值的状态，是否正常，是否出错
}ADDR_WORD_VALUE_PROP;

/*双字地址的结构体，包含值和状态信息*/
typedef struct addrDoubleWordProp{
    uint nValue;                            //地址的值
    CMN_RW_ERROR_CODE eErrorStatus;            //值的状态，是否正常，是否出错
}ADDR_DOUBLE_WORD_PROP;

/*字符串地址的结构体，包含值和状态信息*/
typedef struct addrStringProp{
    string nValue;                            //地址的值
    CMN_RW_ERROR_CODE eErrorStatus;            //值的状态，是否正常，是否出错
}ADDR_STRING_PROP;

/*一个站号里面所有包含的所有寄存器的值
* 一级容器放寄存器号，二级容器存一个寄存器的所有数据
* 比如0x放regBitList.at(0)位置，则1x放regBitList.at(1)位置
*/
typedef struct stationValueProp{
    vector<vector<ADDR_BIT_VALUE_PROP > > regBitList;              //位容器，一个为占一个字节，
    vector<vector<ADDR_WORD_VALUE_PROP > > regWordList;            //字容器，一个为占两个字节
    vector<vector<ADDR_DOUBLE_WORD_PROP > > regDoubleWordList;     //字容器，一个为占四个字节
    vector<vector<ADDR_STRING_PROP > > regStringList;              //字符串容器
}STATION_VALUE_PROP;

/*协议名字和采样周期等多协议采样参数结构体*/
typedef struct plcSampInfo{
    CONNECT_TYPE eConnectType;        //连接口
    int nProtocolIndex;               //一个连接口的连接号
    int nSampRate;                    //最小周期
    string sProtocolName;             //协议名称
}PLC_RATE_INFO;

/*发送结构体*/
typedef struct sendStruct{
    DATA_TYPE nDataType;            //数据类型
    READ_WRITE_COM_TYPE nReadWriteCtlType;    //读写等级和类型
    BYTE_HL_POS nByteHLPos;           //高地位设置
}SEND_DATA_STRUCT;

/*通信需要的参数*/
typedef struct cmnNeedProp{
    ADDR_RW_STRUCT mAddrInfo;         // 要打包的地址
    CMN_SEND_TYPE eSendType;          // 打包的类型
}CMN_NEED_PROP;

/*解包需要的参数*/
typedef struct unpackNeedProp{
    bool bNeedSetData;                // 解包成功后，是否需要更新数据
    int nCurrTimes;                   // 包的次数，属于第几个包，适合通信一次需要发送几次包的情况
    ADDR_RW_STRUCT mCurrAddr;         // 当前地址
}UNPACK_NEED_PROP;

/*串口信息列表*/
typedef struct serialPort{
    int BAUD_RATE ;                // 波特率
    int PARITY_TYPE ;              // 1:NONE(无), 2:EVEN(偶校验), 3:ODD(奇校验)
    int DATA_BIT ;                 // 数据位
    int STOP_BIT ;                 // 停止位
}SERIAL_PORT_PROP;


#endif // STRUCTHEAD_H
