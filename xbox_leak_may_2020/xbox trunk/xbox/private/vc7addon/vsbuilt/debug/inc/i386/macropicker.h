
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Mon Jan 21 14:46:30 2002
 */
/* Compiler settings for macropicker.idl:
    Oicf, W0, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data , no_format_optimization
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 440
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __macropicker_h__
#define __macropicker_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVCMacroPicker_FWD_DEFINED__
#define __IVCMacroPicker_FWD_DEFINED__
typedef interface IVCMacroPicker IVCMacroPicker;
#endif 	/* __IVCMacroPicker_FWD_DEFINED__ */


#ifndef __VCMacroPicker_FWD_DEFINED__
#define __VCMacroPicker_FWD_DEFINED__

#ifdef __cplusplus
typedef class VCMacroPicker VCMacroPicker;
#else
typedef struct VCMacroPicker VCMacroPicker;
#endif /* __cplusplus */

#endif 	/* __VCMacroPicker_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_macropicker_0000 */
/* [local] */ 

typedef /* [public] */ struct __MIDL___MIDL_itf_macropicker_0000_0001
    {
    IUnknown *m_pContainer;
    IUnknown *m_pProjEngine;
    HWND m_hwndEdit;
    } 	SMacroPickerData;



extern RPC_IF_HANDLE __MIDL_itf_macropicker_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_macropicker_0000_v0_0_s_ifspec;

#ifndef __IVCMacroPicker_INTERFACE_DEFINED__
#define __IVCMacroPicker_INTERFACE_DEFINED__

/* interface IVCMacroPicker */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IVCMacroPicker;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("04A9645F-D651-4FB2-B614-D221E04AE611")
    IVCMacroPicker : public IDispatch
    {
    public:
        virtual /* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ LONG_PTR *pHWND) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnInsertMacro( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IUnknown *pPropContainer,
            /* [in] */ IUnknown *pProjEngine,
            /* [in] */ LONG_PTR hwnd,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVCMacroPickerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVCMacroPicker * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVCMacroPicker * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVCMacroPicker * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVCMacroPicker * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVCMacroPicker * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVCMacroPicker * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVCMacroPicker * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][requestedit][bindable][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IVCMacroPicker * This,
            /* [retval][out] */ LONG_PTR *pHWND);
        
        HRESULT ( STDMETHODCALLTYPE *OnInsertMacro )( 
            IVCMacroPicker * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IVCMacroPicker * This,
            /* [in] */ IUnknown *pPropContainer,
            /* [in] */ IUnknown *pProjEngine,
            /* [in] */ LONG_PTR hwnd,
            /* [in] */ BSTR bstrName,
            /* [in] */ BSTR bstrValue);
        
        END_INTERFACE
    } IVCMacroPickerVtbl;

    interface IVCMacroPicker
    {
        CONST_VTBL struct IVCMacroPickerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVCMacroPicker_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IVCMacroPicker_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IVCMacroPicker_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IVCMacroPicker_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IVCMacroPicker_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IVCMacroPicker_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IVCMacroPicker_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IVCMacroPicker_get_HWND(This,pHWND)	\
    (This)->lpVtbl -> get_HWND(This,pHWND)

#define IVCMacroPicker_OnInsertMacro(This)	\
    (This)->lpVtbl -> OnInsertMacro(This)

#define IVCMacroPicker_Initialize(This,pPropContainer,pProjEngine,hwnd,bstrName,bstrValue)	\
    (This)->lpVtbl -> Initialize(This,pPropContainer,pProjEngine,hwnd,bstrName,bstrValue)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][requestedit][bindable][propget] */ HRESULT STDMETHODCALLTYPE IVCMacroPicker_get_HWND_Proxy( 
    IVCMacroPicker * This,
    /* [retval][out] */ LONG_PTR *pHWND);


void __RPC_STUB IVCMacroPicker_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCMacroPicker_OnInsertMacro_Proxy( 
    IVCMacroPicker * This);


void __RPC_STUB IVCMacroPicker_OnInsertMacro_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IVCMacroPicker_Initialize_Proxy( 
    IVCMacroPicker * This,
    /* [in] */ IUnknown *pPropContainer,
    /* [in] */ IUnknown *pProjEngine,
    /* [in] */ LONG_PTR hwnd,
    /* [in] */ BSTR bstrName,
    /* [in] */ BSTR bstrValue);


void __RPC_STUB IVCMacroPicker_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IVCMacroPicker_INTERFACE_DEFINED__ */



#ifndef __MacroPickerLib_LIBRARY_DEFINED__
#define __MacroPickerLib_LIBRARY_DEFINED__

/* library MacroPickerLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_MacroPickerLib;

EXTERN_C const CLSID CLSID_VCMacroPicker;

#ifdef __cplusplus

class DECLSPEC_UUID("3781071C-736B-4121-B7CE-CE7B7EED649D")
VCMacroPicker;
#endif
#endif /* __MacroPickerLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


