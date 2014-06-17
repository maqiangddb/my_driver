#ifndef STRUCTHEAD_H
#define STRUCTHEAD_H

#include "stdafx.h"
#include "enumhead.h"

/*����ַ��Դ��ַ*/
typedef struct addrCheckSourceProp{
    short nUserPlcId;                        // �û������PLC��
    int nAddrLen;                            //��ַ�ĳ���
    int nPlcStationIndex;                    //PLC��վ��
    string sRegName;                         //�Ĵ���������
    string sAddrValue;                       //��ֵַ���ַ���
    READ_WRITE_COM_TYPE eAddrRWprop;         //��ַ�Ķ�дȨ��
}ADDR_CHECK_SOURCE_PROP;

/*У���ת���ɹ���ĵ�ַ�ṹ��*/
typedef struct addrTargetProp{
    short nUserPlcId;                        // �û������PLC��
    short nRegIndex;                         //�Ĵ�����������
    int nAddrLen;                            //��ַ�ĳ���
    int nPlcStationIndex;                    //PLC��վ��
    int nAddrValue;                          //��ֵַ
    READ_WRITE_COM_TYPE eAddrRWprop;         //��ַ�Ķ�дȨ��
}ADDR_TARGET_PROP;

/*��ַ��д�ṹ��*/
typedef struct addrRWStruct{
    int nUseTimes;                           //����д�Ĵ���
    ADDR_TARGET_PROP mAddrProp;              //��ַ�ṹ��
    vector<uchar > nValueList;               //��ַ��ֵ
}ADDR_RW_STRUCT;

/*���������ݽṹ�壬Ҫ���͵�ͨ�ſڵİ�*/
typedef struct sendPackage{
    int nPkgIndex;                           //�����±꣬��1��ʼ
    int nTotalPkgNum;                        //��������
    int nReturnLen;                          //Ҫ���صĳ��ȣ�<0:��ʾ��������=0:��ʾ�����أ�>0:��ʾ���صĳ��ȡ�
    CMN_SEND_TYPE eSendType;                 //���͵����ͣ���������д������дǰ��
    vector<uchar > sendDataList;             //Ҫ���͵����ݰ�
}SEND_PACKAGE;

/*λ��ַ�Ľṹ�壬����ֵ��״̬��Ϣ*/
typedef struct addrBitValueProp{
    uchar nValue;                            //��ַ��ֵ
    CMN_RW_ERROR_CODE eErrorStatus;           //ֵ��״̬���Ƿ��������Ƿ����
}ADDR_BIT_VALUE_PROP;

/*�ֵ�ַ�Ľṹ�壬����ֵ��״̬��Ϣ*/
typedef struct addrWordValueProp{
    ushort nValue;                            //��ַ��ֵ
    CMN_RW_ERROR_CODE eErrorStatus;            //ֵ��״̬���Ƿ��������Ƿ����
}ADDR_WORD_VALUE_PROP;

/*˫�ֵ�ַ�Ľṹ�壬����ֵ��״̬��Ϣ*/
typedef struct addrDoubleWordProp{
    uint nValue;                            //��ַ��ֵ
    CMN_RW_ERROR_CODE eErrorStatus;            //ֵ��״̬���Ƿ��������Ƿ����
}ADDR_DOUBLE_WORD_PROP;

/*�ַ�����ַ�Ľṹ�壬����ֵ��״̬��Ϣ*/
typedef struct addrStringProp{
    string nValue;                            //��ַ��ֵ
    CMN_RW_ERROR_CODE eErrorStatus;            //ֵ��״̬���Ƿ��������Ƿ����
}ADDR_STRING_PROP;

/*һ��վ���������а��������мĴ�����ֵ
* һ�������żĴ����ţ�����������һ���Ĵ�������������
* ����0x��regBitList.at(0)λ�ã���1x��regBitList.at(1)λ��
*/
typedef struct stationValueProp{
    vector<vector<ADDR_BIT_VALUE_PROP > > regBitList;              //λ������һ��Ϊռһ���ֽڣ�
    vector<vector<ADDR_WORD_VALUE_PROP > > regWordList;            //��������һ��Ϊռ�����ֽ�
    vector<vector<ADDR_DOUBLE_WORD_PROP > > regDoubleWordList;     //��������һ��Ϊռ�ĸ��ֽ�
    vector<vector<ADDR_STRING_PROP > > regStringList;              //�ַ�������
}STATION_VALUE_PROP;

/*Э�����ֺͲ������ڵȶ�Э����������ṹ��*/
typedef struct plcSampInfo{
    CONNECT_TYPE eConnectType;        //���ӿ�
    int nProtocolIndex;               //һ�����ӿڵ����Ӻ�
    int nSampRate;                    //��С����
    string sProtocolName;             //Э������
}PLC_RATE_INFO;

/*���ͽṹ��*/
typedef struct sendStruct{
    DATA_TYPE nDataType;            //��������
    READ_WRITE_COM_TYPE nReadWriteCtlType;    //��д�ȼ�������
    BYTE_HL_POS nByteHLPos;           //�ߵ�λ����
}SEND_DATA_STRUCT;

/*ͨ����Ҫ�Ĳ���*/
typedef struct cmnNeedProp{
    ADDR_RW_STRUCT mAddrInfo;         // Ҫ����ĵ�ַ
    CMN_SEND_TYPE eSendType;          // ���������
}CMN_NEED_PROP;

/*�����Ҫ�Ĳ���*/
typedef struct unpackNeedProp{
    bool bNeedSetData;                // ����ɹ����Ƿ���Ҫ��������
    int nCurrTimes;                   // ���Ĵ��������ڵڼ��������ʺ�ͨ��һ����Ҫ���ͼ��ΰ������
    ADDR_RW_STRUCT mCurrAddr;         // ��ǰ��ַ
}UNPACK_NEED_PROP;

/*������Ϣ�б�*/
typedef struct serialPort{
    int BAUD_RATE ;                // ������
    int PARITY_TYPE ;              // 1:NONE(��), 2:EVEN(żУ��), 3:ODD(��У��)
    int DATA_BIT ;                 // ����λ
    int STOP_BIT ;                 // ֹͣλ
}SERIAL_PORT_PROP;


#endif // STRUCTHEAD_H
