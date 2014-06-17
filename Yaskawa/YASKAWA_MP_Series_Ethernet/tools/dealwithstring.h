#ifndef DEALWITHSTRING_H
#define DEALWITHSTRING_H

#include "stdafx.h"

using namespace std;

class DealWithString
{
public:
    DealWithString();

    static vector<string > split(string sSourceStr, string sep);
    static string mid(string sSourceStr, int nStart, int len);
    static long int stringToInt(string sSourceStr, bool &bSuccess, int base = 10);
    static double stringToDouble(string sSourceStr, bool &bSuccess);
    static bool getIsContainSubstr(string sSourceStr, string sSubStr, int &nStartPos);
    static bool compare(string sOneStr, string sOtherStr, bool bCaseSensitive = false);
};

#endif // DEALWITHSTRING_H
