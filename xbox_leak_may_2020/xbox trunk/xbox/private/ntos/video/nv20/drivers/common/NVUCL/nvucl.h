/*
 * (C) Copyright NVIDIA Corporation Inc., 1996,1997,1998. All rights reserved.
 */
/********************************* Direct 3D *******************************\
*                                                                           *
* Module: NVUCL.H                                                           *
*   NVIDIA Unified Component Library Public Interface                       *
*                                                                           *
*****************************************************************************
*                                                                           *
* History:                                                                  *
*       Ben de Waal                 07/06/98 - Created                      *
*                                                                           *
\***************************************************************************/
#ifndef _nvucl_h
#define _nvucl_h
#pragma pack(push,1)

/*
 *---------------------------------------------------------------------------
 * Constants
 *---------------------------------------------------------------------------
 */

/*
 * interface identifiers
 */
#define NVUCL_PROFILER_ID               0x4e560001
#define NVUCL_MATRIX_ID                 0x4e560002
#define NVUCL_DISASM_ID                 0x4e560003

/*
 * error codes
 */
#define NVUCL_OK                        0
#define NVUCL_ERROR_INVALIDPARAM        1
#define NVUCL_ERROR_UNKNOWNINTERFACE    2
#define NVUCL_ERROR_UNKNOWN             3
#define NVUCL_ERROR_OUTOFRESOURCE       4
#define NVUCL_ERROR_INVALIDCALL         5
#define NVUCL_ERROR_UNKNOWNENTITY       6
#define NVUCL_ERROR_NOTIMLEMENTED       7
#define NVUCL_ERROR_ACCESSDENIED        8

/*
 *---------------------------------------------------------------------------
 * Types
 *---------------------------------------------------------------------------
 */
typedef DWORD                           NVUCL_RESULT;

/*
 *---------------------------------------------------------------------------
 * Macros
 *---------------------------------------------------------------------------
 */

/*
 * compiler specific
 */
#ifdef __cplusplus
#define NVUCL_INTERFACE_BEGIN           struct NVUCL_INTERFACE : public NVUCL_BASE {
#define NVUCL_INTERFACE_END             };
#define NVUCL_METHOD(x)                 virtual NVUCL_RESULT NVUCL_STDCALL x (
#define NVUCL_METHOD_(x)                virtual NVUCL_RESULT NVUCL_STDCALL x (void
#define NVUCL_PURE                      )=0
#else //!__cplusplus
#define NVUCL_INTERFACE_BEGIN           typedef struct { struct {
#define NVUCL_INTERFACE_END             } *pVPtr; } *NVUCL_INTERFACE;
#define NVUCL_METHOD(x)                 NVUCL_RESULT (NVUCL_STDCALL *x) (void*,
#define NVUCL_METHOD_(x)                NVUCL_RESULT (NVUCL_STDCALL *x) (void*
#define NVUCL_PURE                      )
#endif //!__cplusplus
#define NVUCL_STDCALL                   __stdcall
#ifdef __NVUCL_INCLUDED
#define NVUCL_EXPORT                    __declspec(dllexport)
#else //!__NVUCL_INCLUDED
#define NVUCL_EXPORT
#endif //!__NVUCL_INCLUDED

/*
 * Dynamic Binding
 */
#define NVUCL_IMPORT                                                           \
    typedef NVUCL_RESULT (NVUCL_STDCALL *_nvuclGetInterfacePtr)(DWORD,void**); \
    _nvuclGetInterfacePtr nvuclGetInterfaceProc = 0;                           \
    HANDLE                _nvuclModule          = 0;

#define NVUCL_LOAD(x)                                                          \
    _nvuclModule = LoadLibrary(x);                                             \
    if (_nvuclModule)                                                          \
    {                                                                          \
        nvuclGetInterfaceProc = (_nvuclGetInterfacePtr)GetProcAddress(_nvuclModule,"_nvuclGetInterface@8");\
    }

#define NVUCL_UNLOAD                                                           \
    if (_nvuclModule) FreeLibrary(_nvuclModule);                               \
    _nvuclModule = 0;

/*
 *---------------------------------------------------------------------------
 * Structures
 *---------------------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 * Interfaces
 *---------------------------------------------------------------------------
 */

/*
 * Base
 */
#ifdef __cplusplus
#undef  NVUCL_INTERFACE
#define NVUCL_INTERFACE     NVUCL_BASE
struct NVUCL_INTERFACE {
    NVUCL_METHOD_(attach) NVUCL_PURE;
    NVUCL_METHOD_(detach) NVUCL_PURE;
};
#endif //__cplusplus

/*
 * Profiler
 */
#undef  NVUCL_INTERFACE
#define NVUCL_INTERFACE     NVUCL_PROFILER
NVUCL_INTERFACE_BEGIN
    NVUCL_METHOD_(attach)                                   NVUCL_PURE;
    NVUCL_METHOD_(detach)                                   NVUCL_PURE;

    NVUCL_METHOD (registerEvent) LPCSTR cszName,DWORD dwID  NVUCL_PURE;
    NVUCL_METHOD (registerCount) LPCSTR cszName,DWORD dwID  NVUCL_PURE;
    NVUCL_METHOD (registerTime)  LPCSTR cszName,DWORD dwID  NVUCL_PURE;

    NVUCL_METHOD (logEvent)      DWORD  dwID                NVUCL_PURE;
    NVUCL_METHOD (logCount)      DWORD  dwID,int iCount     NVUCL_PURE;
    NVUCL_METHOD (logTime)       DWORD  dwID,__int64 iTime  NVUCL_PURE;

    NVUCL_METHOD (startCapture)  LPCSTR cszFileName         NVUCL_PURE;
    NVUCL_METHOD_(stopCapture)                              NVUCL_PURE;
    NVUCL_METHOD (flush)         BOOL bOptional             NVUCL_PURE;

    NVUCL_METHOD (getCPUTime)    __int64 *pdwTicks          NVUCL_PURE;
    NVUCL_METHOD (getCPUSpeed)   DWORD *pdwSpeed            NVUCL_PURE;
NVUCL_INTERFACE_END

/*
 * Matrix
 */
#undef  NVUCL_INTERFACE
#define NVUCL_INTERFACE     NVUCL_MATRIX
NVUCL_INTERFACE_BEGIN
    NVUCL_METHOD_(attach)                                       NVUCL_PURE;
    NVUCL_METHOD_(detach)                                       NVUCL_PURE;

    NVUCL_METHOD (setStore)     float *pfAddress                NVUCL_PURE;
    NVUCL_METHOD_(setUnit)                                      NVUCL_PURE;

    NVUCL_METHOD (setElement)   DWORD dwIndex,float fValue      NVUCL_PURE;
    NVUCL_METHOD (setRow)       DWORD dwRow,  float *pfValue    NVUCL_PURE;
    NVUCL_METHOD (setAll)       float *pfValue                  NVUCL_PURE;

    NVUCL_METHOD (getElement)   DWORD dwIndex,float *pfValue    NVUCL_PURE;
    NVUCL_METHOD (getRow)       DWORD dwRow,  float *pfValue    NVUCL_PURE;
    NVUCL_METHOD (getAll)       float *pfValue                  NVUCL_PURE;

    NVUCL_METHOD (multiply)     void *pMatrix                   NVUCL_PURE;
    NVUCL_METHOD_(invert)                                       NVUCL_PURE;
NVUCL_INTERFACE_END

/*
 * Push-buffer disassembler
 */
#undef  NVUCL_INTERFACE
#define NVUCL_INTERFACE     NVUCL_DISASM
NVUCL_INTERFACE_BEGIN
    NVUCL_METHOD_(attach)                                       NVUCL_PURE;
    NVUCL_METHOD_(detach)                                       NVUCL_PURE;

    NVUCL_METHOD (redirect)     char* pszFilename               NVUCL_PURE;
    NVUCL_METHOD (traverse)     void* pBuffer,DWORD dwByteCount NVUCL_PURE;
NVUCL_INTERFACE_END

/*
 *---------------------------------------------------------------------------
 * Publics
 *---------------------------------------------------------------------------
 */
#ifdef __cplusplus
extern "C" {
#endif //!__cplusplus
NVUCL_RESULT NVUCL_EXPORT NVUCL_STDCALL nvuclGetInterface (DWORD,void**);
#ifdef __cplusplus
}
#endif //!__cplusplus

#pragma pack(pop)
#endif //_nvucl_h