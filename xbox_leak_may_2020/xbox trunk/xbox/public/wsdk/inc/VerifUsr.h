/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Apr 22 20:08:32 1998
 */
/* Compiler settings for verifusr.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __verifusr_h__
#define __verifusr_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __Iverifusr_FWD_DEFINED__
#define __Iverifusr_FWD_DEFINED__
typedef interface Iverifusr Iverifusr;
#endif 	/* __Iverifusr_FWD_DEFINED__ */


#ifndef __verifusr_FWD_DEFINED__
#define __verifusr_FWD_DEFINED__

#ifdef __cplusplus
typedef class verifusr verifusr;
#else
typedef struct verifusr verifusr;
#endif /* __cplusplus */

#endif 	/* __verifusr_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_verifusr_0000
 * at Wed Apr 22 20:08:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


DEFINE_GUID(IID_Iverifusr,0xC4F9C9CA,0xC33E,0x11D0,0xB8,0xA0,0x00,0xC0,0x4F,0xB6,0x16,0xC7);
DEFINE_GUID(CLSID_verifusr,0xC4F9C9CC,0xC33E,0x11D0,0xB8,0xA0,0x00,0xC0,0x4F,0xB6,0x16,0xC7);
DEFINE_GUID(LIBID_VERIFUSRLib,0xC4F9C9BD,0xC33E,0x11D0,0xB8,0xA0,0x00,0xC0,0x4F,0xB6,0x16,0xC7);


extern RPC_IF_HANDLE __MIDL_itf_verifusr_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_verifusr_0000_v0_0_s_ifspec;

#ifndef __Iverifusr_INTERFACE_DEFINED__
#define __Iverifusr_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: Iverifusr
 * at Wed Apr 22 20:08:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_Iverifusr;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C4F9C9CA-C33E-11D0-B8A0-00C04FB616C7")
    Iverifusr : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE OnStartPage( 
            /* [in] */ IUnknown __RPC_FAR *piUnk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnEndPage( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VerifyCredentials( 
            BSTR bszUserName,
            BSTR bszPassword,
            BSTR bszURL,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlToRedirect) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IssueCookieToOldUser( 
            BSTR bszUserName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IssueCookiesToNewUser( 
            BSTR bszUserName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewGUIDValue) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE HashCert( 
            BSTR bszSubjectName,
            BSTR bszIssuerName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCertHash) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IssueRecentChangesCookie( 
            BSTR bszUserName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE VerifyPassword( 
            BSTR bszUserName,
            BSTR bszPassword,
            /* [retval][out] */ LONG __RPC_FAR *plPasswordVerified) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IverifusrVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            Iverifusr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            Iverifusr __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            Iverifusr __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            Iverifusr __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            Iverifusr __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            Iverifusr __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            Iverifusr __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnStartPage )( 
            Iverifusr __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *piUnk);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnEndPage )( 
            Iverifusr __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VerifyCredentials )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszUserName,
            BSTR bszPassword,
            BSTR bszURL,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlToRedirect);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IssueCookieToOldUser )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszUserName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IssueCookiesToNewUser )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszUserName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrNewGUIDValue);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HashCert )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszSubjectName,
            BSTR bszIssuerName,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCertHash);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IssueRecentChangesCookie )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszUserName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *VerifyPassword )( 
            Iverifusr __RPC_FAR * This,
            BSTR bszUserName,
            BSTR bszPassword,
            /* [retval][out] */ LONG __RPC_FAR *plPasswordVerified);
        
        END_INTERFACE
    } IverifusrVtbl;

    interface Iverifusr
    {
        CONST_VTBL struct IverifusrVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define Iverifusr_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define Iverifusr_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define Iverifusr_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define Iverifusr_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define Iverifusr_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define Iverifusr_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define Iverifusr_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define Iverifusr_OnStartPage(This,piUnk)	\
    (This)->lpVtbl -> OnStartPage(This,piUnk)

#define Iverifusr_OnEndPage(This)	\
    (This)->lpVtbl -> OnEndPage(This)

#define Iverifusr_VerifyCredentials(This,bszUserName,bszPassword,bszURL,pbstrUrlToRedirect)	\
    (This)->lpVtbl -> VerifyCredentials(This,bszUserName,bszPassword,bszURL,pbstrUrlToRedirect)

#define Iverifusr_IssueCookieToOldUser(This,bszUserName)	\
    (This)->lpVtbl -> IssueCookieToOldUser(This,bszUserName)

#define Iverifusr_IssueCookiesToNewUser(This,bszUserName,pbstrNewGUIDValue)	\
    (This)->lpVtbl -> IssueCookiesToNewUser(This,bszUserName,pbstrNewGUIDValue)

#define Iverifusr_HashCert(This,bszSubjectName,bszIssuerName,pbstrCertHash)	\
    (This)->lpVtbl -> HashCert(This,bszSubjectName,bszIssuerName,pbstrCertHash)

#define Iverifusr_IssueRecentChangesCookie(This,bszUserName)	\
    (This)->lpVtbl -> IssueRecentChangesCookie(This,bszUserName)

#define Iverifusr_VerifyPassword(This,bszUserName,bszPassword,plPasswordVerified)	\
    (This)->lpVtbl -> VerifyPassword(This,bszUserName,bszPassword,plPasswordVerified)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE Iverifusr_OnStartPage_Proxy( 
    Iverifusr __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *piUnk);


void __RPC_STUB Iverifusr_OnStartPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE Iverifusr_OnEndPage_Proxy( 
    Iverifusr __RPC_FAR * This);


void __RPC_STUB Iverifusr_OnEndPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_VerifyCredentials_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszUserName,
    BSTR bszPassword,
    BSTR bszURL,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrUrlToRedirect);


void __RPC_STUB Iverifusr_VerifyCredentials_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_IssueCookieToOldUser_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszUserName);


void __RPC_STUB Iverifusr_IssueCookieToOldUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_IssueCookiesToNewUser_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszUserName,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrNewGUIDValue);


void __RPC_STUB Iverifusr_IssueCookiesToNewUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_HashCert_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszSubjectName,
    BSTR bszIssuerName,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCertHash);


void __RPC_STUB Iverifusr_HashCert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_IssueRecentChangesCookie_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszUserName);


void __RPC_STUB Iverifusr_IssueRecentChangesCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Iverifusr_VerifyPassword_Proxy( 
    Iverifusr __RPC_FAR * This,
    BSTR bszUserName,
    BSTR bszPassword,
    /* [retval][out] */ LONG __RPC_FAR *plPasswordVerified);


void __RPC_STUB Iverifusr_VerifyPassword_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __Iverifusr_INTERFACE_DEFINED__ */



#ifndef __VERIFUSRLib_LIBRARY_DEFINED__
#define __VERIFUSRLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: VERIFUSRLib
 * at Wed Apr 22 20:08:32 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_VERIFUSRLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_verifusr;

class DECLSPEC_UUID("C4F9C9CC-C33E-11D0-B8A0-00C04FB616C7")
verifusr;
#endif
#endif /* __VERIFUSRLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
