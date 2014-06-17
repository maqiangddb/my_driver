#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*写数据返回的长度*/
#define WRITE_RETURN_LEN      8

/*读数据返回的数据头部长度*/
#define READ_HEAD_LEN         3

/*读数据返回的数据校验的长度*/
#define CHECK_LEN             2

PlcProtocol::PlcProtocol()
{
}

/******************************************************************
 * Function: change register value from string to int
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::addrStringToInt(ADDR_CHECK_SOURCE_PROP mAddrSource, ADDR_TARGET_PROP &mAddrTarget)
{
    /*先初始化目标地址*/
    mAddrTarget.nUserPlcId = mAddrSource.nUserPlcId;
    mAddrTarget.eAddrRWprop = mAddrSource.eAddrRWprop;
    mAddrTarget.nAddrLen = mAddrSource.nAddrLen;
    mAddrTarget.nAddrValue = 0;
    mAddrTarget.nPlcStationIndex = mAddrSource.nPlcStationIndex;

    bool bSuccess = false;
    int nRegIndex = 0;
    bSuccess = getRegIndexFromName(mAddrSource.sRegName, nRegIndex);
    if(!bSuccess) return false;

    mAddrTarget.nRegIndex = nRegIndex;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_O:
    case REG_TYPE_B:
    case REG_TYPE_SBIT:
    case REG_TYPE_NBIT:
        {
            /*按":"拆分*/
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string BLOCKNO = strList.at(0);
                string strELSE = strList.at(1);

                strList = DealWithString::split(strELSE,"/");

                if(strList.size() == 2)
                {
                    string WORDNO = strList.at(0);
                    string BITNO = strList.at(1);

                    int nBlockNo = DealWithString::stringToInt(BLOCKNO,bSuccess);
                    if(!bSuccess)
                    {
                        return false;
                    }

                    int nWordNo = DealWithString::stringToInt(WORDNO,bSuccess);
                    if(!bSuccess)
                    {
                        return false;
                    }
                    int nBitNo = DealWithString::stringToInt(BITNO,bSuccess);
                    if(!bSuccess)
                    {
                        return false;
                    }

                    if(REG_TYPE_I == eRegType)
                    {
                        if(1 != nBlockNo || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return false;
                        }
                    }
                    else if(REG_TYPE_O == eRegType)
                    {
                        if(0 != nBlockNo || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return false;
                        }
                    }
                    else if(REG_TYPE_B == eRegType)
                    {
                        if(nBlockNo < 3 || nBlockNo > 255 || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return false;
                        }
                    }
                    else if(REG_TYPE_SBIT == eRegType)
                    {
                        if(2 != nBlockNo || nWordNo < 0 || nBitNo < 0 || nBitNo > 7)
                        {
                            return false;
                        }
                    }
                    else if(REG_TYPE_NBIT == eRegType)
                    {
                        if(nBlockNo < 7 || nBlockNo > 255 || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return false;
                        }
                    }
                    else
                    {
                        return false;
                    }

                    mAddrTarget.nAddrValue = (nBlockNo*256 + nWordNo)*16 + nBitNo;
                    return true;
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
            break;
        }
    case REG_TYPE_S:
    case REG_TYPE_N:
    case REG_TYPE_F:
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            vector<string > strList = DealWithString::split(mAddrSource.sAddrValue,":");
            if(strList.size() == 2)
            {
                string BLOCKNO = strList.at(0);
                string WORDNO = strList.at(1);
                int nBlockNo = DealWithString::stringToInt(BLOCKNO,bSuccess);
                if(!bSuccess)
                {
                    return false;
                }
                int nWordNo = DealWithString::stringToInt(WORDNO,bSuccess);
                if(!bSuccess)
                {
                    return false;
                }

                if(REG_TYPE_S == eRegType)
                {
                    if(2 != nBlockNo)
                    {
                        return false;
                    }
                }
                else if(REG_TYPE_N == eRegType)
                {
                    if(nBlockNo < 7 || nBlockNo > 255)
                    {
                        return false;
                    }
                }
                else if(REG_TYPE_F == eRegType)
                {
                    if(nBlockNo < 8 || nBlockNo > 255)
                    {
                        return false;
                    }
                }
                else if(REG_TYPE_TACC == eRegType || REG_TYPE_TPRE == eRegType)
                {
                    if(nBlockNo < 4 || nBlockNo > 255)
                    {
                        return false;
                    }
                }
                else if(REG_TYPE_CACC == eRegType || REG_TYPE_CPRE == eRegType)
                {
                    if(nBlockNo < 5 || nBlockNo > 255)
                    {
                        return false;
                    }
                }
                else
                {
                    return false;
                }

                if(nWordNo > 255 || nWordNo < 0)
                {
                    return false;
                }
                mAddrTarget.nAddrValue = nBlockNo*256 + nWordNo;
            }
            else
            {
                return false;
            }
            break;
        }
    default:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
            return bSuccess;
            break;
        }
    }

    return true;
}

/******************************************************************
 * Function: get register name list
 * Parameters: null
 * Return: null
 ******************************************************************/
vector<string > PlcProtocol::getRegNameList(ADDR_REG_TYPE eAddrType)
{
    vector<string > sRegNameList;
    sRegNameList.clear();

    switch(eAddrType)
    {
    case BIT_ADDR_REG:
        {
            sRegNameList.push_back("I");
            sRegNameList.push_back("O");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SBIT");
            sRegNameList.push_back("NBIT");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("S");
            sRegNameList.push_back("N");
            sRegNameList.push_back("F");
            sRegNameList.push_back("TACC");
            sRegNameList.push_back("TPRE");
            sRegNameList.push_back("CACC");
            sRegNameList.push_back("CPRE");
            break;
        }
    case DWORT_ADDR_REG:
        {
            break;
        }
    case STRING_ADDR_REG:
        {
            break;
        }
    case ALL_ADDR_REG:
    case READ_ADDR_REG:
    case WRITE_ADDR_REG:
        {
            sRegNameList.push_back("I");
            sRegNameList.push_back("O");
            sRegNameList.push_back("B");
            sRegNameList.push_back("SBIT");
            sRegNameList.push_back("NBIT");
            sRegNameList.push_back("S");
            sRegNameList.push_back("N");
            sRegNameList.push_back("F");
            sRegNameList.push_back("TACC");
            sRegNameList.push_back("TPRE");
            sRegNameList.push_back("CACC");
            sRegNameList.push_back("CPRE");
            break;
        }
    default:
        {
            break;
        }
    }

    return sRegNameList;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::checkAddrCorrect(ADDR_CHECK_SOURCE_PROP mAddrProp, ADDR_REG_TYPE eAddrRegType)
{
    bool bSuccess = false;

    ADDR_TARGET_PROP mTargetAddr;
    bSuccess = this->addrStringToInt(mAddrProp, mTargetAddr);
    if(!bSuccess)
    {
        return ILLEGAL_VALUE;
    }

    int nRegIndex = 0;
    if(!getRegIndexFromName(mAddrProp.sRegName, nRegIndex)) return ILLEGAL_REG;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:                   //REG_TYPE_0X:
    case REG_TYPE_O:                   //REG_TYPE_1X:
    case REG_TYPE_B:                   //REG_TYPE_3X_BIT:
    case REG_TYPE_SBIT:                   //REG_TYPE_4X_BIT:
    case REG_TYPE_NBIT:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_S:                   //REG_TYPE_3X:
    case REG_TYPE_N:                   //REG_TYPE_4X:
    case REG_TYPE_TACC:                   //REG_TYPE_4X_DB:
    case REG_TYPE_TPRE:
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            if(eAddrRegType != WORT_ADDR_REG)
            {
                return IS_WORD_ADDR;
            }
            break;
        }
    case REG_TYPE_F:
        {
            if(eAddrRegType != WORT_ADDR_REG)
            {
                return IS_WORD_ADDR;
            }
            break;
        }
    default:
        {
            return ILLEGAL_REG;
            break;
        }
    }

    return ADDR_CORRECT;
}

/******************************************************************
 * Function: 地址的多重复制
 * Parameters: address list
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::addrsMutilCopy(ADDR_CHECK_SOURCE_PROP mStartAddr, vector<ADDR_CHECK_SOURCE_PROP> &mSourceAddrList, int nInterval, int nCopyNum)
{
    /*初始化*/
    mSourceAddrList.clear();
    bool bSuccess = false;
    ADDR_CHECK_SOURCE_PROP mTmpAddr;
    mTmpAddr = mStartAddr;

    int nRegIndex = 0;
    if(!getRegIndexFromName(mTmpAddr.sRegName, nRegIndex)) return ILLEGAL_REG;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
    case REG_TYPE_O:
    case REG_TYPE_B:
    case REG_TYPE_SBIT:
    case REG_TYPE_NBIT:
        {
            /*按“.”拆分*/
            vector<string > strList = DealWithString::split(mStartAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string BLOCKNO = strList.at(0);
                string strELSE = strList.at(1);

                strList = DealWithString::split(strELSE,"/");

                if(strList.size() == 2)
                {
                    string WORDNO = strList.at(0);
                    string BITNO = strList.at(1);

                    int nBlockNo = DealWithString::stringToInt(BLOCKNO,bSuccess);
                    if(!bSuccess)
                    {
                        return ILLEGAL_VALUE;
                    }

                    int nWordNo = DealWithString::stringToInt(WORDNO,bSuccess);
                    if(!bSuccess)
                    {
                        return ILLEGAL_VALUE;
                    }
                    int nBitNo = DealWithString::stringToInt(BITNO,bSuccess);
                    if(!bSuccess)
                    {
                        return ILLEGAL_VALUE;
                    }

                    if(REG_TYPE_I == eRegType)
                    {
                        if(1 != nBlockNo || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return ADDR_FORMAT_ERROR;
                        }
                        else
                        {
                            char buf[10] = {0};
                            string sTmpStr;
                            for(int i = 0; i < nCopyNum; i++)
                            {
                                if(nBitNo + nInterval > 15)
                                {
                                    nWordNo += (nBitNo + nInterval)/16;
                                    if(nWordNo > 255)
                                    {
                                        break;
                                    }
                                    nBitNo = (nBitNo + nInterval)%16;
                                }
                                else
                                {
                                    nBitNo += nInterval;
                                }

                                /*添加块部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBlockNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue = sTmpStr + ":";

                                /*添加word部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nWordNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;
                                mTmpAddr.sAddrValue += "/";

                                /*添加BIT部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBitNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;

                                mSourceAddrList.push_back(mTmpAddr);
                            }
                            return ADDR_CORRECT;
                        }
                    }
                    else if(REG_TYPE_O == eRegType)
                    {
                        if(0 != nBlockNo || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return ADDR_FORMAT_ERROR;
                        }
                        else
                        {
                            char buf[10] = {0};
                            string sTmpStr;
                            for(int i = 0; i < nCopyNum; i++)
                            {
                                if(nBitNo + nInterval > 15)
                                {
                                    nWordNo += (nBitNo + nInterval)/16;
                                    if(nWordNo > 255)
                                    {
                                        break;
                                    }
                                    nBitNo = (nBitNo + nInterval)%16;
                                }
                                else
                                {
                                    nBitNo += nInterval;
                                }

                                /*添加块部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBlockNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue = sTmpStr + ":";

                                /*添加word部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nWordNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;
                                mTmpAddr.sAddrValue += "/";

                                /*添加BIT部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBitNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;

                                mSourceAddrList.push_back(mTmpAddr);
                            }
                            return ADDR_CORRECT;
                        }
                    }
                    else if(REG_TYPE_B == eRegType)
                    {
                        if(nBlockNo < 3 || nBlockNo > 255 || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return ADDR_FORMAT_ERROR;
                        }
                        else
                        {
                            char buf[10] = {0};
                            string sTmpStr;
                            for(int i = 0; i < nCopyNum; i++)
                            {
                                if(nBitNo + nInterval > 15)
                                {
                                    nWordNo += (nBitNo + nInterval)/16;
                                    if(nWordNo > 255)
                                    {
                                        nBlockNo += nWordNo/256;
                                        nWordNo = nWordNo%256;
                                        if(nBlockNo > 255)
                                        {
                                            break;
                                        }
                                    }
                                    nBitNo = (nBitNo + nInterval)%16;
                                }
                                else
                                {
                                    nBitNo += nInterval;
                                }

                                /*添加块部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBlockNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue = sTmpStr + ":";

                                /*添加word部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nWordNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;
                                mTmpAddr.sAddrValue += "/";

                                /*添加BIT部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBitNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;

                                mSourceAddrList.push_back(mTmpAddr);
                            }
                            return ADDR_CORRECT;
                        }
                    }
                    else if(REG_TYPE_SBIT == eRegType)
                    {
                        if(2 != nBlockNo || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return ADDR_FORMAT_ERROR;
                        }
                        else
                        {
                            char buf[10] = {0};
                            string sTmpStr;
                            for(int i = 0; i < nCopyNum; i++)
                            {
                                if(nBitNo + nInterval > 15)
                                {
                                    nWordNo += (nBitNo + nInterval)/16;
                                    if(nWordNo > 255)
                                    {
                                        break;
                                    }
                                    nBitNo = (nBitNo + nInterval)%16;
                                }
                                else
                                {
                                    nBitNo += nInterval;
                                }

                                /*添加块部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBlockNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue = sTmpStr + ":";

                                /*添加word部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nWordNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;
                                mTmpAddr.sAddrValue += "/";

                                /*添加BIT部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBitNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;

                                mSourceAddrList.push_back(mTmpAddr);
                            }
                            return ADDR_CORRECT;
                        }
                    }
                    else if(REG_TYPE_NBIT == eRegType)
                    {
                        if(nBlockNo < 7 || nBlockNo > 255 || nWordNo > 255 || nWordNo < 0 || nBitNo < 0 || nBitNo > 15)
                        {
                            return ADDR_FORMAT_ERROR;
                        }
                        else
                        {
                            char buf[10] = {0};
                            string sTmpStr;
                            for(int i = 0; i < nCopyNum; i++)
                            {
                                if(nBitNo + nInterval > 15)
                                {
                                    nWordNo += (nBitNo + nInterval)/16;
                                    if(nWordNo > 255)
                                    {
                                        nBlockNo += nWordNo/256;
                                        nWordNo = nWordNo%256;
                                        if(nBlockNo > 255)
                                        {
                                            break;
                                        }
                                    }
                                    nBitNo = (nBitNo + nInterval)%16;
                                }
                                else
                                {
                                    nBitNo += nInterval;
                                }

                                /*添加块部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBlockNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue = sTmpStr + ":";

                                /*添加word部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nWordNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;
                                mTmpAddr.sAddrValue += "/";

                                /*添加BIT部分*/
                                memset(buf, 0, 10);
                                sprintf(buf, "%d", nBitNo);
                                sTmpStr = buf;
                                mTmpAddr.sAddrValue += sTmpStr;

                                mSourceAddrList.push_back(mTmpAddr);
                            }
                            return ADDR_CORRECT;
                        }
                    }
                    else
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                }
                else
                {
                    return ADDR_FORMAT_ERROR;
                }
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    case REG_TYPE_S:
    case REG_TYPE_N:
    case REG_TYPE_F:
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            vector<string > strList = DealWithString::split(mTmpAddr.sAddrValue,":");
            if(strList.size() == 2)
            {
                string BLOCKNO = strList.at(0);
                string WORDNO = strList.at(1);
                int nBlockNo = DealWithString::stringToInt(BLOCKNO,bSuccess);
                if(!bSuccess)
                {
                    return ILLEGAL_VALUE;
                }
                int nWordNo = DealWithString::stringToInt(WORDNO,bSuccess);
                if(!bSuccess)
                {
                    return ILLEGAL_VALUE;
                }

                if(nWordNo > 255 || nWordNo < 0)
                {
                    return ADDR_FORMAT_ERROR;
                }

                if(REG_TYPE_S == eRegType)
                {
                    if(2 != nBlockNo)
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                    else
                    {
                        char buf[10] = {0};
                        string sTmpStr;
                        for(int i = 0; i < nCopyNum; i++)
                        {
                            if(nWordNo + nInterval > 255)
                            {
                                break;
                            }
                            else
                            {
                                nWordNo += nInterval;
                            }

                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加word部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr;

                            mSourceAddrList.push_back(mTmpAddr);
                        }
                        return ADDR_CORRECT;
                    }
                }
                else if(REG_TYPE_N == eRegType)
                {
                    if(nBlockNo < 7 || nBlockNo > 255)
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                    else
                    {
                        char buf[10] = {0};
                        string sTmpStr;
                        for(int i = 0; i < nCopyNum; i++)
                        {
                            if(nWordNo + nInterval > 255)
                            {
                                nBlockNo += (nWordNo + nInterval)/256;
                                nWordNo = (nWordNo + nInterval)%256;
                                if(nBlockNo > 255)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                nWordNo += nInterval;
                            }

                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加word部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr;

                            mSourceAddrList.push_back(mTmpAddr);
                        }
                        return ADDR_CORRECT;
                    }
                }
                else if(REG_TYPE_F == eRegType)
                {
                    if(nBlockNo < 8 || nBlockNo > 255)
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                    else
                    {
                        char buf[10] = {0};
                        string sTmpStr;
                        for(int i = 0; i < nCopyNum; i++)
                        {
                            if(nWordNo + nInterval > 255)
                            {
                                nBlockNo += (nWordNo + nInterval)/256;
                                nWordNo = (nWordNo + nInterval)%256;
                                if(nBlockNo > 255)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                nWordNo += nInterval;
                            }

                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加word部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr;

                            mSourceAddrList.push_back(mTmpAddr);
                        }
                        return ADDR_CORRECT;
                    }
                }
                else if(REG_TYPE_TACC == eRegType || REG_TYPE_TPRE == eRegType)
                {
                    if(nBlockNo < 4 || nBlockNo > 255)
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                    else
                    {
                        char buf[10] = {0};
                        string sTmpStr;
                        for(int i = 0; i < nCopyNum; i++)
                        {
                            if(nWordNo + nInterval > 255)
                            {
                                nBlockNo += (nWordNo + nInterval)/256;
                                nWordNo = (nWordNo + nInterval)%256;
                                if(nBlockNo > 255)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                nWordNo += nInterval;
                            }

                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加word部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr;

                            mSourceAddrList.push_back(mTmpAddr);
                        }
                        return ADDR_CORRECT;
                    }
                }
                else if(REG_TYPE_CACC == eRegType || REG_TYPE_CPRE == eRegType)
                {
                    if(nBlockNo < 5 || nBlockNo > 255)
                    {
                        return ADDR_FORMAT_ERROR;
                    }
                    else
                    {
                        char buf[10] = {0};
                        string sTmpStr;
                        for(int i = 0; i < nCopyNum; i++)
                        {
                            if(nWordNo + nInterval > 255)
                            {
                                nBlockNo += (nWordNo + nInterval)/256;
                                nWordNo = (nWordNo + nInterval)%256;
                                if(nBlockNo > 255)
                                {
                                    break;
                                }
                            }
                            else
                            {
                                nWordNo += nInterval;
                            }

                            /*添加块部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nBlockNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue = sTmpStr + ":";

                            /*添加word部分*/
                            memset(buf, 0, 10);
                            sprintf(buf, "%d", nWordNo);
                            sTmpStr = buf;
                            mTmpAddr.sAddrValue += sTmpStr;

                            mSourceAddrList.push_back(mTmpAddr);
                        }
                        return ADDR_CORRECT;
                    }
                }
                else
                {
                    return ILLEGAL_REG;
                }
            }
            else
            {
                return ADDR_FORMAT_ERROR;
            }
            break;
        }
    default:
        {
            break;
        }
    }

    return ADDR_CORRECT;
}

/******************************************************************
 * Function: unpack read data
 * Parameters: address list
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackReadData(UNPACK_NEED_PROP &mUnpackProp, vector<unsigned char> rcvList, vector<unsigned char> &targetList)
{
    CMN_RW_ERROR_CODE eCmnInfo = checkRcvData(mUnpackProp, rcvList, CHECK_READ);
    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;


    if(mUnpackProp.bNeedSetData)
    {
        /*取有效数据*/
        int nSize = rcvList.size();
        vector<uchar> nTmpRcvList ;
        nTmpRcvList.clear();
        for(int i = 0; i < nSize; i++)
        {
            nTmpRcvList.push_back(rcvList.at(i));
            if(i < nSize -1)
            {
                if(rcvList.at(i) == 0x10 && rcvList.at(i+1) == 0x10)
                {
                    i++;
                }
            }
        }

        targetList.clear();
        nSize = nTmpRcvList.size();
        for(int i = 10; i < nSize - 3; i++)
        {
            targetList.push_back(nTmpRcvList.at(i));
        }

        eCmnInfo = CMN_NOMAL_CODE;
    }

    return eCmnInfo;
}

/******************************************************************
 * Function: check recerve is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::checkRcvData(UNPACK_NEED_PROP &mUnpackProp, vector<uchar> rcvList, CHECK_TYPE eCheckType)
{
    CMN_RW_ERROR_CODE eCmnInfo = OTHER_ERROR_CODE;

    unsigned short usCalCRC = 0;
    unsigned char ucRetCRC_H = 0;
    unsigned char ucRetCRC_L = 0;
    unsigned char ucCalCRC_H = 0;
    unsigned char ucCalCRC_L = 0;

    vector<uchar> qby_Temp;
    qby_Temp.clear();
    int nRcvSize = rcvList.size();
    int bRepeatFlag = 0;

    if(1 == mUnpackProp.nCurrTimes)
    {
        if(nRcvSize > 8)
        {
            ucRetCRC_H = rcvList.at(nRcvSize - 1);//PLC返回数据中的校验_高位
            ucRetCRC_L = rcvList.at(nRcvSize - 2);//PLC返回数据中的校验_低位

            if (rcvList.at(0) == 0x10 && rcvList.at(1) == 0x06 && rcvList.at(2) == 0x10 && rcvList.at(3) == 0x02
                && rcvList.at(nRcvSize - 3) == 0x03 && rcvList.at(nRcvSize - 4) == 0x10)//开始结束符一致
            {
                for (int i=0; i< (nRcvSize - 8); i++)//减去开始的10 06 10 02 和后面的10 03 CRC_L CRC_H
                {
                    if (rcvList.at(4 + i) == 0x10)//数据中有0x10会重复出现，故校验时需剔除多余0x10
                    {
                        if (bRepeatFlag == 1)//重复出现第二个
                        {
                            bRepeatFlag = 0;//直接剔除，即不添加到qby_Temp中做校验
                        }
                        else
                        {
                            qby_Temp.push_back(rcvList.at(4 + i));//第一个出现0x10，添加到qby_Temp中做校验
                            bRepeatFlag = 1;//标志紧接着0x10需剔除
                        }
                    }
                    else
                    {
                        qby_Temp.push_back(rcvList.at(4 + i));//不是0x10，直接添加到qby_Temp中做校验
                        bRepeatFlag = 0;
                    }
                }
                qby_Temp.push_back(0x03);

                usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);//计算返回数据的校验
                ucCalCRC_H = (usCalCRC >> 8) & 0xff;//计算校验_高位
                ucCalCRC_L = (usCalCRC >> 0) & 0xff;//计算校验_低位

                if ((ucCalCRC_H == ucRetCRC_H) && (ucCalCRC_L == ucRetCRC_L))//校验相同，数据无误
                {
                    if (eCheckType == CHECK_READ)
                    {
                        /*设置数据*/
                        mUnpackProp.bNeedSetData = true;
                        eCmnInfo = CMN_NOMAL_CODE;
                    }
                    else if (eCheckType == CHECK_WRITE)
                    {
                        eCmnInfo = CMN_NOMAL_CODE;//写不处理
                    }
                }
                else
                {
                    eCmnInfo = DATA_CHECK_ERROR;
                }
            }
        }
    }
    //else if(2 == mUnpackProp.nCurrTimes)
    //{
        if(2 == nRcvSize)
        {
            eCmnInfo = CMN_NOMAL_CODE;
        }
    //}
    return eCmnInfo;
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    return false;
}

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes: 次数从1开始
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    return CMN_NOMAL_CODE;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*初始化*/
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

//    if(nValueList.empty()) return mSendPackge;

    /*对当前打包的地址赋值*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;
    mCurrAddr.mAddrProp = mAddrInfo.mAddrProp;

    /*功能码*/
    switch(mCmnProp.eSendType)
    {
    case CMN_READ:
        {
            mCurrAddr.nValueList.clear();
            return makeReadPackage(mCmnProp, nCmnTimes, mSendPackge);
            break;
        }
    case CMN_WRITE:
        {
            mCurrAddr.nValueList = mAddrInfo.nValueList;
            return makeWritePackage(mCmnProp, nCmnTimes, mSendPackge, mCurrAddr);
            break;
        }
    case CMN_READ_BEFORE_WRITE:
        {
            break;
        }
    case CMN_WRITE_AFTER_READ:
        {
            break;
        }
    default:
        {
            break;
        }
    }

    return true;
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makeReadPackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge)
{
    /*初始化*/
    ADDR_TARGET_PROP mAddrInfo = mCmnProp.mAddrInfo.mAddrProp;

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    mSendPackge.sendDataList.push_back(0x10);
    mSendPackge.sendDataList.push_back(0x06);

    int nAddrLen = mAddrInfo.nAddrLen;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_S:
    case REG_TYPE_N:
        {
            nAddrLen = mAddrInfo.nAddrLen;
            vector<uchar> qby_Temp;
            qby_Temp.clear();
            /*vector index = 0*/
            uchar nTmpValue = 0x10;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            uchar temp1  = mAddrInfo.nPlcStationIndex & 0xff;
            qby_Temp.push_back(temp1);//qby index = 0
            qby_Temp.push_back(0x00); //qby index = 1
            if(temp1 == 0x10)
            {
                /*vector index = 2*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 2*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*vector index = 3 或者 4*/
            nTmpValue = 0x00;
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x0f);
            mSendPackge.sendDataList.push_back(0x00);
            qby_Temp.push_back(0x0f); //qby index = 2
            qby_Temp.push_back(0x00); //qby index = 3

            temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
            uchar temp2 = (nCmnTimes >> 0) & 0xff;

            mSendPackge.sendDataList.push_back(temp1);
            qby_Temp.push_back(temp1); //qby index = 4
            qby_Temp.push_back(temp2); //qby index = 5

            if (temp2 == 0x10)                                         //低位可能出现0x10情况
            {
                /*vector index = 7 或者 8*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 8 或者 9*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 7 或者 8*/
                nTmpValue = temp2;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = 0xa2;
            mSendPackge.sendDataList.push_back(nTmpValue);
            qby_Temp.push_back(nTmpValue); //qby index = 6

            temp1 = (nAddrLen*2) & 0xff;                               //读取长度（字节数）
            qby_Temp.push_back(temp1); //qby index = 7

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            temp1 = (mAddrInfo.nAddrValue/256) & 0xff;                             //Bolck No.
            qby_Temp.push_back(temp1);//qby index = 8
            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            switch (eRegType)
            {
            case REG_TYPE_S:
                mSendPackge.sendDataList.push_back(0x84);
                break;
            case REG_TYPE_N:
                mSendPackge.sendDataList.push_back(0x89);
                break;
            default:
                break;
            }

            //qby index = 9
            qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

            temp2 = ((mAddrInfo.nAddrValue%256) >> 8) & 0xff;                      //读取地址，哪个块内的哪个字,低8位先发
            temp1 = ((mAddrInfo.nAddrValue%256) >> 0) & 0xff;

            qby_Temp.push_back(temp1);//qby index = 10
            qby_Temp.push_back(temp2);//qby index = 11

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            if (temp2 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp2);
            }

            mSendPackge.sendDataList.push_back(0x10);                         //DLE
            mSendPackge.sendDataList.push_back(0x03);                         //ETX
            qby_Temp.push_back(0x03);     //qby index = 12      ETX,CRC校验不需要DLE

            ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
            temp1 = (usCalCRC >> 8) & 0xff;
            temp2 = (usCalCRC >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(temp2 & 0xff);
            mSendPackge.sendDataList.push_back(temp1 & 0xff);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = -1;

            //第二个包
//            mSendPackge.sendDataList.clear();
//            mSendPackge.sendDataList.push_back(0x10);
//            mSendPackge.sendDataList.push_back(0x06);

//            mSendPackge.nReturnLen = 0;
//            mSendPkgList.push_back(mSendPackge);
            nCmnTimes++;
            return true;
            break;
        }
    case REG_TYPE_I:
    case REG_TYPE_O:
    case REG_TYPE_B:
    case REG_TYPE_SBIT:
    case REG_TYPE_NBIT:
        {
            if(mAddrInfo.nAddrLen%16 == 0)
            {
                nAddrLen = mAddrInfo.nAddrLen/16;
            }
            else
            {
                nAddrLen = mAddrInfo.nAddrLen/16 + 1;
            }
            vector<uchar> qby_Temp;
            qby_Temp.clear();
            /*vector index = 0*/
            uchar nTmpValue = 0x10;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            uchar temp1  = mAddrInfo.nPlcStationIndex & 0xff;
            qby_Temp.push_back(temp1);//qby index = 0
            qby_Temp.push_back(0x00); //qby index = 1
            if(temp1 == 0x10)
            {
                /*vector index = 2*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 2*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*vector index = 3 或者 4*/
            nTmpValue = 0x00;
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x0f);
            mSendPackge.sendDataList.push_back(0x00);
            qby_Temp.push_back(0x0f); //qby index = 2
            qby_Temp.push_back(0x00); //qby index = 3

            temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
            uchar temp2 = (nCmnTimes >> 0) & 0xff;

            mSendPackge.sendDataList.push_back(temp1);
            qby_Temp.push_back(temp1); //qby index = 4
            qby_Temp.push_back(temp2); //qby index = 5

            if (temp2 == 0x10)                                         //低位可能出现0x10情况
            {
                /*vector index = 7 或者 8*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 8 或者 9*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 7 或者 8*/
                nTmpValue = temp2;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = 0xa2;
            mSendPackge.sendDataList.push_back(nTmpValue);
            qby_Temp.push_back(nTmpValue); //qby index = 6

            temp1 = (nAddrLen*2) & 0xff;                               //读取长度（字节数）
            qby_Temp.push_back(temp1); //qby index = 7

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            temp1 = ((mAddrInfo.nAddrValue/16)/256) & 0xff;                             //Bolck No.
            qby_Temp.push_back(temp1);//qby index = 8
            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            switch (eRegType)
            {
            case REG_TYPE_I:
                mSendPackge.sendDataList.push_back(0x83);
                break;
            case REG_TYPE_O:
                mSendPackge.sendDataList.push_back(0x82);
                break;
            case REG_TYPE_B:
                mSendPackge.sendDataList.push_back(0x85);
                break;
            case REG_TYPE_S:
            case REG_TYPE_SBIT:
                mSendPackge.sendDataList.push_back(0x84);
                break;
            case REG_TYPE_N:
            case REG_TYPE_NBIT:
                mSendPackge.sendDataList.push_back(0x89);
                break;
            default:
                break;
            }

            //qby index = 9
            qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

            temp2 = (((mAddrInfo.nAddrValue/16)%256) >> 8) & 0xff;                      //读取地址，哪个块内的哪个字,低8位先发
            temp1 = (((mAddrInfo.nAddrValue/16)%256) >> 0) & 0xff;

            qby_Temp.push_back(temp1);//qby index = 10
            qby_Temp.push_back(temp2);//qby index = 11

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            if (temp2 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp2);
            }

            mSendPackge.sendDataList.push_back(0x10);                         //DLE
            mSendPackge.sendDataList.push_back(0x03);                         //ETX
            qby_Temp.push_back(0x03);     //qby index = 12      ETX,CRC校验不需要DLE

            ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
            temp1 = (usCalCRC >> 8) & 0xff;
            temp2 = (usCalCRC >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(temp2 & 0xff);
            mSendPackge.sendDataList.push_back(temp1 & 0xff);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = -1;

            //第二个包
//            mSendPackge.sendDataList.clear();
//            mSendPackge.sendDataList.push_back(0x10);
//            mSendPackge.sendDataList.push_back(0x06);

//            mSendPackge.nReturnLen = 0;
//            mSendPkgList.push_back(mSendPackge);
            nCmnTimes++;
            return true;
            break;
        }
    case REG_TYPE_F:
        {
            vector<uchar> qby_Temp;
            qby_Temp.clear();
            /*vector index = 0*/
            uchar nTmpValue = 0x10;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            uchar temp1  = mAddrInfo.nPlcStationIndex & 0xff;
            qby_Temp.push_back(temp1);//qby index = 0
            qby_Temp.push_back(0x00); //qby index = 1
            if(temp1 == 0x10)
            {
                /*vector index = 2*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 2*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*vector index = 3 或者 4*/
            nTmpValue = 0x00;
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x0f);
            mSendPackge.sendDataList.push_back(0x00);

            qby_Temp.push_back(0x0f); //qby index = 2
            qby_Temp.push_back(0x00); //qby index = 3

            temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
            uchar temp2 = (nCmnTimes >> 0) & 0xff;

            mSendPackge.sendDataList.push_back(temp1);

            qby_Temp.push_back(temp1); //qby index = 4
            qby_Temp.push_back(temp2); //qby index = 5

            if (temp2 == 0x10)                                         //低位可能出现0x10情况
            {
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                nTmpValue = temp2;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = 0xa2;
            mSendPackge.sendDataList.push_back(nTmpValue);
            qby_Temp.push_back(nTmpValue); //qby index = 6

            temp1 = (mAddrInfo.nAddrLen*4) & 0xff;                               //读取长度（字节数）
            qby_Temp.push_back(temp1); //qby index = 7

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            temp1 = (mAddrInfo.nAddrValue/256) & 0xff;                             //Bolck No.
            qby_Temp.push_back(temp1);
            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            mSendPackge.sendDataList.push_back(0x8A);
            qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));
            //读取地址，哪个块内的哪个字
            temp1 = (mAddrInfo.nAddrValue%256) & 0xff;

            qby_Temp.push_back(temp1);
            qby_Temp.push_back(0x0);

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x10);                         //DLE
            mSendPackge.sendDataList.push_back(0x03);                         //ETX
            qby_Temp.push_back(0x03);                                       //ETX,CRC校验不需要DLE

            ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
            temp1 = (usCalCRC >> 8) & 0xff;
            temp2 = (usCalCRC >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(temp2 & 0xff);
            mSendPackge.sendDataList.push_back(temp1 & 0xff);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = -1;

            //第二个包
//            mSendPackge.sendDataList.clear();
//            mSendPackge.sendDataList.push_back(0x10);
//            mSendPackge.sendDataList.push_back(0x06);

//            mSendPackge.nReturnLen = 0;
//            mSendPkgList.push_back(mSendPackge);
            nCmnTimes++;
            return true;
            break;
        }
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            vector<uchar> qby_Temp;
            qby_Temp.clear();
            /*vector index = 0*/

            uchar nTmpValue = 0x10;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 0x02;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            uchar temp1  = mAddrInfo.nPlcStationIndex & 0xff;
            qby_Temp.push_back(temp1);//qby index = 0
            qby_Temp.push_back(0x00); //qby index = 1
            if(temp1 == 0x10)
            {
                /*vector index = 2*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 3*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 2*/
                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*vector index = 3 或者 4*/
            nTmpValue = 0x00;
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x0f);
            mSendPackge.sendDataList.push_back(0x00);
            qby_Temp.push_back(0x0f); //qby index = 2
            qby_Temp.push_back(0x00); //qby index = 3

            temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
            uchar temp2 = (nCmnTimes >> 0) & 0xff;

            mSendPackge.sendDataList.push_back(temp1);
            qby_Temp.push_back(temp1); //qby index = 4
            qby_Temp.push_back(temp2); //qby index = 5

            if (temp2 == 0x10)                                         //低位可能出现0x10情况
            {
                /*vector index = 7 或者 8*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
                /*vector index = 8 或者 9*/
                nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 7 或者 8*/
                nTmpValue = temp2;
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = 0xa2;
            mSendPackge.sendDataList.push_back(nTmpValue);
            qby_Temp.push_back(nTmpValue); //qby index = 6

            temp1 = (mAddrInfo.nAddrLen*6) & 0xff;                               //读取长度（字节数）
            qby_Temp.push_back(temp1); //qby index = 7

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            temp1 = (mAddrInfo.nAddrValue/256) & 0xff;                             //Bolck No.
            qby_Temp.push_back(temp1);
            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }

            switch (eRegType)
            {
            case REG_TYPE_TACC:
            case REG_TYPE_TPRE:
                mSendPackge.sendDataList.push_back(0x86);
                break;
            case REG_TYPE_CACC:
            case REG_TYPE_CPRE:
                mSendPackge.sendDataList.push_back(0x87);
                break;
            default:
                break;
            }

            qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));
            //读取地址，哪个块内的哪个字
            temp1 = ((mAddrInfo.nAddrValue%256) >> 0) & 0xff;

            qby_Temp.push_back(temp1);
            qby_Temp.push_back(0x0);

            if (temp1 == 0x10)
            {
                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x10);
            }
            else
            {
                mSendPackge.sendDataList.push_back(temp1);
            }
            mSendPackge.sendDataList.push_back(0x00);

            mSendPackge.sendDataList.push_back(0x10);                         //DLE
            mSendPackge.sendDataList.push_back(0x03);                         //ETX
            qby_Temp.push_back(0x03);                                       //ETX,CRC校验不需要DLE

            ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
            temp1 = (usCalCRC >> 8) & 0xff;
            temp2 = (usCalCRC >> 0) & 0xff;
            mSendPackge.sendDataList.push_back(temp2 & 0xff);
            mSendPackge.sendDataList.push_back(temp1 & 0xff);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = -1;

//            //第二个包
//            mSendPackge.sendDataList.clear();
//            mSendPackge.sendDataList.push_back(0x10);
//            mSendPackge.sendDataList.push_back(0x06);

//            mSendPackge.nReturnLen = 0;
//            mSendPkgList.push_back(mSendPackge);
            nCmnTimes = nCmnTimes + 8;
            return true;
            break;
        }
    default:
        {
            return false;
            break;
        }
    }
}

/******************************************************************
 * Function: check address is correct
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::makeWritePackage(CMN_NEED_PROP mCmnProp, unsigned int &nCmnTimes, SEND_PACKAGE &mSendPackge, ADDR_RW_STRUCT &mCurrAddr)
{
    /*初始化*/
    ADDR_RW_STRUCT mAddrInfo = mCmnProp.mAddrInfo;

    mSendPackge.nTotalPkgNum = 2;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    /*站号 vector index = 0*/
    int BlockNo = 0;
    int WordNo = 0;
    int BitNo = 0;
//    uchar nTmpValue = 0;
    //memcpy(&nTmpValue, &mAddrInfo.mAddrProp.nPlcStationIndex, 1);
    //mSendPackge.sendDataList.push_back(nTmpValue);

//    if(nTmpValue == 0)
//    {
//        mSendPackge.nReturnLen = 0;
//    }
//    else
//    {
//        mSendPackge.nReturnLen = WRITE_RETURN_LEN;
//    }

    if(mSendPackge.nPkgIndex == 1)
    {
        REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
        switch(eRegType)
        {
        case REG_TYPE_O:
        case REG_TYPE_B:
        case REG_TYPE_SBIT:
        case REG_TYPE_NBIT:
            {
                int iReturnPos = 0;
                BlockNo = (mAddrInfo.mAddrProp.nAddrValue/16)/256;
                WordNo = (mAddrInfo.mAddrProp.nAddrValue/16)%256;
                BitNo = mAddrInfo.mAddrProp.nAddrValue%16;

                vector<uchar> qby_Temp;
                qby_Temp.clear();
                /*vector index = 0*/
                uchar nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 1*/
                nTmpValue = 0x02;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 2*/
                uchar temp1  = mAddrInfo.mAddrProp.nPlcStationIndex & 0xff;
                qby_Temp.push_back(temp1);//qby index = 0
                qby_Temp.push_back(0x00); //qby index = 1
                if(temp1 == 0x10)
                {
                    /*vector index = 2*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    /*vector index = 3*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 2*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                /*vector index = 3 或者 4*/
                nTmpValue = 0x00;
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.sendDataList.push_back(0x0f);
                mSendPackge.sendDataList.push_back(0x00);
                qby_Temp.push_back(0x0f); //qby index = 2
                qby_Temp.push_back(0x00); //qby index = 3

                temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
                uchar temp2 = (nCmnTimes >> 0) & 0xff;

                qby_Temp.push_back(temp1); //qby index = 4
                qby_Temp.push_back(temp2); //qby index = 5

                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);

                if (temp2 == 0x10)                                         //低位可能出现0x10情况
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    /*vector index = 8 或者 9*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = temp2;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                nTmpValue = 0xab;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 6

                nTmpValue = 0x02;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 7

                qby_Temp.push_back(BlockNo & 0xff); //qby index = 8

                if (BlockNo == 0x10)                                        //Bolck No.
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(BlockNo & 0xff);
                }

                switch (eRegType)
                {
                case REG_TYPE_O:
                    mSendPackge.sendDataList.push_back(0x82);
                    break;
                case REG_TYPE_B:
                    mSendPackge.sendDataList.push_back(0x85);
                    break;
                case REG_TYPE_SBIT:
                    mSendPackge.sendDataList.push_back(0x84);
                    break;
                case REG_TYPE_NBIT:
                    mSendPackge.sendDataList.push_back(0x89);
                    break;
                default:
                    break;
                }

                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

                qby_Temp.push_back(WordNo & 0xff);                               //Word No.
                qby_Temp.push_back(0x0);
                if (WordNo == 0x10)
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(WordNo & 0xff);
                }
                mSendPackge.sendDataList.push_back(0x0);

                temp1 = (0x0001 << BitNo) & 0xffff;                        //写入该字的哪一位
                temp2 = (temp1 >> 8) & 0xff;                              //Bit No.,低8位先发
                uchar temp3 = (temp1 >> 0) & 0xff;
                qby_Temp.push_back(temp3);
                qby_Temp.push_back(temp2);
                if (temp3 == 0x10)
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(temp3);
                }

                if (temp2 == 0x10)
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(temp2);
                }

                if (mAddrInfo.nValueList.at(0) == 1)                            //置位
                {
                    mSendPackge.sendDataList.push_back(0xff);
                    mSendPackge.sendDataList.push_back(0xff);
                }
                else                                                        //复位
                {
                    mSendPackge.sendDataList.push_back(0x00);
                    mSendPackge.sendDataList.push_back(0x00);
                }

                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 2));
                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x03);
                qby_Temp.push_back(0x03);

                ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
                temp1 = (usCalCRC >> 8) & 0xff;
                temp2 = (usCalCRC >> 0) & 0xff;
                mSendPackge.sendDataList.push_back(temp2 & 0xff);
                mSendPackge.sendDataList.push_back(temp1 & 0xff);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = -1;

                nCmnTimes++;
                break;
            }
        case REG_TYPE_S:
        case REG_TYPE_N:
        case REG_TYPE_F:
            {
                int iReturnPos = 0;
                BlockNo = (mAddrInfo.mAddrProp.nAddrValue)/256;
                WordNo = (mAddrInfo.mAddrProp.nAddrValue)%256;

                vector<uchar> qby_Temp;
                qby_Temp.clear();
                /*vector index = 0*/
                uchar nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 1*/
                nTmpValue = 0x02;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 2*/
                uchar temp1  = mAddrInfo.mAddrProp.nPlcStationIndex & 0xff;
                qby_Temp.push_back(temp1);//qby index = 0
                qby_Temp.push_back(0x00); //qby index = 1
                if(temp1 == 0x10)
                {
                    /*vector index = 2*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    /*vector index = 3*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 2*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                /*vector index = 3 或者 4*/
                nTmpValue = 0x00;
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.sendDataList.push_back(0x0f);
                mSendPackge.sendDataList.push_back(0x00);
                qby_Temp.push_back(0x0f); //qby index = 2
                qby_Temp.push_back(0x00); //qby index = 3

                temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
                uchar temp2 = (nCmnTimes >> 0) & 0xff;

                qby_Temp.push_back(temp1); //qby index = 4
                qby_Temp.push_back(temp2); //qby index = 5

                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);

                if (temp2 == 0x10)                                         //低位可能出现0x10情况
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    /*vector index = 8 或者 9*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = temp2;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                nTmpValue = 0xa9;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 6

                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen*2) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 7

                qby_Temp.push_back(BlockNo & 0xff); //qby index = 8

                if (BlockNo == 0x10)                                        //Bolck No.
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(BlockNo & 0xff);
                }

                switch (eRegType)
                {
                case REG_TYPE_S:
                    mSendPackge.sendDataList.push_back(0x84);
                    break;
                case REG_TYPE_N:
                    mSendPackge.sendDataList.push_back(0x89);
                    break;
                case REG_TYPE_F:
                    mSendPackge.sendDataList.push_back(0x8a);
                    break;
                default:
                    break;
                }

                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

                qby_Temp.push_back(WordNo & 0xff);                               //Word No.
                if (WordNo == 0x10)
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(WordNo);
                }

                for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)                              //循环写入多个数值
                {
                    if(i*2 + 1 >= mAddrInfo.nValueList.size())
                    {
                        break;
                    }

                    temp1 = mAddrInfo.nValueList.at(i*2 + 0);                  //从输入数值中取数据，对应的iTemp2为高位，iTemp1为低位
                    temp2 = mAddrInfo.nValueList.at(i*2 + 1);

                    if (temp1 == 0x10)
                    {
                        mSendPackge.sendDataList.push_back(0x10);
                        mSendPackge.sendDataList.push_back(0x10);
                    }
                    else mSendPackge.sendDataList.push_back(temp1);

                    if (temp2 == 0x10)
                    {
                        mSendPackge.sendDataList.push_back(0x10);
                        mSendPackge.sendDataList.push_back(0x10);
                    }
                    else mSendPackge.sendDataList.push_back(temp2);

                    qby_Temp.push_back(temp1);
                    qby_Temp.push_back(temp2);
                }

                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x03);
                qby_Temp.push_back(0x03);

                ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
                temp1 = (usCalCRC >> 8) & 0xff;
                temp2 = (usCalCRC >> 0) & 0xff;
                mSendPackge.sendDataList.push_back(temp2 & 0xff);
                mSendPackge.sendDataList.push_back(temp1 & 0xff);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = -1;

                nCmnTimes++;
                break;
            }
        case REG_TYPE_TACC:
        case REG_TYPE_TPRE:
        case REG_TYPE_CACC:
        case REG_TYPE_CPRE:
            {
                int iReturnPos = 0;
                BlockNo = (mAddrInfo.mAddrProp.nAddrValue)/256;
                WordNo = (mAddrInfo.mAddrProp.nAddrValue)%256;

                vector<uchar> qby_Temp;
                qby_Temp.clear();
                /*vector index = 0*/
                uchar nTmpValue = 0x10;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 1*/
                nTmpValue = 0x02;
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 2*/
                uchar temp1  = mAddrInfo.mAddrProp.nPlcStationIndex & 0xff;
                qby_Temp.push_back(temp1);//qby index = 0
                qby_Temp.push_back(0x00); //qby index = 1
                if(temp1 == 0x10)
                {
                    /*vector index = 2*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);

                    /*vector index = 3*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 2*/
                    nTmpValue = temp1;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                /*vector index = 3 或者 4*/
                nTmpValue = 0x00;
                mSendPackge.sendDataList.push_back(nTmpValue);

                mSendPackge.sendDataList.push_back(0x0f);
                mSendPackge.sendDataList.push_back(0x00);
                qby_Temp.push_back(0x0f); //qby index = 2
                qby_Temp.push_back(0x00); //qby index = 3

                temp1 = (nCmnTimes >> 8) & 0xff;             //通讯变量次数，最大4096
                uchar temp2 = (nCmnTimes >> 0) & 0xff;

                qby_Temp.push_back(temp1); //qby index = 4
                qby_Temp.push_back(temp2); //qby index = 5

                nTmpValue = temp1;
                mSendPackge.sendDataList.push_back(nTmpValue);

                if (temp2 == 0x10)                                         //低位可能出现0x10情况
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    /*vector index = 8 或者 9*/
                    nTmpValue = 0x10;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                    iReturnPos++;
                }
                else
                {
                    /*vector index = 7 或者 8*/
                    nTmpValue = temp2;
                    mSendPackge.sendDataList.push_back(nTmpValue);
                }

                nTmpValue = 0xab;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 6

                nTmpValue = (mAddrInfo.mAddrProp.nAddrLen*2) & 0xff;
                mSendPackge.sendDataList.push_back(nTmpValue);
                qby_Temp.push_back(nTmpValue); //qby index = 7

                qby_Temp.push_back(BlockNo & 0xff); //qby index = 8

                if (BlockNo == 0x10)                                        //Bolck No.
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(BlockNo & 0xff);
                }

                switch (eRegType)
                {
                case REG_TYPE_TACC:
                case REG_TYPE_TPRE:
                    mSendPackge.sendDataList.push_back(0x86);
                    break;
                case REG_TYPE_CACC:
                case REG_TYPE_CPRE:
                    mSendPackge.sendDataList.push_back(0x87);
                    break;
                default:
                    break;
                }

                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

                qby_Temp.push_back(WordNo & 0xff);                               //Word No.
                if (WordNo == 0x10)
                {
                    mSendPackge.sendDataList.push_back(0x10);
                    mSendPackge.sendDataList.push_back(0x10);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(WordNo & 0xff);
                }

                switch (eRegType)
                {
                case REG_TYPE_TACC:
                case REG_TYPE_TPRE:
                    mSendPackge.sendDataList.push_back(0x02);
                    break;
                case REG_TYPE_CACC:
                case REG_TYPE_CPRE:
                    mSendPackge.sendDataList.push_back(0x87);
                    break;
                default:
                    break;
                }

                qby_Temp.push_back(mSendPackge.sendDataList.at(mSendPackge.sendDataList.size() - 1));

                mSendPackge.sendDataList.push_back(0xff);
                mSendPackge.sendDataList.push_back(0xff);
                qby_Temp.push_back(0xff);
                qby_Temp.push_back(0xff);

                for (int i=0; i<mAddrInfo.mAddrProp.nAddrLen; i++)                              //循环写入多个数值
                {
                    if(i*2 + 1 >= mAddrInfo.nValueList.size())
                    {
                        break;
                    }

                    temp1 = mAddrInfo.nValueList.at(i*2 + 0);                  //从输入数值中取数据，对应的iTemp2为高位，iTemp1为低位
                    temp2 = mAddrInfo.nValueList.at(i*2 + 1);

                    if (temp1 == 0x10)
                    {
                        mSendPackge.sendDataList.push_back(0x10);
                        mSendPackge.sendDataList.push_back(0x10);
                    }
                    else mSendPackge.sendDataList.push_back(temp1);

                    if (temp2 == 0x10)
                    {
                        mSendPackge.sendDataList.push_back(0x10);
                        mSendPackge.sendDataList.push_back(0x10);
                    }
                    else mSendPackge.sendDataList.push_back(temp2);

                    qby_Temp.push_back(temp1);
                    qby_Temp.push_back(temp2);
                }

                mSendPackge.sendDataList.push_back(0x10);
                mSendPackge.sendDataList.push_back(0x03);
                qby_Temp.push_back(0x03);

                ushort usCalCRC = PlcMathMethod::getCRCcheck(qby_Temp);          //计算校验
                temp1 = (usCalCRC >> 8) & 0xff;
                temp2 = (usCalCRC >> 0) & 0xff;
                mSendPackge.sendDataList.push_back(temp2 & 0xff);
                mSendPackge.sendDataList.push_back(temp1 & 0xff);

                /*返回数据的长度*/
                mSendPackge.nReturnLen = -1;

                nCmnTimes++;
                break;
            }
    default:
            {
                return false;
                break;
            }
        }
    }
    else if(mSendPackge.nPkgIndex == 2)
    {
        //第二个包
        mSendPackge.sendDataList.clear();
        mSendPackge.sendDataList.push_back(0x10);
        mSendPackge.sendDataList.push_back(0x06);

        mSendPackge.nReturnLen = 0;
    }

    return true;
}

/******************************************************************
 * Function: merge global address to m_globalReadPlcAddrList
 * Parameters: address list
 * Return: null
 ******************************************************************/
bool PlcProtocol::sortOutAddrList(vector<ADDR_TARGET_PROP> mSourceAddrList, vector<ADDR_TARGET_PROP> &targetList, int nMaxRWlen, bool bWriteAddr)
{
    if(mSourceAddrList.empty()) return false;
    targetList.clear();
    //targetList = mSourceAddrList;
    //return true;

    /*按站号分，放在一个map中，站号为key*/
    int nTotalSize = mSourceAddrList.size();

    /*超过最大读写长度 则等于最大读写长度*/
    if(nMaxRWlen > MAX_RW_LEN)
    {
        nMaxRWlen = MAX_RW_LEN;
    }

    ADDR_TARGET_PROP mTmpAddr;

    /*按寄存器号整理*/
    vector<ADDR_TARGET_PROP > mTmpListBit0;
    vector<ADDR_TARGET_PROP > mTmpListBit1;
    vector<ADDR_TARGET_PROP > mTmpListBit2;
    vector<ADDR_TARGET_PROP > mTmpListBit3;
    vector<ADDR_TARGET_PROP > mTmpListBit4;

    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;
    vector<ADDR_TARGET_PROP > mTmpListWord5;
    vector<ADDR_TARGET_PROP > mTmpListWord6;

    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_I:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_O:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_B:
            {
                mTmpListBit2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_SBIT:
            {
                mTmpListBit3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_NBIT:
            {
                mTmpListBit4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_S:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_N:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_F:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TACC:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TPRE:
            {
                mTmpListWord4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CACC:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CPRE:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit0 = PlcMathMethod::addrBubbleSort(mTmpListBit0);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxRWlen, 8);
    }

    int tmpSize = mTmpListBit0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit0.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit1 = PlcMathMethod::addrBubbleSort(mTmpListBit1);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 8);
    }

    tmpSize = mTmpListBit1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit1.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit2 = PlcMathMethod::addrBubbleSort(mTmpListBit2);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, nMaxRWlen, 8);
    }

    tmpSize = mTmpListBit2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit2.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit3 = PlcMathMethod::addrBubbleSort(mTmpListBit3);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, nMaxRWlen, 8);
    }

    tmpSize = mTmpListBit3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit3.at(i));
    }

    /*这个nMaxRWlen是用户设置的最大读写长度，可能还有与协议中的最大支持长度比较*/
    mTmpListBit4 = PlcMathMethod::addrBubbleSort(mTmpListBit4);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, nMaxRWlen, 8);
    }

    tmpSize = mTmpListBit4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit4.at(i));
    }

    mTmpListWord0 = PlcMathMethod::addrBubbleSort(mTmpListWord0);
    mTmpListWord0 = PlcMathMethod::addrSortOut(mTmpListWord0, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord0.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord0.at(i));
    }

    mTmpListWord1 = PlcMathMethod::addrBubbleSort(mTmpListWord1);
    mTmpListWord1 = PlcMathMethod::addrSortOut(mTmpListWord1, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord1.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord1.at(i));
    }

    mTmpListWord2 = PlcMathMethod::addrBubbleSort(mTmpListWord2);
    mTmpListWord2 = PlcMathMethod::addrSortOut(mTmpListWord2, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord2.at(i));
    }

    mTmpListWord3 = PlcMathMethod::addrBubbleSort(mTmpListWord3);
    mTmpListWord3 = PlcMathMethod::addrSortOut(mTmpListWord3, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord3.at(i));
    }

    mTmpListWord4 = PlcMathMethod::addrBubbleSort(mTmpListWord4);
    mTmpListWord3 = PlcMathMethod::addrSortOut(mTmpListWord4, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord4.at(i));
    }

    mTmpListWord5 = PlcMathMethod::addrBubbleSort(mTmpListWord5);
    mTmpListWord5 = PlcMathMethod::addrSortOut(mTmpListWord5, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord5.at(i));
    }

    mTmpListWord6 = PlcMathMethod::addrBubbleSort(mTmpListWord6);
    mTmpListWord6 = PlcMathMethod::addrSortOut(mTmpListWord6, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord6.at(i));
    }

    return true;
}

/******************************************************************
 * Function: 获得默认最大的读写长度
 * Parameters:
 * Return: 最大长度
 ******************************************************************/
int PlcProtocol::getMaxRwLen()
{
    int nMaxRWlen = 64;
    return nMaxRWlen;
}

/******************************************************************
 * Function: 校验字符串的地址
 * Parameters: null
 * Return: null
 ******************************************************************/
ADDR_CHECK_ERROR_INFO PlcProtocol::checkStrAddr(string sAddrStr, ADDR_REG_TYPE eAddrRegType, ADDR_CHECK_SOURCE_PROP &sAddrProp)
{
    /*取寄存器的名称列表*/
    int nStartPos = -1;
    vector<string > regNameList = getRegNameList();
    int nSize = regNameList.size();

    /*一个个判断*/
    ADDR_CHECK_ERROR_INFO eErrorInfo = ADDR_CORRECT;
    bool bSuccess = false;
    string sRegName ;
    vector<string > compareNameList;
    vector<int > maxIndexList ;
    vector<int > startPosList;
    compareNameList.clear();
    maxIndexList.clear();
    startPosList.clear();
    for(int i = 0; i < nSize; i++)
    {
        sRegName = regNameList.at(i);

        /*包含这个寄存器名字*/
        bSuccess = DealWithString::getIsContainSubstr(sAddrStr, sRegName, nStartPos);
        if(bSuccess)
        {
            compareNameList.push_back(sRegName);
            maxIndexList.push_back(i);
            startPosList.push_back(nStartPos);
        }
    }

    int nCompareSize = compareNameList.size();
    if(nCompareSize < 1)
    {
        return ILLEGAL_REG;
    }

    /*取第一个与剩下的比较*/
    int nIndex = maxIndexList.at(0);
    sRegName = compareNameList.at(0);
    nStartPos = startPosList.at(0);
    for(int i = 1; i < nCompareSize; i++)
    {
        if(sRegName.length() < compareNameList.at(i).length())
        {
            nIndex = maxIndexList.at(i);
            sRegName = compareNameList.at(i);
            nStartPos = startPosList.at(i);
        }
        else if(nStartPos > startPosList.at(i))
        {
            nIndex = maxIndexList.at(i);
            sRegName = compareNameList.at(i);
            nStartPos = startPosList.at(i);
        }
    }

    /*取寄存器名字和地址值的长度*/
    int nNameLen = sRegName.length();
    int nValueLen = sAddrStr.length() - (nStartPos + nNameLen);
    if(nValueLen > 0)
    {
        /*取地址的值*/
        sAddrProp.sRegName = sRegName;
        sAddrProp.nAddrLen = 1;
        sAddrProp.sAddrValue = sAddrStr.substr(nStartPos + nNameLen, nValueLen);
        eErrorInfo = checkAddrCorrect(sAddrProp, eAddrRegType);

        return eErrorInfo;
    }
    else
    {
        return ILLEGAL_VALUE;
    }
}

/******************************************************************
 * Function: 获得当前PLC的类型
 * Parameters: null
 * Return: null
 ******************************************************************/
PLC_FUN_TYPE PlcProtocol::getPlcType()
{
    return PLC_MASTER;
}

/******************************************************************
 * Function: 获得默认站号
 * Parameters: null
 * Return: 默认站号
 ******************************************************************/
int PlcProtocol::getDefaultStation()
{
    int nStation = 1;
    return nStation;
}

/******************************************************************
 * Function: 获得默认本协议的默认串口信息
 * Parameters: null
 * Return: SERIAL_PORT_PROP结构体，串口信息
 ******************************************************************/
SERIAL_PORT_PROP PlcProtocol::getDefaultSerialInfo()
{
    SERIAL_PORT_PROP mSerialInfo;
    mSerialInfo.BAUD_RATE = 19200;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 1;
    mSerialInfo.STOP_BIT = 1;

    return mSerialInfo;
}

/******************************************************************
 * Function: 获得默认超时时间
 * Parameters: null
 * Return: 默认超时时间,单位：ms
 ******************************************************************/
int PlcProtocol::getDefaultTimeout()
{
    int nTimeout = 1000;
    return nTimeout;
}

/******************************************************************
 * Function: 获得默认最小采集周期
 * Parameters: null
 * Return: 默认最小采集周期,单位：毫秒
 ******************************************************************/
int PlcProtocol::getDefaultCollectRate()
{
    int nCollectRate = 1;
    return nCollectRate;
}

/******************************************************************
 * Function: 获得帧间隔时间
 * Parameters: null
 * Return: 获得帧间隔时间,单位：毫秒
 ******************************************************************/
int PlcProtocol::getDefaultIntervalTime()
{
    int nIntervalTime = 10;
    return nIntervalTime;
}

/******************************************************************
 * Function: 获得地址是否可读
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanRead(ADDR_TARGET_PROP mAddr)
{
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_4096 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8184)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_O:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4088)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_B:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_12240 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535 * 16 + 8)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_SBIT:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_512 * 16 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_12280)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_NBIT:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_28672 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535 * 16 + 8)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_512 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_769)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_N:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 7 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 8 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 4 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 5 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    return true;
}

/******************************************************************
 * Function: 获得地址是否可写
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_I:
        {
            return false;
            break;
        }
    case REG_TYPE_O:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_4088)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_B:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_12240 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535 * 16 + 8)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_SBIT:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_512 * 16 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_12280)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_NBIT:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_28672 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535 * 16 + 8)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_S:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_512 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_769)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_N:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 7 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_F:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 8 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TACC:
    case REG_TYPE_TPRE:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 4 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CACC:
    case REG_TYPE_CPRE:
        {
            if(mAddr.nAddrValue < ADDR_MAX_VALUE_256 * 5 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_65535  + 2)
            {
                return false;
            }
            break;
        }
    default:
        {
            return false;
            break;
        }
    }

    return true;
}


/*************************************************************************
 * Funciton: int 类型转换成寄存器的枚举类型
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
REG_TYPE_INDEX PlcProtocol::getRegTypeFromInt(int nRegIndex)
{
    REG_TYPE_INDEX eRegType = REG_TYPE_NULL;
    switch(nRegIndex)
    {
    case 0:
        {
            eRegType = REG_TYPE_I;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_O;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_B;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_SBIT;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_NBIT;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_S;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_N;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_F;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_TACC;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_TPRE;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_CACC;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_CPRE;
            break;
        }
    default:
        {
            break;
        }
    }

    return eRegType;
}

/*************************************************************************
 * Funciton: 通过名字获得寄存器的下标号
 * Parameters:
 * Return: 转换的结果
*************************************************************************/
bool PlcProtocol::getRegIndexFromName(string sRegName, int &nRegIndex)
{
    if(sRegName.empty()) return false;

    if(DealWithString::compare("I", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("O", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("B", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("SBIT", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("NBIT", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("S", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("N", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("F", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("TACC", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("TPRE", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("CACC", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("CPRE", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else
    {
        return false;
    }
}

/*************************************************************************
 * Funciton: 获取协议的连接类型
 * Parameters:
 * Return: 协议类型
*************************************************************************/
PROTOCOL_TYPE PlcProtocol::getProType()
{
    return SERIAL_PORT;
}


