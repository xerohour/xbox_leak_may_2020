//
// Xbox C/C++ AddOn
//
// Xbox Platform C/C++ Tools Component Add-On 'package'
//
// [matthewt]
//

// platforms, tools, option handlers provided by this module

// add-on's name
// HEY! if you change this, change devblg.pkg:project.cpp as well
#define szAddOnXbox "Microsoft Xbox C/C++ v1.0"

// our 'Xbox' build system components

#define BCID_Platform_Xbox			5

#define BCID_OptHdlr_Compiler_Xbox	520
#define BCID_OptHdlr_Linker_XboxExe	521
#define BCID_OptHdlr_Linker_XboxDll	522
// HEY! if you change this, change devblg.pkg:project.cpp as well
#define BCID_OptHdlr_XbeBuilder     523
#define BCID_OptHdlr_Xbcp           524

#define BCID_Tool_Compiler_Xbox		540
#define BCID_Tool_Linker_XboxExe	541
#define BCID_Tool_Linker_XboxDll	542
#define BCID_Tool_XbeBuilder        543
#define BCID_Tool_Xbcp              544

#define BCID_ProjType_XboxExe		560
#define BCID_ProjType_XboxDll		561
#define BCID_ProjType_XboxCon		562
#define BCID_ProjType_XboxLib		563
#define BCID_ProjType_XboxGeneric	564
