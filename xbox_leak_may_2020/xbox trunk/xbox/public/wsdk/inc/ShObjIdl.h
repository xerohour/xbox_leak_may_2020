
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0334 */
/* Compiler settings for shobjidl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext
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

#ifndef __shobjidl_h__
#define __shobjidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPersistFolder_FWD_DEFINED__
#define __IPersistFolder_FWD_DEFINED__
typedef interface IPersistFolder IPersistFolder;
#endif 	/* __IPersistFolder_FWD_DEFINED__ */


#ifndef __IPersistFolder2_FWD_DEFINED__
#define __IPersistFolder2_FWD_DEFINED__
typedef interface IPersistFolder2 IPersistFolder2;
#endif 	/* __IPersistFolder2_FWD_DEFINED__ */


#ifndef __IPersistIDList_FWD_DEFINED__
#define __IPersistIDList_FWD_DEFINED__
typedef interface IPersistIDList IPersistIDList;
#endif 	/* __IPersistIDList_FWD_DEFINED__ */


#ifndef __IEnumIDList_FWD_DEFINED__
#define __IEnumIDList_FWD_DEFINED__
typedef interface IEnumIDList IEnumIDList;
#endif 	/* __IEnumIDList_FWD_DEFINED__ */


#ifndef __IShellFolder_FWD_DEFINED__
#define __IShellFolder_FWD_DEFINED__
typedef interface IShellFolder IShellFolder;
#endif 	/* __IShellFolder_FWD_DEFINED__ */


#ifndef __IEnumExtraSearch_FWD_DEFINED__
#define __IEnumExtraSearch_FWD_DEFINED__
typedef interface IEnumExtraSearch IEnumExtraSearch;
#endif 	/* __IEnumExtraSearch_FWD_DEFINED__ */


#ifndef __IShellFolder2_FWD_DEFINED__
#define __IShellFolder2_FWD_DEFINED__
typedef interface IShellFolder2 IShellFolder2;
#endif 	/* __IShellFolder2_FWD_DEFINED__ */


#ifndef __IShellView_FWD_DEFINED__
#define __IShellView_FWD_DEFINED__
typedef interface IShellView IShellView;
#endif 	/* __IShellView_FWD_DEFINED__ */


#ifndef __IShellView2_FWD_DEFINED__
#define __IShellView2_FWD_DEFINED__
typedef interface IShellView2 IShellView2;
#endif 	/* __IShellView2_FWD_DEFINED__ */


#ifndef __IFolderView_FWD_DEFINED__
#define __IFolderView_FWD_DEFINED__
typedef interface IFolderView IFolderView;
#endif 	/* __IFolderView_FWD_DEFINED__ */


#ifndef __IShellBrowser_FWD_DEFINED__
#define __IShellBrowser_FWD_DEFINED__
typedef interface IShellBrowser IShellBrowser;
#endif 	/* __IShellBrowser_FWD_DEFINED__ */


#ifndef __IShellNetCrawler_FWD_DEFINED__
#define __IShellNetCrawler_FWD_DEFINED__
typedef interface IShellNetCrawler IShellNetCrawler;
#endif 	/* __IShellNetCrawler_FWD_DEFINED__ */


#ifndef __IProfferService_FWD_DEFINED__
#define __IProfferService_FWD_DEFINED__
typedef interface IProfferService IProfferService;
#endif 	/* __IProfferService_FWD_DEFINED__ */


#ifndef __ICompositeFolder_FWD_DEFINED__
#define __ICompositeFolder_FWD_DEFINED__
typedef interface ICompositeFolder ICompositeFolder;
#endif 	/* __ICompositeFolder_FWD_DEFINED__ */


#ifndef __IParentAndItem_FWD_DEFINED__
#define __IParentAndItem_FWD_DEFINED__
typedef interface IParentAndItem IParentAndItem;
#endif 	/* __IParentAndItem_FWD_DEFINED__ */


#ifndef __IPropertyUI_FWD_DEFINED__
#define __IPropertyUI_FWD_DEFINED__
typedef interface IPropertyUI IPropertyUI;
#endif 	/* __IPropertyUI_FWD_DEFINED__ */


#ifndef __ICategoryProvider_FWD_DEFINED__
#define __ICategoryProvider_FWD_DEFINED__
typedef interface ICategoryProvider ICategoryProvider;
#endif 	/* __ICategoryProvider_FWD_DEFINED__ */


#ifndef __ICategorizer_FWD_DEFINED__
#define __ICategorizer_FWD_DEFINED__
typedef interface ICategorizer ICategorizer;
#endif 	/* __ICategorizer_FWD_DEFINED__ */


#ifndef __IShellItem_FWD_DEFINED__
#define __IShellItem_FWD_DEFINED__
typedef interface IShellItem IShellItem;
#endif 	/* __IShellItem_FWD_DEFINED__ */


#ifndef __IEnumShellItems_FWD_DEFINED__
#define __IEnumShellItems_FWD_DEFINED__
typedef interface IEnumShellItems IEnumShellItems;
#endif 	/* __IEnumShellItems_FWD_DEFINED__ */


#ifndef __IItemHandler_FWD_DEFINED__
#define __IItemHandler_FWD_DEFINED__
typedef interface IItemHandler IItemHandler;
#endif 	/* __IItemHandler_FWD_DEFINED__ */


#ifndef __ILocalCopy_FWD_DEFINED__
#define __ILocalCopy_FWD_DEFINED__
typedef interface ILocalCopy ILocalCopy;
#endif 	/* __ILocalCopy_FWD_DEFINED__ */


#ifndef __IShellLinkA_FWD_DEFINED__
#define __IShellLinkA_FWD_DEFINED__
typedef interface IShellLinkA IShellLinkA;
#endif 	/* __IShellLinkA_FWD_DEFINED__ */


#ifndef __IShellLinkW_FWD_DEFINED__
#define __IShellLinkW_FWD_DEFINED__
typedef interface IShellLinkW IShellLinkW;
#endif 	/* __IShellLinkW_FWD_DEFINED__ */


#ifndef __IActionProgressDialog_FWD_DEFINED__
#define __IActionProgressDialog_FWD_DEFINED__
typedef interface IActionProgressDialog IActionProgressDialog;
#endif 	/* __IActionProgressDialog_FWD_DEFINED__ */


#ifndef __IHWEventHandler_FWD_DEFINED__
#define __IHWEventHandler_FWD_DEFINED__
typedef interface IHWEventHandler IHWEventHandler;
#endif 	/* __IHWEventHandler_FWD_DEFINED__ */


#ifndef __IActionProgress_FWD_DEFINED__
#define __IActionProgress_FWD_DEFINED__
typedef interface IActionProgress IActionProgress;
#endif 	/* __IActionProgress_FWD_DEFINED__ */


#ifndef __IShellExtInit_FWD_DEFINED__
#define __IShellExtInit_FWD_DEFINED__
typedef interface IShellExtInit IShellExtInit;
#endif 	/* __IShellExtInit_FWD_DEFINED__ */


#ifndef __IShellPropSheetExt_FWD_DEFINED__
#define __IShellPropSheetExt_FWD_DEFINED__
typedef interface IShellPropSheetExt IShellPropSheetExt;
#endif 	/* __IShellPropSheetExt_FWD_DEFINED__ */


#ifndef __IRemoteComputer_FWD_DEFINED__
#define __IRemoteComputer_FWD_DEFINED__
typedef interface IRemoteComputer IRemoteComputer;
#endif 	/* __IRemoteComputer_FWD_DEFINED__ */


#ifndef __IDavStorage_FWD_DEFINED__
#define __IDavStorage_FWD_DEFINED__
typedef interface IDavStorage IDavStorage;
#endif 	/* __IDavStorage_FWD_DEFINED__ */


#ifndef __IDynamicStorage_FWD_DEFINED__
#define __IDynamicStorage_FWD_DEFINED__
typedef interface IDynamicStorage IDynamicStorage;
#endif 	/* __IDynamicStorage_FWD_DEFINED__ */


#ifndef __IQueryContinue_FWD_DEFINED__
#define __IQueryContinue_FWD_DEFINED__
typedef interface IQueryContinue IQueryContinue;
#endif 	/* __IQueryContinue_FWD_DEFINED__ */


#ifndef __IUserNotification_FWD_DEFINED__
#define __IUserNotification_FWD_DEFINED__
typedef interface IUserNotification IUserNotification;
#endif 	/* __IUserNotification_FWD_DEFINED__ */


#ifndef __IDockingWindow_FWD_DEFINED__
#define __IDockingWindow_FWD_DEFINED__
typedef interface IDockingWindow IDockingWindow;
#endif 	/* __IDockingWindow_FWD_DEFINED__ */


#ifndef __IDeskBand_FWD_DEFINED__
#define __IDeskBand_FWD_DEFINED__
typedef interface IDeskBand IDeskBand;
#endif 	/* __IDeskBand_FWD_DEFINED__ */


#ifndef __DriveSizeCategorizer_FWD_DEFINED__
#define __DriveSizeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class DriveSizeCategorizer DriveSizeCategorizer;
#else
typedef struct DriveSizeCategorizer DriveSizeCategorizer;
#endif /* __cplusplus */

#endif 	/* __DriveSizeCategorizer_FWD_DEFINED__ */


#ifndef __DriveTypeCategorizer_FWD_DEFINED__
#define __DriveTypeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class DriveTypeCategorizer DriveTypeCategorizer;
#else
typedef struct DriveTypeCategorizer DriveTypeCategorizer;
#endif /* __cplusplus */

#endif 	/* __DriveTypeCategorizer_FWD_DEFINED__ */


#ifndef __FreeSpaceCategorizer_FWD_DEFINED__
#define __FreeSpaceCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class FreeSpaceCategorizer FreeSpaceCategorizer;
#else
typedef struct FreeSpaceCategorizer FreeSpaceCategorizer;
#endif /* __cplusplus */

#endif 	/* __FreeSpaceCategorizer_FWD_DEFINED__ */


#ifndef __TimeCategorizer_FWD_DEFINED__
#define __TimeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimeCategorizer TimeCategorizer;
#else
typedef struct TimeCategorizer TimeCategorizer;
#endif /* __cplusplus */

#endif 	/* __TimeCategorizer_FWD_DEFINED__ */


#ifndef __SizeCategorizer_FWD_DEFINED__
#define __SizeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class SizeCategorizer SizeCategorizer;
#else
typedef struct SizeCategorizer SizeCategorizer;
#endif /* __cplusplus */

#endif 	/* __SizeCategorizer_FWD_DEFINED__ */


#ifndef __AlphabeticalCategorizer_FWD_DEFINED__
#define __AlphabeticalCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class AlphabeticalCategorizer AlphabeticalCategorizer;
#else
typedef struct AlphabeticalCategorizer AlphabeticalCategorizer;
#endif /* __cplusplus */

#endif 	/* __AlphabeticalCategorizer_FWD_DEFINED__ */


#ifndef __CompositeFolder_FWD_DEFINED__
#define __CompositeFolder_FWD_DEFINED__

#ifdef __cplusplus
typedef class CompositeFolder CompositeFolder;
#else
typedef struct CompositeFolder CompositeFolder;
#endif /* __cplusplus */

#endif 	/* __CompositeFolder_FWD_DEFINED__ */


#ifndef __LocalCopyHelper_FWD_DEFINED__
#define __LocalCopyHelper_FWD_DEFINED__

#ifdef __cplusplus
typedef class LocalCopyHelper LocalCopyHelper;
#else
typedef struct LocalCopyHelper LocalCopyHelper;
#endif /* __cplusplus */

#endif 	/* __LocalCopyHelper_FWD_DEFINED__ */


#ifndef __ShellItem_FWD_DEFINED__
#define __ShellItem_FWD_DEFINED__

#ifdef __cplusplus
typedef class ShellItem ShellItem;
#else
typedef struct ShellItem ShellItem;
#endif /* __cplusplus */

#endif 	/* __ShellItem_FWD_DEFINED__ */


#ifndef __ImageProperties_FWD_DEFINED__
#define __ImageProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageProperties ImageProperties;
#else
typedef struct ImageProperties ImageProperties;
#endif /* __cplusplus */

#endif 	/* __ImageProperties_FWD_DEFINED__ */


#ifndef __PropertiesUI_FWD_DEFINED__
#define __PropertiesUI_FWD_DEFINED__

#ifdef __cplusplus
typedef class PropertiesUI PropertiesUI;
#else
typedef struct PropertiesUI PropertiesUI;
#endif /* __cplusplus */

#endif 	/* __PropertiesUI_FWD_DEFINED__ */


#ifndef __UserNotification_FWD_DEFINED__
#define __UserNotification_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserNotification UserNotification;
#else
typedef struct UserNotification UserNotification;
#endif /* __cplusplus */

#endif 	/* __UserNotification_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "shtypes.h"
#include "servprov.h"
#include "comcat.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_shobjidl_0000 */
/* [local] */ 

//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 2000.
//
//--------------------------------------------------------------------------

#ifndef _WINRESRC_
#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#else
#if (_WIN32_IE < 0x0400) && defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
#error _WIN32_IE setting conflicts with _WIN32_WINNT setting
#endif
#endif
#endif
//===========================================================================
//
// IPersistFolder Interface
//
//  The IPersistFolder interface is used by the file system implementation of
// IShellFolder::BindToObject when it is initializing a shell folder object.
//
//
// [Member functions]
//
// IPersistFolder::Initialize
//
//  This member function is called when the explorer is initializing a
// shell folder object.
//
//  Parameters:
//   pidl -- Specifies the absolute location of the folder.
//
//===========================================================================


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_v0_0_s_ifspec;

#ifndef __IPersistFolder_INTERFACE_DEFINED__
#define __IPersistFolder_INTERFACE_DEFINED__

/* interface IPersistFolder */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214EA-0000-0000-C000-000000000046")
    IPersistFolder : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistFolder * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IPersistFolder * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        END_INTERFACE
    } IPersistFolderVtbl;

    interface IPersistFolder
    {
        CONST_VTBL struct IPersistFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistFolder_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistFolder_Initialize(This,pidl)	\
    (This)->lpVtbl -> Initialize(This,pidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistFolder_Initialize_Proxy( 
    IPersistFolder * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IPersistFolder_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistFolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0143 */
/* [local] */ 

typedef IPersistFolder *LPPERSISTFOLDER;

#if (_WIN32_IE >= 0x0400)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0143_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0143_v0_0_s_ifspec;

#ifndef __IPersistFolder2_INTERFACE_DEFINED__
#define __IPersistFolder2_INTERFACE_DEFINED__

/* interface IPersistFolder2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistFolder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1AC3D9F0-175C-11d1-95BE-00609797EA4F")
    IPersistFolder2 : public IPersistFolder
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurFolder( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistFolder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistFolder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistFolder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistFolder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistFolder2 * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IPersistFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurFolder )( 
            IPersistFolder2 * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        END_INTERFACE
    } IPersistFolder2Vtbl;

    interface IPersistFolder2
    {
        CONST_VTBL struct IPersistFolder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistFolder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistFolder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistFolder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistFolder2_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistFolder2_Initialize(This,pidl)	\
    (This)->lpVtbl -> Initialize(This,pidl)


#define IPersistFolder2_GetCurFolder(This,ppidl)	\
    (This)->lpVtbl -> GetCurFolder(This,ppidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistFolder2_GetCurFolder_Proxy( 
    IPersistFolder2 * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IPersistFolder2_GetCurFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistFolder2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0144 */
/* [local] */ 

typedef IPersistFolder2 *LPPERSISTFOLDER2;

#endif


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0144_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0144_v0_0_s_ifspec;

#ifndef __IPersistIDList_INTERFACE_DEFINED__
#define __IPersistIDList_INTERFACE_DEFINED__

/* interface IPersistIDList */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistIDList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1079acfc-29bd-11d3-8e0d-00c04f6837d5")
    IPersistIDList : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE LoadIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistIDListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistIDList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistIDList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistIDList * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *LoadIDList )( 
            IPersistIDList * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *SaveIDList )( 
            IPersistIDList * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        END_INTERFACE
    } IPersistIDListVtbl;

    interface IPersistIDList
    {
        CONST_VTBL struct IPersistIDListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistIDList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistIDList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistIDList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistIDList_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistIDList_LoadIDList(This,pidl)	\
    (This)->lpVtbl -> LoadIDList(This,pidl)

#define IPersistIDList_SaveIDList(This,ppidl)	\
    (This)->lpVtbl -> SaveIDList(This,ppidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistIDList_LoadIDList_Proxy( 
    IPersistIDList * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IPersistIDList_LoadIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistIDList_SaveIDList_Proxy( 
    IPersistIDList * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IPersistIDList_SaveIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistIDList_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0145 */
/* [local] */ 

//-------------------------------------------------------------------------
//
// IEnumIDList interface
//
//  IShellFolder::EnumObjects member returns an IEnumIDList object.
//
//-------------------------------------------------------------------------



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0145_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0145_v0_0_s_ifspec;

#ifndef __IEnumIDList_INTERFACE_DEFINED__
#define __IEnumIDList_INTERFACE_DEFINED__

/* interface IEnumIDList */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IEnumIDList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214F2-0000-0000-C000-000000000046")
    IEnumIDList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumIDList **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumIDListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumIDList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumIDList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumIDList * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumIDList * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumIDList * This,
            /* [out] */ IEnumIDList **ppenum);
        
        END_INTERFACE
    } IEnumIDListVtbl;

    interface IEnumIDList
    {
        CONST_VTBL struct IEnumIDListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumIDList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumIDList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumIDList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumIDList_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumIDList_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumIDList_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumIDList_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumIDList_Next_Proxy( 
    IEnumIDList * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumIDList_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Skip_Proxy( 
    IEnumIDList * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumIDList_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Reset_Proxy( 
    IEnumIDList * This);


void __RPC_STUB IEnumIDList_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Clone_Proxy( 
    IEnumIDList * This,
    /* [out] */ IEnumIDList **ppenum);


void __RPC_STUB IEnumIDList_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumIDList_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0146 */
/* [local] */ 

typedef IEnumIDList *LPENUMIDLIST;

//-------------------------------------------------------------------------
//
// IShellFolder interface
//
//
// [Member functions]
//
// IShellFolder::BindToObject(pidl, pbc, riid, ppv)
//   This function returns an instance of a sub-folder which is specified
//  by the IDList (pidl).
//
// IShellFolder::BindToStorage(pidl, pbc, riid, ppv)
//   This function returns a storage instance of a sub-folder which is
//  specified by the IDList (pidl). The shell never calls this member
//  function in the first release of Win95.
//
// IShellFolder::CompareIDs(lParam, pidl1, pidl2)
//   This function compares two IDLists and returns the result. The shell
//  explorer always passes 0 as lParam, which indicates 'sort by name'.
//  It should return 0 (as CODE of the scode), if two id indicates the
//  same object; negative value if pidl1 should be placed before pidl2;
//  positive value if pidl2 should be placed before pidl1.
//
// IShellFolder::CreateViewObject(hwndOwner, riid, ppv)
//   This function creates a view object of the folder itself. The view
//  object is a difference instance from the shell folder object.
//   'hwndOwner' can be used  as the owner window of its dialog box or
//  menu during the lifetime of the view object.
//  This member function should always create a new
//  instance which has only one reference count. The explorer may create
//  more than one instances of view object from one shell folder object
//  and treat them as separate instances.
//
// IShellFolder::GetAttributesOf(cidl, apidl, prgfInOut)
//   This function returns the attributes of specified objects in that
//  folder. 'cidl' and 'apidl' specifies objects. 'apidl' contains only
//  simple IDLists. The explorer initializes *prgfInOut with a set of
//  flags to be evaluated. The shell folder may optimize the operation
//  by not returning unspecified flags.
//
// IShellFolder::GetUIObjectOf(hwndOwner, cidl, apidl, riid, prgfInOut, ppv)
//   This function creates a UI object to be used for specified objects.
//  The shell explorer passes either IID_IDataObject (for transfer operation)
//  or IID_IContextMenu (for context menu operation) as riid.
//
// IShellFolder::GetDisplayNameOf
//   This function returns the display name of the specified object.
//  If the ID contains the display name (in the locale character set),
//  it returns the offset to the name. Otherwise, it returns a pointer
//  to the display name string (UNICODE), which is allocated by the
//  task allocator, or fills in a buffer.
//
// IShellFolder::SetNameOf
//   This function sets the display name of the specified object.
//  If it changes the ID as well, it returns the new ID which is
//  alocated by the task allocator.
//
//-------------------------------------------------------------------------
// IShellFolder::GetDisplayNameOf/SetNameOf uFlags
typedef enum tagSHGDN
{
    SHGDN_NORMAL             = 0x0000,  // default (display purpose)
    SHGDN_INFOLDER           = 0x0001,  // displayed under a folder (relative)
    SHGDN_FOREDITING         = 0x1000,  // for in-place editing
    SHGDN_FORADDRESSBAR      = 0x4000,  // UI friendly parsing name (remove ugly stuff)
    SHGDN_FORPARSING         = 0x8000,  // parsing name for ParseDisplayName()
} SHGNO;
typedef DWORD SHGDNF;

// IShellFolder::EnumObjects grfFlags bits
typedef enum tagSHCONTF
{
    SHCONTF_FOLDERS             = 0x0020,   // only want folders enumerated (SHGAO_FOLDER)
    SHCONTF_NONFOLDERS          = 0x0040,   // include non folders
    SHCONTF_INCLUDEHIDDEN       = 0x0080,   // show items normally hidden
    SHCONTF_INIT_ON_FIRST_NEXT  = 0x0100,   // allow EnumObject() to return before validating enum
    SHCONTF_NETPRINTERSRCH      = 0x0200,   // hint that client is looking for printers
    SHCONTF_SHAREABLE           = 0x0400,   // hint that client is looking sharable resources (remote shares)
};
typedef DWORD SHCONTF;

// IShellFolder::CompareIDs lParam flags
//
//  SHCIDS_ALLFIELDS is a mask for lParam indicating that the shell folder
//  should first compare on the lParam column, and if that proves equal,
//  then perform a full comparison on all fields.  This flag is supported
//  if the IShellFolder supports IShellFolder2.
//
//
#define SHCIDS_ALLFIELDS        0x80000000L
#define SHCIDS_COLUMNMASK       0x0000FFFFL
// IShellFolder::GetAttributesOf flags
// DESCRIPTION:
// SFGAO_CANLINK: If this bit is set on an item in the shell folder, a
//            'Create Shortcut' menu item will be added to the File
//            menu and context menus for the item.  If the user selects
//            that command, your IContextMenu::InvokeCommand() will be called
//            with 'link'.
//                 That flag will also be used to determine if 'Create Shortcut'
//            should be added when the item in your folder is dragged to another
//            folder.
#define SFGAO_CANCOPY           DROPEFFECT_COPY // Objects can be copied    (0x1)
#define SFGAO_CANMOVE           DROPEFFECT_MOVE // Objects can be moved     (0x2)
#define SFGAO_CANLINK           DROPEFFECT_LINK // Objects can be linked    (0x4)
#define SFGAO_CANRENAME         0x00000010L     // Objects can be renamed
#define SFGAO_CANDELETE         0x00000020L     // Objects can be deleted
#define SFGAO_HASPROPSHEET      0x00000040L     // Objects have property sheets
#define SFGAO_DROPTARGET        0x00000100L     // Objects are drop target
#define SFGAO_CAPABILITYMASK    0x00000177L
#define SFGAO_ISSLOW            0x00004000L     // 'slow' object
#define SFGAO_GHOSTED           0x00008000L     // ghosted icon
#define SFGAO_LINK              0x00010000L     // Shortcut (link)
#define SFGAO_SHARE             0x00020000L     // shared
#define SFGAO_READONLY          0x00040000L     // read-only
#define SFGAO_HIDDEN            0x00080000L     // hidden object
#define SFGAO_DISPLAYATTRMASK   0x000FC000L
#define SFGAO_FILESYSANCESTOR   0x10000000L     // It contains file system folder
#define SFGAO_STORAGEANCESTOR   0x10000000L     // contains children with accessible storage (supersets FILESYSANCESTOR)
#define SFGAO_FOLDER            0x20000000L     // It's a folder.
#define SFGAO_FILESYSTEM        0x40000000L     // is a file system thing (file/folder/root)
#define SFGAO_HASSUBFOLDER      0x80000000L     // Expandable in the map pane
#define SFGAO_CONTENTSMASK      0x80000000L
#define SFGAO_VALIDATE          0x01000000L     // invalidate cached information
#define SFGAO_REMOVABLE         0x02000000L     // is this removeable media?
#define SFGAO_COMPRESSED        0x04000000L     // Object is compressed (use alt color)
#define SFGAO_BROWSABLE         0x08000000L     // is in-place browsable
#define SFGAO_NONENUMERATED     0x00100000L     // is a non-enumerated object
#define SFGAO_NEWCONTENT        0x00200000L     // should show bold in explorer tree
#define SFGAO_CANMONIKER        0x00400000L     // can create monikers for its objects
#define SFGAO_HASSTORAGE        0x00400000L     // item has accessible storage (replaces CANMONIKER)
typedef ULONG SFGAOF;

// IShellFolder IBindCtx* parameters. the IUnknown for these are
// accessed through IBindCtx::RegisterObjectParam/GetObjectParam

// this object will support IPersist to query a CLSID that should be skipped
// in the binding process. this is to avoid loops or to allow delegation to
// base name space functionality. see SHSkipJunction()

#define STR_SKIP_BINDING_CLSID      L"Skip Binding CLSID"



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0146_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0146_v0_0_s_ifspec;

#ifndef __IShellFolder_INTERFACE_DEFINED__
#define __IShellFolder_INTERFACE_DEFINED__

/* interface IShellFolder */
/* [unique][local][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E6-0000-0000-C000-000000000046")
    IShellFolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [out][in] */ ULONG *pdwAttributes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumObjects( 
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToObject( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToStorage( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CompareIDs( 
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewObject( 
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributesOf( 
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUIObjectOf( 
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayNameOf( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPSTRRET lpName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNameOf( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [out][in] */ ULONG *pdwAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList);
        
        HRESULT ( STDMETHODCALLTYPE *BindToObject )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *BindToStorage )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CompareIDs )( 
            IShellFolder * This,
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewObject )( 
            IShellFolder * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributesOf )( 
            IShellFolder * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetUIObjectOf )( 
            IShellFolder * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayNameOf )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPSTRRET lpName);
        
        HRESULT ( STDMETHODCALLTYPE *SetNameOf )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut);
        
        END_INTERFACE
    } IShellFolderVtbl;

    interface IShellFolder
    {
        CONST_VTBL struct IShellFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolder_ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)	\
    (This)->lpVtbl -> ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)

#define IShellFolder_EnumObjects(This,hwnd,grfFlags,ppenumIDList)	\
    (This)->lpVtbl -> EnumObjects(This,hwnd,grfFlags,ppenumIDList)

#define IShellFolder_BindToObject(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToObject(This,pidl,pbc,riid,ppv)

#define IShellFolder_BindToStorage(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToStorage(This,pidl,pbc,riid,ppv)

#define IShellFolder_CompareIDs(This,lParam,pidl1,pidl2)	\
    (This)->lpVtbl -> CompareIDs(This,lParam,pidl1,pidl2)

#define IShellFolder_CreateViewObject(This,hwndOwner,riid,ppv)	\
    (This)->lpVtbl -> CreateViewObject(This,hwndOwner,riid,ppv)

#define IShellFolder_GetAttributesOf(This,cidl,apidl,rgfInOut)	\
    (This)->lpVtbl -> GetAttributesOf(This,cidl,apidl,rgfInOut)

#define IShellFolder_GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)	\
    (This)->lpVtbl -> GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)

#define IShellFolder_GetDisplayNameOf(This,pidl,uFlags,lpName)	\
    (This)->lpVtbl -> GetDisplayNameOf(This,pidl,uFlags,lpName)

#define IShellFolder_SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)	\
    (This)->lpVtbl -> SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellFolder_ParseDisplayName_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ LPBC pbc,
    /* [string][in] */ LPOLESTR pszDisplayName,
    /* [out] */ ULONG *pchEaten,
    /* [out] */ LPITEMIDLIST *ppidl,
    /* [out][in] */ ULONG *pdwAttributes);


void __RPC_STUB IShellFolder_ParseDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_EnumObjects_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ SHCONTF grfFlags,
    /* [out] */ IEnumIDList **ppenumIDList);


void __RPC_STUB IShellFolder_EnumObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_BindToObject_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ LPBC pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_BindToObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_BindToStorage_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ LPBC pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_BindToStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_CompareIDs_Proxy( 
    IShellFolder * This,
    /* [in] */ LPARAM lParam,
    /* [in] */ LPCITEMIDLIST pidl1,
    /* [in] */ LPCITEMIDLIST pidl2);


void __RPC_STUB IShellFolder_CompareIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_CreateViewObject_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwndOwner,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_CreateViewObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetAttributesOf_Proxy( 
    IShellFolder * This,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [out][in] */ SFGAOF *rgfInOut);


void __RPC_STUB IShellFolder_GetAttributesOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetUIObjectOf_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwndOwner,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [in] */ REFIID riid,
    UINT *rgfReserved,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_GetUIObjectOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetDisplayNameOf_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ SHGDNF uFlags,
    /* [out] */ LPSTRRET lpName);


void __RPC_STUB IShellFolder_GetDisplayNameOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_SetNameOf_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [string][in] */ LPCOLESTR pszName,
    /* [in] */ SHGDNF uFlags,
    /* [out] */ LPITEMIDLIST *ppidlOut);


void __RPC_STUB IShellFolder_SetNameOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0147 */
/* [local] */ 

typedef IShellFolder *LPSHELLFOLDER;

#if (_WIN32_IE >= 0x0500)
//-------------------------------------------------------------------------
//
// IEnumExtraSearch interface
//
//  IShellFolder2::EnumSearches member returns an IEnumExtraSearch object.
//
//-------------------------------------------------------------------------
typedef struct tagEXTRASEARCH
    {
    GUID guidSearch;
    WCHAR wszFriendlyName[ 80 ];
    WCHAR wszUrl[ 2084 ];
    } 	EXTRASEARCH;

typedef struct tagEXTRASEARCH *LPEXTRASEARCH;

typedef struct IEnumExtraSearch *LPENUMEXTRASEARCH;




extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0147_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0147_v0_0_s_ifspec;

#ifndef __IEnumExtraSearch_INTERFACE_DEFINED__
#define __IEnumExtraSearch_INTERFACE_DEFINED__

/* interface IEnumExtraSearch */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IEnumExtraSearch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0E700BE1-9DB6-11d1-A1CE-00C04FD75D13")
    IEnumExtraSearch : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumExtraSearch **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumExtraSearchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumExtraSearch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumExtraSearch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumExtraSearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumExtraSearch * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumExtraSearch * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumExtraSearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumExtraSearch * This,
            /* [out] */ IEnumExtraSearch **ppenum);
        
        END_INTERFACE
    } IEnumExtraSearchVtbl;

    interface IEnumExtraSearch
    {
        CONST_VTBL struct IEnumExtraSearchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumExtraSearch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumExtraSearch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumExtraSearch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumExtraSearch_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumExtraSearch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumExtraSearch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumExtraSearch_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Next_Proxy( 
    IEnumExtraSearch * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumExtraSearch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Skip_Proxy( 
    IEnumExtraSearch * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumExtraSearch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Reset_Proxy( 
    IEnumExtraSearch * This);


void __RPC_STUB IEnumExtraSearch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Clone_Proxy( 
    IEnumExtraSearch * This,
    /* [out] */ IEnumExtraSearch **ppenum);


void __RPC_STUB IEnumExtraSearch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumExtraSearch_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0148 */
/* [local] */ 

//--------------------------------------------------------------------------
// IShellFolder2
//
// [member functions]
//
// IShellFolder2::GetDefaultSearchGUID(LPGUID pGuid)
//   Returns the guid of the search that is to be invoked when user clicks 
//   on the search toolbar button
//
// IShellFolder2::EnumSearches(IEnumExtraSearch **ppenum)
//   gives an enumerator of the searches to be added to the search menu
//--------------------------------------------------------------------------
//
// IShellFolder2::GetDefaultColumnState values
typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0148_0001
    {	SHCOLSTATE_TYPE_STR	= 0x1,
	SHCOLSTATE_TYPE_INT	= 0x2,
	SHCOLSTATE_TYPE_DATE	= 0x3,
	SHCOLSTATE_TYPEMASK	= 0xf,
	SHCOLSTATE_ONBYDEFAULT	= 0x10,
	SHCOLSTATE_SLOW	= 0x20,
	SHCOLSTATE_EXTENDED	= 0x40,
	SHCOLSTATE_SECONDARYUI	= 0x80,
	SHCOLSTATE_HIDDEN	= 0x100
    } 	SHCOLSTATE;

typedef DWORD SHCOLSTATEF;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_shobjidl_0148_0002
    {
    GUID fmtid;
    DWORD pid;
    } 	SHCOLUMNID;

typedef struct __MIDL___MIDL_itf_shobjidl_0148_0002 *LPSHCOLUMNID;

typedef const SHCOLUMNID *LPCSHCOLUMNID;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0148_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0148_v0_0_s_ifspec;

#ifndef __IShellFolder2_INTERFACE_DEFINED__
#define __IShellFolder2_INTERFACE_DEFINED__

/* interface IShellFolder2 */
/* [unique][local][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellFolder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93F2F68C-1D1B-11d3-A30E-00C04F79ABD1")
    IShellFolder2 : public IShellFolder
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDefaultSearchGUID( 
            /* [out] */ GUID *pguid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumSearches( 
            /* [out] */ IEnumExtraSearch **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultColumn( 
            DWORD dwRes,
            /* [out] */ ULONG *pSort,
            /* [out] */ ULONG *pDisplay) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultColumnState( 
            /* [in] */ UINT iColumn,
            /* [out] */ SHCOLSTATEF *pcsFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDetailsEx( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ const SHCOLUMNID *pscid,
            /* [out] */ VARIANT *pv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDetailsOf( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT iColumn,
            /* [out] */ SHELLDETAILS *psd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapColumnToSCID( 
            /* [in] */ UINT iColumn,
            /* [in] */ SHCOLUMNID *pscid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [out][in] */ ULONG *pdwAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList);
        
        HRESULT ( STDMETHODCALLTYPE *BindToObject )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *BindToStorage )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CompareIDs )( 
            IShellFolder2 * This,
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewObject )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributesOf )( 
            IShellFolder2 * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetUIObjectOf )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayNameOf )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPSTRRET lpName);
        
        HRESULT ( STDMETHODCALLTYPE *SetNameOf )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultSearchGUID )( 
            IShellFolder2 * This,
            /* [out] */ GUID *pguid);
        
        HRESULT ( STDMETHODCALLTYPE *EnumSearches )( 
            IShellFolder2 * This,
            /* [out] */ IEnumExtraSearch **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultColumn )( 
            IShellFolder2 * This,
            DWORD dwRes,
            /* [out] */ ULONG *pSort,
            /* [out] */ ULONG *pDisplay);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultColumnState )( 
            IShellFolder2 * This,
            /* [in] */ UINT iColumn,
            /* [out] */ SHCOLSTATEF *pcsFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetDetailsEx )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ const SHCOLUMNID *pscid,
            /* [out] */ VARIANT *pv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDetailsOf )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT iColumn,
            /* [out] */ SHELLDETAILS *psd);
        
        HRESULT ( STDMETHODCALLTYPE *MapColumnToSCID )( 
            IShellFolder2 * This,
            /* [in] */ UINT iColumn,
            /* [in] */ SHCOLUMNID *pscid);
        
        END_INTERFACE
    } IShellFolder2Vtbl;

    interface IShellFolder2
    {
        CONST_VTBL struct IShellFolder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolder2_ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)	\
    (This)->lpVtbl -> ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)

#define IShellFolder2_EnumObjects(This,hwnd,grfFlags,ppenumIDList)	\
    (This)->lpVtbl -> EnumObjects(This,hwnd,grfFlags,ppenumIDList)

#define IShellFolder2_BindToObject(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToObject(This,pidl,pbc,riid,ppv)

#define IShellFolder2_BindToStorage(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToStorage(This,pidl,pbc,riid,ppv)

#define IShellFolder2_CompareIDs(This,lParam,pidl1,pidl2)	\
    (This)->lpVtbl -> CompareIDs(This,lParam,pidl1,pidl2)

#define IShellFolder2_CreateViewObject(This,hwndOwner,riid,ppv)	\
    (This)->lpVtbl -> CreateViewObject(This,hwndOwner,riid,ppv)

#define IShellFolder2_GetAttributesOf(This,cidl,apidl,rgfInOut)	\
    (This)->lpVtbl -> GetAttributesOf(This,cidl,apidl,rgfInOut)

#define IShellFolder2_GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)	\
    (This)->lpVtbl -> GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)

#define IShellFolder2_GetDisplayNameOf(This,pidl,uFlags,lpName)	\
    (This)->lpVtbl -> GetDisplayNameOf(This,pidl,uFlags,lpName)

#define IShellFolder2_SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)	\
    (This)->lpVtbl -> SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)


#define IShellFolder2_GetDefaultSearchGUID(This,pguid)	\
    (This)->lpVtbl -> GetDefaultSearchGUID(This,pguid)

#define IShellFolder2_EnumSearches(This,ppenum)	\
    (This)->lpVtbl -> EnumSearches(This,ppenum)

#define IShellFolder2_GetDefaultColumn(This,dwRes,pSort,pDisplay)	\
    (This)->lpVtbl -> GetDefaultColumn(This,dwRes,pSort,pDisplay)

#define IShellFolder2_GetDefaultColumnState(This,iColumn,pcsFlags)	\
    (This)->lpVtbl -> GetDefaultColumnState(This,iColumn,pcsFlags)

#define IShellFolder2_GetDetailsEx(This,pidl,pscid,pv)	\
    (This)->lpVtbl -> GetDetailsEx(This,pidl,pscid,pv)

#define IShellFolder2_GetDetailsOf(This,pidl,iColumn,psd)	\
    (This)->lpVtbl -> GetDetailsOf(This,pidl,iColumn,psd)

#define IShellFolder2_MapColumnToSCID(This,iColumn,pscid)	\
    (This)->lpVtbl -> MapColumnToSCID(This,iColumn,pscid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultSearchGUID_Proxy( 
    IShellFolder2 * This,
    /* [out] */ GUID *pguid);


void __RPC_STUB IShellFolder2_GetDefaultSearchGUID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_EnumSearches_Proxy( 
    IShellFolder2 * This,
    /* [out] */ IEnumExtraSearch **ppenum);


void __RPC_STUB IShellFolder2_EnumSearches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultColumn_Proxy( 
    IShellFolder2 * This,
    DWORD dwRes,
    /* [out] */ ULONG *pSort,
    /* [out] */ ULONG *pDisplay);


void __RPC_STUB IShellFolder2_GetDefaultColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultColumnState_Proxy( 
    IShellFolder2 * This,
    /* [in] */ UINT iColumn,
    /* [out] */ SHCOLSTATEF *pcsFlags);


void __RPC_STUB IShellFolder2_GetDefaultColumnState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDetailsEx_Proxy( 
    IShellFolder2 * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ const SHCOLUMNID *pscid,
    /* [out] */ VARIANT *pv);


void __RPC_STUB IShellFolder2_GetDetailsEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDetailsOf_Proxy( 
    IShellFolder2 * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ UINT iColumn,
    /* [out] */ SHELLDETAILS *psd);


void __RPC_STUB IShellFolder2_GetDetailsOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_MapColumnToSCID_Proxy( 
    IShellFolder2 * This,
    /* [in] */ UINT iColumn,
    /* [in] */ SHCOLUMNID *pscid);


void __RPC_STUB IShellFolder2_MapColumnToSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolder2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0149 */
/* [local] */ 

#endif // _WIN32_IE >= 0x0500)
//--------------------------------------------------------------------------
//
// FOLDERSETTINGS
//
//  FOLDERSETTINGS is a data structure that explorer passes from one folder
// view to another, when the user is browsing. It calls ISV::GetCurrentInfo
// member to get the current settings and pass it to ISV::CreateViewWindow
// to allow the next folder view 'inherit' it. These settings assumes a
// particular UI (which the shell's folder view has), and shell extensions
// may or may not use those settings.
//
//--------------------------------------------------------------------------
typedef char *LPVIEWSETTINGS;

// NB Bitfields.
// FWF_DESKTOP implies FWF_TRANSPARENT/NOCLIENTEDGE/NOSCROLL
typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0149_0001
    {	FWF_AUTOARRANGE	= 0x1,
	FWF_ABBREVIATEDNAMES	= 0x2,
	FWF_SNAPTOGRID	= 0x4,
	FWF_OWNERDATA	= 0x8,
	FWF_BESTFITWINDOW	= 0x10,
	FWF_DESKTOP	= 0x20,
	FWF_SINGLESEL	= 0x40,
	FWF_NOSUBFOLDERS	= 0x80,
	FWF_TRANSPARENT	= 0x100,
	FWF_NOCLIENTEDGE	= 0x200,
	FWF_NOSCROLL	= 0x400,
	FWF_ALIGNLEFT	= 0x800,
	FWF_NOICONS	= 0x1000,
	FWF_SHOWSELALWAYS	= 0x2000,
	FWF_NOVISIBLE	= 0x4000,
	FWF_SINGLECLICKACTIVATE	= 0x8000,
	FWF_NOWEBVIEW	= 0x10000,
	FWF_HIDEFILENAMES	= 0x20000
    } 	FOLDERFLAGS;

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0149_0002
    {	FVM_FIRST	= 1,
	FVM_ICON	= 1,
	FVM_SMALLICON	= 2,
	FVM_LIST	= 3,
	FVM_DETAILS	= 4,
	FVM_THUMBNAIL	= 5,
	FVM_TILE	= 6,
	FVM_THUMBSTRIP	= 7,
	FVM_LAST	= 7
    } 	FOLDERVIEWMODE;

typedef /* [public][public][public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_shobjidl_0149_0003
    {
    UINT ViewMode;
    UINT fFlags;
    } 	FOLDERSETTINGS;

typedef FOLDERSETTINGS *LPFOLDERSETTINGS;

typedef const FOLDERSETTINGS *LPCFOLDERSETTINGS;

typedef FOLDERSETTINGS *PFOLDERSETTINGS;

#define SVSI_DESELECT   0x0000
#define SVSI_SELECT     0x0001
#define SVSI_EDIT       0x0003  // includes select
#define SVSI_DESELECTOTHERS 0x0004
#define SVSI_ENSUREVISIBLE  0x0008
#define SVSI_FOCUSED        0x0010
#define SVSI_TRANSLATEPT    0x0020
#define SVSI_SELECTIONMARK  0x0040
#define SVGIO_BACKGROUND    0x00000000
#define SVGIO_SELECTION     0x00000001
#define SVGIO_ALLVIEW       0x00000002
typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0149_0004
    {	SVUIA_DEACTIVATE	= 0,
	SVUIA_ACTIVATE_NOFOCUS	= 1,
	SVUIA_ACTIVATE_FOCUS	= 2,
	SVUIA_INPLACEACTIVATE	= 3
    } 	SVUIA_STATUS;

#ifdef _FIX_ENABLEMODELESS_CONFLICT
#define    EnableModeless EnableModelessSV
#endif
#ifdef _NEVER_
typedef LPARAM LPFNSVADDPROPSHEETPAGE;

#else //!_NEVER_
#include <prsht.h>
typedef LPFNADDPROPSHEETPAGE LPFNSVADDPROPSHEETPAGE;
#endif //_NEVER_



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0149_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0149_v0_0_s_ifspec;

#ifndef __IShellView_INTERFACE_DEFINED__
#define __IShellView_INTERFACE_DEFINED__

/* interface IShellView */
/* [unique][object][uuid][helpstring] */ 

typedef IShellView *LPSHELLVIEW;


EXTERN_C const IID IID_IShellView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E3-0000-0000-C000-000000000046")
    IShellView : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ LPMSG lpmsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UIActivate( 
            /* [in] */ UINT uState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewWindow( 
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS lpfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyViewWindow( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentInfo( 
            /* [out] */ LPFOLDERSETTINGS lpfs) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE AddPropertySheetPages( 
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveViewState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectItem( 
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetItemObject( 
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellView * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellView * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellView * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IShellView * This,
            /* [in] */ LPMSG lpmsg);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IShellView * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *UIActivate )( 
            IShellView * This,
            /* [in] */ UINT uState);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow )( 
            IShellView * This,
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS lpfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyViewWindow )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentInfo )( 
            IShellView * This,
            /* [out] */ LPFOLDERSETTINGS lpfs);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *AddPropertySheetPages )( 
            IShellView * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam);
        
        HRESULT ( STDMETHODCALLTYPE *SaveViewState )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellView * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemObject )( 
            IShellView * This,
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        END_INTERFACE
    } IShellViewVtbl;

    interface IShellView
    {
        CONST_VTBL struct IShellViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellView_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellView_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellView_TranslateAccelerator(This,lpmsg)	\
    (This)->lpVtbl -> TranslateAccelerator(This,lpmsg)

#define IShellView_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define IShellView_UIActivate(This,uState)	\
    (This)->lpVtbl -> UIActivate(This,uState)

#define IShellView_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IShellView_CreateViewWindow(This,psvPrevious,lpfs,psb,prcView,phWnd)	\
    (This)->lpVtbl -> CreateViewWindow(This,psvPrevious,lpfs,psb,prcView,phWnd)

#define IShellView_DestroyViewWindow(This)	\
    (This)->lpVtbl -> DestroyViewWindow(This)

#define IShellView_GetCurrentInfo(This,lpfs)	\
    (This)->lpVtbl -> GetCurrentInfo(This,lpfs)

#define IShellView_AddPropertySheetPages(This,dwReserved,pfn,lparam)	\
    (This)->lpVtbl -> AddPropertySheetPages(This,dwReserved,pfn,lparam)

#define IShellView_SaveViewState(This)	\
    (This)->lpVtbl -> SaveViewState(This)

#define IShellView_SelectItem(This,pidlItem,uFlags)	\
    (This)->lpVtbl -> SelectItem(This,pidlItem,uFlags)

#define IShellView_GetItemObject(This,uItem,riid,ppv)	\
    (This)->lpVtbl -> GetItemObject(This,uItem,riid,ppv)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellView_TranslateAccelerator_Proxy( 
    IShellView * This,
    /* [in] */ LPMSG lpmsg);


void __RPC_STUB IShellView_TranslateAccelerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_EnableModeless_Proxy( 
    IShellView * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IShellView_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_UIActivate_Proxy( 
    IShellView * This,
    /* [in] */ UINT uState);


void __RPC_STUB IShellView_UIActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_Refresh_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_CreateViewWindow_Proxy( 
    IShellView * This,
    /* [in] */ IShellView *psvPrevious,
    /* [in] */ LPCFOLDERSETTINGS lpfs,
    /* [in] */ IShellBrowser *psb,
    /* [out] */ RECT *prcView,
    /* [out] */ HWND *phWnd);


void __RPC_STUB IShellView_CreateViewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_DestroyViewWindow_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_DestroyViewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_GetCurrentInfo_Proxy( 
    IShellView * This,
    /* [out] */ LPFOLDERSETTINGS lpfs);


void __RPC_STUB IShellView_GetCurrentInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellView_AddPropertySheetPages_Proxy( 
    IShellView * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
    /* [in] */ LPARAM lparam);


void __RPC_STUB IShellView_AddPropertySheetPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_SaveViewState_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_SaveViewState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_SelectItem_Proxy( 
    IShellView * This,
    /* [in] */ LPCITEMIDLIST pidlItem,
    /* [in] */ UINT uFlags);


void __RPC_STUB IShellView_SelectItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_GetItemObject_Proxy( 
    IShellView * This,
    /* [in] */ UINT uItem,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellView_GetItemObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellView_INTERFACE_DEFINED__ */


#ifndef __IShellView2_INTERFACE_DEFINED__
#define __IShellView2_INTERFACE_DEFINED__

/* interface IShellView2 */
/* [unique][object][uuid][helpstring] */ 

typedef GUID SHELLVIEWID;

#define SV2GV_CURRENTVIEW ((UINT)-1)
#define SV2GV_DEFAULTVIEW ((UINT)-2)
#include <pshpack8.h>
typedef struct _SV2CVW2_PARAMS
    {
    DWORD cbSize;
    IShellView *psvPrev;
    LPCFOLDERSETTINGS pfs;
    IShellBrowser *psbOwner;
    RECT *prcView;
    const SHELLVIEWID *pvid;
    HWND hwndView;
    } 	SV2CVW2_PARAMS;

typedef struct _SV2CVW2_PARAMS *LPSV2CVW2_PARAMS;

#include <poppack.h>

EXTERN_C const IID IID_IShellView2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88E39E80-3578-11CF-AE69-08002B2E1262")
    IShellView2 : public IShellView
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetView( 
            /* [out][in] */ SHELLVIEWID *pvid,
            /* [in] */ ULONG uView) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewWindow2( 
            /* [in] */ LPSV2CVW2_PARAMS lpParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleRename( 
            /* [in] */ LPCITEMIDLIST pidlNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectAndPositionItem( 
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags,
            /* [in] */ POINT *ppt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellView2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellView2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellView2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellView2 * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellView2 * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellView2 * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IShellView2 * This,
            /* [in] */ LPMSG lpmsg);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IShellView2 * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *UIActivate )( 
            IShellView2 * This,
            /* [in] */ UINT uState);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow )( 
            IShellView2 * This,
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS lpfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyViewWindow )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentInfo )( 
            IShellView2 * This,
            /* [out] */ LPFOLDERSETTINGS lpfs);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *AddPropertySheetPages )( 
            IShellView2 * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam);
        
        HRESULT ( STDMETHODCALLTYPE *SaveViewState )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemObject )( 
            IShellView2 * This,
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetView )( 
            IShellView2 * This,
            /* [out][in] */ SHELLVIEWID *pvid,
            /* [in] */ ULONG uView);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow2 )( 
            IShellView2 * This,
            /* [in] */ LPSV2CVW2_PARAMS lpParams);
        
        HRESULT ( STDMETHODCALLTYPE *HandleRename )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlNew);
        
        HRESULT ( STDMETHODCALLTYPE *SelectAndPositionItem )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags,
            /* [in] */ POINT *ppt);
        
        END_INTERFACE
    } IShellView2Vtbl;

    interface IShellView2
    {
        CONST_VTBL struct IShellView2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellView2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellView2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellView2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellView2_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellView2_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellView2_TranslateAccelerator(This,lpmsg)	\
    (This)->lpVtbl -> TranslateAccelerator(This,lpmsg)

#define IShellView2_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define IShellView2_UIActivate(This,uState)	\
    (This)->lpVtbl -> UIActivate(This,uState)

#define IShellView2_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IShellView2_CreateViewWindow(This,psvPrevious,lpfs,psb,prcView,phWnd)	\
    (This)->lpVtbl -> CreateViewWindow(This,psvPrevious,lpfs,psb,prcView,phWnd)

#define IShellView2_DestroyViewWindow(This)	\
    (This)->lpVtbl -> DestroyViewWindow(This)

#define IShellView2_GetCurrentInfo(This,lpfs)	\
    (This)->lpVtbl -> GetCurrentInfo(This,lpfs)

#define IShellView2_AddPropertySheetPages(This,dwReserved,pfn,lparam)	\
    (This)->lpVtbl -> AddPropertySheetPages(This,dwReserved,pfn,lparam)

#define IShellView2_SaveViewState(This)	\
    (This)->lpVtbl -> SaveViewState(This)

#define IShellView2_SelectItem(This,pidlItem,uFlags)	\
    (This)->lpVtbl -> SelectItem(This,pidlItem,uFlags)

#define IShellView2_GetItemObject(This,uItem,riid,ppv)	\
    (This)->lpVtbl -> GetItemObject(This,uItem,riid,ppv)


#define IShellView2_GetView(This,pvid,uView)	\
    (This)->lpVtbl -> GetView(This,pvid,uView)

#define IShellView2_CreateViewWindow2(This,lpParams)	\
    (This)->lpVtbl -> CreateViewWindow2(This,lpParams)

#define IShellView2_HandleRename(This,pidlNew)	\
    (This)->lpVtbl -> HandleRename(This,pidlNew)

#define IShellView2_SelectAndPositionItem(This,pidlItem,uFlags,ppt)	\
    (This)->lpVtbl -> SelectAndPositionItem(This,pidlItem,uFlags,ppt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellView2_GetView_Proxy( 
    IShellView2 * This,
    /* [out][in] */ SHELLVIEWID *pvid,
    /* [in] */ ULONG uView);


void __RPC_STUB IShellView2_GetView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_CreateViewWindow2_Proxy( 
    IShellView2 * This,
    /* [in] */ LPSV2CVW2_PARAMS lpParams);


void __RPC_STUB IShellView2_CreateViewWindow2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_HandleRename_Proxy( 
    IShellView2 * This,
    /* [in] */ LPCITEMIDLIST pidlNew);


void __RPC_STUB IShellView2_HandleRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_SelectAndPositionItem_Proxy( 
    IShellView2 * This,
    /* [in] */ LPCITEMIDLIST pidlItem,
    /* [in] */ UINT uFlags,
    /* [in] */ POINT *ppt);


void __RPC_STUB IShellView2_SelectAndPositionItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellView2_INTERFACE_DEFINED__ */


#ifndef __IFolderView_INTERFACE_DEFINED__
#define __IFolderView_INTERFACE_DEFINED__

/* interface IFolderView */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IFolderView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("79ed5811-705c-4d33-af5f-b215f4bb645f")
    IFolderView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurrentViewMode( 
            /* [out][in] */ UINT *pViewMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentViewMode( 
            /* [in] */ UINT ViewMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFolder( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ItemCount( 
            /* [in] */ int *pcItems) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ int iItemIndex,
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectedItem( 
            /* [out] */ int *piItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectItem( 
            /* [in] */ int iItem,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentViewMode )( 
            IFolderView * This,
            /* [out][in] */ UINT *pViewMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentViewMode )( 
            IFolderView * This,
            /* [in] */ UINT ViewMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetFolder )( 
            IFolderView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *ItemCount )( 
            IFolderView * This,
            /* [in] */ int *pcItems);
        
        HRESULT ( STDMETHODCALLTYPE *Item )( 
            IFolderView * This,
            /* [in] */ int iItemIndex,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *SelectedItem )( 
            IFolderView * This,
            /* [out] */ int *piItem);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IFolderView * This,
            /* [in] */ int iItem,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IFolderViewVtbl;

    interface IFolderView
    {
        CONST_VTBL struct IFolderViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderView_GetCurrentViewMode(This,pViewMode)	\
    (This)->lpVtbl -> GetCurrentViewMode(This,pViewMode)

#define IFolderView_SetCurrentViewMode(This,ViewMode)	\
    (This)->lpVtbl -> SetCurrentViewMode(This,ViewMode)

#define IFolderView_GetFolder(This,riid,ppv)	\
    (This)->lpVtbl -> GetFolder(This,riid,ppv)

#define IFolderView_ItemCount(This,pcItems)	\
    (This)->lpVtbl -> ItemCount(This,pcItems)

#define IFolderView_Item(This,iItemIndex,ppidl)	\
    (This)->lpVtbl -> Item(This,iItemIndex,ppidl)

#define IFolderView_SelectedItem(This,piItem)	\
    (This)->lpVtbl -> SelectedItem(This,piItem)

#define IFolderView_SelectItem(This,iItem,dwFlags)	\
    (This)->lpVtbl -> SelectItem(This,iItem,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderView_GetCurrentViewMode_Proxy( 
    IFolderView * This,
    /* [out][in] */ UINT *pViewMode);


void __RPC_STUB IFolderView_GetCurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SetCurrentViewMode_Proxy( 
    IFolderView * This,
    /* [in] */ UINT ViewMode);


void __RPC_STUB IFolderView_SetCurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetFolder_Proxy( 
    IFolderView * This,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IFolderView_GetFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_ItemCount_Proxy( 
    IFolderView * This,
    /* [in] */ int *pcItems);


void __RPC_STUB IFolderView_ItemCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_Item_Proxy( 
    IFolderView * This,
    /* [in] */ int iItemIndex,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IFolderView_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SelectedItem_Proxy( 
    IFolderView * This,
    /* [out] */ int *piItem);


void __RPC_STUB IFolderView_SelectedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SelectItem_Proxy( 
    IFolderView * This,
    /* [in] */ int iItem,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IFolderView_SelectItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderView_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0152 */
/* [local] */ 

#ifdef _FIX_ENABLEMODELESS_CONFLICT
#undef    EnableModeless 
#endif
//
// IShellBrowser::BrowseObject(pidl, wFlags)
//
//   The view calls this member to let shell explorer browse to another
//  folder. The pidl and wFlags specifies the folder to be browsed.
#define SBSP_DEFBROWSER         0x0000
#define SBSP_SAMEBROWSER        0x0001
#define SBSP_NEWBROWSER         0x0002
#define SBSP_DEFMODE            0x0000
#define SBSP_OPENMODE           0x0010
#define SBSP_EXPLOREMODE        0x0020
#define SBSP_HELPMODE           0x0040 // IEUNIX : Help window uses this.
#define SBSP_NOTRANSFERHIST     0x0080
#define SBSP_ABSOLUTE           0x0000
#define SBSP_RELATIVE           0x1000
#define SBSP_PARENT             0x2000
#define SBSP_NAVIGATEBACK       0x4000
#define SBSP_NAVIGATEFORWARD    0x8000
#define SBSP_ALLOW_AUTONAVIGATE 0x10000
#define SBSP_NOAUTOSELECT       0x04000000
#define SBSP_WRITENOHISTORY     0x08000000
#define SBSP_REDIRECT                     0x40000000
#define SBSP_INITIATEDBYHLINKFRAME        0x80000000
#define FCW_STATUS      0x0001
#define FCW_TOOLBAR     0x0002
#define FCW_TREE        0x0003
#define FCW_INTERNETBAR 0x0006
#define FCW_PROGRESS    0x0008
#define FCT_MERGE       0x0001
#define FCT_CONFIGABLE  0x0002
#define FCT_ADDTOEND    0x0004
#ifdef _NEVER_
typedef LPARAM LPTBBUTTONSB;

#else //!_NEVER_
#include <commctrl.h>
typedef LPTBBUTTON LPTBBUTTONSB;
#endif //_NEVER_


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0152_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0152_v0_0_s_ifspec;

#ifndef __IShellBrowser_INTERFACE_DEFINED__
#define __IShellBrowser_INTERFACE_DEFINED__

/* interface IShellBrowser */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellBrowser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E2-0000-0000-C000-000000000046")
    IShellBrowser : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InsertMenusSB( 
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMenuSB( 
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenuRes,
            /* [in] */ HWND hwndActiveObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveMenusSB( 
            /* [in] */ HMENU hmenuShared) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStatusTextSB( 
            /* [unique][in] */ LPCOLESTR pszStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModelessSB( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TranslateAcceleratorSB( 
            /* [in] */ MSG *pmsg,
            /* [in] */ WORD wID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BrowseObject( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT wFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetViewStateStream( 
            /* [in] */ DWORD grfMode,
            /* [out] */ IStream **ppStrm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetControlWindow( 
            /* [in] */ UINT id,
            /* [out] */ HWND *lphwnd) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SendControlMsg( 
            /* [in] */ UINT id,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam,
            /* [in] */ LRESULT *pret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryActiveShellView( 
            /* [in] */ IShellView **ppshv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnViewWindowActive( 
            /* [in] */ IShellView *pshv) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetToolbarItems( 
            /* [in] */ LPTBBUTTONSB lpButtons,
            /* [in] */ UINT nButtons,
            /* [in] */ UINT uFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellBrowserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellBrowser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellBrowser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellBrowser * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellBrowser * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellBrowser * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *InsertMenusSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);
        
        HRESULT ( STDMETHODCALLTYPE *SetMenuSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenuRes,
            /* [in] */ HWND hwndActiveObject);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveMenusSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared);
        
        HRESULT ( STDMETHODCALLTYPE *SetStatusTextSB )( 
            IShellBrowser * This,
            /* [unique][in] */ LPCOLESTR pszStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModelessSB )( 
            IShellBrowser * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAcceleratorSB )( 
            IShellBrowser * This,
            /* [in] */ MSG *pmsg,
            /* [in] */ WORD wID);
        
        HRESULT ( STDMETHODCALLTYPE *BrowseObject )( 
            IShellBrowser * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT wFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetViewStateStream )( 
            IShellBrowser * This,
            /* [in] */ DWORD grfMode,
            /* [out] */ IStream **ppStrm);
        
        HRESULT ( STDMETHODCALLTYPE *GetControlWindow )( 
            IShellBrowser * This,
            /* [in] */ UINT id,
            /* [out] */ HWND *lphwnd);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SendControlMsg )( 
            IShellBrowser * This,
            /* [in] */ UINT id,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam,
            /* [in] */ LRESULT *pret);
        
        HRESULT ( STDMETHODCALLTYPE *QueryActiveShellView )( 
            IShellBrowser * This,
            /* [in] */ IShellView **ppshv);
        
        HRESULT ( STDMETHODCALLTYPE *OnViewWindowActive )( 
            IShellBrowser * This,
            /* [in] */ IShellView *pshv);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SetToolbarItems )( 
            IShellBrowser * This,
            /* [in] */ LPTBBUTTONSB lpButtons,
            /* [in] */ UINT nButtons,
            /* [in] */ UINT uFlags);
        
        END_INTERFACE
    } IShellBrowserVtbl;

    interface IShellBrowser
    {
        CONST_VTBL struct IShellBrowserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellBrowser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellBrowser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellBrowser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellBrowser_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellBrowser_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellBrowser_InsertMenusSB(This,hmenuShared,lpMenuWidths)	\
    (This)->lpVtbl -> InsertMenusSB(This,hmenuShared,lpMenuWidths)

#define IShellBrowser_SetMenuSB(This,hmenuShared,holemenuRes,hwndActiveObject)	\
    (This)->lpVtbl -> SetMenuSB(This,hmenuShared,holemenuRes,hwndActiveObject)

#define IShellBrowser_RemoveMenusSB(This,hmenuShared)	\
    (This)->lpVtbl -> RemoveMenusSB(This,hmenuShared)

#define IShellBrowser_SetStatusTextSB(This,pszStatusText)	\
    (This)->lpVtbl -> SetStatusTextSB(This,pszStatusText)

#define IShellBrowser_EnableModelessSB(This,fEnable)	\
    (This)->lpVtbl -> EnableModelessSB(This,fEnable)

#define IShellBrowser_TranslateAcceleratorSB(This,pmsg,wID)	\
    (This)->lpVtbl -> TranslateAcceleratorSB(This,pmsg,wID)

#define IShellBrowser_BrowseObject(This,pidl,wFlags)	\
    (This)->lpVtbl -> BrowseObject(This,pidl,wFlags)

#define IShellBrowser_GetViewStateStream(This,grfMode,ppStrm)	\
    (This)->lpVtbl -> GetViewStateStream(This,grfMode,ppStrm)

#define IShellBrowser_GetControlWindow(This,id,lphwnd)	\
    (This)->lpVtbl -> GetControlWindow(This,id,lphwnd)

#define IShellBrowser_SendControlMsg(This,id,uMsg,wParam,lParam,pret)	\
    (This)->lpVtbl -> SendControlMsg(This,id,uMsg,wParam,lParam,pret)

#define IShellBrowser_QueryActiveShellView(This,ppshv)	\
    (This)->lpVtbl -> QueryActiveShellView(This,ppshv)

#define IShellBrowser_OnViewWindowActive(This,pshv)	\
    (This)->lpVtbl -> OnViewWindowActive(This,pshv)

#define IShellBrowser_SetToolbarItems(This,lpButtons,nButtons,uFlags)	\
    (This)->lpVtbl -> SetToolbarItems(This,lpButtons,nButtons,uFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellBrowser_InsertMenusSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared,
    /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);


void __RPC_STUB IShellBrowser_InsertMenusSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_SetMenuSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared,
    /* [in] */ HOLEMENU holemenuRes,
    /* [in] */ HWND hwndActiveObject);


void __RPC_STUB IShellBrowser_SetMenuSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_RemoveMenusSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared);


void __RPC_STUB IShellBrowser_RemoveMenusSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_SetStatusTextSB_Proxy( 
    IShellBrowser * This,
    /* [unique][in] */ LPCOLESTR pszStatusText);


void __RPC_STUB IShellBrowser_SetStatusTextSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_EnableModelessSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IShellBrowser_EnableModelessSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_TranslateAcceleratorSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ MSG *pmsg,
    /* [in] */ WORD wID);


void __RPC_STUB IShellBrowser_TranslateAcceleratorSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_BrowseObject_Proxy( 
    IShellBrowser * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ UINT wFlags);


void __RPC_STUB IShellBrowser_BrowseObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_GetViewStateStream_Proxy( 
    IShellBrowser * This,
    /* [in] */ DWORD grfMode,
    /* [out] */ IStream **ppStrm);


void __RPC_STUB IShellBrowser_GetViewStateStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_GetControlWindow_Proxy( 
    IShellBrowser * This,
    /* [in] */ UINT id,
    /* [out] */ HWND *lphwnd);


void __RPC_STUB IShellBrowser_GetControlWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellBrowser_SendControlMsg_Proxy( 
    IShellBrowser * This,
    /* [in] */ UINT id,
    /* [in] */ UINT uMsg,
    /* [in] */ WPARAM wParam,
    /* [in] */ LPARAM lParam,
    /* [in] */ LRESULT *pret);


void __RPC_STUB IShellBrowser_SendControlMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_QueryActiveShellView_Proxy( 
    IShellBrowser * This,
    /* [in] */ IShellView **ppshv);


void __RPC_STUB IShellBrowser_QueryActiveShellView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_OnViewWindowActive_Proxy( 
    IShellBrowser * This,
    /* [in] */ IShellView *pshv);


void __RPC_STUB IShellBrowser_OnViewWindowActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellBrowser_SetToolbarItems_Proxy( 
    IShellBrowser * This,
    /* [in] */ LPTBBUTTONSB lpButtons,
    /* [in] */ UINT nButtons,
    /* [in] */ UINT uFlags);


void __RPC_STUB IShellBrowser_SetToolbarItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellBrowser_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0153 */
/* [local] */ 

typedef IShellBrowser *LPSHELLBROWSER;

#define SNCF_IGNORERAS   0x00000001


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0153_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0153_v0_0_s_ifspec;

#ifndef __IShellNetCrawler_INTERFACE_DEFINED__
#define __IShellNetCrawler_INTERFACE_DEFINED__

/* interface IShellNetCrawler */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellNetCrawler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49c929ee-a1b7-4c58-b539-e63be392b6f3")
    IShellNetCrawler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Update( 
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellNetCrawlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellNetCrawler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellNetCrawler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellNetCrawler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Update )( 
            IShellNetCrawler * This,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IShellNetCrawlerVtbl;

    interface IShellNetCrawler
    {
        CONST_VTBL struct IShellNetCrawlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellNetCrawler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellNetCrawler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellNetCrawler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellNetCrawler_Update(This,dwFlags)	\
    (This)->lpVtbl -> Update(This,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellNetCrawler_Update_Proxy( 
    IShellNetCrawler * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IShellNetCrawler_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellNetCrawler_INTERFACE_DEFINED__ */


#ifndef __IProfferService_INTERFACE_DEFINED__
#define __IProfferService_INTERFACE_DEFINED__

/* interface IProfferService */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IProfferService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cb728b20-f786-11ce-92ad-00aa00a74cd0")
    IProfferService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProfferService( 
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeService( 
            /* [in] */ DWORD dwCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProfferServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfferService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfferService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfferService * This);
        
        HRESULT ( STDMETHODCALLTYPE *ProfferService )( 
            IProfferService * This,
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeService )( 
            IProfferService * This,
            /* [in] */ DWORD dwCookie);
        
        END_INTERFACE
    } IProfferServiceVtbl;

    interface IProfferService
    {
        CONST_VTBL struct IProfferServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfferService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProfferService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProfferService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProfferService_ProfferService(This,rguidService,psp,pdwCookie)	\
    (This)->lpVtbl -> ProfferService(This,rguidService,psp,pdwCookie)

#define IProfferService_RevokeService(This,dwCookie)	\
    (This)->lpVtbl -> RevokeService(This,dwCookie)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IProfferService_ProfferService_Proxy( 
    IProfferService * This,
    /* [in] */ REFGUID rguidService,
    /* [in] */ IServiceProvider *psp,
    /* [out] */ DWORD *pdwCookie);


void __RPC_STUB IProfferService_ProfferService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IProfferService_RevokeService_Proxy( 
    IProfferService * This,
    /* [in] */ DWORD dwCookie);


void __RPC_STUB IProfferService_RevokeService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProfferService_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0155 */
/* [local] */ 

#define SID_SProfferService IID_IProfferService    // nearest service that you can proffer to
typedef /* [v1_enum] */ 
enum _CFITYPE
    {	CFITYPE_CSIDL	= 0,
	CFITYPE_PIDL	= CFITYPE_CSIDL + 1,
	CFITYPE_PATH	= CFITYPE_PIDL + 1
    } 	CFITYPE;

/* [v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0155_0001
    {	CFINITF_CHILDREN	= 0,
	CFINITF_FLAT	= 0x1
    } ;
typedef UINT CFINITF;

typedef struct _COMPFOLDERINIT
    {
    UINT uType;
    /* [switch_is][switch_type] */ union 
        {
        /* [case()] */ int csidl;
        /* [case()] */ LPCITEMIDLIST pidl;
        /* [case()][string] */ LPOLESTR pszPath;
        } 	DUMMYUNIONNAME;
    LPOLESTR pszName;
    } 	COMPFOLDERINIT;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0155_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0155_v0_0_s_ifspec;

#ifndef __ICompositeFolder_INTERFACE_DEFINED__
#define __ICompositeFolder_INTERFACE_DEFINED__

/* interface ICompositeFolder */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_ICompositeFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("601ac3dd-786a-4eb0-bf40-ee3521e70bfb")
    ICompositeFolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InitComposite( 
            /* [in] */ WORD wSignature,
            /* [in] */ REFCLSID refclsid,
            /* [in] */ CFINITF flags,
            /* [in] */ ULONG celt,
            /* [size_is][in] */ const COMPFOLDERINIT *rgCFs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToParent( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv,
            /* [out] */ LPITEMIDLIST *ppidlLast) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICompositeFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICompositeFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICompositeFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICompositeFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *InitComposite )( 
            ICompositeFolder * This,
            /* [in] */ WORD wSignature,
            /* [in] */ REFCLSID refclsid,
            /* [in] */ CFINITF flags,
            /* [in] */ ULONG celt,
            /* [size_is][in] */ const COMPFOLDERINIT *rgCFs);
        
        HRESULT ( STDMETHODCALLTYPE *BindToParent )( 
            ICompositeFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv,
            /* [out] */ LPITEMIDLIST *ppidlLast);
        
        END_INTERFACE
    } ICompositeFolderVtbl;

    interface ICompositeFolder
    {
        CONST_VTBL struct ICompositeFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICompositeFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICompositeFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICompositeFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICompositeFolder_InitComposite(This,wSignature,refclsid,flags,celt,rgCFs)	\
    (This)->lpVtbl -> InitComposite(This,wSignature,refclsid,flags,celt,rgCFs)

#define ICompositeFolder_BindToParent(This,pidl,riid,ppv,ppidlLast)	\
    (This)->lpVtbl -> BindToParent(This,pidl,riid,ppv,ppidlLast)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICompositeFolder_InitComposite_Proxy( 
    ICompositeFolder * This,
    /* [in] */ WORD wSignature,
    /* [in] */ REFCLSID refclsid,
    /* [in] */ CFINITF flags,
    /* [in] */ ULONG celt,
    /* [size_is][in] */ const COMPFOLDERINIT *rgCFs);


void __RPC_STUB ICompositeFolder_InitComposite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICompositeFolder_BindToParent_Proxy( 
    ICompositeFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv,
    /* [out] */ LPITEMIDLIST *ppidlLast);


void __RPC_STUB ICompositeFolder_BindToParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICompositeFolder_INTERFACE_DEFINED__ */


#ifndef __IParentAndItem_INTERFACE_DEFINED__
#define __IParentAndItem_INTERFACE_DEFINED__

/* interface IParentAndItem */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IParentAndItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b3a4b685-b685-4805-99d9-5dead2873236")
    IParentAndItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetParentAndItem( 
            /* [in] */ LPCITEMIDLIST pidlParent,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlChild) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParentAndItem( 
            /* [out] */ LPITEMIDLIST *ppidlParent,
            /* [out] */ IShellFolder **ppsf,
            /* [out] */ LPITEMIDLIST *ppidlChild) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IParentAndItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IParentAndItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IParentAndItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IParentAndItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetParentAndItem )( 
            IParentAndItem * This,
            /* [in] */ LPCITEMIDLIST pidlParent,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlChild);
        
        HRESULT ( STDMETHODCALLTYPE *GetParentAndItem )( 
            IParentAndItem * This,
            /* [out] */ LPITEMIDLIST *ppidlParent,
            /* [out] */ IShellFolder **ppsf,
            /* [out] */ LPITEMIDLIST *ppidlChild);
        
        END_INTERFACE
    } IParentAndItemVtbl;

    interface IParentAndItem
    {
        CONST_VTBL struct IParentAndItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IParentAndItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IParentAndItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IParentAndItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IParentAndItem_SetParentAndItem(This,pidlParent,psf,pidlChild)	\
    (This)->lpVtbl -> SetParentAndItem(This,pidlParent,psf,pidlChild)

#define IParentAndItem_GetParentAndItem(This,ppidlParent,ppsf,ppidlChild)	\
    (This)->lpVtbl -> GetParentAndItem(This,ppidlParent,ppsf,ppidlChild)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IParentAndItem_SetParentAndItem_Proxy( 
    IParentAndItem * This,
    /* [in] */ LPCITEMIDLIST pidlParent,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidlChild);


void __RPC_STUB IParentAndItem_SetParentAndItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IParentAndItem_GetParentAndItem_Proxy( 
    IParentAndItem * This,
    /* [out] */ LPITEMIDLIST *ppidlParent,
    /* [out] */ IShellFolder **ppsf,
    /* [out] */ LPITEMIDLIST *ppidlChild);


void __RPC_STUB IParentAndItem_GetParentAndItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IParentAndItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0157 */
/* [local] */ 

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0157_0001
    {	PUIF_DEFAULT	= 0,
	PUIF_RIGHTALIGN	= 0x1,
	PUIF_NOLABELININFOTIP	= 0x2
    } 	PROPERTYUI_FLAGS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0157_0002
    {	PUIFFDF_DEFAULT	= 0,
	PUIFFDF_RIGHTTOLEFT	= 0x1,
	PUIFFDF_SHORTFORMAT	= 0x2,
	PUIFFDF_NOTIME	= 0x4,
	PUIFFDF_FRIENDLYDATE	= 0x8,
	PUIFFDF_NOUNITS	= 0x10
    } 	PROPERTYUI_FORMAT_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0157_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0157_v0_0_s_ifspec;

#ifndef __IPropertyUI_INTERFACE_DEFINED__
#define __IPropertyUI_INTERFACE_DEFINED__

/* interface IPropertyUI */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IPropertyUI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d43eacec-eaaf-4aae-ba46-eecba97d988a")
    IPropertyUI : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParsePropertyName( 
            /* [in] */ LPCWSTR pszName,
            /* [out] */ FMTID *pfmtid,
            /* [out] */ PROPID *ppid,
            /* [out] */ ULONG *pchEaten) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCannonicalName( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertyDescription( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultWidth( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ ULONG *pcxChars) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ PROPERTYUI_FLAGS *pFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FormatForDisplay( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ const VARIANT *pvar,
            /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyUIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertyUI * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertyUI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertyUI * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParsePropertyName )( 
            IPropertyUI * This,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ FMTID *pfmtid,
            /* [out] */ PROPID *ppid,
            /* [out] */ ULONG *pchEaten);
        
        HRESULT ( STDMETHODCALLTYPE *GetCannonicalName )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropertyDescription )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultWidth )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ ULONG *pcxChars);
        
        HRESULT ( STDMETHODCALLTYPE *GetFlags )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ PROPERTYUI_FLAGS *pFlags);
        
        HRESULT ( STDMETHODCALLTYPE *FormatForDisplay )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ const VARIANT *pvar,
            /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        END_INTERFACE
    } IPropertyUIVtbl;

    interface IPropertyUI
    {
        CONST_VTBL struct IPropertyUIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyUI_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertyUI_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertyUI_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertyUI_ParsePropertyName(This,pszName,pfmtid,ppid,pchEaten)	\
    (This)->lpVtbl -> ParsePropertyName(This,pszName,pfmtid,ppid,pchEaten)

#define IPropertyUI_GetCannonicalName(This,fmtid,pid,pwszText,cchText)	\
    (This)->lpVtbl -> GetCannonicalName(This,fmtid,pid,pwszText,cchText)

#define IPropertyUI_GetDisplayName(This,fmtid,pid,pwszText,cchText)	\
    (This)->lpVtbl -> GetDisplayName(This,fmtid,pid,pwszText,cchText)

#define IPropertyUI_GetPropertyDescription(This,fmtid,pid,pwszText,cchText)	\
    (This)->lpVtbl -> GetPropertyDescription(This,fmtid,pid,pwszText,cchText)

#define IPropertyUI_GetDefaultWidth(This,fmtid,pid,pcxChars)	\
    (This)->lpVtbl -> GetDefaultWidth(This,fmtid,pid,pcxChars)

#define IPropertyUI_GetFlags(This,fmtid,pid,pFlags)	\
    (This)->lpVtbl -> GetFlags(This,fmtid,pid,pFlags)

#define IPropertyUI_FormatForDisplay(This,fmtid,pid,pvar,flags,pwszText,cchText)	\
    (This)->lpVtbl -> FormatForDisplay(This,fmtid,pid,pvar,flags,pwszText,cchText)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPropertyUI_ParsePropertyName_Proxy( 
    IPropertyUI * This,
    /* [in] */ LPCWSTR pszName,
    /* [out] */ FMTID *pfmtid,
    /* [out] */ PROPID *ppid,
    /* [out] */ ULONG *pchEaten);


void __RPC_STUB IPropertyUI_ParsePropertyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetCannonicalName_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetCannonicalName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetDisplayName_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetPropertyDescription_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetPropertyDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetDefaultWidth_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [out] */ ULONG *pcxChars);


void __RPC_STUB IPropertyUI_GetDefaultWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetFlags_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [out] */ PROPERTYUI_FLAGS *pFlags);


void __RPC_STUB IPropertyUI_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_FormatForDisplay_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [in] */ const VARIANT *pvar,
    /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_FormatForDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertyUI_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0158 */
/* [local] */ 

#if (_WIN32_IE >= 0x0500)
//-------------------------------------------------------------------------
//
// Folder Category Support
//


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0158_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0158_v0_0_s_ifspec;

#ifndef __ICategoryProvider_INTERFACE_DEFINED__
#define __ICategoryProvider_INTERFACE_DEFINED__

/* interface ICategoryProvider */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_ICategoryProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9af64809-5864-4c26-a720-c1f78c086ee3")
    ICategoryProvider : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CanCategorizeOnSCID( 
            SHCOLUMNID *pscid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDefaultCategory( 
            GUID *pguid,
            SHCOLUMNID *pscid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryForSCID( 
            SHCOLUMNID *pscid,
            GUID *pguid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumCategories( 
            IEnumGUID **penum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryName( 
            GUID *pguid,
            LPWSTR pszName,
            UINT cch) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateCategory( 
            GUID *pguid,
            REFIID riid,
            /* [iid_is] */ void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICategoryProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICategoryProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICategoryProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICategoryProvider * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CanCategorizeOnSCID )( 
            ICategoryProvider * This,
            SHCOLUMNID *pscid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDefaultCategory )( 
            ICategoryProvider * This,
            GUID *pguid,
            SHCOLUMNID *pscid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryForSCID )( 
            ICategoryProvider * This,
            SHCOLUMNID *pscid,
            GUID *pguid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnumCategories )( 
            ICategoryProvider * This,
            IEnumGUID **penum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryName )( 
            ICategoryProvider * This,
            GUID *pguid,
            LPWSTR pszName,
            UINT cch);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateCategory )( 
            ICategoryProvider * This,
            GUID *pguid,
            REFIID riid,
            /* [iid_is] */ void **ppv);
        
        END_INTERFACE
    } ICategoryProviderVtbl;

    interface ICategoryProvider
    {
        CONST_VTBL struct ICategoryProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICategoryProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICategoryProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICategoryProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICategoryProvider_CanCategorizeOnSCID(This,pscid)	\
    (This)->lpVtbl -> CanCategorizeOnSCID(This,pscid)

#define ICategoryProvider_GetDefaultCategory(This,pguid,pscid)	\
    (This)->lpVtbl -> GetDefaultCategory(This,pguid,pscid)

#define ICategoryProvider_GetCategoryForSCID(This,pscid,pguid)	\
    (This)->lpVtbl -> GetCategoryForSCID(This,pscid,pguid)

#define ICategoryProvider_EnumCategories(This,penum)	\
    (This)->lpVtbl -> EnumCategories(This,penum)

#define ICategoryProvider_GetCategoryName(This,pguid,pszName,cch)	\
    (This)->lpVtbl -> GetCategoryName(This,pguid,pszName,cch)

#define ICategoryProvider_CreateCategory(This,pguid,riid,ppv)	\
    (This)->lpVtbl -> CreateCategory(This,pguid,riid,ppv)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_CanCategorizeOnSCID_Proxy( 
    ICategoryProvider * This,
    SHCOLUMNID *pscid);


void __RPC_STUB ICategoryProvider_CanCategorizeOnSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetDefaultCategory_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    SHCOLUMNID *pscid);


void __RPC_STUB ICategoryProvider_GetDefaultCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetCategoryForSCID_Proxy( 
    ICategoryProvider * This,
    SHCOLUMNID *pscid,
    GUID *pguid);


void __RPC_STUB ICategoryProvider_GetCategoryForSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_EnumCategories_Proxy( 
    ICategoryProvider * This,
    IEnumGUID **penum);


void __RPC_STUB ICategoryProvider_EnumCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetCategoryName_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    LPWSTR pszName,
    UINT cch);


void __RPC_STUB ICategoryProvider_GetCategoryName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_CreateCategory_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    REFIID riid,
    /* [iid_is] */ void **ppv);


void __RPC_STUB ICategoryProvider_CreateCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICategoryProvider_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0159 */
/* [local] */ 

typedef /* [public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0159_0001
    {	CATINFO_NORMAL	= 0,
	CATINFO_COLLAPSED	= 0x1,
	CATINFO_HIDDEN	= 0x2
    } 	CATEGORYINFO_FLAGS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0159_0002
    {	CATSORT_DEFAULT	= 0,
	CATSORT_NAME	= 0x1
    } 	CATSORT_FLAGS;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_shobjidl_0159_0003
    {
    CATEGORYINFO_FLAGS cif;
    WCHAR wszName[ 260 ];
    } 	CATEGORY_INFO;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0159_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0159_v0_0_s_ifspec;

#ifndef __ICategorizer_INTERFACE_DEFINED__
#define __ICategorizer_INTERFACE_DEFINED__

/* interface ICategorizer */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_ICategorizer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a3b14589-9174-49a8-89a3-06a1ae2b9ba7")
    ICategorizer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDescription( 
            LPWSTR pszDesc,
            UINT cch) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategory( 
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][out][in] */ DWORD *rgCategoryIds) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryInfo( 
            DWORD dwCategoryId,
            CATEGORY_INFO *pci) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CompareCategory( 
            CATSORT_FLAGS csfFlags,
            DWORD dwCategoryId1,
            DWORD dwCategoryId2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICategorizerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICategorizer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICategorizer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICategorizer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            ICategorizer * This,
            LPWSTR pszDesc,
            UINT cch);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategory )( 
            ICategorizer * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][out][in] */ DWORD *rgCategoryIds);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryInfo )( 
            ICategorizer * This,
            DWORD dwCategoryId,
            CATEGORY_INFO *pci);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CompareCategory )( 
            ICategorizer * This,
            CATSORT_FLAGS csfFlags,
            DWORD dwCategoryId1,
            DWORD dwCategoryId2);
        
        END_INTERFACE
    } ICategorizerVtbl;

    interface ICategorizer
    {
        CONST_VTBL struct ICategorizerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICategorizer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICategorizer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICategorizer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICategorizer_GetDescription(This,pszDesc,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszDesc,cch)

#define ICategorizer_GetCategory(This,cidl,apidl,rgCategoryIds)	\
    (This)->lpVtbl -> GetCategory(This,cidl,apidl,rgCategoryIds)

#define ICategorizer_GetCategoryInfo(This,dwCategoryId,pci)	\
    (This)->lpVtbl -> GetCategoryInfo(This,dwCategoryId,pci)

#define ICategorizer_CompareCategory(This,csfFlags,dwCategoryId1,dwCategoryId2)	\
    (This)->lpVtbl -> CompareCategory(This,csfFlags,dwCategoryId1,dwCategoryId2)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetDescription_Proxy( 
    ICategorizer * This,
    LPWSTR pszDesc,
    UINT cch);


void __RPC_STUB ICategorizer_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetCategory_Proxy( 
    ICategorizer * This,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [size_is][out][in] */ DWORD *rgCategoryIds);


void __RPC_STUB ICategorizer_GetCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetCategoryInfo_Proxy( 
    ICategorizer * This,
    DWORD dwCategoryId,
    CATEGORY_INFO *pci);


void __RPC_STUB ICategorizer_GetCategoryInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_CompareCategory_Proxy( 
    ICategorizer * This,
    CATSORT_FLAGS csfFlags,
    DWORD dwCategoryId1,
    DWORD dwCategoryId2);


void __RPC_STUB ICategorizer_CompareCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICategorizer_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0160 */
/* [local] */ 

#endif // (_WIN32_IE >= 0x0500)
//  BindHandler GUIDs for IShellItem::BindToHandler (defined in shlguid.h)
//  BHID_SFObject            restricts usage to IShellFolder::BindToObject()
//  BHID_SFUIObject          restricts usage to IShellFolder::GetUIObjectOf()
//  BHID_SFViewObject        restricts usage to IShellFolder::CreateViewObject()
//  BHID_LocalCopyHelper     creates and initializes CLSID_LocalCopyHelper
//  BHID_LinkTargetItem      CLSID_ShellItem initialized with the target this item (SFGAO_LINK only)
//  BHID_Storage             attempts to get the stg/stm riid from BTO, but defaults to shell implementations on failure


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0160_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0160_v0_0_s_ifspec;

#ifndef __IShellItem_INTERFACE_DEFINED__
#define __IShellItem_INTERFACE_DEFINED__

/* interface IShellItem */
/* [unique][object][uuid][helpstring] */ 

typedef /* [public][public][v1_enum] */ 
enum __MIDL_IShellItem_0001
    {	SIGDN_NORMALDISPLAY	= 0,
	SIGDN_PARENTRELATIVEPARSING	= 0x80018001,
	SIGDN_DESKTOPABSOLUTEPARSING	= 0x80028000,
	SIGDN_PARENTRELATIVEEDITING	= 0x80031001,
	SIGDN_DESKTOPABSOLUTEEDITING	= 0x8004c000,
	SIGDN_FILESYSPATH	= 0x80058000,
	SIGDN_URL	= 0x80068000
    } 	SIGDN;


EXTERN_C const IID IID_IShellItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1079acf9-29bd-11d3-8e0d-00c04f6837d5")
    IShellItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BindToHandler( 
            /* [in] */ IBindCtx *pbc,
            /* [in] */ REFGUID rbhid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IShellItem **ppsi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [in] */ SIGDN sigdnName,
            /* [string][out] */ LPOLESTR *ppszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributes( 
            /* [in] */ SFGAOF sfgaoMask,
            /* [out] */ SFGAOF *psfgaoAttribs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE IsEqual( 
            /* [in] */ IShellItem *psi) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *BindToHandler )( 
            IShellItem * This,
            /* [in] */ IBindCtx *pbc,
            /* [in] */ REFGUID rbhid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IShellItem * This,
            /* [out] */ IShellItem **ppsi);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            IShellItem * This,
            /* [in] */ SIGDN sigdnName,
            /* [string][out] */ LPOLESTR *ppszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            IShellItem * This,
            /* [in] */ SFGAOF sfgaoMask,
            /* [out] */ SFGAOF *psfgaoAttribs);
        
        HRESULT ( STDMETHODCALLTYPE *IsEqual )( 
            IShellItem * This,
            /* [in] */ IShellItem *psi);
        
        END_INTERFACE
    } IShellItemVtbl;

    interface IShellItem
    {
        CONST_VTBL struct IShellItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellItem_BindToHandler(This,pbc,rbhid,riid,ppvOut)	\
    (This)->lpVtbl -> BindToHandler(This,pbc,rbhid,riid,ppvOut)

#define IShellItem_GetParent(This,ppsi)	\
    (This)->lpVtbl -> GetParent(This,ppsi)

#define IShellItem_GetDisplayName(This,sigdnName,ppszName)	\
    (This)->lpVtbl -> GetDisplayName(This,sigdnName,ppszName)

#define IShellItem_GetAttributes(This,sfgaoMask,psfgaoAttribs)	\
    (This)->lpVtbl -> GetAttributes(This,sfgaoMask,psfgaoAttribs)

#define IShellItem_IsEqual(This,psi)	\
    (This)->lpVtbl -> IsEqual(This,psi)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellItem_BindToHandler_Proxy( 
    IShellItem * This,
    /* [in] */ IBindCtx *pbc,
    /* [in] */ REFGUID rbhid,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvOut);


void __RPC_STUB IShellItem_BindToHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetParent_Proxy( 
    IShellItem * This,
    /* [out] */ IShellItem **ppsi);


void __RPC_STUB IShellItem_GetParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetDisplayName_Proxy( 
    IShellItem * This,
    /* [in] */ SIGDN sigdnName,
    /* [string][out] */ LPOLESTR *ppszName);


void __RPC_STUB IShellItem_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetAttributes_Proxy( 
    IShellItem * This,
    /* [in] */ SFGAOF sfgaoMask,
    /* [out] */ SFGAOF *psfgaoAttribs);


void __RPC_STUB IShellItem_GetAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_IsEqual_Proxy( 
    IShellItem * This,
    /* [in] */ IShellItem *psi);


void __RPC_STUB IShellItem_IsEqual_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellItem_INTERFACE_DEFINED__ */


#ifndef __IEnumShellItems_INTERFACE_DEFINED__
#define __IEnumShellItems_INTERFACE_DEFINED__

/* interface IEnumShellItems */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IEnumShellItems;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70629033-e363-4a28-a567-0db78006e6d7")
    IEnumShellItems : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IShellItem **rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumShellItems **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumShellItemsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumShellItems * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumShellItems * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumShellItems * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumShellItems * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ IShellItem **rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumShellItems * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumShellItems * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumShellItems * This,
            /* [out] */ IEnumShellItems **ppenum);
        
        END_INTERFACE
    } IEnumShellItemsVtbl;

    interface IEnumShellItems
    {
        CONST_VTBL struct IEnumShellItemsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumShellItems_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumShellItems_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumShellItems_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumShellItems_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumShellItems_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumShellItems_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumShellItems_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumShellItems_Next_Proxy( 
    IEnumShellItems * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ IShellItem **rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumShellItems_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumShellItems_Skip_Proxy( 
    IEnumShellItems * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumShellItems_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumShellItems_Reset_Proxy( 
    IEnumShellItems * This);


void __RPC_STUB IEnumShellItems_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumShellItems_Clone_Proxy( 
    IEnumShellItems * This,
    /* [out] */ IEnumShellItems **ppenum);


void __RPC_STUB IEnumShellItems_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumShellItems_INTERFACE_DEFINED__ */


#ifndef __IItemHandler_INTERFACE_DEFINED__
#define __IItemHandler_INTERFACE_DEFINED__

/* interface IItemHandler */
/* [object][uuid] */ 


EXTERN_C const IID IID_IItemHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c830b704-f3a3-4e3c-a099-d8bdbbc29a07")
    IItemHandler : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            IShellItem *psi) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IItemHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IItemHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IItemHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IItemHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IItemHandler * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IItemHandler * This,
            IShellItem *psi);
        
        END_INTERFACE
    } IItemHandlerVtbl;

    interface IItemHandler
    {
        CONST_VTBL struct IItemHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IItemHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IItemHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IItemHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IItemHandler_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IItemHandler_Init(This,psi)	\
    (This)->lpVtbl -> Init(This,psi)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IItemHandler_Init_Proxy( 
    IItemHandler * This,
    IShellItem *psi);


void __RPC_STUB IItemHandler_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IItemHandler_INTERFACE_DEFINED__ */


#ifndef __ILocalCopy_INTERFACE_DEFINED__
#define __ILocalCopy_INTERFACE_DEFINED__

/* interface ILocalCopy */
/* [object][uuid][helpstring] */ 

//  flags for ILocalCopy methods
//  LCDOWN_READONLY            read only helper, Upload() fails with E_ACCESSDENIED
//  LCDOWN_SAVEAS              write only helper, no download required for Download()
//  LC_FORCEROUNDTRIP          never use local cache (always roundtrip both Upload() and Download())

enum __MIDL_ILocalCopy_0001
    {	LCDOWN_READONLY	= 0x1,
	LC_SAVEAS	= 0x2,
	LC_FORCEROUNDTRIP	= 0x10
    } ;
typedef DWORD LCFLAGS;


EXTERN_C const IID IID_ILocalCopy;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("679d9e36-f8f9-11d2-8deb-00c04f6837d5")
    ILocalCopy : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Download( 
            /* [in] */ LCFLAGS flags,
            /* [in] */ IBindCtx *pbc,
            /* [string][out] */ LPWSTR *ppszPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Upload( 
            /* [in] */ LCFLAGS flags,
            /* [in] */ IBindCtx *pbc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ILocalCopyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ILocalCopy * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ILocalCopy * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ILocalCopy * This);
        
        HRESULT ( STDMETHODCALLTYPE *Download )( 
            ILocalCopy * This,
            /* [in] */ LCFLAGS flags,
            /* [in] */ IBindCtx *pbc,
            /* [string][out] */ LPWSTR *ppszPath);
        
        HRESULT ( STDMETHODCALLTYPE *Upload )( 
            ILocalCopy * This,
            /* [in] */ LCFLAGS flags,
            /* [in] */ IBindCtx *pbc);
        
        END_INTERFACE
    } ILocalCopyVtbl;

    interface ILocalCopy
    {
        CONST_VTBL struct ILocalCopyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ILocalCopy_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ILocalCopy_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ILocalCopy_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ILocalCopy_Download(This,flags,pbc,ppszPath)	\
    (This)->lpVtbl -> Download(This,flags,pbc,ppszPath)

#define ILocalCopy_Upload(This,flags,pbc)	\
    (This)->lpVtbl -> Upload(This,flags,pbc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ILocalCopy_Download_Proxy( 
    ILocalCopy * This,
    /* [in] */ LCFLAGS flags,
    /* [in] */ IBindCtx *pbc,
    /* [string][out] */ LPWSTR *ppszPath);


void __RPC_STUB ILocalCopy_Download_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ILocalCopy_Upload_Proxy( 
    ILocalCopy * This,
    /* [in] */ LCFLAGS flags,
    /* [in] */ IBindCtx *pbc);


void __RPC_STUB ILocalCopy_Upload_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ILocalCopy_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0164 */
/* [local] */ 

#ifdef UNICODE
#define IShellLink      IShellLinkW
#else
#define IShellLink      IShellLinkA
#endif
typedef /* [public] */ 
enum __MIDL___MIDL_itf_shobjidl_0164_0001
    {	SLR_NO_UI	= 0x1,
	SLR_ANY_MATCH	= 0x2,
	SLR_UPDATE	= 0x4,
	SLR_NOUPDATE	= 0x8,
	SLR_NOSEARCH	= 0x10,
	SLR_NOTRACK	= 0x20,
	SLR_NOLINKINFO	= 0x40,
	SLR_INVOKE_MSI	= 0x80
    } 	SLR_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_shobjidl_0164_0002
    {	SLGP_SHORTPATH	= 0x1,
	SLGP_UNCPRIORITY	= 0x2,
	SLGP_RAWPATH	= 0x4
    } 	SLGP_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0164_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0164_v0_0_s_ifspec;

#ifndef __IShellLinkA_INTERFACE_DEFINED__
#define __IShellLinkA_INTERFACE_DEFINED__

/* interface IShellLinkA */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IShellLinkA;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214EE-0000-0000-C000-000000000046")
    IShellLinkA : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPath( 
            /* [size_is][out] */ LPSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [size_is][out] */ LPSTR pszName,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDescription( 
            /* [in] */ LPCSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
            /* [size_is][out] */ LPSTR pszDir,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
            /* [in] */ LPCSTR pszDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetArguments( 
            /* [size_is][out] */ LPSTR pszArgs,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetArguments( 
            /* [in] */ LPCSTR pszArgs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHotkey( 
            /* [out] */ WORD *pwHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHotkey( 
            /* [in] */ WORD wHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetShowCmd( 
            /* [out] */ int *piShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetShowCmd( 
            /* [in] */ int iShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIconLocation( 
            /* [size_is][out] */ LPSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconLocation( 
            /* [in] */ LPCSTR pszIconPath,
            /* [in] */ int iIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRelativePath( 
            /* [in] */ LPCSTR pszPathRel,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resolve( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ LPCSTR pszFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkAVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkA * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkA * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkA * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDList )( 
            IShellLinkA * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *SetIDList )( 
            IShellLinkA * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszName,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetDescription )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetWorkingDirectory )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszDir,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetWorkingDirectory )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszDir);
        
        HRESULT ( STDMETHODCALLTYPE *GetArguments )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszArgs,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetArguments )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetHotkey )( 
            IShellLinkA * This,
            /* [out] */ WORD *pwHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *SetHotkey )( 
            IShellLinkA * This,
            /* [in] */ WORD wHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *GetShowCmd )( 
            IShellLinkA * This,
            /* [out] */ int *piShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetShowCmd )( 
            IShellLinkA * This,
            /* [in] */ int iShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszIconPath,
            /* [in] */ int iIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetRelativePath )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszPathRel,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkA * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszFile);
        
        END_INTERFACE
    } IShellLinkAVtbl;

    interface IShellLinkA
    {
        CONST_VTBL struct IShellLinkAVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkA_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkA_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkA_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkA_GetPath(This,pszFile,cch,pfd,fFlags)	\
    (This)->lpVtbl -> GetPath(This,pszFile,cch,pfd,fFlags)

#define IShellLinkA_GetIDList(This,ppidl)	\
    (This)->lpVtbl -> GetIDList(This,ppidl)

#define IShellLinkA_SetIDList(This,pidl)	\
    (This)->lpVtbl -> SetIDList(This,pidl)

#define IShellLinkA_GetDescription(This,pszName,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszName,cch)

#define IShellLinkA_SetDescription(This,pszName)	\
    (This)->lpVtbl -> SetDescription(This,pszName)

#define IShellLinkA_GetWorkingDirectory(This,pszDir,cch)	\
    (This)->lpVtbl -> GetWorkingDirectory(This,pszDir,cch)

#define IShellLinkA_SetWorkingDirectory(This,pszDir)	\
    (This)->lpVtbl -> SetWorkingDirectory(This,pszDir)

#define IShellLinkA_GetArguments(This,pszArgs,cch)	\
    (This)->lpVtbl -> GetArguments(This,pszArgs,cch)

#define IShellLinkA_SetArguments(This,pszArgs)	\
    (This)->lpVtbl -> SetArguments(This,pszArgs)

#define IShellLinkA_GetHotkey(This,pwHotkey)	\
    (This)->lpVtbl -> GetHotkey(This,pwHotkey)

#define IShellLinkA_SetHotkey(This,wHotkey)	\
    (This)->lpVtbl -> SetHotkey(This,wHotkey)

#define IShellLinkA_GetShowCmd(This,piShowCmd)	\
    (This)->lpVtbl -> GetShowCmd(This,piShowCmd)

#define IShellLinkA_SetShowCmd(This,iShowCmd)	\
    (This)->lpVtbl -> SetShowCmd(This,iShowCmd)

#define IShellLinkA_GetIconLocation(This,pszIconPath,cch,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pszIconPath,cch,piIcon)

#define IShellLinkA_SetIconLocation(This,pszIconPath,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,pszIconPath,iIcon)

#define IShellLinkA_SetRelativePath(This,pszPathRel,dwReserved)	\
    (This)->lpVtbl -> SetRelativePath(This,pszPathRel,dwReserved)

#define IShellLinkA_Resolve(This,hwnd,fFlags)	\
    (This)->lpVtbl -> Resolve(This,hwnd,fFlags)

#define IShellLinkA_SetPath(This,pszFile)	\
    (This)->lpVtbl -> SetPath(This,pszFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellLinkA_GetPath_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszFile,
    /* [in] */ int cch,
    /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkA_GetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetIDList_Proxy( 
    IShellLinkA * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IShellLinkA_GetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetIDList_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IShellLinkA_SetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetDescription_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszName,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetDescription_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszName);


void __RPC_STUB IShellLinkA_SetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetWorkingDirectory_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszDir,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetWorkingDirectory_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszDir);


void __RPC_STUB IShellLinkA_SetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetArguments_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszArgs,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetArguments_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszArgs);


void __RPC_STUB IShellLinkA_SetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetHotkey_Proxy( 
    IShellLinkA * This,
    /* [out] */ WORD *pwHotkey);


void __RPC_STUB IShellLinkA_GetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetHotkey_Proxy( 
    IShellLinkA * This,
    /* [in] */ WORD wHotkey);


void __RPC_STUB IShellLinkA_SetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetShowCmd_Proxy( 
    IShellLinkA * This,
    /* [out] */ int *piShowCmd);


void __RPC_STUB IShellLinkA_GetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetShowCmd_Proxy( 
    IShellLinkA * This,
    /* [in] */ int iShowCmd);


void __RPC_STUB IShellLinkA_SetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetIconLocation_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszIconPath,
    /* [in] */ int cch,
    /* [out] */ int *piIcon);


void __RPC_STUB IShellLinkA_GetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetIconLocation_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszIconPath,
    /* [in] */ int iIcon);


void __RPC_STUB IShellLinkA_SetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetRelativePath_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszPathRel,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IShellLinkA_SetRelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_Resolve_Proxy( 
    IShellLinkA * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkA_Resolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetPath_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszFile);


void __RPC_STUB IShellLinkA_SetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkA_INTERFACE_DEFINED__ */


#ifndef __IShellLinkW_INTERFACE_DEFINED__
#define __IShellLinkW_INTERFACE_DEFINED__

/* interface IShellLinkW */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IShellLinkW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214F9-0000-0000-C000-000000000046")
    IShellLinkW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPath( 
            /* [size_is][out] */ LPWSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [size_is][out] */ LPWSTR pszName,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDescription( 
            /* [in] */ LPCWSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
            /* [size_is][out] */ LPWSTR pszDir,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
            /* [in] */ LPCWSTR pszDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetArguments( 
            /* [size_is][out] */ LPWSTR pszArgs,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetArguments( 
            /* [in] */ LPCWSTR pszArgs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHotkey( 
            /* [out] */ WORD *pwHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHotkey( 
            /* [in] */ WORD wHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetShowCmd( 
            /* [out] */ int *piShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetShowCmd( 
            /* [in] */ int iShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIconLocation( 
            /* [size_is][out] */ LPWSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconLocation( 
            /* [in] */ LPCWSTR pszIconPath,
            /* [in] */ int iIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRelativePath( 
            /* [in] */ LPCWSTR pszPathRel,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resolve( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ LPCWSTR pszFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkW * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDList )( 
            IShellLinkW * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *SetIDList )( 
            IShellLinkW * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszName,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetDescription )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetWorkingDirectory )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszDir,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetWorkingDirectory )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszDir);
        
        HRESULT ( STDMETHODCALLTYPE *GetArguments )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszArgs,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetArguments )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetHotkey )( 
            IShellLinkW * This,
            /* [out] */ WORD *pwHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *SetHotkey )( 
            IShellLinkW * This,
            /* [in] */ WORD wHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *GetShowCmd )( 
            IShellLinkW * This,
            /* [out] */ int *piShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetShowCmd )( 
            IShellLinkW * This,
            /* [in] */ int iShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszIconPath,
            /* [in] */ int iIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetRelativePath )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszPathRel,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkW * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszFile);
        
        END_INTERFACE
    } IShellLinkWVtbl;

    interface IShellLinkW
    {
        CONST_VTBL struct IShellLinkWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkW_GetPath(This,pszFile,cch,pfd,fFlags)	\
    (This)->lpVtbl -> GetPath(This,pszFile,cch,pfd,fFlags)

#define IShellLinkW_GetIDList(This,ppidl)	\
    (This)->lpVtbl -> GetIDList(This,ppidl)

#define IShellLinkW_SetIDList(This,pidl)	\
    (This)->lpVtbl -> SetIDList(This,pidl)

#define IShellLinkW_GetDescription(This,pszName,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszName,cch)

#define IShellLinkW_SetDescription(This,pszName)	\
    (This)->lpVtbl -> SetDescription(This,pszName)

#define IShellLinkW_GetWorkingDirectory(This,pszDir,cch)	\
    (This)->lpVtbl -> GetWorkingDirectory(This,pszDir,cch)

#define IShellLinkW_SetWorkingDirectory(This,pszDir)	\
    (This)->lpVtbl -> SetWorkingDirectory(This,pszDir)

#define IShellLinkW_GetArguments(This,pszArgs,cch)	\
    (This)->lpVtbl -> GetArguments(This,pszArgs,cch)

#define IShellLinkW_SetArguments(This,pszArgs)	\
    (This)->lpVtbl -> SetArguments(This,pszArgs)

#define IShellLinkW_GetHotkey(This,pwHotkey)	\
    (This)->lpVtbl -> GetHotkey(This,pwHotkey)

#define IShellLinkW_SetHotkey(This,wHotkey)	\
    (This)->lpVtbl -> SetHotkey(This,wHotkey)

#define IShellLinkW_GetShowCmd(This,piShowCmd)	\
    (This)->lpVtbl -> GetShowCmd(This,piShowCmd)

#define IShellLinkW_SetShowCmd(This,iShowCmd)	\
    (This)->lpVtbl -> SetShowCmd(This,iShowCmd)

#define IShellLinkW_GetIconLocation(This,pszIconPath,cch,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pszIconPath,cch,piIcon)

#define IShellLinkW_SetIconLocation(This,pszIconPath,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,pszIconPath,iIcon)

#define IShellLinkW_SetRelativePath(This,pszPathRel,dwReserved)	\
    (This)->lpVtbl -> SetRelativePath(This,pszPathRel,dwReserved)

#define IShellLinkW_Resolve(This,hwnd,fFlags)	\
    (This)->lpVtbl -> Resolve(This,hwnd,fFlags)

#define IShellLinkW_SetPath(This,pszFile)	\
    (This)->lpVtbl -> SetPath(This,pszFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellLinkW_GetPath_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszFile,
    /* [in] */ int cch,
    /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkW_GetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetIDList_Proxy( 
    IShellLinkW * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IShellLinkW_GetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetIDList_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IShellLinkW_SetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetDescription_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszName,
    int cch);


void __RPC_STUB IShellLinkW_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetDescription_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszName);


void __RPC_STUB IShellLinkW_SetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetWorkingDirectory_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszDir,
    int cch);


void __RPC_STUB IShellLinkW_GetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetWorkingDirectory_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszDir);


void __RPC_STUB IShellLinkW_SetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetArguments_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszArgs,
    int cch);


void __RPC_STUB IShellLinkW_GetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetArguments_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszArgs);


void __RPC_STUB IShellLinkW_SetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetHotkey_Proxy( 
    IShellLinkW * This,
    /* [out] */ WORD *pwHotkey);


void __RPC_STUB IShellLinkW_GetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetHotkey_Proxy( 
    IShellLinkW * This,
    /* [in] */ WORD wHotkey);


void __RPC_STUB IShellLinkW_SetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetShowCmd_Proxy( 
    IShellLinkW * This,
    /* [out] */ int *piShowCmd);


void __RPC_STUB IShellLinkW_GetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetShowCmd_Proxy( 
    IShellLinkW * This,
    /* [in] */ int iShowCmd);


void __RPC_STUB IShellLinkW_SetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetIconLocation_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszIconPath,
    /* [in] */ int cch,
    /* [out] */ int *piIcon);


void __RPC_STUB IShellLinkW_GetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetIconLocation_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszIconPath,
    /* [in] */ int iIcon);


void __RPC_STUB IShellLinkW_SetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetRelativePath_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszPathRel,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IShellLinkW_SetRelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_Resolve_Proxy( 
    IShellLinkW * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkW_Resolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetPath_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszFile);


void __RPC_STUB IShellLinkW_SetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkW_INTERFACE_DEFINED__ */


#ifndef __IActionProgressDialog_INTERFACE_DEFINED__
#define __IActionProgressDialog_INTERFACE_DEFINED__

/* interface IActionProgressDialog */
/* [unique][helpstring][uuid][object] */ 

/* [v1_enum] */ 
enum __MIDL_IActionProgressDialog_0001
    {	SPINITF_NORMAL	= 0,
	SPINITF_MODAL	= 0x1,
	SPINITF_NOMINIMIZE	= 0x8
    } ;
typedef DWORD SPINITF;


EXTERN_C const IID IID_IActionProgressDialog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49ff1172-eadc-446d-9285-156453a6431c")
    IActionProgressDialog : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ SPINITF flags,
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszCancel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActionProgressDialogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionProgressDialog * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionProgressDialog * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionProgressDialog * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IActionProgressDialog * This,
            /* [in] */ SPINITF flags,
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszCancel);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IActionProgressDialog * This);
        
        END_INTERFACE
    } IActionProgressDialogVtbl;

    interface IActionProgressDialog
    {
        CONST_VTBL struct IActionProgressDialogVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionProgressDialog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActionProgressDialog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActionProgressDialog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActionProgressDialog_Initialize(This,flags,pszTitle,pszCancel)	\
    (This)->lpVtbl -> Initialize(This,flags,pszTitle,pszCancel)

#define IActionProgressDialog_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActionProgressDialog_Initialize_Proxy( 
    IActionProgressDialog * This,
    /* [in] */ SPINITF flags,
    /* [string][in] */ LPCWSTR pszTitle,
    /* [string][in] */ LPCWSTR pszCancel);


void __RPC_STUB IActionProgressDialog_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgressDialog_Stop_Proxy( 
    IActionProgressDialog * This);


void __RPC_STUB IActionProgressDialog_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActionProgressDialog_INTERFACE_DEFINED__ */


#ifndef __IHWEventHandler_INTERFACE_DEFINED__
#define __IHWEventHandler_INTERFACE_DEFINED__

/* interface IHWEventHandler */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHWEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1FB73D0-EC3A-4ba2-B512-8CDB9187B6D1")
    IHWEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [string][in] */ LPCWSTR pszParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleEvent( 
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHWEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHWEventHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHWEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHWEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IHWEventHandler * This,
            /* [string][in] */ LPCWSTR pszParams);
        
        HRESULT ( STDMETHODCALLTYPE *HandleEvent )( 
            IHWEventHandler * This,
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType);
        
        END_INTERFACE
    } IHWEventHandlerVtbl;

    interface IHWEventHandler
    {
        CONST_VTBL struct IHWEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHWEventHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHWEventHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHWEventHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHWEventHandler_Initialize(This,pszParams)	\
    (This)->lpVtbl -> Initialize(This,pszParams)

#define IHWEventHandler_HandleEvent(This,pszDeviceID,pszAltDeviceID,pszEventType)	\
    (This)->lpVtbl -> HandleEvent(This,pszDeviceID,pszAltDeviceID,pszEventType)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHWEventHandler_Initialize_Proxy( 
    IHWEventHandler * This,
    /* [string][in] */ LPCWSTR pszParams);


void __RPC_STUB IHWEventHandler_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHWEventHandler_HandleEvent_Proxy( 
    IHWEventHandler * This,
    /* [string][in] */ LPCWSTR pszDeviceID,
    /* [string][in] */ LPCWSTR pszAltDeviceID,
    /* [string][in] */ LPCWSTR pszEventType);


void __RPC_STUB IHWEventHandler_HandleEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHWEventHandler_INTERFACE_DEFINED__ */


#ifndef __IActionProgress_INTERFACE_DEFINED__
#define __IActionProgress_INTERFACE_DEFINED__

/* interface IActionProgress */
/* [unique][helpstring][uuid][object] */ 

/* [v1_enum] */ 
enum __MIDL_IActionProgress_0001
    {	SPBEGINF_NORMAL	= 0,
	SPBEGINF_AUTOTIME	= 0x2,
	SPBEGINF_NOPROGRESSBAR	= 0x10
    } ;
typedef DWORD SPBEGINF;

typedef /* [v1_enum] */ 
enum _SPACTION
    {	SPACTION_NONE	= 0,
	SPACTION_MOVING	= SPACTION_NONE + 1,
	SPACTION_COPYING	= SPACTION_MOVING + 1,
	SPACTION_RECYCLING	= SPACTION_COPYING + 1,
	SPACTION_APPLYINGATTRIBS	= SPACTION_RECYCLING + 1,
	SPACTION_DOWNLOADING	= SPACTION_APPLYINGATTRIBS + 1,
	SPACTION_SEARCHING_INTERNET	= SPACTION_DOWNLOADING + 1,
	SPACTION_CALCULATING	= SPACTION_SEARCHING_INTERNET + 1,
	SPACTION_UPLOADING	= SPACTION_CALCULATING + 1
    } 	SPACTION;

typedef /* [v1_enum] */ 
enum _SPTEXT
    {	SPTEXT_ACTIONDESCRIPTION	= 1,
	SPTEXT_ACTIONDETAIL	= SPTEXT_ACTIONDESCRIPTION + 1
    } 	SPTEXT;


EXTERN_C const IID IID_IActionProgress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49ff1173-eadc-446d-9285-156453a6431c")
    IActionProgress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin( 
            /* [in] */ SPACTION action,
            /* [in] */ SPBEGINF flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateProgress( 
            /* [in] */ ULONGLONG ulCompleted,
            /* [in] */ ULONGLONG ulTotal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateText( 
            /* [in] */ SPTEXT sptext,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ BOOL fMayCompact) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryCancel( 
            /* [out] */ BOOL *pfCancelled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetCancel( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE End( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActionProgressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionProgress * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionProgress * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionProgress * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin )( 
            IActionProgress * This,
            /* [in] */ SPACTION action,
            /* [in] */ SPBEGINF flags);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateProgress )( 
            IActionProgress * This,
            /* [in] */ ULONGLONG ulCompleted,
            /* [in] */ ULONGLONG ulTotal);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateText )( 
            IActionProgress * This,
            /* [in] */ SPTEXT sptext,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ BOOL fMayCompact);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCancel )( 
            IActionProgress * This,
            /* [out] */ BOOL *pfCancelled);
        
        HRESULT ( STDMETHODCALLTYPE *ResetCancel )( 
            IActionProgress * This);
        
        HRESULT ( STDMETHODCALLTYPE *End )( 
            IActionProgress * This);
        
        END_INTERFACE
    } IActionProgressVtbl;

    interface IActionProgress
    {
        CONST_VTBL struct IActionProgressVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionProgress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActionProgress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActionProgress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActionProgress_Begin(This,action,flags)	\
    (This)->lpVtbl -> Begin(This,action,flags)

#define IActionProgress_UpdateProgress(This,ulCompleted,ulTotal)	\
    (This)->lpVtbl -> UpdateProgress(This,ulCompleted,ulTotal)

#define IActionProgress_UpdateText(This,sptext,pszText,fMayCompact)	\
    (This)->lpVtbl -> UpdateText(This,sptext,pszText,fMayCompact)

#define IActionProgress_QueryCancel(This,pfCancelled)	\
    (This)->lpVtbl -> QueryCancel(This,pfCancelled)

#define IActionProgress_ResetCancel(This)	\
    (This)->lpVtbl -> ResetCancel(This)

#define IActionProgress_End(This)	\
    (This)->lpVtbl -> End(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActionProgress_Begin_Proxy( 
    IActionProgress * This,
    /* [in] */ SPACTION action,
    /* [in] */ SPBEGINF flags);


void __RPC_STUB IActionProgress_Begin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_UpdateProgress_Proxy( 
    IActionProgress * This,
    /* [in] */ ULONGLONG ulCompleted,
    /* [in] */ ULONGLONG ulTotal);


void __RPC_STUB IActionProgress_UpdateProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_UpdateText_Proxy( 
    IActionProgress * This,
    /* [in] */ SPTEXT sptext,
    /* [string][in] */ LPCWSTR pszText,
    /* [in] */ BOOL fMayCompact);


void __RPC_STUB IActionProgress_UpdateText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_QueryCancel_Proxy( 
    IActionProgress * This,
    /* [out] */ BOOL *pfCancelled);


void __RPC_STUB IActionProgress_QueryCancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_ResetCancel_Proxy( 
    IActionProgress * This);


void __RPC_STUB IActionProgress_ResetCancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_End_Proxy( 
    IActionProgress * This);


void __RPC_STUB IActionProgress_End_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActionProgress_INTERFACE_DEFINED__ */


#ifndef __IShellExtInit_INTERFACE_DEFINED__
#define __IShellExtInit_INTERFACE_DEFINED__

/* interface IShellExtInit */
/* [unique][local][object][uuid] */ 


EXTERN_C const IID IID_IShellExtInit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E8-0000-0000-C000-000000000046")
    IShellExtInit : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ IDataObject *pdtobj,
            /* [in] */ HKEY hkeyProgID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellExtInitVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellExtInit * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellExtInit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellExtInit * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IShellExtInit * This,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ IDataObject *pdtobj,
            /* [in] */ HKEY hkeyProgID);
        
        END_INTERFACE
    } IShellExtInitVtbl;

    interface IShellExtInit
    {
        CONST_VTBL struct IShellExtInitVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellExtInit_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellExtInit_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellExtInit_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellExtInit_Initialize(This,pidlFolder,pdtobj,hkeyProgID)	\
    (This)->lpVtbl -> Initialize(This,pidlFolder,pdtobj,hkeyProgID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellExtInit_Initialize_Proxy( 
    IShellExtInit * This,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    /* [in] */ IDataObject *pdtobj,
    /* [in] */ HKEY hkeyProgID);


void __RPC_STUB IShellExtInit_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellExtInit_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0170 */
/* [local] */ 

typedef IShellExtInit *LPSHELLEXTINIT;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0170_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0170_v0_0_s_ifspec;

#ifndef __IShellPropSheetExt_INTERFACE_DEFINED__
#define __IShellPropSheetExt_INTERFACE_DEFINED__

/* interface IShellPropSheetExt */
/* [unique][local][object][uuid] */ 


enum __MIDL_IShellPropSheetExt_0001
    {	EXPPS_FILETYPES	= 0x1
    } ;
typedef UINT EXPPS;


EXTERN_C const IID IID_IShellPropSheetExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E9-0000-0000-C000-000000000046")
    IShellPropSheetExt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddPages( 
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
            /* [in] */ LPARAM lParam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplacePage( 
            /* [in] */ EXPPS uPageID,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
            /* [in] */ LPARAM lParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellPropSheetExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellPropSheetExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellPropSheetExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellPropSheetExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPages )( 
            IShellPropSheetExt * This,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
            /* [in] */ LPARAM lParam);
        
        HRESULT ( STDMETHODCALLTYPE *ReplacePage )( 
            IShellPropSheetExt * This,
            /* [in] */ EXPPS uPageID,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
            /* [in] */ LPARAM lParam);
        
        END_INTERFACE
    } IShellPropSheetExtVtbl;

    interface IShellPropSheetExt
    {
        CONST_VTBL struct IShellPropSheetExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellPropSheetExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellPropSheetExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellPropSheetExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellPropSheetExt_AddPages(This,pfnAddPage,lParam)	\
    (This)->lpVtbl -> AddPages(This,pfnAddPage,lParam)

#define IShellPropSheetExt_ReplacePage(This,uPageID,pfnReplaceWith,lParam)	\
    (This)->lpVtbl -> ReplacePage(This,uPageID,pfnReplaceWith,lParam)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellPropSheetExt_AddPages_Proxy( 
    IShellPropSheetExt * This,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
    /* [in] */ LPARAM lParam);


void __RPC_STUB IShellPropSheetExt_AddPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellPropSheetExt_ReplacePage_Proxy( 
    IShellPropSheetExt * This,
    /* [in] */ EXPPS uPageID,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
    /* [in] */ LPARAM lParam);


void __RPC_STUB IShellPropSheetExt_ReplacePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellPropSheetExt_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0171 */
/* [local] */ 

typedef IShellPropSheetExt *LPSHELLPROPSHEETEXT;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0171_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0171_v0_0_s_ifspec;

#ifndef __IRemoteComputer_INTERFACE_DEFINED__
#define __IRemoteComputer_INTERFACE_DEFINED__

/* interface IRemoteComputer */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IRemoteComputer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214FE-0000-0000-C000-000000000046")
    IRemoteComputer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCWSTR pszMachine,
            /* [in] */ BOOL bEnumerating) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRemoteComputerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRemoteComputer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRemoteComputer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRemoteComputer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IRemoteComputer * This,
            /* [in] */ LPCWSTR pszMachine,
            /* [in] */ BOOL bEnumerating);
        
        END_INTERFACE
    } IRemoteComputerVtbl;

    interface IRemoteComputer
    {
        CONST_VTBL struct IRemoteComputerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRemoteComputer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteComputer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRemoteComputer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRemoteComputer_Initialize(This,pszMachine,bEnumerating)	\
    (This)->lpVtbl -> Initialize(This,pszMachine,bEnumerating)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRemoteComputer_Initialize_Proxy( 
    IRemoteComputer * This,
    /* [in] */ LPCWSTR pszMachine,
    /* [in] */ BOOL bEnumerating);


void __RPC_STUB IRemoteComputer_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRemoteComputer_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0172 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0172_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0172_v0_0_s_ifspec;

#ifndef __IDavStorage_INTERFACE_DEFINED__
#define __IDavStorage_INTERFACE_DEFINED__

/* interface IDavStorage */
/* [object][uuid][helpstring] */ 


EXTERN_C const IID IID_IDavStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D0E62481-D5A2-4b86-AC11-C048E6D81DB6")
    IDavStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ LPWSTR pwszUrl,
            /* [in] */ LPWSTR pwszUsername,
            /* [in] */ LPWSTR pwszPassword) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDavStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDavStorage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDavStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDavStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *Init )( 
            IDavStorage * This,
            /* [in] */ LPWSTR pwszUrl,
            /* [in] */ LPWSTR pwszUsername,
            /* [in] */ LPWSTR pwszPassword);
        
        END_INTERFACE
    } IDavStorageVtbl;

    interface IDavStorage
    {
        CONST_VTBL struct IDavStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDavStorage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDavStorage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDavStorage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDavStorage_Init(This,pwszUrl,pwszUsername,pwszPassword)	\
    (This)->lpVtbl -> Init(This,pwszUrl,pwszUsername,pwszPassword)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDavStorage_Init_Proxy( 
    IDavStorage * This,
    /* [in] */ LPWSTR pwszUrl,
    /* [in] */ LPWSTR pwszUsername,
    /* [in] */ LPWSTR pwszPassword);


void __RPC_STUB IDavStorage_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDavStorage_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0173 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0173_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0173_v0_0_s_ifspec;

#ifndef __IDynamicStorage_INTERFACE_DEFINED__
#define __IDynamicStorage_INTERFACE_DEFINED__

/* interface IDynamicStorage */
/* [object][uuid][helpstring] */ 


EXTERN_C const IID IID_IDynamicStorage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("694D8DB5-F7A4-4e72-A547-2F3DD5FA5B0D")
    IDynamicStorage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddIDList( 
            /* [in] */ DWORD cpidl,
            /* [size_is][in] */ LPITEMIDLIST *rgpidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddUnknown( 
            /* [in] */ REFIID riid,
            /* [in] */ IUnknown *pUnknown) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddStorage( 
            /* [in] */ IStorage *pStorage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddStream( 
            /* [in] */ IStream *pStream) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddDataObject( 
            /* [in] */ LPDATAOBJECT pdtobj) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDynamicStorageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDynamicStorage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDynamicStorage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDynamicStorage * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddIDList )( 
            IDynamicStorage * This,
            /* [in] */ DWORD cpidl,
            /* [size_is][in] */ LPITEMIDLIST *rgpidl);
        
        HRESULT ( STDMETHODCALLTYPE *AddUnknown )( 
            IDynamicStorage * This,
            /* [in] */ REFIID riid,
            /* [in] */ IUnknown *pUnknown);
        
        HRESULT ( STDMETHODCALLTYPE *AddStorage )( 
            IDynamicStorage * This,
            /* [in] */ IStorage *pStorage);
        
        HRESULT ( STDMETHODCALLTYPE *AddStream )( 
            IDynamicStorage * This,
            /* [in] */ IStream *pStream);
        
        HRESULT ( STDMETHODCALLTYPE *AddDataObject )( 
            IDynamicStorage * This,
            /* [in] */ LPDATAOBJECT pdtobj);
        
        END_INTERFACE
    } IDynamicStorageVtbl;

    interface IDynamicStorage
    {
        CONST_VTBL struct IDynamicStorageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDynamicStorage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDynamicStorage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDynamicStorage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDynamicStorage_AddIDList(This,cpidl,rgpidl)	\
    (This)->lpVtbl -> AddIDList(This,cpidl,rgpidl)

#define IDynamicStorage_AddUnknown(This,riid,pUnknown)	\
    (This)->lpVtbl -> AddUnknown(This,riid,pUnknown)

#define IDynamicStorage_AddStorage(This,pStorage)	\
    (This)->lpVtbl -> AddStorage(This,pStorage)

#define IDynamicStorage_AddStream(This,pStream)	\
    (This)->lpVtbl -> AddStream(This,pStream)

#define IDynamicStorage_AddDataObject(This,pdtobj)	\
    (This)->lpVtbl -> AddDataObject(This,pdtobj)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDynamicStorage_AddIDList_Proxy( 
    IDynamicStorage * This,
    /* [in] */ DWORD cpidl,
    /* [size_is][in] */ LPITEMIDLIST *rgpidl);


void __RPC_STUB IDynamicStorage_AddIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDynamicStorage_AddUnknown_Proxy( 
    IDynamicStorage * This,
    /* [in] */ REFIID riid,
    /* [in] */ IUnknown *pUnknown);


void __RPC_STUB IDynamicStorage_AddUnknown_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDynamicStorage_AddStorage_Proxy( 
    IDynamicStorage * This,
    /* [in] */ IStorage *pStorage);


void __RPC_STUB IDynamicStorage_AddStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDynamicStorage_AddStream_Proxy( 
    IDynamicStorage * This,
    /* [in] */ IStream *pStream);


void __RPC_STUB IDynamicStorage_AddStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDynamicStorage_AddDataObject_Proxy( 
    IDynamicStorage * This,
    /* [in] */ LPDATAOBJECT pdtobj);


void __RPC_STUB IDynamicStorage_AddDataObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDynamicStorage_INTERFACE_DEFINED__ */


#ifndef __IQueryContinue_INTERFACE_DEFINED__
#define __IQueryContinue_INTERFACE_DEFINED__

/* interface IQueryContinue */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IQueryContinue;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7307055c-b24a-486b-9f25-163e597a28a9")
    IQueryContinue : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryContinue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueryContinueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IQueryContinue * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IQueryContinue * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IQueryContinue * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryContinue )( 
            IQueryContinue * This);
        
        END_INTERFACE
    } IQueryContinueVtbl;

    interface IQueryContinue
    {
        CONST_VTBL struct IQueryContinueVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueryContinue_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueryContinue_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueryContinue_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueryContinue_QueryContinue(This)	\
    (This)->lpVtbl -> QueryContinue(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQueryContinue_QueryContinue_Proxy( 
    IQueryContinue * This);


void __RPC_STUB IQueryContinue_QueryContinue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQueryContinue_INTERFACE_DEFINED__ */


#ifndef __IUserNotification_INTERFACE_DEFINED__
#define __IUserNotification_INTERFACE_DEFINED__

/* interface IUserNotification */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IUserNotification;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70f9d15e-6e45-45e7-aaff-836f0343485f")
    IUserNotification : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetBalloonInfo( 
            /* [in] */ LPCWSTR pszTitle,
            /* [in] */ LPCWSTR pszText,
            /* [in] */ DWORD dwInfoFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBalloonRetry( 
            /* [in] */ DWORD dwShowTime,
            /* [in] */ DWORD dwInterval,
            /* [in] */ UINT cRetryCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconInfo( 
            /* [in] */ HICON hIcon,
            /* [in] */ LPCWSTR pszToolTip) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ IQueryContinue *pqc,
            /* [in] */ DWORD dwContinuePollInterval) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserNotificationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUserNotification * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUserNotification * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUserNotification * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetBalloonInfo )( 
            IUserNotification * This,
            /* [in] */ LPCWSTR pszTitle,
            /* [in] */ LPCWSTR pszText,
            /* [in] */ DWORD dwInfoFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetBalloonRetry )( 
            IUserNotification * This,
            /* [in] */ DWORD dwShowTime,
            /* [in] */ DWORD dwInterval,
            /* [in] */ UINT cRetryCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconInfo )( 
            IUserNotification * This,
            /* [in] */ HICON hIcon,
            /* [in] */ LPCWSTR pszToolTip);
        
        HRESULT ( STDMETHODCALLTYPE *Show )( 
            IUserNotification * This,
            /* [in] */ IQueryContinue *pqc,
            /* [in] */ DWORD dwContinuePollInterval);
        
        END_INTERFACE
    } IUserNotificationVtbl;

    interface IUserNotification
    {
        CONST_VTBL struct IUserNotificationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserNotification_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserNotification_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserNotification_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserNotification_SetBalloonInfo(This,pszTitle,pszText,dwInfoFlags)	\
    (This)->lpVtbl -> SetBalloonInfo(This,pszTitle,pszText,dwInfoFlags)

#define IUserNotification_SetBalloonRetry(This,dwShowTime,dwInterval,cRetryCount)	\
    (This)->lpVtbl -> SetBalloonRetry(This,dwShowTime,dwInterval,cRetryCount)

#define IUserNotification_SetIconInfo(This,hIcon,pszToolTip)	\
    (This)->lpVtbl -> SetIconInfo(This,hIcon,pszToolTip)

#define IUserNotification_Show(This,pqc,dwContinuePollInterval)	\
    (This)->lpVtbl -> Show(This,pqc,dwContinuePollInterval)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUserNotification_SetBalloonInfo_Proxy( 
    IUserNotification * This,
    /* [in] */ LPCWSTR pszTitle,
    /* [in] */ LPCWSTR pszText,
    /* [in] */ DWORD dwInfoFlags);


void __RPC_STUB IUserNotification_SetBalloonInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_SetBalloonRetry_Proxy( 
    IUserNotification * This,
    /* [in] */ DWORD dwShowTime,
    /* [in] */ DWORD dwInterval,
    /* [in] */ UINT cRetryCount);


void __RPC_STUB IUserNotification_SetBalloonRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_SetIconInfo_Proxy( 
    IUserNotification * This,
    /* [in] */ HICON hIcon,
    /* [in] */ LPCWSTR pszToolTip);


void __RPC_STUB IUserNotification_SetIconInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_Show_Proxy( 
    IUserNotification * This,
    /* [in] */ IQueryContinue *pqc,
    /* [in] */ DWORD dwContinuePollInterval);


void __RPC_STUB IUserNotification_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserNotification_INTERFACE_DEFINED__ */


#ifndef __IDockingWindow_INTERFACE_DEFINED__
#define __IDockingWindow_INTERFACE_DEFINED__

/* interface IDockingWindow */
/* [object][uuid] */ 


EXTERN_C const IID IID_IDockingWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("012dd920-7b26-11d0-8ca9-00a0c92dbfe8")
    IDockingWindow : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShowDW( 
            /* [in] */ BOOL fShow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseDW( 
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResizeBorderDW( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDockingWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDockingWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDockingWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDockingWindow * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IDockingWindow * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IDockingWindow * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *ShowDW )( 
            IDockingWindow * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *CloseDW )( 
            IDockingWindow * This,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ResizeBorderDW )( 
            IDockingWindow * This,
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved);
        
        END_INTERFACE
    } IDockingWindowVtbl;

    interface IDockingWindow
    {
        CONST_VTBL struct IDockingWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDockingWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDockingWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDockingWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDockingWindow_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IDockingWindow_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IDockingWindow_ShowDW(This,fShow)	\
    (This)->lpVtbl -> ShowDW(This,fShow)

#define IDockingWindow_CloseDW(This,dwReserved)	\
    (This)->lpVtbl -> CloseDW(This,dwReserved)

#define IDockingWindow_ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)	\
    (This)->lpVtbl -> ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDockingWindow_ShowDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ BOOL fShow);


void __RPC_STUB IDockingWindow_ShowDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDockingWindow_CloseDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IDockingWindow_CloseDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDockingWindow_ResizeBorderDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ IUnknown *punkToolbarSite,
    /* [in] */ BOOL fReserved);


void __RPC_STUB IDockingWindow_ResizeBorderDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDockingWindow_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0177 */
/* [local] */ 

#define DBIM_MINSIZE    0x0001
#define DBIM_MAXSIZE    0x0002
#define DBIM_INTEGRAL   0x0004
#define DBIM_ACTUAL     0x0008
#define DBIM_TITLE      0x0010
#define DBIM_MODEFLAGS  0x0020
#define DBIM_BKCOLOR    0x0040
#include <pshpack8.h>
typedef /* [public][public] */ struct __MIDL___MIDL_itf_shobjidl_0177_0001
    {
    DWORD dwMask;
    POINTL ptMinSize;
    POINTL ptMaxSize;
    POINTL ptIntegral;
    POINTL ptActual;
    WCHAR wszTitle[ 256 ];
    DWORD dwModeFlags;
    COLORREF crBkgnd;
    } 	DESKBANDINFO;

#include <poppack.h>
#define DBIMF_NORMAL            0x0000
#define DBIMF_FIXED             0x0001
#define DBIMF_FIXEDBMP          0x0004   // a fixed background bitmap (if supported)
#define DBIMF_VARIABLEHEIGHT    0x0008
#define DBIMF_UNDELETEABLE      0x0010
#define DBIMF_DEBOSSED          0x0020
#define DBIMF_BKCOLOR           0x0040
#define DBIMF_USECHEVRON        0x0080
#define DBIMF_BREAK             0x0100
#define DBIF_VIEWMODE_NORMAL         0x0000
#define DBIF_VIEWMODE_VERTICAL       0x0001
#define DBIF_VIEWMODE_FLOATING       0x0002
#define DBIF_VIEWMODE_TRANSPARENT    0x0004

enum __MIDL___MIDL_itf_shobjidl_0177_0002
    {	DBID_BANDINFOCHANGED	= 0,
	DBID_SHOWONLY	= 1,
	DBID_MAXIMIZEBAND	= 2,
	DBID_PUSHCHEVRON	= 3,
	DBID_DELAYINIT	= 4,
	DBID_FINISHINIT	= 5,
	DBID_SETWINDOWTHEME	= 6
    } ;
#define DBPC_SELECTFIRST    (DWORD)-1
#define DBPC_SELECTLAST     (DWORD)-2
#define CGID_DeskBand IID_IDeskBand


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0177_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0177_v0_0_s_ifspec;

#ifndef __IDeskBand_INTERFACE_DEFINED__
#define __IDeskBand_INTERFACE_DEFINED__

/* interface IDeskBand */
/* [object][uuid] */ 


EXTERN_C const IID IID_IDeskBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB0FE172-1A3A-11D0-89B3-00A0C90A90AC")
    IDeskBand : public IDockingWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBandInfo( 
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwViewMode,
            /* [out][in] */ DESKBANDINFO *pdbi) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeskBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeskBand * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeskBand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeskBand * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IDeskBand * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IDeskBand * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *ShowDW )( 
            IDeskBand * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *CloseDW )( 
            IDeskBand * This,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ResizeBorderDW )( 
            IDeskBand * This,
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved);
        
        HRESULT ( STDMETHODCALLTYPE *GetBandInfo )( 
            IDeskBand * This,
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwViewMode,
            /* [out][in] */ DESKBANDINFO *pdbi);
        
        END_INTERFACE
    } IDeskBandVtbl;

    interface IDeskBand
    {
        CONST_VTBL struct IDeskBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeskBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeskBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeskBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeskBand_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IDeskBand_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IDeskBand_ShowDW(This,fShow)	\
    (This)->lpVtbl -> ShowDW(This,fShow)

#define IDeskBand_CloseDW(This,dwReserved)	\
    (This)->lpVtbl -> CloseDW(This,dwReserved)

#define IDeskBand_ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)	\
    (This)->lpVtbl -> ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)


#define IDeskBand_GetBandInfo(This,dwBandID,dwViewMode,pdbi)	\
    (This)->lpVtbl -> GetBandInfo(This,dwBandID,dwViewMode,pdbi)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDeskBand_GetBandInfo_Proxy( 
    IDeskBand * This,
    /* [in] */ DWORD dwBandID,
    /* [in] */ DWORD dwViewMode,
    /* [out][in] */ DESKBANDINFO *pdbi);


void __RPC_STUB IDeskBand_GetBandInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeskBand_INTERFACE_DEFINED__ */



#ifndef __ShellObjects_LIBRARY_DEFINED__
#define __ShellObjects_LIBRARY_DEFINED__

/* library ShellObjects */
/* [version][lcid][helpstring][uuid] */ 


EXTERN_C const IID LIBID_ShellObjects;

EXTERN_C const CLSID CLSID_DriveSizeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("94357B53-CA29-4b78-83AE-E8FE7409134F")
DriveSizeCategorizer;
#endif

EXTERN_C const CLSID CLSID_DriveTypeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("B0A8F3CF-4333-4bab-8873-1CCB1CADA48B")
DriveTypeCategorizer;
#endif

EXTERN_C const CLSID CLSID_FreeSpaceCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("B5607793-24AC-44c7-82E2-831726AA6CB7")
FreeSpaceCategorizer;
#endif

EXTERN_C const CLSID CLSID_TimeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("3bb4118f-ddfd-4d30-a348-9fb5d6bf1afe")
TimeCategorizer;
#endif

EXTERN_C const CLSID CLSID_SizeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("55d7b852-f6d1-42f2-aa75-8728a1b2d264")
SizeCategorizer;
#endif

EXTERN_C const CLSID CLSID_AlphabeticalCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("3c2654c6-7372-4f6b-b310-55d6128f49d2")
AlphabeticalCategorizer;
#endif

EXTERN_C const CLSID CLSID_CompositeFolder;

#ifdef __cplusplus

class DECLSPEC_UUID("FEF10DED-355E-4e06-9381-9B24D7F7CC88")
CompositeFolder;
#endif

EXTERN_C const CLSID CLSID_LocalCopyHelper;

#ifdef __cplusplus

class DECLSPEC_UUID("021003e9-aac0-4975-979f-14b5d4e717f8")
LocalCopyHelper;
#endif

EXTERN_C const CLSID CLSID_ShellItem;

#ifdef __cplusplus

class DECLSPEC_UUID("9ac9fbe1-e0a2-4ad6-b4ee-e212013ea917")
ShellItem;
#endif

EXTERN_C const CLSID CLSID_ImageProperties;

#ifdef __cplusplus

class DECLSPEC_UUID("7ab770c7-0e23-4d7a-8aa2-19bfad479829")
ImageProperties;
#endif

EXTERN_C const CLSID CLSID_PropertiesUI;

#ifdef __cplusplus

class DECLSPEC_UUID("d912f8cf-0396-4915-884e-fb425d32943b")
PropertiesUI;
#endif

EXTERN_C const CLSID CLSID_UserNotification;

#ifdef __cplusplus

class DECLSPEC_UUID("0010890e-8789-413c-adbc-48f5b511b3af")
UserNotification;
#endif
#endif /* __ShellObjects_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_shobjidl_0178 */
/* [local] */ 


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0178_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0178_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HGLOBAL_UserSize(     unsigned long *, unsigned long            , HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserMarshal(  unsigned long *, unsigned char *, HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserUnmarshal(unsigned long *, unsigned char *, HGLOBAL * ); 
void                      __RPC_USER  HGLOBAL_UserFree(     unsigned long *, HGLOBAL * ); 

unsigned long             __RPC_USER  HICON_UserSize(     unsigned long *, unsigned long            , HICON * ); 
unsigned char * __RPC_USER  HICON_UserMarshal(  unsigned long *, unsigned char *, HICON * ); 
unsigned char * __RPC_USER  HICON_UserUnmarshal(unsigned long *, unsigned char *, HICON * ); 
void                      __RPC_USER  HICON_UserFree(     unsigned long *, HICON * ); 

unsigned long             __RPC_USER  HMENU_UserSize(     unsigned long *, unsigned long            , HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserMarshal(  unsigned long *, unsigned char *, HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserUnmarshal(unsigned long *, unsigned char *, HMENU * ); 
void                      __RPC_USER  HMENU_UserFree(     unsigned long *, HMENU * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  LPCITEMIDLIST_UserSize(     unsigned long *, unsigned long            , LPCITEMIDLIST * ); 
unsigned char * __RPC_USER  LPCITEMIDLIST_UserMarshal(  unsigned long *, unsigned char *, LPCITEMIDLIST * ); 
unsigned char * __RPC_USER  LPCITEMIDLIST_UserUnmarshal(unsigned long *, unsigned char *, LPCITEMIDLIST * ); 
void                      __RPC_USER  LPCITEMIDLIST_UserFree(     unsigned long *, LPCITEMIDLIST * ); 

unsigned long             __RPC_USER  LPITEMIDLIST_UserSize(     unsigned long *, unsigned long            , LPITEMIDLIST * ); 
unsigned char * __RPC_USER  LPITEMIDLIST_UserMarshal(  unsigned long *, unsigned char *, LPITEMIDLIST * ); 
unsigned char * __RPC_USER  LPITEMIDLIST_UserUnmarshal(unsigned long *, unsigned char *, LPITEMIDLIST * ); 
void                      __RPC_USER  LPITEMIDLIST_UserFree(     unsigned long *, LPITEMIDLIST * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



