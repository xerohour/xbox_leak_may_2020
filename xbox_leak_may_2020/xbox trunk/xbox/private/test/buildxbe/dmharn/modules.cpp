/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    modules.cpp

Abstract:

    List of all test modules

Environment:

    Xbox

Revision History:

--*/


//
// Tell the linker to include the following symbols so that "export table"
// of each test module will be placed and sorted in "export directory" inside
// the test harness
//

#pragma comment( linker, "/include:_DMTest1_ExportTableDirectory" )
