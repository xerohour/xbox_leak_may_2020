//
// Alpha (RISC) C/C++ AddOn
//
// Alpha (RISC) Platform C/C++ Tools Component Add-On 'package'
//
// [matthewt]
//

#pragma once

// platforms, tools, option handlers provided by this module

// add-on's name
#define szAddOnAlpha "Microsoft Alpha (RISC) C/C++ v1.0"

// our 'Alpha' build system components

#define BCID_Platform_Alpha			3		

#define BCID_OptHdlr_Compiler_Alpha	320
#define BCID_OptHdlr_Linker_Alpha	321

#define BCID_Tool_Compiler_Alpha	340
#define BCID_Tool_Linker_Alpha		341

#define BCID_ProjType_AlphaExe		360
#define BCID_ProjType_AlphaDll		361
#define BCID_ProjType_AlphaCon		362
#define BCID_ProjType_AlphaLib		363
