
#ifndef __ROMBLD_H__
#define __ROMBLD_H__

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdarg.h>
#include <time.h>
#include "list.h"
#include "property.h"
#include "util.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

#if DBG == 1
    #define DEBUG
#else
    #undef DEBUG
#endif



// Debug defines. Note that we could completely remove in retail, but since this
// is a tool, it's quite useful to be able to optionally turn on debug spew.
#define TRACE_ALWAYS    0       // Always display
#define TRACE_VERBOSE   1       // Display if verbose logging turned on
#define TRACE_DEBUG     2       // Display if debug-level logging turned on
#define TRACE_SPEW      3       // Display only if very chatty logging turned on


class CMemFile;

enum { ROMBLD_XDK, ROMBLD_XM3, ROMBLD_XM3P };


class CRomBuilder
{

public:
    CPropertyBundle m_prop;
    int             m_nTraceLevel;
    CListHead       m_Warnings;         // Warnings to be displayed at the end--list of CStrNode
    CListHead       m_MiscRomFileList;
    int             m_nRomSize;
    int             m_HackInitTable;
    int             m_nBuildFor;
    char            m_szPassword[PASSWORD_MAXSIZE];
    char            m_szRomEncKeyFN[MAX_PATH];
    char            m_szEEPROMKeyFN[MAX_PATH];
    char            m_szCERTKeyFN[MAX_PATH];
    char            m_szPubKeyFN[MAX_PATH];
    BOOL            m_UseSpecifiedKeys;

    CRomBuilder()
        : m_nTraceLevel(TRACE_VERBOSE),
          m_nRomSize(256),
          m_pszExitMsg(NULL),
          m_HackInitTable(0),
          m_UseSpecifiedKeys(FALSE)
    {
        m_szPassword[0] = '\0';
        m_szRomEncKeyFN[0] = '\0';
        m_szEEPROMKeyFN[0] = '\0';
        m_szCERTKeyFN[0] = '\0';
        m_szPubKeyFN[0] = '\0';
    }

    ~CRomBuilder()
    {
        // Global object, everything should be cleaned up by Exit()
    }

    void Initialize(int argc, char** argv);
    void ReadSwitchesFromFile(LPCSTR pszFN);
    void Exit(int nReturnCode, LPCSTR lpszFatalError);
    void SetExitMsg(HRESULT hr, LPCSTR pszExitMsg);
    void SetExitMsg(LPCSTR pszExitMsg);
    void AddWarning(LPCSTR pszFormat, ...);

    void BuildImage(BOOL External);
    void UpdateKeys(PBYTE rgbRandomHash, PBYTE rgbRandomKey,
        PBYTE rgbKernelKey, PBYTE rgbBldrEncKey, PBYTE pBldrKeyData);

    BOOL LocateDataSection(CMemFile &PEFile, LPVOID *ppvDataSectionStart,
        DWORD *pdwDataSectionSize);
    PVOID LocateExportOrdinal(CMemFile& PEFile, ULONG Ordinal);

    void PrintUsage();

    // static functions
    static void TraceOut(int nLevel, LPCSTR lpszFormat, va_list vararg); // Use TRACE_OUT inline
    static void ErrorOut(BOOL bErr, LPCSTR lpszFormat, va_list vararg);  // use ERROR_OUT or WARNING_OUT inlines


private:
    void ParseSwitch(LPCTSTR pszArg);

    LPSTR m_pszExitMsg;
};



// Global class
extern CRomBuilder g_ib;

#define BLOCK

inline void TRACE_OUT(int nLevel, LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CRomBuilder::TraceOut(nLevel, lpszFormat, vararg);
    va_end(vararg);
}


inline void ERROR_OUT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CRomBuilder::ErrorOut(TRUE, lpszFormat, vararg);
    va_end(vararg);
}


inline void WARNING_OUT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CRomBuilder::ErrorOut(FALSE, lpszFormat, vararg);
    va_end(vararg);
}


inline void DEBUG_STOP_FORMAT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    TRACE_OUT(TRACE_VERBOSE, TEXT("DebugStop:"));
    CRomBuilder::TraceOut(TRACE_ALWAYS, lpszFormat, vararg);
    va_end(vararg);
}

#ifdef DEBUG
#ifdef _M_IX86
#define DEBUG_STOP_HERE { __asm int 3 }
#else
#define DEBUG_STOP_HERE { DebugBreak(); }
#endif
#else
#define DEBUG_STOP_HERE
#endif
    
#define DEBUG_STOP(x)   \
{                       \
    TRACE_OUT(TRACE_VERBOSE, TEXT("DebugStop in %s at line %d (message follows):"), TEXT(__FILE__), __LINE__); \
    TRACE_OUT(TRACE_ALWAYS, x); \
    DEBUG_STOP_HERE \
}

#define ASSERT(x)   \
    (void)((x) || (TRACE_OUT(TRACE_VERBOSE, TEXT("Assert: failed [%s] in %s(%d)"), #x, \
        TEXT(__FILE__), __LINE__),0))


#endif // #ifndef __ROMBLD_H__
