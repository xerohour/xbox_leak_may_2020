//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1997-1999. All rights reserved.
//
//  MODULE: NMMonitor.h
//
//  External Header for all NPP monitors
//=============================================================================

#ifndef NMMONITOR_H
#define NMMONITOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <netmon.h>
#include <nmMCS.h>
#include <nmblob.h>
#include <tchar.h>

// Forward reference
struct MONITOR;
typedef MONITOR* PMONITOR;

typedef void (WINAPI* MCSALERTPROC) (PMONITOR pMonitor, char* alert);

//****************************************************************************
// Our exported Monitor functions, that must be supported by ALL monitors 
//****************************************************************************
// Create the Monitor, function called "CreateMonitor". The
// argument is a potential configuration structure
typedef DWORD (WINAPI* CREATEMONITOR)(PMONITOR*     ppMonitor, 
                                      HBLOB         hInputNPPBlob,
                                      char*         pConfiguration,
                                      MCSALERTPROC  McsAlertProc);

// Destroy the Monitor, function called "DestroyMonitor"
typedef DWORD (WINAPI* DESTROYMONITOR)(PMONITOR);

// We need the monitor's NPP filter blob: "GetMonitorFilter"
typedef DWORD (WINAPI* GETMONITORFILTER) (HBLOB* pFilterBlob);

// Get the monitor configuration "GetMonitorConfig"
// The pMonitor argument can not be null
typedef DWORD (WINAPI* GETMONITORCONFIG) (PMONITOR pMonitor,
                                          char**   ppScript,
                                          char**   ppConfiguration);

// Set the monitor configuration "SetMonitorConfig"
// The pMonitor argument can not be null
typedef DWORD (WINAPI* SETMONITORCONFIG) (PMONITOR pMonitor, 
                                          char* pConfiguration);

// The monitor's connect function: "ConnectMonitor"
typedef DWORD (WINAPI* CONNECTMONITOR) (PMONITOR pMonitor);

// The monitor's start function: "StartMonitor"
typedef DWORD (WINAPI* STARTMONITOR) (PMONITOR pMonitor, char** ppResponse);

// The monitor's stop function: "StopMonitor"
typedef DWORD (WINAPI* STOPMONITOR) (PMONITOR pMonitor);

// Get the monitor status: "GetMonitorStatus"
typedef DWORD (WINAPI* GETMONITORSTATUS) (PMONITOR pMonitor, DWORD* pStatus);

//****************************************************************************
// Optional function that allows the monitor dll to do specific functions
// prior to the creation of any monitors. "OneTimeSetup"
typedef DWORD (WINAPI* ONETIMESETUP) (void);
//****************************************************************************

//****************************************************************************
// Optional function that provides a description of the monitor
//****************************************************************************
// For current display porpoises, we could use this: "DescribeSelf"
typedef DWORD (WINAPI* DESCRIBESELF) (const char** ppName,
                                      const char** ppDescription);

#ifdef __cplusplus
}
#endif

//=============================================================================
//  Semi-useful class which writes out frame to a cap file Netmon can read.
//=============================================================================
struct CSCAPF;

class __declspec (dllexport) CCaptureFile
{
public:
    CCaptureFile(const char* FileName,
                 HWND hwndParent,
                 DWORD FileSize,
                 int EstimatedAvgFrameSize);
    CCaptureFile(const char* FileName,
                 DWORD FileSize,
                 int EstimatedAvgFrameSize);

    ~CCaptureFile();
    
    DWORD SaveFrame(LPFRAME_DESCRIPTOR lpFrameDesc);
    DWORD Save(void);
    DWORD Resize(DWORD NewFileSize);
    DWORD SetNetworkInfo(LPNETWORKINFO lpNetworkInfo);

private:
    CSCAPF* m_cscapf;
};

//=============================================================================
//  Class definition for the resource string loader
//=============================================================================
#define INITIAL_TABLE_SIZE          (100 * sizeof(LPSTR))
#define INITIAL_TEMP_BUFFER_SIZE    512

// IDS Class
class __declspec (dllexport) IDS
{
    public:
        IDS( DWORD StringBase, HINSTANCE hInstance );
        ~IDS();

        LPSTR GetString( DWORD StringID );

    private:
        // configuration parameters
        DWORD m_dwStringBase;
        HINSTANCE m_hInstance;

        // the array of pointers to strings
        // (this holds strings already loaded)
        LPSTR *m_pMasterList;

        // this counts the number of Entries (not bytes)
        DWORD m_dwMasterListLength;

        // a temporary buffer to hold fresh strings
        LPSTR m_szTempBuffer;
        DWORD m_dwTempBufferLength;

        // this holds the default string ""
        char m_szBlank[1];
};

//=============================================================================
//  Various helpers for monitors, IP monitors, in particular.
//=============================================================================
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    DWORD Mask;
    DWORD Addr;
} SAddrMaskPair;

// Convert HTML CP_UTF8 version to Unicode
__declspec (dllexport) WCHAR* HTMLValueToUnicode(const TCHAR* pValue);

// Replace HTML funkiness with special characters (like ;:&, etc)
__declspec (dllexport) void CleanHTMLString(TCHAR* string);

// Load TCHAR string from html config string
__declspec (dllexport) BOOL LoadTCHAR(const char*  pConfig, 
                                      const char*  pVarName, 
                                      TCHAR**      ppszString);

// Load DWORD from html config string
__declspec (dllexport) BOOL LoadDWORD(const char*  pConfig, 
                                      const char*  pVarName, 
                                      DWORD* pValue);

// Load IP address list from HTML TEXTAREA config string
__declspec (dllexport) BOOL LoadHTMAddrMaskPairs(const char*     pConfig,
                                                 const char*     pVarName,
                                                 SAddrMaskPair** ppAddrMask, 
                                                 DWORD*          pNumAddresses);

// Load IP address list from HTML TEXTAREA config string
__declspec (dllexport) BOOL LoadIPAddresses(const char* pConfig, 
                                            const char* pVarName,
                                            DWORD**     ppAddresses,
                                            DWORD*      pNumAddresses);

// Load IPX address list from HTML TEXTAREA config string
__declspec (dllexport) BOOL LoadIPXAddresses(const char*   pConfig, 
                                             const char*   pVarName,
                                             IPX_ADDRESS** ppAddresses,
                                             DWORD*        pNumAddresses);

// Load MAC address list from HTML TEXTAREA config string
__declspec (dllexport) BOOL LoadMACAddresses(const char*   pConfig, 
                                             const char*   pVarName,
                                             LPBYTE*       ppAddresses,
                                             DWORD*        pNumAddresses);

// SpecialFormatIPAddresses
// Uses the address mask to stick in '*'s, as necessary. Bonus, dude.
__declspec (dllexport) int SpecialFormatIPAddresses(char* str, 
                                                    int index, 
                                                    DWORD dwaddr);

// LoadStringAddrMaskPair
// Takes a string like "157.54.*.*" and creates DWORD address and mask pair
__declspec (dllexport) BOOL LoadStringAddrMaskPair(SAddrMaskPair* pAddrMask, 
                                                   const char* str);

// LoadStringAddr
// Takes a string like "157.54.32.45" and creates DWORD address
__declspec (dllexport) BOOL LoadStringAddr( DWORD* pAddress, const char* str);

// LoadHTMLScript 
// allocates and loads a string with the contents of the file pointed
// to by argument szPath.
__declspec (dllexport) char* LoadHTMLScript(char* szPath);
/*
// BOOL LoadProfileAddressSection 
// Takes an ini file section that looks like this:
sip0=157.54.*.*
sip1=157.55.*.*
sip2=157.56.*.*
sip3=157.57.*.*
sip4=157.58.*.*
sip5=157.59.*.*
sip6=157.60.*.*
sip7=157.61.*.*
sip8=157.62.*.*
sip9=192.168.*.*
and allocates and fills an array of SAddrMaskPairs.
*/
__declspec (dllexport) BOOL LoadProfileAddressSection(const char* Section, 
                               SAddrMaskPair** ppAddrMaskPairs, 
                               DWORD* pNumAddrMaskPairs);

// LoadProfileAddresses
/* Takes an ini file section that looks like this:
sip0=157.54.32.45
sip1=157.55.25.35
and allocates and fills an array of DWORDs.
*/
__declspec (dllexport) BOOL LoadProfileAddresses(const char* Section,
                               DWORD**  ppAddresses,
                               DWORD*   pNumAddresses);
//
// Some MAC helper functions for cracking, These belong in cracker.
__declspec (dllexport) BOOL GetMACAddresses_Ethernet( LPFRAME_DESCRIPTOR lpFrameDesc,
                               LPBYTE*            ppSrcMacAddress,
                               LPBYTE*            ppDstMacAddress);

__declspec (dllexport) BOOL GetMACAddresses_FDDI(     LPFRAME_DESCRIPTOR lpFrameDesc,
                               LPBYTE*            ppSrcMacAddress,
                               LPBYTE*            ppDstMacAddress);

__declspec (dllexport) BOOL GetMACAddresses_TokenRing(LPFRAME_DESCRIPTOR lpFrameDesc,
                               LPBYTE*            ppSrcMacAddress,
                               LPBYTE*            ppDstMacAddress);

__declspec (dllexport) BOOL LoadTextAreaAsMultiSZ( IN  const char*  pConfig,
                            IN  const char*  pVarName,
                            IN        BOOL   fNullDivider,
                            OUT       char * szBuffer,
                            OUT       DWORD* Length );

#ifdef __cplusplus
}
#endif


#endif
