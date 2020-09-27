
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 5.03.0279 */
/* at Thu Nov 11 19:02:33 1999
 */
/* Compiler settings for mswtecom.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32 (32b run), ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
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

#ifndef __mswtecom_h__
#define __mswtecom_h__

/* Forward Declarations */ 

#ifndef __IWTE_FWD_DEFINED__
#define __IWTE_FWD_DEFINED__
typedef interface IWTE IWTE;
#endif 	/* __IWTE_FWD_DEFINED__ */


#ifndef __IWTEArrays_FWD_DEFINED__
#define __IWTEArrays_FWD_DEFINED__
typedef interface IWTEArrays IWTEArrays;
#endif 	/* __IWTEArrays_FWD_DEFINED__ */


#ifndef __IWTEArray_FWD_DEFINED__
#define __IWTEArray_FWD_DEFINED__
typedef interface IWTEArray IWTEArray;
#endif 	/* __IWTEArray_FWD_DEFINED__ */


#ifndef __IWTEServers_FWD_DEFINED__
#define __IWTEServers_FWD_DEFINED__
typedef interface IWTEServers IWTEServers;
#endif 	/* __IWTEServers_FWD_DEFINED__ */


#ifndef __IWTEServer_FWD_DEFINED__
#define __IWTEServer_FWD_DEFINED__
typedef interface IWTEServer IWTEServer;
#endif 	/* __IWTEServer_FWD_DEFINED__ */


#ifndef __IWTETTSModes_FWD_DEFINED__
#define __IWTETTSModes_FWD_DEFINED__
typedef interface IWTETTSModes IWTETTSModes;
#endif 	/* __IWTETTSModes_FWD_DEFINED__ */


#ifndef __IWTESRModes_FWD_DEFINED__
#define __IWTESRModes_FWD_DEFINED__
typedef interface IWTESRModes IWTESRModes;
#endif 	/* __IWTESRModes_FWD_DEFINED__ */


#ifndef __IWTEAddresses_FWD_DEFINED__
#define __IWTEAddresses_FWD_DEFINED__
typedef interface IWTEAddresses IWTEAddresses;
#endif 	/* __IWTEAddresses_FWD_DEFINED__ */


#ifndef __IWTEAddress_FWD_DEFINED__
#define __IWTEAddress_FWD_DEFINED__
typedef interface IWTEAddress IWTEAddress;
#endif 	/* __IWTEAddress_FWD_DEFINED__ */


#ifndef __IWTEAddressGroups_FWD_DEFINED__
#define __IWTEAddressGroups_FWD_DEFINED__
typedef interface IWTEAddressGroups IWTEAddressGroups;
#endif 	/* __IWTEAddressGroups_FWD_DEFINED__ */


#ifndef __IWTEAddressGroup_FWD_DEFINED__
#define __IWTEAddressGroup_FWD_DEFINED__
typedef interface IWTEAddressGroup IWTEAddressGroup;
#endif 	/* __IWTEAddressGroup_FWD_DEFINED__ */


#ifndef __IWTEDNISes_FWD_DEFINED__
#define __IWTEDNISes_FWD_DEFINED__
typedef interface IWTEDNISes IWTEDNISes;
#endif 	/* __IWTEDNISes_FWD_DEFINED__ */


#ifndef __IWTEDNIS_FWD_DEFINED__
#define __IWTEDNIS_FWD_DEFINED__
typedef interface IWTEDNIS IWTEDNIS;
#endif 	/* __IWTEDNIS_FWD_DEFINED__ */


#ifndef __IWTEVendorDataSets_FWD_DEFINED__
#define __IWTEVendorDataSets_FWD_DEFINED__
typedef interface IWTEVendorDataSets IWTEVendorDataSets;
#endif 	/* __IWTEVendorDataSets_FWD_DEFINED__ */


#ifndef __IWTEVendorDataSet_FWD_DEFINED__
#define __IWTEVendorDataSet_FWD_DEFINED__
typedef interface IWTEVendorDataSet IWTEVendorDataSet;
#endif 	/* __IWTEVendorDataSet_FWD_DEFINED__ */


#ifndef __IWTELog_FWD_DEFINED__
#define __IWTELog_FWD_DEFINED__
typedef interface IWTELog IWTELog;
#endif 	/* __IWTELog_FWD_DEFINED__ */


#ifndef __IWTEApplications_FWD_DEFINED__
#define __IWTEApplications_FWD_DEFINED__
typedef interface IWTEApplications IWTEApplications;
#endif 	/* __IWTEApplications_FWD_DEFINED__ */


#ifndef __IWTEApplication_FWD_DEFINED__
#define __IWTEApplication_FWD_DEFINED__
typedef interface IWTEApplication IWTEApplication;
#endif 	/* __IWTEApplication_FWD_DEFINED__ */


#ifndef __IWTEStepConstrains_FWD_DEFINED__
#define __IWTEStepConstrains_FWD_DEFINED__
typedef interface IWTEStepConstrains IWTEStepConstrains;
#endif 	/* __IWTEStepConstrains_FWD_DEFINED__ */


#ifndef __IWTESession_FWD_DEFINED__
#define __IWTESession_FWD_DEFINED__
typedef interface IWTESession IWTESession;
#endif 	/* __IWTESession_FWD_DEFINED__ */


#ifndef __IWTETTSMode_FWD_DEFINED__
#define __IWTETTSMode_FWD_DEFINED__
typedef interface IWTETTSMode IWTETTSMode;
#endif 	/* __IWTETTSMode_FWD_DEFINED__ */


#ifndef __IWTESRMode_FWD_DEFINED__
#define __IWTESRMode_FWD_DEFINED__
typedef interface IWTESRMode IWTESRMode;
#endif 	/* __IWTESRMode_FWD_DEFINED__ */


#ifndef __IWTEStep_FWD_DEFINED__
#define __IWTEStep_FWD_DEFINED__
typedef interface IWTEStep IWTEStep;
#endif 	/* __IWTEStep_FWD_DEFINED__ */


#ifndef __IWTELogCustomField_FWD_DEFINED__
#define __IWTELogCustomField_FWD_DEFINED__
typedef interface IWTELogCustomField IWTELogCustomField;
#endif 	/* __IWTELogCustomField_FWD_DEFINED__ */


#ifndef __IWTEExOM_FWD_DEFINED__
#define __IWTEExOM_FWD_DEFINED__
typedef interface IWTEExOM IWTEExOM;
#endif 	/* __IWTEExOM_FWD_DEFINED__ */


#ifndef __IWTEActiveConnection_FWD_DEFINED__
#define __IWTEActiveConnection_FWD_DEFINED__
typedef interface IWTEActiveConnection IWTEActiveConnection;
#endif 	/* __IWTEActiveConnection_FWD_DEFINED__ */


#ifndef __IWTEService_FWD_DEFINED__
#define __IWTEService_FWD_DEFINED__
typedef interface IWTEService IWTEService;
#endif 	/* __IWTEService_FWD_DEFINED__ */


#ifndef __IWTESnapinNode_FWD_DEFINED__
#define __IWTESnapinNode_FWD_DEFINED__
typedef interface IWTESnapinNode IWTESnapinNode;
#endif 	/* __IWTESnapinNode_FWD_DEFINED__ */


#ifndef __WTE_FWD_DEFINED__
#define __WTE_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTE WTE;
#else
typedef struct WTE WTE;
#endif /* __cplusplus */

#endif 	/* __WTE_FWD_DEFINED__ */


#ifndef __WTEArray_FWD_DEFINED__
#define __WTEArray_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEArray WTEArray;
#else
typedef struct WTEArray WTEArray;
#endif /* __cplusplus */

#endif 	/* __WTEArray_FWD_DEFINED__ */


#ifndef __WTEArrays_FWD_DEFINED__
#define __WTEArrays_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEArrays WTEArrays;
#else
typedef struct WTEArrays WTEArrays;
#endif /* __cplusplus */

#endif 	/* __WTEArrays_FWD_DEFINED__ */


#ifndef __WTEServers_FWD_DEFINED__
#define __WTEServers_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEServers WTEServers;
#else
typedef struct WTEServers WTEServers;
#endif /* __cplusplus */

#endif 	/* __WTEServers_FWD_DEFINED__ */


#ifndef __WTEServer_FWD_DEFINED__
#define __WTEServer_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEServer WTEServer;
#else
typedef struct WTEServer WTEServer;
#endif /* __cplusplus */

#endif 	/* __WTEServer_FWD_DEFINED__ */


#ifndef __WTEAddresses_FWD_DEFINED__
#define __WTEAddresses_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEAddresses WTEAddresses;
#else
typedef struct WTEAddresses WTEAddresses;
#endif /* __cplusplus */

#endif 	/* __WTEAddresses_FWD_DEFINED__ */


#ifndef __WTEAddress_FWD_DEFINED__
#define __WTEAddress_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEAddress WTEAddress;
#else
typedef struct WTEAddress WTEAddress;
#endif /* __cplusplus */

#endif 	/* __WTEAddress_FWD_DEFINED__ */


#ifndef __WTEAddressGroups_FWD_DEFINED__
#define __WTEAddressGroups_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEAddressGroups WTEAddressGroups;
#else
typedef struct WTEAddressGroups WTEAddressGroups;
#endif /* __cplusplus */

#endif 	/* __WTEAddressGroups_FWD_DEFINED__ */


#ifndef __WTEAddressGroup_FWD_DEFINED__
#define __WTEAddressGroup_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEAddressGroup WTEAddressGroup;
#else
typedef struct WTEAddressGroup WTEAddressGroup;
#endif /* __cplusplus */

#endif 	/* __WTEAddressGroup_FWD_DEFINED__ */


#ifndef __WTEDNISes_FWD_DEFINED__
#define __WTEDNISes_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEDNISes WTEDNISes;
#else
typedef struct WTEDNISes WTEDNISes;
#endif /* __cplusplus */

#endif 	/* __WTEDNISes_FWD_DEFINED__ */


#ifndef __WTEDNIS_FWD_DEFINED__
#define __WTEDNIS_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEDNIS WTEDNIS;
#else
typedef struct WTEDNIS WTEDNIS;
#endif /* __cplusplus */

#endif 	/* __WTEDNIS_FWD_DEFINED__ */


#ifndef __IWTEVendorDataChanges_FWD_DEFINED__
#define __IWTEVendorDataChanges_FWD_DEFINED__
typedef interface IWTEVendorDataChanges IWTEVendorDataChanges;
#endif 	/* __IWTEVendorDataChanges_FWD_DEFINED__ */


#ifndef __WTEVendorDataSets_FWD_DEFINED__
#define __WTEVendorDataSets_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEVendorDataSets WTEVendorDataSets;
#else
typedef struct WTEVendorDataSets WTEVendorDataSets;
#endif /* __cplusplus */

#endif 	/* __WTEVendorDataSets_FWD_DEFINED__ */


#ifndef __WTEVendorDataSet_FWD_DEFINED__
#define __WTEVendorDataSet_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEVendorDataSet WTEVendorDataSet;
#else
typedef struct WTEVendorDataSet WTEVendorDataSet;
#endif /* __cplusplus */

#endif 	/* __WTEVendorDataSet_FWD_DEFINED__ */


#ifndef __WTELog_FWD_DEFINED__
#define __WTELog_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTELog WTELog;
#else
typedef struct WTELog WTELog;
#endif /* __cplusplus */

#endif 	/* __WTELog_FWD_DEFINED__ */


#ifndef __WTEApplications_FWD_DEFINED__
#define __WTEApplications_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEApplications WTEApplications;
#else
typedef struct WTEApplications WTEApplications;
#endif /* __cplusplus */

#endif 	/* __WTEApplications_FWD_DEFINED__ */


#ifndef __WTEApplication_FWD_DEFINED__
#define __WTEApplication_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEApplication WTEApplication;
#else
typedef struct WTEApplication WTEApplication;
#endif /* __cplusplus */

#endif 	/* __WTEApplication_FWD_DEFINED__ */


#ifndef __IWTEConnectionEvents_FWD_DEFINED__
#define __IWTEConnectionEvents_FWD_DEFINED__
typedef interface IWTEConnectionEvents IWTEConnectionEvents;
#endif 	/* __IWTEConnectionEvents_FWD_DEFINED__ */


#ifndef __WTEService_FWD_DEFINED__
#define __WTEService_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEService WTEService;
#else
typedef struct WTEService WTEService;
#endif /* __cplusplus */

#endif 	/* __WTEService_FWD_DEFINED__ */


#ifndef __WTEActiveConnection_FWD_DEFINED__
#define __WTEActiveConnection_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEActiveConnection WTEActiveConnection;
#else
typedef struct WTEActiveConnection WTEActiveConnection;
#endif /* __cplusplus */

#endif 	/* __WTEActiveConnection_FWD_DEFINED__ */


#ifndef __WTEStep_FWD_DEFINED__
#define __WTEStep_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEStep WTEStep;
#else
typedef struct WTEStep WTEStep;
#endif /* __cplusplus */

#endif 	/* __WTEStep_FWD_DEFINED__ */


#ifndef __WTEStepConstrains_FWD_DEFINED__
#define __WTEStepConstrains_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEStepConstrains WTEStepConstrains;
#else
typedef struct WTEStepConstrains WTEStepConstrains;
#endif /* __cplusplus */

#endif 	/* __WTEStepConstrains_FWD_DEFINED__ */


#ifndef __WTESession_FWD_DEFINED__
#define __WTESession_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTESession WTESession;
#else
typedef struct WTESession WTESession;
#endif /* __cplusplus */

#endif 	/* __WTESession_FWD_DEFINED__ */


#ifndef __WTETTSModes_FWD_DEFINED__
#define __WTETTSModes_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTETTSModes WTETTSModes;
#else
typedef struct WTETTSModes WTETTSModes;
#endif /* __cplusplus */

#endif 	/* __WTETTSModes_FWD_DEFINED__ */


#ifndef __WTESRModes_FWD_DEFINED__
#define __WTESRModes_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTESRModes WTESRModes;
#else
typedef struct WTESRModes WTESRModes;
#endif /* __cplusplus */

#endif 	/* __WTESRModes_FWD_DEFINED__ */


#ifndef __WTETTSMode_FWD_DEFINED__
#define __WTETTSMode_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTETTSMode WTETTSMode;
#else
typedef struct WTETTSMode WTETTSMode;
#endif /* __cplusplus */

#endif 	/* __WTETTSMode_FWD_DEFINED__ */


#ifndef __WTESRMode_FWD_DEFINED__
#define __WTESRMode_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTESRMode WTESRMode;
#else
typedef struct WTESRMode WTESRMode;
#endif /* __cplusplus */

#endif 	/* __WTESRMode_FWD_DEFINED__ */


#ifndef __WTEExOM_FWD_DEFINED__
#define __WTEExOM_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTEExOM WTEExOM;
#else
typedef struct WTEExOM WTEExOM;
#endif /* __cplusplus */

#endif 	/* __WTEExOM_FWD_DEFINED__ */


#ifndef __WTELogCustomField_FWD_DEFINED__
#define __WTELogCustomField_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTELogCustomField WTELogCustomField;
#else
typedef struct WTELogCustomField WTELogCustomField;
#endif /* __cplusplus */

#endif 	/* __WTELogCustomField_FWD_DEFINED__ */


#ifndef __WTESnapinNode_FWD_DEFINED__
#define __WTESnapinNode_FWD_DEFINED__

#ifdef __cplusplus
typedef class WTESnapinNode WTESnapinNode;
#else
typedef struct WTESnapinNode WTESnapinNode;
#endif /* __cplusplus */

#endif 	/* __WTESnapinNode_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "tapi3if.h"
#include "mswteImports.h"

#ifdef __cplusplus
extern "C"{
#endif 

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/* interface __MIDL_itf_mswtecom_0000 */
/* [local] */ 






















extern RPC_IF_HANDLE __MIDL_itf_mswtecom_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_mswtecom_0000_v0_0_s_ifspec;

#ifndef __IWTE_INTERFACE_DEFINED__
#define __IWTE_INTERFACE_DEFINED__

/* interface IWTE */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTE;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8310-336C-11D3-8657-0090272F9EEB")
    IWTE : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Arrays( 
            /* [retval][out] */ IWTEArrays __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTE __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTE __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTE __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTE __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTE __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTE __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTE __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Arrays )( 
            IWTE __RPC_FAR * This,
            /* [retval][out] */ IWTEArrays __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTE __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTE __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTE __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEVtbl;

    interface IWTE
    {
        CONST_VTBL struct IWTEVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTE_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTE_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTE_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTE_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTE_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTE_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTE_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTE_get_Arrays(This,pVal)	\
    (This)->lpVtbl -> get_Arrays(This,pVal)

#define IWTE_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTE_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTE_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTE_get_Arrays_Proxy( 
    IWTE __RPC_FAR * This,
    /* [retval][out] */ IWTEArrays __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTE_get_Arrays_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTE_Save_Proxy( 
    IWTE __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTE_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTE_Refresh_Proxy( 
    IWTE __RPC_FAR * This);


void __RPC_STUB IWTE_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTE_get_VendorDataSets_Proxy( 
    IWTE __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTE_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTE_INTERFACE_DEFINED__ */


#ifndef __IWTEArrays_INTERFACE_DEFINED__
#define __IWTEArrays_INTERFACE_DEFINED__

/* interface IWTEArrays */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEArrays;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8311-336C-11D3-8657-0090272F9EEB")
    IWTEArrays : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetContainingArray( 
            /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            VARIANT v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEArraysVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEArrays __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEArrays __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEArrays __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEArrays __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEArrays __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEArrays __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEArrays __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEArrays __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEArrays __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEArrays __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEArrays __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEArrays __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetContainingArray )( 
            IWTEArrays __RPC_FAR * This,
            /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEArrays __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IWTEArrays __RPC_FAR * This,
            VARIANT v);
        
        END_INTERFACE
    } IWTEArraysVtbl;

    interface IWTEArrays
    {
        CONST_VTBL struct IWTEArraysVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEArrays_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEArrays_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEArrays_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEArrays_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEArrays_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEArrays_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEArrays_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEArrays_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEArrays_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEArrays_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEArrays_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEArrays_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEArrays_GetContainingArray(This,pItem)	\
    (This)->lpVtbl -> GetContainingArray(This,pItem)

#define IWTEArrays_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEArrays_Remove(This,v)	\
    (This)->lpVtbl -> Remove(This,v)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEArrays_Save_Proxy( 
    IWTEArrays __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEArrays_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEArrays_Refresh_Proxy( 
    IWTEArrays __RPC_FAR * This);


void __RPC_STUB IWTEArrays_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEArrays_get_Count_Proxy( 
    IWTEArrays __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEArrays_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEArrays_Item_Proxy( 
    IWTEArrays __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEArrays_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEArrays_get__NewEnum_Proxy( 
    IWTEArrays __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArrays_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEArrays_GetContainingArray_Proxy( 
    IWTEArrays __RPC_FAR * This,
    /* [retval][out] */ IWTEArray __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEArrays_GetContainingArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEArrays_get_VendorDataSets_Proxy( 
    IWTEArrays __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArrays_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEArrays_Remove_Proxy( 
    IWTEArrays __RPC_FAR * This,
    VARIANT v);


void __RPC_STUB IWTEArrays_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEArrays_INTERFACE_DEFINED__ */


#ifndef __IWTEArray_INTERFACE_DEFINED__
#define __IWTEArray_INTERFACE_DEFINED__

/* interface IWTEArray */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEArray;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8312-336C-11D3-8657-0090272F9EEB")
    IWTEArray : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Servers( 
            /* [retval][out] */ IWTEServers __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Applications( 
            /* [retval][out] */ IWTEApplications __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_AddressGroups( 
            /* [retval][out] */ IWTEAddressGroups __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_Log( 
            /* [retval][out] */ IWTELog __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEArrayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEArray __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEArray __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEArray __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEArray __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Servers )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ IWTEServers __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Applications )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ IWTEApplications __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AddressGroups )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ IWTEAddressGroups __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Log )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ IWTELog __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEArray __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEArray __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEArray __RPC_FAR * This);
        
        END_INTERFACE
    } IWTEArrayVtbl;

    interface IWTEArray
    {
        CONST_VTBL struct IWTEArrayVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEArray_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEArray_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEArray_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEArray_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEArray_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEArray_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEArray_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEArray_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IWTEArray_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IWTEArray_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IWTEArray_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IWTEArray_get_Servers(This,pVal)	\
    (This)->lpVtbl -> get_Servers(This,pVal)

#define IWTEArray_get_Applications(This,pVal)	\
    (This)->lpVtbl -> get_Applications(This,pVal)

#define IWTEArray_get_AddressGroups(This,pVal)	\
    (This)->lpVtbl -> get_AddressGroups(This,pVal)

#define IWTEArray_get_Log(This,pVal)	\
    (This)->lpVtbl -> get_Log(This,pVal)

#define IWTEArray_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEArray_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEArray_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_Name_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEArray_put_Name_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEArray_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_Description_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEArray_put_Description_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEArray_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_Servers_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ IWTEServers __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_Servers_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_Applications_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ IWTEApplications __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_Applications_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_AddressGroups_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ IWTEAddressGroups __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_AddressGroups_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_Log_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ IWTELog __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_Log_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IWTEArray_get_VendorDataSets_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEArray_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWTEArray_Save_Proxy( 
    IWTEArray __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEArray_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWTEArray_Refresh_Proxy( 
    IWTEArray __RPC_FAR * This);


void __RPC_STUB IWTEArray_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEArray_INTERFACE_DEFINED__ */


#ifndef __IWTEServers_INTERFACE_DEFINED__
#define __IWTEServers_INTERFACE_DEFINED__

/* interface IWTEServers */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEServers;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8313-336C-11D3-8657-0090272F9EEB")
    IWTEServers : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEServer __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEServersVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEServers __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEServers __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEServers __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEServers __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEServers __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEServers __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEServers __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEServers __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEServers __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEServers __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEServer __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEServers __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEServers __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEServersVtbl;

    interface IWTEServers
    {
        CONST_VTBL struct IWTEServersVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEServers_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEServers_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEServers_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEServers_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEServers_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEServers_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEServers_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEServers_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEServers_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEServers_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEServers_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEServers_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEServers_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServers_get_Count_Proxy( 
    IWTEServers __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEServers_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEServers_Save_Proxy( 
    IWTEServers __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEServers_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEServers_Refresh_Proxy( 
    IWTEServers __RPC_FAR * This);


void __RPC_STUB IWTEServers_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEServers_Item_Proxy( 
    IWTEServers __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEServer __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEServers_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServers_get__NewEnum_Proxy( 
    IWTEServers __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServers_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServers_get_VendorDataSets_Proxy( 
    IWTEServers __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServers_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEServers_INTERFACE_DEFINED__ */


#ifndef __IWTEServer_INTERFACE_DEFINED__
#define __IWTEServer_INTERFACE_DEFINED__

/* interface IWTEServer */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEServer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8314-336C-11D3-8657-0090272F9EEB")
    IWTEServer : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductVersion( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Addresses( 
            /* [retval][out] */ IWTEAddresses __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TTSModes( 
            /* [retval][out] */ IWTETTSModes __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SRModes( 
            /* [retval][out] */ IWTESRModes __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEServerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEServer __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEServer __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEServer __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEServer __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEServer __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEServer __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProductVersion )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEServer __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Addresses )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ IWTEAddresses __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TTSModes )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ IWTETTSModes __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SRModes )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ IWTESRModes __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEServer __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEServer __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEServer __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEServerVtbl;

    interface IWTEServer
    {
        CONST_VTBL struct IWTEServerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEServer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEServer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEServer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEServer_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEServer_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEServer_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEServer_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEServer_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IWTEServer_get_ProductVersion(This,pVal)	\
    (This)->lpVtbl -> get_ProductVersion(This,pVal)

#define IWTEServer_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IWTEServer_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IWTEServer_get_Addresses(This,pVal)	\
    (This)->lpVtbl -> get_Addresses(This,pVal)

#define IWTEServer_get_TTSModes(This,pVal)	\
    (This)->lpVtbl -> get_TTSModes(This,pVal)

#define IWTEServer_get_SRModes(This,pVal)	\
    (This)->lpVtbl -> get_SRModes(This,pVal)

#define IWTEServer_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEServer_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEServer_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_Name_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_ProductVersion_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_ProductVersion_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_Description_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEServer_put_Description_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEServer_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_Addresses_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ IWTEAddresses __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_Addresses_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_TTSModes_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ IWTETTSModes __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_TTSModes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_SRModes_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ IWTESRModes __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_SRModes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEServer_Save_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEServer_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEServer_Refresh_Proxy( 
    IWTEServer __RPC_FAR * This);


void __RPC_STUB IWTEServer_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEServer_get_VendorDataSets_Proxy( 
    IWTEServer __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEServer_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEServer_INTERFACE_DEFINED__ */


#ifndef __IWTETTSModes_INTERFACE_DEFINED__
#define __IWTETTSModes_INTERFACE_DEFINED__

/* interface IWTETTSModes */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTETTSModes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("23383000-6c46-11d3-8661-0090272f9eeb")
    IWTETTSModes : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTETTSModesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTETTSModes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTETTSModes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTETTSModes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTETTSModes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTETTSModes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTETTSModes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTETTSModes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTETTSModes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTETTSModes __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTETTSModes __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTETTSModes __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTETTSModes __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTETTSModesVtbl;

    interface IWTETTSModes
    {
        CONST_VTBL struct IWTETTSModesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTETTSModes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTETTSModes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTETTSModes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTETTSModes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTETTSModes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTETTSModes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTETTSModes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTETTSModes_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTETTSModes_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTETTSModes_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTETTSModes_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTETTSModes_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSModes_get_Count_Proxy( 
    IWTETTSModes __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSModes_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTETTSModes_Refresh_Proxy( 
    IWTETTSModes __RPC_FAR * This);


void __RPC_STUB IWTETTSModes_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTETTSModes_Item_Proxy( 
    IWTETTSModes __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTETTSModes_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSModes_get__NewEnum_Proxy( 
    IWTETTSModes __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTETTSModes_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSModes_get_VendorDataSets_Proxy( 
    IWTETTSModes __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTETTSModes_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTETTSModes_INTERFACE_DEFINED__ */


#ifndef __IWTESRModes_INTERFACE_DEFINED__
#define __IWTESRModes_INTERFACE_DEFINED__

/* interface IWTESRModes */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTESRModes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a0293ef0-6e66-11d3-8661-0090272f9eeb")
    IWTESRModes : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTESRModesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTESRModes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTESRModes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTESRModes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTESRModes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTESRModes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTESRModes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTESRModes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTESRModes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTESRModes __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTESRModes __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTESRModes __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTESRModes __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTESRModesVtbl;

    interface IWTESRModes
    {
        CONST_VTBL struct IWTESRModesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTESRModes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTESRModes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTESRModes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTESRModes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTESRModes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTESRModes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTESRModes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTESRModes_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTESRModes_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTESRModes_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTESRModes_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTESRModes_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRModes_get_Count_Proxy( 
    IWTESRModes __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRModes_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTESRModes_Refresh_Proxy( 
    IWTESRModes __RPC_FAR * This);


void __RPC_STUB IWTESRModes_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTESRModes_Item_Proxy( 
    IWTESRModes __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTESRModes_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRModes_get__NewEnum_Proxy( 
    IWTESRModes __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTESRModes_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRModes_get_VendorDataSets_Proxy( 
    IWTESRModes __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTESRModes_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTESRModes_INTERFACE_DEFINED__ */


#ifndef __IWTEAddresses_INTERFACE_DEFINED__
#define __IWTEAddresses_INTERFACE_DEFINED__

/* interface IWTEAddresses */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEAddresses;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8315-336C-11D3-8657-0090272F9EEB")
    IWTEAddresses : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEAddress __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEAddressesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEAddresses __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEAddresses __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEAddresses __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEAddresses __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEAddresses __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEAddresses __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEAddresses __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEAddresses __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEAddresses __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEAddresses __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEAddresses __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEAddress __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEAddresses __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEAddresses __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEAddressesVtbl;

    interface IWTEAddresses
    {
        CONST_VTBL struct IWTEAddressesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEAddresses_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEAddresses_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEAddresses_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEAddresses_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEAddresses_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEAddresses_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEAddresses_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEAddresses_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEAddresses_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEAddresses_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEAddresses_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEAddresses_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEAddresses_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_get_Count_Proxy( 
    IWTEAddresses __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEAddresses_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_Save_Proxy( 
    IWTEAddresses __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEAddresses_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_Refresh_Proxy( 
    IWTEAddresses __RPC_FAR * This);


void __RPC_STUB IWTEAddresses_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_Item_Proxy( 
    IWTEAddresses __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEAddress __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEAddresses_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_get__NewEnum_Proxy( 
    IWTEAddresses __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddresses_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddresses_get_VendorDataSets_Proxy( 
    IWTEAddresses __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddresses_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEAddresses_INTERFACE_DEFINED__ */


#ifndef __IWTEAddress_INTERFACE_DEFINED__
#define __IWTEAddress_INTERFACE_DEFINED__

/* interface IWTEAddress */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEAddress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8316-336C-11D3-8657-0090272F9EEB")
    IWTEAddress : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SupportsCalledID( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DialableAddress( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AssignedApplication( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AssignedApplication( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AssignedGroup( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AssignedGroup( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEAddressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEAddress __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEAddress __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEAddress __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SupportsCalledID )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DialableAddress )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AssignedApplication )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AssignedApplication )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AssignedGroup )( 
            IWTEAddress __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AssignedGroup )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEAddress __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEAddress __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEAddress __RPC_FAR * This);
        
        END_INTERFACE
    } IWTEAddressVtbl;

    interface IWTEAddress
    {
        CONST_VTBL struct IWTEAddressVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEAddress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEAddress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEAddress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEAddress_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEAddress_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEAddress_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEAddress_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEAddress_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IWTEAddress_get_SupportsCalledID(This,pVal)	\
    (This)->lpVtbl -> get_SupportsCalledID(This,pVal)

#define IWTEAddress_get_DialableAddress(This,pVal)	\
    (This)->lpVtbl -> get_DialableAddress(This,pVal)

#define IWTEAddress_put_AssignedApplication(This,newVal)	\
    (This)->lpVtbl -> put_AssignedApplication(This,newVal)

#define IWTEAddress_get_AssignedApplication(This,pVal)	\
    (This)->lpVtbl -> get_AssignedApplication(This,pVal)

#define IWTEAddress_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IWTEAddress_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IWTEAddress_put_AssignedGroup(This,newVal)	\
    (This)->lpVtbl -> put_AssignedGroup(This,newVal)

#define IWTEAddress_get_AssignedGroup(This,pVal)	\
    (This)->lpVtbl -> get_AssignedGroup(This,pVal)

#define IWTEAddress_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEAddress_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEAddress_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_Name_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_SupportsCalledID_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_SupportsCalledID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_DialableAddress_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_DialableAddress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEAddress_put_AssignedApplication_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEAddress_put_AssignedApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_AssignedApplication_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_AssignedApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_Description_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEAddress_put_Description_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEAddress_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEAddress_put_AssignedGroup_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEAddress_put_AssignedGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_AssignedGroup_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_AssignedGroup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddress_get_VendorDataSets_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddress_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddress_Save_Proxy( 
    IWTEAddress __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEAddress_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddress_Refresh_Proxy( 
    IWTEAddress __RPC_FAR * This);


void __RPC_STUB IWTEAddress_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEAddress_INTERFACE_DEFINED__ */


#ifndef __IWTEAddressGroups_INTERFACE_DEFINED__
#define __IWTEAddressGroups_INTERFACE_DEFINED__

/* interface IWTEAddressGroups */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEAddressGroups;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("667d6160-5a22-11d3-865d-0090272f9eeb")
    IWTEAddressGroups : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *ppNew) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            VARIANT v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEAddressGroupsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEAddressGroups __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEAddressGroups __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEAddressGroups __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEAddressGroups __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IWTEAddressGroups __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *ppNew);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IWTEAddressGroups __RPC_FAR * This,
            VARIANT v);
        
        END_INTERFACE
    } IWTEAddressGroupsVtbl;

    interface IWTEAddressGroups
    {
        CONST_VTBL struct IWTEAddressGroupsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEAddressGroups_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEAddressGroups_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEAddressGroups_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEAddressGroups_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEAddressGroups_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEAddressGroups_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEAddressGroups_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEAddressGroups_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEAddressGroups_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEAddressGroups_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEAddressGroups_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEAddressGroups_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEAddressGroups_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEAddressGroups_Add(This,Name,ppNew)	\
    (This)->lpVtbl -> Add(This,Name,ppNew)

#define IWTEAddressGroups_Remove(This,v)	\
    (This)->lpVtbl -> Remove(This,v)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_get_Count_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEAddressGroups_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_Save_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEAddressGroups_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_Refresh_Proxy( 
    IWTEAddressGroups __RPC_FAR * This);


void __RPC_STUB IWTEAddressGroups_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_Item_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEAddressGroups_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_get__NewEnum_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddressGroups_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_get_VendorDataSets_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddressGroups_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_Add_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ IWTEAddressGroup __RPC_FAR *__RPC_FAR *ppNew);


void __RPC_STUB IWTEAddressGroups_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroups_Remove_Proxy( 
    IWTEAddressGroups __RPC_FAR * This,
    VARIANT v);


void __RPC_STUB IWTEAddressGroups_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEAddressGroups_INTERFACE_DEFINED__ */


#ifndef __IWTEAddressGroup_INTERFACE_DEFINED__
#define __IWTEAddressGroup_INTERFACE_DEFINED__

/* interface IWTEAddressGroup */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEAddressGroup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("680b55a0-5a27-11d3-865d-0090272f9eeb")
    IWTEAddressGroup : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR Description) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DNISes( 
            /* [retval][out] */ IWTEDNISes __RPC_FAR *__RPC_FAR *ppVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEAddressGroupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEAddressGroup __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEAddressGroup __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [in] */ BSTR Description);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DNISes )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [retval][out] */ IWTEDNISes __RPC_FAR *__RPC_FAR *ppVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEAddressGroup __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEAddressGroup __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEAddressGroupVtbl;

    interface IWTEAddressGroup
    {
        CONST_VTBL struct IWTEAddressGroupVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEAddressGroup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEAddressGroup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEAddressGroup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEAddressGroup_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEAddressGroup_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEAddressGroup_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEAddressGroup_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEAddressGroup_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IWTEAddressGroup_put_Name(This,Name)	\
    (This)->lpVtbl -> put_Name(This,Name)

#define IWTEAddressGroup_get_Description(This,pDescription)	\
    (This)->lpVtbl -> get_Description(This,pDescription)

#define IWTEAddressGroup_put_Description(This,Description)	\
    (This)->lpVtbl -> put_Description(This,Description)

#define IWTEAddressGroup_get_DNISes(This,ppVal)	\
    (This)->lpVtbl -> get_DNISes(This,ppVal)

#define IWTEAddressGroup_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEAddressGroup_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEAddressGroup_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_get_Name_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IWTEAddressGroup_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_put_Name_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IWTEAddressGroup_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_get_Description_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pDescription);


void __RPC_STUB IWTEAddressGroup_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_put_Description_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [in] */ BSTR Description);


void __RPC_STUB IWTEAddressGroup_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_get_DNISes_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [retval][out] */ IWTEDNISes __RPC_FAR *__RPC_FAR *ppVal);


void __RPC_STUB IWTEAddressGroup_get_DNISes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_Save_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEAddressGroup_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_Refresh_Proxy( 
    IWTEAddressGroup __RPC_FAR * This);


void __RPC_STUB IWTEAddressGroup_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEAddressGroup_get_VendorDataSets_Proxy( 
    IWTEAddressGroup __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEAddressGroup_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEAddressGroup_INTERFACE_DEFINED__ */


#ifndef __IWTEDNISes_INTERFACE_DEFINED__
#define __IWTEDNISes_INTERFACE_DEFINED__

/* interface IWTEDNISes */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEDNISes;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("36776540-5a29-11d3-865d-0090272f9eeb")
    IWTEDNISes : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR DialedNumberIdentification,
            /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *ppNew) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            VARIANT v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEDNISesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEDNISes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEDNISes __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEDNISes __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEDNISes __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEDNISes __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEDNISes __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEDNISes __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEDNISes __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEDNISes __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEDNISes __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEDNISes __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEDNISes __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEDNISes __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IWTEDNISes __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR DialedNumberIdentification,
            /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *ppNew);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IWTEDNISes __RPC_FAR * This,
            VARIANT v);
        
        END_INTERFACE
    } IWTEDNISesVtbl;

    interface IWTEDNISes
    {
        CONST_VTBL struct IWTEDNISesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEDNISes_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEDNISes_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEDNISes_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEDNISes_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEDNISes_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEDNISes_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEDNISes_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEDNISes_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEDNISes_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEDNISes_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEDNISes_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEDNISes_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEDNISes_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEDNISes_Add(This,Name,DialedNumberIdentification,ppNew)	\
    (This)->lpVtbl -> Add(This,Name,DialedNumberIdentification,ppNew)

#define IWTEDNISes_Remove(This,v)	\
    (This)->lpVtbl -> Remove(This,v)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_get_Count_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEDNISes_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_Save_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEDNISes_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_Refresh_Proxy( 
    IWTEDNISes __RPC_FAR * This);


void __RPC_STUB IWTEDNISes_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_Item_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEDNISes_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_get__NewEnum_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEDNISes_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_get_VendorDataSets_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEDNISes_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_Add_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR DialedNumberIdentification,
    /* [retval][out] */ IWTEDNIS __RPC_FAR *__RPC_FAR *ppNew);


void __RPC_STUB IWTEDNISes_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNISes_Remove_Proxy( 
    IWTEDNISes __RPC_FAR * This,
    VARIANT v);


void __RPC_STUB IWTEDNISes_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEDNISes_INTERFACE_DEFINED__ */


#ifndef __IWTEDNIS_INTERFACE_DEFINED__
#define __IWTEDNIS_INTERFACE_DEFINED__

/* interface IWTEDNIS */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEDNIS;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("02468d40-5a2e-11d3-865d-0090272f9eeb")
    IWTEDNIS : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pDescription) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR Description) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DialedNumberIdentification( 
            /* [retval][out] */ BSTR __RPC_FAR *pDialedNumberIdentification) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DialedNumberIdentification( 
            /* [in] */ BSTR DialedNumberIdentification) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_AssignedApplication( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AssignedApplication( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEDNISVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEDNIS __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEDNIS __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEDNIS __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEDNIS __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEDNIS __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pDescription);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ BSTR Description);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DialedNumberIdentification )( 
            IWTEDNIS __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pDialedNumberIdentification);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DialedNumberIdentification )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ BSTR DialedNumberIdentification);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_AssignedApplication )( 
            IWTEDNIS __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AssignedApplication )( 
            IWTEDNIS __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEDNIS __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEDNIS __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEDNIS __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEDNISVtbl;

    interface IWTEDNIS
    {
        CONST_VTBL struct IWTEDNISVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEDNIS_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEDNIS_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEDNIS_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEDNIS_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEDNIS_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEDNIS_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEDNIS_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEDNIS_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IWTEDNIS_put_Name(This,Name)	\
    (This)->lpVtbl -> put_Name(This,Name)

#define IWTEDNIS_get_Description(This,pDescription)	\
    (This)->lpVtbl -> get_Description(This,pDescription)

#define IWTEDNIS_put_Description(This,Description)	\
    (This)->lpVtbl -> put_Description(This,Description)

#define IWTEDNIS_get_DialedNumberIdentification(This,pDialedNumberIdentification)	\
    (This)->lpVtbl -> get_DialedNumberIdentification(This,pDialedNumberIdentification)

#define IWTEDNIS_put_DialedNumberIdentification(This,DialedNumberIdentification)	\
    (This)->lpVtbl -> put_DialedNumberIdentification(This,DialedNumberIdentification)

#define IWTEDNIS_put_AssignedApplication(This,newVal)	\
    (This)->lpVtbl -> put_AssignedApplication(This,newVal)

#define IWTEDNIS_get_AssignedApplication(This,pVal)	\
    (This)->lpVtbl -> get_AssignedApplication(This,pVal)

#define IWTEDNIS_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEDNIS_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEDNIS_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_get_Name_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IWTEDNIS_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_put_Name_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IWTEDNIS_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_get_Description_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pDescription);


void __RPC_STUB IWTEDNIS_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_put_Description_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [in] */ BSTR Description);


void __RPC_STUB IWTEDNIS_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_get_DialedNumberIdentification_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pDialedNumberIdentification);


void __RPC_STUB IWTEDNIS_get_DialedNumberIdentification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_put_DialedNumberIdentification_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [in] */ BSTR DialedNumberIdentification);


void __RPC_STUB IWTEDNIS_put_DialedNumberIdentification_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_put_AssignedApplication_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEDNIS_put_AssignedApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_get_AssignedApplication_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEDNIS_get_AssignedApplication_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_Save_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEDNIS_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_Refresh_Proxy( 
    IWTEDNIS __RPC_FAR * This);


void __RPC_STUB IWTEDNIS_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEDNIS_get_VendorDataSets_Proxy( 
    IWTEDNIS __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEDNIS_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEDNIS_INTERFACE_DEFINED__ */


#ifndef __IWTEVendorDataSets_INTERFACE_DEFINED__
#define __IWTEVendorDataSets_INTERFACE_DEFINED__

/* interface IWTEVendorDataSets */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEVendorDataSets;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8318-336C-11D3-8657-0090272F9EEB")
    IWTEVendorDataSets : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            VARIANT v,
            /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *pItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Id,
            /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *ppNewSet) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            VARIANT v) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEVendorDataSetsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEVendorDataSets __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEVendorDataSets __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEVendorDataSets __RPC_FAR * This,
            VARIANT v,
            /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *pItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEVendorDataSets __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IWTEVendorDataSets __RPC_FAR * This,
            /* [in] */ BSTR Id,
            /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *ppNewSet);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IWTEVendorDataSets __RPC_FAR * This,
            VARIANT v);
        
        END_INTERFACE
    } IWTEVendorDataSetsVtbl;

    interface IWTEVendorDataSets
    {
        CONST_VTBL struct IWTEVendorDataSetsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEVendorDataSets_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEVendorDataSets_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEVendorDataSets_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEVendorDataSets_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEVendorDataSets_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEVendorDataSets_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEVendorDataSets_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEVendorDataSets_Item(This,v,pItem)	\
    (This)->lpVtbl -> Item(This,v,pItem)

#define IWTEVendorDataSets_get__NewEnum(This,pVal)	\
    (This)->lpVtbl -> get__NewEnum(This,pVal)

#define IWTEVendorDataSets_get_Count(This,pVal)	\
    (This)->lpVtbl -> get_Count(This,pVal)

#define IWTEVendorDataSets_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEVendorDataSets_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEVendorDataSets_Add(This,Id,ppNewSet)	\
    (This)->lpVtbl -> Add(This,Id,ppNewSet)

#define IWTEVendorDataSets_Remove(This,v)	\
    (This)->lpVtbl -> Remove(This,v)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_Item_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    VARIANT v,
    /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *pItem);


void __RPC_STUB IWTEVendorDataSets_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_get__NewEnum_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEVendorDataSets_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_get_Count_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEVendorDataSets_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_Save_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEVendorDataSets_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_Refresh_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This);


void __RPC_STUB IWTEVendorDataSets_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_Add_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    /* [in] */ BSTR Id,
    /* [retval][out] */ IWTEVendorDataSet __RPC_FAR *__RPC_FAR *ppNewSet);


void __RPC_STUB IWTEVendorDataSets_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSets_Remove_Proxy( 
    IWTEVendorDataSets __RPC_FAR * This,
    VARIANT v);


void __RPC_STUB IWTEVendorDataSets_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEVendorDataSets_INTERFACE_DEFINED__ */


#ifndef __IWTEVendorDataSet_INTERFACE_DEFINED__
#define __IWTEVendorDataSet_INTERFACE_DEFINED__

/* interface IWTEVendorDataSet */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEVendorDataSet;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E831D-336C-11D3-8657-0090272F9EEB")
    IWTEVendorDataSet : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE RemoveValue( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE WaitForChanges( 
            /* [in] */ DWORD hEvent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AllNames( 
            /* [retval][out] */ VARIANT __RPC_FAR *pItem) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEVendorDataSetVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEVendorDataSet __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEVendorDataSet __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ VARIANT newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *RemoveValue )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEVendorDataSet __RPC_FAR * This);
        
        /* [hidden][helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WaitForChanges )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [in] */ DWORD hEvent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AllNames )( 
            IWTEVendorDataSet __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pItem);
        
        END_INTERFACE
    } IWTEVendorDataSetVtbl;

    interface IWTEVendorDataSet
    {
        CONST_VTBL struct IWTEVendorDataSetVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEVendorDataSet_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEVendorDataSet_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEVendorDataSet_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEVendorDataSet_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEVendorDataSet_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEVendorDataSet_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEVendorDataSet_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEVendorDataSet_get_Value(This,Name,pVal)	\
    (This)->lpVtbl -> get_Value(This,Name,pVal)

#define IWTEVendorDataSet_put_Value(This,Name,newVal)	\
    (This)->lpVtbl -> put_Value(This,Name,newVal)

#define IWTEVendorDataSet_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IWTEVendorDataSet_RemoveValue(This,Name)	\
    (This)->lpVtbl -> RemoveValue(This,Name)

#define IWTEVendorDataSet_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEVendorDataSet_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEVendorDataSet_WaitForChanges(This,hEvent)	\
    (This)->lpVtbl -> WaitForChanges(This,hEvent)

#define IWTEVendorDataSet_get_AllNames(This,pItem)	\
    (This)->lpVtbl -> get_AllNames(This,pItem)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_get_Value_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IWTEVendorDataSet_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_put_Value_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ VARIANT newVal);


void __RPC_STUB IWTEVendorDataSet_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_get_Name_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IWTEVendorDataSet_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_RemoveValue_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IWTEVendorDataSet_RemoveValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_Save_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEVendorDataSet_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_Refresh_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This);


void __RPC_STUB IWTEVendorDataSet_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [hidden][helpstring] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_WaitForChanges_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [in] */ DWORD hEvent);


void __RPC_STUB IWTEVendorDataSet_WaitForChanges_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEVendorDataSet_get_AllNames_Proxy( 
    IWTEVendorDataSet __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pItem);


void __RPC_STUB IWTEVendorDataSet_get_AllNames_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEVendorDataSet_INTERFACE_DEFINED__ */


#ifndef __IWTELog_INTERFACE_DEFINED__
#define __IWTELog_INTERFACE_DEFINED__

/* interface IWTELog */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTELog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E831F-336C-11D3-8657-0090272F9EEB")
    IWTELog : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogType( 
            /* [retval][out] */ LogType __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogType( 
            /* [in] */ LogType newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFilePeriod( 
            /* [retval][out] */ LogPeriodType __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFilePeriod( 
            /* [in] */ LogPeriodType newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFieldSelection( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFieldSelection( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFilesCompress( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFilesCompress( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFileDirectory( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFileDirectory( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogSqlDataSource( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogSqlDataSource( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogSqlTableName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogSqlTableName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogFileDirectoryType( 
            /* [retval][out] */ LogFileDirectoryType __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogFileDirectoryType( 
            /* [in] */ LogFileDirectoryType newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogEnabled( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogEnabled( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTELogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTELog __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTELog __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTELog __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTELog __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTELog __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogType )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ LogType __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogType )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ LogType newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFilePeriod )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ LogPeriodType __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFilePeriod )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ LogPeriodType newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFieldSelection )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFieldSelection )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFilesCompress )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFilesCompress )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFileDirectory )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFileDirectory )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogSqlDataSource )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogSqlDataSource )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogSqlTableName )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogSqlTableName )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogFileDirectoryType )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ LogFileDirectoryType __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogFileDirectoryType )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ LogFileDirectoryType newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogEnabled )( 
            IWTELog __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogEnabled )( 
            IWTELog __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        END_INTERFACE
    } IWTELogVtbl;

    interface IWTELog
    {
        CONST_VTBL struct IWTELogVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTELog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTELog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTELog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTELog_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTELog_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTELog_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTELog_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTELog_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTELog_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTELog_get_LogType(This,pVal)	\
    (This)->lpVtbl -> get_LogType(This,pVal)

#define IWTELog_put_LogType(This,newVal)	\
    (This)->lpVtbl -> put_LogType(This,newVal)

#define IWTELog_get_LogFilePeriod(This,pVal)	\
    (This)->lpVtbl -> get_LogFilePeriod(This,pVal)

#define IWTELog_put_LogFilePeriod(This,newVal)	\
    (This)->lpVtbl -> put_LogFilePeriod(This,newVal)

#define IWTELog_get_LogFieldSelection(This,pVal)	\
    (This)->lpVtbl -> get_LogFieldSelection(This,pVal)

#define IWTELog_put_LogFieldSelection(This,newVal)	\
    (This)->lpVtbl -> put_LogFieldSelection(This,newVal)

#define IWTELog_get_LogFilesCompress(This,pVal)	\
    (This)->lpVtbl -> get_LogFilesCompress(This,pVal)

#define IWTELog_put_LogFilesCompress(This,newVal)	\
    (This)->lpVtbl -> put_LogFilesCompress(This,newVal)

#define IWTELog_get_LogFileDirectory(This,pVal)	\
    (This)->lpVtbl -> get_LogFileDirectory(This,pVal)

#define IWTELog_put_LogFileDirectory(This,newVal)	\
    (This)->lpVtbl -> put_LogFileDirectory(This,newVal)

#define IWTELog_get_LogSqlDataSource(This,pVal)	\
    (This)->lpVtbl -> get_LogSqlDataSource(This,pVal)

#define IWTELog_put_LogSqlDataSource(This,newVal)	\
    (This)->lpVtbl -> put_LogSqlDataSource(This,newVal)

#define IWTELog_get_LogSqlTableName(This,pVal)	\
    (This)->lpVtbl -> get_LogSqlTableName(This,pVal)

#define IWTELog_put_LogSqlTableName(This,newVal)	\
    (This)->lpVtbl -> put_LogSqlTableName(This,newVal)

#define IWTELog_get_LogFileDirectoryType(This,pVal)	\
    (This)->lpVtbl -> get_LogFileDirectoryType(This,pVal)

#define IWTELog_put_LogFileDirectoryType(This,newVal)	\
    (This)->lpVtbl -> put_LogFileDirectoryType(This,newVal)

#define IWTELog_get_LogEnabled(This,pVal)	\
    (This)->lpVtbl -> get_LogEnabled(This,pVal)

#define IWTELog_put_LogEnabled(This,newVal)	\
    (This)->lpVtbl -> put_LogEnabled(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTELog_Save_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTELog_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTELog_Refresh_Proxy( 
    IWTELog __RPC_FAR * This);


void __RPC_STUB IWTELog_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogType_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ LogType __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogType_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ LogType newVal);


void __RPC_STUB IWTELog_put_LogType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogFilePeriod_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ LogPeriodType __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogFilePeriod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogFilePeriod_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ LogPeriodType newVal);


void __RPC_STUB IWTELog_put_LogFilePeriod_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogFieldSelection_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogFieldSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogFieldSelection_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTELog_put_LogFieldSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogFilesCompress_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogFilesCompress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogFilesCompress_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTELog_put_LogFilesCompress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogFileDirectory_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogFileDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogFileDirectory_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTELog_put_LogFileDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogSqlDataSource_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogSqlDataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogSqlDataSource_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTELog_put_LogSqlDataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogSqlTableName_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogSqlTableName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogSqlTableName_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTELog_put_LogSqlTableName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogFileDirectoryType_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ LogFileDirectoryType __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogFileDirectoryType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogFileDirectoryType_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ LogFileDirectoryType newVal);


void __RPC_STUB IWTELog_put_LogFileDirectoryType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTELog_get_LogEnabled_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTELog_get_LogEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELog_put_LogEnabled_Proxy( 
    IWTELog __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTELog_put_LogEnabled_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTELog_INTERFACE_DEFINED__ */


#ifndef __IWTEApplications_INTERFACE_DEFINED__
#define __IWTEApplications_INTERFACE_DEFINED__

/* interface IWTEApplications */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEApplications;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8321-336C-11D3-8657-0090272F9EEB")
    IWTEApplications : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR HomePage,
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppVal) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT v) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEApplicationsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEApplications __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEApplications __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEApplications __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IWTEApplications __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *retval);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEApplications __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *retval);
        
        /* [helpstring][id][restricted][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IWTEApplications __RPC_FAR * This,
            /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR HomePage,
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppVal);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IWTEApplications __RPC_FAR * This,
            /* [in] */ VARIANT v);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEApplications __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEApplications __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        END_INTERFACE
    } IWTEApplicationsVtbl;

    interface IWTEApplications
    {
        CONST_VTBL struct IWTEApplicationsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEApplications_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEApplications_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEApplications_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEApplications_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEApplications_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEApplications_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEApplications_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEApplications_get_Count(This,retval)	\
    (This)->lpVtbl -> get_Count(This,retval)

#define IWTEApplications_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEApplications_Item(This,Index,retval)	\
    (This)->lpVtbl -> Item(This,Index,retval)

#define IWTEApplications_get__NewEnum(This,retval)	\
    (This)->lpVtbl -> get__NewEnum(This,retval)

#define IWTEApplications_Add(This,Name,HomePage,ppVal)	\
    (This)->lpVtbl -> Add(This,Name,HomePage,ppVal)

#define IWTEApplications_Remove(This,v)	\
    (This)->lpVtbl -> Remove(This,v)

#define IWTEApplications_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEApplications_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplications_get_Count_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *retval);


void __RPC_STUB IWTEApplications_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplications_get_VendorDataSets_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEApplications_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplications_Item_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *retval);


void __RPC_STUB IWTEApplications_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][restricted][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplications_get__NewEnum_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [retval][out] */ LPUNKNOWN __RPC_FAR *retval);


void __RPC_STUB IWTEApplications_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplications_Add_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR HomePage,
    /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppVal);


void __RPC_STUB IWTEApplications_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplications_Remove_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [in] */ VARIANT v);


void __RPC_STUB IWTEApplications_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplications_Refresh_Proxy( 
    IWTEApplications __RPC_FAR * This);


void __RPC_STUB IWTEApplications_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplications_Save_Proxy( 
    IWTEApplications __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEApplications_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEApplications_INTERFACE_DEFINED__ */


#ifndef __IWTEApplication_INTERFACE_DEFINED__
#define __IWTEApplication_INTERFACE_DEFINED__

/* interface IWTEApplication */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEApplication;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("B83E8322-336C-11D3-8657-0090272F9EEB")
    IWTEApplication : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent = ( VARIANT_BOOL  )0) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_VendorDataSets( 
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Description( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Description( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomePage( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HomePage( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InputTerminator( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InputTerminator( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_BackKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_BackKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_HomeKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_HomeKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OperatorKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OperatorKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OperatorExtension( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OperatorExtension( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClickButtonKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClickButtonKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipButtonKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipButtonKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelectCheckBoxKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SelectCheckBoxKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClearCheckBoxKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClearCheckBoxKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IgnoreNavigationKeysTillTermination( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_IgnoreNavigationKeysTillTermination( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DetectABCD( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DetectABCD( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxRecordTime( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxRecordTime( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecordStopSilenceTime( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecordStopSilenceTime( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InvalidKeyAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InvalidKeyAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TimeoutAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TimeoutAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxRetries( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_MaxRetries( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_FirstTimeoutInterval( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_FirstTimeoutInterval( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InterTimeoutInterval( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InterTimeoutInterval( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecurringErrorPage( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecurringErrorPage( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RenderTTS( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RenderTTS( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TTSParameters( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TTSParameters( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SRParameters( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SRParameters( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogEventSelection( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_LogEventSelection( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DelayedAnswering( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DelayedAnswering( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_PostCallPage( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_PostCallPage( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RenderTablesAsTabularData( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RenderTablesAsTabularData( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InitialAutoNumber( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InitialAutoNumber( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShutdownTime( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShutdownTime( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShutdownAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShutdownAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SRThreshold( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SRThreshold( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UseSR( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_UseSR( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConfirmBefore( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConfirmBefore( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConfirmAfter( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConfirmAfter( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OfferItems( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OfferItems( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClickButtonPhrases( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClickButtonPhrases( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SkipButtonPhrases( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SkipButtonPhrases( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SelectCheckBoxPhrases( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SelectCheckBoxPhrases( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ClearCheckBoxPhrases( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ClearCheckBoxPhrases( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_NavigationAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_NavigationAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RepeatMenuAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RepeatMenuAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RepeatMenuKey( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RepeatMenuKey( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DownloadControlFlags( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DownloadControlFlags( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_OperatorTransferType( 
            /* [retval][out] */ int __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_OperatorTransferType( 
            /* [in] */ int newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Vendor( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Vendor( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EnableFullConfiguration( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_EnableFullConfiguration( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_RecordStopOnAnyKey( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_RecordStopOnAnyKey( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConfirmationMenuAnnouncement( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConfirmationMenuAnnouncement( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SuggestConfirmation( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_SuggestConfirmation( 
            /* [in] */ VARIANT_BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TTSWaveFormat( 
            /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_TTSWaveFormat( 
            /* [in] */ PCMWaveFormats newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InputWaveFormat( 
            /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_InputWaveFormat( 
            /* [in] */ PCMWaveFormats newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEApplicationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEApplication __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEApplication __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEApplication __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Refresh )( 
            IWTEApplication __RPC_FAR * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IWTEApplication __RPC_FAR * This,
            /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_VendorDataSets )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Description )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Description )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HomePage )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HomePage )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InputTerminator )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InputTerminator )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_BackKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_BackKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_HomeKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_HomeKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OperatorKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OperatorKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OperatorExtension )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OperatorExtension )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClickButtonKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClickButtonKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SkipButtonKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SkipButtonKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectCheckBoxKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectCheckBoxKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClearCheckBoxKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClearCheckBoxKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IgnoreNavigationKeysTillTermination )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_IgnoreNavigationKeysTillTermination )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DetectABCD )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DetectABCD )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRecordTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRecordTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordStopSilenceTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RecordStopSilenceTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InvalidKeyAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InvalidKeyAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TimeoutAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TimeoutAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxRetries )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_MaxRetries )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FirstTimeoutInterval )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_FirstTimeoutInterval )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InterTimeoutInterval )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InterTimeoutInterval )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecurringErrorPage )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RecurringErrorPage )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RenderTTS )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RenderTTS )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TTSParameters )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TTSParameters )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SRParameters )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SRParameters )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogEventSelection )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_LogEventSelection )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DelayedAnswering )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DelayedAnswering )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_PostCallPage )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_PostCallPage )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RenderTablesAsTabularData )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RenderTablesAsTabularData )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InitialAutoNumber )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InitialAutoNumber )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShutdownTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShutdownTime )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShutdownAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShutdownAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SRThreshold )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SRThreshold )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UseSR )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_UseSR )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConfirmBefore )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConfirmBefore )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConfirmAfter )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConfirmAfter )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OfferItems )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OfferItems )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClickButtonPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClickButtonPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SkipButtonPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SkipButtonPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SelectCheckBoxPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SelectCheckBoxPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClearCheckBoxPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClearCheckBoxPhrases )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_NavigationAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_NavigationAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RepeatMenuAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RepeatMenuAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RepeatMenuKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RepeatMenuKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DownloadControlFlags )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DownloadControlFlags )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_OperatorTransferType )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ int __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_OperatorTransferType )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ int newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Vendor )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Vendor )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EnableFullConfiguration )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_EnableFullConfiguration )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_RecordStopOnAnyKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_RecordStopOnAnyKey )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConfirmationMenuAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConfirmationMenuAnnouncement )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SuggestConfirmation )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_SuggestConfirmation )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ VARIANT_BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TTSWaveFormat )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_TTSWaveFormat )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ PCMWaveFormats newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InputWaveFormat )( 
            IWTEApplication __RPC_FAR * This,
            /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_InputWaveFormat )( 
            IWTEApplication __RPC_FAR * This,
            /* [in] */ PCMWaveFormats newVal);
        
        END_INTERFACE
    } IWTEApplicationVtbl;

    interface IWTEApplication
    {
        CONST_VTBL struct IWTEApplicationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEApplication_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEApplication_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEApplication_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEApplication_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEApplication_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEApplication_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEApplication_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEApplication_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IWTEApplication_Save(This,SaveOnlyIfCurrent)	\
    (This)->lpVtbl -> Save(This,SaveOnlyIfCurrent)

#define IWTEApplication_get_VendorDataSets(This,pVal)	\
    (This)->lpVtbl -> get_VendorDataSets(This,pVal)

#define IWTEApplication_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IWTEApplication_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IWTEApplication_get_Description(This,pVal)	\
    (This)->lpVtbl -> get_Description(This,pVal)

#define IWTEApplication_put_Description(This,newVal)	\
    (This)->lpVtbl -> put_Description(This,newVal)

#define IWTEApplication_get_HomePage(This,pVal)	\
    (This)->lpVtbl -> get_HomePage(This,pVal)

#define IWTEApplication_put_HomePage(This,newVal)	\
    (This)->lpVtbl -> put_HomePage(This,newVal)

#define IWTEApplication_get_InputTerminator(This,pVal)	\
    (This)->lpVtbl -> get_InputTerminator(This,pVal)

#define IWTEApplication_put_InputTerminator(This,newVal)	\
    (This)->lpVtbl -> put_InputTerminator(This,newVal)

#define IWTEApplication_get_BackKey(This,pVal)	\
    (This)->lpVtbl -> get_BackKey(This,pVal)

#define IWTEApplication_put_BackKey(This,newVal)	\
    (This)->lpVtbl -> put_BackKey(This,newVal)

#define IWTEApplication_get_HomeKey(This,pVal)	\
    (This)->lpVtbl -> get_HomeKey(This,pVal)

#define IWTEApplication_put_HomeKey(This,newVal)	\
    (This)->lpVtbl -> put_HomeKey(This,newVal)

#define IWTEApplication_get_OperatorKey(This,pVal)	\
    (This)->lpVtbl -> get_OperatorKey(This,pVal)

#define IWTEApplication_put_OperatorKey(This,newVal)	\
    (This)->lpVtbl -> put_OperatorKey(This,newVal)

#define IWTEApplication_get_OperatorExtension(This,pVal)	\
    (This)->lpVtbl -> get_OperatorExtension(This,pVal)

#define IWTEApplication_put_OperatorExtension(This,newVal)	\
    (This)->lpVtbl -> put_OperatorExtension(This,newVal)

#define IWTEApplication_get_ClickButtonKey(This,pVal)	\
    (This)->lpVtbl -> get_ClickButtonKey(This,pVal)

#define IWTEApplication_put_ClickButtonKey(This,newVal)	\
    (This)->lpVtbl -> put_ClickButtonKey(This,newVal)

#define IWTEApplication_get_SkipButtonKey(This,pVal)	\
    (This)->lpVtbl -> get_SkipButtonKey(This,pVal)

#define IWTEApplication_put_SkipButtonKey(This,newVal)	\
    (This)->lpVtbl -> put_SkipButtonKey(This,newVal)

#define IWTEApplication_get_SelectCheckBoxKey(This,pVal)	\
    (This)->lpVtbl -> get_SelectCheckBoxKey(This,pVal)

#define IWTEApplication_put_SelectCheckBoxKey(This,newVal)	\
    (This)->lpVtbl -> put_SelectCheckBoxKey(This,newVal)

#define IWTEApplication_get_ClearCheckBoxKey(This,pVal)	\
    (This)->lpVtbl -> get_ClearCheckBoxKey(This,pVal)

#define IWTEApplication_put_ClearCheckBoxKey(This,newVal)	\
    (This)->lpVtbl -> put_ClearCheckBoxKey(This,newVal)

#define IWTEApplication_get_IgnoreNavigationKeysTillTermination(This,pVal)	\
    (This)->lpVtbl -> get_IgnoreNavigationKeysTillTermination(This,pVal)

#define IWTEApplication_put_IgnoreNavigationKeysTillTermination(This,newVal)	\
    (This)->lpVtbl -> put_IgnoreNavigationKeysTillTermination(This,newVal)

#define IWTEApplication_get_DetectABCD(This,pVal)	\
    (This)->lpVtbl -> get_DetectABCD(This,pVal)

#define IWTEApplication_put_DetectABCD(This,newVal)	\
    (This)->lpVtbl -> put_DetectABCD(This,newVal)

#define IWTEApplication_get_MaxRecordTime(This,pVal)	\
    (This)->lpVtbl -> get_MaxRecordTime(This,pVal)

#define IWTEApplication_put_MaxRecordTime(This,newVal)	\
    (This)->lpVtbl -> put_MaxRecordTime(This,newVal)

#define IWTEApplication_get_RecordStopSilenceTime(This,pVal)	\
    (This)->lpVtbl -> get_RecordStopSilenceTime(This,pVal)

#define IWTEApplication_put_RecordStopSilenceTime(This,newVal)	\
    (This)->lpVtbl -> put_RecordStopSilenceTime(This,newVal)

#define IWTEApplication_get_InvalidKeyAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_InvalidKeyAnnouncement(This,pVal)

#define IWTEApplication_put_InvalidKeyAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_InvalidKeyAnnouncement(This,newVal)

#define IWTEApplication_get_TimeoutAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_TimeoutAnnouncement(This,pVal)

#define IWTEApplication_put_TimeoutAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_TimeoutAnnouncement(This,newVal)

#define IWTEApplication_get_MaxRetries(This,pVal)	\
    (This)->lpVtbl -> get_MaxRetries(This,pVal)

#define IWTEApplication_put_MaxRetries(This,newVal)	\
    (This)->lpVtbl -> put_MaxRetries(This,newVal)

#define IWTEApplication_get_FirstTimeoutInterval(This,pVal)	\
    (This)->lpVtbl -> get_FirstTimeoutInterval(This,pVal)

#define IWTEApplication_put_FirstTimeoutInterval(This,newVal)	\
    (This)->lpVtbl -> put_FirstTimeoutInterval(This,newVal)

#define IWTEApplication_get_InterTimeoutInterval(This,pVal)	\
    (This)->lpVtbl -> get_InterTimeoutInterval(This,pVal)

#define IWTEApplication_put_InterTimeoutInterval(This,newVal)	\
    (This)->lpVtbl -> put_InterTimeoutInterval(This,newVal)

#define IWTEApplication_get_RecurringErrorPage(This,pVal)	\
    (This)->lpVtbl -> get_RecurringErrorPage(This,pVal)

#define IWTEApplication_put_RecurringErrorPage(This,newVal)	\
    (This)->lpVtbl -> put_RecurringErrorPage(This,newVal)

#define IWTEApplication_get_RenderTTS(This,pVal)	\
    (This)->lpVtbl -> get_RenderTTS(This,pVal)

#define IWTEApplication_put_RenderTTS(This,newVal)	\
    (This)->lpVtbl -> put_RenderTTS(This,newVal)

#define IWTEApplication_get_TTSParameters(This,pVal)	\
    (This)->lpVtbl -> get_TTSParameters(This,pVal)

#define IWTEApplication_put_TTSParameters(This,newVal)	\
    (This)->lpVtbl -> put_TTSParameters(This,newVal)

#define IWTEApplication_get_SRParameters(This,pVal)	\
    (This)->lpVtbl -> get_SRParameters(This,pVal)

#define IWTEApplication_put_SRParameters(This,newVal)	\
    (This)->lpVtbl -> put_SRParameters(This,newVal)

#define IWTEApplication_get_LogEventSelection(This,pVal)	\
    (This)->lpVtbl -> get_LogEventSelection(This,pVal)

#define IWTEApplication_put_LogEventSelection(This,newVal)	\
    (This)->lpVtbl -> put_LogEventSelection(This,newVal)

#define IWTEApplication_get_DelayedAnswering(This,pVal)	\
    (This)->lpVtbl -> get_DelayedAnswering(This,pVal)

#define IWTEApplication_put_DelayedAnswering(This,newVal)	\
    (This)->lpVtbl -> put_DelayedAnswering(This,newVal)

#define IWTEApplication_get_PostCallPage(This,pVal)	\
    (This)->lpVtbl -> get_PostCallPage(This,pVal)

#define IWTEApplication_put_PostCallPage(This,newVal)	\
    (This)->lpVtbl -> put_PostCallPage(This,newVal)

#define IWTEApplication_get_RenderTablesAsTabularData(This,pVal)	\
    (This)->lpVtbl -> get_RenderTablesAsTabularData(This,pVal)

#define IWTEApplication_put_RenderTablesAsTabularData(This,newVal)	\
    (This)->lpVtbl -> put_RenderTablesAsTabularData(This,newVal)

#define IWTEApplication_get_InitialAutoNumber(This,pVal)	\
    (This)->lpVtbl -> get_InitialAutoNumber(This,pVal)

#define IWTEApplication_put_InitialAutoNumber(This,newVal)	\
    (This)->lpVtbl -> put_InitialAutoNumber(This,newVal)

#define IWTEApplication_get_ShutdownTime(This,pVal)	\
    (This)->lpVtbl -> get_ShutdownTime(This,pVal)

#define IWTEApplication_put_ShutdownTime(This,newVal)	\
    (This)->lpVtbl -> put_ShutdownTime(This,newVal)

#define IWTEApplication_get_ShutdownAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_ShutdownAnnouncement(This,pVal)

#define IWTEApplication_put_ShutdownAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_ShutdownAnnouncement(This,newVal)

#define IWTEApplication_get_SRThreshold(This,pVal)	\
    (This)->lpVtbl -> get_SRThreshold(This,pVal)

#define IWTEApplication_put_SRThreshold(This,newVal)	\
    (This)->lpVtbl -> put_SRThreshold(This,newVal)

#define IWTEApplication_get_UseSR(This,pVal)	\
    (This)->lpVtbl -> get_UseSR(This,pVal)

#define IWTEApplication_put_UseSR(This,newVal)	\
    (This)->lpVtbl -> put_UseSR(This,newVal)

#define IWTEApplication_get_ConfirmBefore(This,pVal)	\
    (This)->lpVtbl -> get_ConfirmBefore(This,pVal)

#define IWTEApplication_put_ConfirmBefore(This,newVal)	\
    (This)->lpVtbl -> put_ConfirmBefore(This,newVal)

#define IWTEApplication_get_ConfirmAfter(This,pVal)	\
    (This)->lpVtbl -> get_ConfirmAfter(This,pVal)

#define IWTEApplication_put_ConfirmAfter(This,newVal)	\
    (This)->lpVtbl -> put_ConfirmAfter(This,newVal)

#define IWTEApplication_get_OfferItems(This,pVal)	\
    (This)->lpVtbl -> get_OfferItems(This,pVal)

#define IWTEApplication_put_OfferItems(This,newVal)	\
    (This)->lpVtbl -> put_OfferItems(This,newVal)

#define IWTEApplication_get_ClickButtonPhrases(This,pVal)	\
    (This)->lpVtbl -> get_ClickButtonPhrases(This,pVal)

#define IWTEApplication_put_ClickButtonPhrases(This,newVal)	\
    (This)->lpVtbl -> put_ClickButtonPhrases(This,newVal)

#define IWTEApplication_get_SkipButtonPhrases(This,pVal)	\
    (This)->lpVtbl -> get_SkipButtonPhrases(This,pVal)

#define IWTEApplication_put_SkipButtonPhrases(This,newVal)	\
    (This)->lpVtbl -> put_SkipButtonPhrases(This,newVal)

#define IWTEApplication_get_SelectCheckBoxPhrases(This,pVal)	\
    (This)->lpVtbl -> get_SelectCheckBoxPhrases(This,pVal)

#define IWTEApplication_put_SelectCheckBoxPhrases(This,newVal)	\
    (This)->lpVtbl -> put_SelectCheckBoxPhrases(This,newVal)

#define IWTEApplication_get_ClearCheckBoxPhrases(This,pVal)	\
    (This)->lpVtbl -> get_ClearCheckBoxPhrases(This,pVal)

#define IWTEApplication_put_ClearCheckBoxPhrases(This,newVal)	\
    (This)->lpVtbl -> put_ClearCheckBoxPhrases(This,newVal)

#define IWTEApplication_get_NavigationAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_NavigationAnnouncement(This,pVal)

#define IWTEApplication_put_NavigationAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_NavigationAnnouncement(This,newVal)

#define IWTEApplication_get_RepeatMenuAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_RepeatMenuAnnouncement(This,pVal)

#define IWTEApplication_put_RepeatMenuAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_RepeatMenuAnnouncement(This,newVal)

#define IWTEApplication_get_RepeatMenuKey(This,pVal)	\
    (This)->lpVtbl -> get_RepeatMenuKey(This,pVal)

#define IWTEApplication_put_RepeatMenuKey(This,newVal)	\
    (This)->lpVtbl -> put_RepeatMenuKey(This,newVal)

#define IWTEApplication_get_DownloadControlFlags(This,pVal)	\
    (This)->lpVtbl -> get_DownloadControlFlags(This,pVal)

#define IWTEApplication_put_DownloadControlFlags(This,newVal)	\
    (This)->lpVtbl -> put_DownloadControlFlags(This,newVal)

#define IWTEApplication_get_OperatorTransferType(This,pVal)	\
    (This)->lpVtbl -> get_OperatorTransferType(This,pVal)

#define IWTEApplication_put_OperatorTransferType(This,newVal)	\
    (This)->lpVtbl -> put_OperatorTransferType(This,newVal)

#define IWTEApplication_get_Vendor(This,pVal)	\
    (This)->lpVtbl -> get_Vendor(This,pVal)

#define IWTEApplication_put_Vendor(This,newVal)	\
    (This)->lpVtbl -> put_Vendor(This,newVal)

#define IWTEApplication_get_EnableFullConfiguration(This,pVal)	\
    (This)->lpVtbl -> get_EnableFullConfiguration(This,pVal)

#define IWTEApplication_put_EnableFullConfiguration(This,newVal)	\
    (This)->lpVtbl -> put_EnableFullConfiguration(This,newVal)

#define IWTEApplication_get_RecordStopOnAnyKey(This,pVal)	\
    (This)->lpVtbl -> get_RecordStopOnAnyKey(This,pVal)

#define IWTEApplication_put_RecordStopOnAnyKey(This,newVal)	\
    (This)->lpVtbl -> put_RecordStopOnAnyKey(This,newVal)

#define IWTEApplication_get_ConfirmationMenuAnnouncement(This,pVal)	\
    (This)->lpVtbl -> get_ConfirmationMenuAnnouncement(This,pVal)

#define IWTEApplication_put_ConfirmationMenuAnnouncement(This,newVal)	\
    (This)->lpVtbl -> put_ConfirmationMenuAnnouncement(This,newVal)

#define IWTEApplication_get_SuggestConfirmation(This,pVal)	\
    (This)->lpVtbl -> get_SuggestConfirmation(This,pVal)

#define IWTEApplication_put_SuggestConfirmation(This,newVal)	\
    (This)->lpVtbl -> put_SuggestConfirmation(This,newVal)

#define IWTEApplication_get_TTSWaveFormat(This,pVal)	\
    (This)->lpVtbl -> get_TTSWaveFormat(This,pVal)

#define IWTEApplication_put_TTSWaveFormat(This,newVal)	\
    (This)->lpVtbl -> put_TTSWaveFormat(This,newVal)

#define IWTEApplication_get_InputWaveFormat(This,pVal)	\
    (This)->lpVtbl -> get_InputWaveFormat(This,pVal)

#define IWTEApplication_put_InputWaveFormat(This,newVal)	\
    (This)->lpVtbl -> put_InputWaveFormat(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplication_Refresh_Proxy( 
    IWTEApplication __RPC_FAR * This);


void __RPC_STUB IWTEApplication_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEApplication_Save_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [defaultvalue][in] */ VARIANT_BOOL SaveOnlyIfCurrent);


void __RPC_STUB IWTEApplication_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_VendorDataSets_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ IWTEVendorDataSets __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_VendorDataSets_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_Name_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_Name_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_Description_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_Description_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_Description_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_HomePage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_HomePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_HomePage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_HomePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_InputTerminator_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_InputTerminator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_InputTerminator_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_InputTerminator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_BackKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_BackKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_BackKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_BackKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_HomeKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_HomeKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_HomeKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_HomeKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_OperatorKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_OperatorKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_OperatorKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_OperatorKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_OperatorExtension_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_OperatorExtension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_OperatorExtension_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_OperatorExtension_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ClickButtonKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ClickButtonKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ClickButtonKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ClickButtonKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SkipButtonKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SkipButtonKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SkipButtonKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_SkipButtonKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SelectCheckBoxKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SelectCheckBoxKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SelectCheckBoxKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_SelectCheckBoxKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ClearCheckBoxKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ClearCheckBoxKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ClearCheckBoxKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ClearCheckBoxKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_IgnoreNavigationKeysTillTermination_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_IgnoreNavigationKeysTillTermination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_IgnoreNavigationKeysTillTermination_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_IgnoreNavigationKeysTillTermination_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_DetectABCD_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_DetectABCD_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_DetectABCD_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_DetectABCD_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_MaxRecordTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_MaxRecordTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_MaxRecordTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_MaxRecordTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RecordStopSilenceTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RecordStopSilenceTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RecordStopSilenceTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_RecordStopSilenceTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_InvalidKeyAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_InvalidKeyAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_InvalidKeyAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_InvalidKeyAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_TimeoutAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_TimeoutAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_TimeoutAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_TimeoutAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_MaxRetries_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_MaxRetries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_MaxRetries_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_MaxRetries_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_FirstTimeoutInterval_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_FirstTimeoutInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_FirstTimeoutInterval_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_FirstTimeoutInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_InterTimeoutInterval_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_InterTimeoutInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_InterTimeoutInterval_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_InterTimeoutInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RecurringErrorPage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RecurringErrorPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RecurringErrorPage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_RecurringErrorPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RenderTTS_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RenderTTS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RenderTTS_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_RenderTTS_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_TTSParameters_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_TTSParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_TTSParameters_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_TTSParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SRParameters_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SRParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SRParameters_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_SRParameters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_LogEventSelection_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_LogEventSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_LogEventSelection_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_LogEventSelection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_DelayedAnswering_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_DelayedAnswering_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_DelayedAnswering_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_DelayedAnswering_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_PostCallPage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_PostCallPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_PostCallPage_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_PostCallPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RenderTablesAsTabularData_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RenderTablesAsTabularData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RenderTablesAsTabularData_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_RenderTablesAsTabularData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_InitialAutoNumber_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_InitialAutoNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_InitialAutoNumber_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_InitialAutoNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ShutdownTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ShutdownTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ShutdownTime_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_ShutdownTime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ShutdownAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ShutdownAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ShutdownAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ShutdownAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SRThreshold_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SRThreshold_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SRThreshold_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_SRThreshold_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_UseSR_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_UseSR_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_UseSR_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_UseSR_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ConfirmBefore_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ConfirmBefore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ConfirmBefore_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ConfirmBefore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ConfirmAfter_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ConfirmAfter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ConfirmAfter_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ConfirmAfter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_OfferItems_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_OfferItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_OfferItems_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_OfferItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ClickButtonPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ClickButtonPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ClickButtonPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ClickButtonPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SkipButtonPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SkipButtonPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SkipButtonPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_SkipButtonPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SelectCheckBoxPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SelectCheckBoxPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SelectCheckBoxPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_SelectCheckBoxPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ClearCheckBoxPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ClearCheckBoxPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ClearCheckBoxPhrases_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ClearCheckBoxPhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_NavigationAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_NavigationAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_NavigationAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_NavigationAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RepeatMenuAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RepeatMenuAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RepeatMenuAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_RepeatMenuAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RepeatMenuKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RepeatMenuKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RepeatMenuKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_RepeatMenuKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_DownloadControlFlags_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_DownloadControlFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_DownloadControlFlags_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_DownloadControlFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_OperatorTransferType_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ int __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_OperatorTransferType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_OperatorTransferType_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ int newVal);


void __RPC_STUB IWTEApplication_put_OperatorTransferType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_Vendor_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_Vendor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_Vendor_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_Vendor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_EnableFullConfiguration_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_EnableFullConfiguration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_EnableFullConfiguration_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_EnableFullConfiguration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_RecordStopOnAnyKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_RecordStopOnAnyKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_RecordStopOnAnyKey_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_RecordStopOnAnyKey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_ConfirmationMenuAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_ConfirmationMenuAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_ConfirmationMenuAnnouncement_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTEApplication_put_ConfirmationMenuAnnouncement_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_SuggestConfirmation_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_SuggestConfirmation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_SuggestConfirmation_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ VARIANT_BOOL newVal);


void __RPC_STUB IWTEApplication_put_SuggestConfirmation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_TTSWaveFormat_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_TTSWaveFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_TTSWaveFormat_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ PCMWaveFormats newVal);


void __RPC_STUB IWTEApplication_put_TTSWaveFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEApplication_get_InputWaveFormat_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [retval][out] */ PCMWaveFormats __RPC_FAR *pVal);


void __RPC_STUB IWTEApplication_get_InputWaveFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTEApplication_put_InputWaveFormat_Proxy( 
    IWTEApplication __RPC_FAR * This,
    /* [in] */ PCMWaveFormats newVal);


void __RPC_STUB IWTEApplication_put_InputWaveFormat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEApplication_INTERFACE_DEFINED__ */


#ifndef __IWTEStepConstrains_INTERFACE_DEFINED__
#define __IWTEStepConstrains_INTERFACE_DEFINED__

/* interface IWTEStepConstrains */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEStepConstrains;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD0-33A8-11D3-8658-0090272F9EEB")
    IWTEStepConstrains : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TerminationString( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LegalDigits( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_InitialNumber( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LowerBoundary( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpperBoundary( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LowerBlocked( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_UpperBlocked( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEStepConstrainsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEStepConstrains __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEStepConstrains __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TerminationString )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LegalDigits )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_InitialNumber )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LowerBoundary )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UpperBoundary )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LowerBlocked )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_UpperBlocked )( 
            IWTEStepConstrains __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTEStepConstrainsVtbl;

    interface IWTEStepConstrains
    {
        CONST_VTBL struct IWTEStepConstrainsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEStepConstrains_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEStepConstrains_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEStepConstrains_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEStepConstrains_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEStepConstrains_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEStepConstrains_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEStepConstrains_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEStepConstrains_get_Type(This,pVal)	\
    (This)->lpVtbl -> get_Type(This,pVal)

#define IWTEStepConstrains_get_TerminationString(This,pVal)	\
    (This)->lpVtbl -> get_TerminationString(This,pVal)

#define IWTEStepConstrains_get_LegalDigits(This,pVal)	\
    (This)->lpVtbl -> get_LegalDigits(This,pVal)

#define IWTEStepConstrains_get_InitialNumber(This,pVal)	\
    (This)->lpVtbl -> get_InitialNumber(This,pVal)

#define IWTEStepConstrains_get_LowerBoundary(This,pVal)	\
    (This)->lpVtbl -> get_LowerBoundary(This,pVal)

#define IWTEStepConstrains_get_UpperBoundary(This,pVal)	\
    (This)->lpVtbl -> get_UpperBoundary(This,pVal)

#define IWTEStepConstrains_get_LowerBlocked(This,pVal)	\
    (This)->lpVtbl -> get_LowerBlocked(This,pVal)

#define IWTEStepConstrains_get_UpperBlocked(This,pVal)	\
    (This)->lpVtbl -> get_UpperBlocked(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_Type_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_TerminationString_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_TerminationString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_LegalDigits_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_LegalDigits_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_InitialNumber_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_InitialNumber_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_LowerBoundary_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_LowerBoundary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_UpperBoundary_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_UpperBoundary_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_LowerBlocked_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_LowerBlocked_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStepConstrains_get_UpperBlocked_Proxy( 
    IWTEStepConstrains __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTEStepConstrains_get_UpperBlocked_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEStepConstrains_INTERFACE_DEFINED__ */


#ifndef __IWTESession_INTERFACE_DEFINED__
#define __IWTESession_INTERFACE_DEFINED__

/* interface IWTESession */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTESession;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD1-33A8-11D3-8658-0090272F9EEB")
    IWTESession : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Value( 
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT __RPC_FAR *pnewVal) = 0;
        
        virtual /* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE putref_Value( 
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT __RPC_FAR *pnewVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastVisitedPage( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedPage) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LastVisitedTagID( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedTagID) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SessionState( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionState) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ServerName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrServerName) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SessionName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTESessionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTESession __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTESession __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTESession __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Value )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [retval][out] */ VARIANT __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT __RPC_FAR *pnewVal);
        
        /* [helpstring][id][propputref] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *putref_Value )( 
            IWTESession __RPC_FAR * This,
            /* [in] */ BSTR bstrName,
            /* [in] */ VARIANT __RPC_FAR *pnewVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastVisitedPage )( 
            IWTESession __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedPage);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LastVisitedTagID )( 
            IWTESession __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedTagID);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SessionState )( 
            IWTESession __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionState);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ServerName )( 
            IWTESession __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrServerName);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SessionName )( 
            IWTESession __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionName);
        
        END_INTERFACE
    } IWTESessionVtbl;

    interface IWTESession
    {
        CONST_VTBL struct IWTESessionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTESession_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTESession_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTESession_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTESession_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTESession_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTESession_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTESession_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTESession_get_Value(This,bstrName,pVal)	\
    (This)->lpVtbl -> get_Value(This,bstrName,pVal)

#define IWTESession_put_Value(This,bstrName,pnewVal)	\
    (This)->lpVtbl -> put_Value(This,bstrName,pnewVal)

#define IWTESession_putref_Value(This,bstrName,pnewVal)	\
    (This)->lpVtbl -> putref_Value(This,bstrName,pnewVal)

#define IWTESession_get_LastVisitedPage(This,pbstrLastVisitedPage)	\
    (This)->lpVtbl -> get_LastVisitedPage(This,pbstrLastVisitedPage)

#define IWTESession_get_LastVisitedTagID(This,pbstrLastVisitedTagID)	\
    (This)->lpVtbl -> get_LastVisitedTagID(This,pbstrLastVisitedTagID)

#define IWTESession_get_SessionState(This,pbstrSessionState)	\
    (This)->lpVtbl -> get_SessionState(This,pbstrSessionState)

#define IWTESession_get_ServerName(This,pbstrServerName)	\
    (This)->lpVtbl -> get_ServerName(This,pbstrServerName)

#define IWTESession_get_SessionName(This,pbstrSessionName)	\
    (This)->lpVtbl -> get_SessionName(This,pbstrSessionName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_Value_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [retval][out] */ VARIANT __RPC_FAR *pVal);


void __RPC_STUB IWTESession_get_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTESession_put_Value_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [in] */ VARIANT __RPC_FAR *pnewVal);


void __RPC_STUB IWTESession_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propputref] */ HRESULT STDMETHODCALLTYPE IWTESession_putref_Value_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [in] */ BSTR bstrName,
    /* [in] */ VARIANT __RPC_FAR *pnewVal);


void __RPC_STUB IWTESession_putref_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_LastVisitedPage_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedPage);


void __RPC_STUB IWTESession_get_LastVisitedPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_LastVisitedTagID_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrLastVisitedTagID);


void __RPC_STUB IWTESession_get_LastVisitedTagID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_SessionState_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionState);


void __RPC_STUB IWTESession_get_SessionState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_ServerName_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrServerName);


void __RPC_STUB IWTESession_get_ServerName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESession_get_SessionName_Proxy( 
    IWTESession __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrSessionName);


void __RPC_STUB IWTESession_get_SessionName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTESession_INTERFACE_DEFINED__ */


#ifndef __IWTETTSMode_INTERFACE_DEFINED__
#define __IWTETTSMode_INTERFACE_DEFINED__

/* interface IWTETTSMode */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTETTSMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD2-33A8-11D3-8658-0090272F9EEB")
    IWTETTSMode : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EngineID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MfgName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ModeID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ModeName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LanguageID( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Dialect( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Speaker( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Style( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Gender( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Age( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Features( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Interfaces( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EngineFeatures( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTETTSModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTETTSMode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTETTSMode __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTETTSMode __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTETTSMode __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTETTSMode __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTETTSMode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTETTSMode __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EngineID )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MfgName )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProductName )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModeID )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModeName )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LanguageID )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Dialect )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Speaker )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Style )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Gender )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Age )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Features )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Interfaces )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EngineFeatures )( 
            IWTETTSMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTETTSModeVtbl;

    interface IWTETTSMode
    {
        CONST_VTBL struct IWTETTSModeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTETTSMode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTETTSMode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTETTSMode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTETTSMode_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTETTSMode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTETTSMode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTETTSMode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTETTSMode_get_EngineID(This,pVal)	\
    (This)->lpVtbl -> get_EngineID(This,pVal)

#define IWTETTSMode_get_MfgName(This,pVal)	\
    (This)->lpVtbl -> get_MfgName(This,pVal)

#define IWTETTSMode_get_ProductName(This,pVal)	\
    (This)->lpVtbl -> get_ProductName(This,pVal)

#define IWTETTSMode_get_ModeID(This,pVal)	\
    (This)->lpVtbl -> get_ModeID(This,pVal)

#define IWTETTSMode_get_ModeName(This,pVal)	\
    (This)->lpVtbl -> get_ModeName(This,pVal)

#define IWTETTSMode_get_LanguageID(This,pVal)	\
    (This)->lpVtbl -> get_LanguageID(This,pVal)

#define IWTETTSMode_get_Dialect(This,pVal)	\
    (This)->lpVtbl -> get_Dialect(This,pVal)

#define IWTETTSMode_get_Speaker(This,pVal)	\
    (This)->lpVtbl -> get_Speaker(This,pVal)

#define IWTETTSMode_get_Style(This,pVal)	\
    (This)->lpVtbl -> get_Style(This,pVal)

#define IWTETTSMode_get_Gender(This,pVal)	\
    (This)->lpVtbl -> get_Gender(This,pVal)

#define IWTETTSMode_get_Age(This,pVal)	\
    (This)->lpVtbl -> get_Age(This,pVal)

#define IWTETTSMode_get_Features(This,pVal)	\
    (This)->lpVtbl -> get_Features(This,pVal)

#define IWTETTSMode_get_Interfaces(This,pVal)	\
    (This)->lpVtbl -> get_Interfaces(This,pVal)

#define IWTETTSMode_get_EngineFeatures(This,pVal)	\
    (This)->lpVtbl -> get_EngineFeatures(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_EngineID_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_EngineID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_MfgName_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_MfgName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_ProductName_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_ProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_ModeID_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_ModeID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_ModeName_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_ModeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_LanguageID_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_LanguageID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Dialect_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Dialect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Speaker_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Speaker_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Style_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Style_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Gender_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Gender_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Age_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Age_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Features_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Features_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_Interfaces_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_Interfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTETTSMode_get_EngineFeatures_Proxy( 
    IWTETTSMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTETTSMode_get_EngineFeatures_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTETTSMode_INTERFACE_DEFINED__ */


#ifndef __IWTESRMode_INTERFACE_DEFINED__
#define __IWTESRMode_INTERFACE_DEFINED__

/* interface IWTESRMode */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTESRMode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD3-33A8-11D3-8658-0090272F9EEB")
    IWTESRMode : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EngineID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MfgName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ProductName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ModeID( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ModeName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LanguageID( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Dialect( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Sequencing( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxWordsVocab( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MaxWordsState( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Grammars( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Features( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Interfaces( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_EngineFeatures( 
            /* [retval][out] */ long __RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTESRModeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTESRMode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTESRMode __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTESRMode __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTESRMode __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTESRMode __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTESRMode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTESRMode __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EngineID )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MfgName )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ProductName )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModeID )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ModeName )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LanguageID )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Dialect )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Sequencing )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxWordsVocab )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MaxWordsState )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Grammars )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Features )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Interfaces )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_EngineFeatures )( 
            IWTESRMode __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTESRModeVtbl;

    interface IWTESRMode
    {
        CONST_VTBL struct IWTESRModeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTESRMode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTESRMode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTESRMode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTESRMode_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTESRMode_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTESRMode_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTESRMode_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTESRMode_get_EngineID(This,pVal)	\
    (This)->lpVtbl -> get_EngineID(This,pVal)

#define IWTESRMode_get_MfgName(This,pVal)	\
    (This)->lpVtbl -> get_MfgName(This,pVal)

#define IWTESRMode_get_ProductName(This,pVal)	\
    (This)->lpVtbl -> get_ProductName(This,pVal)

#define IWTESRMode_get_ModeID(This,pVal)	\
    (This)->lpVtbl -> get_ModeID(This,pVal)

#define IWTESRMode_get_ModeName(This,pVal)	\
    (This)->lpVtbl -> get_ModeName(This,pVal)

#define IWTESRMode_get_LanguageID(This,pVal)	\
    (This)->lpVtbl -> get_LanguageID(This,pVal)

#define IWTESRMode_get_Dialect(This,pVal)	\
    (This)->lpVtbl -> get_Dialect(This,pVal)

#define IWTESRMode_get_Sequencing(This,pVal)	\
    (This)->lpVtbl -> get_Sequencing(This,pVal)

#define IWTESRMode_get_MaxWordsVocab(This,pVal)	\
    (This)->lpVtbl -> get_MaxWordsVocab(This,pVal)

#define IWTESRMode_get_MaxWordsState(This,pVal)	\
    (This)->lpVtbl -> get_MaxWordsState(This,pVal)

#define IWTESRMode_get_Grammars(This,pVal)	\
    (This)->lpVtbl -> get_Grammars(This,pVal)

#define IWTESRMode_get_Features(This,pVal)	\
    (This)->lpVtbl -> get_Features(This,pVal)

#define IWTESRMode_get_Interfaces(This,pVal)	\
    (This)->lpVtbl -> get_Interfaces(This,pVal)

#define IWTESRMode_get_EngineFeatures(This,pVal)	\
    (This)->lpVtbl -> get_EngineFeatures(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_EngineID_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_EngineID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_MfgName_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_MfgName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_ProductName_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_ProductName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_ModeID_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_ModeID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_ModeName_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_ModeName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_LanguageID_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_LanguageID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_Dialect_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_Dialect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_Sequencing_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_Sequencing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_MaxWordsVocab_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_MaxWordsVocab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_MaxWordsState_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_MaxWordsState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_Grammars_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_Grammars_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_Features_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_Features_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_Interfaces_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_Interfaces_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESRMode_get_EngineFeatures_Proxy( 
    IWTESRMode __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pVal);


void __RPC_STUB IWTESRMode_get_EngineFeatures_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTESRMode_INTERFACE_DEFINED__ */


#ifndef __IWTEStep_INTERFACE_DEFINED__
#define __IWTEStep_INTERFACE_DEFINED__

/* interface IWTEStep */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEStep;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD4-33A8-11D3-8658-0090272F9EEB")
    IWTEStep : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_MenuItemsLength( 
            /* [retval][out] */ long __RPC_FAR *plLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetMenuItem( 
            /* [in] */ long lIndex,
            /* [out][retval] */ IDispatch __RPC_FAR *__RPC_FAR *ppMenuItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SubmitMenu( 
            /* [retval][out] */ BOOL __RPC_FAR *pfIsSubmit) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AudioItemsLength( 
            /* [retval][out] */ long __RPC_FAR *plLength) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetAudioItem( 
            /* [in] */ long lIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrAudioItem) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CutThroughMode( 
            /* [retval][out] */ BOOL __RPC_FAR *pfCutThroughMode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_IsRecordType( 
            /* [retval][out] */ BOOL __RPC_FAR *pfIsRecord) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEStepVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEStep __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEStep __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEStep __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_MenuItemsLength )( 
            IWTEStep __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetMenuItem )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [out][retval] */ IDispatch __RPC_FAR *__RPC_FAR *ppMenuItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SubmitMenu )( 
            IWTEStep __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfIsSubmit);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AudioItemsLength )( 
            IWTEStep __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plLength);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetAudioItem )( 
            IWTEStep __RPC_FAR * This,
            /* [in] */ long lIndex,
            /* [out][retval] */ BSTR __RPC_FAR *pbstrAudioItem);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CutThroughMode )( 
            IWTEStep __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfCutThroughMode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_IsRecordType )( 
            IWTEStep __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pfIsRecord);
        
        END_INTERFACE
    } IWTEStepVtbl;

    interface IWTEStep
    {
        CONST_VTBL struct IWTEStepVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEStep_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEStep_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEStep_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEStep_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEStep_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEStep_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEStep_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEStep_get_MenuItemsLength(This,plLength)	\
    (This)->lpVtbl -> get_MenuItemsLength(This,plLength)

#define IWTEStep_GetMenuItem(This,lIndex,ppMenuItem)	\
    (This)->lpVtbl -> GetMenuItem(This,lIndex,ppMenuItem)

#define IWTEStep_get_SubmitMenu(This,pfIsSubmit)	\
    (This)->lpVtbl -> get_SubmitMenu(This,pfIsSubmit)

#define IWTEStep_get_AudioItemsLength(This,plLength)	\
    (This)->lpVtbl -> get_AudioItemsLength(This,plLength)

#define IWTEStep_GetAudioItem(This,lIndex,pbstrAudioItem)	\
    (This)->lpVtbl -> GetAudioItem(This,lIndex,pbstrAudioItem)

#define IWTEStep_get_CutThroughMode(This,pfCutThroughMode)	\
    (This)->lpVtbl -> get_CutThroughMode(This,pfCutThroughMode)

#define IWTEStep_get_IsRecordType(This,pfIsRecord)	\
    (This)->lpVtbl -> get_IsRecordType(This,pfIsRecord)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStep_get_MenuItemsLength_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plLength);


void __RPC_STUB IWTEStep_get_MenuItemsLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEStep_GetMenuItem_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [out][retval] */ IDispatch __RPC_FAR *__RPC_FAR *ppMenuItem);


void __RPC_STUB IWTEStep_GetMenuItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStep_get_SubmitMenu_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfIsSubmit);


void __RPC_STUB IWTEStep_get_SubmitMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStep_get_AudioItemsLength_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plLength);


void __RPC_STUB IWTEStep_get_AudioItemsLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEStep_GetAudioItem_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [in] */ long lIndex,
    /* [out][retval] */ BSTR __RPC_FAR *pbstrAudioItem);


void __RPC_STUB IWTEStep_GetAudioItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStep_get_CutThroughMode_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfCutThroughMode);


void __RPC_STUB IWTEStep_get_CutThroughMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEStep_get_IsRecordType_Proxy( 
    IWTEStep __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pfIsRecord);


void __RPC_STUB IWTEStep_get_IsRecordType_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEStep_INTERFACE_DEFINED__ */


#ifndef __IWTELogCustomField_INTERFACE_DEFINED__
#define __IWTELogCustomField_INTERFACE_DEFINED__

/* interface IWTELogCustomField */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTELogCustomField;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD5-33A8-11D3-8658-0090272F9EEB")
    IWTELogCustomField : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Value( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_DefaultValue( 
            /* [in] */ BSTR newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTELogCustomFieldVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTELogCustomField __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTELogCustomField __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Value )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR newVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DefaultValue )( 
            IWTELogCustomField __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        END_INTERFACE
    } IWTELogCustomFieldVtbl;

    interface IWTELogCustomField
    {
        CONST_VTBL struct IWTELogCustomFieldVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTELogCustomField_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTELogCustomField_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTELogCustomField_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTELogCustomField_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTELogCustomField_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTELogCustomField_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTELogCustomField_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTELogCustomField_put_Value(This,Name,newVal)	\
    (This)->lpVtbl -> put_Value(This,Name,newVal)

#define IWTELogCustomField_put_DefaultValue(This,newVal)	\
    (This)->lpVtbl -> put_DefaultValue(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELogCustomField_put_Value_Proxy( 
    IWTELogCustomField __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTELogCustomField_put_Value_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IWTELogCustomField_put_DefaultValue_Proxy( 
    IWTELogCustomField __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IWTELogCustomField_put_DefaultValue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTELogCustomField_INTERFACE_DEFINED__ */


#ifndef __IWTEExOM_INTERFACE_DEFINED__
#define __IWTEExOM_INTERFACE_DEFINED__

/* interface IWTEExOM */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEExOM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD6-33A8-11D3-8658-0090272F9EEB")
    IWTEExOM : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_hostName( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHostName) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Abandon( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Session( 
            /* [retval][out] */ IWTESession __RPC_FAR *__RPC_FAR *ppSession) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Configuration( 
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppApp) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Log( void) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ITBasicCallControl( 
            /* [retval][out] */ ITBasicCallControl __RPC_FAR *__RPC_FAR *ppCall) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_AlternativePhrases( 
            /* [retval][out] */ VARIANT __RPC_FAR *pAlternativePhrases) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_LogCustomField( 
            /* [retval][out] */ IWTELogCustomField __RPC_FAR *__RPC_FAR *ppLogCustomField) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Event( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *ppEvent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_TTSMode( 
            /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *ppTTSMode) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_SRMode( 
            /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *ppSRMode) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEExOMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEExOM __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEExOM __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEExOM __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEExOM __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEExOM __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEExOM __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEExOM __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_hostName )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrHostName);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Abandon )( 
            IWTEExOM __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Session )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ IWTESession __RPC_FAR *__RPC_FAR *ppSession);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Configuration )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppApp);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Log )( 
            IWTEExOM __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ITBasicCallControl )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ ITBasicCallControl __RPC_FAR *__RPC_FAR *ppCall);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_AlternativePhrases )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ VARIANT __RPC_FAR *pAlternativePhrases);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_LogCustomField )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ IWTELogCustomField __RPC_FAR *__RPC_FAR *ppLogCustomField);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Event )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *ppEvent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_TTSMode )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *ppTTSMode);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_SRMode )( 
            IWTEExOM __RPC_FAR * This,
            /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *ppSRMode);
        
        END_INTERFACE
    } IWTEExOMVtbl;

    interface IWTEExOM
    {
        CONST_VTBL struct IWTEExOMVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEExOM_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEExOM_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEExOM_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEExOM_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEExOM_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEExOM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEExOM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEExOM_get_hostName(This,pbstrHostName)	\
    (This)->lpVtbl -> get_hostName(This,pbstrHostName)

#define IWTEExOM_Abandon(This)	\
    (This)->lpVtbl -> Abandon(This)

#define IWTEExOM_get_Session(This,ppSession)	\
    (This)->lpVtbl -> get_Session(This,ppSession)

#define IWTEExOM_get_Configuration(This,ppApp)	\
    (This)->lpVtbl -> get_Configuration(This,ppApp)

#define IWTEExOM_Log(This)	\
    (This)->lpVtbl -> Log(This)

#define IWTEExOM_get_ITBasicCallControl(This,ppCall)	\
    (This)->lpVtbl -> get_ITBasicCallControl(This,ppCall)

#define IWTEExOM_get_AlternativePhrases(This,pAlternativePhrases)	\
    (This)->lpVtbl -> get_AlternativePhrases(This,pAlternativePhrases)

#define IWTEExOM_get_LogCustomField(This,ppLogCustomField)	\
    (This)->lpVtbl -> get_LogCustomField(This,ppLogCustomField)

#define IWTEExOM_get_Event(This,ppEvent)	\
    (This)->lpVtbl -> get_Event(This,ppEvent)

#define IWTEExOM_get_TTSMode(This,ppTTSMode)	\
    (This)->lpVtbl -> get_TTSMode(This,ppTTSMode)

#define IWTEExOM_get_SRMode(This,ppSRMode)	\
    (This)->lpVtbl -> get_SRMode(This,ppSRMode)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_hostName_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrHostName);


void __RPC_STUB IWTEExOM_get_hostName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEExOM_Abandon_Proxy( 
    IWTEExOM __RPC_FAR * This);


void __RPC_STUB IWTEExOM_Abandon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_Session_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ IWTESession __RPC_FAR *__RPC_FAR *ppSession);


void __RPC_STUB IWTEExOM_get_Session_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_Configuration_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ IWTEApplication __RPC_FAR *__RPC_FAR *ppApp);


void __RPC_STUB IWTEExOM_get_Configuration_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEExOM_Log_Proxy( 
    IWTEExOM __RPC_FAR * This);


void __RPC_STUB IWTEExOM_Log_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_ITBasicCallControl_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ ITBasicCallControl __RPC_FAR *__RPC_FAR *ppCall);


void __RPC_STUB IWTEExOM_get_ITBasicCallControl_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_AlternativePhrases_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ VARIANT __RPC_FAR *pAlternativePhrases);


void __RPC_STUB IWTEExOM_get_AlternativePhrases_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_LogCustomField_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ IWTELogCustomField __RPC_FAR *__RPC_FAR *ppLogCustomField);


void __RPC_STUB IWTEExOM_get_LogCustomField_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_Event_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *ppEvent);


void __RPC_STUB IWTEExOM_get_Event_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_TTSMode_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ IWTETTSMode __RPC_FAR *__RPC_FAR *ppTTSMode);


void __RPC_STUB IWTEExOM_get_TTSMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEExOM_get_SRMode_Proxy( 
    IWTEExOM __RPC_FAR * This,
    /* [retval][out] */ IWTESRMode __RPC_FAR *__RPC_FAR *ppSRMode);


void __RPC_STUB IWTEExOM_get_SRMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEExOM_INTERFACE_DEFINED__ */


#ifndef __IWTEActiveConnection_INTERFACE_DEFINED__
#define __IWTEActiveConnection_INTERFACE_DEFINED__

/* interface IWTEActiveConnection */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEActiveConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD7-33A8-11D3-8658-0090272F9EEB")
    IWTEActiveConnection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ExtendedObjectModel( 
            /* [retval][out] */ IWTEExOM __RPC_FAR *__RPC_FAR *ppExOm) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Step( 
            /* [retval][out] */ IWTEStep __RPC_FAR *__RPC_FAR *ppStep) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEActiveConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEActiveConnection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEActiveConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ExtendedObjectModel )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [retval][out] */ IWTEExOM __RPC_FAR *__RPC_FAR *ppExOm);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Step )( 
            IWTEActiveConnection __RPC_FAR * This,
            /* [retval][out] */ IWTEStep __RPC_FAR *__RPC_FAR *ppStep);
        
        END_INTERFACE
    } IWTEActiveConnectionVtbl;

    interface IWTEActiveConnection
    {
        CONST_VTBL struct IWTEActiveConnectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEActiveConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEActiveConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEActiveConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEActiveConnection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEActiveConnection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEActiveConnection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEActiveConnection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEActiveConnection_get_ExtendedObjectModel(This,ppExOm)	\
    (This)->lpVtbl -> get_ExtendedObjectModel(This,ppExOm)

#define IWTEActiveConnection_get_Step(This,ppStep)	\
    (This)->lpVtbl -> get_Step(This,ppStep)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEActiveConnection_get_ExtendedObjectModel_Proxy( 
    IWTEActiveConnection __RPC_FAR * This,
    /* [retval][out] */ IWTEExOM __RPC_FAR *__RPC_FAR *ppExOm);


void __RPC_STUB IWTEActiveConnection_get_ExtendedObjectModel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTEActiveConnection_get_Step_Proxy( 
    IWTEActiveConnection __RPC_FAR * This,
    /* [retval][out] */ IWTEStep __RPC_FAR *__RPC_FAR *ppStep);


void __RPC_STUB IWTEActiveConnection_get_Step_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEActiveConnection_INTERFACE_DEFINED__ */


#ifndef __IWTEService_INTERFACE_DEFINED__
#define __IWTEService_INTERFACE_DEFINED__

/* interface IWTEService */
/* [unique][helpstring][dual][uuid][object] */ 


EXTERN_C const IID IID_IWTEService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("AFA50DD8-33A8-11D3-8658-0090272F9EEB")
    IWTEService : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetActiveConnection( 
            /* [in] */ BSTR bstrApplicationName,
            /* [out][retval] */ IWTEActiveConnection __RPC_FAR *__RPC_FAR *ppConnection) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTEServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEService __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEService __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEService __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEService __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEService __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEService __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetActiveConnection )( 
            IWTEService __RPC_FAR * This,
            /* [in] */ BSTR bstrApplicationName,
            /* [out][retval] */ IWTEActiveConnection __RPC_FAR *__RPC_FAR *ppConnection);
        
        END_INTERFACE
    } IWTEServiceVtbl;

    interface IWTEService
    {
        CONST_VTBL struct IWTEServiceVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEService_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEService_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEService_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEService_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IWTEService_GetActiveConnection(This,bstrApplicationName,ppConnection)	\
    (This)->lpVtbl -> GetActiveConnection(This,bstrApplicationName,ppConnection)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IWTEService_GetActiveConnection_Proxy( 
    IWTEService __RPC_FAR * This,
    /* [in] */ BSTR bstrApplicationName,
    /* [out][retval] */ IWTEActiveConnection __RPC_FAR *__RPC_FAR *ppConnection);


void __RPC_STUB IWTEService_GetActiveConnection_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTEService_INTERFACE_DEFINED__ */


#ifndef __IWTESnapinNode_INTERFACE_DEFINED__
#define __IWTESnapinNode_INTERFACE_DEFINED__

/* interface IWTESnapinNode */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IWTESnapinNode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c59f0ca0-401c-11d3-8659-0090272f9eeb")
    IWTESnapinNode : public IUnknown
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CurrentObject( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ParentArray( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWTESnapinNodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTESnapinNode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTESnapinNode __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTESnapinNode __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CurrentObject )( 
            IWTESnapinNode __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ParentArray )( 
            IWTESnapinNode __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);
        
        END_INTERFACE
    } IWTESnapinNodeVtbl;

    interface IWTESnapinNode
    {
        CONST_VTBL struct IWTESnapinNodeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTESnapinNode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTESnapinNode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTESnapinNode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTESnapinNode_get_CurrentObject(This,pVal)	\
    (This)->lpVtbl -> get_CurrentObject(This,pVal)

#define IWTESnapinNode_get_ParentArray(This,pVal)	\
    (This)->lpVtbl -> get_ParentArray(This,pVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESnapinNode_get_CurrentObject_Proxy( 
    IWTESnapinNode __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTESnapinNode_get_CurrentObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IWTESnapinNode_get_ParentArray_Proxy( 
    IWTESnapinNode __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IWTESnapinNode_get_ParentArray_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWTESnapinNode_INTERFACE_DEFINED__ */



#ifndef __mswteCom_LIBRARY_DEFINED__
#define __mswteCom_LIBRARY_DEFINED__

/* library mswteCom */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_mswteCom;

EXTERN_C const CLSID CLSID_WTE;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8324-336C-11D3-8657-0090272F9EEB")
WTE;
#endif

EXTERN_C const CLSID CLSID_WTEArray;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8325-336C-11D3-8657-0090272F9EEB")
WTEArray;
#endif

EXTERN_C const CLSID CLSID_WTEArrays;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8326-336C-11D3-8657-0090272F9EEB")
WTEArrays;
#endif

EXTERN_C const CLSID CLSID_WTEServers;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8327-336C-11D3-8657-0090272F9EEB")
WTEServers;
#endif

EXTERN_C const CLSID CLSID_WTEServer;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8328-336C-11D3-8657-0090272F9EEB")
WTEServer;
#endif

EXTERN_C const CLSID CLSID_WTEAddresses;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8329-336C-11D3-8657-0090272F9EEB")
WTEAddresses;
#endif

EXTERN_C const CLSID CLSID_WTEAddress;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E832A-336C-11D3-8657-0090272F9EEB")
WTEAddress;
#endif

EXTERN_C const CLSID CLSID_WTEAddressGroups;

#ifdef __cplusplus

class DECLSPEC_UUID("d5059820-5a34-11d3-865d-0090272f9eeb")
WTEAddressGroups;
#endif

EXTERN_C const CLSID CLSID_WTEAddressGroup;

#ifdef __cplusplus

class DECLSPEC_UUID("f54ef550-5a34-11d3-865d-0090272f9eeb")
WTEAddressGroup;
#endif

EXTERN_C const CLSID CLSID_WTEDNISes;

#ifdef __cplusplus

class DECLSPEC_UUID("07bf2f80-5a35-11d3-865d-0090272f9eeb")
WTEDNISes;
#endif

EXTERN_C const CLSID CLSID_WTEDNIS;

#ifdef __cplusplus

class DECLSPEC_UUID("22b3fa40-5a35-11d3-865d-0090272f9eeb")
WTEDNIS;
#endif

#ifndef __IWTEVendorDataChanges_DISPINTERFACE_DEFINED__
#define __IWTEVendorDataChanges_DISPINTERFACE_DEFINED__

/* dispinterface IWTEVendorDataChanges */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID_IWTEVendorDataChanges;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B83E832C-336C-11D3-8657-0090272F9EEB")
    IWTEVendorDataChanges : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IWTEVendorDataChangesVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEVendorDataChanges __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEVendorDataChanges __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEVendorDataChanges __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEVendorDataChanges __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEVendorDataChanges __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEVendorDataChanges __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEVendorDataChanges __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IWTEVendorDataChangesVtbl;

    interface IWTEVendorDataChanges
    {
        CONST_VTBL struct IWTEVendorDataChangesVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEVendorDataChanges_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEVendorDataChanges_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEVendorDataChanges_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEVendorDataChanges_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEVendorDataChanges_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEVendorDataChanges_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEVendorDataChanges_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IWTEVendorDataChanges_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WTEVendorDataSets;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E832D-336C-11D3-8657-0090272F9EEB")
WTEVendorDataSets;
#endif

EXTERN_C const CLSID CLSID_WTEVendorDataSet;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E832E-336C-11D3-8657-0090272F9EEB")
WTEVendorDataSet;
#endif

EXTERN_C const CLSID CLSID_WTELog;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8334-336C-11D3-8657-0090272F9EEB")
WTELog;
#endif

EXTERN_C const CLSID CLSID_WTEApplications;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8336-336C-11D3-8657-0090272F9EEB")
WTEApplications;
#endif

EXTERN_C const CLSID CLSID_WTEApplication;

#ifdef __cplusplus

class DECLSPEC_UUID("B83E8337-336C-11D3-8657-0090272F9EEB")
WTEApplication;
#endif

#ifndef __IWTEConnectionEvents_DISPINTERFACE_DEFINED__
#define __IWTEConnectionEvents_DISPINTERFACE_DEFINED__

/* dispinterface IWTEConnectionEvents */
/* [uuid] */ 


EXTERN_C const IID DIID_IWTEConnectionEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("AFA50DDA-33A8-11D3-8658-0090272F9EEB")
    IWTEConnectionEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IWTEConnectionEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IWTEConnectionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IWTEConnectionEvents __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IWTEConnectionEvents __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IWTEConnectionEvents __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IWTEConnectionEvents __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IWTEConnectionEvents __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IWTEConnectionEvents __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IWTEConnectionEventsVtbl;

    interface IWTEConnectionEvents
    {
        CONST_VTBL struct IWTEConnectionEventsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWTEConnectionEvents_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWTEConnectionEvents_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWTEConnectionEvents_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWTEConnectionEvents_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IWTEConnectionEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IWTEConnectionEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IWTEConnectionEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IWTEConnectionEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_WTEService;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DDB-33A8-11D3-8658-0090272F9EEB")
WTEService;
#endif

EXTERN_C const CLSID CLSID_WTEActiveConnection;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DDC-33A8-11D3-8658-0090272F9EEB")
WTEActiveConnection;
#endif

EXTERN_C const CLSID CLSID_WTEStep;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DDD-33A8-11D3-8658-0090272F9EEB")
WTEStep;
#endif

EXTERN_C const CLSID CLSID_WTEStepConstrains;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DDE-33A8-11D3-8658-0090272F9EEB")
WTEStepConstrains;
#endif

EXTERN_C const CLSID CLSID_WTESession;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DDF-33A8-11D3-8658-0090272F9EEB")
WTESession;
#endif

EXTERN_C const CLSID CLSID_WTETTSModes;

#ifdef __cplusplus

class DECLSPEC_UUID("b79f37a0-6c46-11d3-8661-0090272f9eeb")
WTETTSModes;
#endif

EXTERN_C const CLSID CLSID_WTESRModes;

#ifdef __cplusplus

class DECLSPEC_UUID("e74537a0-6e66-11d3-8661-0090272f9eeb")
WTESRModes;
#endif

EXTERN_C const CLSID CLSID_WTETTSMode;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DE0-33A8-11D3-8658-0090272F9EEB")
WTETTSMode;
#endif

EXTERN_C const CLSID CLSID_WTESRMode;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DE1-33A8-11D3-8658-0090272F9EEB")
WTESRMode;
#endif

EXTERN_C const CLSID CLSID_WTEExOM;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DE2-33A8-11D3-8658-0090272F9EEB")
WTEExOM;
#endif

EXTERN_C const CLSID CLSID_WTELogCustomField;

#ifdef __cplusplus

class DECLSPEC_UUID("AFA50DE3-33A8-11D3-8658-0090272F9EEB")
WTELogCustomField;
#endif

EXTERN_C const CLSID CLSID_WTESnapinNode;

#ifdef __cplusplus

class DECLSPEC_UUID("b3a60310-401c-11d3-8659-0090272f9eeb")
WTESnapinNode;
#endif
#endif /* __mswteCom_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long __RPC_FAR *, unsigned long            , BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  BSTR_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, BSTR __RPC_FAR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long __RPC_FAR *, BSTR __RPC_FAR * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long __RPC_FAR *, unsigned long            , VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  VARIANT_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, VARIANT __RPC_FAR * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long __RPC_FAR *, VARIANT __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


