//
// INTEL CONFIDENTIAL.  Copyright (c) 1997-1999 Intel Corp. All rights reserved.
//
#pragma once

LPDWORD AsmDwordMemCmp(LPVOID dst,LPVOID src,DWORD len);
LPWORD  AsmWordMemCmp (LPVOID dst,LPVOID src,DWORD len);
LPBYTE  AsmByteMemCmp (LPVOID dst,LPVOID src,DWORD len);
void    AsmDwordMemMov(LPVOID dst,LPVOID src,DWORD len);
void    AsmWordMemMov (LPVOID dst,LPVOID src,DWORD len);
void    AsmByteMemMov (LPVOID dst,LPVOID src,DWORD len);
