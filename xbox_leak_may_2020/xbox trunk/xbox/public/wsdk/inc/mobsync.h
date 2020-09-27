
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Fri Sep 24 19:25:10 1999
 */
/* Compiler settings for mobsync.idl:
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

#ifndef __mobsync_h__
#define __mobsync_h__

/* Forward Declarations */ 

#ifndef __ISyncMgrSynchronize_FWD_DEFINED__
#define __ISyncMgrSynchronize_FWD_DEFINED__
typedef interface ISyncMgrSynchronize ISyncMgrSynchronize;
#endif 	/* __ISyncMgrSynchronize_FWD_DEFINED__ */


#ifndef __ISyncMgrSynchronizeCallback_FWD_DEFINED__
#define __ISyncMgrSynchronizeCallback_FWD_DEFINED__
typedef interface ISyncMgrSynchronizeCallback ISyncMgrSynchronizeCallback;
#endif 	/* __ISyncMgrSynchronizeCallback_FWD_DEFINED__ */


#ifndef __ISyncMgrEnumItems_FWD_DEFINED__
#define __ISyncMgrEnumItems_FWD_DEFINED__
typedef interface ISyncMgrEnumItems ISyncMgrEnumItems;
#endif 	/* __ISyncMgrEnumItems_FWD_DEFINED__ */


#ifndef __ISyncMgrSynchronizeInvoke_FWD_DEFINED__
#define __ISyncMgrSynchronizeInvoke_FWD_DEFINED__
typedef interface ISyncMgrSynchronizeInvoke ISyncMgrSynchronizeInvoke;
#endif 	/* __ISyncMgrSynchronizeInvoke_FWD_DEFINED__ */


#ifndef __ISyncMgrRegister_FWD_DEFINED__
#define __ISyncMgrRegister_FWD_DEFINED__
typedef interface ISyncMgrRegister ISyncMgrRegister;
#endif 	/* __ISyncMgrRegister_FWD_DEFINED__ */


#ifndef __SyncMgr_FWD_DEFINED__
#define __SyncMgr_FWD_DEFINED__

#ifdef __cplusplus
typedef class SyncMgr SyncMgr;
#else
typedef struct SyncMgr SyncMgr;
#endif /* __cplusplus */

#endif 	/* __SyncMgr_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_mobsync_0000 */
/* [local] */ 






typedef GUID SYNCMGRITEMID;

typedef REFGUID REFSYNCMGRITEMID;

typedef GUID SYNCMGRERRORID;

typedef REFGUID REFSYNCMGRERRORID;

DEFINE_GUID(CLSID_SyncMgr,0x6295df27, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
DEFINE_GUID(IID_ISyncMgrSynchronize,0x6295df40, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
DEFINE_GUID(IID_ISyncMgrSynchronizeCallback,0x6295df41, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
DEFINE_GUID(IID_ISyncMgrRegister, 0x6295df42, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
DEFINE_GUID(IID_ISyncMgrEnumItems,0x6295df2a, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
DEFINE_GUID(IID_ISyncMgrSynchronizeInvoke,0x6295df2c, 0x35ee, 0x11d1, 0x87, 0x7, 0x0, 0xc0, 0x4f, 0xd9, 0x33, 0x27);
#define S_SYNCMGR_MISSINGITEMS       MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0201)
#define S_SYNCMGR_RETRYSYNC          MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0202)
#define S_SYNCMGR_CANCELITEM         MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0203)
#define S_SYNCMGR_CANCELALL          MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0204)
#define S_SYNCMGR_ITEMDELETED        MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0210)
#define S_SYNCMGR_ENUMITEMS          MAKE_SCODE(SEVERITY_SUCCESS,FACILITY_ITF,0x0211)


extern RPC_IF_HANDLE __MIDL_itf_mobsync_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mobsync_0000_v0_0_s_ifspec;

#ifndef __ISyncMgrSynchronize_INTERFACE_DEFINED__
#define __ISyncMgrSynchronize_INTERFACE_DEFINED__

/* interface ISyncMgrSynchronize */
/* [uuid][object][local] */ 

typedef /* [unique] */ ISyncMgrSynchronize __RPC_FAR *LPSYNCMGRSYNCHRONIZE;

typedef 
enum _tagSYNCMGRFLAG
    {	SYNCMGRFLAG_CONNECT	= 0x1,
	SYNCMGRFLAG_PENDINGDISCONNECT	= 0x2,
	SYNCMGRFLAG_MANUAL	= 0x3,
	SYNCMGRFLAG_IDLE	= 0x4,
	SYNCMGRFLAG_INVOKE	= 0x5,
	SYNCMGRFLAG_SCHEDULED	= 0x6,
	SYNCMGRFLAG_EVENTMASK	= 0xff,
	SYNCMGRFLAG_SETTINGS	= 0x100,
	SYNCMGRFLAG_MAYBOTHERUSER	= 0x200
    }	SYNCMGRFLAG;

#define	MAX_SYNCMGRHANDLERNAME	( 32 )

#define SYNCMGRHANDLERFLAG_MASK 0x07

typedef 
enum _tagSYNCMGRHANDLERFLAGS
    {	SYNCMGRHANDLER_HASPROPERTIES	= 0x1,
	SYNCMGRHANDLER_MAYESTABLISHCONNECTION	= 0x2,
	SYNCMGRHANDLER_ALWAYSLISTHANDLER	= 0x4
    }	SYNCMGRHANDLERFLAGS;

typedef struct _tagSYNCMGRHANDLERINFO
    {
    DWORD cbSize;
    HICON hIcon;
    DWORD SyncMgrHandlerFlags;
    WCHAR wszHandlerName[ 32 ];
    }	SYNCMGRHANDLERINFO;

typedef struct _tagSYNCMGRHANDLERINFO __RPC_FAR *LPSYNCMGRHANDLERINFO;

#define   SYNCMGRITEMSTATE_UNCHECKED    0x0000
#define   SYNCMGRITEMSTATE_CHECKED      0x0001

typedef 
enum _tagSYNCMGRSTATUS
    {	SYNCMGRSTATUS_STOPPED	= 0,
	SYNCMGRSTATUS_SKIPPED	= 0x1,
	SYNCMGRSTATUS_PENDING	= 0x2,
	SYNCMGRSTATUS_UPDATING	= 0x3,
	SYNCMGRSTATUS_SUCCEEDED	= 0x4,
	SYNCMGRSTATUS_FAILED	= 0x5,
	SYNCMGRSTATUS_PAUSED	= 0x6,
	SYNCMGRSTATUS_RESUMING	= 0x7,
	SYNCMGRSTATUS_DELETED	= 0x100
    }	SYNCMGRSTATUS;


EXTERN_C const IID IID_ISyncMgrSynchronize;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6295DF40-35EE-11d1-8707-00C04FD93327")
    ISyncMgrSynchronize : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ DWORD dwReserved,
            /* [in] */ DWORD dwSyncMgrFlags,
            /* [in] */ DWORD cbCookie,
            /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandlerInfo( 
            /* [out] */ LPSYNCMGRHANDLERINFO __RPC_FAR *ppSyncMgrHandlerInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumSyncMgrItems( 
            /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppSyncMgrEnumItems) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetItemObject( 
            /* [in] */ REFSYNCMGRITEMID ItemID,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowProperties( 
            /* [in] */ HWND hWndParent,
            /* [in] */ REFSYNCMGRITEMID ItemID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetProgressCallback( 
            /* [unique][in] */ ISyncMgrSynchronizeCallback __RPC_FAR *lpCallBack) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PrepareForSync( 
            /* [in] */ ULONG cbNumItems,
            /* [size_is][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs,
            /* [in] */ HWND hWndParent,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Synchronize( 
            /* [in] */ HWND hWndParent) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetItemStatus( 
            /* [in] */ REFSYNCMGRITEMID pItemID,
            /* [in] */ DWORD dwSyncMgrStatus) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowError( 
            /* [in] */ HWND hWndParent,
            /* [in] */ REFSYNCMGRERRORID ErrorID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyncMgrSynchronizeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISyncMgrSynchronize __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISyncMgrSynchronize __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Initialize )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ DWORD dwSyncMgrFlags,
            /* [in] */ DWORD cbCookie,
            /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHandlerInfo )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [out] */ LPSYNCMGRHANDLERINFO __RPC_FAR *ppSyncMgrHandlerInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnumSyncMgrItems )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppSyncMgrEnumItems);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetItemObject )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ REFSYNCMGRITEMID ItemID,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowProperties )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ HWND hWndParent,
            /* [in] */ REFSYNCMGRITEMID ItemID);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetProgressCallback )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [unique][in] */ ISyncMgrSynchronizeCallback __RPC_FAR *lpCallBack);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareForSync )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ ULONG cbNumItems,
            /* [size_is][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs,
            /* [in] */ HWND hWndParent,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Synchronize )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ HWND hWndParent);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetItemStatus )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ REFSYNCMGRITEMID pItemID,
            /* [in] */ DWORD dwSyncMgrStatus);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowError )( 
            ISyncMgrSynchronize __RPC_FAR * This,
            /* [in] */ HWND hWndParent,
            /* [in] */ REFSYNCMGRERRORID ErrorID);
        
        END_INTERFACE
    } ISyncMgrSynchronizeVtbl;

    interface ISyncMgrSynchronize
    {
        CONST_VTBL struct ISyncMgrSynchronizeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncMgrSynchronize_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISyncMgrSynchronize_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISyncMgrSynchronize_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISyncMgrSynchronize_Initialize(This,dwReserved,dwSyncMgrFlags,cbCookie,lpCookie)	\
    (This)->lpVtbl -> Initialize(This,dwReserved,dwSyncMgrFlags,cbCookie,lpCookie)

#define ISyncMgrSynchronize_GetHandlerInfo(This,ppSyncMgrHandlerInfo)	\
    (This)->lpVtbl -> GetHandlerInfo(This,ppSyncMgrHandlerInfo)

#define ISyncMgrSynchronize_EnumSyncMgrItems(This,ppSyncMgrEnumItems)	\
    (This)->lpVtbl -> EnumSyncMgrItems(This,ppSyncMgrEnumItems)

#define ISyncMgrSynchronize_GetItemObject(This,ItemID,riid,ppv)	\
    (This)->lpVtbl -> GetItemObject(This,ItemID,riid,ppv)

#define ISyncMgrSynchronize_ShowProperties(This,hWndParent,ItemID)	\
    (This)->lpVtbl -> ShowProperties(This,hWndParent,ItemID)

#define ISyncMgrSynchronize_SetProgressCallback(This,lpCallBack)	\
    (This)->lpVtbl -> SetProgressCallback(This,lpCallBack)

#define ISyncMgrSynchronize_PrepareForSync(This,cbNumItems,pItemIDs,hWndParent,dwReserved)	\
    (This)->lpVtbl -> PrepareForSync(This,cbNumItems,pItemIDs,hWndParent,dwReserved)

#define ISyncMgrSynchronize_Synchronize(This,hWndParent)	\
    (This)->lpVtbl -> Synchronize(This,hWndParent)

#define ISyncMgrSynchronize_SetItemStatus(This,pItemID,dwSyncMgrStatus)	\
    (This)->lpVtbl -> SetItemStatus(This,pItemID,dwSyncMgrStatus)

#define ISyncMgrSynchronize_ShowError(This,hWndParent,ErrorID)	\
    (This)->lpVtbl -> ShowError(This,hWndParent,ErrorID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_Initialize_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ DWORD dwSyncMgrFlags,
    /* [in] */ DWORD cbCookie,
    /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie);


void __RPC_STUB ISyncMgrSynchronize_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_GetHandlerInfo_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [out] */ LPSYNCMGRHANDLERINFO __RPC_FAR *ppSyncMgrHandlerInfo);


void __RPC_STUB ISyncMgrSynchronize_GetHandlerInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_EnumSyncMgrItems_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppSyncMgrEnumItems);


void __RPC_STUB ISyncMgrSynchronize_EnumSyncMgrItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_GetItemObject_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ REFSYNCMGRITEMID ItemID,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppv);


void __RPC_STUB ISyncMgrSynchronize_GetItemObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_ShowProperties_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ HWND hWndParent,
    /* [in] */ REFSYNCMGRITEMID ItemID);


void __RPC_STUB ISyncMgrSynchronize_ShowProperties_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_SetProgressCallback_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [unique][in] */ ISyncMgrSynchronizeCallback __RPC_FAR *lpCallBack);


void __RPC_STUB ISyncMgrSynchronize_SetProgressCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_PrepareForSync_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ ULONG cbNumItems,
    /* [size_is][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs,
    /* [in] */ HWND hWndParent,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ISyncMgrSynchronize_PrepareForSync_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_Synchronize_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ HWND hWndParent);


void __RPC_STUB ISyncMgrSynchronize_Synchronize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_SetItemStatus_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ REFSYNCMGRITEMID pItemID,
    /* [in] */ DWORD dwSyncMgrStatus);


void __RPC_STUB ISyncMgrSynchronize_SetItemStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronize_ShowError_Proxy( 
    ISyncMgrSynchronize __RPC_FAR * This,
    /* [in] */ HWND hWndParent,
    /* [in] */ REFSYNCMGRERRORID ErrorID);


void __RPC_STUB ISyncMgrSynchronize_ShowError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISyncMgrSynchronize_INTERFACE_DEFINED__ */


#ifndef __ISyncMgrSynchronizeCallback_INTERFACE_DEFINED__
#define __ISyncMgrSynchronizeCallback_INTERFACE_DEFINED__

/* interface ISyncMgrSynchronizeCallback */
/* [uuid][object][local] */ 

typedef /* [unique] */ ISyncMgrSynchronizeCallback __RPC_FAR *LPSYNCMGRSYNCHRONIZECALLBACK;

#define   SYNCMGRPROGRESSITEM_STATUSTEXT         0x0001
#define   SYNCMGRPROGRESSITEM_STATUSTYPE         0x0002
#define   SYNCMGRPROGRESSITEM_PROGVALUE      0x0004
#define   SYNCMGRPROGRESSITEM_MAXVALUE       0x0008

typedef struct _tagSYNCMGRPROGRESSITEM
    {
    DWORD cbSize;
    UINT mask;
    LPCWSTR lpcStatusText;
    DWORD dwStatusType;
    INT iProgValue;
    INT iMaxValue;
    }	SYNCMGRPROGRESSITEM;

typedef struct _tagSYNCMGRPROGRESSITEM __RPC_FAR *LPSYNCMGRPROGRESSITEM;

typedef 
enum _tagSYNCMGRLOGLEVEL
    {	SYNCMGRLOGLEVEL_INFORMATION	= 0x1,
	SYNCMGRLOGLEVEL_WARNING	= 0x2,
	SYNCMGRLOGLEVEL_ERROR	= 0x3
    }	SYNCMGRLOGLEVEL;

#define   SYNCMGRLOGERROR_ERRORFLAGS                     0x0001
#define   SYNCMGRLOGERROR_ERRORID                        0x0002
#define   SYNCMGRLOGERROR_ITEMID                         0x0004

typedef 
enum _tagSYNCMGRERRORFLAGS
    {	SYNCMGRERRORFLAG_ENABLEJUMPTEXT	= 0x1
    }	SYNCMGRERRORFLAGS;

typedef struct _tagSYNCMGRLOGERRORINFO
    {
    DWORD cbSize;
    DWORD mask;
    DWORD dwSyncMgrErrorFlags;
    SYNCMGRERRORID ErrorID;
    SYNCMGRITEMID ItemID;
    }	SYNCMGRLOGERRORINFO;

typedef struct _tagSYNCMGRLOGERRORINFO __RPC_FAR *LPSYNCMGRLOGERRORINFO;


EXTERN_C const IID IID_ISyncMgrSynchronizeCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6295DF41-35EE-11d1-8707-00C04FD93327")
    ISyncMgrSynchronizeCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShowPropertiesCompleted( 
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PrepareForSyncCompleted( 
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SynchronizeCompleted( 
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowErrorCompleted( 
            /* [in] */ HRESULT hr,
            /* [in] */ ULONG cbNumItems,
            /* [size_is][unique][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Progress( 
            /* [in] */ REFSYNCMGRITEMID pItemID,
            /* [unique][in] */ LPSYNCMGRPROGRESSITEM lpSyncProgressItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LogError( 
            /* [in] */ DWORD dwErrorLevel,
            /* [in] */ LPCWSTR lpcErrorText,
            /* [unique][in] */ LPSYNCMGRLOGERRORINFO lpSyncLogError) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteLogError( 
            /* [in] */ REFSYNCMGRERRORID ErrorID,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EstablishConnection( 
            /* [unique][in] */ LPCWSTR lpwszConnection,
            /* [in] */ DWORD dwReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyncMgrSynchronizeCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowPropertiesCompleted )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrepareForSyncCompleted )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SynchronizeCompleted )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowErrorCompleted )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ HRESULT hr,
            /* [in] */ ULONG cbNumItems,
            /* [size_is][unique][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableModeless )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Progress )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ REFSYNCMGRITEMID pItemID,
            /* [unique][in] */ LPSYNCMGRPROGRESSITEM lpSyncProgressItem);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *LogError )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ DWORD dwErrorLevel,
            /* [in] */ LPCWSTR lpcErrorText,
            /* [unique][in] */ LPSYNCMGRLOGERRORINFO lpSyncLogError);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeleteLogError )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [in] */ REFSYNCMGRERRORID ErrorID,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EstablishConnection )( 
            ISyncMgrSynchronizeCallback __RPC_FAR * This,
            /* [unique][in] */ LPCWSTR lpwszConnection,
            /* [in] */ DWORD dwReserved);
        
        END_INTERFACE
    } ISyncMgrSynchronizeCallbackVtbl;

    interface ISyncMgrSynchronizeCallback
    {
        CONST_VTBL struct ISyncMgrSynchronizeCallbackVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncMgrSynchronizeCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISyncMgrSynchronizeCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISyncMgrSynchronizeCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISyncMgrSynchronizeCallback_ShowPropertiesCompleted(This,hr)	\
    (This)->lpVtbl -> ShowPropertiesCompleted(This,hr)

#define ISyncMgrSynchronizeCallback_PrepareForSyncCompleted(This,hr)	\
    (This)->lpVtbl -> PrepareForSyncCompleted(This,hr)

#define ISyncMgrSynchronizeCallback_SynchronizeCompleted(This,hr)	\
    (This)->lpVtbl -> SynchronizeCompleted(This,hr)

#define ISyncMgrSynchronizeCallback_ShowErrorCompleted(This,hr,cbNumItems,pItemIDs)	\
    (This)->lpVtbl -> ShowErrorCompleted(This,hr,cbNumItems,pItemIDs)

#define ISyncMgrSynchronizeCallback_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define ISyncMgrSynchronizeCallback_Progress(This,pItemID,lpSyncProgressItem)	\
    (This)->lpVtbl -> Progress(This,pItemID,lpSyncProgressItem)

#define ISyncMgrSynchronizeCallback_LogError(This,dwErrorLevel,lpcErrorText,lpSyncLogError)	\
    (This)->lpVtbl -> LogError(This,dwErrorLevel,lpcErrorText,lpSyncLogError)

#define ISyncMgrSynchronizeCallback_DeleteLogError(This,ErrorID,dwReserved)	\
    (This)->lpVtbl -> DeleteLogError(This,ErrorID,dwReserved)

#define ISyncMgrSynchronizeCallback_EstablishConnection(This,lpwszConnection,dwReserved)	\
    (This)->lpVtbl -> EstablishConnection(This,lpwszConnection,dwReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_ShowPropertiesCompleted_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ HRESULT hr);


void __RPC_STUB ISyncMgrSynchronizeCallback_ShowPropertiesCompleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_PrepareForSyncCompleted_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ HRESULT hr);


void __RPC_STUB ISyncMgrSynchronizeCallback_PrepareForSyncCompleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_SynchronizeCompleted_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ HRESULT hr);


void __RPC_STUB ISyncMgrSynchronizeCallback_SynchronizeCompleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_ShowErrorCompleted_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ HRESULT hr,
    /* [in] */ ULONG cbNumItems,
    /* [size_is][unique][in] */ SYNCMGRITEMID __RPC_FAR *pItemIDs);


void __RPC_STUB ISyncMgrSynchronizeCallback_ShowErrorCompleted_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_EnableModeless_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB ISyncMgrSynchronizeCallback_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_Progress_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ REFSYNCMGRITEMID pItemID,
    /* [unique][in] */ LPSYNCMGRPROGRESSITEM lpSyncProgressItem);


void __RPC_STUB ISyncMgrSynchronizeCallback_Progress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_LogError_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ DWORD dwErrorLevel,
    /* [in] */ LPCWSTR lpcErrorText,
    /* [unique][in] */ LPSYNCMGRLOGERRORINFO lpSyncLogError);


void __RPC_STUB ISyncMgrSynchronizeCallback_LogError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_DeleteLogError_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [in] */ REFSYNCMGRERRORID ErrorID,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ISyncMgrSynchronizeCallback_DeleteLogError_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeCallback_EstablishConnection_Proxy( 
    ISyncMgrSynchronizeCallback __RPC_FAR * This,
    /* [unique][in] */ LPCWSTR lpwszConnection,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ISyncMgrSynchronizeCallback_EstablishConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISyncMgrSynchronizeCallback_INTERFACE_DEFINED__ */


#ifndef __ISyncMgrEnumItems_INTERFACE_DEFINED__
#define __ISyncMgrEnumItems_INTERFACE_DEFINED__

/* interface ISyncMgrEnumItems */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ ISyncMgrEnumItems __RPC_FAR *LPSYNCMGRENUMITEMS;

#define SYNCMGRITEM_ITEMFLAGMASK  0x1F
#define	MAX_SYNCMGRITEMNAME	( 128 )

typedef 
enum _tagSYNCMGRITEMFLAGS
    {	SYNCMGRITEM_HASPROPERTIES	= 0x1,
	SYNCMGRITEM_TEMPORARY	= 0x2,
	SYNCMGRITEM_ROAMINGUSER	= 0x4,
	SYNCMGRITEM_LASTUPDATETIME	= 0x8,
	SYNCMGRITEM_MAYDELETEITEM	= 0x10
    }	SYNCMGRITEMFLAGS;

typedef struct _tagSYNCMGRITEM
    {
    DWORD cbSize;
    DWORD dwFlags;
    SYNCMGRITEMID ItemID;
    DWORD dwItemState;
    HICON hIcon;
    WCHAR wszItemName[ 128 ];
    FILETIME ftLastUpdate;
    }	SYNCMGRITEM;

typedef struct _tagSYNCMGRITEM __RPC_FAR *LPSYNCMGRITEM;


EXTERN_C const IID IID_ISyncMgrEnumItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6295DF2A-35EE-11d1-8707-00C04FD93327")
    ISyncMgrEnumItems : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPSYNCMGRITEM rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyncMgrEnumItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISyncMgrEnumItems __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISyncMgrEnumItems __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISyncMgrEnumItems __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Next )( 
            ISyncMgrEnumItems __RPC_FAR * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPSYNCMGRITEM rgelt,
            /* [out] */ ULONG __RPC_FAR *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Skip )( 
            ISyncMgrEnumItems __RPC_FAR * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reset )( 
            ISyncMgrEnumItems __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clone )( 
            ISyncMgrEnumItems __RPC_FAR * This,
            /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppenum);
        
        END_INTERFACE
    } ISyncMgrEnumItemsVtbl;

    interface ISyncMgrEnumItems
    {
        CONST_VTBL struct ISyncMgrEnumItemsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncMgrEnumItems_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISyncMgrEnumItems_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISyncMgrEnumItems_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISyncMgrEnumItems_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define ISyncMgrEnumItems_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define ISyncMgrEnumItems_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define ISyncMgrEnumItems_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISyncMgrEnumItems_Next_Proxy( 
    ISyncMgrEnumItems __RPC_FAR * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPSYNCMGRITEM rgelt,
    /* [out] */ ULONG __RPC_FAR *pceltFetched);


void __RPC_STUB ISyncMgrEnumItems_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrEnumItems_Skip_Proxy( 
    ISyncMgrEnumItems __RPC_FAR * This,
    /* [in] */ ULONG celt);


void __RPC_STUB ISyncMgrEnumItems_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrEnumItems_Reset_Proxy( 
    ISyncMgrEnumItems __RPC_FAR * This);


void __RPC_STUB ISyncMgrEnumItems_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrEnumItems_Clone_Proxy( 
    ISyncMgrEnumItems __RPC_FAR * This,
    /* [out] */ ISyncMgrEnumItems __RPC_FAR *__RPC_FAR *ppenum);


void __RPC_STUB ISyncMgrEnumItems_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISyncMgrEnumItems_INTERFACE_DEFINED__ */


#ifndef __ISyncMgrSynchronizeInvoke_INTERFACE_DEFINED__
#define __ISyncMgrSynchronizeInvoke_INTERFACE_DEFINED__

/* interface ISyncMgrSynchronizeInvoke */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ ISyncMgrSynchronizeInvoke __RPC_FAR *LPSYNCMGRSYNCHRONIZEINVOKE;

typedef 
enum _tagSYNCMGRINVOKEFLAGS
    {	SYNCMGRINVOKE_STARTSYNC	= 0x2,
	SYNCMGRINVOKE_MINIMIZED	= 0x4
    }	SYNCMGRINVOKEFLAGS;


EXTERN_C const IID IID_ISyncMgrSynchronizeInvoke;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6295DF2C-35EE-11d1-8707-00C04FD93327")
    ISyncMgrSynchronizeInvoke : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE UpdateItems( 
            /* [in] */ DWORD dwInvokeFlags,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ DWORD cbCookie,
            /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateAll( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyncMgrSynchronizeInvokeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISyncMgrSynchronizeInvoke __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISyncMgrSynchronizeInvoke __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISyncMgrSynchronizeInvoke __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateItems )( 
            ISyncMgrSynchronizeInvoke __RPC_FAR * This,
            /* [in] */ DWORD dwInvokeFlags,
            /* [in] */ REFCLSID rclsid,
            /* [in] */ DWORD cbCookie,
            /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UpdateAll )( 
            ISyncMgrSynchronizeInvoke __RPC_FAR * This);
        
        END_INTERFACE
    } ISyncMgrSynchronizeInvokeVtbl;

    interface ISyncMgrSynchronizeInvoke
    {
        CONST_VTBL struct ISyncMgrSynchronizeInvokeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncMgrSynchronizeInvoke_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISyncMgrSynchronizeInvoke_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISyncMgrSynchronizeInvoke_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISyncMgrSynchronizeInvoke_UpdateItems(This,dwInvokeFlags,rclsid,cbCookie,lpCookie)	\
    (This)->lpVtbl -> UpdateItems(This,dwInvokeFlags,rclsid,cbCookie,lpCookie)

#define ISyncMgrSynchronizeInvoke_UpdateAll(This)	\
    (This)->lpVtbl -> UpdateAll(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeInvoke_UpdateItems_Proxy( 
    ISyncMgrSynchronizeInvoke __RPC_FAR * This,
    /* [in] */ DWORD dwInvokeFlags,
    /* [in] */ REFCLSID rclsid,
    /* [in] */ DWORD cbCookie,
    /* [size_is][unique][in] */ const BYTE __RPC_FAR *lpCookie);


void __RPC_STUB ISyncMgrSynchronizeInvoke_UpdateItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrSynchronizeInvoke_UpdateAll_Proxy( 
    ISyncMgrSynchronizeInvoke __RPC_FAR * This);


void __RPC_STUB ISyncMgrSynchronizeInvoke_UpdateAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISyncMgrSynchronizeInvoke_INTERFACE_DEFINED__ */


#ifndef __ISyncMgrRegister_INTERFACE_DEFINED__
#define __ISyncMgrRegister_INTERFACE_DEFINED__

/* interface ISyncMgrRegister */
/* [unique][uuid][object][local] */ 

typedef /* [unique] */ ISyncMgrRegister __RPC_FAR *LPSYNCMGRREGISTER;

#define SYNCMGRREGISTERFLAGS_MASK  0x07

typedef 
enum _tagSYNCMGRREGISTERFLAGS
    {	SYNCMGRREGISTERFLAG_CONNECT	= 0x1,
	SYNCMGRREGISTERFLAG_PENDINGDISCONNECT	= 0x2,
	SYNCMGRREGISTERFLAG_IDLE	= 0x4
    }	SYNCMGRREGISTERFLAGS;


EXTERN_C const IID IID_ISyncMgrRegister;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("6295DF42-35EE-11d1-8707-00C04FD93327")
    ISyncMgrRegister : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RegisterSyncMgrHandler( 
            /* [in] */ REFCLSID rclsidHandler,
            /* [unique][in] */ LPCWSTR pwszDescription,
            /* [in] */ DWORD dwSyncMgrRegisterFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnregisterSyncMgrHandler( 
            /* [in] */ REFCLSID rclsidHandler,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHandlerRegistrationInfo( 
            /* [in] */ REFCLSID rclsidHandler,
            /* [out][in] */ LPDWORD pdwSyncMgrRegisterFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISyncMgrRegisterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISyncMgrRegister __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISyncMgrRegister __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISyncMgrRegister __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RegisterSyncMgrHandler )( 
            ISyncMgrRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsidHandler,
            /* [unique][in] */ LPCWSTR pwszDescription,
            /* [in] */ DWORD dwSyncMgrRegisterFlags);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UnregisterSyncMgrHandler )( 
            ISyncMgrRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsidHandler,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetHandlerRegistrationInfo )( 
            ISyncMgrRegister __RPC_FAR * This,
            /* [in] */ REFCLSID rclsidHandler,
            /* [out][in] */ LPDWORD pdwSyncMgrRegisterFlags);
        
        END_INTERFACE
    } ISyncMgrRegisterVtbl;

    interface ISyncMgrRegister
    {
        CONST_VTBL struct ISyncMgrRegisterVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISyncMgrRegister_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISyncMgrRegister_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISyncMgrRegister_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISyncMgrRegister_RegisterSyncMgrHandler(This,rclsidHandler,pwszDescription,dwSyncMgrRegisterFlags)	\
    (This)->lpVtbl -> RegisterSyncMgrHandler(This,rclsidHandler,pwszDescription,dwSyncMgrRegisterFlags)

#define ISyncMgrRegister_UnregisterSyncMgrHandler(This,rclsidHandler,dwReserved)	\
    (This)->lpVtbl -> UnregisterSyncMgrHandler(This,rclsidHandler,dwReserved)

#define ISyncMgrRegister_GetHandlerRegistrationInfo(This,rclsidHandler,pdwSyncMgrRegisterFlags)	\
    (This)->lpVtbl -> GetHandlerRegistrationInfo(This,rclsidHandler,pdwSyncMgrRegisterFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISyncMgrRegister_RegisterSyncMgrHandler_Proxy( 
    ISyncMgrRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsidHandler,
    /* [unique][in] */ LPCWSTR pwszDescription,
    /* [in] */ DWORD dwSyncMgrRegisterFlags);


void __RPC_STUB ISyncMgrRegister_RegisterSyncMgrHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrRegister_UnregisterSyncMgrHandler_Proxy( 
    ISyncMgrRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsidHandler,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB ISyncMgrRegister_UnregisterSyncMgrHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISyncMgrRegister_GetHandlerRegistrationInfo_Proxy( 
    ISyncMgrRegister __RPC_FAR * This,
    /* [in] */ REFCLSID rclsidHandler,
    /* [out][in] */ LPDWORD pdwSyncMgrRegisterFlags);


void __RPC_STUB ISyncMgrRegister_GetHandlerRegistrationInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISyncMgrRegister_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_mobsync_0112 */
/* [local] */ 


// Declarations for Conflict resolution dialog      

// Flags                                            
#define RFCF_APPLY_ALL           0x0001             

// Parameter values for RFCCM_NEEDELEMENT (unimplemented)
#define RFCD_NAME                0x0001             
#define RFCD_KEEPBOTHICON        0x0002             
#define RFCD_KEEPLOCALICON       0x0004             
#define RFCD_KEEPSERVERICON      0x0008             
#define RFCD_NETWORKMODIFIEDBY   0x0010             
#define RFCD_NETWORKMODIFIEDON   0x0020             
#define RFCD_LOCALMODIFIEDBY     0x0040             
#define RFCD_LOCALMODIFIEDON     0x0080             
#define RFCD_NEWNAME             0x0100             
#define RFCD_LOCATION            0x0200             
#define RFCD_ALL                 0x03FF             

// Callback Messages                                
#define RFCCM_VIEWLOCAL          0x0001             
#define RFCCM_VIEWNETWORK        0x0002             
#define RFCCM_NEEDELEMENT        0x0003             

// Return values                                  
#define RFC_CANCEL               0x00             
#define RFC_KEEPBOTH             0x01             
#define RFC_KEEPLOCAL            0x02             
#define RFC_KEEPNETWORK          0x03             
#define RFC_APPLY_TO_ALL         0x10             

typedef BOOL (WINAPI *PFNRFCDCALLBACK)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

typedef struct tagRFCDLGPARAMW {                                                 
    DWORD    dwFlags;               // RFCF_*                                    
    LPCWSTR  pszFilename;           // File name of the file conflicted          
    LPCWSTR  pszLocation;           // Location of the file                      
    LPCWSTR  pszNewName;            // The  new name to give the file            
    LPCWSTR  pszNetworkModifiedBy;  // Name of person who changed the net doc    
    LPCWSTR  pszLocalModifiedBy;    // Name of person who changed the local doc  
    LPCWSTR  pszNetworkModifiedOn;  // When the net doc was changed              
    LPCWSTR  pszLocalModifiedOn;    // When the local doc was changed            
    HICON 	hIKeepBoth;            // Icon                                      
    HICON	hIKeepLocal;           //                                           
    HICON 	hIKeepNetwork;         //                                           
    PFNRFCDCALLBACK pfnCallBack;    // Callback                                  
    LPARAM  lCallerData;            // Place where the caller can keep some context data
} RFCDLGPARAMW;

typedef struct tagRFCDLGPARAMA {
    DWORD   dwFlags;                // RFCF_*                                    
    LPCSTR  pszFilename;            // File name of the file conflicted          
    LPCSTR  pszLocation;            // Location of the file                      
    LPCSTR  pszNewName;             // The  new name to give the file            
    LPCSTR  pszNetworkModifiedBy;   // Name of person who changed the net doc    
    LPCSTR  pszLocalModifiedBy;     // Name of person who changed the local doc  
    LPCSTR  pszNetworkModifiedOn;   // When the net doc was changed              
    LPCSTR  pszLocalModifiedOn;     // When the local doc was changed            
    HICON   hIKeepBoth;             // Icon                                      
    HICON   hIKeepLocal;            //                                           
    HICON   hIKeepNetwork;          //                                           
    PFNRFCDCALLBACK pfnCallBack;    // Callback                                  
    LPARAM  lCallerData;            // Place where the caller can keep some context data
} RFCDLGPARAMA;


int WINAPI SyncMgrResolveConflictW(HWND hWndParent, RFCDLGPARAMW *pdlgParam);
int WINAPI SyncMgrResolveConflictA(HWND hWndParent, RFCDLGPARAMA *pdlgParam);

#ifdef UNICODE                                                       
#define SyncMgrResolveConflict SyncMgrResolveConflictW               
#define RFCDLGPARAM RFCDLGPARAMW                                     
#else                                                                
#define SyncMgrResolveConflict SyncMgrResolveConflictA               
#define RFCDLGPARAM RFCDLGPARAMA                                     
#endif // !UNICODE                                                   



extern RPC_IF_HANDLE __MIDL_itf_mobsync_0112_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mobsync_0112_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


