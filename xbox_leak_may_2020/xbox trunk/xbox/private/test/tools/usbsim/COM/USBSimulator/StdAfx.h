/*****************************************************************************

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    stdafx.h

Abstract:

    Precomiled Header

Author:

    Josh Poley (jpoley)

Revision History:

*****************************************************************************/

#ifndef _USBSIM_PHC_
#define _USBSIM_PHC_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define STRICT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#define _ATL_APARTMENT_THREADED

#include <atlbase.h>
extern CComModule _Module;
#include <atlcom.h>

#include "COMUSBSimulator.h"
#include "..\..\inc\usbsimulator.h"
#include "AnsiBstr.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // _USBSIM_PHC_
