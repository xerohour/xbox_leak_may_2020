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
INT	GetIniInt( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  INT nDefault,       // return value if key name not found
  LPCTSTR lpFileName  // initialization file name
);

FLOAT GetIniFloat( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  FLOAT fDefault,       // return value if key name not found
  LPCTSTR lpFileName  // initialization file name
);

INT GetIniString( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  LPCTSTR lpDefault,       // return value if key name not found
  LPTSTR lpReturnedString,
  DWORD	nSize,
  LPCTSTR lpFileName  // initialization file name
);

DWORD GetIniConst( LPCTSTR lpAppName,  // section name
  LPCTSTR lpKeyName,  // key name
  DWORD dwDefault,       // return value if key name not found
  LPCTSTR lpFileName  // initialization file name
);
	
#endif // #ifndef __INI_H__
