
#pragma warning( disable: 4049 )  /* more than 64k source lines */

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0347 */
/* at Sun May 05 19:52:16 2002
 */
/* Compiler settings for xbshlext.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __xbshlext_h__
#define __xbshlext_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __XboxFolder_FWD_DEFINED__
#define __XboxFolder_FWD_DEFINED__

#ifdef __cplusplus
typedef class XboxFolder XboxFolder;
#else
typedef struct XboxFolder XboxFolder;
#endif /* __cplusplus */

#endif 	/* __XboxFolder_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"
#include "shObjIdl.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 


#ifndef __SHELLEXTLib_LIBRARY_DEFINED__
#define __SHELLEXTLib_LIBRARY_DEFINED__

/* library SHELLEXTLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_SHELLEXTLib;

EXTERN_C const CLSID CLSID_XboxFolder;

#ifdef __cplusplus

class DECLSPEC_UUID("DB15FEDD-96B8-4DA9-97E0-7E5CCA05CC44")
XboxFolder;
#endif
#endif /* __SHELLEXTLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


