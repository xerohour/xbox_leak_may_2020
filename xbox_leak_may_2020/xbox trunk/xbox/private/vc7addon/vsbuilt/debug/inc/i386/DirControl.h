
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Tue Jan 08 04:20:14 2002
 */
/* Compiler settings for dircontrol.idl:
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

#ifndef __dircontrol_h__
#define __dircontrol_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IDirListSite_FWD_DEFINED__
#define __IDirListSite_FWD_DEFINED__
typedef interface IDirListSite IDirListSite;
#endif 	/* __IDirListSite_FWD_DEFINED__ */


#ifndef __IDirListEvents_FWD_DEFINED__
#define __IDirListEvents_FWD_DEFINED__
typedef interface IDirListEvents IDirListEvents;
#endif 	/* __IDirListEvents_FWD_DEFINED__ */


#ifndef __IDirList_FWD_DEFINED__
#define __IDirList_FWD_DEFINED__
typedef interface IDirList IDirList;
#endif 	/* __IDirList_FWD_DEFINED__ */


#ifndef __IDirAutoCompleteList_FWD_DEFINED__
#define __IDirAutoCompleteList_FWD_DEFINED__
typedef interface IDirAutoCompleteList IDirAutoCompleteList;
#endif 	/* __IDirAutoCompleteList_FWD_DEFINED__ */


#ifndef __DirList_FWD_DEFINED__
#define __DirList_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirList DirList;
#else
typedef struct DirList DirList;
#endif /* __cplusplus */

#endif 	/* __DirList_FWD_DEFINED__ */


#ifndef __DirAutoCompleteList_FWD_DEFINED__
#define __DirAutoCompleteList_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirAutoCompleteList DirAutoCompleteList;
#else
typedef struct DirAutoCompleteList DirAutoCompleteList;
#endif /* __cplusplus */

#endif 	/* __DirAutoCompleteList_FWD_DEFINED__ */


#ifndef __DirListPackage_FWD_DEFINED__
#define __DirListPackage_FWD_DEFINED__

#ifdef __cplusplus
typedef class DirListPackage DirListPackage;
#else
typedef struct DirListPackage DirListPackage;
#endif /* __cplusplus */

#endif 	/* __DirListPackage_FWD_DEFINED__ */


#ifndef __IDirListEvents_FWD_DEFINED__
#define __IDirListEvents_FWD_DEFINED__
typedef interface IDirListEvents IDirListEvents;
#endif 	/* __IDirListEvents_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_dircontrol_0000 */
/* [local] */ 

extern "C" const __declspec(selectany) GUID SID_SDirList = { 0x5403ac85L,0xc8c8,0x4967,0xbd,0xb0,0x66,0x2d,0x48,0x5f,0x0c,0x28 };


extern RPC_IF_HANDLE __MIDL_itf_dircontrol_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dircontrol_0000_v0_0_s_ifspec;

#ifndef __IDirListSite_INTERFACE_DEFINED__
#define __IDirListSite_INTERFACE_DEFINED__

/* interface IDirListSite */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IDirListSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C3270982-3ADA-42bf-8EC5-0681DD3F7789")
    IDirListSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetSite( 
            /* [in] */ IUnknown *__MIDL_0011) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirListSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirListSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirListSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirListSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetSite )( 
            IDirListSite * This,
            /* [in] */ IUnknown *__MIDL_0011);
        
        END_INTERFACE
    } IDirListSiteVtbl;

    interface IDirListSite
    {
        CONST_VTBL struct IDirListSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirListSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirListSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirListSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirListSite_SetSite(This,__MIDL_0011)	\
    (This)->lpVtbl -> SetSite(This,__MIDL_0011)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDirListSite_SetSite_Proxy( 
    IDirListSite * This,
    /* [in] */ IUnknown *__MIDL_0011);


void __RPC_STUB IDirListSite_SetSite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirListSite_INTERFACE_DEFINED__ */


#ifndef __IDirListEvents_INTERFACE_DEFINED__
#define __IDirListEvents_INTERFACE_DEFINED__

/* interface IDirListEvents */
/* [object][dual][uuid] */ 


EXTERN_C const IID IID_IDirListEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4A68351F-7AEB-4af0-9771-3F206E1FF62F")
    IDirListEvents : public IDispatch
    {
    public:
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE OnChange( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirListEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirListEvents * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirListEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirListEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDirListEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDirListEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDirListEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDirListEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *OnChange )( 
            IDirListEvents * This);
        
        END_INTERFACE
    } IDirListEventsVtbl;

    interface IDirListEvents
    {
        CONST_VTBL struct IDirListEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirListEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirListEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirListEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirListEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDirListEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDirListEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDirListEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDirListEvents_OnChange(This)	\
    (This)->lpVtbl -> OnChange(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id] */ HRESULT STDMETHODCALLTYPE IDirListEvents_OnChange_Proxy( 
    IDirListEvents * This);


void __RPC_STUB IDirListEvents_OnChange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirListEvents_INTERFACE_DEFINED__ */


#ifndef __IDirList_INTERFACE_DEFINED__
#define __IDirList_INTERFACE_DEFINED__

/* interface IDirList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IDirList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("13ACEC02-FA52-4C89-915B-888D05844A6F")
    IDirList : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_HWND( 
            /* [retval][out] */ LONG_PTR *pHWND) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR *pbstrDescription) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR bstrDescription) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Title( 
            /* [retval][out] */ BSTR *pbstrTitle) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Title( 
            /* [in] */ BSTR bstrTitle) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowTitle( 
            /* [in] */ VARIANT_BOOL bShow) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowCheck( 
            /* [in] */ VARIANT_BOOL bShow) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long *nItems) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Index( 
            /* [in] */ long nItem,
            /* [retval][out] */ BSTR *pbstrItem) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR bstrItem) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDirListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDirList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDirList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDirList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDirList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_HWND )( 
            IDirList * This,
            /* [retval][out] */ LONG_PTR *pHWND);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Description )( 
            IDirList * This,
            /* [retval][out] */ BSTR *pbstrDescription);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Description )( 
            IDirList * This,
            /* [in] */ BSTR bstrDescription);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Title )( 
            IDirList * This,
            /* [retval][out] */ BSTR *pbstrTitle);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_Title )( 
            IDirList * This,
            /* [in] */ BSTR bstrTitle);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowTitle )( 
            IDirList * This,
            /* [in] */ VARIANT_BOOL bShow);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE *put_ShowCheck )( 
            IDirList * This,
            /* [in] */ VARIANT_BOOL bShow);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE *get_Count )( 
            IDirList * This,
            /* [retval][out] */ long *nItems);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Index )( 
            IDirList * This,
            /* [in] */ long nItem,
            /* [retval][out] */ BSTR *pbstrItem);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Add )( 
            IDirList * This,
            /* [in] */ BSTR bstrItem);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IDirList * This);
        
        END_INTERFACE
    } IDirListVtbl;

    interface IDirList
    {
        CONST_VTBL struct IDirListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDirList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDirList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDirList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDirList_get_HWND(This,pHWND)	\
    (This)->lpVtbl -> get_HWND(This,pHWND)

#define IDirList_get_Description(This,pbstrDescription)	\
    (This)->lpVtbl -> get_Description(This,pbstrDescription)

#define IDirList_put_Description(This,bstrDescription)	\
    (This)->lpVtbl -> put_Description(This,bstrDescription)

#define IDirList_get_Title(This,pbstrTitle)	\
    (This)->lpVtbl -> get_Title(This,pbstrTitle)

#define IDirList_put_Title(This,bstrTitle)	\
    (This)->lpVtbl -> put_Title(This,bstrTitle)

#define IDirList_put_ShowTitle(This,bShow)	\
    (This)->lpVtbl -> put_ShowTitle(This,bShow)

#define IDirList_put_ShowCheck(This,bShow)	\
    (This)->lpVtbl -> put_ShowCheck(This,bShow)

#define IDirList_get_Count(This,nItems)	\
    (This)->lpVtbl -> get_Count(This,nItems)

#define IDirList_Index(This,nItem,pbstrItem)	\
    (This)->lpVtbl -> Index(This,nItem,pbstrItem)

#define IDirList_Add(This,bstrItem)	\
    (This)->lpVtbl -> Add(This,bstrItem)

#define IDirList_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDirList_get_HWND_Proxy( 
    IDirList * This,
    /* [retval][out] */ LONG_PTR *pHWND);


void __RPC_STUB IDirList_get_HWND_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDirList_get_Description_Proxy( 
    IDirList * This,
    /* [retval][out] */ BSTR *pbstrDescription);


void __RPC_STUB IDirList_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IDirList_put_Description_Proxy( 
    IDirList * This,
    /* [in] */ BSTR bstrDescription);


void __RPC_STUB IDirList_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDirList_get_Title_Proxy( 
    IDirList * This,
    /* [retval][out] */ BSTR *pbstrTitle);


void __RPC_STUB IDirList_get_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IDirList_put_Title_Proxy( 
    IDirList * This,
    /* [in] */ BSTR bstrTitle);


void __RPC_STUB IDirList_put_Title_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IDirList_put_ShowTitle_Proxy( 
    IDirList * This,
    /* [in] */ VARIANT_BOOL bShow);


void __RPC_STUB IDirList_put_ShowTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IDirList_put_ShowCheck_Proxy( 
    IDirList * This,
    /* [in] */ VARIANT_BOOL bShow);


void __RPC_STUB IDirList_put_ShowCheck_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IDirList_get_Count_Proxy( 
    IDirList * This,
    /* [retval][out] */ long *nItems);


void __RPC_STUB IDirList_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IDirList_Index_Proxy( 
    IDirList * This,
    /* [in] */ long nItem,
    /* [retval][out] */ BSTR *pbstrItem);


void __RPC_STUB IDirList_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IDirList_Add_Proxy( 
    IDirList * This,
    /* [in] */ BSTR bstrItem);


void __RPC_STUB IDirList_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IDirList_Reset_Proxy( 
    IDirList * This);


void __RPC_STUB IDirList_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDirList_INTERFACE_DEFINED__ */


#ifndef __IDirAutoCompleteList_INTERFACE_DEFINED__
#define __IDirAutoCompleteList_INTERFACE_DEFINED__

/* interface IDirAutoCompleteList */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IDirAutoCompleteList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7A51B0B6-C730-44CF-9833-EC5D666B23DB")
    IDirAutoCompleteList : public IDispatch
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IDirAutoCompleteListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDirAutoCompleteList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDirAutoCompleteList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDirAutoCompleteList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IDirAutoCompleteList * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IDirAutoCompleteList * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IDirAutoCompleteList * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IDirAutoCompleteList * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IDirAutoCompleteListVtbl;

    interface IDirAutoCompleteList
    {
        CONST_VTBL struct IDirAutoCompleteListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDirAutoCompleteList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDirAutoCompleteList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDirAutoCompleteList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDirAutoCompleteList_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDirAutoCompleteList_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDirAutoCompleteList_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDirAutoCompleteList_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IDirAutoCompleteList_INTERFACE_DEFINED__ */



#ifndef __DirControlLib_LIBRARY_DEFINED__
#define __DirControlLib_LIBRARY_DEFINED__

/* library DirControlLib */
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_DirControlLib;

EXTERN_C const CLSID CLSID_DirList;

#ifdef __cplusplus

class DECLSPEC_UUID("AFD20A96-EED8-4D8C-A4EA-18AB96F6C432")
DirList;
#endif

EXTERN_C const CLSID CLSID_DirAutoCompleteList;

#ifdef __cplusplus

class DECLSPEC_UUID("106E2A2B-F06A-4535-9894-43FA4673F9BE")
DirAutoCompleteList;
#endif

EXTERN_C const CLSID CLSID_DirListPackage;

#ifdef __cplusplus

class DECLSPEC_UUID("5010C52F-44AB-4051-8CE1-D36C20D989B4")
DirListPackage;
#endif
#endif /* __DirControlLib_LIBRARY_DEFINED__ */

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


