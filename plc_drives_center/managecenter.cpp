
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
 * Function: 获得写数据的打包数据
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getWriteDataStr(vector<SEND_PACKAGE> &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*加锁*/
    this->threadLock();

    bool bSuccess = false;

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*对打包的结构体进行初始化*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;

    /*取要写的数据的地址*/
    if(CMN_WRITE_AFTER_READ == eSendType)
    {
        mCmnProp.mAddrInfo = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;

        /*写之前读取的数据赋值给这个地址的list中，需要些的值放在当前地址的值中*/
        mCmnProp.mAddrInfo.nValueList = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).nRcvValueList;

        /*返回发送的包*/
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    }
    else
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.getNextWriteAddr(mCmnProp.mAddrInfo);
        if(!bSuccess)
        {
    //        cout << "getNextWriteAddr failed";
            /*解锁*/
            this->threadUnlock();
            return false;
        }

        /*返回发送的包*/
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    }

    /*清空接收的数据*/
    m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).nRcvValueList.clear();

    if(sendPkgList.empty())
    {
        cout << "makePackage failed" << "eSendType = " << eSendType;
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 获得读数据的打包数据
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadDataStr(vector<SEND_PACKAGE> &sendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*加锁*/
    this->threadLock();

    bool bSuccess = false;

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*对打包的结构体进行初始化*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;

    /*取要写的数据的地址*/
    if(CMN_READ_BEFORE_WRITE == eSendType)
    {
        mCmnProp.mAddrInfo = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;
    }
    else
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).addrManageObj.getNextReadAddr(mCmnProp.mAddrInfo);
        if(!bSuccess)
        {
            /*解锁*/
            this->threadUnlock();
            return false;
        }
    }

    /*返回发送的包*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, sendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);

    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 获得握手的打包数据
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getHandshakePkg(PLC_RATE_INFO mPlcRateInfo, SEND_PACKAGE &mHandShake, int nPkgIndex, int nStationId, int nBaudRate)
{
    /*加锁*/
    this->threadLock();

    bool bSuccess = false;

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*是否需要握手协议*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).bNeedHandShake;
    if(bSuccess)
    {
        bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.getHandshakePkg(mHandShake, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, nPkgIndex, nStationId, nBaudRate);
    }

    /*解锁*/
    this->threadUnlock();
    return bSuccess;
}

/******************************************************************
 * Function: 获得握手的打包数据
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getCurrAddr(PLC_RATE_INFO mPlcRateInfo, ADDR_TARGET_PROP &mCurrAddr)
{
    /*加锁*/
    this->threadLock();

    bool bSuccess = false;

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    mCurrAddr = m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr.mAddrProp;

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 对读到的数据进行解包
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkReadData(char *pRrcvData, int nLen, int nReturnLen, int nCurrTimes, CMN_SEND_TYPE eSendType, PLC_RATE_INFO mPlcRateInfo, bool &bChange)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*把数据存储到list中*/
    uchar *pURrcvData = (uchar *)pRrcvData;
    vector<uchar > sourceList, targetList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pURrcvData[i]);
    }

    /*对解包需要的属性进行初始化*/
    ADDR_RW_STRUCT mCurrAddr = m_protocolLinkList.at(nLinkIndex).mPlcChannelList.at(nPlcIndex).mCurrAddr;

    UNPACK_NEED_PROP mUnpackProp;
    mUnpackProp.bNeedSetData = false;
    mUnpackProp.mCurrAddr = mCurrAddr;
    mUnpackProp.nCurrTimes = nCurrTimes;

    /*对数据进行解包*/
    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.unpackReadData(mUnpackProp, sourceList, targetList);
    if(CMN_NOMAL_CODE != eCmnInfo)
    {
        /*释放内存*/
        targetList.clear();
        vector<uchar >().swap(targetList);

        /*如果是不定长接收，返回从新接收数据进行检查*/
        if(nReturnLen < 0)
        {
            /*解锁*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*如果不需要设置数据（不是有用数据），则返回*/
    if(!mUnpackProp.bNeedSetData)
    {
        /*释放内存*/
        targetList.clear();
        vector<uchar >().swap(targetList);

        /*解锁*/
        this->threadUnlock();
        return eCmnInfo;
    }

    /*对有效数据进行处理，或保存或设置值*/
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

    /*释放内存*/
    targetList.clear();
    vector<uchar >().swap(targetList);

    /*解锁*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: 校验收到的数据
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkWriteData(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo, bool &bChange)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*把数据存储到list中*/
    uchar *pUWrcvData = (uchar *)pWrcvData;
    vector<uchar > sourceList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pUWrcvData[i]);
    }

    /*对解包需要的属性进行初始化*/
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
            /*解锁*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*发送写数据的同时，把数据也写到内部存储区*/
    m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mCurrAddr, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].readDataList, true, eCmnInfo);
    bChange = true;

    /*解锁*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: 对握手协议的返回数据校验
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
int ManageCenter::checkHandshakePkg(char *pWrcvData, int nLen, int nReturnLen, int nCurrTimes, PLC_RATE_INFO mPlcRateInfo)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*把数据存储到list中*/
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
            /*解锁*/
            this->threadUnlock();
            eCmnInfo = UNRCV_ALL_DATA;
            return eCmnInfo;
        }
    }

    /*解锁*/
    this->threadUnlock();
    return eCmnInfo;
}

/******************************************************************
 * Function: 设置通信标志
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::setCmnInfo(int nCmnInfo, PLC_RATE_INFO mPlcRateInfo)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
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

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 设置通信标志
 * Parameters: pWriteStr, nLen
 * Return: true or false
 ******************************************************************/
bool ManageCenter::setDataListErrorCode(STATION_VALUE_PROP &readDataList, CMN_RW_ERROR_CODE eErrorCode)
{
    /*位地址值通信状态*/
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

    /*字地址值通信状态*/
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

    /*双字地址值通信状态*/
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

    /*字符串地址值通信状态*/
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
 * Function: 获取协议的功能类型，主要是主站还是从站
 * Parameters:
 * Return: 1是主站，2是从站
 ******************************************************************/
int ManageCenter::getProtocolFunType(PLC_RATE_INFO mPlcRateInfo)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return PLC_OTHER_TYPE;
    }

    int nPlcType = m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType();

    /*解锁*/
    this->threadUnlock();
    return nPlcType;
}

/******************************************************************
 * Function: 设置所有的读地址
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::setAllReadAddrs(vector<ADDR_TARGET_PROP> addrList, bool bCover, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return ;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        /*解锁*/
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

    /*解锁*/
    this->threadUnlock();
}

/******************************************************************
 * Function: 清除所有的读地址
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::clearAllReadAddr()
{
    /*查找所有的协议*/
    int nProtocolSize = m_protocolLinkList.size();
    for(int i = 0; i < nProtocolSize; i++)
    {
        /*查找所有的PLC*/
        PROTOCOL_LINK_INFO mPlcInfo = m_protocolLinkList.at(i);
        int nPlcSize = mPlcInfo.mPlcChannelList.size();
        for(int j = 0; j < nPlcSize; j++)
        {
            /*清除读地址*/
            m_protocolLinkList.at(i).mPlcChannelList.at(j).addrManageObj.clearReadAddr();
        }
    }
}

/******************************************************************
 * Function: 设置所有的读地址
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, PLC_RATE_INFO mPlcRateInfo, int nMaxRWlen, bool bWriteAddr)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*到驱动去校验*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.sortOutAddrList(mSourceAddrList, targetList, nMaxRWlen, bWriteAddr);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 对写地址进行打包
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::makeWritePackage(ADDR_TARGET_PROP mAddrProp, vector<unsigned char> dataList, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*对打包的结构体进行初始化*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;
    mCmnProp.mAddrInfo.mAddrProp = mAddrProp;
    mCmnProp.mAddrInfo.nValueList = dataList;

    /*到驱动去校验*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, mSendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 对写地址进行打包
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::makeReadPackage(ADDR_TARGET_PROP mAddrProp, vector<SEND_PACKAGE> &mSendPkgList, PLC_RATE_INFO mPlcRateInfo, CMN_SEND_TYPE eSendType)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*对打包的结构体进行初始化*/
    CMN_NEED_PROP mCmnProp;
    mCmnProp.eSendType = eSendType;
    mCmnProp.mAddrInfo.mAddrProp = mAddrProp;
    mCmnProp.mAddrInfo.nValueList.clear();

    /*到驱动去校验*/
    bSuccess = m_protocolLinkList.at(nLinkIndex).mLibObj.makePackage(mCmnProp, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].nCmnTimes, mSendPkgList, m_protocolLinkList[nLinkIndex].mPlcChannelList[nPlcIndex].mCurrAddr);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return false;
    }

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 根据地址值设置数值
 * Parameters:
 * Return: null
 ******************************************************************/
void ManageCenter::setWriteRegDataList(ADDR_TARGET_PROP mAddr, vector<unsigned char> dataList, PLC_RATE_INFO mPlcRateInfo, int nSimulateType)
{
    /*加锁*/
    this->threadLock();

    /*如果是内部地址*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        m_localAddrObj.setWriteRegDataList(mAddr, dataList);

        /*解锁*/
        this->threadUnlock();
        return ;
    }

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        return ;
    }

    if(!m_protocolLinkList.at(nLinkIndex).mLibObj.getAddrCanWrite(mAddr))
    {
        /*解锁*/
        this->threadUnlock();
        return ;
    }

    /*如果是slave协议*/
    if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_SLAVE)
    {
        ADDR_RW_STRUCT mWriteAddr;
        mWriteAddr.mAddrProp = mAddr;
        mWriteAddr.nValueList = dataList;
        m_protocolLinkList.at(nLinkIndex).mLibObj.setReadData(mWriteAddr, m_localAddrObj.m_localAddrValueList);
    }
    else if(m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType() == PLC_MASTER)
    {
        /*添加地址到写地址容器中*/
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

    /*解锁*/
    this->threadUnlock();
}

/******************************************************************
 * Function: 根据地址值读取数值
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadRegDataList(ADDR_TARGET_PROP mAddr, vector<ADDR_BIT_VALUE_PROP> &valueList, PLC_RATE_INFO mPlcRateInfo)
{
    /*加锁*/
    this->threadLock();

    bool bSuccess = false;
    /*如果是内部地址*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        bSuccess = m_localAddrObj.getReadRegDataList(mAddr, valueList);

        /*解锁*/
        this->threadUnlock();
        return bSuccess;
    }

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
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

    /*解锁*/
    this->threadUnlock();
    return bSuccess;
}

/******************************************************************
 * Function: 根据地址值设置字符串
 * Parameters:
 * Return: null
 ******************************************************************/
bool ManageCenter::setWriteRegStrList(ADDR_TARGET_PROP mAddr, vector<string> dataList, PLC_RATE_INFO mPlcRateInfo)
{
    /*如果是内部地址*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        /*加锁*/
        this->threadLock();
        m_localAddrObj.setWriteRegStrList(mAddr, dataList);
        /*解锁*/
        this->threadUnlock();
        return true;
    }

    return false;
}

/******************************************************************
 * Function: 根据地址值读取数值
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getReadRegStrList(ADDR_TARGET_PROP mAddr, vector<string> &valueList, PLC_RATE_INFO mPlcRateInfo)
{
    /*如果是内部地址*/
    if(mPlcRateInfo.eConnectType == LOCAL)
    {
        /*加锁*/
        this->threadLock();
        bool bSuccess = m_localAddrObj.getReadRegStrList(mAddr, valueList);
        /*解锁*/
        this->threadUnlock();
        return bSuccess;
    }

    return false;
}

/******************************************************************
 * Function: 关闭一个协议
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::closeProtocolSo(PLC_RATE_INFO mPlcRateInfo)
{
    /*加锁*/
    this->threadLock();

    PROTOCOL_LINK_INFO mLinkInfo;
    int nLinkSize = m_protocolLinkList.size();
    for(int i = 0; i < nLinkSize; i++)
    {
        mLinkInfo = m_protocolLinkList.at(i);

        /*如果已经打开过这个协议*/
        if(mLinkInfo.sLibName == mPlcRateInfo.sProtocolName)
        {
            mLinkInfo.mLibObj.closeLib();

            /*解锁*/
            this->threadUnlock();
            return true;
        }
    }

    /*解锁*/
    this->threadUnlock();
    return false;
}

/******************************************************************
 * Function: 关闭所有协议
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::closeAllSo()
{
    /*加锁*/
    this->threadLock();

    PROTOCOL_LINK_INFO mLinkInfo;
    int nLinkSize = m_protocolLinkList.size();
    for(int i = 0; i < nLinkSize; i++)
    {
        mLinkInfo = m_protocolLinkList.at(i);
        mLinkInfo.mLibObj.closeLib();
    }

    clearAllReadAddr();

    /*解锁*/
    this->threadUnlock();
    return true;
}

/******************************************************************
 * Function: 对驱动资源加锁
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadLock()
{
    int rt = pthread_mutex_lock(&m_mutex);

    /*成功返回true*/
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
 * Function: 对驱动资源解锁
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadUnlock()
{
    int rt = pthread_mutex_unlock(&m_mutex);

    /*成功返回true*/
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
 * Function: 对驱动资源尝试加锁
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::threadTryLock()
{
    int rt = pthread_mutex_trylock(&m_mutex);

    /*成功返回true*/
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
 * Function: 根据地址值读取数值
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool ManageCenter::getProtocolLinkInfo(PLC_RATE_INFO mPlcRateInfo, int &nLinkIndex, int &nPlcIndex)
{
    /*内部地址不需要打开文件*/
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

        /*如果已经打开过这个协议*/
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

            /*协议虽然存在，但是PLC不存在，则添加*/
            PlC_CHANNEL plcChannel;

            /*初始化PLC信息结构体*/
            plcChannel.mPlcRateInfo.nSampRate = mPlcRateInfo.nSampRate;
            plcChannel.mPlcRateInfo.eConnectType = mPlcRateInfo.eConnectType;
            plcChannel.mPlcRateInfo.nProtocolIndex = mPlcRateInfo.nProtocolIndex;
            plcChannel.mPlcRateInfo.sProtocolName = mPlcRateInfo.sProtocolName;

            /*清除接收数据*/
            plcChannel.nRcvValueList.clear();
            plcChannel.nCmnTimes = 0;
            plcChannel.bNeedHandShake = true;

            /*初始化当前地址*/
            plcChannel.mCurrAddr.nValueList.resize(1000);
            plcChannel.mCurrAddr.nValueList.clear();
            plcChannel.mCurrAddr.nUseTimes = 0;
            plcChannel.mCurrAddr.mAddrProp.eAddrRWprop = OTHER_CONTROL_TYPE;
            plcChannel.mCurrAddr.mAddrProp.nAddrLen = 1;
            plcChannel.mCurrAddr.mAddrProp.nAddrValue = 0;
            plcChannel.mCurrAddr.mAddrProp.nPlcStationIndex = 0;
            plcChannel.mCurrAddr.mAddrProp.nRegIndex = 0;

            m_protocolLinkList[i].mPlcChannelList.push_back(plcChannel);

            /*对返回结果赋值*/
            nLinkIndex = i;
            nPlcIndex = m_protocolLinkList[i].mPlcChannelList.size() -1;

            /*打开协议*/
            bSuccess = m_protocolLinkList.at(i).mLibObj.openLib(mPlcRateInfo.sProtocolName);
            if(!bSuccess)
            {
                return false;
            }
        }
    }

    /*查找不到，则重新生成协议*/
    LOGD("create new protocol : %s", mPlcRateInfo.sProtocolName.data());
    PlC_CHANNEL plcChannel;

    /*初始化PLC信息结构体*/
    plcChannel.mPlcRateInfo.nSampRate = mPlcRateInfo.nSampRate;
    plcChannel.mPlcRateInfo.eConnectType = mPlcRateInfo.eConnectType;
    plcChannel.mPlcRateInfo.nProtocolIndex = mPlcRateInfo.nProtocolIndex;
    plcChannel.mPlcRateInfo.sProtocolName = mPlcRateInfo.sProtocolName;

    /*清除接收数据*/
    plcChannel.nRcvValueList.clear();
    plcChannel.nCmnTimes = 0;
    plcChannel.bNeedHandShake = true;

    /*初始化当前地址*/
    plcChannel.mCurrAddr.nValueList.resize(1000);
    plcChannel.mCurrAddr.nValueList.clear();
    plcChannel.mCurrAddr.nUseTimes = 0;
    plcChannel.mCurrAddr.mAddrProp.eAddrRWprop = OTHER_CONTROL_TYPE;
    plcChannel.mCurrAddr.mAddrProp.nAddrLen = 1;
    plcChannel.mCurrAddr.mAddrProp.nAddrValue = 0;
    plcChannel.mCurrAddr.mAddrProp.nPlcStationIndex = 0;
    plcChannel.mCurrAddr.mAddrProp.nRegIndex = 0;

    /*初始化当前协议*/
    mLinkInfo.sLibName = mPlcRateInfo.sProtocolName;
    mLinkInfo.mPlcChannelList.clear();
    mLinkInfo.mPlcChannelList.push_back(plcChannel);
    m_protocolLinkList.push_back(mLinkInfo);
    nLinkSize = m_protocolLinkList.size();

    /*对返回结果赋值*/
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
 * Function: 对读到的数据进行解包
 * Parameters:
 * Return: true or false
 ******************************************************************/
int ManageCenter::rcvStrForSlave(char *pRrcvData, int nLen, int nStationId, PLC_RATE_INFO mPlcRateInfo, vector<SEND_PACKAGE> &eSendPkg)
{
    /*加锁*/
    this->threadLock();

    /*取得协议的指针*/
    int nLinkIndex = -1;
    int nPlcIndex = -1;
    CMN_RW_ERROR_CODE eCmnInfo = CMN_NOMAL_CODE;

    bool bSuccess = getProtocolLinkInfo(mPlcRateInfo, nLinkIndex, nPlcIndex);
    if(!bSuccess)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_NOT_EXSIT;
        return eCmnInfo;
    }

    /*不是从站则返回*/
    PLC_FUN_TYPE ePlcFunType = m_protocolLinkList.at(nLinkIndex).mLibObj.getPlcType();
    if(PLC_SLAVE != ePlcFunType)
    {
        /*解锁*/
        this->threadUnlock();
        eCmnInfo = PROTO_TYPE_ERROR;
        return eCmnInfo;
    }

    /*把数据存储到list中*/
    uchar *pURrcvData = (uchar *)pRrcvData;
    vector<uchar > sourceList;
    for(int i = 0; i < nLen; i++)
    {
        sourceList.push_back(pURrcvData[i]);
    }

    /*对从站数据进行解包*/
    eCmnInfo = m_protocolLinkList.at(nLinkIndex).mLibObj.getSlaveDataStr(sourceList, nStationId, m_localAddrObj.m_localAddrValueList, eSendPkg);

    /*解锁*/
    this->threadUnlock();
    return eCmnInfo;
}
