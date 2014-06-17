#ifndef ENUMHEAD_H
#define ENUMHEAD_H

/******************************************************************
* Function: һЩ������Ķ���
******************************************************************/

/*��ַ��������޵�һЩö�ٶ���*/
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
* ���弸��һ��Ҫ�������ӦREG_TYPE_INDEX�����ö��, ������
******************************************************************/
#define MAX_BIT_REG_NUN                 0

/******************************************************************
* ���弸��һ��Ҫ�������ӦREG_TYPE_INDEX�����ö��, ����
******************************************************************/
#define MAX_WORD_REG_NUN                11

/******************************************************************
* ���弸��һ��Ҫ�������ӦREG_TYPE_INDEX�����ö��, ����
******************************************************************/
#define MAX_DWORD_REG_NUN               0

/*����д����*/
#define MAX_RW_LEN                     200

/*�Ĵ���������ö��*/
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

/*��ַ���͵�ö�٣�����λ��ַ�������ֵ�ַ*/
typedef enum addrRegType{
    BIT_ADDR_REG = 0x01,            //λ��ַ
    WORT_ADDR_REG = 0x02,           //�ֵ�ַ
    DWORT_ADDR_REG = 0x03,          //˫�ֵ�ַ
    STRING_ADDR_REG = 0x04,         //�ַ�����ַ
    ALL_ADDR_REG = 0x05,            // ���е�ַ
    READ_ADDR_REG = 0x06,           // ����ַ
    WRITE_ADDR_REG = 0x07,          // д��ַ
    OTHER_ADDR_REG_TYE = 0xff
}ADDR_REG_TYPE;

/*���ӿڵĶ���*/
typedef enum connectType{
    LOCAL = 1,           //�ڲ�
    COM0 = 2,            //����0
    COM1 = 3,            //����1
    COM2 = 4,            //����2
    COM3 = 5,            //����3
    COM4 = 6,            //����4
    COM5 = 7,            //����5
    NET0 = 8,            //����0
    NET1 = 9,            //����1
    NET2 = 10,           //����2
    NET3 = 11,           //����3
    NET4 = 12,           //����4
    NET5 = 13,           //����5
    CAN0 = 14,           //CAN0
    CAN1 = 15,           //CAN1
    CAN2 = 16,           //CAN2
    CAN3 = 17,           //CAN3
    CAN4 = 18,           //CAN4
    OTHER = 0xff         //����
}CONNECT_TYPE;

/*��ַ��д�����ͣ���ַ��д�����ȼ�*/
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

/*��һ��ͨ�ŵ�ö�٣������Ƕ���������д������,��Ҫ����ȡ����õĶ�д����ʱ��*/
typedef enum cmnSendType{
    CMN_READ = 0x01,                      //ͨ�Ŷ�����
    CMN_WRITE = 0x02,                     //ͨ��д����
    CMN_READ_BEFORE_WRITE = 0x03,         //д֮ǰ�Ķ�����
    CMN_WRITE_AFTER_READ = 0x04,          //��֮���д����
    CMN_HAND_CHECK = 0x05,                //����У��
    CMN_OTHER_TYPE = 0xff                 //������ʽ
}CMN_SEND_TYPE;

/*PLC������ö�٣���վ���Ǵ�վ*/
typedef enum plcFunType{
    PLC_MASTER = 0x01,         //plc��վ
    PLC_SLAVE = 0x02,          //PLC��վ
    PLC_OTHER_TYPE = 0x03      //PLC������ʽ
}PLC_FUN_TYPE;

/*��ַ������ö��*/
typedef enum addrCheckErrorInfo{
    ADDR_CORRECT = 0x01,               //��ַ��ȷ
    IS_BIT_ADDR = 0x02,                //��λ��ַ
    IS_WORD_ADDR = 0x03,               //���ֵ�ַ
    ADDR_FORMAT_ERROR = 0x04,          //��ַ��ʽ������Ҫ��û�а��ռĴ����ĸ�ʽ�������ռĴ���������
    ADDR_OUT_BOUND = 0x05,             //��ַԽ��
    ILLEGAL_REG = 0x06,                //�Ƿ��Ĵ�������
    ILLEGAL_VALUE = 0x07,              //�Ƿ���ֵַ����Ҫֵָת����int����ȷ
    ADDR_OTHER_ERROR                   //δ֪����
}ADDR_CHECK_ERROR_INFO;


/*��������*/
typedef enum dataType{
    BIT_1 = 0x01,                      // λ
    POSITIVE_INT_16 = 0x02,            // 16λ����
    POSITIVE_INT_32 = 0x03,            // 32λ����
    INT_16 = 0x04,                     // 16λ������
    INT_32 = 0x05,                     // 32λ������
    BCD_16 = 0x06,                     // 16λBCD��
    BCD_32 = 0x07,                     // 32λBCD��
    FLOAT_32 = 0x08,                   // 32λ������
    ASCII_STRING = 0x09,               // ASCII��
    HEX_16 = 0x0a,                     //16λ16����
    HEX_32 = 0x0b,                     //32λ16����
    OTC_16 = 0x0c,                     //16λ�˽���
    OTC_32 = 0x0d,                     //32λ�˽���
    OTHER_DATA_TYPE
}DATA_TYPE;

/*�ߵ�λ���÷�ʽ*/
typedef enum byteHLPos{
    L_BYTE_FIRST = 0x01,                   // low byte at first of high byte.
    H_BYTE_FIRST = 0x02,                   // high byte at first of low byte.
    OTHER_H_L_POS                          // other high or low pos
}BYTE_HL_POS;

/*ͨ�� �������صĴ�����*/
typedef enum cmnRwErrorCode{
    CMN_NOMAL_CODE = 0,              //ͨ������
    FUN_ERROR_CODE = 1,              //���������
    ADDR_OUT_RANG = 2,               //��ַԽ�����
    DATA_VALUE_ERROR = 3,            //ֵ����
    ADDR_UNREAD = 4,                 //��ַ���ɶ�
    ADDR_UNWRITE = 5,                //��ַ����д
    DATA_LEN_ERROR = 6,              //���ݳ��Ȳ���
    CHECK_OK_UNRCV_ALL = 7,          //У��ɹ����ǳ��Ȳ���
    DATA_CHECK_ERROR = 8,            //����У�����
    WRITE_DATA_FAIL = 9,             //д����ʧ��
    READ_DATA_FAIL = 10,             //������ʱ��ʧ��
    WRITE_CHECK_FAIL = 11,           //д���غ�У��ʧ��
    READ_CHECK_FAIL = 12,            //��ȡ������У��ʧ��
    CMN_CONNECT_FAIL = 13,           //ͨ��ʧ�ܣ���������Ƿ���ȷ
    PROTO_NOT_EXSIT = 14,            //Э�鲻����
    PROTO_TYPE_ERROR = 15,           //Э�����ͳ���
    UNRCV_ALL_DATA = 16,             //���ݽ���δ��ɣ���������
    NO_RCV_DATA = 17,                //û�н��յ�����
    STATION_ERROR = 18,              //վ�Ų���
    ADDR_LEN_ERROR = 19,             //��ַ���Ȳ���
    CALL_PROTO_ERROR = 20,           //����Э�����
    SEND_DATA_FAILED = 21,           //��������ʧ��
    RCV_DATA_FAILED = 22,            //��������ʧ��
    CHANGE_BAUD_RATE_9600 = 9600,      //��Ҫ�޸Ĳ�����Ϊ9600
    CHANGE_BAUD_RATE_115200 = 115200,  //��Ҫ�޸Ĳ�����Ϊ115200
    OTHER_ERROR_CODE = 0xff
}CMN_RW_ERROR_CODE;

/*У�������*/
typedef enum checkType{
    CHECK_HAND_SHAKE = 0x01,           //����У��
    CHECK_READ = 0x02,                 //��У��
    CHECK_WRITE = 0x03,                //дУ��
    CHECK_OTHER = 0xff,                //����У��
}CHECK_TYPE;

typedef enum ProtocolType
{
    SERIAL_PORT = 0x01,               //����
    NET_PORT    = 0x02,               //����
    NET_SERIAL_PORT = 0x03,           //��������
}PROTOCOL_TYPE;

#endif // ENUMHEAD_H
