#ifndef PLCPROTOCOL_H
#define PLCPROTOCOL_H

#include "../tools/stdafx.h"
#include "../tools/structHead.h"

class PlcProtocol
{
public:
    PlcProtocol();

    /*�ַ������͵ĵ�ֵַת��Ϊ��������,����λ����ʱ��*/
    bool addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget);

    /*�Ե�ַ�������������,����λ����ʱ��*/
    bool sortOutAddrList(vector<ADDR_TARGET_PROP > mSourceAddrList, vector<ADDR_TARGET_PROP > &targetList, int nMaxRWlen, bool bWriteAddr = false);

    /*��ַ�Ķ��ظ���*/
    ADDR_CHECK_ERROR_INFO addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP > &mSourceAddrList, int nInterval, int nCopyNum);

    /*��ñ�Э��ļĴ�������,����λ��*/
    vector<string > getRegNameList(ADDR_REG_TYPE eAddrType = ALL_ADDR_REG);

    /*����ֵַ�Ƿ���ȷ,����λ��*/
    ADDR_CHECK_ERROR_INFO checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType);

    /*��鴫�������ַ����Ƿ�Ϻ���ַ�淶,����λ��ַ�����*/
    ADDR_CHECK_ERROR_INFO checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp);

    /*��վ�����������ڽ��յ�������*/
    CMN_RW_ERROR_CODE getSlaveDataStr(vector<uchar > rcvList, int nStationId, STATION_VALUE_PROP &readDataList, SEND_PACKAGE &mSendPkg);

    /*У����յĽ��*/
    CMN_RW_ERROR_CODE checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType = CHECK_WRITE);

    /*�������Э��ķ��Ͱ�*/
    bool getHandshakePkg(SEND_PACKAGE &mHandShake, uint &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate = 0);

    /*����Э����*/
    CMN_RW_ERROR_CODE unpackHandshakePkg(vector<uchar > rcvList, int nCurrTimes = 0);

    /*�������д����*/
    int getMaxRwLen();

    /*����PLC�����ͣ�����վ���Ǵ�վ,1����վ��2�Ǵ�վ*/
    PLC_FUN_TYPE getPlcType();

    /*���Ĭ��վ��*/
    int getDefaultStation();

    /*���Ĭ�ϱ�Э���Ĭ�ϴ�����Ϣ*/
    SERIAL_PORT_PROP getDefaultSerialInfo();

    /*���Ĭ�ϳ�ʱʱ��*/
    int getDefaultTimeout();

    /*���Ĭ����С�ɼ�����*/
    int getDefaultCollectRate();

    /*���֡���ʱ��*/
    int getDefaultIntervalTime();

    /*������ֵ�ַ�Ĵ����Ƿ�ɶ�*/
    bool getAddrCanRead(ADDR_TARGET_PROP mAddr);

    /*������ֵ�ַ�Ĵ����Ƿ��д*/
    bool getAddrCanWrite(ADDR_TARGET_PROP mAddr);

    /*���ݼĴ������±�ֵ��üĴ�����ö��*/
    static REG_TYPE_INDEX getRegTypeFromInt(int nRegIndex);

    /*���ݼĴ��������ֻ�üĴ������±�*/
    static bool getRegIndexFromName(string sRegName, int &nRegIndex);

    /*��ȡЭ��ʹ��ͨѶ��ʽ������or����*/
    PROTOCOL_TYPE getProType();
};

#endif // PLCPROTOCOL_H
