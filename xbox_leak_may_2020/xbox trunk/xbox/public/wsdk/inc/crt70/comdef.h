/***
* comdef.h - Native C++ compiler COM support - main definitions header
*
*   Copyright (C) 1996-1997 Microsoft Corporation
*   All rights reserved.
*
****/

#if _MSC_VER > 1000
#pragma once
#endif

#if !defined(_INC_COMDEF)
#define _INC_COMDEF

#ifndef  __cplusplus
#error Native Compiler support only available in C++ compiler
#endif

#include <ole2.h>
#include <olectl.h>

#include <comutil.h>

#pragma warning(push)
#pragma warning(disable: 4244)
#pragma warning(disable: 4290)

#pragma comment(lib, "comsupp.lib")

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ole32.lib")
#pragma comment(lib, "oleaut32.lib")

class _com_error;

void __stdcall
    _com_raise_error(HRESULT hr, IErrorInfo* perrinfo = 0) throw(_com_error);

void __stdcall 
	_set_com_error_handler(void (__stdcall *pHandler)(HRESULT hr, IErrorInfo* perrinfo));

void __stdcall
    _com_issue_error(HRESULT) throw(_com_error);
void __stdcall
    _com_issue_errorex(HRESULT, IUnknown*, REFIID) throw(_com_error);

HRESULT __stdcall
    _com_dispatch_propget(IDispatch*, DISPID, VARTYPE, void*) throw(_com_error);
HRESULT __cdecl
    _com_dispatch_propput(IDispatch*, DISPID, VARTYPE, ...) throw(_com_error);
HRESULT __cdecl
    _com_dispatch_method(IDispatch*, DISPID, WORD, VARTYPE, void*,
                         const wchar_t*, ...) throw(_com_error);

HRESULT __stdcall
    _com_dispatch_raw_propget(IDispatch*, DISPID, VARTYPE, void*) throw();
HRESULT __cdecl
    _com_dispatch_raw_propput(IDispatch*, DISPID, VARTYPE, ...) throw();
HRESULT __cdecl
    _com_dispatch_raw_method(IDispatch*, DISPID, WORD, VARTYPE, void*,
                             const wchar_t*, ...) throw();

class _com_error {
public:
    // Constructors
    //
    _com_error(HRESULT hr,
               IErrorInfo* perrinfo = NULL,
               bool fAddRef = false) throw();
    _com_error(const _com_error& that) throw();

    // Destructor
    //
    virtual ~_com_error() throw();

    // Assignment operator
    //
    _com_error& operator=(const _com_error& that) throw();

    // Accessors
    //
    HRESULT Error() const throw();
    WORD WCode() const throw();
    IErrorInfo * ErrorInfo() const throw();

    // IErrorInfo method accessors
    //
    _bstr_t Description() const throw(_com_error);
    DWORD HelpContext() const throw();
    _bstr_t HelpFile() const throw(_com_error);
    _bstr_t Source() const throw(_com_error);
    GUID GUID() const throw();

    // FormatMessage accessors
    const TCHAR * ErrorMessage() const throw();

    // EXCEPINFO.wCode <-> HRESULT mappers
    static HRESULT WCodeToHRESULT(WORD wCode) throw();
    static WORD HRESULTToWCode(HRESULT hr) throw();

private:
    enum {
        WCODE_HRESULT_FIRST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 0x200),
        WCODE_HRESULT_LAST = MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF+1, 0) - 1
    };
    const HRESULT           m_hresult;
    IErrorInfo *            m_perrinfo;
    mutable TCHAR *         m_pszMsg;
};

inline _com_error::_com_error(HRESULT hr,
                              IErrorInfo* perrinfo,
                              bool fAddRef) throw()
    : m_hresult(hr), m_perrinfo(perrinfo), m_pszMsg(NULL)
{
    if (m_perrinfo != NULL && fAddRef) {
        m_perrinfo->AddRef();
    }
}

inline _com_error::_com_error(const _com_error& that) throw()
    : m_hresult(that.m_hresult), m_perrinfo(that.m_perrinfo), m_pszMsg(NULL)
{
    if (m_perrinfo != NULL) {
        m_perrinfo->AddRef();
    }
}

inline _com_error::~_com_error() throw()
{
    if (m_perrinfo != NULL) {
        m_perrinfo->Release();
    }
    if (m_pszMsg != NULL) {
        LocalFree((HLOCAL)m_pszMsg);
    }
}

inline _com_error& _com_error::operator=(const _com_error& that) throw()
{
    if (this != &that) {
        this->_com_error::~_com_error();
        this->_com_error::_com_error(that);
    }
    return *this;
}

inline HRESULT _com_error::Error() const throw()
{
    return m_hresult;
}

inline WORD _com_error::WCode() const throw()
{
    return HRESULTToWCode(m_hresult);
}

inline IErrorInfo * _com_error::ErrorInfo() const throw()
{
    if (m_perrinfo != NULL) {
        m_perrinfo->AddRef();
    }
    return m_perrinfo;
}

inline _bstr_t _com_error::Description() const throw(_com_error)
{
    BSTR bstr = NULL;
    if (m_perrinfo != NULL) {
        m_perrinfo->GetDescription(&bstr);
    }
    return _bstr_t(bstr, false);
}

inline DWORD _com_error::HelpContext() const throw()
{
    DWORD dwHelpContext = 0;
    if (m_perrinfo != NULL) {
        m_perrinfo->GetHelpContext(&dwHelpContext);
    }
    return dwHelpContext;
}

inline _bstr_t _com_error::HelpFile() const throw(_com_error)
{
    BSTR bstr = NULL;
    if (m_perrinfo != NULL) {
        m_perrinfo->GetHelpFile(&bstr);
    }
    return _bstr_t(bstr, false);
}

inline _bstr_t _com_error::Source() const throw(_com_error)
{
    BSTR bstr = NULL;
    if (m_perrinfo != NULL) {
        m_perrinfo->GetSource(&bstr);
    }
    return _bstr_t(bstr, false);
}

inline _GUID _com_error::GUID() const throw()
{
    _GUID guid;
    memcpy(&guid, &__uuidof(NULL), sizeof(_GUID));
    if (m_perrinfo != NULL) {
        m_perrinfo->GetGUID(&guid);
    }
    return guid;
}

inline const TCHAR * _com_error::ErrorMessage() const throw()
{
    if (m_pszMsg == NULL) {
        FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM,
                      NULL,
                      m_hresult,
                      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                      (LPTSTR)&m_pszMsg,
                      0,
                      NULL );
        if (m_pszMsg != NULL) {
            int nLen = lstrlen(m_pszMsg);
            if (nLen > 1 && m_pszMsg[nLen - 1] == '\n') {
                m_pszMsg[nLen - 1] = 0;
                if (m_pszMsg[nLen - 2] == '\r') {
                        m_pszMsg[nLen - 2] = 0;
                }
            }
        } else {
            m_pszMsg = (LPTSTR)LocalAlloc(0, 32 * sizeof(TCHAR));
            if (m_pszMsg != NULL) {
                WORD wCode = WCode();
                if (wCode != 0) {
                    wsprintf(m_pszMsg, TEXT("IDispatch error #%d"), wCode);
                } else {
                    wsprintf(m_pszMsg, TEXT("Unknown error 0x%0lX"), m_hresult);
                }
            }
        }
    }
    return m_pszMsg;
}

inline HRESULT _com_error::WCodeToHRESULT(WORD wCode) throw()
{
    return wCode >= 0xFE00 ? WCODE_HRESULT_LAST : WCODE_HRESULT_FIRST + wCode;
}

inline WORD _com_error::HRESULTToWCode(HRESULT hr) throw()
{
    return (hr >= WCODE_HRESULT_FIRST && hr <= WCODE_HRESULT_LAST)
        ? WORD(hr - WCODE_HRESULT_FIRST)
        : 0;
}

//
// give missing types from dependent type libraries a chance
//
typedef int __missing_type__;

#if !defined(_COM_SMARTPTR)
 #if !defined(_INC_COMIP)
  #include <comip.h>
 #endif
 #define _COM_SMARTPTR        _com_ptr_t
 #define _COM_SMARTPTR_LEVEL2 _com_IIID
#endif
#if defined(_COM_SMARTPTR)
 #if !defined(_COM_SMARTPTR_TYPEDEF)
  #if defined(_COM_SMARTPTR_LEVEL2)
   #define _COM_SMARTPTR_TYPEDEF(Interface, IID) \
    typedef _COM_SMARTPTR<_COM_SMARTPTR_LEVEL2<Interface, &IID> > \
            Interface ## Ptr
  #else
   #define _COM_SMARTPTR_TYPEDEF(Interface, IID) \
    typedef _COM_SMARTPTR<Interface, &IID> \
            Interface ## Ptr
  #endif
 #endif
#endif

#if !defined(_COM_NO_STANDARD_GUIDS_)

// hard-coded smart pointer defs
#if defined(__IFontDisp_INTERFACE_DEFINED__)
__if_not_exists(Font)
{
	struct Font : IFontDisp {};
}
_COM_SMARTPTR_TYPEDEF(Font, __uuidof(IDispatch));
#endif
#if defined(__IFontEventsDisp_INTERFACE_DEFINED__)
__if_not_exists(FontEvents)
{
	struct FontEvents : IFontEventsDisp {};
}
_COM_SMARTPTR_TYPEDEF(FontEvents, __uuidof(IDispatch));
#endif
#if defined(__IPictureDisp_INTERFACE_DEFINED__)
__if_not_exists(Picture)
{
	struct Picture : IPictureDisp {};
}
_COM_SMARTPTR_TYPEDEF(Picture, __uuidof(IDispatch));
#endif

// interface smart pointer defs
#if defined(__IAdviseSink_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAdviseSink, __uuidof(IAdviseSink));
#endif// #if defined(__IAdviseSink_INTERFACE_DEFINED__)
#if defined(__IAdviseSink2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAdviseSink2, __uuidof(IAdviseSink2));
#endif// #if defined(__IAdviseSink2_INTERFACE_DEFINED__)
#if defined(__IAdviseSinkEx_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAdviseSinkEx, __uuidof(IAdviseSinkEx));
#endif// #if defined(__IAdviseSinkEx_INTERFACE_DEFINED__)
#if defined(__IAsyncManager_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAsyncManager, __uuidof(IAsyncManager));
#endif// #if defined(__IAsyncManager_INTERFACE_DEFINED__)
#if defined(__IAsyncSetup_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAsyncSetup, __uuidof(IAsyncSetup));
#endif// #if defined(__IAsyncSetup_INTERFACE_DEFINED__)
#if defined(__IAuthenticate_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IAuthenticate, __uuidof(IAuthenticate));
#endif// #if defined(__IAuthenticate_INTERFACE_DEFINED__)
#if defined(__IBindCtx_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IBindCtx, __uuidof(IBindCtx));
#endif// #if defined(__IBindCtx_INTERFACE_DEFINED__)
#if defined(__IBindHost_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IBindHost, __uuidof(IBindHost));
#endif// #if defined(__IBindHost_INTERFACE_DEFINED__)
#if defined(__IBindProtocol_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IBindProtocol, __uuidof(IBindProtocol));
#endif// #if defined(__IBindProtocol_INTERFACE_DEFINED__)
#if defined(__IBindStatusCallback_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IBindStatusCallback, __uuidof(IBindStatusCallback));
#endif// #if defined(__IBindStatusCallback_INTERFACE_DEFINED__)
#if defined(__IBinding_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IBinding, __uuidof(IBinding));
#endif// #if defined(__IBinding_INTERFACE_DEFINED__)
#if defined(__ICancelMethodCalls_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICancelMethodCalls, __uuidof(ICancelMethodCalls));
#endif// #if defined(__ICancelMethodCalls_INTERFACE_DEFINED__)
#if defined(__IChannelHook_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IChannelHook, __uuidof(IChannelHook));
#endif// #if defined(__IChannelHook_INTERFACE_DEFINED__)
#if defined(__IClassAccess_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassAccess, __uuidof(IClassAccess));
#endif// #if defined(__IClassAccess_INTERFACE_DEFINED__)
#if defined(__IClassActivator_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassActivator, __uuidof(IClassActivator));
#endif// #if defined(__IClassActivator_INTERFACE_DEFINED__)
#if defined(__IClassAdmin_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassAdmin, __uuidof(IClassAdmin));
#endif// #if defined(__IClassAdmin_INTERFACE_DEFINED__)
#if defined(__IClassFactory_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassFactory, __uuidof(IClassFactory));
#endif// #if defined(__IClassFactory_INTERFACE_DEFINED__)
#if defined(__IClassFactory2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassFactory2, __uuidof(IClassFactory2));
#endif// #if defined(__IClassFactory2_INTERFACE_DEFINED__)
#if defined(__IClassRefresh_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClassRefresh, __uuidof(IClassRefresh));
#endif// #if defined(__IClassRefresh_INTERFACE_DEFINED__)
#if defined(__IClientSecurity_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IClientSecurity, __uuidof(IClientSecurity));
#endif// #if defined(__IClientSecurity_INTERFACE_DEFINED__)
#if defined(__ICodeInstall_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICodeInstall, __uuidof(ICodeInstall));
#endif// #if defined(__ICodeInstall_INTERFACE_DEFINED__)
#if defined(__IComBinding_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IComBinding, __uuidof(IComBinding));
#endif// #if defined(__IComBinding_INTERFACE_DEFINED__)
#if defined(__IConnectionPoint_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IConnectionPoint, __uuidof(IConnectionPoint));
#endif// #if defined(__IConnectionPoint_INTERFACE_DEFINED__)
#if defined(__IConnectionPointContainer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IConnectionPointContainer, __uuidof(IConnectionPointContainer));
#endif// #if defined(__IConnectionPointContainer_INTERFACE_DEFINED__)
#if defined(__IContinue_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IContinue, __uuidof(IContinue));
#endif// #if defined(__IContinue_INTERFACE_DEFINED__)
#if defined(__ICreateErrorInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICreateErrorInfo, __uuidof(ICreateErrorInfo));
#endif// #if defined(__ICreateErrorInfo_INTERFACE_DEFINED__)
#if defined(__ICreateTypeInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICreateTypeInfo, __uuidof(ICreateTypeInfo));
#endif// #if defined(__ICreateTypeInfo_INTERFACE_DEFINED__)
#if defined(__ICreateTypeInfo2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICreateTypeInfo2, __uuidof(ICreateTypeInfo2));
#endif// #if defined(__ICreateTypeInfo2_INTERFACE_DEFINED__)
#if defined(__ICreateTypeLib_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICreateTypeLib, __uuidof(ICreateTypeLib));
#endif// #if defined(__ICreateTypeLib_INTERFACE_DEFINED__)
#if defined(__ICreateTypeLib2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ICreateTypeLib2, __uuidof(ICreateTypeLib2));
#endif// #if defined(__ICreateTypeLib2_INTERFACE_DEFINED__)
#if defined(__IDataAdviseHolder_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDataAdviseHolder, __uuidof(IDataAdviseHolder));
#endif// #if defined(__IDataAdviseHolder_INTERFACE_DEFINED__)
#if defined(__IDataFilter_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDataFilter, __uuidof(IDataFilter));
#endif// #if defined(__IDataFilter_INTERFACE_DEFINED__)
#if defined(__IDataObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDataObject, __uuidof(IDataObject));
#endif// #if defined(__IDataObject_INTERFACE_DEFINED__)
#if defined(__IDirectWriterLock_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDirectWriterLock, __uuidof(IDirectWriterLock));
#endif// #if defined(__IDirectWriterLock_INTERFACE_DEFINED__)
#if defined(__IDispatch_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDispatch, __uuidof(IDispatch));
#endif// #if defined(__IDispatch_INTERFACE_DEFINED__)
#if defined(__IDropSource_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDropSource, __uuidof(IDropSource));
#endif// #if defined(__IDropSource_INTERFACE_DEFINED__)
#if defined(__IDropTarget_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IDropTarget, __uuidof(IDropTarget));
#endif// #if defined(__IDropTarget_INTERFACE_DEFINED__)
#if defined(__IEncodingFilterFactory_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEncodingFilterFactory, __uuidof(IEncodingFilterFactory));
#endif// #if defined(__IEncodingFilterFactory_INTERFACE_DEFINED__)
#if defined(__IEnumClass_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumClass, __uuidof(IEnumClass));
#endif// #if defined(__IEnumClass_INTERFACE_DEFINED__)
#if defined(__IEnumConnectionPoints_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumConnectionPoints, __uuidof(IEnumConnectionPoints));
#endif// #if defined(__IEnumConnectionPoints_INTERFACE_DEFINED__)
#if defined(__IEnumConnections_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumConnections, __uuidof(IEnumConnections));
#endif// #if defined(__IEnumConnections_INTERFACE_DEFINED__)
#if defined(__IEnumFORMATETC_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumFORMATETC, __uuidof(IEnumFORMATETC));
#endif// #if defined(__IEnumFORMATETC_INTERFACE_DEFINED__)
#if defined(__IEnumMoniker_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumMoniker, __uuidof(IEnumMoniker));
#endif// #if defined(__IEnumMoniker_INTERFACE_DEFINED__)
#if defined(__IEnumOLEVERB_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumOLEVERB, __uuidof(IEnumOLEVERB));
#endif// #if defined(__IEnumOLEVERB_INTERFACE_DEFINED__)
#if defined(__IEnumOleUndoUnits_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumOleUndoUnits, __uuidof(IEnumOleUndoUnits));
#endif// #if defined(__IEnumOleUndoUnits_INTERFACE_DEFINED__)
#if defined(__IEnumPackage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumPackage, __uuidof(IEnumPackage));
#endif// #if defined(__IEnumPackage_INTERFACE_DEFINED__)
#if defined(__IEnumSTATDATA_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumSTATDATA, __uuidof(IEnumSTATDATA));
#endif// #if defined(__IEnumSTATDATA_INTERFACE_DEFINED__)
#if defined(__IEnumSTATPROPSETSTG_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumSTATPROPSETSTG, __uuidof(IEnumSTATPROPSETSTG));
#endif// #if defined(__IEnumSTATPROPSETSTG_INTERFACE_DEFINED__)
#if defined(__IEnumSTATPROPSTG_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumSTATPROPSTG, __uuidof(IEnumSTATPROPSTG));
#endif// #if defined(__IEnumSTATPROPSTG_INTERFACE_DEFINED__)
#if defined(__IEnumSTATSTG_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumSTATSTG, __uuidof(IEnumSTATSTG));
#endif// #if defined(__IEnumSTATSTG_INTERFACE_DEFINED__)
#if defined(__IEnumString_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumString, __uuidof(IEnumString));
#endif// #if defined(__IEnumString_INTERFACE_DEFINED__)
#if defined(__IEnumUnknown_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumUnknown, __uuidof(IEnumUnknown));
#endif// #if defined(__IEnumUnknown_INTERFACE_DEFINED__)
#if defined(__IEnumVARIANT_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IEnumVARIANT, __uuidof(IEnumVARIANT));
#endif// #if defined(__IEnumVARIANT_INTERFACE_DEFINED__)
#if defined(__IErrorInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IErrorInfo, __uuidof(IErrorInfo));
#endif// #if defined(__IErrorInfo_INTERFACE_DEFINED__)
#if defined(__IErrorLog_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IErrorLog, __uuidof(IErrorLog));
#endif// #if defined(__IErrorLog_INTERFACE_DEFINED__)
#if defined(__IExternalConnection_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IExternalConnection, __uuidof(IExternalConnection));
#endif// #if defined(__IExternalConnection_INTERFACE_DEFINED__)
#if defined(__IFillLockBytes_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IFillLockBytes, __uuidof(IFillLockBytes));
#endif// #if defined(__IFillLockBytes_INTERFACE_DEFINED__)
#if defined(__IFont_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IFont, __uuidof(IFont));
#endif// #if defined(__IFont_INTERFACE_DEFINED__)
#if defined(__IFontDisp_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IFontDisp, __uuidof(IFontDisp));
#endif// #if defined(__IFontDisp_INTERFACE_DEFINED__)
#if defined(__IFontEventsDisp_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IFontEventsDisp, __uuidof(IFontEventsDisp));
#endif// #if defined(__IFontEventsDisp_INTERFACE_DEFINED__)
#if defined(__IGlobalInterfaceTable_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IGlobalInterfaceTable, __uuidof(IGlobalInterfaceTable));
#endif// #if defined(__IGlobalInterfaceTable_INTERFACE_DEFINED__)
#if defined(__IHttpNegotiate_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IHttpNegotiate, __uuidof(IHttpNegotiate));
#endif// #if defined(__IHttpNegotiate_INTERFACE_DEFINED__)
#if defined(__IHttpSecurity_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IHttpSecurity, __uuidof(IHttpSecurity));
#endif// #if defined(__IHttpSecurity_INTERFACE_DEFINED__)
#if defined(__IInternet_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternet, __uuidof(IInternet));
#endif// #if defined(__IInternet_INTERFACE_DEFINED__)
#if defined(__IInternetBindInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetBindInfo, __uuidof(IInternetBindInfo));
#endif// #if defined(__IInternetBindInfo_INTERFACE_DEFINED__)
#if defined(__IInternetHostSecurityManager_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetHostSecurityManager, __uuidof(IInternetHostSecurityManager));
#endif// #if defined(__IInternetHostSecurityManager_INTERFACE_DEFINED__)
#if defined(__IInternetPriority_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetPriority, __uuidof(IInternetPriority));
#endif// #if defined(__IInternetPriority_INTERFACE_DEFINED__)
#if defined(__IInternetProtocol_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetProtocol, __uuidof(IInternetProtocol));
#endif// #if defined(__IInternetProtocol_INTERFACE_DEFINED__)
#if defined(__IInternetProtocolInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetProtocolInfo, __uuidof(IInternetProtocolInfo));
#endif// #if defined(__IInternetProtocolInfo_INTERFACE_DEFINED__)
#if defined(__IInternetProtocolRoot_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetProtocolRoot, __uuidof(IInternetProtocolRoot));
#endif// #if defined(__IInternetProtocolRoot_INTERFACE_DEFINED__)
#if defined(__IInternetProtocolSink_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetProtocolSink, __uuidof(IInternetProtocolSink));
#endif// #if defined(__IInternetProtocolSink_INTERFACE_DEFINED__)
#if defined(__IInternetSecurityManager_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetSecurityManager, __uuidof(IInternetSecurityManager));
#endif// #if defined(__IInternetSecurityManager_INTERFACE_DEFINED__)
#if defined(__IInternetSecurityMgrSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetSecurityMgrSite, __uuidof(IInternetSecurityMgrSite));
#endif// #if defined(__IInternetSecurityMgrSite_INTERFACE_DEFINED__)
#if defined(__IInternetSession_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetSession, __uuidof(IInternetSession));
#endif// #if defined(__IInternetSession_INTERFACE_DEFINED__)
#if defined(__IInternetThreadSwitch_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetThreadSwitch, __uuidof(IInternetThreadSwitch));
#endif// #if defined(__IInternetThreadSwitch_INTERFACE_DEFINED__)
#if defined(__IInternetZoneManager_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IInternetZoneManager, __uuidof(IInternetZoneManager));
#endif// #if defined(__IInternetZoneManager_INTERFACE_DEFINED__)
#if defined(__ILayoutStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ILayoutStorage, __uuidof(ILayoutStorage));
#endif// #if defined(__ILayoutStorage_INTERFACE_DEFINED__)
#if defined(__ILockBytes_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ILockBytes, __uuidof(ILockBytes));
#endif// #if defined(__ILockBytes_INTERFACE_DEFINED__)
#if defined(__IMalloc_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMalloc, __uuidof(IMalloc));
#endif// #if defined(__IMalloc_INTERFACE_DEFINED__)
#if defined(__IMallocSpy_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMallocSpy, __uuidof(IMallocSpy));
#endif// #if defined(__IMallocSpy_INTERFACE_DEFINED__)
#if defined(__IMarshal_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMarshal, __uuidof(IMarshal));
#endif// #if defined(__IMarshal_INTERFACE_DEFINED__)
#if defined(__IMessageFilter_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMessageFilter, __uuidof(IMessageFilter));
#endif// #if defined(__IMessageFilter_INTERFACE_DEFINED__)
#if defined(__IMoniker_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMoniker, __uuidof(IMoniker));
#endif// #if defined(__IMoniker_INTERFACE_DEFINED__)
#if defined(__IMultiQI_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IMultiQI, __uuidof(IMultiQI));
#endif// #if defined(__IMultiQI_INTERFACE_DEFINED__)
#if defined(__IObjectWithSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IObjectWithSite, __uuidof(IObjectWithSite));
#endif// #if defined(__IObjectWithSite_INTERFACE_DEFINED__)
#if defined(__IOleAdviseHolder_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleAdviseHolder, __uuidof(IOleAdviseHolder));
#endif// #if defined(__IOleAdviseHolder_INTERFACE_DEFINED__)
#if defined(__IOleCache_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleCache, __uuidof(IOleCache));
#endif// #if defined(__IOleCache_INTERFACE_DEFINED__)
#if defined(__IOleCache2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleCache2, __uuidof(IOleCache2));
#endif// #if defined(__IOleCache2_INTERFACE_DEFINED__)
#if defined(__IOleCacheControl_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleCacheControl, __uuidof(IOleCacheControl));
#endif// #if defined(__IOleCacheControl_INTERFACE_DEFINED__)
#if defined(__IOleClientSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleClientSite, __uuidof(IOleClientSite));
#endif// #if defined(__IOleClientSite_INTERFACE_DEFINED__)
#if defined(__IOleContainer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleContainer, __uuidof(IOleContainer));
#endif// #if defined(__IOleContainer_INTERFACE_DEFINED__)
#if defined(__IOleControl_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleControl, __uuidof(IOleControl));
#endif// #if defined(__IOleControl_INTERFACE_DEFINED__)
#if defined(__IOleControlSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleControlSite, __uuidof(IOleControlSite));
#endif// #if defined(__IOleControlSite_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceActiveObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceActiveObject, __uuidof(IOleInPlaceActiveObject));
#endif// #if defined(__IOleInPlaceActiveObject_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceFrame_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceFrame, __uuidof(IOleInPlaceFrame));
#endif// #if defined(__IOleInPlaceFrame_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceObject, __uuidof(IOleInPlaceObject));
#endif// #if defined(__IOleInPlaceObject_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceObjectWindowless_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceObjectWindowless, __uuidof(IOleInPlaceObjectWindowless));
#endif// #if defined(__IOleInPlaceObjectWindowless_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSite, __uuidof(IOleInPlaceSite));
#endif// #if defined(__IOleInPlaceSite_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceSiteEx_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSiteEx, __uuidof(IOleInPlaceSiteEx));
#endif// #if defined(__IOleInPlaceSiteEx_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceSiteWindowless_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceSiteWindowless, __uuidof(IOleInPlaceSiteWindowless));
#endif// #if defined(__IOleInPlaceSiteWindowless_INTERFACE_DEFINED__)
#if defined(__IOleInPlaceUIWindow_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleInPlaceUIWindow, __uuidof(IOleInPlaceUIWindow));
#endif// #if defined(__IOleInPlaceUIWindow_INTERFACE_DEFINED__)
#if defined(__IOleItemContainer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleItemContainer, __uuidof(IOleItemContainer));
#endif// #if defined(__IOleItemContainer_INTERFACE_DEFINED__)
#if defined(__IOleLink_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleLink, __uuidof(IOleLink));
#endif// #if defined(__IOleLink_INTERFACE_DEFINED__)
#if defined(__IOleObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleObject, __uuidof(IOleObject));
#endif// #if defined(__IOleObject_INTERFACE_DEFINED__)
#if defined(__IOleParentUndoUnit_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleParentUndoUnit, __uuidof(IOleParentUndoUnit));
#endif// #if defined(__IOleParentUndoUnit_INTERFACE_DEFINED__)
#if defined(__IOleUndoManager_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleUndoManager, __uuidof(IOleUndoManager));
#endif// #if defined(__IOleUndoManager_INTERFACE_DEFINED__)
#if defined(__IOleUndoUnit_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleUndoUnit, __uuidof(IOleUndoUnit));
#endif// #if defined(__IOleUndoUnit_INTERFACE_DEFINED__)
#if defined(__IOleWindow_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IOleWindow, __uuidof(IOleWindow));
#endif// #if defined(__IOleWindow_INTERFACE_DEFINED__)
#if defined(__IPSFactoryBuffer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPSFactoryBuffer, __uuidof(IPSFactoryBuffer));
#endif// #if defined(__IPSFactoryBuffer_INTERFACE_DEFINED__)
#if defined(__IParseDisplayName_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IParseDisplayName, __uuidof(IParseDisplayName));
#endif// #if defined(__IParseDisplayName_INTERFACE_DEFINED__)
#if defined(__IPerPropertyBrowsing_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPerPropertyBrowsing, __uuidof(IPerPropertyBrowsing));
#endif// #if defined(__IPerPropertyBrowsing_INTERFACE_DEFINED__)
#if defined(__IPersist_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersist, __uuidof(IPersist));
#endif// #if defined(__IPersist_INTERFACE_DEFINED__)
#if defined(__IPersistFile_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistFile, __uuidof(IPersistFile));
#endif// #if defined(__IPersistFile_INTERFACE_DEFINED__)
#if defined(__IPersistMemory_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistMemory, __uuidof(IPersistMemory));
#endif// #if defined(__IPersistMemory_INTERFACE_DEFINED__)
#if defined(__IPersistMoniker_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistMoniker, __uuidof(IPersistMoniker));
#endif// #if defined(__IPersistMoniker_INTERFACE_DEFINED__)
#if defined(__IPersistPropertyBag_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistPropertyBag, __uuidof(IPersistPropertyBag));
#endif// #if defined(__IPersistPropertyBag_INTERFACE_DEFINED__)
#if defined(__IPersistPropertyBag2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistPropertyBag2, __uuidof(IPersistPropertyBag2));
#endif// #if defined(__IPersistPropertyBag2_INTERFACE_DEFINED__)
#if defined(__IPersistStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistStorage, __uuidof(IPersistStorage));
#endif// #if defined(__IPersistStorage_INTERFACE_DEFINED__)
#if defined(__IPersistStream_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistStream, __uuidof(IPersistStream));
#endif// #if defined(__IPersistStream_INTERFACE_DEFINED__)
#if defined(__IPersistStreamInit_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPersistStreamInit, __uuidof(IPersistStreamInit));
#endif// #if defined(__IPersistStreamInit_INTERFACE_DEFINED__)
#if defined(__IPicture_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPicture, __uuidof(IPicture));
#endif// #if defined(__IPicture_INTERFACE_DEFINED__)
#if defined(__IPictureDisp_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPictureDisp, __uuidof(IPictureDisp));
#endif// #if defined(__IPictureDisp_INTERFACE_DEFINED__)
#if defined(__IPointerInactive_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPointerInactive, __uuidof(IPointerInactive));
#endif// #if defined(__IPointerInactive_INTERFACE_DEFINED__)
#if defined(__IProgressNotify_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IProgressNotify, __uuidof(IProgressNotify));
#endif// #if defined(__IProgressNotify_INTERFACE_DEFINED__)
#if defined(__IPropertyBag_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyBag, __uuidof(IPropertyBag));
#endif// #if defined(__IPropertyBag_INTERFACE_DEFINED__)
#if defined(__IPropertyBag2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyBag2, __uuidof(IPropertyBag2));
#endif// #if defined(__IPropertyBag2_INTERFACE_DEFINED__)
#if defined(__IPropertyNotifySink_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyNotifySink, __uuidof(IPropertyNotifySink));
#endif// #if defined(__IPropertyNotifySink_INTERFACE_DEFINED__)
#if defined(__IPropertyPage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyPage, __uuidof(IPropertyPage));
#endif// #if defined(__IPropertyPage_INTERFACE_DEFINED__)
#if defined(__IPropertyPage2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyPage2, __uuidof(IPropertyPage2));
#endif// #if defined(__IPropertyPage2_INTERFACE_DEFINED__)
#if defined(__IPropertyPageSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyPageSite, __uuidof(IPropertyPageSite));
#endif// #if defined(__IPropertyPageSite_INTERFACE_DEFINED__)
#if defined(__IPropertySetStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertySetStorage, __uuidof(IPropertySetStorage));
#endif// #if defined(__IPropertySetStorage_INTERFACE_DEFINED__)
#if defined(__IPropertyStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IPropertyStorage, __uuidof(IPropertyStorage));
#endif// #if defined(__IPropertyStorage_INTERFACE_DEFINED__)
#if defined(__IProvideClassInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IProvideClassInfo, __uuidof(IProvideClassInfo));
#endif// #if defined(__IProvideClassInfo_INTERFACE_DEFINED__)
#if defined(__IProvideClassInfo2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IProvideClassInfo2, __uuidof(IProvideClassInfo2));
#endif// #if defined(__IProvideClassInfo2_INTERFACE_DEFINED__)
#if defined(__IProvideMultipleClassInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IProvideMultipleClassInfo, __uuidof(IProvideMultipleClassInfo));
#endif// #if defined(__IProvideMultipleClassInfo_INTERFACE_DEFINED__)
#if defined(__IQuickActivate_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IQuickActivate, __uuidof(IQuickActivate));
#endif// #if defined(__IQuickActivate_INTERFACE_DEFINED__)
#if defined(__IROTData_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IROTData, __uuidof(IROTData));
#endif// #if defined(__IROTData_INTERFACE_DEFINED__)
#if defined(__IRecordInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRecordInfo, __uuidof(IRecordInfo));
#endif// #if defined(__IRecordInfo_INTERFACE_DEFINED__)
#if defined(__IRootStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRootStorage, __uuidof(IRootStorage));
#endif// #if defined(__IRootStorage_INTERFACE_DEFINED__)
#if defined(__IRpcChannelBuffer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer, __uuidof(IRpcChannelBuffer));
#endif// #if defined(__IRpcChannelBuffer_INTERFACE_DEFINED__)
#if defined(__IRpcChannelBuffer2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer2, __uuidof(IRpcChannelBuffer2));
#endif// #if defined(__IRpcChannelBuffer2_INTERFACE_DEFINED__)
#if defined(__IRpcChannelBuffer3_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcChannelBuffer3, __uuidof(IRpcChannelBuffer3));
#endif// #if defined(__IRpcChannelBuffer3_INTERFACE_DEFINED__)
#if defined(__IRpcOptions_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcOptions, __uuidof(IRpcOptions));
#endif// #if defined(__IRpcOptions_INTERFACE_DEFINED__)
#if defined(__IRpcProxyBuffer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcProxyBuffer, __uuidof(IRpcProxyBuffer));
#endif// #if defined(__IRpcProxyBuffer_INTERFACE_DEFINED__)
#if defined(__IRpcStubBuffer_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRpcStubBuffer, __uuidof(IRpcStubBuffer));
#endif// #if defined(__IRpcStubBuffer_INTERFACE_DEFINED__)
#if defined(__IRunnableObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRunnableObject, __uuidof(IRunnableObject));
#endif// #if defined(__IRunnableObject_INTERFACE_DEFINED__)
#if defined(__IRunningObjectTable_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IRunningObjectTable, __uuidof(IRunningObjectTable));
#endif// #if defined(__IRunningObjectTable_INTERFACE_DEFINED__)
#if defined(__ISequentialStream_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISequentialStream, __uuidof(ISequentialStream));
#endif// #if defined(__ISequentialStream_INTERFACE_DEFINED__)
#if defined(__IServerSecurity_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IServerSecurity, __uuidof(IServerSecurity));
#endif// #if defined(__IServerSecurity_INTERFACE_DEFINED__)
#if defined(__IServiceProvider_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IServiceProvider, __uuidof(IServiceProvider));
#endif// #if defined(__IServiceProvider_INTERFACE_DEFINED__)
#if defined(__ISimpleFrameSite_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISimpleFrameSite, __uuidof(ISimpleFrameSite));
#endif// #if defined(__ISimpleFrameSite_INTERFACE_DEFINED__)
#if defined(__ISoftDistExt_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISoftDistExt, __uuidof(ISoftDistExt));
#endif// #if defined(__ISoftDistExt_INTERFACE_DEFINED__)
#if defined(__ISpecifyPropertyPages_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISpecifyPropertyPages, __uuidof(ISpecifyPropertyPages));
#endif// #if defined(__ISpecifyPropertyPages_INTERFACE_DEFINED__)
#if defined(__IStdMarshalInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IStdMarshalInfo, __uuidof(IStdMarshalInfo));
#endif// #if defined(__IStdMarshalInfo_INTERFACE_DEFINED__)
#if defined(__IStorage_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IStorage, __uuidof(IStorage));
#endif// #if defined(__IStorage_INTERFACE_DEFINED__)
#if defined(__IStream_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IStream, __uuidof(IStream));
#endif// #if defined(__IStream_INTERFACE_DEFINED__)
#if defined(__ISupportErrorInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISupportErrorInfo, __uuidof(ISupportErrorInfo));
#endif// #if defined(__ISupportErrorInfo_INTERFACE_DEFINED__)
#if defined(__ISurrogate_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISurrogate, __uuidof(ISurrogate));
#endif// #if defined(__ISurrogate_INTERFACE_DEFINED__)
#if defined(__ISynchronize_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISynchronize, __uuidof(ISynchronize));
#endif// #if defined(__ISynchronize_INTERFACE_DEFINED__)
#if defined(__ISynchronizeEvent_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISynchronizeEvent, __uuidof(ISynchronizeEvent));
#endif// #if defined(__ISynchronizeEvent_INTERFACE_DEFINED__)
#if defined(__ISynchronizeMutex_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ISynchronizeMutex, __uuidof(ISynchronizeMutex));
#endif// #if defined(__ISynchronizeMutex_INTERFACE_DEFINED__)
#if defined(__ITypeChangeEvents_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeChangeEvents, __uuidof(ITypeChangeEvents));
#endif// #if defined(__ITypeChangeEvents_INTERFACE_DEFINED__)
#if defined(__ITypeComp_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeComp, __uuidof(ITypeComp));
#endif// #if defined(__ITypeComp_INTERFACE_DEFINED__)
#if defined(__ITypeFactory_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeFactory, __uuidof(ITypeFactory));
#endif// #if defined(__ITypeFactory_INTERFACE_DEFINED__)
#if defined(__ITypeInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeInfo, __uuidof(ITypeInfo));
#endif// #if defined(__ITypeInfo_INTERFACE_DEFINED__)
#if defined(__ITypeInfo2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeInfo2, __uuidof(ITypeInfo2));
#endif// #if defined(__ITypeInfo2_INTERFACE_DEFINED__)
#if defined(__ITypeLib_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeLib, __uuidof(ITypeLib));
#endif// #if defined(__ITypeLib_INTERFACE_DEFINED__)
#if defined(__ITypeLib2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeLib2, __uuidof(ITypeLib2));
#endif// #if defined(__ITypeLib2_INTERFACE_DEFINED__)
#if defined(__ITypeMarshal_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(ITypeMarshal, __uuidof(ITypeMarshal));
#endif// #if defined(__ITypeMarshal_INTERFACE_DEFINED__)
#if defined(__IUnknown_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IUnknown, __uuidof(IUnknown));
#endif// #if defined(__IUnknown_INTERFACE_DEFINED__)
#if defined(__IUrlMon_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IUrlMon, __uuidof(IUrlMon));
#endif// #if defined(__IUrlMon_INTERFACE_DEFINED__)
#if defined(__IViewObject_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IViewObject, __uuidof(IViewObject));
#endif// #if defined(__IViewObject_INTERFACE_DEFINED__)
#if defined(__IViewObject2_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IViewObject2, __uuidof(IViewObject2));
#endif// #if defined(__IViewObject2_INTERFACE_DEFINED__)
#if defined(__IViewObjectEx_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IViewObjectEx, __uuidof(IViewObjectEx));
#endif// #if defined(__IViewObjectEx_INTERFACE_DEFINED__)
#if defined(__IWaitMultiple_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IWaitMultiple, __uuidof(IWaitMultiple));
#endif// #if defined(__IWaitMultiple_INTERFACE_DEFINED__)
#if defined(__IWinInetHttpInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IWinInetHttpInfo, __uuidof(IWinInetHttpInfo));
#endif// #if defined(__IWinInetHttpInfo_INTERFACE_DEFINED__)
#if defined(__IWinInetInfo_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IWinInetInfo, __uuidof(IWinInetInfo));
#endif// #if defined(__IWinInetInfo_INTERFACE_DEFINED__)
#if defined(__IWindowForBindingUI_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IWindowForBindingUI, __uuidof(IWindowForBindingUI));
#endif// #if defined(__IWindowForBindingUI_INTERFACE_DEFINED__)
#if defined(__IXMLDocument_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IXMLDocument, __uuidof(IXMLDocument));
#endif// #if defined(__IXMLDocument_INTERFACE_DEFINED__)
#if defined(__IXMLElement_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IXMLElement, __uuidof(IXMLElement));
#endif// #if defined(__IXMLElement_INTERFACE_DEFINED__)
#if defined(__IXMLElementCollection_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IXMLElementCollection, __uuidof(IXMLElementCollection));
#endif// #if defined(__IXMLElementCollection_INTERFACE_DEFINED__)
#if defined(__IXMLElementNotificationSink_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IXMLElementNotificationSink, __uuidof(IXMLElementNotificationSink));
#endif// #if defined(__IXMLElementNotificationSink_INTERFACE_DEFINED__)
#if defined(__IXMLError_INTERFACE_DEFINED__)
_COM_SMARTPTR_TYPEDEF(IXMLError, __uuidof(IXMLError));
#endif// #if defined(__IXMLError_INTERFACE_DEFINED__)

// coclass smart pointer defs
#if defined(__XMLDocument_FWD_DEFINED__)
_COM_SMARTPTR_TYPEDEF(XMLDocument, __uuidof(XMLDocument));
#endif// #if defined(__XMLDocument_FWD_DEFINED__)

#endif  /* _COM_NO_STANDARD_GUIDS_ */

#pragma warning(pop)

#endif  /* _INC_COMDEF */

