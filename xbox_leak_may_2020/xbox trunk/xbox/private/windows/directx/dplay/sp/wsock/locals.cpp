/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       Locals.cpp
 *  Content:	Global variables for the DNWsock service provider
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/25/98	jtk		Created
 ***************************************************************************/

#include "wsockspi.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_WSOCK

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//
// count of outstanding COM interfaces
//
volatile LONG	g_lOutstandingInterfaceCount = 0;

//
// invalid adapter guid
//
const GUID	g_InvalidAdapterGuid = { 0 };

//
// thread count
//
UINT_PTR	g_uThreadCount = 0;

//
// Winssock receive buffer size
//
BOOL	g_fWinsockReceiveBufferSizeOverridden = FALSE;
INT		g_iWinsockReceiveBufferSize = 0;

//
// Winsock receive buffer multiplier
//
DWORD_PTR		g_dwWinsockReceiveBufferMultiplier = 1;



//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************

