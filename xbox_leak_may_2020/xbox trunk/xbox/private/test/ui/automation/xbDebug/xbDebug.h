/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Mon Feb 11 15:00:43 2002
 */
/* Compiler settings for C:\xbox\private\test\ui\automation\xbDebug\xbDebug.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __xbDebug_h__
#define __xbDebug_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __INetDebug_FWD_DEFINED__
#define __INetDebug_FWD_DEFINED__
typedef interface INetDebug INetDebug;
#endif 	/* __INetDebug_FWD_DEFINED__ */


#ifndef __NetDebug_FWD_DEFINED__
#define __NetDebug_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetDebug NetDebug;
#else
typedef struct NetDebug NetDebug;
#endif /* __cplusplus */

#endif 	/* __NetDebug_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __INetDebug_INTERFACE_DEFINED__
#define __INetDebug_INTERFACE_DEFINED__

/* interface INetDebug */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_INetDebug;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B6BD872B-D6C7-4ACD-9704-81F4DCE65CA4")
    INetDebug : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ BSTR xboxName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Start( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetLine( 
            /* [in] */ unsigned int lineNum,
            /* [in] */ short operation,
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAllLines( 
            /* [in] */ BOOL clear,
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParseTag( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ParseTag( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ClearLines( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetDebugVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            INetDebug __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            INetDebug __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            INetDebug __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ BSTR xboxName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Start )( 
            INetDebug __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Stop )( 
            INetDebug __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLine )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ unsigned int lineNum,
            /* [in] */ short operation,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAllLines )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ BOOL clear,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParseTag )( 
            INetDebug __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ParseTag )( 
            INetDebug __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            INetDebug __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ClearLines )( 
            INetDebug __RPC_FAR * This);
        
        END_INTERFACE
    } INetDebugVtbl;

    interface INetDebug
    {
        CONST_VTBL struct INetDebugVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetDebug_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetDebug_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetDebug_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetDebug_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define INetDebug_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define INetDebug_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define INetDebug_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define INetDebug_Connect(This,xboxName)	\
    (This)->lpVtbl -> Connect(This,xboxName)

#define INetDebug_Start(This)	\
    (This)->lpVtbl -> Start(This)

#define INetDebug_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#define INetDebug_GetLine(This,lineNum,operation,pVal)	\
    (This)->lpVtbl -> GetLine(This,lineNum,operation,pVal)

#define INetDebug_GetAllLines(This,clear,pVal)	\
    (This)->lpVtbl -> GetAllLines(This,clear,pVal)

#define INetDebug_get_ParseTag(This,pVal)	\
    (This)->lpVtbl -> get_ParseTag(This,pVal)

#define INetDebug_put_ParseTag(This,newVal)	\
    (This)->lpVtbl -> put_ParseTag(This,newVal)

#define INetDebug_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define INetDebug_ClearLines(This)	\
    (This)->lpVtbl -> ClearLines(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_Connect_Proxy( 
    INetDebug __RPC_FAR * This,
    /* [in] */ BSTR xboxName);


void __RPC_STUB INetDebug_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_Start_Proxy( 
    INetDebug __RPC_FAR * This);


void __RPC_STUB INetDebug_Start_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_Stop_Proxy( 
    INetDebug __RPC_FAR * This);


void __RPC_STUB INetDebug_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_GetLine_Proxy( 
    INetDebug __RPC_FAR * This,
    /* [in] */ unsigned int lineNum,
    /* [in] */ short operation,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB INetDebug_GetLine_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_GetAllLines_Proxy( 
    INetDebug __RPC_FAR * This,
    /* [in] */ BOOL clear,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB INetDebug_GetAllLines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE INetDebug_get_ParseTag_Proxy( 
    INetDebug __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB INetDebug_get_ParseTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE INetDebug_put_ParseTag_Proxy( 
    INetDebug __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB INetDebug_put_ParseTag_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_Disconnect_Proxy( 
    INetDebug __RPC_FAR * This);


void __RPC_STUB INetDebug_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE INetDebug_ClearLines_Proxy( 
    INetDebug __RPC_FAR * This);


void __RPC_STUB INetDebug_ClearLines_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetDebug_INTERFACE_DEFINED__ */



#ifndef __XBDEBUGLib_LIBRARY_DEFINED__
#define __XBDEBUGLib_LIBRARY_DEFINED__

/* library XBDEBUGLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_XBDEBUGLib;

EXTERN_C const CLSID CLSID_NetDebug;

#ifdef __cplusplus

class DECLSPEC_UUID("B11C43CA-5735-43FD-98AF-52875CC56EA7")
NetDebug;
#endif
#endif /* __XBDEBUGLib_LIBRARY_DEFINED__ */

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
