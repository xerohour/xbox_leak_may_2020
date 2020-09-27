/*++

  Copyright (c) 2000 Microsoft Corporation

  Module Name:

	dll.h

  Abstract:

	TBD

  Author:

	Toby Dziubala (a-tobydz) 25-April-2000

  Revision History:

--*/
#ifndef __DLL_H__
#define __DLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MEMDLL_API
#define MEMDLL_API
#endif

#define TRUE 1
#define FALSE 0


/// Function Prototypes ///
MEMDLL_API void XMmApiStartTest( HANDLE LogHandle );
MEMDLL_API void XMmApiEndTest( void );


#endif

#ifdef __cplusplus
}
#endif