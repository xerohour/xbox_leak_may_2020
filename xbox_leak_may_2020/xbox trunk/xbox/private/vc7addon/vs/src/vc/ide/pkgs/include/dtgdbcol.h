/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.59 */
/* at Wed Dec 11 12:17:41 1996
 */
/* Compiler settings for dtgdbcol.idl:
    Os (OptLev=s), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"

#ifndef __dtgdbcol_h__
#define __dtgdbcol_h__

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


#ifndef __IDBConnection_FWD_DEFINED__
#define __IDBConnection_FWD_DEFINED__
typedef interface IDBConnection IDBConnection;
#endif 	/* __IDBConnection_FWD_DEFINED__ */


#ifndef __IDBConnections_FWD_DEFINED__
#define __IDBConnections_FWD_DEFINED__
typedef interface IDBConnections IDBConnections;
#endif 	/* __IDBConnections_FWD_DEFINED__ */


#ifndef __IDataProject_FWD_DEFINED__
#define __IDataProject_FWD_DEFINED__
typedef interface IDataProject IDataProject;
#endif 	/* __IDataProject_FWD_DEFINED__ */


#ifndef __IDispDataProject_FWD_DEFINED__
#define __IDispDataProject_FWD_DEFINED__
typedef interface IDispDataProject IDispDataProject;
#endif 	/* __IDispDataProject_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_dtgdbcol_0000
 * at Wed Dec 11 12:17:41 1996
 * using MIDL 3.01.59
 ****************************************/
/* [local] */ 


#ifndef _OLECTL_H_
#include <olectl.h>
#endif
#include <ObjModel\AppDefs.h>
#include <ObjModel\AppAuto.h>


extern RPC_IF_HANDLE __MIDL_itf_dtgdbcol_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_dtgdbcol_0000_v0_0_s_ifspec;


#ifndef __DSDataProjects_LIBRARY_DEFINED__
#define __DSDataProjects_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: DSDataProjects
 * at Wed Dec 11 12:17:41 1996
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

EXTERN_C const IID LIBID_DSDataProjects;

#ifndef __IGenericDocument_INTERFACE_DEFINED__
#define __IGenericDocument_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IGenericDocument
 * at Wed Dec 11 12:17:41 1996
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
 * at Wed Dec 11 12:17:41 1996
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
 * at Wed Dec 11 12:17:41 1996
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


#ifndef __IDBConnection_INTERFACE_DEFINED__
#define __IDBConnection_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IDBConnection
 * at Wed Dec 11 12:17:41 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IDBConnection;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("F7CCDBA0-AF0C-11cf-97EA-00AA00C006B6")
    IDBConnection : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Name( 
            /* [retval][out] */ BSTR __RPC_FAR *pName) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_Name( 
            /* [in] */ BSTR Name) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pParent) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectString( 
            /* [retval][out] */ BSTR __RPC_FAR *pConnectString) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectString( 
            /* [in] */ BSTR ConnectString) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ConnectionTimeout( 
            /* [retval][out] */ long __RPC_FAR *pTimeout) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ConnectionTimeout( 
            /* [in] */ long Timeout) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_CommandTimeout( 
            /* [retval][out] */ long __RPC_FAR *pTimeout) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_CommandTimeout( 
            /* [in] */ long Timeout) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Runtime( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pRuntime) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Connected( 
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pValid) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDBConnectionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDBConnection __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDBConnection __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDBConnection __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pName);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_Name )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ BSTR Name);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pParent);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectString )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pConnectString);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectString )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ BSTR ConnectString);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ConnectionTimeout )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pTimeout);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ConnectionTimeout )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ long Timeout);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_CommandTimeout )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pTimeout);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_CommandTimeout )( 
            IDBConnection __RPC_FAR * This,
            /* [in] */ long Timeout);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Runtime )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pRuntime);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Connected )( 
            IDBConnection __RPC_FAR * This,
            /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pValid);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Connect )( 
            IDBConnection __RPC_FAR * This);
        
        END_INTERFACE
    } IDBConnectionVtbl;

    interface IDBConnection
    {
        CONST_VTBL struct IDBConnectionVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDBConnection_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDBConnection_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDBConnection_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDBConnection_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDBConnection_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDBConnection_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDBConnection_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDBConnection_get_Name(This,pName)	\
    (This)->lpVtbl -> get_Name(This,pName)

#define IDBConnection_put_Name(This,Name)	\
    (This)->lpVtbl -> put_Name(This,Name)

#define IDBConnection_get_Parent(This,pParent)	\
    (This)->lpVtbl -> get_Parent(This,pParent)

#define IDBConnection_get_ConnectString(This,pConnectString)	\
    (This)->lpVtbl -> get_ConnectString(This,pConnectString)

#define IDBConnection_put_ConnectString(This,ConnectString)	\
    (This)->lpVtbl -> put_ConnectString(This,ConnectString)

#define IDBConnection_get_ConnectionTimeout(This,pTimeout)	\
    (This)->lpVtbl -> get_ConnectionTimeout(This,pTimeout)

#define IDBConnection_put_ConnectionTimeout(This,Timeout)	\
    (This)->lpVtbl -> put_ConnectionTimeout(This,Timeout)

#define IDBConnection_get_CommandTimeout(This,pTimeout)	\
    (This)->lpVtbl -> get_CommandTimeout(This,pTimeout)

#define IDBConnection_put_CommandTimeout(This,Timeout)	\
    (This)->lpVtbl -> put_CommandTimeout(This,Timeout)

#define IDBConnection_get_Runtime(This,pRuntime)	\
    (This)->lpVtbl -> get_Runtime(This,pRuntime)

#define IDBConnection_get_Connected(This,pValid)	\
    (This)->lpVtbl -> get_Connected(This,pValid)

#define IDBConnection_Connect(This)	\
    (This)->lpVtbl -> Connect(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_Name_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pName);


void __RPC_STUB IDBConnection_get_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDBConnection_put_Name_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [in] */ BSTR Name);


void __RPC_STUB IDBConnection_put_Name_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_Parent_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pParent);


void __RPC_STUB IDBConnection_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_ConnectString_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ BSTR __RPC_FAR *pConnectString);


void __RPC_STUB IDBConnection_get_ConnectString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDBConnection_put_ConnectString_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [in] */ BSTR ConnectString);


void __RPC_STUB IDBConnection_put_ConnectString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_ConnectionTimeout_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pTimeout);


void __RPC_STUB IDBConnection_get_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDBConnection_put_ConnectionTimeout_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [in] */ long Timeout);


void __RPC_STUB IDBConnection_put_ConnectionTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_CommandTimeout_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pTimeout);


void __RPC_STUB IDBConnection_get_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IDBConnection_put_CommandTimeout_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [in] */ long Timeout);


void __RPC_STUB IDBConnection_put_CommandTimeout_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_Runtime_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pRuntime);


void __RPC_STUB IDBConnection_get_Runtime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnection_get_Connected_Proxy( 
    IDBConnection __RPC_FAR * This,
    /* [retval][out] */ VARIANT_BOOL __RPC_FAR *pValid);


void __RPC_STUB IDBConnection_get_Connected_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDBConnection_Connect_Proxy( 
    IDBConnection __RPC_FAR * This);


void __RPC_STUB IDBConnection_Connect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDBConnection_INTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_DBConnection;

class DECLSPEC_UUID("F7CCDBA2-AF0C-11cf-97EA-00AA00C006B6")
DBConnection;
#endif

#ifndef __IDBConnections_INTERFACE_DEFINED__
#define __IDBConnections_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IDBConnections
 * at Wed Dec 11 12:17:41 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IDBConnections;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("79937DA0-AA6D-11cf-97EA-00AA00C006B6")
    IDBConnections : public IDispatch
    {
    public:
        virtual /* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE get__NewEnum( 
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *_ppNewEnum) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Count( 
            /* [retval][out] */ long __RPC_FAR *pCount) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_Parent( 
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pWebProject) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Add( 
            /* [in] */ BSTR Name,
            /* [in] */ BSTR ConnectString,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Remove( 
            /* [in] */ VARIANT Index) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SaveConnections( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDBConnectionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDBConnections __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDBConnections __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDBConnections __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][restricted][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get__NewEnum )( 
            IDBConnections __RPC_FAR * This,
            /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *_ppNewEnum);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Item )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ VARIANT Index,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Count )( 
            IDBConnections __RPC_FAR * This,
            /* [retval][out] */ long __RPC_FAR *pCount);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IDBConnections __RPC_FAR * This,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pWebProject);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Add )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ BSTR Name,
            /* [in] */ BSTR ConnectString,
            /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Remove )( 
            IDBConnections __RPC_FAR * This,
            /* [in] */ VARIANT Index);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveConnections )( 
            IDBConnections __RPC_FAR * This);
        
        END_INTERFACE
    } IDBConnectionsVtbl;

    interface IDBConnections
    {
        CONST_VTBL struct IDBConnectionsVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDBConnections_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDBConnections_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDBConnections_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDBConnections_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDBConnections_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDBConnections_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDBConnections_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDBConnections_get__NewEnum(This,_ppNewEnum)	\
    (This)->lpVtbl -> get__NewEnum(This,_ppNewEnum)

#define IDBConnections_Item(This,Index,pDBConnection)	\
    (This)->lpVtbl -> Item(This,Index,pDBConnection)

#define IDBConnections_get_Count(This,pCount)	\
    (This)->lpVtbl -> get_Count(This,pCount)

#define IDBConnections_get_Parent(This,pWebProject)	\
    (This)->lpVtbl -> get_Parent(This,pWebProject)

#define IDBConnections_Add(This,Name,ConnectString,pDBConnection)	\
    (This)->lpVtbl -> Add(This,Name,ConnectString,pDBConnection)

#define IDBConnections_Remove(This,Index)	\
    (This)->lpVtbl -> Remove(This,Index)

#define IDBConnections_SaveConnections(This)	\
    (This)->lpVtbl -> SaveConnections(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][restricted][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnections_get__NewEnum_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [retval][out] */ IUnknown __RPC_FAR *__RPC_FAR *_ppNewEnum);


void __RPC_STUB IDBConnections_get__NewEnum_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDBConnections_Item_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [in] */ VARIANT Index,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection);


void __RPC_STUB IDBConnections_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnections_get_Count_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [retval][out] */ long __RPC_FAR *pCount);


void __RPC_STUB IDBConnections_get_Count_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDBConnections_get_Parent_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pWebProject);


void __RPC_STUB IDBConnections_get_Parent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDBConnections_Add_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [in] */ BSTR Name,
    /* [in] */ BSTR ConnectString,
    /* [retval][out] */ LPDISPATCH __RPC_FAR *pDBConnection);


void __RPC_STUB IDBConnections_Add_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDBConnections_Remove_Proxy( 
    IDBConnections __RPC_FAR * This,
    /* [in] */ VARIANT Index);


void __RPC_STUB IDBConnections_Remove_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id] */ HRESULT STDMETHODCALLTYPE IDBConnections_SaveConnections_Proxy( 
    IDBConnections __RPC_FAR * This);


void __RPC_STUB IDBConnections_SaveConnections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDBConnections_INTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_DBConnections;

class DECLSPEC_UUID("79937DA2-AA6D-11cf-97EA-00AA00C006B6")
DBConnections;
#endif

#ifndef __IDataProject_INTERFACE_DEFINED__
#define __IDataProject_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IDataProject
 * at Wed Dec 11 12:17:41 1996
 * using MIDL 3.01.59
 ****************************************/
/* [object][hidden][dual][oleautomation][uuid] */ 



EXTERN_C const IID IID_IDataProject;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("1AEE2F43-A42B-11cf-97EA-00AA00C006B6")
    IDataProject : public IGenericProject
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_DBConnections( 
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *IDispatch) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDataProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDataProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDataProject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDataProject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDataProject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDataProject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDataProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDataProject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Name )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_FullName )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *Name);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Application )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Application);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Parent )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *Parent);
        
        /* [propget][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_Type )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ BSTR __RPC_FAR *pType);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved1 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved2 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved3 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved4 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved5 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved6 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved7 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved8 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved9 )( 
            IDataProject __RPC_FAR * This);
        
        /* [restricted][hidden][id] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Reserved10 )( 
            IDataProject __RPC_FAR * This);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_DBConnections )( 
            IDataProject __RPC_FAR * This,
            /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *IDispatch);
        
        END_INTERFACE
    } IDataProjectVtbl;

    interface IDataProject
    {
        CONST_VTBL struct IDataProjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDataProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDataProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDataProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDataProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDataProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDataProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDataProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IDataProject_get_Name(This,Name)	\
    (This)->lpVtbl -> get_Name(This,Name)

#define IDataProject_get_FullName(This,Name)	\
    (This)->lpVtbl -> get_FullName(This,Name)

#define IDataProject_get_Application(This,Application)	\
    (This)->lpVtbl -> get_Application(This,Application)

#define IDataProject_get_Parent(This,Parent)	\
    (This)->lpVtbl -> get_Parent(This,Parent)

#define IDataProject_get_Type(This,pType)	\
    (This)->lpVtbl -> get_Type(This,pType)

#define IDataProject_Reserved1(This)	\
    (This)->lpVtbl -> Reserved1(This)

#define IDataProject_Reserved2(This)	\
    (This)->lpVtbl -> Reserved2(This)

#define IDataProject_Reserved3(This)	\
    (This)->lpVtbl -> Reserved3(This)

#define IDataProject_Reserved4(This)	\
    (This)->lpVtbl -> Reserved4(This)

#define IDataProject_Reserved5(This)	\
    (This)->lpVtbl -> Reserved5(This)

#define IDataProject_Reserved6(This)	\
    (This)->lpVtbl -> Reserved6(This)

#define IDataProject_Reserved7(This)	\
    (This)->lpVtbl -> Reserved7(This)

#define IDataProject_Reserved8(This)	\
    (This)->lpVtbl -> Reserved8(This)

#define IDataProject_Reserved9(This)	\
    (This)->lpVtbl -> Reserved9(This)

#define IDataProject_Reserved10(This)	\
    (This)->lpVtbl -> Reserved10(This)


#define IDataProject_get_DBConnections(This,IDispatch)	\
    (This)->lpVtbl -> get_DBConnections(This,IDispatch)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IDataProject_get_DBConnections_Proxy( 
    IDataProject __RPC_FAR * This,
    /* [retval][out] */ IDispatch __RPC_FAR *__RPC_FAR *IDispatch);


void __RPC_STUB IDataProject_get_DBConnections_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDataProject_INTERFACE_DEFINED__ */


#ifndef __IDispDataProject_DISPINTERFACE_DEFINED__
#define __IDispDataProject_DISPINTERFACE_DEFINED__

/****************************************
 * Generated header for dispinterface: IDispDataProject
 * at Wed Dec 11 12:17:41 1996
 * using MIDL 3.01.59
 ****************************************/
/* [hidden][uuid] */ 



EXTERN_C const IID DIID_IDispDataProject;

#if defined(__cplusplus) && !defined(CINTERFACE)

    interface DECLSPEC_UUID("1c39e7e1-a9b7-11cf-97ea-00aa00c006b6")
    IDispDataProject : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IDispDataProjectVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IDispDataProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IDispDataProject __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IDispDataProject __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IDispDataProject __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IDispDataProject __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IDispDataProject __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IDispDataProject __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        END_INTERFACE
    } IDispDataProjectVtbl;

    interface IDispDataProject
    {
        CONST_VTBL struct IDispDataProjectVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDispDataProject_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDispDataProject_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDispDataProject_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDispDataProject_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IDispDataProject_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IDispDataProject_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IDispDataProject_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IDispDataProject_DISPINTERFACE_DEFINED__ */


#ifdef __cplusplus
EXTERN_C const CLSID CLSID_DataProject;

class DECLSPEC_UUID("E50D4940-A9BD-11cf-97EA-00AA00C006B6")
DataProject;
#endif
#endif /* __DSDataProjects_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
