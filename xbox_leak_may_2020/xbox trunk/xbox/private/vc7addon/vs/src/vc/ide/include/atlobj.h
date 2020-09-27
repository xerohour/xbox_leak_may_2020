/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 5.01.0164 */
/* at Fri Dec 18 13:39:09 1998
 */
/* Compiler settings for atlobj.idl:
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

#ifndef __atlobj_h__
#define __atlobj_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ISymbolMap_FWD_DEFINED__
#define __ISymbolMap_FWD_DEFINED__
typedef interface ISymbolMap ISymbolMap;
#endif 	/* __ISymbolMap_FWD_DEFINED__ */


#ifndef __IAtlInterfaceWizard_FWD_DEFINED__
#define __IAtlInterfaceWizard_FWD_DEFINED__
typedef interface IAtlInterfaceWizard IAtlInterfaceWizard;
#endif 	/* __IAtlInterfaceWizard_FWD_DEFINED__ */


#ifndef __IAtlObjWiz_FWD_DEFINED__
#define __IAtlObjWiz_FWD_DEFINED__
typedef interface IAtlObjWiz IAtlObjWiz;
#endif 	/* __IAtlObjWiz_FWD_DEFINED__ */


#ifndef __IObjWiz_FWD_DEFINED__
#define __IObjWiz_FWD_DEFINED__
typedef interface IObjWiz IObjWiz;
#endif 	/* __IObjWiz_FWD_DEFINED__ */


#ifndef __ISymbolMapDisp_FWD_DEFINED__
#define __ISymbolMapDisp_FWD_DEFINED__
typedef interface ISymbolMapDisp ISymbolMapDisp;
#endif 	/* __ISymbolMapDisp_FWD_DEFINED__ */


#ifndef __IOLEDBConsCode_FWD_DEFINED__
#define __IOLEDBConsCode_FWD_DEFINED__
typedef interface IOLEDBConsCode IOLEDBConsCode;
#endif 	/* __IOLEDBConsCode_FWD_DEFINED__ */


#ifndef __CtlComp_FWD_DEFINED__
#define __CtlComp_FWD_DEFINED__

#ifdef __cplusplus
typedef class CtlComp CtlComp;
#else
typedef struct CtlComp CtlComp;
#endif /* __cplusplus */

#endif 	/* __CtlComp_FWD_DEFINED__ */


#ifndef __CNames_FWD_DEFINED__
#define __CNames_FWD_DEFINED__

#ifdef __cplusplus
typedef class CNames CNames;
#else
typedef struct CNames CNames;
#endif /* __cplusplus */

#endif 	/* __CNames_FWD_DEFINED__ */


#ifndef __CObjDlg_FWD_DEFINED__
#define __CObjDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CObjDlg CObjDlg;
#else
typedef struct CObjDlg CObjDlg;
#endif /* __cplusplus */

#endif 	/* __CObjDlg_FWD_DEFINED__ */


#ifndef __CCtlDlg_FWD_DEFINED__
#define __CCtlDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCtlDlg CCtlDlg;
#else
typedef struct CCtlDlg CCtlDlg;
#endif /* __cplusplus */

#endif 	/* __CCtlDlg_FWD_DEFINED__ */


#ifndef __CStockProps_FWD_DEFINED__
#define __CStockProps_FWD_DEFINED__

#ifdef __cplusplus
typedef class CStockProps CStockProps;
#else
typedef struct CStockProps CStockProps;
#endif /* __cplusplus */

#endif 	/* __CStockProps_FWD_DEFINED__ */


#ifndef __CPropDlg_FWD_DEFINED__
#define __CPropDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CPropDlg CPropDlg;
#else
typedef struct CPropDlg CPropDlg;
#endif /* __cplusplus */

#endif 	/* __CPropDlg_FWD_DEFINED__ */


#ifndef __CAddInDlg_FWD_DEFINED__
#define __CAddInDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAddInDlg CAddInDlg;
#else
typedef struct CAddInDlg CAddInDlg;
#endif /* __cplusplus */

#endif 	/* __CAddInDlg_FWD_DEFINED__ */


#ifndef __CViperDlg_FWD_DEFINED__
#define __CViperDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CViperDlg CViperDlg;
#else
typedef struct CViperDlg CViperDlg;
#endif /* __cplusplus */

#endif 	/* __CViperDlg_FWD_DEFINED__ */


#ifndef __CCompRegDlg_FWD_DEFINED__
#define __CCompRegDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CCompRegDlg CCompRegDlg;
#else
typedef struct CCompRegDlg CCompRegDlg;
#endif /* __cplusplus */

#endif 	/* __CCompRegDlg_FWD_DEFINED__ */


#ifndef __CDenaliDlg_FWD_DEFINED__
#define __CDenaliDlg_FWD_DEFINED__

#ifdef __cplusplus
typedef class CDenaliDlg CDenaliDlg;
#else
typedef struct CDenaliDlg CDenaliDlg;
#endif /* __cplusplus */

#endif 	/* __CDenaliDlg_FWD_DEFINED__ */


#ifndef __AtlInterfaceWizard_FWD_DEFINED__
#define __AtlInterfaceWizard_FWD_DEFINED__

#ifdef __cplusplus
typedef class AtlInterfaceWizard AtlInterfaceWizard;
#else
typedef struct AtlInterfaceWizard AtlInterfaceWizard;
#endif /* __cplusplus */

#endif 	/* __AtlInterfaceWizard_FWD_DEFINED__ */


#ifndef __ConsProp_FWD_DEFINED__
#define __ConsProp_FWD_DEFINED__

#ifdef __cplusplus
typedef class ConsProp ConsProp;
#else
typedef struct ConsProp ConsProp;
#endif /* __cplusplus */

#endif 	/* __ConsProp_FWD_DEFINED__ */


#ifndef __ProvName_FWD_DEFINED__
#define __ProvName_FWD_DEFINED__

#ifdef __cplusplus
typedef class ProvName ProvName;
#else
typedef struct ProvName ProvName;
#endif /* __cplusplus */

#endif 	/* __ProvName_FWD_DEFINED__ */


#ifndef __SnapInObj_FWD_DEFINED__
#define __SnapInObj_FWD_DEFINED__

#ifdef __cplusplus
typedef class SnapInObj SnapInObj;
#else
typedef struct SnapInObj SnapInObj;
#endif /* __cplusplus */

#endif 	/* __SnapInObj_FWD_DEFINED__ */


#ifndef __CtlDlg2_FWD_DEFINED__
#define __CtlDlg2_FWD_DEFINED__

#ifdef __cplusplus
typedef class CtlDlg2 CtlDlg2;
#else
typedef struct CtlDlg2 CtlDlg2;
#endif /* __cplusplus */

#endif 	/* __CtlDlg2_FWD_DEFINED__ */


#ifndef __Htmpage_FWD_DEFINED__
#define __Htmpage_FWD_DEFINED__

#ifdef __cplusplus
typedef class Htmpage Htmpage;
#else
typedef struct Htmpage Htmpage;
#endif /* __cplusplus */

#endif 	/* __Htmpage_FWD_DEFINED__ */


#ifndef __OLEDBConsCode_FWD_DEFINED__
#define __OLEDBConsCode_FWD_DEFINED__

#ifdef __cplusplus
typedef class OLEDBConsCode OLEDBConsCode;
#else
typedef struct OLEDBConsCode OLEDBConsCode;
#endif /* __cplusplus */

#endif 	/* __OLEDBConsCode_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ISymbolMap_INTERFACE_DEFINED__
#define __ISymbolMap_INTERFACE_DEFINED__

/* interface ISymbolMap */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_ISymbolMap;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C6D58201-1FA3-11D0-BF1E-0000E8D0D146")
    ISymbolMap : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ LPCOLESTR strSymbol,
            /* [in] */ LPCOLESTR strValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ LPCOLESTR strSymbol,
            /* [retval][out] */ BSTR __RPC_FAR *pstrValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStatus( 
            const CLSID __RPC_FAR *pclsid,
            BOOL bEnableOK) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISymbolMapVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISymbolMap __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISymbolMap __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISymbolMap __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISymbolMap __RPC_FAR * This,
            /* [in] */ LPCOLESTR strSymbol,
            /* [in] */ LPCOLESTR strValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISymbolMap __RPC_FAR * This,
            /* [in] */ LPCOLESTR strSymbol,
            /* [retval][out] */ BSTR __RPC_FAR *pstrValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            ISymbolMap __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetStatus )( 
            ISymbolMap __RPC_FAR * This,
            const CLSID __RPC_FAR *pclsid,
            BOOL bEnableOK);
        
        END_INTERFACE
    } ISymbolMapVtbl;

    interface ISymbolMap
    {
        CONST_VTBL struct ISymbolMapVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISymbolMap_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISymbolMap_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISymbolMap_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISymbolMap_Set(This,strSymbol,strValue)	\
    (This)->lpVtbl -> Set(This,strSymbol,strValue)

#define ISymbolMap_Get(This,strSymbol,pstrValue)	\
    (This)->lpVtbl -> Get(This,strSymbol,pstrValue)

#define ISymbolMap_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define ISymbolMap_SetStatus(This,pclsid,bEnableOK)	\
    (This)->lpVtbl -> SetStatus(This,pclsid,bEnableOK)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISymbolMap_Set_Proxy( 
    ISymbolMap __RPC_FAR * This,
    /* [in] */ LPCOLESTR strSymbol,
    /* [in] */ LPCOLESTR strValue);


void __RPC_STUB ISymbolMap_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMap_Get_Proxy( 
    ISymbolMap __RPC_FAR * This,
    /* [in] */ LPCOLESTR strSymbol,
    /* [retval][out] */ BSTR __RPC_FAR *pstrValue);


void __RPC_STUB ISymbolMap_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMap_Clear_Proxy( 
    ISymbolMap __RPC_FAR * This);


void __RPC_STUB ISymbolMap_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMap_SetStatus_Proxy( 
    ISymbolMap __RPC_FAR * This,
    const CLSID __RPC_FAR *pclsid,
    BOOL bEnableOK);


void __RPC_STUB ISymbolMap_SetStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISymbolMap_INTERFACE_DEFINED__ */


#ifndef __IAtlInterfaceWizard_INTERFACE_DEFINED__
#define __IAtlInterfaceWizard_INTERFACE_DEFINED__

/* interface IAtlInterfaceWizard */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IAtlInterfaceWizard;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0E491292-0A94-11D1-AF5E-00C04FB99436")
    IAtlInterfaceWizard : public IUnknown
    {
    public:
    };
    
#else 	/* C style interface */

    typedef struct IAtlInterfaceWizardVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAtlInterfaceWizard __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAtlInterfaceWizard __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAtlInterfaceWizard __RPC_FAR * This);
        
        END_INTERFACE
    } IAtlInterfaceWizardVtbl;

    interface IAtlInterfaceWizard
    {
        CONST_VTBL struct IAtlInterfaceWizardVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlInterfaceWizard_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAtlInterfaceWizard_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAtlInterfaceWizard_Release(This)	\
    (This)->lpVtbl -> Release(This)


#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IAtlInterfaceWizard_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_atlobj_0095 */
/* [local] */ 

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_atlobj_0095_0001
    {	atlDA_ODBC	= 0x1,
	atlDA_OLEDB	= 0x2
    }	atlDataAccessTypes;



extern RPC_IF_HANDLE __MIDL_itf_atlobj_0095_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_atlobj_0095_v0_0_s_ifspec;


#ifndef __ATLOBJLib_LIBRARY_DEFINED__
#define __ATLOBJLib_LIBRARY_DEFINED__

/* library ATLOBJLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_ATLOBJLib;

#ifndef __IAtlObjWiz_INTERFACE_DEFINED__
#define __IAtlObjWiz_INTERFACE_DEFINED__

/* interface IAtlObjWiz */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IAtlObjWiz;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("50B42014-8C8A-11d1-B017-00C04FB99436")
    IAtlObjWiz : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE ValidateSymbol( 
            BSTR symbol) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAtlObjWizVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IAtlObjWiz __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IAtlObjWiz __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IAtlObjWiz __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ValidateSymbol )( 
            IAtlObjWiz __RPC_FAR * This,
            BSTR symbol);
        
        END_INTERFACE
    } IAtlObjWizVtbl;

    interface IAtlObjWiz
    {
        CONST_VTBL struct IAtlObjWizVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAtlObjWiz_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAtlObjWiz_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAtlObjWiz_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAtlObjWiz_ValidateSymbol(This,symbol)	\
    (This)->lpVtbl -> ValidateSymbol(This,symbol)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IAtlObjWiz_ValidateSymbol_Proxy( 
    IAtlObjWiz __RPC_FAR * This,
    BSTR symbol);


void __RPC_STUB IAtlObjWiz_ValidateSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAtlObjWiz_INTERFACE_DEFINED__ */


#ifndef __IObjWiz_INTERFACE_DEFINED__
#define __IObjWiz_INTERFACE_DEFINED__

/* interface IObjWiz */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IObjWiz;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("f16680a6-7852-11d2-81c3-00c04f797ab7")
    IObjWiz : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE IsValidClassSymbol( 
            BSTR symbol) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IObjWizVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IObjWiz __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IObjWiz __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IObjWiz __RPC_FAR * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *IsValidClassSymbol )( 
            IObjWiz __RPC_FAR * This,
            BSTR symbol);
        
        END_INTERFACE
    } IObjWizVtbl;

    interface IObjWiz
    {
        CONST_VTBL struct IObjWizVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IObjWiz_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IObjWiz_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IObjWiz_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IObjWiz_IsValidClassSymbol(This,symbol)	\
    (This)->lpVtbl -> IsValidClassSymbol(This,symbol)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE IObjWiz_IsValidClassSymbol_Proxy( 
    IObjWiz __RPC_FAR * This,
    BSTR symbol);


void __RPC_STUB IObjWiz_IsValidClassSymbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IObjWiz_INTERFACE_DEFINED__ */


#ifndef __ISymbolMapDisp_INTERFACE_DEFINED__
#define __ISymbolMapDisp_INTERFACE_DEFINED__

/* interface ISymbolMapDisp */
/* [unique][helpstring][uuid][dual][object] */ 


EXTERN_C const IID IID_ISymbolMapDisp;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("262CBA84-4105-11d1-AFB0-00C04FB99436")
    ISymbolMapDisp : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Set( 
            /* [in] */ BSTR strSymbol,
            /* [in] */ BSTR strValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Get( 
            /* [in] */ BSTR strSymbol,
            /* [retval][out] */ BSTR __RPC_FAR *pstrValue) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clear( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStatus( 
            VARIANT_BOOL bEnableOK) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISymbolMapDispVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ISymbolMapDisp __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ISymbolMapDisp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Set )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ BSTR strSymbol,
            /* [in] */ BSTR strValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Get )( 
            ISymbolMapDisp __RPC_FAR * This,
            /* [in] */ BSTR strSymbol,
            /* [retval][out] */ BSTR __RPC_FAR *pstrValue);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Clear )( 
            ISymbolMapDisp __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetStatus )( 
            ISymbolMapDisp __RPC_FAR * This,
            VARIANT_BOOL bEnableOK);
        
        END_INTERFACE
    } ISymbolMapDispVtbl;

    interface ISymbolMapDisp
    {
        CONST_VTBL struct ISymbolMapDispVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISymbolMapDisp_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ISymbolMapDisp_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ISymbolMapDisp_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ISymbolMapDisp_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ISymbolMapDisp_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ISymbolMapDisp_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ISymbolMapDisp_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ISymbolMapDisp_Set(This,strSymbol,strValue)	\
    (This)->lpVtbl -> Set(This,strSymbol,strValue)

#define ISymbolMapDisp_Get(This,strSymbol,pstrValue)	\
    (This)->lpVtbl -> Get(This,strSymbol,pstrValue)

#define ISymbolMapDisp_Clear(This)	\
    (This)->lpVtbl -> Clear(This)

#define ISymbolMapDisp_SetStatus(This,bEnableOK)	\
    (This)->lpVtbl -> SetStatus(This,bEnableOK)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ISymbolMapDisp_Set_Proxy( 
    ISymbolMapDisp __RPC_FAR * This,
    /* [in] */ BSTR strSymbol,
    /* [in] */ BSTR strValue);


void __RPC_STUB ISymbolMapDisp_Set_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMapDisp_Get_Proxy( 
    ISymbolMapDisp __RPC_FAR * This,
    /* [in] */ BSTR strSymbol,
    /* [retval][out] */ BSTR __RPC_FAR *pstrValue);


void __RPC_STUB ISymbolMapDisp_Get_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMapDisp_Clear_Proxy( 
    ISymbolMapDisp __RPC_FAR * This);


void __RPC_STUB ISymbolMapDisp_Clear_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ISymbolMapDisp_SetStatus_Proxy( 
    ISymbolMapDisp __RPC_FAR * This,
    VARIANT_BOOL bEnableOK);


void __RPC_STUB ISymbolMapDisp_SetStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ISymbolMapDisp_INTERFACE_DEFINED__ */


#ifndef __IOLEDBConsCode_INTERFACE_DEFINED__
#define __IOLEDBConsCode_INTERFACE_DEFINED__

/* interface IOLEDBConsCode */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IOLEDBConsCode;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F6F76FF2-30A6-11D2-8345-00C04F797AAA")
    IOLEDBConsCode : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE PromptUser( 
            /* [in] */ HWND hParent,
            atlDataAccessTypes iDataAccessType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ProcessSettings( void) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Code( 
            /* [out] */ BSTR __RPC_FAR *pCode) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ClassName( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ClassName( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionString( 
            /* [retval][out] */ BSTR __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionString( 
            /* [in] */ BSTR newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Command( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Command( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_DataSource( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_DataSource( 
            /* [in] */ IUnknown __RPC_FAR *newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Procedure( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Procedure( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Update( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Update( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Insert( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Insert( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [propget] */ HRESULT STDMETHODCALLTYPE get_Delete( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [propput] */ HRESULT STDMETHODCALLTYPE put_Delete( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE get_ODBCSnapshot( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE put_ODBCSnapshot( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IOLEDBConsCodeVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IOLEDBConsCode __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IOLEDBConsCode __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PromptUser )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ HWND hParent,
            atlDataAccessTypes iDataAccessType);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ProcessSettings )( 
            IOLEDBConsCode __RPC_FAR * This);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Code )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [out] */ BSTR __RPC_FAR *pCode);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ClassName )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ClassName )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionString )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionString )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BSTR newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Command )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Command )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DataSource )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_DataSource )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Procedure )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Procedure )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Update )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Update )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Insert )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Insert )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Delete )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Delete )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ODBCSnapshot )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ODBCSnapshot )( 
            IOLEDBConsCode __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IOLEDBConsCodeVtbl;

    interface IOLEDBConsCode
    {
        CONST_VTBL struct IOLEDBConsCodeVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IOLEDBConsCode_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IOLEDBConsCode_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IOLEDBConsCode_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IOLEDBConsCode_PromptUser(This,hParent,iDataAccessType)	\
    (This)->lpVtbl -> PromptUser(This,hParent,iDataAccessType)

#define IOLEDBConsCode_ProcessSettings(This)	\
    (This)->lpVtbl -> ProcessSettings(This)

#define IOLEDBConsCode_get_Code(This,pCode)	\
    (This)->lpVtbl -> get_Code(This,pCode)

#define IOLEDBConsCode_get_Name(This,pVal)	\
    (This)->lpVtbl -> get_Name(This,pVal)

#define IOLEDBConsCode_put_Name(This,newVal)	\
    (This)->lpVtbl -> put_Name(This,newVal)

#define IOLEDBConsCode_get_ClassName(This,pVal)	\
    (This)->lpVtbl -> get_ClassName(This,pVal)

#define IOLEDBConsCode_put_ClassName(This,newVal)	\
    (This)->lpVtbl -> put_ClassName(This,newVal)

#define IOLEDBConsCode_get_ConnectionString(This,pVal)	\
    (This)->lpVtbl -> get_ConnectionString(This,pVal)

#define IOLEDBConsCode_put_ConnectionString(This,newVal)	\
    (This)->lpVtbl -> put_ConnectionString(This,newVal)

#define IOLEDBConsCode_get_Command(This,pVal)	\
    (This)->lpVtbl -> get_Command(This,pVal)

#define IOLEDBConsCode_put_Command(This,newVal)	\
    (This)->lpVtbl -> put_Command(This,newVal)

#define IOLEDBConsCode_get_DataSource(This,pVal)	\
    (This)->lpVtbl -> get_DataSource(This,pVal)

#define IOLEDBConsCode_put_DataSource(This,newVal)	\
    (This)->lpVtbl -> put_DataSource(This,newVal)

#define IOLEDBConsCode_get_Procedure(This,pVal)	\
    (This)->lpVtbl -> get_Procedure(This,pVal)

#define IOLEDBConsCode_put_Procedure(This,newVal)	\
    (This)->lpVtbl -> put_Procedure(This,newVal)

#define IOLEDBConsCode_get_Update(This,pVal)	\
    (This)->lpVtbl -> get_Update(This,pVal)

#define IOLEDBConsCode_put_Update(This,newVal)	\
    (This)->lpVtbl -> put_Update(This,newVal)

#define IOLEDBConsCode_get_Insert(This,pVal)	\
    (This)->lpVtbl -> get_Insert(This,pVal)

#define IOLEDBConsCode_put_Insert(This,newVal)	\
    (This)->lpVtbl -> put_Insert(This,newVal)

#define IOLEDBConsCode_get_Delete(This,pVal)	\
    (This)->lpVtbl -> get_Delete(This,pVal)

#define IOLEDBConsCode_put_Delete(This,newVal)	\
    (This)->lpVtbl -> put_Delete(This,newVal)

#define IOLEDBConsCode_get_ODBCSnapshot(This,pVal)	\
    (This)->lpVtbl -> get_ODBCSnapshot(This,pVal)

#define IOLEDBConsCode_put_ODBCSnapshot(This,newVal)	\
    (This)->lpVtbl -> put_ODBCSnapshot(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IOLEDBConsCode_PromptUser_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ HWND hParent,
    atlDataAccessTypes iDataAccessType);


void __RPC_STUB IOLEDBConsCode_PromptUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IOLEDBConsCode_ProcessSettings_Proxy( 
    IOLEDBConsCode __RPC_FAR * This);


void __RPC_STUB IOLEDBConsCode_ProcessSettings_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Code_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [out] */ BSTR __RPC_FAR *pCode);


void __RPC_STUB IOLEDBConsCode_get_Code_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Name_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Name_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOLEDBConsCode_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_ClassName_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_ClassName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_ClassName_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOLEDBConsCode_put_ClassName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_ConnectionString_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_ConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_ConnectionString_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BSTR newVal);


void __RPC_STUB IOLEDBConsCode_put_ConnectionString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Command_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Command_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_Command_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_DataSource_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_DataSource_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *newVal);


void __RPC_STUB IOLEDBConsCode_put_DataSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Procedure_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Procedure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Procedure_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_Procedure_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Update_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Update_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Insert_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Insert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Insert_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_Insert_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_Delete_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_Delete_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propget] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_get_ODBCSnapshot_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IOLEDBConsCode_get_ODBCSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][propput] */ HRESULT STDMETHODCALLTYPE IOLEDBConsCode_put_ODBCSnapshot_Proxy( 
    IOLEDBConsCode __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IOLEDBConsCode_put_ODBCSnapshot_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IOLEDBConsCode_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CtlComp;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc6990d-5f09-11d2-81a1-00c04f797ab7")
CtlComp;
#endif

EXTERN_C const CLSID CLSID_CNames;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc6990e-5f09-11d2-81a1-00c04f797ab7")
CNames;
#endif

EXTERN_C const CLSID CLSID_CObjDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc6990f-5f09-11d2-81a1-00c04f797ab7")
CObjDlg;
#endif

EXTERN_C const CLSID CLSID_CCtlDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69910-5f09-11d2-81a1-00c04f797ab7")
CCtlDlg;
#endif

EXTERN_C const CLSID CLSID_CStockProps;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69911-5f09-11d2-81a1-00c04f797ab7")
CStockProps;
#endif

EXTERN_C const CLSID CLSID_CPropDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69912-5f09-11d2-81a1-00c04f797ab7")
CPropDlg;
#endif

EXTERN_C const CLSID CLSID_CAddInDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69913-5f09-11d2-81a1-00c04f797ab7")
CAddInDlg;
#endif

EXTERN_C const CLSID CLSID_CViperDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69914-5f09-11d2-81a1-00c04f797ab7")
CViperDlg;
#endif

EXTERN_C const CLSID CLSID_CCompRegDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69915-5f09-11d2-81a1-00c04f797ab7")
CCompRegDlg;
#endif

EXTERN_C const CLSID CLSID_CDenaliDlg;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69916-5f09-11d2-81a1-00c04f797ab7")
CDenaliDlg;
#endif

EXTERN_C const CLSID CLSID_AtlInterfaceWizard;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69917-5f09-11d2-81a1-00c04f797ab7")
AtlInterfaceWizard;
#endif

EXTERN_C const CLSID CLSID_ConsProp;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69918-5f09-11d2-81a1-00c04f797ab7")
ConsProp;
#endif

EXTERN_C const CLSID CLSID_ProvName;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc69919-5f09-11d2-81a1-00c04f797ab7")
ProvName;
#endif

EXTERN_C const CLSID CLSID_SnapInObj;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc6991A-5f09-11d2-81a1-00c04f797ab7")
SnapInObj;
#endif

EXTERN_C const CLSID CLSID_CtlDlg2;

#ifdef __cplusplus

class DECLSPEC_UUID("7230C7C3-6454-11D1-AAD8-000000000000")
CtlDlg2;
#endif

EXTERN_C const CLSID CLSID_Htmpage;

#ifdef __cplusplus

class DECLSPEC_UUID("6bc6991B-5f09-11d2-81a1-00c04f797ab7")
Htmpage;
#endif

EXTERN_C const CLSID CLSID_OLEDBConsCode;

#ifdef __cplusplus

class DECLSPEC_UUID("F6F76FF3-30A6-11D2-8345-00C04F797AAA")
OLEDBConsCode;
#endif
#endif /* __ATLOBJLib_LIBRARY_DEFINED__ */

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
