///////////////////////////////////////////////////////////////////////////////
//      DOCKIDS.H
//              contains the hard coded IDs for the standard dock windows that will
//              registered by our packages.  Some day we may generate these more
//              dynamically.  Windows based off of IDDW_BASE should be of type dtEdit,
//              all others show up in the Options/Toolbars dialog and are hence
//              considered toolbars in terms of ordering and grouping in the UI.

#ifndef __DOCKIDS_H__
#define __DOCKIDS_H__


// Added by MarcI, taken from sym\shlsrvc.h
#define IDDW_SHELL_BASE         0x7000
#define IDDW_SHELL_MAX			(IDDW_SHELL_BASE + 0x00FF)
#define IDDW_PACKAGE_BASE		(IDDW_SHELL_MAX + 1)
// End addition by MarcI

#define IDDW_BASE               (IDDW_SHELL_BASE)

#define IDDW_SUSHI_BASE         (IDDW_BASE + 0x0000)
#define IDDW_VRES_BASE          (IDDW_BASE + 0x0100)
#define IDDW_VCPP_BASE          (IDDW_BASE + 0x0200)
#define IDDW_VPROJ_BASE         (IDDW_BASE + 0x0300)
#define IDDW_PROJECT_BASE       (IDDW_BASE + 0x0310)
#define IDDW_MSIN_BASE          (IDDW_BASE + 0x0320) // PLEASE LEAVE 30 IDS for MSIN
#define IDDW_MSTEST_BASE                (IDDW_BASE + 0x0350)
#define IDDW_ESPRESSO_BASE              (IDDW_BASE + 0x0358)
#define IDDW_IV_BASE			(IDDW_PACKAGE_BASE + 0x0240)

#define IDTB_BASE               (IDDW_BASE + 0x0400)
#define IDTB_SUSHI_BASE         (IDTB_BASE + 0x0000)
#define IDTB_VRES_BASE          (IDTB_BASE + 0x0100)
#define IDTB_VCPP_BASE          (IDTB_BASE + 0x0200)
#define IDTB_VPROJ_BASE         (IDTB_BASE + 0x0300)
#define IDTB_PROJECT_BASE       (IDTB_BASE + 0x0310)
#define IDTB_MSTEST_BASE                (IDTB_BASE + 0x0320)
#define IDTB_ESPRESSO_BASE              (IDTB_BASE + 0x0328)
#define IDTB_MSIN_BASE          (IDTB_BASE + 0x0340)
#define IDTB_CUSTOM_BASE        (IDTB_BASE + 0x0400)
#define IDTB_PACKAGE_BASE       (IDDW_BASE + 0x0500)

///////////////////////////////////////////////////////////////////////////////
//      SUSHI docked windows.

// Offsets from IDTB_SUSHI_BASE
#define IDTB_SUSHI_STANDARD (IDTB_SUSHI_BASE+1)
#define IDTB_SUSHI_EDIT     (IDTB_SUSHI_BASE+2)
#define IDTB_SUSHI_FULLSCREEN (IDTB_SUSHI_BASE+3)

///////////////////////////////////////////////////////////////////////////////
//      VRES docked windows.

// Offsets from IDTB_VRES_BASE
#define IDTB_VRES_RESOURCE (IDTB_VRES_BASE+3)
#define IDTB_VRES_DIALOG   (IDTB_VRES_BASE+4)
#define IDDW_CONTROLS      (IDTB_VRES_BASE+5)   // Show up in toolbar dialog.
#define IDDW_GRAPHICS      (IDTB_VRES_BASE+6)
#define IDDW_COLORS        (IDTB_VRES_BASE+7)
#define IDDW_TEXTTOOL      (IDTB_VRES_BASE+8)

///////////////////////////////////////////////////////////////////////////////
//      VPROJ docked windows.

// Offsets from IDTB_VPROJ_BASE
#define IDTB_VPROJ_BUILD (IDTB_VPROJ_BASE+0)
#define IDTB_SOURCE_CONTROL     (IDTB_VPROJ_BASE+1)

///////////////////////////////////////////////////////////////////////////////
//      VCPP docked windows.

// Offsets from IDDW_VCPP_BASE
#define IDDW_VCPP_ERROR_WIN      (IDDW_VCPP_BASE+0)
#define IDDW_VCPP_WATCH_WIN      (IDDW_VCPP_BASE+1)
#define IDDW_VCPP_VARIABLES_WIN  (IDDW_VCPP_BASE+2)
#define IDDW_VCPP_CPU_WIN        (IDDW_VCPP_BASE+3)
#define IDDW_VCPP_MEMORY_WIN     (IDDW_VCPP_BASE+4)
#define IDDW_VCPP_CALLS_WIN      (IDDW_VCPP_BASE+5)
//#define IDDW_VCPP_BROWSE_WIN     (IDDW_VCPP_BASE+6)
#define IDDW_VCPP_DISASSY_WIN    (IDDW_VCPP_BASE+7)
#define IDDW_VCPP_BREAKPT_WIN    (IDDW_VCPP_BASE+8)
#define IDDW_VCPP_OLDWATCH_WIN  (IDDW_VCPP_BASE+9)
#define IDDW_VCPP_LOCALS_WIN    (IDDW_VCPP_BASE+10)

// Offsets from IDTB_VCPP_BASE
#define IDTB_VCPP_DEBUG (IDTB_VCPP_BASE+0)
#define IDTB_VCPP_BROWSE (IDTB_VCPP_BASE+1)
#define IDTB_VCPP_MACRO (IDTB_VCPP_BASE+2)

///////////////////////////////////////////////////////////////////////////////
//      PROJECT docked window(s).

// Offsets from IDDW_PROJECT_BASE
#define IDDW_PROJECT            (IDDW_PROJECT_BASE+0)
#define IDDW_GLOBAL				(IDDW_PROJECT_BASE+1)

// Offsets from IDTB_PROJECT_BASE
#define IDTB_PROJECT            (IDTB_PROJECT_BASE+0)

///////////////////////////////////////////////////////////////////////////////
//      MSIN docked window(s).

// Offsets from IDDW_MSIN_BASE
#define IDDW_MSIN               (IDDW_MSIN_BASE+0)
#define IDTB_MSIN               (IDTB_MSIN_BASE+0)
#define IDTB_CONTENTS           (IDTB_MSIN_BASE+1)


///////////////////////////////////////////////////////////////////////////////
//      IV5 docked window(s).
// Package defines actually used in the shell for things like hiding the
// output and topic windows when the Esc key is pressed.  Do not add to these.
#define IDDW_IV                IDDW_IV_BASE
#define IDDW_IVTOPIC          (IDDW_IV_BASE+0)
#define IDDW_IVRESULTS        (IDDW_IV_BASE+1)


///////////////////////////////////////////////////////////////////////////////
//      ClassView/WizardBar docked window(s).

// Offsets from IDTB_PACKAGE_BASE
#define IDTB_CLASSVIEW_WIZBARU      (IDTB_PACKAGE_BASE+0x0288)


#endif  // __DOCKIDS_H__


