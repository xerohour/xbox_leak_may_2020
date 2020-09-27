
#ifndef __XCRYPT_H__
#define __XCRYPT_H__

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdarg.h>
#include <time.h>
#include "list.h"
#include "property.h"

#define ARRAYSIZE(a) (sizeof(a) / sizeof(a[0]))

#if DBG == 1
    #define DEBUG
#else
    #undef DEBUG
#endif



// Debug defines. Note that we could completely remove in retail, but since this
// is a tool, it's quite useful to be able to optionally turn on debug spew.
#define TRACE_ALWAYS      0       // Always display
#define TRACE_VERBOSE     1       // Display if verbose logging turned on
#define TRACE_DEBUG       2       // Display if debug-level logging turned on
#define TRACE_SPEW        3       // Display only if very chatty logging turned on

#define PASSWORD_MAXSIZE  21
#define PASSWORD_MINLEN   8


class CXCryptMain
{

public:
    CPropertyBundle m_prop;
    int             m_nTraceLevel;
    CListHead       m_Warnings;         // Warnings to be displayed at the end--list of CStrNode

    CXCryptMain() 
        : m_nTraceLevel(TRACE_VERBOSE),
          m_pszExitMsg(NULL)
    { 
    }

    ~CXCryptMain() 
    { 
        // Global object, everything should be cleaned up by Exit() 
    }

    void Initialize(int argc, char** argv);
    void ReadSwitchesFromFile(LPCSTR pszFN);
    void Exit(int nReturnCode, LPCSTR lpszFatalError);
    void SetExitMsg(HRESULT hr, LPCSTR pszExitMsg);
    void SetExitMsg(LPCSTR pszExitMsg);
    void AddWarning(LPCSTR pszFormat, ...);

    void BuildImage();
    void PrintUsage();
  
    // static functions
    static void TraceOut(int nLevel, LPCSTR lpszFormat, va_list vararg); // Use TRACE_OUT inline
    static void ErrorOut(BOOL bErr, LPCSTR lpszFormat, va_list vararg);  // use ERROR_OUT or WARNING_OUT inlines


private:
    void ParseSwitch(LPCTSTR pszArg);

    LPSTR m_pszExitMsg;
};


typedef struct
{
    enum VARTYPE {VAR_NONE, VAR_INT, VAR_STRING, VAR_STRINGLIST};

    LPCTSTR lpCmdLine;
    LPCTSTR lpPropName;
    VARTYPE vartype;
    void* lpVar;

    static int _cdecl Compare(const void*, const void*);
} CMDTABLE;


class CStrNode : public CNode
{
public:
    CStrNode() 
        : m_pszValue(NULL)
    {
    }
    ~CStrNode()
    {
        if (m_pszValue != NULL)
            free(m_pszValue);
    }
    void SetValue(LPCSTR pszValue)
    {
        if (m_pszValue != NULL)
            free(m_pszValue);
        m_pszValue = _strdup(pszValue);
    }
    LPCSTR GetValue()
    {
        return m_pszValue;
    }

    virtual void Dump();

private:
    LPSTR m_pszValue;
};



class CRandom
{
public:
    CRandom(void);

    ULONG Rand(void);
    void RandBytes(LPBYTE pb, DWORD cb);
private:
    static const ULONG rgulMults[];
    void Seed(ULONG ulSeed);
    ULONG m_ulMult;
    ULONG m_ulCur;
    ULONG m_ulMask;
};



// Global class
extern CXCryptMain g_ib;

#define BLOCK

inline void TRACE_OUT(int nLevel, LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CXCryptMain::TraceOut(nLevel, lpszFormat, vararg);
    va_end(vararg);
}


inline void ERROR_OUT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CXCryptMain::ErrorOut(TRUE, lpszFormat, vararg);
    va_end(vararg);
}


inline void WARNING_OUT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    CXCryptMain::ErrorOut(FALSE, lpszFormat, vararg);
    va_end(vararg);
}


inline void DEBUG_STOP_FORMAT(LPCSTR lpszFormat, ...)
{
    va_list vararg;
    va_start(vararg, lpszFormat);
    TRACE_OUT(TRACE_VERBOSE, TEXT("DebugStop:"));
    CXCryptMain::TraceOut(TRACE_ALWAYS, lpszFormat, vararg);
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


#endif // #ifndef __XCRYPT_H__
 
