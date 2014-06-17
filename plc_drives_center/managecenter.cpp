
#include "managecenter.h"

ManageCenter::ManageCenter()
{
    pthread_mutex_init(&m_mutex, NULL);
}

ManageCenter::~ManageCenter()
{
    pthread_mutex_destroy(&m_mutex);
}

/******************************************************************
 * Function: ���д���ݵĴ������
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getWriteDataStr(vector<SEND_PACKAGE> &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*����*/
    this->threadLock();

    bool bSuccess = false;

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�Դ���Ľṹ����г�ʼ��*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;

    /*ȡҪд�����ݵĵ�ַ*/
    if(CMN_WRITE_AFTER_READ == eSendType)
    {
        mCmnProp.mAddrInfo = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;

        /*д֮ǰ��ȡ�����ݸ�ֵ�������ַ��list�У���ҪЩ��ֵ���ڵ�ǰ��ַ��ֵ��*/
        mCmnProp.mAddrInfo.nValueList = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).nRcvValueList;

        /*���ط��͵İ�*/
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    }
    else
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.getNextWriteAddr(mCmnProp.mAddrInfo);
        if(!bSuccess)
        {
    //        cout << "getNextWriteAddr failed";
            /*����*/
            this->threadUnlock();
            return false;
        }

        /*���ط��͵İ�*/
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    }

    /*��ս��յ�����*/
    m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).nRcvValueList.clear();

    if(sendPkgList.empty())
    {
        cout << "makePackage failed" << "eSendType = " << eSendType;
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ��ö����ݵĴ������
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadDataStr(vector<SEND_PACKAGE> &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*����*/
    this->threadLock();

    bool bSuccess = false;

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�Դ���Ľṹ����г�ʼ��*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;

    /*ȡҪд�����ݵĵ�ַ*/
    if(CMN_READ_BEFORE_WRITE == eSendType)
    {
        mCmnProp.mAddrInfo = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;
    }
    else
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.getNextReadAddr(mCmnProp.mAddrInfo);
        if(!bSuccess)
        {
            /*����*/
            this->threadUnlock();
            return false;
        }
    }

    /*���ط��͵İ�*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);

    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ������ֵĴ������
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getHandshakePkg(PLC_RATE_INFO mPlcRateInfo, SEND_PACKAGE &mHandShake, int nPkgIndex, int nStationId, int nBaudRate)
{
    /*����*/
    this->threadLock();

    bool bSuccess = false;

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�Ƿ���Ҫ����Э��*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).bNeedHandShake;
    if(bSuccess)
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.getHandshakePkg(mHandShake, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, nPkgIndex, nStationId, nBaudRate);
    }

    /*����*/
    this->threadUnlock();
    return bSuccess;
}

/******************************************************************
 * Function: ������ֵĴ������
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getCurrAddr(PLC_RATE_INFO mPlcRateInfo, ADDR_TARGET_PROP &mCurrAddr)
{
    /*����*/
    this->threadLock();

    bool bSuccess = false;

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    mCurrAddr = m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr.mAddrProp;

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: �Զ��������ݽ��н��
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkReadData(char *pRrcvData, int nLen, int nReturnLen, int nCurrTimes, CMN_SEND_TYPE eSendType, PLC_RATE_INFO mPlcRateInfo, bool &bChange)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*�����ݴ洢��list��*/
    uchar *pURrcvData = (uchar *)pRrcvData;
    vector<uchar > sourceList, targetList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pURrcvData[i]);
    }

    /*�Խ����Ҫ�����Խ��г�ʼ��*/
    ADDR_RW_STRUCT mCurrAddr = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;

    UNPACK_NEED_PROP mUnpackProp;
    mUnpackProp.bNeedSetData = false;
    mUnpackProp.mCurrAddr = mCurrAddr;
    mUnpackProp.nCurrTimes = nCurrTimes;

    /*�����ݽ��н��*/
    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.unpackReadData(mUnpackProp, sourceList, targetList);
    if(CMN_NOMAL_CODE != eCmnInfo)
    {
        /*�ͷ��ڴ�*/
        targetList.clear();
        vector<uchar >().swap(targetList);

        /*����ǲ��������գ����ش��½������ݽ��м��*/
        if(nReturnLen < 0)
        {
            /*����*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*�������Ҫ�������ݣ������������ݣ����򷵻�*/
    if(!mUnpackProp.bNeedSetData)
    {
        /*�ͷ��ڴ�*/
        targetList.clear();
        vector<uchar >().swap(targetList);

        /*����*/
        this->threadUnlock();
        return eCmnInfo;
    }

    /*����Ч���ݽ��д����򱣴������ֵ*/
    mCurrAddr.nValueList = targetList;
    if(eSendType == CMN_READ_BEFORE_WRITE)
    {
        m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nRcvValueList = targetList;
    }
    else
    {
        m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mCurrAddr, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList, false, eCmnInfo);
        bChange = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.setAddrValue(mCurrAddr.mAddrProp, targetList);
    }

    /*�ͷ��ڴ�*/
    targetList.clear();
    vector<uchar >().swap(targetList);

    /*����*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: У���յ�������
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkWriteData(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo, bool &bChange)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*�����ݴ洢��list��*/
    uchar *pUWrcvData = (uchar *)pWrcvData;
    vector<uchar > sourceList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pUWrcvData[i]);
    }

    /*�Խ����Ҫ�����Խ��г�ʼ��*/
    ADDR_RW_STRUCT mCurrAddr = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;

    UNPACK_NEED_PROP mUnpackProp;
    mUnpackProp.bNeedSetData = false;
    mUnpackProp.mCurrAddr = mCurrAddr;
    mUnpackProp.nCurrTimes = nCurrTimes;

    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.checkRcvData(mUnpackProp, sourceList, CHECK_WRITE);
    if(CMN_NOMAL_CODE != eCmnInfo)
    {
        if(nReturnLen < 0)
        {
            /*����*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*����д���ݵ�ͬʱ��������Ҳд���ڲ��洢��*/
    m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mCurrAddr, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList, true, eCmnInfo);
    bChange = true;

    /*����*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: ������Э��ķ�������У��
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkHandshakePkg(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*�����ݴ洢��list��*/
    vector<uchar > sourceList;
    if(pWrcvData != 0)
    {
        uchar *pUWrcvData = (uchar *)pWrcvData;
        for(int i = 0; i < nLen; i++)
        {
            sourceList.push_back(pUWrcvData[i]);
        }
    }

    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.unpackHandshakePkg(sourceList, nCurrTimes);
    if(CMN_NOMAL_CODE != eCmnInfo)
    {
        if(nReturnLen < 0 && !sourceList.empty())
        {
            /*����*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*����*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: ����ͨ�ű�־
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::setCmnInfo(int nCmnInfo, PLC_RATE_INFO mPlcRateInfo)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    if(nCmnInfo != 0)
    {
        m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].bNeedHandShake = true;
        m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes = 0;
        m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.clearWriteAddr();

        setDataListErrorCode(m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList, CMN_CONNECT_FAIL);
    }
    else
    {
        m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].bNeedHandShake = false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ����ͨ�ű�־
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::setDataListErrorCode(STATION_VALUE_PROP &readDataList, CMN_RW_ERROR_CODE eErrorCode)
{
    /*λ��ֵַͨ��״̬*/
    int nRegSize = readDataList.regBitList.size();
    for(int i = 0; i < nRegSize; i++)
    {
        int nAddrSize = readDataList.regBitList[i].size();
        for(int j = 0; j < nAddrSize; j++)
        {
            if(readDataList.regBitList[i][j].eErrorStatus == CMN_NOMAL_CODE)
            {
                readDataList.regBitList[i][j].eErrorStatus = eErrorCode;
            }
        }
    }

    /*�ֵ�ֵַͨ��״̬*/
    nRegSize = readDataList.regWordList.size();
    for(int i = 0; i < nRegSize; i++)
    {
        int nAddrSize = readDataList.regWordList[i].size();
        for(int j = 0; j < nAddrSize; j++)
        {
            if(readDataList.regWordList[i][j].eErrorStatus == CMN_NOMAL_CODE)
            {
                readDataList.regWordList[i][j].eErrorStatus = eErrorCode;
            }
        }
    }

    /*˫�ֵ�ֵַͨ��״̬*/
    nRegSize = readDataList.regDoubleWordList.size();
    for(int i = 0; i < nRegSize; i++)
    {
        int nAddrSize = readDataList.regDoubleWordList[i].size();
        for(int j = 0; j < nAddrSize; j++)
        {
            if(readDataList.regDoubleWordList[i][j].eErrorStatus == CMN_NOMAL_CODE)
            {
                readDataList.regDoubleWordList[i][j].eErrorStatus = eErrorCode;
            }
        }
    }

    /*�ַ�����ֵַͨ��״̬*/
    nRegSize = readDataList.regStringList.size();
    for(int i = 0; i < nRegSize; i++)
    {
        int nAddrSize = readDataList.regStringList[i].size();
        for(int j = 0; j < nAddrSize; j++)
        {
            if(readDataList.regStringList[i][j].eErrorStatus == CMN_NOMAL_CODE)
            {
                readDataList.regStringList[i][j].eErrorStatus = eErrorCode;
            }
        }
    }

    return true;
}

/******************************************************************
 * Function: ��ȡЭ��Ĺ������ͣ���Ҫ����վ���Ǵ�վ
 * Parameters:
 * Return: 1����վ��2�Ǵ�վ
 ******************************************************************/
int ManageCenter::getProtocolFunType(PLC_RATE_INFO mPlcRateInfo)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return PLC_OTHER_TYPE;
    }

    int nPlcType = m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType();

    /*����*/
    this->threadUnlock();
    return nPlcType;
}

/******************************************************************
 * Function: �������еĶ���ַ
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::setAllReadAddrs(vector<ADDR_TARGET_PROP> addrList, bool bCover, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return ;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*����*/
        this->threadUnlock();
        return ;
    }

    vector<ADDR_RW_STRUCT> mSourceAddrList;
    mSourceAddrList.clear();

    ADDR_RW_STRUCT mTmpAddr;
    mTmpAddr.nUseTimes = 0;
    mTmpAddr.nValueList.clear();

    if(!bCover && nMaxRWlen > 0)
    {
        vector<ADDR_TARGET_PROP> addrBufList = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.getAddrListBuf();
        for(int i = 0; i < addrList.size();i++)
        {
            addrBufList.push_back(addrList.at(i));
        }
        addrList.clear();
        m_protocolLinkList.at(nLinkIndex).mLibObj.sortOutAddrList(addrBufList,addrList,nMaxRWlen,false);
        bCover = true;
    }

    int nSize = addrList.size();
    for(int i = 0; i < nSize; i++)
    {
        mTmpAddr.mAddrProp = addrList.at(i);
        if(!m_protocolLinkList.at(nLinkIndex).mLibObj.getAddrCanRead(mTmpAddr.mAddrProp))
        {
            continue;
        }
        mSourceAddrList.push_back(mTmpAddr);
    }
    m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.setAddrListBuf(addrList);
    m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.setReadAddrList(mSourceAddrList,bCover);

    /*����*/
    this->threadUnlock();
}

/******************************************************************
 * Function: ������еĶ���ַ
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::clearAllReadAddr()
{
    /*�������е�Э��*/
    int nProtocolSize = m_protocolLinkList.size();
    for(int i = 0; i < nProtocolSize; i++)
    {
        /*�������е�PLC*/
        PROTOCOL_LINK_INFO mPlcInfo = m_protocolLinkList.at(i);
        int nPlcSize = mPlcInfo.mPlcChannelList.size();
        for(int j = 0; j < nPlcSize; j++)
        {
            /*�������ַ*/
            m_protocolLinkList.at(i).mPlcChannelList.at(j).addrManageObj.clearReadAddr();
        }
    }
}

/******************************************************************
 * Function: �������еĶ���ַ
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen, bool bWriteAddr)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*������ȥУ��*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.sortOutAddrList(mSourceAddrList, targetList, nMaxRWlen, bWriteAddr);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ��д��ַ���д��
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::makeWritePackage(ADDR_TARGET_PROP mAddrProp, vector<unsigned char> dataList, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�Դ���Ľṹ����г�ʼ��*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;
    mCmnProp.mAddrInfo.mAddrProp = mAddrProp;
    mCmnProp.mAddrInfo.nValueList = dataList;

    /*������ȥУ��*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, mSendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ��д��ַ���д��
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::makeReadPackage(ADDR_TARGET_PROP mAddrProp, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*�Դ���Ľṹ����г�ʼ��*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;
    mCmnProp.mAddrInfo.mAddrProp = mAddrProp;
    mCmnProp.mAddrInfo.nValueList.clear();

    /*������ȥУ��*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, mSendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ���ݵ�ֵַ������ֵ
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<unsigned char> dataList, PLC_RATE_INFO mPlcRateInfo, int nSimulateType)
{
    /*����*/
    this->threadLock();

    /*������ڲ���ַ*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        m_localAddrObj.setWriteRegDataList(mAddr, dataList);

        /*����*/
        this->threadUnlock();
        return ;
    }

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return ;
    }

    if(!m_protocolLinkList.at(nLinkIndex).mLibObj.getAddrCanWrite(mAddr))
    {
        /*����*/
        this->threadUnlock();
        return ;
    }

    /*�����slaveЭ��*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        ADDR_RW_STRUCT mWriteAddr;
        mWriteAddr.mAddrProp = mAddr;
        mWriteAddr.nValueList = dataList;
        m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mWriteAddr, m_localAddrObj.m_localAddrValueList);
    }
    else if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_MASTER)
    {
        /*��ӵ�ַ��д��ַ������*/
        ADDR_RW_STRUCT mTmpAddr;
        mTmpAddr.nUseTimes = 0;
        mTmpAddr.nValueList.clear();
        mTmpAddr.mAddrProp = mAddr;
        mTmpAddr.nValueList = dataList;
        if(nSimulateType == 1)
        {
            m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mTmpAddr, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList);
        }
        else
        {
            m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.setWriteAddrList(mTmpAddr);
        }
    }

    /*����*/
    this->threadUnlock();
}

/******************************************************************
 * Function: ���ݵ�ֵַ��ȡ��ֵ
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP> &valueList, PLC_RATE_INFO mPlcRateInfo)
{
    /*����*/
    this->threadLock();

    bool bSuccess = false;
    /*������ڲ���ַ*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        bSuccess = m_localAddrObj.getReadRegDataList(mAddr, valueList);

        /*����*/
        this->threadUnlock();
        return bSuccess;
    }

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        return false;
    }

    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.getReadData(mAddr, m_localAddrObj.m_localAddrValueList, valueList);
    }
    else
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.getReadData(mAddr, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList, valueList);
    }

    /*����*/
    this->threadUnlock();
    return bSuccess;
}

/******************************************************************
 * Function: ���ݵ�ֵַ�����ַ���
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string> dataList, PLC_RATE_INFO mPlcRateInfo)
{
    /*������ڲ���ַ*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        /*����*/
        this->threadLock();
        m_localAddrObj.setWriteRegStrList(mAddr, dataList);
        /*����*/
        this->threadUnlock();
        return true;
    }

    return false;
}

/******************************************************************
 * Function: ���ݵ�ֵַ��ȡ��ֵ
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string> &valueList, PLC_RATE_INFO mPlcRateInfo)
{
    /*������ڲ���ַ*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        /*����*/
        this->threadLock();
        bool bSuccess = m_localAddrObj.getReadRegStrList(mAddr, valueList);
        /*����*/
        this->threadUnlock();
        return bSuccess;
    }

    return false;
}

/******************************************************************
 * Function: �ر�һ��Э��
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::closeProtocolSo(PLC_RATE_INFO mPlcRateInfo)
{
    /*����*/
    this->threadLock();

    PROTOCOL_LINK_INFO mLinkInfo;
    int nLinkSize = m_protocolLinkList.size();
    for(int i = 0; i < nLinkSize; i++)
    {
        mLinkInfo = m_protocolLinkList.at(i);

        /*����Ѿ��򿪹����Э��*/
        if(mLinkInfo.sLibName == mPlcRateInfo.sProtocolName)
        {
            mLinkInfo.mLibObj.closeLib();

            /*����*/
            this->threadUnlock();
            return true;
        }
    }

    /*����*/
    this->threadUnlock();
    return false;
}

/******************************************************************
 * Function: �ر�����Э��
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::closeAllSo()
{
    /*����*/
    this->threadLock();

    PROTOCOL_LINK_INFO mLinkInfo;
    int nLinkSize = m_protocolLinkList.size();
    for(int i = 0; i < nLinkSize; i++)
    {
        mLinkInfo = m_protocolLinkList.at(i);
        mLinkInfo.mLibObj.closeLib();
    }

    clearAllReadAddr();

    /*����*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: ��������Դ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadLock()
{
    int rt = pthread_mutex_lock(&m_mutex);

    /*�ɹ�����true*/
    if(rt == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************
 * Function: ��������Դ����
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadUnlock()
{
    int rt = pthread_mutex_unlock(&m_mutex);

    /*�ɹ�����true*/
    if(rt == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************
 * Function: ��������Դ���Լ���
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadTryLock()
{
    int rt = pthread_mutex_trylock(&m_mutex);

    /*�ɹ�����true*/
    if(rt == 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************
 * Function: ���ݵ�ֵַ��ȡ��ֵ
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getProtocolLinkInfo(PLC_RATE_INFO mPlcRateInfo, int &nLinkIndex, int &nPlcIndex)
{
    /*�ڲ���ַ����Ҫ���ļ�*/
    if(mPlcRateInfo.eConnectType == LOCAL || OTHER == mPlcRateInfo.eConnectType)
    {
        return false;
    }

    bool bSuccess = false;
    PROTOCOL_LINK_INFO mLinkInfo;
    int nLinkSize = m_protocolLinkList.size();

    for(int i = 0; i < nLinkSize; i++)
    {
        mLinkInfo = m_protocolLinkList.at(i);

        /*����Ѿ��򿪹����Э��*/
        if(mLinkInfo.sLibName == mPlcRateInfo.sProtocolName)
        {
            int nSize = mLinkInfo.mPlcChannelList.size();
            for(int j = 0; j < nSize; j++)
            {
                if(mPlcRateInfo.eConnectType == mLinkInfo.mPlcChannelList.at(j).mPlcRateInfo.eConnectType &&
                   mPlcRateInfo.nProtocolIndex == mLinkInfo.mPlcChannelList.at(j).mPlcRateInfo.nProtocolIndex &&
                   mPlcRateInfo.sProtocolName == mLinkInfo.mPlcChannelList.at(j).mPlcRateInfo.sProtocolName)
                {
                    nLinkIndex = i;
                    nPlcIndex = j;
                    return true;
                }
            }

            /*Э����Ȼ���ڣ�����PLC�����ڣ������*/
            PlC_CHANNEL plcChannel;

            /*��ʼ��PLC��Ϣ�ṹ��*/
            plcChannel.mPlcRateInfo.nSampRate = mPlcRateInfo.nSampRate;
            plcChannel.mPlcRateInfo.eConnectType = mPlcRateInfo.eConnectType;
            plcChannel.mPlcRateInfo.nProtocolIndex = mPlcRateInfo.nProtocolIndex;
            plcChannel.mPlcRateInfo.sProtocolName = mPlcRateInfo.sProtocolName;

            /*�����������*/
            plcChannel.nRcvValueList.clear();
            plcChannel.nCmnTimes = 0;
            plcChannel.bNeedHandShake = true;

            /*��ʼ����ǰ��ַ*/
            plcChannel.mCurrAddr.nValueList.resize(1000);
            plcChannel.mCurrAddr.nValueList.clear();
            plcChannel.mCurrAddr.nUseTimes = 0;
            plcChannel.mCurrAddr.mAddrProp.eAddrRWprop = OTHER_CONTROL_TYPE;
            plcChannel.mCurrAddr.mAddrProp.nAddrLen = 1;
            plcChannel.mCurrAddr.mAddrProp.nAddrValue = 0;
            plcChannel.mCurrAddr.mAddrProp.nPlcStationIndex = 0;
            plcChannel.mCurrAddr.mAddrProp.nRegIndex = 0;

            m_protocolLinkList[i].mPlcChannelList.push_back(plcChannel);

            /*�Է��ؽ����ֵ*/
            nLinkIndex = i;
            nPlcIndex = m_protocolLinkList[i].mPlcChannelList.size() -1;

            /*��Э��*/
            bSuccess = m_protocolLinkList.at(i).mLibObj.openLib(mPlcRateInfo.sProtocolName);
            if(!bSuccess)
            {
                return false;
            }
        }
    }

    /*���Ҳ���������������Э��*/
    LOGD("create new protocol : %s", mPlcRateInfo.sProtocolName.data());
    PlC_CHANNEL plcChannel;

    /*��ʼ��PLC��Ϣ�ṹ��*/
    plcChannel.mPlcRateInfo.nSampRate = mPlcRateInfo.nSampRate;
    plcChannel.mPlcRateInfo.eConnectType = mPlcRateInfo.eConnectType;
    plcChannel.mPlcRateInfo.nProtocolIndex = mPlcRateInfo.nProtocolIndex;
    plcChannel.mPlcRateInfo.sProtocolName = mPlcRateInfo.sProtocolName;

    /*�����������*/
    plcChannel.nRcvValueList.clear();
    plcChannel.nCmnTimes = 0;
    plcChannel.bNeedHandShake = true;

    /*��ʼ����ǰ��ַ*/
    plcChannel.mCurrAddr.nValueList.resize(1000);
    plcChannel.mCurrAddr.nValueList.clear();
    plcChannel.mCurrAddr.nUseTimes = 0;
    plcChannel.mCurrAddr.mAddrProp.eAddrRWprop = OTHER_CONTROL_TYPE;
    plcChannel.mCurrAddr.mAddrProp.nAddrLen = 1;
    plcChannel.mCurrAddr.mAddrProp.nAddrValue = 0;
    plcChannel.mCurrAddr.mAddrProp.nPlcStationIndex = 0;
    plcChannel.mCurrAddr.mAddrProp.nRegIndex = 0;

    /*��ʼ����ǰЭ��*/
    mLinkInfo.sLibName = mPlcRateInfo.sProtocolName;
    mLinkInfo.mPlcChannelList.clear();
    mLinkInfo.mPlcChannelList.push_back(plcChannel);
    m_protocolLinkList.push_back(mLinkInfo);
    nLinkSize = m_protocolLinkList.size();

    /*�Է��ؽ����ֵ*/
    nLinkIndex = nLinkSize - 1;
    nPlcIndex = 0;
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.openLib(mPlcRateInfo.sProtocolName);
    if(!bSuccess)
    {
        return false;
    }

    LOGD("create new protocol:%s success", mPlcRateInfo.sProtocolName.data());
    return true;
}

/******************************************************************
 * Function: �Զ��������ݽ��н��
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::rcvStrForSlave(char *pRrcvData, int nLen, int nStationId, PLC_RATE_INFO mPlcRateInfo, vector<SEND_PACKAGE> &eSendPkg)
{
    /*����*/
    this->threadLock();

    /*ȡ��Э���ָ��*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*���Ǵ�վ�򷵻�*/
    PLC_FUN_TYPE ePlcFunType = m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType();
    if(PLC_SLAVE != ePlcFunType)
    {
        /*����*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*�����ݴ洢��list��*/
    uchar *pURrcvData = (uchar *)pRrcvData;
    vector<uchar > sourceList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pURrcvData[i]);
    }

    /*�Դ�վ���ݽ��н��*/
    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.getSlaveDataStr(sourceList, nStationId, m_localAddrObj.m_localAddrValueList, eSendPkg);

    /*����*/
    this->threadUnlock();
    return eCmnInfo;
}
