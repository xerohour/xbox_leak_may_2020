#ifndef __XBOXSYMS_H__
#define __XBOXSYMS_H__

// emmang@xbox

#define IMAGEBLD_IDC_CATEGORY		16112 /*VPROJ_IDC_MINI_PAGE*/	 	// Combo box
#define IMAGEBLD_IDC_DEFAULT		16074 /*VPROJ_IDC_SET_DEFAULT*/	// Button
#define IMAGEBLD_IDC_OPTIONS		16017 /*VPROJ_IDC_OPTSTR*/	 	// Multi line Edit box

// extracted from xboxdbg.pkg
// using resource hacker tool

#define IMAGEBLD_IDC_XBE_NAME       0x3D05  // Edit Box
#define IMAGEBLD_IDC_XBE_FLAGS      0x3D18  // Edit Box
#define IMAGEBLD_IDC_STACK_SIZE     0x3CFB  // Edit Box
#define IMAGEBLD_IDC_XBE_DEBUG      0x3CFC  // Check button
#define IMAGEBLD_IDC_AUTOCOPY_OFF   0x3D04  // Check button
#define IMAGEBLD_IDC_64MB_MEM       0x3D10  // Check button

#define IMAGEBLD_IDC_TITLE_ID       0x3CF7  // Edit Box
#define IMAGEBLD_IDC_TITLE_NAME     0x3CF8  // Edit Box
#define IMAGEBLD_IDC_PUBLISHER_NAME 0x3CF9  // Edit Box
#define IMAGEBLD_IDC_TITLE_IMAGE    0x3D12  // Edit Box
#define IMAGEBLD_IDC_TITLE_NAME2    0x3D14  // Edit Box
#define IMAGEBLD_IDC_DEFSAVE_IMAG   0x3D16  // Edit Box

// category indices for Xbox Image (ImageBld) tab
#define IMAGEBLD_GENERAL		1		
#define IMAGEBLD_TITLE			2

#endif
