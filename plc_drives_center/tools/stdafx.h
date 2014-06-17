#ifndef STDAFX_H
#define STDAFX_H

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <list>
#include <deque>
#include <utility>
#include <algorithm>
#include <functional>
#include <string.h>

using namespace std;

/*编译给上位用的编译开关*/
//#undef ANDROID_PLC_DLL

/*编译给下位android用的编译开关*/
#define ANDROID_PLC_DLL

#ifndef ANDROID_PLC_DLL
#ifdef __cplusplus
#define DLL_EXPORT extern "C" __declspec(dllexport)
#define DLL_IMPORT extern "C" __declspec(dllimport)
#else
#define DLL_EXPORT __declspec(dllexport)
#define DLL_IMPORT __declspec(dllimport)
#endif
#else
#ifdef __cplusplus
#define DLL_EXPORT extern "C"
#define DLL_IMPORT extern "C"
#else
#define DLL_EXPORT
#define DLL_IMPORT
#endif
#endif

#define uchar unsigned char
#define ushort unsigned short
#define uint unsigned int
#define MAXADDR     9999990
//#define RECOLLATE_ADDR_TIMES          20
//#define NULL                         0

#if 1
#ifdef ANDROID_PLC_DLL

#include <android/log.h>
#define LOG_TAG "debug"

#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#endif
#endif

#endif // STDAFX_H
