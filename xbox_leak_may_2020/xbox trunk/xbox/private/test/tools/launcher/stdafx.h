// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__C5110465_9B32_48AE_884D_CB0151C70C0A__INCLUDED_)
#define AFX_STDAFX_H__C5110465_9B32_48AE_884D_CB0151C70C0A__INCLUDED_

#if _MSC_VER > 1000
    #pragma once
#endif // _MSC_VER > 1000

#ifndef WIN32_LEAN_AND_MEAN
    #define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

//#ifndef UNICODE
//    #define _UNICODE
//    #define UNICODE
//#endif

#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_MIPS_) && !defined(_X86_) && !defined(_IA64_) && defined(_M_IX86)
#define _X86_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_IA64_) && !defined(_MIPS_) && defined(_M_MRX000)
#define _MIPS_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_IA64_) && !defined(_MIPS_) && defined(_M_ALPHA)
#define _ALPHA_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_IA64_) && !defined(_MIPS_) && defined(_M_PPC)
#define _PPC_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_IA64_) && !defined(_MIPS_) && defined(_M_M68K)
#define _68K_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_X86_) && !defined(_IA64_) && !defined(_MIPS_) && defined(_M_MPPC)
#define _MPPC_
#endif
#if !defined(_68K_) && !defined(_MPPC_) && !defined(_PPC_) && !defined(_ALPHA_) && !defined(_MIPS_) && !defined(_X86_) && !defined(_M_IX86) && defined(_M_IA64)
#if !defined(_IA64_)
#define _IA64_
#endif // !_IA64_
#endif

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <ntexapi.h>

extern "C"
    {
    #include <ntos.h>
    }

#include <stdio.h>
#include <xtl.h>
#include <ldr.h>
#include <xbeimage.h>


extern "C"
    {
    ULONG DebugPrint(PCHAR Format, ...);
    typedef VOID (CALLBACK *PFORMAT_PROGRESS_ROUTINE)(UINT uPercent);

    BOOL WINAPI XapiFormatFATVolume(POBJECT_STRING pcVolume);
    NTSTATUS MU_CreateDeviceObject(ULONG Port, ULONG Slot, POBJECT_STRING DeviceName);
    VOID MU_CloseDeviceObject(ULONG Port, ULONG Slot);
    }


//
// items from <wbasenoxbox.h>
//
extern "C"
    {
    WINBASEAPI
    BOOL
    WINAPI
    TerminateThread(
        IN OUT HANDLE hThread,
        IN DWORD dwExitCode
        );
    }


//
// Cut from mu.h
//
#define MU_VSC_GET_BAD_BLOCK_TABLE   0
#define MU_VSC_MEMORY_TEST           1
#define MU_VSC_BAD_BLOCK_TABLE_SIZE (sizeof(USHORT)*16)
#define MU_VSC_BAD_BLOCK_COUNT_SIZE sizeof(USHORT)
#define MU_IOCTL_GET_BAD_BLOCK_TABLE\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_GET_BAD_BLOCK_TABLE, METHOD_NEITHER, FILE_ANY_ACCESS)
#define MU_IOCTL_MEMORY_TEST\
    CTL_CODE(FILE_DEVICE_DISK, MU_VSC_MEMORY_TEST, METHOD_NEITHER, FILE_ANY_ACCESS)


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__C5110465_9B32_48AE_884D_CB0151C70C0A__INCLUDED_)
