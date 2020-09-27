/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    main.cpp

Abstract:

    Entry point and initialization routines for test harness

--*/

#include <xtl.h>

extern "C"
DWORD
WINAPI
HarnessEntryPoint(
    VOID
    );

void __cdecl main()
{
    //
    // Call main routine of the tets harness library in harnesslib.lib
    //

    HarnessEntryPoint();
}

//
// Tell the linker to include the following symbols so that XIU and XTU
// sections inside CRT will get built and startup/cleanup routines in
// xtestlib will get executed
//

#pragma comment( linker, "/include:_xtestlib_startup" )
#pragma comment( linker, "/include:_xtestlib_cleanup" )

//
// Tell the linker to include the following symbols so that "export table"
// of each test module will be placed and sorted in "export directory" inside
// the test harness
//

#pragma comment( linker, "/include:_memsize_ExportTableDirectory" )
#pragma comment( linker, "/include:_speedtest_ExportTableDirectory" )
#pragma comment( linker, "/include:_memtest_ExportTableDirectory" )
#pragma comment( linker, "/include:_disktest_ExportTableDirectory" )
#pragma comment( linker, "/include:_floatpt_ExportTableDirectory" )
#pragma comment( linker, "/include:_cpuid_ExportTableDirectory" )
#pragma comment( linker, "/include:_msr_ExportTableDirectory" )
#pragma comment( linker, "/include:_launcher_ExportTableDirectory" )

