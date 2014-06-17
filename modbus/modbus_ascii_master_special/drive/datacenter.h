#ifndef DATACENTER_H
#define DATACENTER_H

#include "../tools/structHead.h"

class DataCenter
{
public:
    DataCenter();

    /*���ö�ȡ��������*/
    bool setReadData(ADDR_RW_STRUCT mAddrInfo, STATION_VALUE_PROP &readDataList, bool bUserData = true, CMN_RW_ERROR_CODE eSuccess = CMN_NOMAL_CODE);

    /*��õ�ֵַ*/
    bool getReadData(ADDR_TARGET_PROP mAddr, STATION_VALUE_PROP &readDataList, vector<ADDR_BIT_VALUE_PROP > &nResultList);

#if 0
private:
    int getVectorIndex(vector<STATION_VALUE_PROP > stationList, int nStationId);
#endif

};

#endif // DATACENTER_H
