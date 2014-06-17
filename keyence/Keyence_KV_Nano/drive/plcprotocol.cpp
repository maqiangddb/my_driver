#include "plcprotocol.h"
#include "../tools/dealwithstring.h"
#include "../tools/plcmathmethod.h"

/*写数据返回的长度*/
#define WRITE_RETURN_LEN      8

/*读数据返回的数据头部长度*/
#define READ_HEAD_LEN         3

/*读数据返回的数据校验的长度*/
#define CHECK_LEN             0

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
    case REG_TYPE_R:                      //REG_TYPE_RLY:
    case REG_TYPE_MR:
        {
            if (mAddrSource.sAddrValue.size() <= 2)
            {
                int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
                if(bSuccess == false || nIntValue > 15 || nIntValue < 0)
                {
                    return false;
                }
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_9600)//两位数字肯定不会有这么大
                {
                    return false;
                }
                return true;
            }
            else
            {
                string IntPass = DealWithString::mid(mAddrSource.sAddrValue,0,mAddrSource.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mAddrSource.sAddrValue,mAddrSource.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                int IntTemp = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }
                int DecTemp = DealWithString::stringToInt(DecPass,bSuccess);
                if(DecTemp >= 16)
                {
                    return false;
                }
                int nIntValue = IntTemp*16 + DecTemp;
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_9600)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_B:
    case REG_TYPE_VB:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_8192)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_LR:
        {
            if (mAddrSource.sAddrValue.size() <= 2)
            {
                int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
                if(bSuccess == false || nIntValue > 15 || nIntValue < 0)
                {
                    return false;
                }
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_3200)//两位数字肯定不会有这么大
                {
                    return false;
                }
                return true;
            }
            else
            {
                string IntPass = DealWithString::mid(mAddrSource.sAddrValue,0,mAddrSource.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mAddrSource.sAddrValue,mAddrSource.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                int IntTemp = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }
                int DecTemp = DealWithString::stringToInt(DecPass,bSuccess);
                if(DecTemp >= 16)
                {
                    return false;
                }
                int nIntValue = IntTemp*16 + DecTemp;
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_3200)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_CR:
        {
            if (mAddrSource.sAddrValue.size() <= 2)
            {
                int nIntValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);
                if(bSuccess == false || nIntValue > 15 || nIntValue < 0)
                {
                    return false;
                }
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1440)//两位数字肯定不会有这么大
                {
                    return false;
                }
                return true;
            }
            else
            {
                string IntPass = DealWithString::mid(mAddrSource.sAddrValue,0,mAddrSource.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mAddrSource.sAddrValue,mAddrSource.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                int IntTemp = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return false;
                }
                int DecTemp = DealWithString::stringToInt(DecPass,bSuccess);
                if(DecTemp >= 16)
                {
                    return false;
                }
                int nIntValue = IntTemp*16 + DecTemp;
                mAddrTarget.nAddrValue = nIntValue;
                if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_1440)
                {
                    return false;
                }
                return true;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_32768)
            {
                return false;
            }
            return true;
        }
        break;
    case REG_TYPE_W:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess,16);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_16384)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_TM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
        }
        break;
    case REG_TYPE_Z:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_13)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue < 1)
            {
                return false;
            }
            return true;
        }
        break;
    case REG_TYPE_T:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_C:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_CTH:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 4)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue > 2)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_CTC:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > 7)
            {
                return false;
            }
            if(mAddrTarget.nAddrValue > 5)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_CM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_9000)
            {
                return false;
            }
            return true;
        }
    case REG_TYPE_VM:
        {
            mAddrTarget.nAddrValue = DealWithString::stringToInt(mAddrSource.sAddrValue,bSuccess);

            /*判断长度是否超限*/
            if(mAddrTarget.nAddrValue + mAddrTarget.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
            return true;
        }
    default:
        {
            break;
        }
    }

    return false;
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
            sRegNameList.push_back("R");
            sRegNameList.push_back("B");
            sRegNameList.push_back("MR");
            sRegNameList.push_back("LR");
            sRegNameList.push_back("CR");
            sRegNameList.push_back("VB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");
            break;
        }
    case WORT_ADDR_REG:
        {
            sRegNameList.push_back("DM");
            sRegNameList.push_back("W");
            sRegNameList.push_back("TM");
            sRegNameList.push_back("Z");
            sRegNameList.push_back("TC");
            sRegNameList.push_back("TS");
            sRegNameList.push_back("CC");
            sRegNameList.push_back("CS");
            sRegNameList.push_back("CTH");
            sRegNameList.push_back("CTC");
            sRegNameList.push_back("CM");
            sRegNameList.push_back("VM");
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
            sRegNameList.push_back("R");
            sRegNameList.push_back("B");
            sRegNameList.push_back("MR");
            sRegNameList.push_back("LR");
            sRegNameList.push_back("CR");
            sRegNameList.push_back("VB");
            sRegNameList.push_back("T");
            sRegNameList.push_back("C");

            sRegNameList.push_back("DM");
            sRegNameList.push_back("W");
            sRegNameList.push_back("TM");
            sRegNameList.push_back("Z");
            sRegNameList.push_back("TC");
            sRegNameList.push_back("TS");
            sRegNameList.push_back("CC");
            sRegNameList.push_back("CS");
            sRegNameList.push_back("CTH");
            sRegNameList.push_back("CTC");
            sRegNameList.push_back("CM");
            sRegNameList.push_back("VM");
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
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
    case REG_TYPE_T:
    case REG_TYPE_C:
        {
            if(eAddrRegType != BIT_ADDR_REG)
            {
                return IS_BIT_ADDR;
            }
            break;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
    case REG_TYPE_CTH:
    case REG_TYPE_CTC:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
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
    case REG_TYPE_R:                   //REG_TYPE_RLY:
    case REG_TYPE_MR:
        {
            int nIntValue = 0;
            int nDecValue = 0;
            if (mStartAddr.sAddrValue.size() <= 2)
            {
                nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nIntValue = 0;
            }
            else
            {
                string IntPass = DealWithString::mid(mStartAddr.sAddrValue,0,mStartAddr.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mStartAddr.sAddrValue,mStartAddr.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                nIntValue = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nDecValue = DealWithString::stringToInt(DecPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
            }
            /*小数部分不能超过16，也不能为负数*/
            if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                if(nDecValue + nInterval > 15)
                {
                    nIntValue += (nDecValue + nInterval)/16;
                    nDecValue = (nDecValue + nInterval)%16;
                }
                else
                {
                    nDecValue += nInterval;
                }

                /*整合后的结果*/
                int nStartValue = nIntValue*16 + nDecValue;
                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_9600)
                {
                    /*添加整数部分*/
                    if(nIntValue > 0)
                    {
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }
                    else
                    {
                        mTmpAddr.sAddrValue = "";
                    }

                    /*添加小数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nDecValue);
                    if(nIntValue > 0 && nDecValue < 10)
                    {
                        sTmpStr = "0";
                        sTmpStr += buf;
                    }
                    else
                    {
                        sTmpStr = buf;
                    }
                    mTmpAddr.sAddrValue += sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }

            }
            return ADDR_CORRECT;
        }
    case REG_TYPE_B:
    case REG_TYPE_VB:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess,16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_8192)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_8192)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%x", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_LR:
        {
            int nIntValue = 0;
            int nDecValue = 0;
            if (mStartAddr.sAddrValue.size() <= 2)
            {
                nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nIntValue = 0;
            }
            else
            {
                string IntPass = DealWithString::mid(mStartAddr.sAddrValue,0,mStartAddr.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mStartAddr.sAddrValue,mStartAddr.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                nIntValue = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nDecValue = DealWithString::stringToInt(DecPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
            }
            /*小数部分不能超过16，也不能为负数*/
            if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                if(nDecValue + nInterval > 15)
                {
                    nIntValue += (nDecValue + nInterval)/16;
                    nDecValue = (nDecValue + nInterval)%16;
                }
                else
                {
                    nDecValue += nInterval;
                }

                /*整合后的结果*/
                int nStartValue = nIntValue*16 + nDecValue;
                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_3200)
                {
                    /*添加整数部分*/
                    if(nIntValue > 0)
                    {
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }
                    else
                    {
                        mTmpAddr.sAddrValue = "";
                    }

                    /*添加小数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nDecValue);
                    if(nIntValue > 0 && nDecValue < 10)
                    {
                        sTmpStr = "0";
                        sTmpStr += buf;
                    }
                    else
                    {
                        sTmpStr = buf;
                    }
                    mTmpAddr.sAddrValue += sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }

            }
            return ADDR_CORRECT;
        }
    case REG_TYPE_CR:
        {
            int nIntValue = 0;
            int nDecValue = 0;
            if (mStartAddr.sAddrValue.size() <= 2)
            {
                nDecValue = DealWithString::stringToInt(mStartAddr.sAddrValue,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nIntValue = 0;
            }
            else
            {
                string IntPass = DealWithString::mid(mStartAddr.sAddrValue,0,mStartAddr.sAddrValue.size() - 2);//sSource_Add.left(sSource_Add.size() - 2);
                string DecPass = DealWithString::mid(mStartAddr.sAddrValue,mStartAddr.sAddrValue.size() - 2,2);//sSource_Add.mid(IntPass.size(),2);
                nIntValue = DealWithString::stringToInt(IntPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
                nDecValue = DealWithString::stringToInt(DecPass,bSuccess);
                if(bSuccess == false)
                {
                    return ILLEGAL_VALUE;
                }
            }
            /*小数部分不能超过16，也不能为负数*/
            if(nIntValue < 0 || nDecValue < 0 || nDecValue > 15)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                if(nDecValue + nInterval > 15)
                {
                    nIntValue += (nDecValue + nInterval)/16;
                    nDecValue = (nDecValue + nInterval)%16;
                }
                else
                {
                    nDecValue += nInterval;
                }

                /*整合后的结果*/
                int nStartValue = nIntValue*16 + nDecValue;
                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_1440)
                {
                    /*添加整数部分*/
                    if(nIntValue > 0)
                    {
                        memset(buf, 0, 10);
                        sprintf(buf, "%d", nIntValue);
                        sTmpStr = buf;
                        mTmpAddr.sAddrValue = sTmpStr;
                    }
                    else
                    {
                        mTmpAddr.sAddrValue = "";
                    }

                    /*添加小数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nDecValue);
                    if(nIntValue > 0 && nDecValue < 10)
                    {
                        sTmpStr = "0";
                        sTmpStr += buf;
                    }
                    else
                    {
                        sTmpStr = buf;
                    }
                    mTmpAddr.sAddrValue += sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }

            }
            return ADDR_CORRECT;
        }
    case REG_TYPE_DM:                   //REG_TYPE_DM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_32768)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_32768)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_W:                   //REG_TYPE_DM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess,16);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_16384)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_16384)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_TM:                   //REG_TYPE_DM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_512)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_512)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_Z:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 1 || nStartValue >= ADDR_MAX_VALUE_13)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_13)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_512)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_512)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_C:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_256)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_256)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= 3)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 4)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_CTC:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= 5)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= 7)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_CM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_9000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_9000)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
            }
            break;
        }
    case REG_TYPE_VM:
        {
            int nStartValue = DealWithString::stringToInt(mStartAddr.sAddrValue, bSuccess);
            if(false == bSuccess)
            {
                return ILLEGAL_VALUE;
            }

            if(nStartValue < 0 || nStartValue >= ADDR_MAX_VALUE_10000)
            {
                return ILLEGAL_VALUE;
            }

            char buf[10] = {0};
            string sTmpStr;
            for(int i = 0; i < nCopyNum; i++)
            {
                nStartValue += nInterval;

                if(nStartValue + mTmpAddr.nAddrLen <= ADDR_MAX_VALUE_10000)
                {
                    /*添加整数部分*/
                    memset(buf, 0, 10);
                    sprintf(buf, "%d", nStartValue);
                    sTmpStr = buf;
                    mTmpAddr.sAddrValue = sTmpStr;

                    mSourceAddrList.push_back(mTmpAddr);
                }
                else
                {
                    break;
                }
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
    ADDR_TARGET_PROP mAddrInfo = mUnpackProp.mCurrAddr.mAddrProp;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);

    if(CMN_NOMAL_CODE != eCmnInfo) return eCmnInfo;

    targetList.clear();
    if(mUnpackProp.bNeedSetData)
    {
        int nSize = rcvList.size();
        int nStart = 0; //有效数据开始地址
        int nValidLen = 0; //有效数据长度
        switch(eRegType)
        {
        case REG_TYPE_R:
        case REG_TYPE_B:
        case REG_TYPE_MR:
        case REG_TYPE_LR:
        case REG_TYPE_CR:
        case REG_TYPE_VB:
            nValidLen = nSize - 2;
            for(int i = nStart; i <nValidLen; i++)
            {
                //LOGE("unpackReadData i = %d, value = %x",i,rcvList.at(i));
                if(rcvList.at(i) == 0x20)
                {
                    continue;
                }
                targetList.push_back(rcvList.at(i));
            }
            break;
        case REG_TYPE_DM:
        case REG_TYPE_W:
        case REG_TYPE_TM:
        case REG_TYPE_Z:
        case REG_TYPE_TC:
        case REG_TYPE_TS:
        case REG_TYPE_CC:
        case REG_TYPE_CS:
        case REG_TYPE_CM:
        case REG_TYPE_VM:
            {
            nValidLen = nSize - 2;
            for(int i = nStart; i <nValidLen; i++)
            {
                //LOGE("unpackReadData i = %d, value = %x",i,rcvList.at(i));
                if(rcvList.at(i) == 0x20)
                {
                    continue;
                }
                targetList.push_back(rcvList.at(i));
            }
            break;
        }
        case REG_TYPE_C:
        case REG_TYPE_T:
        case REG_TYPE_CTH:
        case REG_TYPE_CTC:
            {
            nValidLen = nSize - 2;
            for(int i = nStart; i <nValidLen; i++)
            {
                //LOGE("unpackReadData i = %d, value = %x",i,rcvList.at(i));
                if(rcvList.at(i) == 0x20)
                {
                    continue;
                }
                targetList.push_back(rcvList.at(i));
            }
            break;
        }
        default:
            break;
        }

        /*取有效数据*/

        eCmnInfo = CMN_NOMAL_CODE;
        return eCmnInfo;
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

    if(CHECK_READ == eCheckType)//读校验
    {
        int nRcvSize = rcvList.size();
        if(nRcvSize < 2)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }
        if (rcvList.at(nRcvSize - 1) == 0x0a  &&  rcvList.at(nRcvSize - 2) == 0x0d)
        {
            mUnpackProp.bNeedSetData = true;
            eCmnInfo = CMN_NOMAL_CODE;
            return eCmnInfo;
        }
    }
    else if(CHECK_WRITE == eCheckType)//写校验
    {
        int nRcvSize = rcvList.size();
        if(nRcvSize < 4)
        {
            eCmnInfo = DATA_LEN_ERROR;
            return eCmnInfo;
        }

        if (rcvList.at(0) == 0x4F &&
            rcvList.at(1) == 0x4B &&
            rcvList.at(2) == 0x0D &&
            rcvList.at(3) == 0x0A)//返回O K CR LF是正确的
        {
            mUnpackProp.bNeedSetData = true;
            eCmnInfo = CMN_NOMAL_CODE;
        }
        else
        {
            eCmnInfo = DATA_CHECK_ERROR;
        }
        //LOGE("checkRcvData %d",eCmnInfo);
        return eCmnInfo;
    }
}

/******************************************************************
 * Function: 获得握手协议的包
 * Parameters: mHandShakeList
 * Return: null
 ******************************************************************/
bool PlcProtocol::getHandshakePkg(SEND_PACKAGE &mHandShake, unsigned int &nCmnTimes, int nPkgIndex, int nStationId, int nBaudRate)
{
    switch(nPkgIndex)
    {
    case 1:
        mHandShake.sendDataList.clear();
        mHandShake.sendDataList.push_back(0x43);
        mHandShake.sendDataList.push_back(0x52);
        mHandShake.sendDataList.push_back(0x0d);
        mHandShake.sendDataList.push_back(0x0a);

        mHandShake.nReturnLen = 4;
        nCmnTimes = 1;
        return true;
    default:
        return false;
    }
}

/******************************************************************
 * Function: 对握手协议的包，进行解包
 * Parameters: rcvList nCurrTimes: 次数从1开始
 * Return: true or false
 ******************************************************************/
CMN_RW_ERROR_CODE PlcProtocol::unpackHandshakePkg(vector<unsigned char> rcvList, int nCurrTimes)
{
    if (rcvList.size() == 4)//返回长度为11个字节
    {
        if (rcvList.at(0) == 0x43 &&
            rcvList.at(1) == 0x43 &&
            rcvList.at(2) == 0x0d &&
            rcvList.at(3) == 0x0a)//开始结束符正确
        {
            return CMN_NOMAL_CODE;
        }
    }

    return CMN_CONNECT_FAIL;
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

    ushort nAddrValue = 0;
    int nReturnLen = 1;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
        {

            /*算起始地址，一次最小取一个byte*/
            nAddrValue = mAddrInfo.nAddrValue/16;

            nReturnLen = mAddrInfo.nAddrLen;
            /*算取得地址长度*/
            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

            /*站号 vector index = 0*/
            uchar nTmpValue = 0x52;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*对功能码赋值 vector index = 1*/
            nTmpValue = 0x44;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 0x53;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            switch(eRegType)
            {
            case REG_TYPE_R:
                iTemp1 = (mAddrInfo.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.nAddrValue%16)%10;

                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_B:

                iTemp2 = (mAddrInfo.nAddrValue >> 12) & 0xf ;
                iTemp3 = (mAddrInfo.nAddrValue >> 8) & 0xf;
                iTemp4 = (mAddrInfo.nAddrValue >> 4) & 0xf;

                iTemp5 = (mAddrInfo.nAddrValue >> 0) & 0xf;

                iTemp1 = 0;

                nTmpValue = 'B';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_MR:
                iTemp1 = (mAddrInfo.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.nAddrValue%16)%10;
                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_LR:
                iTemp1 = (mAddrInfo.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.nAddrValue%16)%10;
                nTmpValue = 'L';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_CR:
                iTemp1 = (mAddrInfo.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.nAddrValue%16)%10;
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_VB:
                iTemp2 = (mAddrInfo.nAddrValue >> 12) & 0xf ;
                iTemp3 = (mAddrInfo.nAddrValue >> 8) & 0xf;
                iTemp4 = (mAddrInfo.nAddrValue >> 4) & 0xf;

                iTemp5 = (mAddrInfo.nAddrValue >> 0) & 0xf;
                iTemp1 = 0;
                nTmpValue = 'V';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'B';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            default:
                break;
            }

            if(REG_TYPE_VB != eRegType && REG_TYPE_B != eRegType)
            {
                /*vector index = 3*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*读取长度*/
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            iTemp1 = nReturnLen/100;
            iTemp2 = (nReturnLen%100)/10;
            iTemp3 = nReturnLen%10;

            /*vector index = 3*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*读取长度*/
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*站号 vector index = 8*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = nReturnLen * 2 + 1;//有效长度+固定字节长度
            return true;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
    case REG_TYPE_TC:
    case REG_TYPE_CC:
    case REG_TYPE_TS:
    case REG_TYPE_CS:
        {
            /*算起始地址，一次最小取一个byte*/
        nAddrValue = mAddrInfo.nAddrValue;
        nReturnLen = mAddrInfo.nAddrLen;

        /*算取得地址长度*/
        char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

        iTemp1 = mAddrInfo.nAddrValue/10000;
        iTemp2 = (mAddrInfo.nAddrValue%10000)/1000;
        iTemp3 = (mAddrInfo.nAddrValue%1000)/100;
        iTemp4 = (mAddrInfo.nAddrValue%100)/10;
        iTemp5 = mAddrInfo.nAddrValue%10;

        /*站号 vector index = 0*/
        uchar nTmpValue = 0x52;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*对功能码赋值 vector index = 1*/
        nTmpValue = 0x44;
        mSendPackge.sendDataList.push_back(nTmpValue);

        nTmpValue = 0x53;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*开始地址 vector index = 2*/
        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        if(REG_TYPE_DM == eRegType)
        {
            nTmpValue = 'D';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'M';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_W == eRegType)
        {
            iTemp1 = (mAddrInfo.nAddrValue >> 12) & 0xf ;
            iTemp2 = (mAddrInfo.nAddrValue >> 8) & 0xf;
            iTemp3 = (mAddrInfo.nAddrValue >> 4) & 0xf;

            iTemp4 = (mAddrInfo.nAddrValue >> 0) & 0xf;
            //iTemp1 = 0;

            nTmpValue = 'W';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_Z == eRegType)
        {
            nTmpValue = 'Z';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_TM == eRegType)
        {
            nTmpValue = 'T';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'M';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_CM == eRegType)
        {
            nTmpValue = 'C';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'M';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_VM == eRegType)
        {
            nTmpValue = 'V';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'M';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_TS == eRegType)
        {
            nTmpValue = 'T';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'S';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_CS == eRegType)
        {
            nTmpValue = 'C';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'S';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }       
        else if(REG_TYPE_TC == eRegType)
        {
            nTmpValue = 'T';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'C';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else if(REG_TYPE_CC == eRegType)
        {
            nTmpValue = 'C';
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'C';
            mSendPackge.sendDataList.push_back(nTmpValue);
        }

        if(REG_TYPE_W == eRegType)
        {
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*读取长度*/
            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }
        else
        {
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*读取长度*/
            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);
        }


        mSendPackge.sendDataList.push_back(0x2e);

        mSendPackge.sendDataList.push_back('H');

        nTmpValue = 0x20;
        mSendPackge.sendDataList.push_back(nTmpValue);

        iTemp1 = mAddrInfo.nAddrLen/100;
        iTemp2 = (mAddrInfo.nAddrLen%100)/10;
        iTemp3 = mAddrInfo.nAddrLen%10;

        /*vector index = 4*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*读取长度*/
        /*vector index = 5*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*vector index = 6*/
        nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*站号 vector index = 7*/
        nTmpValue = 0x0d;
        mSendPackge.sendDataList.push_back(nTmpValue);

        /*返回数据的长度*/
        mSendPackge.nReturnLen = nReturnLen * 5 + 1;

        return true;
        break;
        }
    case REG_TYPE_C:
    case REG_TYPE_T:
    case REG_TYPE_CTC:
    case REG_TYPE_CTH:
        {
        /*算起始地址，一次最小取一个byte*/
    nAddrValue = mAddrInfo.nAddrValue;
    nReturnLen = mAddrInfo.nAddrLen;

    /*算取得地址长度*/
    char iTemp1,iTemp2,iTemp3,iTemp4;

    iTemp1 = mAddrInfo.nAddrValue/1000;
    iTemp2 = (mAddrInfo.nAddrValue%1000)/100;
    iTemp3 = (mAddrInfo.nAddrValue%100)/10;
    iTemp4 = mAddrInfo.nAddrValue%10;

    /*站号 vector index = 0*/
    uchar nTmpValue = 0x52;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*对功能码赋值 vector index = 1*/
    nTmpValue = 0x44;
    mSendPackge.sendDataList.push_back(nTmpValue);

    nTmpValue = 0x53;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*开始地址 vector index = 2*/
    nTmpValue = 0x20;
    mSendPackge.sendDataList.push_back(nTmpValue);

    if(REG_TYPE_C == eRegType)
    {
        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);
    }
    else if(REG_TYPE_T == eRegType)
    {
        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);
    }
    else if(REG_TYPE_CTC == eRegType)
    {
        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);

        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);
    }
    else if(REG_TYPE_CTH == eRegType)
    {
        nTmpValue = 'C';
        mSendPackge.sendDataList.push_back(nTmpValue);

        nTmpValue = 'T';
        mSendPackge.sendDataList.push_back(nTmpValue);

        nTmpValue = 'H';
        mSendPackge.sendDataList.push_back(nTmpValue);
    }

    /*vector index = 4*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*读取长度*/
    /*vector index = 5*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 6*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 6*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
    mSendPackge.sendDataList.push_back(nTmpValue);

    mSendPackge.sendDataList.push_back(0x2e);

    mSendPackge.sendDataList.push_back('H');

    nTmpValue = 0x20;
    mSendPackge.sendDataList.push_back(nTmpValue);

    iTemp1 = mAddrInfo.nAddrLen/100;
    iTemp2 = (mAddrInfo.nAddrLen%100)/10;
    iTemp3 = mAddrInfo.nAddrLen%10;

    /*vector index = 4*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*读取长度*/
    /*vector index = 5*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*vector index = 6*/
    nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*站号 vector index = 7*/
    nTmpValue = 0x0d;
    mSendPackge.sendDataList.push_back(nTmpValue);

    /*返回数据的长度*/
    mSendPackge.nReturnLen = nReturnLen * 12 + 1;

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

    mSendPackge.nTotalPkgNum = 1;
    mSendPackge.sendDataList.clear();
    mSendPackge.eSendType = mCmnProp.eSendType;

    if(mAddrInfo.nValueList.empty()) return false;

    uchar nTmpValue = 0;
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddrInfo.mAddrProp.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:
    case REG_TYPE_B:
    case REG_TYPE_MR:
    case REG_TYPE_LR:
    case REG_TYPE_CR:
    case REG_TYPE_VB:
        {
            /*vector index = 2*/
            nTmpValue = 0x57;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            nTmpValue = 0x52;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;
            switch(eRegType)
            {
            case REG_TYPE_R:
                iTemp1 = (mAddrInfo.mAddrProp.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue%16)%10;
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_B:
                iTemp2 = (mAddrInfo.mAddrProp.nAddrValue >> 12) & 0xf ;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xf;
                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue >> 4) & 0xf;

                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xf;
                iTemp1 = 0;
                nTmpValue = 'B';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_MR:
                iTemp1 = (mAddrInfo.mAddrProp.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue%16)%10;
                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_LR:
                iTemp1 = (mAddrInfo.mAddrProp.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue%16)%10;
                nTmpValue = 'L';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_CR:
                iTemp1 = (mAddrInfo.mAddrProp.nAddrValue/16)/100;
                iTemp2 = ((mAddrInfo.mAddrProp.nAddrValue/16)%100)/10;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue/16)%10;

                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%16)/10;
                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue%16)%10;
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'R';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            case REG_TYPE_VB:
                iTemp2 = (mAddrInfo.mAddrProp.nAddrValue >> 12) & 0xf ;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xf;
                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue >> 4) & 0xf;

                iTemp5 = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xf;
                iTemp1 = 0;
                nTmpValue = 'V';
                mSendPackge.sendDataList.push_back(nTmpValue);
                nTmpValue = 'B';
                mSendPackge.sendDataList.push_back(nTmpValue);
                break;
            default:
                break;
            }

            if(REG_TYPE_VB != eRegType && REG_TYPE_B != eRegType)
            {
                /*vector index = 3*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            /*读取长度*/
            /*vector index = 4*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 5*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            if(!mAddrInfo.nValueList.empty())
            {
                if (mAddrInfo.nValueList.at(0) == 0x01)
                {
                    mSendPackge.sendDataList.push_back(0x31);
                }
                else
                {
                    mSendPackge.sendDataList.push_back(0x30);
                }
            }

            /*站号 vector index = 8*/
            nTmpValue = 0x0D;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 4;//有效长度+固定字节长度

            return true;
        }
    case REG_TYPE_DM:
    case REG_TYPE_W:
    case REG_TYPE_TM:
    case REG_TYPE_Z:
    case REG_TYPE_CM:
    case REG_TYPE_VM:
    case REG_TYPE_CTC:
    case REG_TYPE_CTH:
    case REG_TYPE_TC:
    case REG_TYPE_CC:
    case REG_TYPE_TS:
    case REG_TYPE_CS:
        {
            char iTemp1,iTemp2,iTemp3,iTemp4,iTemp5;

            iTemp1 = mAddrInfo.mAddrProp.nAddrValue/10000;
            iTemp2 = (mAddrInfo.mAddrProp.nAddrValue%10000)/1000;
            iTemp3 = (mAddrInfo.mAddrProp.nAddrValue%1000)/100;
            iTemp4 = (mAddrInfo.mAddrProp.nAddrValue%100)/10;
            iTemp5 = mAddrInfo.mAddrProp.nAddrValue%10;

            /*vector index = 0*/
            nTmpValue = 'W';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 'R';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 1*/
            nTmpValue = 'S';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*开始地址 vector index = 2*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            if(REG_TYPE_DM == eRegType)
            {
                nTmpValue = 'D';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_W == eRegType)
            {
                iTemp1 = (mAddrInfo.mAddrProp.nAddrValue >> 12) & 0xf ;
                iTemp2 = (mAddrInfo.mAddrProp.nAddrValue >> 8) & 0xf;
                iTemp3 = (mAddrInfo.mAddrProp.nAddrValue >> 4) & 0xf;

                iTemp4 = (mAddrInfo.mAddrProp.nAddrValue >> 0) & 0xf;
                nTmpValue = 'W';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_Z == eRegType)
            {
                nTmpValue = 'Z';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_TM == eRegType)
            {
                nTmpValue = 'T';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_CM == eRegType)
            {
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_VM == eRegType)
            {
                nTmpValue = 'V';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'M';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_TS == eRegType)
            {
                nTmpValue = 'T';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'S';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_CS == eRegType)
            {
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'S';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_CTC == eRegType)
            {
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'T';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_CTH == eRegType)
            {
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'T';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'H';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_TC == eRegType)
            {
                nTmpValue = 'T';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else if(REG_TYPE_CC == eRegType)
            {
                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);

                nTmpValue = 'C';
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            if(REG_TYPE_W == eRegType)
            {
                /*vector index = 5*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 6*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 7*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 8*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }
            else
            {
                /*vector index = 5*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 6*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 7*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 8*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp4);
                mSendPackge.sendDataList.push_back(nTmpValue);

                /*vector index = 8*/
                nTmpValue = PlcMathMethod::intToAsciiValue(iTemp5);
                mSendPackge.sendDataList.push_back(nTmpValue);
            }

            nTmpValue = 0x2E;
            mSendPackge.sendDataList.push_back(nTmpValue);

            nTmpValue = 'H';
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 9*/
            nTmpValue = 0x20;
            mSendPackge.sendDataList.push_back(nTmpValue);

            iTemp1 = mAddrInfo.mAddrProp.nAddrLen/100;
            iTemp2 = (mAddrInfo.mAddrProp.nAddrLen%100)/10;
            iTemp3 = mAddrInfo.mAddrProp.nAddrLen%10;

            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp1);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 6*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp2);
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*vector index = 7*/
            nTmpValue = PlcMathMethod::intToAsciiValue(iTemp3);
            mSendPackge.sendDataList.push_back(nTmpValue);

            mSendPackge.sendDataList.push_back(0x20);

            for(int i = 0; i < mAddrInfo.mAddrProp.nAddrLen;i++)
            {
                if((2*i + 1) < mAddrInfo.nValueList.size())
                {
                    uchar Temp1 = (mAddrInfo.nValueList[2*i + 1] >> 4) & 0xf;
                    uchar Temp2 = mAddrInfo.nValueList[2*i + 1] & 0xf;
                    uchar Temp3 = (mAddrInfo.nValueList[2*i + 0] >> 4) & 0xf;
                    uchar Temp4 = mAddrInfo.nValueList[2*i + 0] & 0xf;

                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(Temp1));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(Temp2));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(Temp3));
                    mSendPackge.sendDataList.push_back(PlcMathMethod::intToAsciiValue(Temp4));

                    if(i < mAddrInfo.mAddrProp.nAddrLen - 1)
                    {
                        mSendPackge.sendDataList.push_back(0x20);
                    }
                }
            }

            /*vector index = 15*/
            nTmpValue = 0x0d;
            mSendPackge.sendDataList.push_back(nTmpValue);

            /*返回数据的长度*/
            mSendPackge.nReturnLen = 4;


            return true;
        }
    }

    return false;
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
    vector<ADDR_TARGET_PROP > mTmpListBit5;
    vector<ADDR_TARGET_PROP > mTmpListBit6;
    vector<ADDR_TARGET_PROP > mTmpListBit7;
    vector<ADDR_TARGET_PROP > mTmpListWord0;
    vector<ADDR_TARGET_PROP > mTmpListWord1;
    vector<ADDR_TARGET_PROP > mTmpListWord2;
    vector<ADDR_TARGET_PROP > mTmpListWord3;
    vector<ADDR_TARGET_PROP > mTmpListWord4;
    vector<ADDR_TARGET_PROP > mTmpListWord5;
    vector<ADDR_TARGET_PROP > mTmpListWord6;
    vector<ADDR_TARGET_PROP > mTmpListWord7;
    vector<ADDR_TARGET_PROP > mTmpListWord8;
    vector<ADDR_TARGET_PROP > mTmpListWord9;
    vector<ADDR_TARGET_PROP > mTmpListWord10;
    vector<ADDR_TARGET_PROP > mTmpListWord11;
    vector<ADDR_TARGET_PROP > mTmpListWord12;
    vector<ADDR_TARGET_PROP > mTmpListWord13;


    for(int i = 0; i < nTotalSize; i++)
    {
        mTmpAddr = mSourceAddrList.at(i);

        REG_TYPE_INDEX eRegIndex = PlcProtocol::getRegTypeFromInt(mTmpAddr.nRegIndex);
        switch(eRegIndex)
        {
        case REG_TYPE_R:
            {
                mTmpListBit0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_B:
            {
                mTmpListBit1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_MR:
            {
                mTmpListBit2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_LR:
            {
                mTmpListBit3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CR:
            {
                mTmpListBit4.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_VB:
            {
                mTmpListBit5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_DM:
            {
                mTmpListWord0.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_W:
            {
                mTmpListWord1.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TM:
            {
                mTmpListWord2.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_Z:
            {
                mTmpListWord3.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_T:
            {
                mTmpListBit6.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TC:
            {
                mTmpListWord5.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_TS:
            {
                mTmpListWord6.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_C:
            {
                mTmpListBit7.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CC:
            {
                mTmpListWord8.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CS:
            {
                mTmpListWord9.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CTH:
            {
                mTmpListWord10.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CTC:
            {
                mTmpListWord11.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_CM:
            {
                mTmpListWord12.push_back(mTmpAddr);
                break;
            }
        case REG_TYPE_VM:
            {
                mTmpListWord13.push_back(mTmpAddr);
                break;
            }
        default:
            {
                break;
            }
        }
    }//end for//把0.x 1.x 2.x····分开
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
        mTmpListBit0 = PlcMathMethod::addrSortOut(mTmpListBit0, nMaxRWlen, 1);
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
        mTmpListBit1 = PlcMathMethod::addrSortOut(mTmpListBit1, nMaxRWlen, 1);
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
        mTmpListBit2 = PlcMathMethod::addrSortOut(mTmpListBit2, nMaxRWlen, 1);
    }

    tmpSize = mTmpListBit2.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit2.at(i));
    }

    mTmpListBit3 = PlcMathMethod::addrBubbleSort(mTmpListBit3);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit3 = PlcMathMethod::addrSortOut(mTmpListBit3, nMaxRWlen, 1);
    }

    tmpSize = mTmpListBit3.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit3.at(i));
    }


    mTmpListBit4 = PlcMathMethod::addrBubbleSort(mTmpListBit4);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit4 = PlcMathMethod::addrSortOut(mTmpListBit4, nMaxRWlen, 1);
    }

    tmpSize = mTmpListBit4.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit4.at(i));
    }

    mTmpListBit5 = PlcMathMethod::addrBubbleSort(mTmpListBit5);
    if(bWriteAddr)
    {
        /*写地址，则只支持长度为1的写*/
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, 1, 1);
    }
    else
    {
        /*读地址，可支持多个长度*/
        mTmpListBit5 = PlcMathMethod::addrSortOut(mTmpListBit5, nMaxRWlen, 1);
    }

    tmpSize = mTmpListBit5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit5.at(i));
    }

    tmpSize = mTmpListBit6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit6.at(i));
    }

    tmpSize = mTmpListBit7.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListBit7.at(i));
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

//    mTmpListWord4 = PlcMathMethod::addrBubbleSort(mTmpListWord4);
//    mTmpListWord4 = PlcMathMethod::addrSortOut(mTmpListWord4, nMaxRWlen/2, 1);
//    tmpSize = mTmpListWord4.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListWord4.at(i));
//    }

    //mTmpListWord5 = PlcMathMethod::addrBubbleSort(mTmpListWord5);
    //mTmpListWord5 = PlcMathMethod::addrSortOut(mTmpListWord5, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord5.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord5.at(i));
    }

    //mTmpListWord6 = PlcMathMethod::addrBubbleSort(mTmpListWord6);
    //mTmpListWord6 = PlcMathMethod::addrSortOut(mTmpListWord6, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord6.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord6.at(i));
    }

//    mTmpListWord7 = PlcMathMethod::addrBubbleSort(mTmpListWord7);
//    mTmpListWord7 = PlcMathMethod::addrSortOut(mTmpListWord7, nMaxRWlen/2, 1);
//    tmpSize = mTmpListWord7.size();
//    for(int i = 0; i < tmpSize; i++)
//    {
//        targetList.push_back(mTmpListWord7.at(i));
//    }


    //mTmpListWord8 = PlcMathMethod::addrBubbleSort(mTmpListWord8);
    //mTmpListWord8 = PlcMathMethod::addrSortOut(mTmpListWord8, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord8.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord8.at(i));
    }

    //mTmpListWord9 = PlcMathMethod::addrBubbleSort(mTmpListWord9);
    //mTmpListWord9 = PlcMathMethod::addrSortOut(mTmpListWord9, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord9.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord9.at(i));
    }

    //mTmpListWord10 = PlcMathMethod::addrBubbleSort(mTmpListWord10);
    //mTmpListWord10 = PlcMathMethod::addrSortOut(mTmpListWord10, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord10.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord10.at(i));
    }

    //mTmpListWord11 = PlcMathMethod::addrBubbleSort(mTmpListWord11);
    //mTmpListWord11 = PlcMathMethod::addrSortOut(mTmpListWord11, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord11.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord11.at(i));
    }

    mTmpListWord12 = PlcMathMethod::addrBubbleSort(mTmpListWord12);
    mTmpListWord12 = PlcMathMethod::addrSortOut(mTmpListWord12, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord12.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord12.at(i));
    }

    mTmpListWord13 = PlcMathMethod::addrBubbleSort(mTmpListWord13);
    mTmpListWord13 = PlcMathMethod::addrSortOut(mTmpListWord13, nMaxRWlen/2, 1);
    tmpSize = mTmpListWord13.size();
    for(int i = 0; i < tmpSize; i++)
    {
        targetList.push_back(mTmpListWord13.at(i));
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
    int nMaxRWlen = 32;//一次最大读取长度2个字节 一个字
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
    mSerialInfo.BAUD_RATE = 9600;
    mSerialInfo.DATA_BIT = 8;
    mSerialInfo.PARITY_TYPE = 2;
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
    /*只有以下四种寄存器支持写*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:                      //REG_TYPE_RLY:
    case REG_TYPE_MR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_9600)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_B:                      //REG_TYPE_RLY:
    case REG_TYPE_VB:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8192)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_LR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_3200)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1440)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_W:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_16384)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_Z:
        {
            if(mAddr.nAddrValue < 1 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_13)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_T:
    case REG_TYPE_TC:
    case REG_TYPE_TS:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_C:
    case REG_TYPE_CC:
    case REG_TYPE_CS:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            if(mAddr.nAddrValue > 2 || mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 4)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTC:
        {
            if(mAddr.nAddrValue > 5 || mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 7)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_9000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_VM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
            {
                return false;
            }
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
 * Function: 获得地址是否可写
 * Parameters: null
 * Return: true or false
 ******************************************************************/
bool PlcProtocol::getAddrCanWrite(ADDR_TARGET_PROP mAddr)
{
    /*只有以下四种寄存器支持写*/
    REG_TYPE_INDEX eRegType = PlcProtocol::getRegTypeFromInt(mAddr.nRegIndex);
    switch(eRegType)
    {
    case REG_TYPE_R:                      //REG_TYPE_RLY:
    case REG_TYPE_MR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_9600)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_B:                      //REG_TYPE_RLY:
    case REG_TYPE_VB:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_8192)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_LR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_3200)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CR:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_1440)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_DM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_32768)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_W:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_16384)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_Z:
        {
            if(mAddr.nAddrValue < 1 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_13)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_TC:
    case REG_TYPE_TS:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_512)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CC:
    case REG_TYPE_CS:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_256)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTH:
        {
            if(mAddr.nAddrValue > 2 || mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 4)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CTC:
        {
            if(mAddr.nAddrValue > 5 || mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > 7)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_CM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_9000)
            {
                return false;
            }
            break;
        }
    case REG_TYPE_VM:
        {
            if(mAddr.nAddrValue < 0 || mAddr.nAddrValue + mAddr.nAddrLen > ADDR_MAX_VALUE_10000)
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
            eRegType = REG_TYPE_R;
            break;
        }
    case 1:
        {
            eRegType = REG_TYPE_B;
            break;
        }
    case 2:
        {
            eRegType = REG_TYPE_MR;
            break;
        }
    case 3:
        {
            eRegType = REG_TYPE_LR;
            break;
        }
    case 4:
        {
            eRegType = REG_TYPE_CR;
            break;
        }
    case 5:
        {
            eRegType = REG_TYPE_VB;
            break;
        }
    case 6:
        {
            eRegType = REG_TYPE_T;
            break;
        }
    case 7:
        {
            eRegType = REG_TYPE_C;
            break;
        }
    case 8:
        {
            eRegType = REG_TYPE_DM;
            break;
        }
    case 9:
        {
            eRegType = REG_TYPE_W;
            break;
        }
    case 10:
        {
            eRegType = REG_TYPE_TM;
            break;
        }
    case 11:
        {
            eRegType = REG_TYPE_Z;
            break;
        }
    case 12:
        {
            eRegType = REG_TYPE_TC;
            break;
        }
    case 13:
        {
            eRegType = REG_TYPE_TS;
            break;
        }
    case 14:
        {
            eRegType = REG_TYPE_CC;
            break;
        }
    case 15:
        {
            eRegType = REG_TYPE_CS;
            break;
        }
    case 16:
        {
            eRegType = REG_TYPE_CTH;
            break;
        }
    case 17:
        {
            eRegType = REG_TYPE_CTC;
            break;
        }
    case 18:
        {
            eRegType = REG_TYPE_CM;
            break;
        }
    case 19:
        {
            eRegType = REG_TYPE_VM;
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

    if(DealWithString::compare("R", sRegName))
    {
        nRegIndex = 0;
        return true;
    }
    else if((DealWithString::compare("B", sRegName)))
    {
        nRegIndex = 1;
        return true;
    }
    else if((DealWithString::compare("MR", sRegName)))
    {
        nRegIndex = 2;
        return true;
    }
    else if((DealWithString::compare("LR", sRegName)))
    {
        nRegIndex = 3;
        return true;
    }
    else if((DealWithString::compare("CR", sRegName)))
    {
        nRegIndex = 4;
        return true;
    }
    else if((DealWithString::compare("VB", sRegName)))
    {
        nRegIndex = 5;
        return true;
    }
    else if((DealWithString::compare("T", sRegName)))
    {
        nRegIndex = 6;
        return true;
    }
    else if((DealWithString::compare("C", sRegName)))
    {
        nRegIndex = 7;
        return true;
    }
    else if((DealWithString::compare("DM", sRegName)))
    {
        nRegIndex = 8;
        return true;
    }
    else if((DealWithString::compare("W", sRegName)))
    {
        nRegIndex = 9;
        return true;
    }
    else if((DealWithString::compare("TM", sRegName)))
    {
        nRegIndex = 10;
        return true;
    }
    else if((DealWithString::compare("Z", sRegName)))
    {
        nRegIndex = 11;
        return true;
    }
    else if((DealWithString::compare("TC", sRegName)))
    {
        nRegIndex = 12;
        return true;
    }
    else if((DealWithString::compare("TS", sRegName)))
    {
        nRegIndex = 13;
        return true;
    }
    else if((DealWithString::compare("CC", sRegName)))
    {
        nRegIndex = 14;
        return true;
    }
    else if((DealWithString::compare("CS", sRegName)))
    {
        nRegIndex = 15;
        return true;
    }
    else if((DealWithString::compare("CTH", sRegName)))
    {
        nRegIndex = 16;
        return true;
    }
    else if((DealWithString::compare("CTC", sRegName)))
    {
        nRegIndex = 17;
        return true;
    }
    else if((DealWithString::compare("CM", sRegName)))
    {
        nRegIndex = 18;
        return true;
    }
    else if((DealWithString::compare("VM", sRegName)))
    {
        nRegIndex = 19;
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


