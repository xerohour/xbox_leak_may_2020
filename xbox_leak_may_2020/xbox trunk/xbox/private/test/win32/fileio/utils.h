/*

Copyright (C) Microsoft Corporation.  All rights reserved.

Module Name:

    utils.h

Abstract:

    This module contains the helper functions used by the API tests

Author:

    Josh Poley (jpoley)

Environment:

    XBox

Revision History:
    04-21-2000  Created

Notes:

*/
#ifndef _UTILS_H_
#define _UTILS_H_

#include "stdafx.h"
#include <xlog.h>

namespace FILEIO
    {
    extern bool FileExists(const char *filename);
    extern DWORD FillFile(HANDLE hFile, DWORD size);
    extern DWORD CreateFilledFile(const char *filename, DWORD size=16384);
    extern HANDLE FileOpen(const char *filename);
    extern HANDLE FileOpenRW(const char *filename);
    extern BOOL CreateFileTime(LPFILETIME fileTime, WORD year, WORD month, WORD day, WORD hour, WORD min, WORD sec, WORD msec);
    extern DWORD FillHDPartition(const char *drive, char *lpFile);
    extern void LogResourceStatus(HANDLE hLog, bool debugger);

    class iLARGE_INTEGER
        {
        public: 
            __int64 value;

        public:
            iLARGE_INTEGER() { value = 0; }
            iLARGE_INTEGER(int i) { value = i; }
            iLARGE_INTEGER(DWORD i) { value = i; }
            iLARGE_INTEGER(__int64 i) { value = i; }
            iLARGE_INTEGER(const LARGE_INTEGER &i) { value = i.QuadPart; }

            operator int() {  return (int)value; }
            operator DWORD() {  return (DWORD)value; }
            operator __int64() {  return value; }
            operator LARGE_INTEGER() {  LARGE_INTEGER a; a.QuadPart=value; return a; }
        };

    } // namespace FILEIO
#endif // _UTILS_H_