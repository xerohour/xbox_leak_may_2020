/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.59 */
/* at Mon Nov 25 18:35:22 1996
 */
/* Compiler settings for ocdesign.idl:
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

#ifndef __ocdesign_h__
#define __ocdesign_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IBuilderWizardManager_FWD_DEFINED__
#define __IBuilderWizardManager_FWD_DEFINED__
typedef interface IBuilderWizardManager IBuilderWizardManager;
#endif 	/* __IBuilderWizardManager_FWD_DEFINED__ */


#ifndef __IProvidePropertyBuilder_FWD_DEFINED__
#define __IProvidePropertyBuilder_FWD_DEFINED__
typedef interface IProvidePropertyBuilder IProvidePropertyBuilder;
#endif 	/* __IProvidePropertyBuilder_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_ocdesign_0000
 * at Mon Nov 25 18:35:22 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1993 - 1996.
//
//--------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////
// IBuilderWizardManager Component Categories
// 
///////////////////////////////////////////////////////////////////////////////
// CATID_HTMLBuilder (HTML builder signature)
// 1. IDispatch  - Application object
// 2. long       - hwndPromptOwner 
// 3. IUnknown   - IServiceProvider
// 4. long       - Width
// 5. long       - Height
// 6. BSTR       - ParameterString
// 7. IDispatch  - Container
// 8. [OUT] BSTR - HTMLString
// 
// {73CEF3D4-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_HTMLBuilder, 
0x73cef3d4, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_ObjectBuilder (activeX control builder signature)
// 1. IDispatch        - Application object
// 2. long             - hwndPromptOwner 
// 3. [IN/OUT] VARIANT - current property value, new property value
// 
// {73CEF3D5-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_ObjectBuilder, 
0x73cef3d5, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_PropertyBuilder (activeX control property builder signature)
// 
// {73CEF3D6-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_PropertyBuilder, 
0x73cef3d6, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_URLBuilder (URL builder signature)
// 1.  IDispatch        - Application object
// 2.  long             - hwndPromptOwner 
// 3.  IUnknown         - IServiceProvider
// 4.  [IN/OUT] VARIANT - current/new url value
// 5.  BSTR             - base url
// 6.  BSTR             - additional filters
// 7.  BSTR             - custom title
// 8.  [IN/OUT] VARIANT - target frame value
// 9.  [IN/OUT] long    - flags
// 10. [OUT]    bool    - return value
// 
// {73CEF3D9-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_URLBuilder, 
0x73cef3d9, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_ColorBuilder (activeX control color builder signature)
// 1. IDispatch        - Application object
// 2. long             - hwndPromptOwner 
// 3. [IN/OUT] VARIANT - current color, new color value
// 
// {73CEF3DA-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_ColorBuilder, 
0x73cef3da, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_FontBuilder (activeX control font builder signature)
// 1. IDispatch        - Application object
// 2. long             - hwndPromptOwner 
// 3. [IN/OUT] VARIANT - (IDispatch) current color, new color value
// 
// {73CEF3DB-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_FontBuilder, 
0x73cef3db, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
// {73CEF3DE-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_ActiveXControlBuilder, 
0x73cef3de, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_ScriptBuilder (script builder signature)
// 1. BSTR - Default Language.
// 2. long - HKey of the Application
// 
// {73CEF3DF-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_ScriptBuilder, 
0x73cef3df, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// CATID_PictureBuilder (activeX control picture builder signature)
// 1. IDispatch        - Application object
// 2. long             - hwndPromptOwner 
// 3. [IN/OUT] VARIANT - (IDispatch) current picture, new picture value
// 
// {73CEF3E0-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_PictureBuilder, 
0x73cef3e0, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// IBuilderWizardManager Interface
// 
// {95FC88C2-9FCB-11cf-A405-00AA00C00940}
DEFINE_GUID(SID_SBuilderWizardManager,
0x95fc88c2, 0x9fcb, 0x11cf, 0xa4, 0x5, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
// {95FC88C3-9FCB-11cf-A405-00AA00C00940}
DEFINE_GUID(IID_IBuilderWizardManager,
0x95fc88c3, 0x9fcb, 0x11cf, 0xa4, 0x5, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
#ifndef _tagBLDPROMPTOPT_DEFINED
#define _tagBLDPROMPTOPT_DEFINED
#define _BLDPROMPTOPT_DEFINED
typedef 
enum tagBLDPROMPTOPT
    {	BLDPROMPTOPT_PROMPTIFMULTIPLE	= 0,
	BLDPROMPTOPT_PROMPTALWAYS	= 1,
	BLDPROMPTOPT_PROMPTNEVER	= 2
    }	BLDPROMPTOPT;

#endif
#ifndef _tagBLDGETOPT_DEFINED
#define _tagBLDGETOPT_DEFINED
#define _BLDGETOPT_DEFINED
typedef 
enum tagBLDGETOPT
    {	BLDGETOPT_FAUTOMAPGUID	= 0x1,
	BLDGETOPT_FAUTOMAPENABLEPROMPT	= 0x2,
	BLDGETOPT_FAUTOMAPPROMPTALWAYS	= 0x4,
	BLDGETOPT_FOBJECTBUILDER	= 0x8,
	BLDGETOPT_FNOINTRINSICS	= 0x80000000
    }	BLDGETFLAGS;

#endif


extern RPC_IF_HANDLE __MIDL_itf_ocdesign_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocdesign_0000_v0_0_s_ifspec;

#ifndef __IBuilderWizardManager_INTERFACE_DEFINED__
#define __IBuilderWizardManager_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IBuilderWizardManager
 * at Mon Nov 25 18:35:22 1996
 * using MIDL 3.01.59
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IBuilderWizardManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("95FC88C3-9FCB-11cf-A405-00AA00C00940")
    IBuilderWizardManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DoesBuilderExist( 
            /* [in] */ REFGUID rguidBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapObjectToBuilderCLSID( 
            /* [in] */ REFCLSID rclsidObject,
            /* [in] */ DWORD dwPromptOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ CLSID __RPC_FAR *pclsidBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapBuilderCATIDToCLSID( 
            /* [in] */ REFGUID rguidBuilder,
            /* [in] */ DWORD dwPromptOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ CLSID __RPC_FAR *pclsidBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBuilder( 
            /* [in] */ REFGUID rguidBuilder,
            /* [in] */ DWORD grfGetOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispApp,
            /* [out] */ HWND __RPC_FAR *pwndBuilderOwner,
            /* [in] */ REFIID riidBuilder,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkBuilder) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBuilderWizardManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IBuilderWizardManager __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IBuilderWizardManager __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DoesBuilderExist )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ REFGUID rguidBuilder);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MapObjectToBuilderCLSID )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ REFCLSID rclsidObject,
            /* [in] */ DWORD dwPromptOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ CLSID __RPC_FAR *pclsidBuilder);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MapBuilderCATIDToCLSID )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ REFGUID rguidBuilder,
            /* [in] */ DWORD dwPromptOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ CLSID __RPC_FAR *pclsidBuilder);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetBuilder )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ REFGUID rguidBuilder,
            /* [in] */ DWORD grfGetOpt,
            /* [in] */ HWND hwndPromptOwner,
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispApp,
            /* [out] */ HWND __RPC_FAR *pwndBuilderOwner,
            /* [in] */ REFIID riidBuilder,
            /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkBuilder);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *EnableModeless )( 
            IBuilderWizardManager __RPC_FAR * This,
            /* [in] */ BOOL fEnable);
        
        END_INTERFACE
    } IBuilderWizardManagerVtbl;

    interface IBuilderWizardManager
    {
        CONST_VTBL struct IBuilderWizardManagerVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBuilderWizardManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBuilderWizardManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBuilderWizardManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBuilderWizardManager_DoesBuilderExist(This,rguidBuilder)	\
    (This)->lpVtbl -> DoesBuilderExist(This,rguidBuilder)

#define IBuilderWizardManager_MapObjectToBuilderCLSID(This,rclsidObject,dwPromptOpt,hwndPromptOwner,pclsidBuilder)	\
    (This)->lpVtbl -> MapObjectToBuilderCLSID(This,rclsidObject,dwPromptOpt,hwndPromptOwner,pclsidBuilder)

#define IBuilderWizardManager_MapBuilderCATIDToCLSID(This,rguidBuilder,dwPromptOpt,hwndPromptOwner,pclsidBuilder)	\
    (This)->lpVtbl -> MapBuilderCATIDToCLSID(This,rguidBuilder,dwPromptOpt,hwndPromptOwner,pclsidBuilder)

#define IBuilderWizardManager_GetBuilder(This,rguidBuilder,grfGetOpt,hwndPromptOwner,ppdispApp,pwndBuilderOwner,riidBuilder,ppunkBuilder)	\
    (This)->lpVtbl -> GetBuilder(This,rguidBuilder,grfGetOpt,hwndPromptOwner,ppdispApp,pwndBuilderOwner,riidBuilder,ppunkBuilder)

#define IBuilderWizardManager_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBuilderWizardManager_DoesBuilderExist_Proxy( 
    IBuilderWizardManager __RPC_FAR * This,
    /* [in] */ REFGUID rguidBuilder);


void __RPC_STUB IBuilderWizardManager_DoesBuilderExist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBuilderWizardManager_MapObjectToBuilderCLSID_Proxy( 
    IBuilderWizardManager __RPC_FAR * This,
    /* [in] */ REFCLSID rclsidObject,
    /* [in] */ DWORD dwPromptOpt,
    /* [in] */ HWND hwndPromptOwner,
    /* [out] */ CLSID __RPC_FAR *pclsidBuilder);


void __RPC_STUB IBuilderWizardManager_MapObjectToBuilderCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBuilderWizardManager_MapBuilderCATIDToCLSID_Proxy( 
    IBuilderWizardManager __RPC_FAR * This,
    /* [in] */ REFGUID rguidBuilder,
    /* [in] */ DWORD dwPromptOpt,
    /* [in] */ HWND hwndPromptOwner,
    /* [out] */ CLSID __RPC_FAR *pclsidBuilder);


void __RPC_STUB IBuilderWizardManager_MapBuilderCATIDToCLSID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBuilderWizardManager_GetBuilder_Proxy( 
    IBuilderWizardManager __RPC_FAR * This,
    /* [in] */ REFGUID rguidBuilder,
    /* [in] */ DWORD grfGetOpt,
    /* [in] */ HWND hwndPromptOwner,
    /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppdispApp,
    /* [out] */ HWND __RPC_FAR *pwndBuilderOwner,
    /* [in] */ REFIID riidBuilder,
    /* [out] */ IUnknown __RPC_FAR *__RPC_FAR *ppunkBuilder);


void __RPC_STUB IBuilderWizardManager_GetBuilder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBuilderWizardManager_EnableModeless_Proxy( 
    IBuilderWizardManager __RPC_FAR * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IBuilderWizardManager_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBuilderWizardManager_INTERFACE_DEFINED__ */


/****************************************
 * Generated header for interface: __MIDL_itf_ocdesign_0096
 * at Mon Nov 25 18:35:22 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


///////////////////////////////////////////////////////////////////////////////
// IProvidePropertyBuilder Interface
// 
#ifndef _tagCTLBLDRTYPE_DEFINED
#define _tagCTLBLDRTYPE_DEFINED
#define _CTLBLDRTYPE_DEFINED
typedef 
enum tagCTLBLDTYPE
    {	CTLBLDTYPE_FSTDPROPBUILDER	= 0x1,
	CTLBLDTYPE_FINTERNALBUILDER	= 0x2,
	CTLBLDTYPE_FEDITSOBJDIRECTLY	= 0x4
    }	CTLBLDTYPE;

#endif
// {95FC88C1-9FCB-11cf-A405-00AA00C00940}
DEFINE_GUID(IID_IProvidePropertyBuilder,
0x95fc88c1, 0x9fcb, 0x11cf, 0xa4, 0x5, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);


extern RPC_IF_HANDLE __MIDL_itf_ocdesign_0096_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocdesign_0096_v0_0_s_ifspec;


#ifndef __MSADCTL_LIBRARY_DEFINED__
#define __MSADCTL_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: MSADCTL
 * at Mon Nov 25 18:35:22 1996
 * using MIDL 3.01.59
 ****************************************/
/* [version][lcid][helpstring][uuid] */ 



EXTERN_C const IID LIBID_MSADCTL;

#ifndef __IProvidePropertyBuilder_INTERFACE_DEFINED__
#define __IProvidePropertyBuilder_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IProvidePropertyBuilder
 * at Mon Nov 25 18:35:22 1996
 * using MIDL 3.01.59
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IProvidePropertyBuilder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("95FC88C1-9FCB-11cf-A405-00AA00C00940")
    IProvidePropertyBuilder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE MapPropertyToBuilder( 
            /* [in] */ LONG dispid,
            /* [out][in] */ LONG __RPC_FAR *pdwCtlBldType,
            /* [out][in] */ BSTR __RPC_FAR *pbstrGuidBldr,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ExecuteBuilder( 
            /* [in] */ LONG dispid,
            /* [in] */ BSTR bstrGuidBldr,
            /* [in] */ IDispatch __RPC_FAR *pdispApp,
            /* [in] */ LONG hwndBldrOwner,
            /* [out][in] */ VARIANT __RPC_FAR *pvarValue,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProvidePropertyBuilderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IProvidePropertyBuilder __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IProvidePropertyBuilder __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IProvidePropertyBuilder __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MapPropertyToBuilder )( 
            IProvidePropertyBuilder __RPC_FAR * This,
            /* [in] */ LONG dispid,
            /* [out][in] */ LONG __RPC_FAR *pdwCtlBldType,
            /* [out][in] */ BSTR __RPC_FAR *pbstrGuidBldr,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ExecuteBuilder )( 
            IProvidePropertyBuilder __RPC_FAR * This,
            /* [in] */ LONG dispid,
            /* [in] */ BSTR bstrGuidBldr,
            /* [in] */ IDispatch __RPC_FAR *pdispApp,
            /* [in] */ LONG hwndBldrOwner,
            /* [out][in] */ VARIANT __RPC_FAR *pvarValue,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal);
        
        END_INTERFACE
    } IProvidePropertyBuilderVtbl;

    interface IProvidePropertyBuilder
    {
        CONST_VTBL struct IProvidePropertyBuilderVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProvidePropertyBuilder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProvidePropertyBuilder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProvidePropertyBuilder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProvidePropertyBuilder_MapPropertyToBuilder(This,dispid,pdwCtlBldType,pbstrGuidBldr,pfRetVal)	\
    (This)->lpVtbl -> MapPropertyToBuilder(This,dispid,pdwCtlBldType,pbstrGuidBldr,pfRetVal)

#define IProvidePropertyBuilder_ExecuteBuilder(This,dispid,bstrGuidBldr,pdispApp,hwndBldrOwner,pvarValue,pfRetVal)	\
    (This)->lpVtbl -> ExecuteBuilder(This,dispid,bstrGuidBldr,pdispApp,hwndBldrOwner,pvarValue,pfRetVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IProvidePropertyBuilder_MapPropertyToBuilder_Proxy( 
    IProvidePropertyBuilder __RPC_FAR * This,
    /* [in] */ LONG dispid,
    /* [out][in] */ LONG __RPC_FAR *pdwCtlBldType,
    /* [out][in] */ BSTR __RPC_FAR *pbstrGuidBldr,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal);


void __RPC_STUB IProvidePropertyBuilder_MapPropertyToBuilder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IProvidePropertyBuilder_ExecuteBuilder_Proxy( 
    IProvidePropertyBuilder __RPC_FAR * This,
    /* [in] */ LONG dispid,
    /* [in] */ BSTR bstrGuidBldr,
    /* [in] */ IDispatch __RPC_FAR *pdispApp,
    /* [in] */ LONG hwndBldrOwner,
    /* [out][in] */ VARIANT __RPC_FAR *pvarValue,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pfRetVal);


void __RPC_STUB IProvidePropertyBuilder_ExecuteBuilder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProvidePropertyBuilder_INTERFACE_DEFINED__ */

#endif /* __MSADCTL_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long __RPC_FAR *, unsigned long            , HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long __RPC_FAR *, HWND __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
