//------------------------------------------------------------------------------
// XBox Online
//
// File: 		StdAfx.h
//
// Copyright:	Copyright (c) 2001 Microsoft Corporation.
//
// Contents:	include file for standard system include files,
//  			or project specific include files that are used frequently, but
//      		are changed infrequently
//
// History:	01/15/2001	JohnBlac	Created
//
//------------------------------------------------------------------------------

#if !defined(AFX_STDAFX_H__62E4D979_49B6_11D2_8A7B_00C04F8ECE95__INCLUDED_)
#define AFX_STDAFX_H__62E4D979_49B6_11D2_8A7B_00C04F8ECE95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#pragma warning(push)
#pragma warning(disable : 4995)

#pragma warning(pop)

#include <windows.h>
#include <xbox.h>
#include <winsockx.h>
#include <winsockp.h>
#include <xonlinep.h>

#include <stdlib.h>
#include <time.h>
#include <vlan.h>

#include <stdio.h>
#include <winsock2.h>
#include <oledb.h>		// Needed for SafeArrays and TLDB lib
#include "Iphlpapi.h"

#define NUMELEM(p1) (sizeof(p1) / sizeof(*p1))
#ifdef _DEBUG
#define ASSERT(x) if(!x){DebugBreak();}
#else
#define ASSERT(x) 1
#endif 

#define Assert(exp) _ASSERTE(exp)
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__62E4D979_49B6_11D2_8A7B_00C04F8ECE95__INCLUDED_)
