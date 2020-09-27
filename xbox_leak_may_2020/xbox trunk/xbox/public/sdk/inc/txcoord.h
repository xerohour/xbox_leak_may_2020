
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Wed Sep 15 03:27:40 1999
 */
/* Compiler settings for txcoord.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
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

#ifndef __txcoord_h__
#define __txcoord_h__

/* Forward Declarations */ 

#ifndef __ITransactionResourceAsync_FWD_DEFINED__
#define __ITransactionResourceAsync_FWD_DEFINED__
typedef interface ITransactionResourceAsync ITransactionResourceAsync;
#endif 	/* __ITransactionResourceAsync_FWD_DEFINED__ */


#ifndef __ITransactionResource_FWD_DEFINED__
#define __ITransactionResource_FWD_DEFINED__
typedef interface ITransactionResource ITransactionResource;
#endif 	/* __ITransactionResource_FWD_DEFINED__ */


#ifndef __ITransactionEnlistmentAsync_FWD_DEFINED__
#define __ITransactionEnlistmentAsync_FWD_DEFINED__
typedef interface ITransactionEnlistmentAsync ITransactionEnlistmentAsync;
#endif 	/* __ITransactionEnlistmentAsync_FWD_DEFINED__ */


#ifndef __ITransactionExportFactory_FWD_DEFINED__
#define __ITransactionExportFactory_FWD_DEFINED__
typedef interface ITransactionExportFactory ITransactionExportFactory;
#endif 	/* __ITransactionExportFactory_FWD_DEFINED__ */


#ifndef __ITransactionImportWhereabouts_FWD_DEFINED__
#define __ITransactionImportWhereabouts_FWD_DEFINED__
typedef interface ITransactionImportWhereabouts ITransactionImportWhereabouts;
#endif 	/* __ITransactionImportWhereabouts_FWD_DEFINED__ */


#ifndef __ITransactionExport_FWD_DEFINED__
#define __ITransactionExport_FWD_DEFINED__
typedef interface ITransactionExport ITransactionExport;
#endif 	/* __ITransactionExport_FWD_DEFINED__ */


#ifndef __ITransactionImport_FWD_DEFINED__
#define __ITransactionImport_FWD_DEFINED__
typedef interface ITransactionImport ITransactionImport;
#endif 	/* __ITransactionImport_FWD_DEFINED__ */


#ifndef __ITipTransaction_FWD_DEFINED__
#define __ITipTransaction_FWD_DEFINED__
typedef interface ITipTransaction ITipTransaction;
#endif 	/* __ITipTransaction_FWD_DEFINED__ */


#ifndef __ITipHelper_FWD_DEFINED__
#define __ITipHelper_FWD_DEFINED__
typedef interface ITipHelper ITipHelper;
#endif 	/* __ITipHelper_FWD_DEFINED__ */


#ifndef __ITipPullSink_FWD_DEFINED__
#define __ITipPullSink_FWD_DEFINED__
typedef interface ITipPullSink ITipPullSink;
#endif 	/* __ITipPullSink_FWD_DEFINED__ */


/* header files for imported files */
#include "transact.h"
#include "objidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_txcoord_0000 */
/* [local] */ 













extern RPC_IF_HANDLE __MIDL_itf_txcoord_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_txcoord_0000_v0_0_s_ifspec;

#ifndef __ITransactionResourceAsync_INTERFACE_DEFINED__
#define __ITransactionResourceAsync_INTERFACE_DEFINED__

/* interface ITransactionResourceAsync */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionResourceAsync;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("69E971F0-23CE-11cf-AD60-00AA00A74CCD")
    ITransactionResourceAsync : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PrepareRequest( 
            /* [in] */ BOOL fRetaining,
            /* [in] */ DWORD grfRM,
            /* [in] */ BOOL fWantMoniker,
            /* [in] */ BOOL fSinglePhase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitRequest( 
            /* [in] */ DWORD grfRM,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AbortRequest( 
            /* [unique][in] */ BOID __RPC_FAR *pboidReason,
            /* [in] */ BOOL fRetaining,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TMDown( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionResourceAsyncVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionResourceAsync __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionResourceAsync __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionResourceAsync __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareRequest )( 
            ITransactionResourceAsync __RPC_FAR * This,
            /* [in] */ BOOL fRetaining,
            /* [in] */ DWORD grfRM,
            /* [in] */ BOOL fWantMoniker,
            /* [in] */ BOOL fSinglePhase);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitRequest )( 
            ITransactionResourceAsync __RPC_FAR * This,
            /* [in] */ DWORD grfRM,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortRequest )( 
            ITransactionResourceAsync __RPC_FAR * This,
            /* [unique][in] */ BOID __RPC_FAR *pboidReason,
            /* [in] */ BOOL fRetaining,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TMDown )( 
            ITransactionResourceAsync __RPC_FAR * This);
        
        END_INTERFACE
    } ITransactionResourceAsyncVtbl;

    interface ITransactionResourceAsync
    {
        CONST_VTBL struct ITransactionResourceAsyncVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionResourceAsync_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionResourceAsync_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionResourceAsync_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionResourceAsync_PrepareRequest(This,fRetaining,grfRM,fWantMoniker,fSinglePhase)	\
    (This)->lpVtbl -> PrepareRequest(This,fRetaining,grfRM,fWantMoniker,fSinglePhase)

#define ITransactionResourceAsync_CommitRequest(This,grfRM,pNewUOW)	\
    (This)->lpVtbl -> CommitRequest(This,grfRM,pNewUOW)

#define ITransactionResourceAsync_AbortRequest(This,pboidReason,fRetaining,pNewUOW)	\
    (This)->lpVtbl -> AbortRequest(This,pboidReason,fRetaining,pNewUOW)

#define ITransactionResourceAsync_TMDown(This)	\
    (This)->lpVtbl -> TMDown(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionResourceAsync_PrepareRequest_Proxy( 
    ITransactionResourceAsync __RPC_FAR * This,
    /* [in] */ BOOL fRetaining,
    /* [in] */ DWORD grfRM,
    /* [in] */ BOOL fWantMoniker,
    /* [in] */ BOOL fSinglePhase);


void __RPC_STUB ITransactionResourceAsync_PrepareRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResourceAsync_CommitRequest_Proxy( 
    ITransactionResourceAsync __RPC_FAR * This,
    /* [in] */ DWORD grfRM,
    /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);


void __RPC_STUB ITransactionResourceAsync_CommitRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResourceAsync_AbortRequest_Proxy( 
    ITransactionResourceAsync __RPC_FAR * This,
    /* [unique][in] */ BOID __RPC_FAR *pboidReason,
    /* [in] */ BOOL fRetaining,
    /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);


void __RPC_STUB ITransactionResourceAsync_AbortRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResourceAsync_TMDown_Proxy( 
    ITransactionResourceAsync __RPC_FAR * This);


void __RPC_STUB ITransactionResourceAsync_TMDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionResourceAsync_INTERFACE_DEFINED__ */


#ifndef __ITransactionResource_INTERFACE_DEFINED__
#define __ITransactionResource_INTERFACE_DEFINED__

/* interface ITransactionResource */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionResource;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE5FF7B3-4572-11d0-9452-00A0C905416E")
    ITransactionResource : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PrepareRequest( 
            /* [in] */ BOOL fRetaining,
            /* [in] */ DWORD grfRM,
            /* [in] */ BOOL fWantMoniker,
            /* [in] */ BOOL fSinglePhase) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitRequest( 
            /* [in] */ DWORD grfRM,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AbortRequest( 
            /* [unique][in] */ BOID __RPC_FAR *pboidReason,
            /* [in] */ BOOL fRetaining,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TMDown( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionResourceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionResource __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionResource __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionResource __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareRequest )( 
            ITransactionResource __RPC_FAR * This,
            /* [in] */ BOOL fRetaining,
            /* [in] */ DWORD grfRM,
            /* [in] */ BOOL fWantMoniker,
            /* [in] */ BOOL fSinglePhase);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitRequest )( 
            ITransactionResource __RPC_FAR * This,
            /* [in] */ DWORD grfRM,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortRequest )( 
            ITransactionResource __RPC_FAR * This,
            /* [unique][in] */ BOID __RPC_FAR *pboidReason,
            /* [in] */ BOOL fRetaining,
            /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *TMDown )( 
            ITransactionResource __RPC_FAR * This);
        
        END_INTERFACE
    } ITransactionResourceVtbl;

    interface ITransactionResource
    {
        CONST_VTBL struct ITransactionResourceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionResource_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionResource_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionResource_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionResource_PrepareRequest(This,fRetaining,grfRM,fWantMoniker,fSinglePhase)	\
    (This)->lpVtbl -> PrepareRequest(This,fRetaining,grfRM,fWantMoniker,fSinglePhase)

#define ITransactionResource_CommitRequest(This,grfRM,pNewUOW)	\
    (This)->lpVtbl -> CommitRequest(This,grfRM,pNewUOW)

#define ITransactionResource_AbortRequest(This,pboidReason,fRetaining,pNewUOW)	\
    (This)->lpVtbl -> AbortRequest(This,pboidReason,fRetaining,pNewUOW)

#define ITransactionResource_TMDown(This)	\
    (This)->lpVtbl -> TMDown(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionResource_PrepareRequest_Proxy( 
    ITransactionResource __RPC_FAR * This,
    /* [in] */ BOOL fRetaining,
    /* [in] */ DWORD grfRM,
    /* [in] */ BOOL fWantMoniker,
    /* [in] */ BOOL fSinglePhase);


void __RPC_STUB ITransactionResource_PrepareRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResource_CommitRequest_Proxy( 
    ITransactionResource __RPC_FAR * This,
    /* [in] */ DWORD grfRM,
    /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);


void __RPC_STUB ITransactionResource_CommitRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResource_AbortRequest_Proxy( 
    ITransactionResource __RPC_FAR * This,
    /* [unique][in] */ BOID __RPC_FAR *pboidReason,
    /* [in] */ BOOL fRetaining,
    /* [unique][in] */ XACTUOW __RPC_FAR *pNewUOW);


void __RPC_STUB ITransactionResource_AbortRequest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionResource_TMDown_Proxy( 
    ITransactionResource __RPC_FAR * This);


void __RPC_STUB ITransactionResource_TMDown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionResource_INTERFACE_DEFINED__ */


#ifndef __ITransactionEnlistmentAsync_INTERFACE_DEFINED__
#define __ITransactionEnlistmentAsync_INTERFACE_DEFINED__

/* interface ITransactionEnlistmentAsync */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionEnlistmentAsync;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0fb15081-af41-11ce-bd2b-204c4f4f5020")
    ITransactionEnlistmentAsync : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PrepareRequestDone( 
            /* [in] */ HRESULT hr,
            /* [unique][in] */ IMoniker __RPC_FAR *pmk,
            /* [unique][in] */ BOID __RPC_FAR *pboidReason) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CommitRequestDone( 
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AbortRequestDone( 
            /* [in] */ HRESULT hr) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionEnlistmentAsyncVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionEnlistmentAsync __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionEnlistmentAsync __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionEnlistmentAsync __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareRequestDone )( 
            ITransactionEnlistmentAsync __RPC_FAR * This,
            /* [in] */ HRESULT hr,
            /* [unique][in] */ IMoniker __RPC_FAR *pmk,
            /* [unique][in] */ BOID __RPC_FAR *pboidReason);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CommitRequestDone )( 
            ITransactionEnlistmentAsync __RPC_FAR * This,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AbortRequestDone )( 
            ITransactionEnlistmentAsync __RPC_FAR * This,
            /* [in] */ HRESULT hr);
        
        END_INTERFACE
    } ITransactionEnlistmentAsyncVtbl;

    interface ITransactionEnlistmentAsync
    {
        CONST_VTBL struct ITransactionEnlistmentAsyncVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionEnlistmentAsync_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionEnlistmentAsync_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionEnlistmentAsync_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionEnlistmentAsync_PrepareRequestDone(This,hr,pmk,pboidReason)	\
    (This)->lpVtbl -> PrepareRequestDone(This,hr,pmk,pboidReason)

#define ITransactionEnlistmentAsync_CommitRequestDone(This,hr)	\
    (This)->lpVtbl -> CommitRequestDone(This,hr)

#define ITransactionEnlistmentAsync_AbortRequestDone(This,hr)	\
    (This)->lpVtbl -> AbortRequestDone(This,hr)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionEnlistmentAsync_PrepareRequestDone_Proxy( 
    ITransactionEnlistmentAsync __RPC_FAR * This,
    /* [in] */ HRESULT hr,
    /* [unique][in] */ IMoniker __RPC_FAR *pmk,
    /* [unique][in] */ BOID __RPC_FAR *pboidReason);


void __RPC_STUB ITransactionEnlistmentAsync_PrepareRequestDone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionEnlistmentAsync_CommitRequestDone_Proxy( 
    ITransactionEnlistmentAsync __RPC_FAR * This,
    /* [in] */ HRESULT hr);


void __RPC_STUB ITransactionEnlistmentAsync_CommitRequestDone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionEnlistmentAsync_AbortRequestDone_Proxy( 
    ITransactionEnlistmentAsync __RPC_FAR * This,
    /* [in] */ HRESULT hr);


void __RPC_STUB ITransactionEnlistmentAsync_AbortRequestDone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionEnlistmentAsync_INTERFACE_DEFINED__ */


#ifndef __ITransactionExportFactory_INTERFACE_DEFINED__
#define __ITransactionExportFactory_INTERFACE_DEFINED__

/* interface ITransactionExportFactory */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionExportFactory;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E1CF9B53-8745-11ce-A9BA-00AA006C3706")
    ITransactionExportFactory : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRemoteClassId( 
            /* [out] */ CLSID __RPC_FAR *pclsid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Create( 
            /* [in] */ ULONG cbWhereabouts,
            /* [size_is][in] */ byte __RPC_FAR *rgbWhereabouts,
            /* [out] */ ITransactionExport __RPC_FAR *__RPC_FAR *ppExport) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionExportFactoryVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionExportFactory __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionExportFactory __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionExportFactory __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetRemoteClassId )( 
            ITransactionExportFactory __RPC_FAR * This,
            /* [out] */ CLSID __RPC_FAR *pclsid);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Create )( 
            ITransactionExportFactory __RPC_FAR * This,
            /* [in] */ ULONG cbWhereabouts,
            /* [size_is][in] */ byte __RPC_FAR *rgbWhereabouts,
            /* [out] */ ITransactionExport __RPC_FAR *__RPC_FAR *ppExport);
        
        END_INTERFACE
    } ITransactionExportFactoryVtbl;

    interface ITransactionExportFactory
    {
        CONST_VTBL struct ITransactionExportFactoryVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionExportFactory_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionExportFactory_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionExportFactory_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionExportFactory_GetRemoteClassId(This,pclsid)	\
    (This)->lpVtbl -> GetRemoteClassId(This,pclsid)

#define ITransactionExportFactory_Create(This,cbWhereabouts,rgbWhereabouts,ppExport)	\
    (This)->lpVtbl -> Create(This,cbWhereabouts,rgbWhereabouts,ppExport)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionExportFactory_GetRemoteClassId_Proxy( 
    ITransactionExportFactory __RPC_FAR * This,
    /* [out] */ CLSID __RPC_FAR *pclsid);


void __RPC_STUB ITransactionExportFactory_GetRemoteClassId_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITransactionExportFactory_Create_Proxy( 
    ITransactionExportFactory __RPC_FAR * This,
    /* [in] */ ULONG cbWhereabouts,
    /* [size_is][in] */ byte __RPC_FAR *rgbWhereabouts,
    /* [out] */ ITransactionExport __RPC_FAR *__RPC_FAR *ppExport);


void __RPC_STUB ITransactionExportFactory_Create_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionExportFactory_INTERFACE_DEFINED__ */


#ifndef __ITransactionImportWhereabouts_INTERFACE_DEFINED__
#define __ITransactionImportWhereabouts_INTERFACE_DEFINED__

/* interface ITransactionImportWhereabouts */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionImportWhereabouts;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0141fda4-8fc0-11ce-bd18-204c4f4f5020")
    ITransactionImportWhereabouts : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetWhereaboutsSize( 
            /* [out] */ ULONG __RPC_FAR *pcbWhereabouts) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetWhereabouts( 
            /* [in] */ ULONG cbWhereabouts,
            /* [size_is][out] */ byte __RPC_FAR *rgbWhereabouts,
            /* [out] */ ULONG __RPC_FAR *pcbUsed) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionImportWhereaboutsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionImportWhereabouts __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionImportWhereabouts __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionImportWhereabouts __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhereaboutsSize )( 
            ITransactionImportWhereabouts __RPC_FAR * This,
            /* [out] */ ULONG __RPC_FAR *pcbWhereabouts);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetWhereabouts )( 
            ITransactionImportWhereabouts __RPC_FAR * This,
            /* [in] */ ULONG cbWhereabouts,
            /* [size_is][out] */ byte __RPC_FAR *rgbWhereabouts,
            /* [out] */ ULONG __RPC_FAR *pcbUsed);
        
        END_INTERFACE
    } ITransactionImportWhereaboutsVtbl;

    interface ITransactionImportWhereabouts
    {
        CONST_VTBL struct ITransactionImportWhereaboutsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionImportWhereabouts_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionImportWhereabouts_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionImportWhereabouts_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionImportWhereabouts_GetWhereaboutsSize(This,pcbWhereabouts)	\
    (This)->lpVtbl -> GetWhereaboutsSize(This,pcbWhereabouts)

#define ITransactionImportWhereabouts_GetWhereabouts(This,cbWhereabouts,rgbWhereabouts,pcbUsed)	\
    (This)->lpVtbl -> GetWhereabouts(This,cbWhereabouts,rgbWhereabouts,pcbUsed)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionImportWhereabouts_GetWhereaboutsSize_Proxy( 
    ITransactionImportWhereabouts __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcbWhereabouts);


void __RPC_STUB ITransactionImportWhereabouts_GetWhereaboutsSize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ITransactionImportWhereabouts_RemoteGetWhereabouts_Proxy( 
    ITransactionImportWhereabouts __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcbUsed,
    /* [in] */ ULONG cbWhereabouts,
    /* [length_is][size_is][out] */ byte __RPC_FAR *rgbWhereabouts);


void __RPC_STUB ITransactionImportWhereabouts_RemoteGetWhereabouts_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionImportWhereabouts_INTERFACE_DEFINED__ */


#ifndef __ITransactionExport_INTERFACE_DEFINED__
#define __ITransactionExport_INTERFACE_DEFINED__

/* interface ITransactionExport */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionExport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0141fda5-8fc0-11ce-bd18-204c4f4f5020")
    ITransactionExport : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Export( 
            /* [in] */ IUnknown __RPC_FAR *punkTransaction,
            /* [out] */ ULONG __RPC_FAR *pcbTransactionCookie) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE GetTransactionCookie( 
            /* [in] */ IUnknown __RPC_FAR *punkTransaction,
            /* [in] */ ULONG cbTransactionCookie,
            /* [size_is][out] */ byte __RPC_FAR *rgbTransactionCookie,
            /* [out] */ ULONG __RPC_FAR *pcbUsed) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionExportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionExport __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionExport __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionExport __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Export )( 
            ITransactionExport __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *punkTransaction,
            /* [out] */ ULONG __RPC_FAR *pcbTransactionCookie);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransactionCookie )( 
            ITransactionExport __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *punkTransaction,
            /* [in] */ ULONG cbTransactionCookie,
            /* [size_is][out] */ byte __RPC_FAR *rgbTransactionCookie,
            /* [out] */ ULONG __RPC_FAR *pcbUsed);
        
        END_INTERFACE
    } ITransactionExportVtbl;

    interface ITransactionExport
    {
        CONST_VTBL struct ITransactionExportVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionExport_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionExport_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionExport_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionExport_Export(This,punkTransaction,pcbTransactionCookie)	\
    (This)->lpVtbl -> Export(This,punkTransaction,pcbTransactionCookie)

#define ITransactionExport_GetTransactionCookie(This,punkTransaction,cbTransactionCookie,rgbTransactionCookie,pcbUsed)	\
    (This)->lpVtbl -> GetTransactionCookie(This,punkTransaction,cbTransactionCookie,rgbTransactionCookie,pcbUsed)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionExport_Export_Proxy( 
    ITransactionExport __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *punkTransaction,
    /* [out] */ ULONG __RPC_FAR *pcbTransactionCookie);


void __RPC_STUB ITransactionExport_Export_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ITransactionExport_RemoteGetTransactionCookie_Proxy( 
    ITransactionExport __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *punkTransaction,
    /* [out] */ ULONG __RPC_FAR *pcbUsed,
    /* [in] */ ULONG cbTransactionCookie,
    /* [length_is][size_is][out] */ byte __RPC_FAR *rgbTransactionCookie);


void __RPC_STUB ITransactionExport_RemoteGetTransactionCookie_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionExport_INTERFACE_DEFINED__ */


#ifndef __ITransactionImport_INTERFACE_DEFINED__
#define __ITransactionImport_INTERFACE_DEFINED__

/* interface ITransactionImport */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITransactionImport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("E1CF9B5A-8745-11ce-A9BA-00AA006C3706")
    ITransactionImport : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Import( 
            /* [in] */ ULONG cbTransactionCookie,
            /* [size_is][in] */ byte __RPC_FAR *rgbTransactionCookie,
            /* [in] */ IID __RPC_FAR *piid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvTransaction) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITransactionImportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITransactionImport __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITransactionImport __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITransactionImport __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Import )( 
            ITransactionImport __RPC_FAR * This,
            /* [in] */ ULONG cbTransactionCookie,
            /* [size_is][in] */ byte __RPC_FAR *rgbTransactionCookie,
            /* [in] */ IID __RPC_FAR *piid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvTransaction);
        
        END_INTERFACE
    } ITransactionImportVtbl;

    interface ITransactionImport
    {
        CONST_VTBL struct ITransactionImportVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITransactionImport_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITransactionImport_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITransactionImport_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITransactionImport_Import(This,cbTransactionCookie,rgbTransactionCookie,piid,ppvTransaction)	\
    (This)->lpVtbl -> Import(This,cbTransactionCookie,rgbTransactionCookie,piid,ppvTransaction)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITransactionImport_Import_Proxy( 
    ITransactionImport __RPC_FAR * This,
    /* [in] */ ULONG cbTransactionCookie,
    /* [size_is][in] */ byte __RPC_FAR *rgbTransactionCookie,
    /* [in] */ IID __RPC_FAR *piid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvTransaction);


void __RPC_STUB ITransactionImport_Import_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITransactionImport_INTERFACE_DEFINED__ */


#ifndef __ITipTransaction_INTERFACE_DEFINED__
#define __ITipTransaction_INTERFACE_DEFINED__

/* interface ITipTransaction */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITipTransaction;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17CF72D0-BAC5-11d1-B1BF-00C04FC2F3EF")
    ITipTransaction : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Push( 
            /* [in] */ char __RPC_FAR *i_pszRemoteTmUrl,
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszRemoteTxUrl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTransactionUrl( 
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTxUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITipTransactionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITipTransaction __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITipTransaction __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITipTransaction __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Push )( 
            ITipTransaction __RPC_FAR * This,
            /* [in] */ char __RPC_FAR *i_pszRemoteTmUrl,
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszRemoteTxUrl);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTransactionUrl )( 
            ITipTransaction __RPC_FAR * This,
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTxUrl);
        
        END_INTERFACE
    } ITipTransactionVtbl;

    interface ITipTransaction
    {
        CONST_VTBL struct ITipTransactionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITipTransaction_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITipTransaction_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITipTransaction_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITipTransaction_Push(This,i_pszRemoteTmUrl,o_ppszRemoteTxUrl)	\
    (This)->lpVtbl -> Push(This,i_pszRemoteTmUrl,o_ppszRemoteTxUrl)

#define ITipTransaction_GetTransactionUrl(This,o_ppszLocalTxUrl)	\
    (This)->lpVtbl -> GetTransactionUrl(This,o_ppszLocalTxUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITipTransaction_Push_Proxy( 
    ITipTransaction __RPC_FAR * This,
    /* [in] */ char __RPC_FAR *i_pszRemoteTmUrl,
    /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszRemoteTxUrl);


void __RPC_STUB ITipTransaction_Push_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITipTransaction_GetTransactionUrl_Proxy( 
    ITipTransaction __RPC_FAR * This,
    /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTxUrl);


void __RPC_STUB ITipTransaction_GetTransactionUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITipTransaction_INTERFACE_DEFINED__ */


#ifndef __ITipHelper_INTERFACE_DEFINED__
#define __ITipHelper_INTERFACE_DEFINED__

/* interface ITipHelper */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITipHelper;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17CF72D1-BAC5-11d1-B1BF-00C04FC2F3EF")
    ITipHelper : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Pull( 
            /* [in] */ char __RPC_FAR *i_pszTxUrl,
            /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PullAsync( 
            /* [in] */ char __RPC_FAR *i_pszTxUrl,
            /* [in] */ ITipPullSink __RPC_FAR *i_pTipPullSink,
            /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLocalTmUrl( 
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTmUrl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITipHelperVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITipHelper __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITipHelper __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITipHelper __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Pull )( 
            ITipHelper __RPC_FAR * This,
            /* [in] */ char __RPC_FAR *i_pszTxUrl,
            /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PullAsync )( 
            ITipHelper __RPC_FAR * This,
            /* [in] */ char __RPC_FAR *i_pszTxUrl,
            /* [in] */ ITipPullSink __RPC_FAR *i_pTipPullSink,
            /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetLocalTmUrl )( 
            ITipHelper __RPC_FAR * This,
            /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTmUrl);
        
        END_INTERFACE
    } ITipHelperVtbl;

    interface ITipHelper
    {
        CONST_VTBL struct ITipHelperVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITipHelper_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITipHelper_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITipHelper_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITipHelper_Pull(This,i_pszTxUrl,o_ppITransaction)	\
    (This)->lpVtbl -> Pull(This,i_pszTxUrl,o_ppITransaction)

#define ITipHelper_PullAsync(This,i_pszTxUrl,i_pTipPullSink,o_ppITransaction)	\
    (This)->lpVtbl -> PullAsync(This,i_pszTxUrl,i_pTipPullSink,o_ppITransaction)

#define ITipHelper_GetLocalTmUrl(This,o_ppszLocalTmUrl)	\
    (This)->lpVtbl -> GetLocalTmUrl(This,o_ppszLocalTmUrl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITipHelper_Pull_Proxy( 
    ITipHelper __RPC_FAR * This,
    /* [in] */ char __RPC_FAR *i_pszTxUrl,
    /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction);


void __RPC_STUB ITipHelper_Pull_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITipHelper_PullAsync_Proxy( 
    ITipHelper __RPC_FAR * This,
    /* [in] */ char __RPC_FAR *i_pszTxUrl,
    /* [in] */ ITipPullSink __RPC_FAR *i_pTipPullSink,
    /* [out] */ ITransaction __RPC_FAR *__RPC_FAR *o_ppITransaction);


void __RPC_STUB ITipHelper_PullAsync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITipHelper_GetLocalTmUrl_Proxy( 
    ITipHelper __RPC_FAR * This,
    /* [out] */ char __RPC_FAR *__RPC_FAR *o_ppszLocalTmUrl);


void __RPC_STUB ITipHelper_GetLocalTmUrl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITipHelper_INTERFACE_DEFINED__ */


#ifndef __ITipPullSink_INTERFACE_DEFINED__
#define __ITipPullSink_INTERFACE_DEFINED__

/* interface ITipPullSink */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ITipPullSink;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("17CF72D2-BAC5-11d1-B1BF-00C04FC2F3EF")
    ITipPullSink : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PullComplete( 
            /* [in] */ HRESULT i_hrPull) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITipPullSinkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ITipPullSink __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ITipPullSink __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ITipPullSink __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PullComplete )( 
            ITipPullSink __RPC_FAR * This,
            /* [in] */ HRESULT i_hrPull);
        
        END_INTERFACE
    } ITipPullSinkVtbl;

    interface ITipPullSink
    {
        CONST_VTBL struct ITipPullSinkVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITipPullSink_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITipPullSink_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITipPullSink_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITipPullSink_PullComplete(This,i_hrPull)	\
    (This)->lpVtbl -> PullComplete(This,i_hrPull)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITipPullSink_PullComplete_Proxy( 
    ITipPullSink __RPC_FAR * This,
    /* [in] */ HRESULT i_hrPull);


void __RPC_STUB ITipPullSink_PullComplete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITipPullSink_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_txcoord_0101 */
/* [local] */ 



#if _MSC_VER < 1100 || !defined(__cplusplus)

DEFINE_GUID(IID_ITransactionResourceAsync,		0x69E971F0, 0x23CE, 0x11cf, 0xAD, 0x60, 0x00, 0xAA, 0x00, 0xA7, 0x4C, 0xCD);
DEFINE_GUID(IID_ITransactionResource,			0xEE5FF7B3, 0x4572, 0x11d0, 0x94, 0x52, 0x00, 0xA0, 0xC9, 0x05, 0x41, 0x6E);
DEFINE_GUID(IID_ITransactionEnlistmentAsync,		0x0fb15081, 0xaf41, 0x11ce, 0xbd, 0x2b, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);
DEFINE_GUID(IID_ITransactionExportFactory,		0xE1CF9B53, 0x8745, 0x11ce, 0xA9, 0xBA, 0x00, 0xAA, 0x00, 0x6C, 0x37, 0x06);
DEFINE_GUID(IID_ITransactionImportWhereabouts,	0x0141fda4, 0x8fc0, 0x11ce, 0xbd, 0x18, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);
DEFINE_GUID(IID_ITransactionExport,				0x0141fda5, 0x8fc0, 0x11ce, 0xbd, 0x18, 0x20, 0x4c, 0x4f, 0x4f, 0x50, 0x20);
DEFINE_GUID(IID_ITransactionImport,				0xE1CF9B5A, 0x8745, 0x11ce, 0xA9, 0xBA, 0x00, 0xAA, 0x00, 0x6C, 0x37, 0x06);
DEFINE_GUID(IID_ITipTransaction,					0x17cf72d0, 0xbac5, 0x11d1, 0xb1, 0xbf, 0x0, 0xc0, 0x4f, 0xc2, 0xf3, 0xef);
DEFINE_GUID(IID_ITipHelper,						0x17cf72d1, 0xbac5, 0x11d1, 0xb1, 0xbf, 0x0, 0xc0, 0x4f, 0xc2, 0xf3, 0xef);
DEFINE_GUID(IID_ITipPullSink,					0x17cf72d2, 0xbac5, 0x11d1, 0xb1, 0xbf, 0x0, 0xc0, 0x4f, 0xc2, 0xf3, 0xef);

#else

#define  IID_ITransactionResourceAsync               __uuidof(ITransactionResourceAsync)
#define  IID_ITransactionResource                    __uuidof(ITransactionResource)
#define  IID_ITransactionEnlistmentAsync             __uuidof(ITransactionEnlistmentAsync)
#define  IID_ITransactionExportFactory               __uuidof(ITransactionExportFactory)
#define  IID_ITransactionImportWhereabouts           __uuidof(ITransactionImportWhereabouts)
#define  IID_ITransactionExport                      __uuidof(ITransactionExport)
#define  IID_ITransactionImport                      __uuidof(ITransactionImport)
#define  IID_ITipTransaction                         __uuidof(ITipTransaction)
#define  IID_ITipHelper                              __uuidof(ITipHelper)
#define  IID_ITipPullSink                            __uuidof(ITipPullSink)

#endif


extern RPC_IF_HANDLE __MIDL_itf_txcoord_0101_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_txcoord_0101_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* [local] */ HRESULT STDMETHODCALLTYPE ITransactionImportWhereabouts_GetWhereabouts_Proxy( 
    ITransactionImportWhereabouts __RPC_FAR * This,
    /* [in] */ ULONG cbWhereabouts,
    /* [size_is][out] */ byte __RPC_FAR *rgbWhereabouts,
    /* [out] */ ULONG __RPC_FAR *pcbUsed);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ITransactionImportWhereabouts_GetWhereabouts_Stub( 
    ITransactionImportWhereabouts __RPC_FAR * This,
    /* [out] */ ULONG __RPC_FAR *pcbUsed,
    /* [in] */ ULONG cbWhereabouts,
    /* [length_is][size_is][out] */ byte __RPC_FAR *rgbWhereabouts);

/* [local] */ HRESULT STDMETHODCALLTYPE ITransactionExport_GetTransactionCookie_Proxy( 
    ITransactionExport __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *punkTransaction,
    /* [in] */ ULONG cbTransactionCookie,
    /* [size_is][out] */ byte __RPC_FAR *rgbTransactionCookie,
    /* [out] */ ULONG __RPC_FAR *pcbUsed);


/* [call_as] */ HRESULT STDMETHODCALLTYPE ITransactionExport_GetTransactionCookie_Stub( 
    ITransactionExport __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *punkTransaction,
    /* [out] */ ULONG __RPC_FAR *pcbUsed,
    /* [in] */ ULONG cbTransactionCookie,
    /* [length_is][size_is][out] */ byte __RPC_FAR *rgbTransactionCookie);



/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


