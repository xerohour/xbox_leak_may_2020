/*
    FILE:   regfuncs.h
    DATE:   4/8/99

    This file holds code to get certain things from the registry.
*/

typedef struct  tagDEVICEANDTYPE
{
    LPCHAR  lpszName;
    char    cType;
}   DEVICEANDTYPE;

typedef DEVICEANDTYPE FAR *LPDEVICEANDTYPE;


// This routine is in getit.c
extern int CFUNC GrabModeInfoString (LPREGDATA lpRegData, ULONG dwLogDevice, LPDEVDATA lpDevData, LPCHAR lpModeString, LPMODEINFO lpMI, int nType);

// All these are in regfuncs.c
extern int CFUNC GetLocalPath (LPREGDATA lpRegData, LPCHAR lpBuffer);
extern int CFUNC GetLocalNvidiaPath (LPREGDATA lpRegData, LPCHAR lpBuffer);
extern int CFUNC GetLocalNvidiaDisplayPath (LPREGDATA lpRegData, LPCHAR lpBuffer);
extern int CFUNC GetLocalNvidiaDisplayLogPath (LPREGDATA lpRegData, ULONG dwLogDevice, LPCHAR lpBuffer);
extern int CFUNC GetLocalNvidiaDisplayDevPath (LPREGDATA lpRegData, LPDEVDATA lpDevData, LPCHAR lpBuffer);
extern ULONG CFUNC GetLocalSubKey (LPREGDATA lpRegData);
extern ULONG CFUNC GetLocalNvidiaSubKey (LPREGDATA lpRegData);
extern ULONG CFUNC GetLocalNvidiaDisplaySubKey (LPREGDATA lpRegData);
extern ULONG CFUNC GetLocalNvidiaDisplayLogSubKey (LPREGDATA lpRegData, ULONG dwLogDevice);
extern ULONG CFUNC GetLocalNvidiaDisplayDevSubKey (LPREGDATA lpRegData, LPDEVDATA lpDevData);
extern int CFUNC GetRegValue (ULONG hKey, LPCHAR lpStr, int nDefault);
extern int CFUNC GetLocalRegValue (LPREGDATA lpRegData, LPCHAR lpStr, int nDefault);
extern int CFUNC GetLocalNvidiaRegValue (LPREGDATA lpRegData, LPCHAR lpStr, int nDefault);
extern int CFUNC GetLocalNvidiaDisplayRegValue (LPREGDATA lpRegData, LPCHAR lpStr, int nDefault);
extern int CFUNC GetLocalNvidiaDisplayLogRegValue (LPREGDATA lpRegData, ULONG dwLogDevice, LPCHAR lpStr, int nDefault);
extern int CFUNC GetLocalNvidiaDisplayDevRegValue (LPREGDATA lpRegData, LPDEVDATA lpDevData, LPCHAR lpStr, int nDefault);
extern int CFUNC GetRegString (ULONG hKey, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC GetLocalRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC GetLocalNvidiaRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC GetLocalNvidiaDisplayRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC GetLocalNvidiaDisplayLogRegString (LPREGDATA lpRegData, ULONG dwLogDevice, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC GetLocalNvidiaDisplayDevRegString (LPREGDATA lpRegData, LPDEVDATA lpDevData, LPCHAR lpStr, LPCHAR lpRet);
extern int CFUNC SetRegString (ULONG hKey, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int CFUNC SetRegValue (ULONG hKey, LPCHAR lpStr, int nVal);
extern int CFUNC SetLocalRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int CFUNC SetLocalNvidiaRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int CFUNC SetLocalNvidiaRegValue (LPREGDATA lpRegData, LPCHAR lpStr, int nVal);
extern int CFUNC SetLocalNvidiaDisplayRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int CFUNC SetLocalNvidiaDisplayLogRegString (LPREGDATA lpRegData, ULONG dwLogDevice, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int CFUNC SetLocalNvidiaDisplayDevRegString (LPREGDATA lpRegData, LPDEVDATA lpDevData, LPCHAR lpStr, LPCHAR lpData, ULONG dwDataLen);
extern int WINAPI DeleteRegEntry (ULONG hKey, LPCHAR lpStr);
extern int WINAPI DeleteLocalRegEntry (LPREGDATA lpRegData, LPCHAR lpStr);
extern int WINAPI DeleteLocalNvidiaRegEntry (LPREGDATA lpRegData, LPCHAR lpStr);
extern int WINAPI DeleteLocalNvidiaDisplayRegEntry (LPREGDATA lpRegData, LPCHAR lpStr);
extern int WINAPI DeleteLocalNvidiaDisplayLogRegEntry (LPREGDATA lpRegData, ULONG dwLogDevice, LPCHAR lpStr);
extern int WINAPI DeleteLocalNvidiaDisplayDevRegEntry (LPREGDATA lpRegData, LPDEVDATA lpDevData, LPCHAR lpStr);
extern ULONG CFUNC GetMonitorLocalSubKey (LPREGDATA lpRegData);
extern ULONG CFUNC GetMonitorLocalRegString (LPREGDATA lpRegData, LPCHAR lpStr, LPCHAR lpData);
extern int CFUNC PrintString0 (LPCHAR lpStr);
extern int CFUNC PrintString1 (LPCHAR lpStr, ULONG dwValue);
extern LPCHAR CFUNC MemoryAlloc (ULONG dwSize);
extern int CFUNC MemoryFree (LPULONG lpMem);

