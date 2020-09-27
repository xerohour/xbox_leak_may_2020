/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.59 */
/* at Wed Dec 11 10:58:12 1996
 */
/* Compiler settings for htmlpkg.idl:
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

#ifndef __htmlpkg_h__
#define __htmlpkg_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IGenericDocument_FWD_DEFINED__
#define __IGenericDocument_FWD_DEFINED__
typedef interface IGenericDocument IGenericDocument;
#endif 	/* __IGenericDocument_FWD_DEFINED__ */


#ifndef __IGenericWindow_FWD_DEFINED__
#define __IGenericWindow_FWD_DEFINED__
typedef interface IGenericWindow IGenericWindow;
#endif 	/* __IGenericWindow_FWD_DEFINED__ */


#ifndef __IGenericProject_FWD_DEFINED__
#define __IGenericProject_FWD_DEFINED__
typedef interface IGenericProject IGenericProject;
#endif 	/* __IGenericProject_FWD_DEFINED__ */


#ifndef __IHtmlLayoutDocument_FWD_DEFINED__
#define __IHtmlLayoutDocument_FWD_DEFINED__
typedef interface IHtmlLayoutDocument IHtmlLayoutDocument;
#endif 	/* __IHtmlLayoutDocument_FWD_DEFINED__ */


#ifndef __IHtmlDocument_FWD_DEFINED__
#define __IHtmlDocument_FWD_DEFINED__
typedef interface IHtmlDocument IHtmlDocument;
#endif 	/* __IHtmlDocument_FWD_DEFINED__ */


#ifndef __IHtmlSourceEditorSupport_FWD_DEFINED__
#define __IHtmlSourceEditorSupport_FWD_DEFINED__
typedef interface IHtmlSourceEditorSupport IHtmlSourceEditorSupport;
#endif 	/* __IHtmlSourceEditorSupport_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_htmlpkg_0000
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1993 - 1996.
//
//--------------------------------------------------------------------------
#ifndef _OLECTL_H_
#include <olectl.h>
#endif
#include <ObjModel\AppDefs.h>
#include <ObjModel\AppAuto.h>


extern RPC_IF_HANDLE __MIDL_itf_htmlpkg_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_htmlpkg_0000_v0_0_s_ifspec;


#ifndef __DSHTMLEditor_LIBRARY_DEFINED__
#define __DSHTMLEditor_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: DSHTMLEditor
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [version][lcid][helpstring][uuid] */ 


#if 0
typedef /* [hidden] */ 
enum tagDSWINDOWSTATE
    {	dsWindowStateMaximized	= 1,
	dsWindowStateMinimized	= 2,
	dsWindowStateNormal	= 3
    }	DsWindowState;

typedef /* [hidden] */ 
enum tagDSARRANGESTYLE
    {	dsMinimize	= 1,
	dsTileHorizontal	= 2,
	dsTileVertical	= 3,
	dsCascade	= 4
    }	DsArrangeStyle;

typedef /* [hidden] */ 
enum tagDSBUTTONTYPE
    {	dsGlyph	= 1,
	dsText	= 2
    }	DsButtonType;

typedef /* [hidden] */ 
enum tagDSSAVECHANGES
    {	dsSaveChangesYes	= 1,
	dsSaveChangesNo	= 2,
	dsSaveChangesPrompt	= 3
    }	DsSaveChanges;

typedef /* [hidden] */ 
enum tagDSSAVESTATUS
    {	dsSaveSucceeded	= 1,
	dsSaveCanceled	= 2
    }	DsSaveStatus;

#endif // 0

EXTERN_C const IID LIBID_DSHTMLEditor;

#ifndef __IGenericDocument_INTERFACE_DEFINED__
#define __IGenericDocument_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGenericDocument
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][dual][hidden][oleautomation][uuid] */ 



EXTERN_C const IID IID_IGenericDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("FB7FDAE1-89B8-11cf-9BE8-00A0C90A632C")
    IGenericDocument : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppApplication) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppParent) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Path( 
            /* [retval][out] */ BSTR __RPC_FAR *pPath) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Saved( 
            /* [retval][out] */ boolean __RPC_FAR *pSaved) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ActiveWindow( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_ReadOnly( 
            /* [retval][out] */ boolean __RPC_FAR *pReadOnly) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_ReadOnly( 
            /* [in] */ boolean ReadOnly) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pType) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Windows( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindows) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Active( 
            /* [in] */ boolean bActive) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ boolean __RPC_FAR *pbActive) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE NewWindow( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Save( 
            /* [optional][in] */ VARIANT vFilename,
            /* [optional][in] */ VARIANT vBoolPrompt,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Undo( 
            /* [retval][out] */ boolean __RPC_FAR *pSuccess) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Redo( 
            /* [retval][out] */ boolean __RPC_FAR *pSuccess) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE PrintOut( 
            /* [retval][out] */ boolean __RPC_FAR *pSuccess) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [optional][in] */ VARIANT vSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved1( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved2( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved3( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved4( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved5( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved6( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved7( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved8( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved9( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved10( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGenericDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGenericDocument __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGenericDocument __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGenericDocument __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppApplication);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppParent);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pPath);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Saved )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSaved);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveWindow )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnly )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pReadOnly);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReadOnly )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ boolean ReadOnly);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pType);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Windows )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindows);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Active )( 
            IGenericDocument __RPC_FAR * This,
            /* [in] */ boolean bActive);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pbActive);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NewWindow )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IGenericDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vFilename,
            /* [optional][in] */ VARIANT vBoolPrompt,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Undo )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Redo )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrintOut )( 
            IGenericDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IGenericDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IGenericDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IGenericDocument __RPC_FAR * This);
        
        END_INTERFACE
    } IGenericDocumentVtbl;

    interface IGenericDocument
    {
        CONST_VTBL struct IGenericDocumentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGenericDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGenericDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGenericDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGenericDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGenericDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGenericDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGenericDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGenericDocument_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IGenericDocument_get_FullName(This,pName)	\
    (This)->lpVtbl -> get_FullName(This,pName)

#define IGenericDocument_get_Application(This,ppApplication)	\
    (This)->lpVtbl -> get_Application(This,ppApplication)

#define IGenericDocument_get_Parent(This,ppParent)	\
    (This)->lpVtbl -> get_Parent(This,ppParent)

#define IGenericDocument_get_Path(This,pPath)	\
    (This)->lpVtbl -> get_Path(This,pPath)

#define IGenericDocument_get_Saved(This,pSaved)	\
    (This)->lpVtbl -> get_Saved(This,pSaved)

#define IGenericDocument_get_ActiveWindow(This,ppWindow)	\
    (This)->lpVtbl -> get_ActiveWindow(This,ppWindow)

#define IGenericDocument_get_ReadOnly(This,pReadOnly)	\
    (This)->lpVtbl -> get_ReadOnly(This,pReadOnly)

#define IGenericDocument_put_ReadOnly(This,ReadOnly)	\
    (This)->lpVtbl -> put_ReadOnly(This,ReadOnly)

#define IGenericDocument_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IGenericDocument_get_Windows(This,ppWindows)	\
    (This)->lpVtbl -> get_Windows(This,ppWindows)

#define IGenericDocument_put_Active(This,bActive)	\
    (This)->lpVtbl -> put_Active(This,bActive)

#define IGenericDocument_get_Active(This,pbActive)	\
    (This)->lpVtbl -> get_Active(This,pbActive)

#define IGenericDocument_NewWindow(This,ppWindow)	\
    (This)->lpVtbl -> NewWindow(This,ppWindow)

#define IGenericDocument_Save(This,vFilename,vBoolPrompt,pSaved)	\
    (This)->lpVtbl -> Save(This,vFilename,vBoolPrompt,pSaved)

#define IGenericDocument_Undo(This,pSuccess)	\
    (This)->lpVtbl -> Undo(This,pSuccess)

#define IGenericDocument_Redo(This,pSuccess)	\
    (This)->lpVtbl -> Redo(This,pSuccess)

#define IGenericDocument_PrintOut(This,pSuccess)	\
    (This)->lpVtbl -> PrintOut(This,pSuccess)

#define IGenericDocument_Close(This,vSaveChanges,pSaved)	\
    (This)->lpVtbl -> Close(This,vSaveChanges,pSaved)

#define IGenericDocument_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IGenericDocument_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IGenericDocument_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IGenericDocument_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IGenericDocument_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IGenericDocument_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IGenericDocument_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IGenericDocument_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IGenericDocument_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IGenericDocument_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Name_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IGenericDocument_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_FullName_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IGenericDocument_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Application_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppApplication);


void __RPC_STUB IGenericDocument_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Parent_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppParent);


void __RPC_STUB IGenericDocument_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Path_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pPath);


void __RPC_STUB IGenericDocument_get_Path_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Saved_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pSaved);


void __RPC_STUB IGenericDocument_get_Saved_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_ActiveWindow_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);


void __RPC_STUB IGenericDocument_get_ActiveWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_ReadOnly_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pReadOnly);


void __RPC_STUB IGenericDocument_get_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericDocument_put_ReadOnly_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [in] */ boolean ReadOnly);


void __RPC_STUB IGenericDocument_put_ReadOnly_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Type_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pType);


void __RPC_STUB IGenericDocument_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Windows_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindows);


void __RPC_STUB IGenericDocument_get_Windows_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericDocument_put_Active_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [in] */ boolean bActive);


void __RPC_STUB IGenericDocument_put_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericDocument_get_Active_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pbActive);


void __RPC_STUB IGenericDocument_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_NewWindow_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);


void __RPC_STUB IGenericDocument_NewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Save_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [optional][in] */ VARIANT vFilename,
    /* [optional][in] */ VARIANT vBoolPrompt,
    /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);


void __RPC_STUB IGenericDocument_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Undo_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pSuccess);


void __RPC_STUB IGenericDocument_Undo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Redo_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pSuccess);


void __RPC_STUB IGenericDocument_Redo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_PrintOut_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pSuccess);


void __RPC_STUB IGenericDocument_PrintOut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Close_Proxy( 
    IGenericDocument __RPC_FAR * This,
    /* [optional][in] */ VARIANT vSaveChanges,
    /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);


void __RPC_STUB IGenericDocument_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved1_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved2_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved3_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved4_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved5_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved6_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved7_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved8_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved8_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved9_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericDocument_Reserved10_Proxy( 
    IGenericDocument __RPC_FAR * This);


void __RPC_STUB IGenericDocument_Reserved10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGenericDocument_INTERFACE_DEFINED__ */


#ifndef __IGenericWindow_INTERFACE_DEFINED__
#define __IGenericWindow_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGenericWindow
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][dual][hidden][oleautomation][uuid] */ 



EXTERN_C const IID IID_IGenericWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("FD20FC80-A9D2-11cf-9C13-00A0C90A632C")
    IGenericWindow : public IDispatch
    {
    public:
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Caption( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Active( 
            /* [in] */ boolean bActive) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Active( 
            /* [retval][out] */ boolean __RPC_FAR *pbActive) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Left( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Left( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Top( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Top( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Height( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Height( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_Width( 
            /* [in] */ long lVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Width( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Index( 
            /* [retval][out] */ long __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Next( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Previous( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
        virtual /* [id][propput] */ HRESULT STDMETHODCALLTYPE put_WindowState( 
            /* [in] */ DsWindowState lVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_WindowState( 
            /* [retval][out] */ DsWindowState __RPC_FAR *plVal) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
        virtual /* [id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch) = 0;
        
        virtual /* [id] */ HRESULT STDMETHODCALLTYPE Close( 
            /* [in] */ VARIANT boolSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved1( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved2( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved3( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved4( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved5( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved6( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved7( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved8( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved9( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved10( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGenericWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGenericWindow __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGenericWindow __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGenericWindow __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Caption )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Active )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ boolean bActive);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pbActive);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Left )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Left )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Top )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Top )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Height )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Height )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Width )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ long lVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Width )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Index )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *plVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Next )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Previous )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_WindowState )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ DsWindowState lVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_WindowState )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ DsWindowState __RPC_FAR *plVal);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IGenericWindow __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IGenericWindow __RPC_FAR * This,
            /* [in] */ VARIANT boolSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IGenericWindow __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IGenericWindow __RPC_FAR * This);
        
        END_INTERFACE
    } IGenericWindowVtbl;

    interface IGenericWindow
    {
        CONST_VTBL struct IGenericWindowVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGenericWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGenericWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGenericWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGenericWindow_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGenericWindow_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGenericWindow_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGenericWindow_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGenericWindow_get_Caption(This,pbstrCaption)	\
    (This)->lpVtbl -> get_Caption(This,pbstrCaption)

#define IGenericWindow_get_Type(This,pbstrCaption)	\
    (This)->lpVtbl -> get_Type(This,pbstrCaption)

#define IGenericWindow_put_Active(This,bActive)	\
    (This)->lpVtbl -> put_Active(This,bActive)

#define IGenericWindow_get_Active(This,pbActive)	\
    (This)->lpVtbl -> get_Active(This,pbActive)

#define IGenericWindow_put_Left(This,lVal)	\
    (This)->lpVtbl -> put_Left(This,lVal)

#define IGenericWindow_get_Left(This,plVal)	\
    (This)->lpVtbl -> get_Left(This,plVal)

#define IGenericWindow_put_Top(This,lVal)	\
    (This)->lpVtbl -> put_Top(This,lVal)

#define IGenericWindow_get_Top(This,plVal)	\
    (This)->lpVtbl -> get_Top(This,plVal)

#define IGenericWindow_put_Height(This,lVal)	\
    (This)->lpVtbl -> put_Height(This,lVal)

#define IGenericWindow_get_Height(This,plVal)	\
    (This)->lpVtbl -> get_Height(This,plVal)

#define IGenericWindow_put_Width(This,lVal)	\
    (This)->lpVtbl -> put_Width(This,lVal)

#define IGenericWindow_get_Width(This,plVal)	\
    (This)->lpVtbl -> get_Width(This,plVal)

#define IGenericWindow_get_Index(This,plVal)	\
    (This)->lpVtbl -> get_Index(This,plVal)

#define IGenericWindow_get_Next(This,ppDispatch)	\
    (This)->lpVtbl -> get_Next(This,ppDispatch)

#define IGenericWindow_get_Previous(This,ppDispatch)	\
    (This)->lpVtbl -> get_Previous(This,ppDispatch)

#define IGenericWindow_put_WindowState(This,lVal)	\
    (This)->lpVtbl -> put_WindowState(This,lVal)

#define IGenericWindow_get_WindowState(This,plVal)	\
    (This)->lpVtbl -> get_WindowState(This,plVal)

#define IGenericWindow_get_Application(This,ppDispatch)	\
    (This)->lpVtbl -> get_Application(This,ppDispatch)

#define IGenericWindow_get_Parent(This,ppDispatch)	\
    (This)->lpVtbl -> get_Parent(This,ppDispatch)

#define IGenericWindow_Close(This,boolSaveChanges,pSaved)	\
    (This)->lpVtbl -> Close(This,boolSaveChanges,pSaved)

#define IGenericWindow_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IGenericWindow_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IGenericWindow_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IGenericWindow_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IGenericWindow_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IGenericWindow_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IGenericWindow_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IGenericWindow_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IGenericWindow_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IGenericWindow_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Caption_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption);


void __RPC_STUB IGenericWindow_get_Caption_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Type_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pbstrCaption);


void __RPC_STUB IGenericWindow_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_Active_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ boolean bActive);


void __RPC_STUB IGenericWindow_put_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Active_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ boolean __RPC_FAR *pbActive);


void __RPC_STUB IGenericWindow_get_Active_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_Left_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IGenericWindow_put_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Left_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_Left_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_Top_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IGenericWindow_put_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Top_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_Top_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_Height_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IGenericWindow_put_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Height_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_Height_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_Width_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ long lVal);


void __RPC_STUB IGenericWindow_put_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Width_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_Width_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Index_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_Index_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Next_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IGenericWindow_get_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Previous_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IGenericWindow_get_Previous_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propput] */ HRESULT STDMETHODCALLTYPE IGenericWindow_put_WindowState_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ DsWindowState lVal);


void __RPC_STUB IGenericWindow_put_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_WindowState_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ DsWindowState __RPC_FAR *plVal);


void __RPC_STUB IGenericWindow_get_WindowState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Application_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IGenericWindow_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id][propget] */ HRESULT STDMETHODCALLTYPE IGenericWindow_get_Parent_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);


void __RPC_STUB IGenericWindow_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Close_Proxy( 
    IGenericWindow __RPC_FAR * This,
    /* [in] */ VARIANT boolSaveChanges,
    /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);


void __RPC_STUB IGenericWindow_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved1_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved2_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved3_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved4_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved5_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved6_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved7_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved8_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved8_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved9_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericWindow_Reserved10_Proxy( 
    IGenericWindow __RPC_FAR * This);


void __RPC_STUB IGenericWindow_Reserved10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGenericWindow_INTERFACE_DEFINED__ */


#ifndef __IGenericProject_INTERFACE_DEFINED__
#define __IGenericProject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGenericProject
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][dual][hidden][oleautomation][uuid] */ 



EXTERN_C const IID IID_IGenericProject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("8CA5A960-FC7D-11cf-927D-00A0C9138C45")
    IGenericProject : public IDispatch
    {
    public:
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_FullName( 
            /* [retval][out] */ BSTR __RPC_FAR *Name) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Application( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent) = 0;
        
        virtual /* [propget][id] */ HRESULT STDMETHODCALLTYPE get_Type( 
            /* [retval][out] */ BSTR __RPC_FAR *pType) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved1( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved2( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved3( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved4( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved5( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved6( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved7( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved8( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved9( void) = 0;
        
        virtual /* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE Reserved10( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IGenericProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IGenericProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IGenericProject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IGenericProject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IGenericProject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IGenericProject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IGenericProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IGenericProject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IGenericProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IGenericProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IGenericProject __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IGenericProject __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IGenericProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pType);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IGenericProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IGenericProject __RPC_FAR * This);
        
        END_INTERFACE
    } IGenericProjectVtbl;

    interface IGenericProject
    {
        CONST_VTBL struct IGenericProjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IGenericProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IGenericProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IGenericProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IGenericProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IGenericProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IGenericProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IGenericProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IGenericProject_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IGenericProject_get_FullName(This,Name)	\
    (This)->lpVtbl -> get_FullName(This,Name)

#define IGenericProject_get_Application(This,Application)	\
    (This)->lpVtbl -> get_Application(This,Application)

#define IGenericProject_get_Parent(This,Parent)	\
    (This)->lpVtbl -> get_Parent(This,Parent)

#define IGenericProject_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IGenericProject_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IGenericProject_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IGenericProject_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IGenericProject_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IGenericProject_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IGenericProject_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IGenericProject_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IGenericProject_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IGenericProject_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IGenericProject_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [propget][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_get_Name_Proxy( 
    IGenericProject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB IGenericProject_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_get_FullName_Proxy( 
    IGenericProject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *Name);


void __RPC_STUB IGenericProject_get_FullName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_get_Application_Proxy( 
    IGenericProject __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application);


void __RPC_STUB IGenericProject_get_Application_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_get_Parent_Proxy( 
    IGenericProject __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent);


void __RPC_STUB IGenericProject_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [propget][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_get_Type_Proxy( 
    IGenericProject __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pType);


void __RPC_STUB IGenericProject_get_Type_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved1_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved1_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved2_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved3_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved3_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved4_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved4_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved5_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved5_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved6_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved6_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved7_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved7_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved8_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved8_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved9_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved9_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [restricted][hidden][id] */ HRESULT STDMETHODCALLTYPE IGenericProject_Reserved10_Proxy( 
    IGenericProject __RPC_FAR * This);


void __RPC_STUB IGenericProject_Reserved10_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IGenericProject_INTERFACE_DEFINED__ */


#ifndef __IHtmlLayoutDocument_INTERFACE_DEFINED__
#define __IHtmlLayoutDocument_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHtmlLayoutDocument
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IHtmlLayoutDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("E6899811-0C72-11d0-8BF1-00A0C90F55D6")
    IHtmlLayoutDocument : public IGenericDocument
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Contents( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHtmlLayoutDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppApplication);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppParent);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pPath);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Saved )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSaved);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveWindow )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnly )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pReadOnly);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReadOnly )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ boolean ReadOnly);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pType);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Windows )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindows);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Active )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [in] */ boolean bActive);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pbActive);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NewWindow )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vFilename,
            /* [optional][in] */ VARIANT vBoolPrompt,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Undo )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Redo )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrintOut )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IHtmlLayoutDocument __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Contents )( 
            IHtmlLayoutDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents);
        
        END_INTERFACE
    } IHtmlLayoutDocumentVtbl;

    interface IHtmlLayoutDocument
    {
        CONST_VTBL struct IHtmlLayoutDocumentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHtmlLayoutDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHtmlLayoutDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHtmlLayoutDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHtmlLayoutDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHtmlLayoutDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHtmlLayoutDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHtmlLayoutDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHtmlLayoutDocument_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IHtmlLayoutDocument_get_FullName(This,pName)	\
    (This)->lpVtbl -> get_FullName(This,pName)

#define IHtmlLayoutDocument_get_Application(This,ppApplication)	\
    (This)->lpVtbl -> get_Application(This,ppApplication)

#define IHtmlLayoutDocument_get_Parent(This,ppParent)	\
    (This)->lpVtbl -> get_Parent(This,ppParent)

#define IHtmlLayoutDocument_get_Path(This,pPath)	\
    (This)->lpVtbl -> get_Path(This,pPath)

#define IHtmlLayoutDocument_get_Saved(This,pSaved)	\
    (This)->lpVtbl -> get_Saved(This,pSaved)

#define IHtmlLayoutDocument_get_ActiveWindow(This,ppWindow)	\
    (This)->lpVtbl -> get_ActiveWindow(This,ppWindow)

#define IHtmlLayoutDocument_get_ReadOnly(This,pReadOnly)	\
    (This)->lpVtbl -> get_ReadOnly(This,pReadOnly)

#define IHtmlLayoutDocument_put_ReadOnly(This,ReadOnly)	\
    (This)->lpVtbl -> put_ReadOnly(This,ReadOnly)

#define IHtmlLayoutDocument_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IHtmlLayoutDocument_get_Windows(This,ppWindows)	\
    (This)->lpVtbl -> get_Windows(This,ppWindows)

#define IHtmlLayoutDocument_put_Active(This,bActive)	\
    (This)->lpVtbl -> put_Active(This,bActive)

#define IHtmlLayoutDocument_get_Active(This,pbActive)	\
    (This)->lpVtbl -> get_Active(This,pbActive)

#define IHtmlLayoutDocument_NewWindow(This,ppWindow)	\
    (This)->lpVtbl -> NewWindow(This,ppWindow)

#define IHtmlLayoutDocument_Save(This,vFilename,vBoolPrompt,pSaved)	\
    (This)->lpVtbl -> Save(This,vFilename,vBoolPrompt,pSaved)

#define IHtmlLayoutDocument_Undo(This,pSuccess)	\
    (This)->lpVtbl -> Undo(This,pSuccess)

#define IHtmlLayoutDocument_Redo(This,pSuccess)	\
    (This)->lpVtbl -> Redo(This,pSuccess)

#define IHtmlLayoutDocument_PrintOut(This,pSuccess)	\
    (This)->lpVtbl -> PrintOut(This,pSuccess)

#define IHtmlLayoutDocument_Close(This,vSaveChanges,pSaved)	\
    (This)->lpVtbl -> Close(This,vSaveChanges,pSaved)

#define IHtmlLayoutDocument_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IHtmlLayoutDocument_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IHtmlLayoutDocument_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IHtmlLayoutDocument_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IHtmlLayoutDocument_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IHtmlLayoutDocument_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IHtmlLayoutDocument_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IHtmlLayoutDocument_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IHtmlLayoutDocument_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IHtmlLayoutDocument_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)


#define IHtmlLayoutDocument_get_Contents(This,ppContents)	\
    (This)->lpVtbl -> get_Contents(This,ppContents)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHtmlLayoutDocument_get_Contents_Proxy( 
    IHtmlLayoutDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents);


void __RPC_STUB IHtmlLayoutDocument_get_Contents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHtmlLayoutDocument_INTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_HtmlLayoutDocument;

class DECLSPEC_UUID("E6899813-0C72-11d0-8BF1-00A0C90F55D6")
HtmlLayoutDocument;
#endif

#ifndef __IHtmlDocument_INTERFACE_DEFINED__
#define __IHtmlDocument_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHtmlDocument
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IHtmlDocument;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C92A2284-E3DD-11cf-A96F-00A0C90F55D6")
    IHtmlDocument : public IGenericDocument
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Contents( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHtmlDocumentVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHtmlDocument __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHtmlDocument __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IHtmlDocument __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppApplication);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppParent);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Path )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pPath);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Saved )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSaved);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ActiveWindow )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ReadOnly )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pReadOnly);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ReadOnly )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ boolean ReadOnly);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pType);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Windows )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindows);
        
        /* [id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Active )( 
            IHtmlDocument __RPC_FAR * This,
            /* [in] */ boolean bActive);
        
        /* [id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Active )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pbActive);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NewWindow )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppWindow);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            IHtmlDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vFilename,
            /* [optional][in] */ VARIANT vBoolPrompt,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Undo )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Redo )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrintOut )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ boolean __RPC_FAR *pSuccess);
        
        /* [id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            IHtmlDocument __RPC_FAR * This,
            /* [optional][in] */ VARIANT vSaveChanges,
            /* [retval][out] */ DsSaveStatus __RPC_FAR *pSaved);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IHtmlDocument __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Contents )( 
            IHtmlDocument __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents);
        
        END_INTERFACE
    } IHtmlDocumentVtbl;

    interface IHtmlDocument
    {
        CONST_VTBL struct IHtmlDocumentVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHtmlDocument_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHtmlDocument_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHtmlDocument_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHtmlDocument_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IHtmlDocument_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IHtmlDocument_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IHtmlDocument_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IHtmlDocument_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IHtmlDocument_get_FullName(This,pName)	\
    (This)->lpVtbl -> get_FullName(This,pName)

#define IHtmlDocument_get_Application(This,ppApplication)	\
    (This)->lpVtbl -> get_Application(This,ppApplication)

#define IHtmlDocument_get_Parent(This,ppParent)	\
    (This)->lpVtbl -> get_Parent(This,ppParent)

#define IHtmlDocument_get_Path(This,pPath)	\
    (This)->lpVtbl -> get_Path(This,pPath)

#define IHtmlDocument_get_Saved(This,pSaved)	\
    (This)->lpVtbl -> get_Saved(This,pSaved)

#define IHtmlDocument_get_ActiveWindow(This,ppWindow)	\
    (This)->lpVtbl -> get_ActiveWindow(This,ppWindow)

#define IHtmlDocument_get_ReadOnly(This,pReadOnly)	\
    (This)->lpVtbl -> get_ReadOnly(This,pReadOnly)

#define IHtmlDocument_put_ReadOnly(This,ReadOnly)	\
    (This)->lpVtbl -> put_ReadOnly(This,ReadOnly)

#define IHtmlDocument_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IHtmlDocument_get_Windows(This,ppWindows)	\
    (This)->lpVtbl -> get_Windows(This,ppWindows)

#define IHtmlDocument_put_Active(This,bActive)	\
    (This)->lpVtbl -> put_Active(This,bActive)

#define IHtmlDocument_get_Active(This,pbActive)	\
    (This)->lpVtbl -> get_Active(This,pbActive)

#define IHtmlDocument_NewWindow(This,ppWindow)	\
    (This)->lpVtbl -> NewWindow(This,ppWindow)

#define IHtmlDocument_Save(This,vFilename,vBoolPrompt,pSaved)	\
    (This)->lpVtbl -> Save(This,vFilename,vBoolPrompt,pSaved)

#define IHtmlDocument_Undo(This,pSuccess)	\
    (This)->lpVtbl -> Undo(This,pSuccess)

#define IHtmlDocument_Redo(This,pSuccess)	\
    (This)->lpVtbl -> Redo(This,pSuccess)

#define IHtmlDocument_PrintOut(This,pSuccess)	\
    (This)->lpVtbl -> PrintOut(This,pSuccess)

#define IHtmlDocument_Close(This,vSaveChanges,pSaved)	\
    (This)->lpVtbl -> Close(This,vSaveChanges,pSaved)

#define IHtmlDocument_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IHtmlDocument_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IHtmlDocument_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IHtmlDocument_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IHtmlDocument_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IHtmlDocument_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IHtmlDocument_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IHtmlDocument_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IHtmlDocument_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IHtmlDocument_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)


#define IHtmlDocument_get_Contents(This,ppContents)	\
    (This)->lpVtbl -> get_Contents(This,ppContents)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IHtmlDocument_get_Contents_Proxy( 
    IHtmlDocument __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *ppContents);


void __RPC_STUB IHtmlDocument_get_Contents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHtmlDocument_INTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_HtmlDocument;

class DECLSPEC_UUID("C92A2286-E3DD-11cf-A96F-00A0C90F55D6")
HtmlDocument;
#endif
#endif /* __DSHTMLEditor_LIBRARY_DEFINED__ */

/****************************************
 * Generated header for interface: __MIDL_itf_htmlpkg_0101
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


///////////////////////////////////////////////////////////////////////////////
// IHtmlSourceEditorSupport Interface
// 
//
//	HRESULT GetPopDesc
//
//	Description:
//		Returns POPDESC for context menu.  Parses the text document for 
//		current state and returns appropriate POPDESC.  If GetPopDesc returns
//		S_OK, then all interfaces are held onto.  These interfaces will
//		be released in InvokeCommand.  You must always call InvokeCommand
//		if GetPopDesc succeeds.
//			
//	Arguments:
//		IUnknown* pSrcEdit: IN - ISourceEdit* for HTML document
//
//		IUnknown* pTextDoc: IN - ITextDocument for HTML document
//
//		IUnknown* pTextSel: IN - ITextSelection for HTML document
//
//		BYTE** ppPopDesc:  OUT - POPDESC for context menu to be displayed
//
//	Return (HRESULT): S_OK on success, error on failure
//
//
//
//
//	HRESULT InvokeCommand
//
//	Description:
//		Handles the Context menu command.  Releases the interfaces passed to
//		GetPopDesc.  This must be called ONLY if GetPopDesc succeeds.  Returns
//		S_OK if it handles the command.  If an error is returned then the 
//		command belongs to some other component.  In this case the command
//		should be routed in the appropriate manner.
//
//	Arguments:
//		int iCmdID: IN - Menu Command ID
//
//		HWND hwndOwner: HWND for HTML document
//
//	Return (HRESULT): S_OK if handles the command, ERROR otherwise.
//						
//
//
//	HRESULT HandleContextmenuUpdateCommand
//
//	Description:
//		OnUpdate handler for all context menu commands.  Takes the menu command
//		id and enables it depending on the results of parsing the HTML document.
//
//	Arguments:
//		CCmdUI* pCmdUI - IN : CCMDUI for menu item
//
//	Return (HRESULT): S_OK if valid command, E_NOTIMPL otherwise
//
//
//
//	HRESULT HandleCommand
//
//	Description:
//		Handles the top level menu command.  If it cant successfully handle the
//		command it will display an error message, and return an error.
//
//	Arguments:
//		int nID: IN - Menu Command ID
//
//		IUnknown* pSrcEdit: IN - ISourceEdit* for HTML document
//
//		IUnknown* pTextDoc: IN - ITextDocument for HTML document
//
//		IUnknown* pTextSel: IN - ITextSelection for HTML document
//
//		HWND hwndOwner: HWND for HTML document
//
//	Return (HRESULT): S_OK if successfully handled the command, error otherwise.
//


extern RPC_IF_HANDLE __MIDL_itf_htmlpkg_0101_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_htmlpkg_0101_v0_0_s_ifspec;

#ifndef __IHtmlSourceEditorSupport_INTERFACE_DEFINED__
#define __IHtmlSourceEditorSupport_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IHtmlSourceEditorSupport
 * at Wed Dec 11 10:58:12 1996
 * using MIDL 3.01.59
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IHtmlSourceEditorSupport;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("E95101F2-BA66-11cf-A40E-00AA00C00940")
    IHtmlSourceEditorSupport : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPopDesc( 
            /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
            /* [in] */ IUnknown __RPC_FAR *pTextDoc,
            /* [in] */ IUnknown __RPC_FAR *pTextSel,
            /* [out] */ byte __RPC_FAR *__RPC_FAR *ppPopDesc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InvokeCommand( 
            /* [in] */ int nCmdID,
            /* [in] */ HWND hwndOwner) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleContextmenuUpdateCommand( 
            /* [in] */ byte __RPC_FAR *pCmdUI) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleCommand( 
            /* [in] */ int nID,
            /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
            /* [in] */ IUnknown __RPC_FAR *pTextDoc,
            /* [in] */ IUnknown __RPC_FAR *pTextSel,
            /* [in] */ HWND hwndOwner) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHtmlSourceEditorSupportVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IHtmlSourceEditorSupport __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IHtmlSourceEditorSupport __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IHtmlSourceEditorSupport __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetPopDesc )( 
            IHtmlSourceEditorSupport __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
            /* [in] */ IUnknown __RPC_FAR *pTextDoc,
            /* [in] */ IUnknown __RPC_FAR *pTextSel,
            /* [out] */ byte __RPC_FAR *__RPC_FAR *ppPopDesc);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *InvokeCommand )( 
            IHtmlSourceEditorSupport __RPC_FAR * This,
            /* [in] */ int nCmdID,
            /* [in] */ HWND hwndOwner);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleContextmenuUpdateCommand )( 
            IHtmlSourceEditorSupport __RPC_FAR * This,
            /* [in] */ byte __RPC_FAR *pCmdUI);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HandleCommand )( 
            IHtmlSourceEditorSupport __RPC_FAR * This,
            /* [in] */ int nID,
            /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
            /* [in] */ IUnknown __RPC_FAR *pTextDoc,
            /* [in] */ IUnknown __RPC_FAR *pTextSel,
            /* [in] */ HWND hwndOwner);
        
        END_INTERFACE
    } IHtmlSourceEditorSupportVtbl;

    interface IHtmlSourceEditorSupport
    {
        CONST_VTBL struct IHtmlSourceEditorSupportVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHtmlSourceEditorSupport_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHtmlSourceEditorSupport_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHtmlSourceEditorSupport_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHtmlSourceEditorSupport_GetPopDesc(This,pSrcEdit,pTextDoc,pTextSel,ppPopDesc)	\
    (This)->lpVtbl -> GetPopDesc(This,pSrcEdit,pTextDoc,pTextSel,ppPopDesc)

#define IHtmlSourceEditorSupport_InvokeCommand(This,nCmdID,hwndOwner)	\
    (This)->lpVtbl -> InvokeCommand(This,nCmdID,hwndOwner)

#define IHtmlSourceEditorSupport_HandleContextmenuUpdateCommand(This,pCmdUI)	\
    (This)->lpVtbl -> HandleContextmenuUpdateCommand(This,pCmdUI)

#define IHtmlSourceEditorSupport_HandleCommand(This,nID,pSrcEdit,pTextDoc,pTextSel,hwndOwner)	\
    (This)->lpVtbl -> HandleCommand(This,nID,pSrcEdit,pTextDoc,pTextSel,hwndOwner)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHtmlSourceEditorSupport_GetPopDesc_Proxy( 
    IHtmlSourceEditorSupport __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
    /* [in] */ IUnknown __RPC_FAR *pTextDoc,
    /* [in] */ IUnknown __RPC_FAR *pTextSel,
    /* [out] */ byte __RPC_FAR *__RPC_FAR *ppPopDesc);


void __RPC_STUB IHtmlSourceEditorSupport_GetPopDesc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHtmlSourceEditorSupport_InvokeCommand_Proxy( 
    IHtmlSourceEditorSupport __RPC_FAR * This,
    /* [in] */ int nCmdID,
    /* [in] */ HWND hwndOwner);


void __RPC_STUB IHtmlSourceEditorSupport_InvokeCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHtmlSourceEditorSupport_HandleContextmenuUpdateCommand_Proxy( 
    IHtmlSourceEditorSupport __RPC_FAR * This,
    /* [in] */ byte __RPC_FAR *pCmdUI);


void __RPC_STUB IHtmlSourceEditorSupport_HandleContextmenuUpdateCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHtmlSourceEditorSupport_HandleCommand_Proxy( 
    IHtmlSourceEditorSupport __RPC_FAR * This,
    /* [in] */ int nID,
    /* [in] */ IUnknown __RPC_FAR *pSrcEdit,
    /* [in] */ IUnknown __RPC_FAR *pTextDoc,
    /* [in] */ IUnknown __RPC_FAR *pTextSel,
    /* [in] */ HWND hwndOwner);


void __RPC_STUB IHtmlSourceEditorSupport_HandleCommand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHtmlSourceEditorSupport_INTERFACE_DEFINED__ */


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
