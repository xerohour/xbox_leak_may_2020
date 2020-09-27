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

#pragma comment( linker, "/include:_dsAPI_ExportTableDirectory" )
#pragma comment( linker, "/include:_dsBVT_ExportTableDirectory" )
#pragma comment( linker, "/include:_ds3D_ExportTableDirectory"  )
#pragma comment( linker, "/include:_ds3DStrs_ExportTableDirectory"  )
#pragma comment( linker, "/include:_mixer_ExportTableDirectory" )
#pragma comment( linker, "/include:_dsReg_ExportTableDirectory" )
#pragma comment( linker, "/include:_DMTest1_ExportTableDirectory" )
#pragma comment( linker, "/include:_xmoBVT_ExportTableDirectory" )
#pragma comment( linker, "/include:_xmoReg_ExportTableDirectory" )
#pragma comment( linker, "/include:_WMABVT_ExportTableDirectory" )
#pragma comment( linker, "/include:_WMAReg_ExportTableDirectory" )
#pragma comment( linker, "/include:_DirectSoundCreate_ExportTableDirectory" )
#pragma comment( linker, "/include:_DirectSoundCreateBuffer_ExportTableDirectory" )
#pragma comment( linker, "/include:_DirectSoundCreateStream_ExportTableDirectory" )
