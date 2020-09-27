//=============================================================================
//  Microsoft (R) Network Monitor (tm). 
//  Copyright (C) 1991-1999. All rights reserved.
//
//  MODULE: NMRegHelp.h
//
//  Prototypes of Registry helper functions
//=============================================================================

#ifndef NMREGHELP_H
#define NMREGHELP_H


#ifdef __cplusplus
extern "C" {
#endif

// Registry helpers
LPCSTR _cdecl FindOneOf(LPCSTR p1, LPCSTR p2);

LONG _cdecl recursiveDeleteKey(HKEY hKeyParent,            // Parent of key to delete.
                        const char* lpszKeyChild);  // Key to delete.

BOOL _cdecl SubkeyExists(const char* pszPath,              // Path of key to check
                  const char* szSubkey);            // Key to check

BOOL _cdecl setKeyAndValue(const char* szKey, 
                    const char* szSubkey, 
                    const char* szValue,
                    const char* szName) ;

#ifdef __cplusplus
}
#endif

#endif // NMREGHELP_H