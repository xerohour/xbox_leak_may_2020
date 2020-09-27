/*++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Copyright (c) Microsoft Corporation

Module Name:

  modules.c

Abstract:

  Includes all netsync modules

Author:

  Steven Kehrli (steveke) 13-Nov-2000

------------------------------------------------------------------------------*/

// Have the linker include the following symbols so the export table of each module will be included in the export directory

#pragma comment( linker, "/include:_xnetbvt_xbox_NetsyncExportTableDirectory" )
#pragma comment( linker, "/include:_xnetapi_xbox_NetsyncExportTableDirectory" )
#pragma comment( linker, "/include:_xnetstress_xbox_NetsyncExportTableDirectory" )
#pragma comment( linker, "/include:_xboxkeys_xbox_NetsyncExportTableDirectory" )
