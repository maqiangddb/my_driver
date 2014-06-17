#ifndef ENUMHEAD_H
#define ENUMHEAD_H

/******************************************************************
* Function: 一些公共宏的定义
******************************************************************/

/*地址的最大上限的一些枚举定义*/
#define ADDR_MAX_VALUE_16              16
#define ADDR_MAX_VALUE_32              32
#define ADDR_MAX_VALUE_64              64
#define ADDR_MAX_VALUE_128             128
#define ADDR_MAX_VALUE_256             256
#define ADDR_MAX_VALUE_512             512
#define ADDR_MAX_VALUE_1024            1024
#define ADDR_MAX_VALUE_2048            2048
#define ADDR_MAX_VALUE_4096            4096
#define ADDR_MAX_VALUE_4096            4096
#define ADDR_MAX_VALUE_8192            8192

#define ADDR_MAX_VALUE_56              56
#define ADDR_MAX_VALUE_200             200
#define ADDR_MAX_VALUE_1000            1000
#define ADDR_MAX_VALUE_8000            8000
#define ADDR_MAX_VALUE_9600            9600
#define ADDR_MAX_VALUE_10000           10000
#define ADDR_MAX_VALUE_16000           16000
#define ADDR_MAX_VALUE_32768           32768
#define ADDR_MAX_VALUE_65536           65536
#define ADDR_MAX_VALUE_100000          100000
#define ADDR_MAX_VALUE_128000          128000

/******************************************************************
* 定义几个一定要在这里对应REG_TYPE_INDEX里面的枚举, 像这样
******************************************************************/
#define MAX_BIT_REG_NUN                 0

/******************************************************************
* 定义几个一定要在这里对应REG_TYPE_INDEX里面的枚举, 比如
******************************************************************/
#define MAX_WORD_REG_NUN                11

/******************************************************************
* 定义几个一定要在这里对应REG_TYPE_INDEX里面的枚举, 比如
******************************************************************/
#define MAX_DWORD_REG_NUN               0

/*最大读写长度*/
#define MAX_RW_LEN                     200

/*寄存器索引的枚举*/
typedef enum regTypeIndex{
    REG_TYPE_T              = 0,
    REG_TYPE_C              = 1,
    REG_TYPE_X              = 2,
    REG_TYPE_Y              = 3,
    REG_TYPE_R              = 4,
    REG_TYPE_L              = 5,
    REG_TYPE_WX             = 6,
    REG_TYPE_WY             = 7,
    REG_TYPE_WR             = 8,
    REG_TYPE_WL             = 9,
    REG_TYPE_DT             = 10,
    REG_TYPE_LD             = 11,
    REG_TYPE_SV             = 12,
    REG_TYPE_EV             = 13,
    REG_TYPE_NULL        = 0xee
}REG_TYPE_INDEX;

/*地址类型的枚举，比如位地址，比如字地址*/
typedef enum addrRegType{
    BIT_ADDR_REG = 0x01,            //位地址
    WORT_ADDR_REG = 0x02,           //字地址
    DWORT_ADDR_REG = 0x03,          //双字地址
    STRING_ADDR_REG = 0x04,         //字符串地址
    ALL_ADDR_REG = 0x05,            // 所有地址
    READ_ADDR_REG = 0x06,           // 读地址
    WRITE_ADDR_REG = 0x07,          // 写地址
    OTHER_ADDR_REG_TYE = 0xff
}ADDR_REG_TYPE;

/*连接口的定义*/
typedef enum connectType{
    LOCAL = 1,           //内部
    COM0 = 2,            //串口0
    COM1 = 3,            //串口1
    COM2 = 4,            //串口2
    COM3 = 5,            //串口3
    COM4 = 6,            //串口4
    COM5 = 7,            //串口5
    NET0 = 8,            //网口0
    NET1 = 9,            //网口1
    NET2 = 10,           //网口2
    NET3 = 11,           //网口3
    NET4 = 12,           //网口4
    NET5 = 13,           //网口5
    CAN0 = 14,           //CAN0
    CAN1 = 15,           //CAN1
    CAN2 = 16,           //CAN2
    CAN3 = 17,           //CAN3
    CAN4 = 18,           //CAN4
    OTHER = 0xff         //其他
}CONNECT_TYPE;

/*地址读写的类型，地址读写的优先级*/
typedef enum readWriteComType{
    SCENE_CONTROL_ONCE_W = 0x01,                       // scene Control only write once
    SCENE_CONTROL_LOOP_W = 0x02,                       // scene Control loop write
    GLOBAL_ONCE_W = 0x03,                              // global only write once
    GLOBAL_LOOP_W = 0x04,                              // global loop write
    MACRO_ONCE_W = 0x05,                               // macro only write once
    MACRO_LOOP_W = 0x06,                               // macro loop write
    SCENE_CONTROL_ONCE_R = 0x07,                       // scene Control only read once
    SCENE_CONTROL_LOOP_R = 0x08,                       // scene Control loop read
    ALARM_LOOP_R = 0x09,                               // alarm loop read
    RECIPE_ONCE_R = 0x0a,                              // recipe once read
    GLOBAL_ONCE_R = 0x0b,                              // global only read once
    GLOBAL_LOOP_R = 0x0c,                              // global loop read
    MACRO_ONCE_R = 0x0d,                               // macro only read once
    MACRO_LOOP_R = 0x0e,                               // macro loop read
    DATA_COLLECT_ONCE_R = 0x0f,                        // data collect only read once
    DATA_COLLECT_LOOP_R = 0x10,                        // data collect loop read
    OTHER_CONTROL_TYPE = 0x11                          // other control type
}READ_WRITE_COM_TYPE;

/*这一次通信的枚举，比如是读操作还是写操作等,主要用于取打包好的读写数据时用*/
typedef enum cmnSendType{
    CMN_READ = 0x01,                      //通信读操作
    CMN_WRITE = 0x02,                     //通信写操作
    CMN_READ_BEFORE_WRITE = 0x03,         //写之前的读操作
    CMN_WRITE_AFTER_READ = 0x04,          //读之后的写操作
    CMN_HAND_CHECK = 0x05,                //握手校验
    CMN_OTHER_TYPE = 0xff                 //其他方式
}CMN_SEND_TYPE;

/*PLC的类型枚举，主站还是从站*/
typedef enum plcFunType{
    PLC_MASTER = 0x01,         //plc主站
    PLC_SLAVE = 0x02,          //PLC从站
    PLC_OTHER_TYPE = 0x03      //PLC其他方式
}PLC_FUN_TYPE;

/*地址检查出错枚举*/
typedef enum addrCheckErrorInfo{
    ADDR_CORRECT = 0x01,               //地址正确
    IS_BIT_ADDR = 0x02,                //是位地址
    IS_WORD_ADDR = 0x03,               //是字地址
    ADDR_FORMAT_ERROR = 0x04,          //地址格式错误，主要是没有按照寄存器的格式来，参照寄存器的名称
    ADDR_OUT_BOUND = 0x05,             //地址越界
    ILLEGAL_REG = 0x06,                //非法寄存器名字
    ILLEGAL_VALUE = 0x07,              //非法地址值，主要指值转换成int不正确
    ADDR_OTHER_ERROR                   //未知错误
}ADDR_CHECK_ERROR_INFO;


/*数据类型*/
typedef enum dataType{
    BIT_1 = 0x01,                      // 位
    POSITIVE_INT_16 = 0x02,            // 16位整数
    POSITIVE_INT_32 = 0x03,            // 32位整数
    INT_16 = 0x04,                     // 16位正整数
    INT_32 = 0x05,                     // 32位正整数
    BCD_16 = 0x06,                     // 16位BCD码
    BCD_32 = 0x07,                     // 32位BCD码
    FLOAT_32 = 0x08,                   // 32位浮点数
    ASCII_STRING = 0x09,               // ASCII码
    HEX_16 = 0x0a,                     //16位16进制
    HEX_32 = 0x0b,                     //32位16进制
    OTC_16 = 0x0c,                     //16位八进制
    OTC_32 = 0x0d,                     //32位八进制
    OTHER_DATA_TYPE
}DATA_TYPE;

/*高地位设置方式*/
typedef enum byteHLPos{
    L_BYTE_FIRST = 0x01,                   // low byte at first of high byte.
    H_BYTE_FIRST = 0x02,                   // high byte at first of low byte.
    OTHER_H_L_POS                          // other high or low pos
}BYTE_HL_POS;

/*通信 驱动返回的错误码*/
typedef enum cmnRwErrorCode{
    CMN_NOMAL_CODE = 0,              //通信正常
    FUN_ERROR_CODE = 1,              //功能码错误
    ADDR_OUT_RANG = 2,               //地址越界错误
    DATA_VALUE_ERROR = 3,            //值错误
    ADDR_UNREAD = 4,                 //地址不可读
    ADDR_UNWRITE = 5,                //地址不可写
    DATA_LEN_ERROR = 6,              //数据长度不对
    CHECK_OK_UNRCV_ALL = 7,          //校验成功但是长度不够
    DATA_CHECK_ERROR = 8,            //数据校验错误
    WRITE_DATA_FAIL = 9,             //写数据失败
    READ_DATA_FAIL = 10,             //读数据时候失败
    WRITE_CHECK_FAIL = 11,           //写返回后校验失败
    READ_CHECK_FAIL = 12,            //读取的数据校验失败
    CMN_CONNECT_FAIL = 13,           //通信失败，检查连线是否正确
    PROTO_NOT_EXSIT = 14,            //协议不存在
    PROTO_TYPE_ERROR = 15,           //协议类型出错
    UNRCV_ALL_DATA = 16,             //数据接收未完成，继续接收
    NO_RCV_DATA = 17,                //没有接收到数据
    STATION_ERROR = 18,              //站号不对
    ADDR_LEN_ERROR = 19,             //地址长度不对
    CALL_PROTO_ERROR = 20,           //调用协议出错
    SEND_DATA_FAILED = 21,           //发送数据失败
    RCV_DATA_FAILED = 22,            //接收数据失败
    CHANGE_BAUD_RATE_9600 = 9600,      //需要修改波特率为9600
    CHANGE_BAUD_RATE_115200 = 115200,  //需要修改波特率为115200
    OTHER_ERROR_CODE = 0xff
}CMN_RW_ERROR_CODE;

/*校验的类型*/
typedef enum checkType{
    CHECK_HAND_SHAKE = 0x01,           //握手校验
    CHECK_READ = 0x02,                 //读校验
    CHECK_WRITE = 0x03,                //写校验
    CHECK_OTHER = 0xff,                //其他校验
}CHECK_TYPE;

typedef enum ProtocolType
{
    SERIAL_PORT = 0x01,               //串口
    NET_PORT    = 0x02,               //网口
    NET_SERIAL_PORT = 0x03,           //串口网口
}PROTOCOL_TYPE;

#endif // ENUMHEAD_H
