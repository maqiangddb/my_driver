#include "dealwithstring.h"
#include <errno.h>

/*
QString s2q(const string &s)
{
    return QString(QString::fromLocal8Bit(s.c_str()));
}
string q2s(const QString &s)
{
    return string((const char *)s.toLocal8Bit());
}
*/

DealWithString::DealWithString()
{
}

/******************************************************************
 * Function: 字符串分割函数
 * Parameters: null
 * Return: 分割后的字符串集合
 ******************************************************************/
vector<string > DealWithString::split(string sSourceStr, string sep)
{
    vector<string > resultList;
    resultList.clear();
    if(sep.empty() || sSourceStr.empty())
    {
        resultList.push_back(sSourceStr);
        return resultList;
    }

    /*start split sSourceStr*/
    string sCompareStr;
    sCompareStr.clear();
    string sSplitStr;
    sSplitStr.clear();

    int nSepLen = sep.length();
    int nSourceLen = sSourceStr.length();
    int nSearchLen = nSourceLen - nSepLen;
    int nSplitPos = 0;

    for(int i = 0; i < nSearchLen; i++)
    {
        sCompareStr.clear();
        for(int j = i; j < i + nSepLen; j++)
        {
            sCompareStr.push_back(sSourceStr.at(j));
        }
        if(sep == sCompareStr)
        {
            sSplitStr = sSourceStr.substr(nSplitPos, i - nSplitPos);
            resultList.push_back(sSplitStr);
            nSplitPos = i + nSepLen;
        }
    }
    sSplitStr = sSourceStr.substr(nSplitPos,nSourceLen - nSplitPos);
    if(!sSplitStr.empty())
    {
        resultList.push_back(sSplitStr);
    }

    return resultList;
}

/******************************************************************
 * Function: 字符串转整数
 * Parameters: null
 * Return: 转换后的数
 ******************************************************************/
long int DealWithString::stringToInt(string sSourceStr, bool &bSuccess, int base)
{
    char *endptr = NULL;
    const char *str = sSourceStr.c_str();
    long int nResult = 0;
    nResult = strtol(str,&endptr,base);
    if(NULL != endptr)
    {
        /*去掉空格*/
        while(*endptr == ' ')
        {
            endptr++;
        }

        /*不等于结束符则成功*/
        if(*endptr == '\0')
        {
            bSuccess = true;
        }
        else
        {
            bSuccess = false;
        }
    }
    return nResult;
}

/******************************************************************
 * Function: 字符串转浮点数
 * Parameters: null
 * Return: 转换后的数
 ******************************************************************/
double DealWithString::stringToDouble(string sSourceStr, bool &bSuccess)
{
    char *endptr = NULL;
    const char *str = sSourceStr.c_str();
    double nResult = 0;
    nResult = strtod(str,&endptr);
    if(NULL != endptr)
    {
        /*去掉空格*/
        while(*endptr == ' ')
        {
            endptr++;
        }

        /*不等于结束符则成功*/
        if(*endptr == '\0')
        {
            bSuccess = true;
        }
        else
        {
            bSuccess = false;
        }
    }
    return nResult;
}

/******************************************************************
 * Function: 判断一个字符串是否包含另一个字符串，如果包含则返回起始位置
 * Parameters:
 * Return: true or false
 ******************************************************************/
bool DealWithString::getIsContainSubstr(string sSourceStr, string sSubStr, int &nStartPos)
{
    int nSubStrLen = sSubStr.length();
    int nStrLen = sSourceStr.length();
    if(nSubStrLen <= 0 || nStrLen < nSubStrLen) return false;

    nStartPos = -1;
    nStartPos = sSourceStr.find(sSubStr);
    if(nStartPos != sSourceStr.npos)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/******************************************************************
 * Function: 比较两个字符串是否相等
 * Parameters: bCaseSensitive : true 是区分大小写， false：不区分大小写
 * Return: 转换后的数
 ******************************************************************/
bool DealWithString::compare(string sOneStr, string sOtherStr, bool bCaseSensitive)
{
    /*区分大小写*/
    if(bCaseSensitive)
    {
        if(sOneStr.compare(sOtherStr) == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        int nResult = -1;

#ifdef ANDROID_PLC_DLL
        nResult = strcasecmp(sOneStr.data(), sOtherStr.data());
#else
        nResult = stricmp(sOneStr.data(), sOtherStr.data());
#endif
        if(nResult == 0)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}


string DealWithString::mid(string sSourceStr, int nStart, int len)
{
    string sResultStr;
    sResultStr.clear();
    if(sSourceStr.empty()) return sResultStr;

    int nStrLen = sSourceStr.length();
    if(nStart < 0)
    {
        nStart = 0;
    }
    if(nStart >= nStrLen)
    {
        return sResultStr;
    }
    if(len < 0 || nStrLen < nStart + len)
    {
        len = nStrLen - nStart;
    }

    for(int i = nStart; i < nStart + len; i++)
    {
        sResultStr.push_back(sSourceStr.at(i));
    }

    return sResultStr;
}
