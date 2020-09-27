/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Ini.h

Abstract:

	abstract-for-module

Author:

	Jeff Sullivan (jeffsul) 11-Jul-2001

[Environment:]

	optional-environment-info (e.g. kernel mode only...)

[Notes:]

	optional-notes

Revision History:

	11-Jul-2001 jeffsul
		Initial Version

--*/

#ifndef __INI_H__
#define __INI_H__

#include <xtl.h>

FLOAT GetPrivateProfileFloat( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  FLOAT fDefault,       // return value if key name not found
  LPCTSTR lpFileName  // initialization file name
);

DWORD GetPrivateProfileConst( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  DWORD dwDefault,       // return value if key name not found
  LPCTSTR lpFileName  // initialization file name
);
	
#endif // #ifndef __INI_H__
