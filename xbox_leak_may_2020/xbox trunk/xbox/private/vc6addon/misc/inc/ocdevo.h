/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.59 */
/* at Tue Nov 26 10:32:11 1996
 */
/* Compiler settings for ocdevo.idl:
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

#ifndef __ocdevo_h__
#define __ocdevo_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IPropertyPageUndoString_FWD_DEFINED__
#define __IPropertyPageUndoString_FWD_DEFINED__
typedef interface IPropertyPageUndoString IPropertyPageUndoString;
#endif 	/* __IPropertyPageUndoString_FWD_DEFINED__ */


/* header files for imported files */
#include "oleidl.h"
#include "oaidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_ocdevo_0000
 * at Tue Nov 26 10:32:11 1996
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
// IStudio Specific IBuilderWizardManager Component Categories
// 
// {21448B91-0788-11d0-8144-00A0C91BBEE3}
DEFINE_GUID(CATID_NewFileBuilder, 
0x21448b91, 0x788, 0x11d0, 0x81, 0x44, 0x0, 0xa0, 0xc9, 0x1b, 0xbe, 0xe3);
// {21448B92-0788-11d0-8144-00A0C91BBEE3}
DEFINE_GUID(CATID_NewWebProjectBuilder, 
0x21448b92, 0x788, 0x11d0, 0x81, 0x44, 0x0, 0xa0, 0xc9, 0x1b, 0xbe, 0xe3);
// {21448B93-0788-11d0-8144-00A0C91BBEE3}
DEFINE_GUID(CATID_SQLTextBuilder, 
0x21448b93, 0x788, 0x11d0, 0x81, 0x44, 0x0, 0xa0, 0xc9, 0x1b, 0xbe, 0xe3);
// {E7879321-A77E-11cf-A406-00AA00C00940}
DEFINE_GUID(SID_SIntrinsicBuilderWizardManager,
0xe7879321, 0xa77e, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
// {73CEF3DD-AE85-11cf-A406-00AA00C00940}
DEFINE_GUID(CATID_HTMLDesignControl, 
0x73cef3dd, 0xae85, 0x11cf, 0xa4, 0x6, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);
///////////////////////////////////////////////////////////////////////////////
// IPropertyPageUndoString Interface
// 
// This interface is used by the daVinci component to provide it own
// Undo string for its property pages.  The shell, by default, provides
// a generic string for the undo record.  The interface allows the daVinci
// components to provide a more descriptive string for their undo transaction.
// {44AAE5F1-9FC6-11cf-A405-00AA00C00940}
DEFINE_GUID(IID_IPropertyPageUndoString,
0x44aae5f1, 0x9fc6, 0x11cf, 0xa4, 0x5, 0x0, 0xaa, 0x0, 0xc0, 0x9, 0x40);


extern RPC_IF_HANDLE __MIDL_itf_ocdevo_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_ocdevo_0000_v0_0_s_ifspec;

#ifndef __IPropertyPageUndoString_INTERFACE_DEFINED__
#define __IPropertyPageUndoString_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IPropertyPageUndoString
 * at Tue Nov 26 10:32:11 1996
 * using MIDL 3.01.59
 ****************************************/
/* [unique][uuid][object] */ 



EXTERN_C const IID IID_IPropertyPageUndoString;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("44AAE5F1-9FC6-11cf-A405-00AA00C00940")
    IPropertyPageUndoString : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetUndoString( 
            /* [out] */ LPOLESTR __RPC_FAR *ppszUndo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyPageUndoStringVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IPropertyPageUndoString __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IPropertyPageUndoString __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IPropertyPageUndoString __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetUndoString )( 
            IPropertyPageUndoString __RPC_FAR * This,
            /* [out] */ LPOLESTR __RPC_FAR *ppszUndo);
        
        END_INTERFACE
    } IPropertyPageUndoStringVtbl;

    interface IPropertyPageUndoString
    {
        CONST_VTBL struct IPropertyPageUndoStringVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyPageUndoString_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertyPageUndoString_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertyPageUndoString_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertyPageUndoString_GetUndoString(This,ppszUndo)	\
    (This)->lpVtbl -> GetUndoString(This,ppszUndo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPropertyPageUndoString_GetUndoString_Proxy( 
    IPropertyPageUndoString __RPC_FAR * This,
    /* [out] */ LPOLESTR __RPC_FAR *ppszUndo);


void __RPC_STUB IPropertyPageUndoString_GetUndoString_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertyPageUndoString_INTERFACE_DEFINED__ */


/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
