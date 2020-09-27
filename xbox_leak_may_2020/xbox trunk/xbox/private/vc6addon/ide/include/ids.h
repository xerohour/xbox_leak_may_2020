/////////////////////////////////////////////////////////////////////////////
//  IDS.H
//      Header files containing all shared ID spaces.  These are 16 or 32-bit
//      IDs which used by packages with no methodology to guarantee uniqueness
//      other than ID or range allocation.

#ifndef __IDS_H__
#define __IDS_H__

//  Contains:
//      Package IDs
//      Resource and string ID ranges
//      Command ID ranges
//      Status Bar Indicator IDs
//      Command group IDs
//      Help IDs
//      Package notification IDs.   *Fix for v5.0*
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Package IDs

#define PACKAGE_SUSHI            1   // DEVSHL Must be first
#define PACKAGE_VCPP             2   // DEVSRC
#define PACKAGE_VRES             3   // DEVRES
#define PACKAGE_VPROJ            4   // DEVBLD
#define PACKAGE_PRJSYS           5   // DEVPRJ
#define PACKAGE_MSIN             6   // DEVMSIN
#define PACKAGE_LANGCPP          7   // DEVCPP
#define PACKAGE_LANGFOR          8   // DEVFOR
#define PACKAGE_GALLERY         10   // DEVGAL
#define PACKAGE_LANGMST         11   // DEVTEST
#define PACKAGE_CLASSVIEW       12   // DEVCLS
#define PACKAGE_ENT             13   // DEVENT
#define PACKAGE_LANGHTML        14
#define PACKAGE_BIN             15   // DEVBIN
#define PACKAGE_IMG             16   // DEVIMG
#define PACKAGE_NUMEGA          17
#define PACKAGE_HTMLEX          18   // DEVHTMX
#define PACKAGE_IATOOL          19  // Interactive Authoring Tool Package
#define PACKAGE_NCB             20
#define PACKAGE_IST             21  // Internet Studio
#define PACKAGE_LANGJVA         22  // Java Language Package
#define PACKAGE_JVA             23  // Java Build Package 
#define PACKAGE_AUT1            24   // DEVAUT1
#define PACKAGE_DEBUG           25   // DEVDBG
#define PACKAGE_ODL             26   // DEVODL.PKG
#define PACKAGE_AUT2            27   // DEVAUT2
#define PACKAGE_HTM             28   // DEVHTM
#define PACKAGE_DTG             29   // DEVDTG
#define PACKAGE_DDK             30   // DEVDDK
#define PACKAGE_IV              31   // DEVIV (eventual replacement for devmsin)

#define PACKAGE_ESPRESSO_PRJ    51
#define PACKAGE_ESPRESSO_RESTBL 52

#define PACKAGE_PARTNER        100   // Package wizard initial ID.
#define PACKAGE_SIMPLE         101   // DEVPKG (Sample package)

// Package Partners package IDs
#define PACKAGE_BLUESKY        102
#define PACKAGE_RST            103
#define PACKAGE_RATIONAL       104
#define PACKAGE_STINGRAY       105
#define PACKAGE_NUMEGA2		   106
#define PACKAGE_NUMEGA3		   107

#define PACKET_NIL      UINT(-1)    // For dockable windows with no command ui.

// This is for internal use by the system only
#define PACKET_SHARED   UINT(-2)    // the command is shared by more than one packet

// REVIEW(davidga): move these into Espresso?
// Packet IDs for Espresso
//
#define RESTBL_PACKET           2
#define PROJECT_PACKET          3

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Resource and string ID ranges
//

//  Sample Package
#define MIN_SAMPLE_RESOURCE_ID     0        // 0000
#define MAX_SAMPLE_RESOURCE_ID   399        // 018f
//  Espresso
#define MIN_ESPRESSO_RESOURCE_ID 12000       // 2EE0
#define MAX_ESPRESSO_RESOURCE_ID 12999       // 32C7
//  Test
#define MIN_TEST_RESOURCE_ID    2000        // 07d0
#define MAX_TEST_RESOURCE_ID    2991        // 0baf
//  Fortran
#define MIN_FORTRAN_RESOURCE_ID 2992        // 0bb0
#define MAX_FORTRAN_RESOURCE_ID 3999        // 0f9f
//  Intel x86 for Fortran
#define MIN_X86FOR_RESOURCE_ID  4000        // 0fa0
#define MAX_X86FOR_RESOURCE_ID  4999        // 1387

// SPACE 5000-5999  (0x1388 - 0x176f)

//  Package Partner Package (define one min/max set per partner--0x200 IDs)
//  Partners use 'PARTNER' IDs until a specific range is established for them.
#define MIN_PARTNER_RESOURCE_ID 6000        // 1770
#define MAX_PARTNER_RESOURCE_ID 6495        // 195f
// IATOOL (Interactive Authoring Tool Package)
#define MIN_IATOOL_RESOURCE_ID  6496        // 1960
#define MAX_IATOOL_RESOURCE_ID  6687        // 101f
// IST  (Internet Studio) 
#define MIN_IST_RESOURCE_ID     6688        // 1a20
#define MAX_IST_RESOURCE_ID     6895        // 1aef
// LANGJVA  (Java Language package)
#define MIN_LANGJVA_RESOURCE_ID 6896        // 1af0
#define MAX_LANGJVA_RESOURCE_ID 6959        // 1b2f
// JVA  (Java Build Package)
#define MIN_JVA_RESOURCE_ID     6960        // 1b30
#define MAX_JVA_RESOURCE_ID     7023        // 1b6f
// HTMLEX (HTML lexer)
#define MIN_HTMLEX_RESOURCE_ID  7024        // 1b70 
#define MAX_HTMLEX_RESOURCE_ID  7039        // 1b7f
// DEVAUT1 (OLE Automation 1)
#define MIN_AUT1_RESOURCE_ID    7040        // 1b80 
#define MAX_AUT1_RESOURCE_ID    7535        // 1d6f
// DEVAUT2 (OLE Automation 2)
#define MIN_AUT2_RESOURCE_ID    7536        // 1d70 
#define MAX_AUT2_RESOURCE_ID    7631        // 1dcf
// DEVHTM
#define MIN_HTM_RESOURCE_ID     7632        // 1dd0 
#define MAX_HTM_RESOURCE_ID     7679        // 1dff
// DEVDTG
#define MIN_DTG_RESOURCE_ID     7680        // 1e00 
#define MAX_DTG_RESOURCE_ID     7759        // 1e4f
// DEVDDK
#define MIN_DDK_RESOURCE_ID     7760        // 1e50 
#define MAX_DDK_RESOURCE_ID     7807        // 1e7f

//  SPACE 7776-9999  (0x1e60 - 0x270f)

// Helpstring IDs (intentionally not contiguous to allow neighbors room
//  to grow)--stored in DEVBLD.PKG, DEVEDIT.PKG, DEVSHL.DLL, DEVDBG.PKG, etc
// These IDs are shared by packages exposing a type library.  Each such package
//  stores its helpstrings in its string table, with IDs in this range.
//  Multiple packages reuse this range, but conflict isn't a problem, since
//  the helpstrings are loaded (by DevTLDc.dll) explicitly from the package.
// Strings shared across type libraries (e.g., helpstring for Application
//  properties) are stored in DevTLDc.dll's string table, with IDs in the range
//  between MIN_SHARED_HELPSTRING_ID and MAX_SHARED_HELPSTRING_ID (see below).
//  This range must not conflict with the SHARED_HELPSTRING_ID range.
#define MIN_HELPSTRING_ID               10000           // 0x2710
#define MAX_HELPSTRING_ID               11000           // 0x2af8

//  NuMega Package
#define MIN_NUMEGA_RESOURCE_ID    11001       // 2af9
#define MAX_NUMEGA_RESOURCE_ID    12500       // 30d4
//  BlueSky Package
#define MIN_BLUESKY_RESOURCE_ID   12501       // 30d5
#define MAX_BLUESKY_RESOURCE_ID   13500       // 34bc
//  Rational Package
#define MIN_RATIONAL_RESOURCE_ID  13501       // 34bd
#define MAX_RATIONAL_RESOURCE_ID  14500       // 38a4
// StingRay Package
#define MIN_STINGRAY_RESOURCE_ID  14501       // 38a5
#define MAX_STINGRAY_RESOURCE_ID  15200       // 3b60
// RST Package
#define MIN_RST_RESOURCE_ID       15201       // 3b61
#define MAX_RST_RESOURCE_ID       15583       // 3cdf

//  PowerPC
#define MIN_POWERPC_RESOURCE_ID 15584       // 3ce0
#define MAX_POWERPC_RESOURCE_ID 15615       // 3cff
//  PowerMac
#define MIN_POWERMAC_RESOURCE_ID    15616   // 3d00
#define MAX_POWERMAC_RESOURCE_ID    15711   // 3d5f
//  Mips
#define MIN_MIPS_RESOURCE_ID    15712       // 3d60
#define MAX_MIPS_RESOURCE_ID    15775       // 3d9f
//  Alpha
#define MIN_ALPHA_RESOURCE_ID   15776       // 3da0
#define MAX_ALPHA_RESOURCE_ID   15839       // 3ddf
//  Mac68K
#define MIN_68K_RESOURCE_ID     15840       // 3de0
#define MAX_68K_RESOURCE_ID     15919       // 3e2f
//  Intel x86
#define MIN_X86_RESOURCE_ID     15920       // 3e30
#define MAX_X86_RESOURCE_ID     15999       // 3e7f
//  Build package
#define MIN_BUILD_RESOURCE_ID   16000       // 3e80
#define MAX_BUILD_RESOURCE_ID   16991       // 425f
// App wizard
#define MIN_APP_WIZARD_ID       16992       // 4260
#define MAX_APP_WIZARD_ID       17487       // 444f
// Class wizard
#define MIN_CLASS_WIZARD_ID     17488       // 4450
#define MAX_CLASS_WIZARD_ID     17999       // 464f

#define MIN_NUMEGA2_ID			18000		// 4650
#define MAX_NUMEGA2_ID			19999		// 4e1f

//  Shell
#define MIN_SHARED_RESOURCE_ID  20000       // 4e20
#define MAX_SHARED_RESOURCE_ID  20207       // 4eef
#define MIN_EXE_RESOURCE_ID     20208       // 4f00
#define MAX_EXE_RESOURCE_ID     20287       // 4f3f
#define MIN_EXE_STRING_ID       57344               // e000
#define MAX_EXE_STRING_ID       57359               // e00f
#define MIN_SHELL_RESOURCE_ID   20288       // 4f40
#define MAX_SHELL_RESOURCE_ID   21999       // 55ef

//  Object Gallery
#define MIN_GALLERY_RESOURCE_ID 22000       // 55f0
#define MAX_GALLERY_RESOURCE_ID 22495       // 57df
// HTML
#define MIN_HTML_RESOURCE_ID    22496       // 57e0
#define MAX_HTML_RESOURCE_ID    22591       // 583f

//  SPACE 22592-22991  (0x5840 - 0x59ef)

//  Debug
#define MIN_DEBUG_RESOURCE_ID   24000       // 5dc0
#define MAX_DEBUG_RESOURCE_ID   25951       // 655f
//  CPP LangPackage
#define MIN_CPP_RESOURCE_ID     25952       // 6560
#define MAX_CPP_RESOURCE_ID     25999       // 658f
//  Resource package
#define MIN_RES_RESOURCE_ID     26000       // 6590
#define MAX_RES_RESOURCE_ID     27951       // 6d2f
//  BIN (Binary Editor)
#define MIN_BIN_RESOURCE_ID     27952       // 6d30
#define MAX_BIN_RESOURCE_ID     27967       // 6d3f
//  IMG (Image Editor)
#define MIN_IMG_RESOURCE_ID     27968       // 6d40
#define MAX_IMG_RESOURCE_ID     27999       // 6d5f
//  Edit package
#define MIN_EDIT_RESOURCE_ID    28000       // 6d60
#define MAX_EDIT_RESOURCE_ID    28527       // 6f6f
//  Odl package
#define MIN_ODL_RESOURCE_ID     28528       // 6f70
#define MAX_ODL_RESOURCE_ID     28591       // 6faf

//  SPACE 28591-28991  (0x6faf - 0x713f)

//  MSIN
#define MIN_MSIN_RESOURCE_ID    28992       // 7140
#define MAX_MSIN_RESOURCE_ID    29999       // 752f

//  IV
#define MIN_IV_RESOURCE_ID      30000       // 7530
#define MAX_IV_RESOURCE_ID      30499       // 7723

//  SPACE 30500-30992

//  Proj
#define MIN_WORKSPACE_RESOURCE_ID   30992   // 7910
#define MAX_WORKSPACE_RESOURCE_ID   31999   // 7cff
//  ClsView
#define MIN_CLSVIEW_RESOURCE_ID 32000       // 7d00
#define MAX_CLSVIEW_RESOURCE_ID 32095       // 7d5f
//  ENT (Galileo)  
#define MIN_ENT_RESOURCE_ID     32096       // 7d60
#define MAX_ENT_RESOURCE_ID     32511       // 7eff
// NCB (No Compile Browser)  
#define MIN_NCB_RESOURCE_ID     32512       // 7f00
#define MAX_NCB_RESOURCE_ID     32543       // 7f1f

//  String IDs  (Range 32k to 64k) 
//  These ranges can ONLY be used by string resource types

// IV (InfoViewer)
#define MIN_IV_STRING_ID        30200               // 75f8
#define MAX_IV_STRING_ID        30326               // 7676

//  HTMLEX (HTML Lexer)
#define MIN_HTMLEX_STRING_ID    32768               // 8000
#define MAX_HTMLEX_STRING_ID    32831               // 803f
// NCB (No Compile Browser)
#define MIN_NCB_STRING_ID       32832               // 8040
#define MAX_NCB_STRING_ID       32895               // 807f
//  IATool (Interactive Authoring Tool Package)
#define MIN_IATOOL_STRING_ID    32896               // 8080
#define MAX_IATOOL_STRING_ID    33951               // 849f

// SPACE 33952 - 34463 (84a0 - 869f)

// LANGJVA (Java Language Package)
#define MIN_LANGJVA_STRING_ID   34464               // 86a0
#define MAX_LANGJVA_STRING_ID   34671               // 876f
//  Odl package
#define MIN_ODL_STRING_ID       34672               // 8770
#define MAX_ODL_STRING_ID       34719               // 879f

//  SPACE 34719-35007  (0x879f - 0x88bf)

// CLASSVIEW
#define MIN_CLASSVIEW_STRING_ID 35008               // 88c0
#define MAX_CLASSVIEW_STRING_ID 35103               // 891f
// HTM
#define MIN_HTM_STRING_ID       35104               // 8920
#define MAX_HTM_STRING_ID       35295               // 89df
// DTG
#define MIN_DTG_STRING_ID       35296               // 89e0
#define MAX_DTG_STRING_ID       35839               // 8bff
// DDK
#define MIN_DDK_STRING_ID       35840               // 8c00
#define MAX_DDK_STRING_ID       35935               // 8c5f
// POWERPC
#define MIN_POWERPC_STRING_ID   35936               // 8c60
#define MAX_POWERPC_STRING_ID   35999               // 8c9f
// LANGCPP
#define MIN_LANGCPP_STRING_ID   36000               // 8ca0
#define MAX_LANGCPP_STRING_ID   36255               // 8d9f
// IST  (Internet Studio) 
#define MIN_IST_STRING_ID       36256               // 8dA0
#define MAX_IST_STRING_ID       37503               // 927F

//  NuMega Package
#define MIN_NUMEGA_STRING_ID    37632               // 9300
#define MAX_NUMEGA_STRING_ID    37887               // 93ff
//  BlueSky Package
#define MIN_BLUESKY_STRING_ID   37888               // 9400
#define MAX_BLUESKY_STRING_ID   38143               // 94ff
//  Rational Package
#define MIN_RATIONAL_STRING_ID  38144               // 9500
#define MAX_RATIONAL_STRING_ID  38399               // 95ff
// StingRay Package
#define MIN_STINGRAY_STRING_ID  38400               // 9600
#define MAX_STINGRAY_STRING_ID  38655               // 96ff
// RST Package
#define MIN_RST_STRING_ID       38656               // 9700
#define MAX_RST_STRING_ID       38911               // 97ff

// NUMEGA2 
#define MIN_NUMEGA2_STRING_ID	38912				// 9800
#define MAX_NUMEGA2_STRING_ID	41216				// a100

/////////////////////////////////////////////////////////////////////////////
// Shared resource IDs
//

// resource type ids
// The numbering of these types has been modified to fit with the 
// shared resource numbering scheme. This allows any package
// to use its own resource types in safety. martynl 13May96

// This one is used in entry.rc and the shell package for the default UI layout
#define IDRT_LAYOUT          20000
// Used in various packages (*.rc2 files) to store their encrypted package id information
#define IDRT_PID             20001

// The actual product ID resource number
#define ID_PID               20000

// command table stored in the main executable file
#define IDCT_MAINEXE                    20397

// Dialog controls
#define IDC_CAPTION                     102
#define IDC_ID_CAP                      221
#define IDC_ID                          222
#define IDC_SCC_STATUS                  16830
#define IDC_SCC_STATUS_TEXT             16831

#define IDC_SCC                         21094
#define IDC_SCC_HELPERTEXT              21101
#define IDC_SCC_SEPARATOR               21102

// Resources
#define IDC_CURSOR_RECORD               7066    // Stored in DevAut1.pkg
#define IDR_MAINFRAME                   20000
#define IDR_FIND_SPECIAL                21093 // stored in devshl.dll

#define IDB_VC256                       20001
#define IDB_VC16                        20002
#define IDB_ABOUT                       20003

#define IDB_DROP                        20508
#define IDB_DROP95                      21086
#define IDB_DROPLARGE                   21182
#define IDB_DROP95LARGE                 21183

#define IDB_DROPFLAT                    21200
#define IDB_DROP95FLAT                  21201
#define IDB_DROP95LARGEFLAT             21204
#define IDB_DROPLARGEFLAT               21205

#define IDB_DROPCL                      20544
#define IDB_DROPCL95                    21083
#define IDB_DROPCLLARGE                 21184
#define IDB_DROPCL95LARGE               21185

#define IDB_DROPCR                      20545
#define IDB_DROPCR95                    21084
#define IDB_DROPCRLARGE                 21186
#define IDB_DROPCR95LARGE               21187

#define IDB_DROPDISABLED                20547
#define IDB_DROPDISABLED95              21085
#define IDB_DROPDISABLEDLARGE           21188
#define IDB_DROPDISABLED95LARGE         21189

#define IDC_RIGHT_ARROW                 20001
#define IDC_SIZE_BAR_HORZ               20002
#define IDCUR_VSIZE                     20003
#define IDCUR_COPY                      20004

#define IDD_ABOUTBOX                    20001
#define IDD_GOTODIALOG                  20002
#define IDD_PUSHPIN_DIALOGBAR           20003
#define IDD_SUSHI_FILEOPEN              20522
#define IDD_OPTIONS_FONTCOLOR           21080

// Strings
#define IDS_DEBUG_CONFIG                20001
#define IDS_RELEASE_CONFIG              20002
#define IDS_UNDO_PROPEDIT               20003
#define IDS_UNDO_MOVE                   20004
#define IDS_UNDO_COPY                   20005
#define IDS_UNDO_PASTE                  20006
#define IDS_UNDO_CUT                    20007
#define IDS_UNDO_DELETE                 20008
#define IDS_ABOUTTITLE                  20009
#define IDS_COPYRIGHT                   20010
#define IDS_PRODUCTLICENSEDTO           20011
#define IDS_SERIALNUMBER                20012
#define IDS_ADDITIONS                   20013
#define IDS_PID                         20014
#define IDS_WARNING1                    20015
#define IDS_WARNING2                    20016
#define IDS_WARNING3                    20017
#define IDS_WARNING4                    20018
#define IDS_WARNING5                    20019
#define IDS_WARNING6                    20020
#define IDS_WARNING7                    20021
#define IDS_NOMATHCHIP                  20022
#define IDS_HASMATHCHIP                 20023
#define IDS_FMTFREESPACE                20024
#define IDS_UNAVAILABLE                 20025
#define IDS_FMTAVAILMEM                 20026
#define IDS_CANT_LOAD_PACKAGE           20027
#define IDS_CANT_LOAD_MAC               20028
#define IDS_LICENSEDTO                  20029
#define IDS_GENERAL                     20030
#define IDS_STYLES                      20031
#define IDS_EXSTYLES                    20032
#define IDS_UNNAMED                     20033
#define IDS_ABOUTTITLEPADDING           20034
#define IDS_DSPROJ_SIG_STRING           20035
#define IDS_PACKAGE_OWNER               20036
#define IDS_KEYBOARD                    20037
#define IDS_COMMANDS                    20038
#define IDS_DSPROJ_NAME                 20039

// Shared Help String Context IDs--strings stored in DEVTLDC.DLL
// These are IDs used in the string table of DevTLDc.dll and are referenced
//  by type libraries via helpstringcontext attribute.  These strings are
//  NOT stored in devshl.dll-- they're in DevTLDc.dll.  As long as these IDs
//  don't collide with the HELPSTRING_ID range (see above) no conflicts can
//  arise.
#define MIN_SHARED_HELPSTRING_ID                1000
#define MAX_SHARED_HELPSTRING_ID                2000
// The following strings are all between the above two ranges.

// Misc. Shared help strings.
#define MIN_HS_SHARED                                                   MIN_SHARED_HELPSTRING_ID
#define IDS_HS_SHARED_APPLICATION                               MIN_HS_SHARED
#define IDS_HS_SHARED_PARENT                                    (IDS_HS_SHARED_APPLICATION+1)
#define MAX_HS_SHARED                                                   (IDS_HS_SHARED_PARENT+1)

// Help strings for IGenericDocument.
#define IDS_HS_GENERICDOCUMENT                                  MAX_HS_SHARED
#define IDS_HS_GENERICDOCUMENT_NAME                             (IDS_HS_GENERICDOCUMENT+1)
#define IDS_HS_GENERICDOCUMENT_FULLNAME                 (IDS_HS_GENERICDOCUMENT+2)
#define IDS_HS_GENERICDOCUMENT_PATH                             (IDS_HS_GENERICDOCUMENT+3)
#define IDS_HS_GENERICDOCUMENT_SAVED                    (IDS_HS_GENERICDOCUMENT+4)
#define IDS_HS_GENERICDOCUMENT_ACTIVEWINDOW             (IDS_HS_GENERICDOCUMENT+5)
#define IDS_HS_GENERICDOCUMENT_READONLY                 (IDS_HS_GENERICDOCUMENT+6)
#define IDS_HS_GENERICDOCUMENT_TYPE                             (IDS_HS_GENERICDOCUMENT+7)
#define IDS_HS_GENERICDOCUMENT_WINDOWS                  (IDS_HS_GENERICDOCUMENT+8)
#define IDS_HS_GENERICDOCUMENT_NEWWINDOW                (IDS_HS_GENERICDOCUMENT+9)
#define IDS_HS_GENERICDOCUMENT_SAVE                             (IDS_HS_GENERICDOCUMENT+10)
#define IDS_HS_GENERICDOCUMENT_CLOSE                    (IDS_HS_GENERICDOCUMENT+11)
#define IDS_HS_GENERICDOCUMENT_UNDO                             (IDS_HS_GENERICDOCUMENT+12)
#define IDS_HS_GENERICDOCUMENT_REDO                             (IDS_HS_GENERICDOCUMENT+13)
#define IDS_HS_GENERICDOCUMENT_PRINTOUT                 (IDS_HS_GENERICDOCUMENT+14)
#define IDS_HS_GENERICDOCUMENT_ACTIVE                   (IDS_HS_GENERICDOCUMENT+15)
#define IDS_HS_GENERICDOCUMENT_max                              (IDS_HS_GENERICDOCUMENT+17)

// Help strings for IGenericWindow
#define IDS_HS_GENERICWINDOW                                    IDS_HS_GENERICDOCUMENT_max
#define IDS_HS_GENERICWINDOW_CAPTION                    (IDS_HS_GENERICWINDOW+1)
#define IDS_HS_GENERICWINDOW_TYPE                               (IDS_HS_GENERICWINDOW+2)
#define IDS_HS_GENERICWINDOW_ACTIVE                     (IDS_HS_GENERICWINDOW+3)
#define IDS_HS_GENERICWINDOW_LEFT                               (IDS_HS_GENERICWINDOW+4)
#define IDS_HS_GENERICWINDOW_TOP                                (IDS_HS_GENERICWINDOW+5)
#define IDS_HS_GENERICWINDOW_WIDTH                              (IDS_HS_GENERICWINDOW+6)
#define IDS_HS_GENERICWINDOW_HEIGHT                     (IDS_HS_GENERICWINDOW+7)
#define IDS_HS_GENERICWINDOW_INDEX                              (IDS_HS_GENERICWINDOW+8)
#define IDS_HS_GENERICWINDOW_NEXT                               (IDS_HS_GENERICWINDOW+9)
#define IDS_HS_GENERICWINDOW_PREVIOUS                   (IDS_HS_GENERICWINDOW+10)
#define IDS_HS_GENERICWINDOW_WINDOWSTATE                (IDS_HS_GENERICWINDOW+11)
#define IDS_HS_GENERICWINDOW_CLOSE                              (IDS_HS_GENERICWINDOW+12)
#define IDS_HS_GENERICWINDOW_PARENT                     (IDS_HS_GENERICWINDOW+13)
#define IDS_HS_GENERICWINDOW_max                                (IDS_HS_GENERICWINDOW+14)


// Help strings for IGenericProject
#define IDS_HS_GENERICPROJECT                                   IDS_HS_GENERICWINDOW_max
#define IDS_HS_GENERICPROJECT_NAME                              (IDS_HS_GENERICPROJECT+1)
#define IDS_HS_GENERICPROJECT_FULLNAME                  (IDS_HS_GENERICPROJECT+2)
#define IDS_HS_GENERICPROJECT_TYPE                              (IDS_HS_GENERICPROJECT+3)
#define IDS_HS_GENERICPROJECT_max                               (IDS_HS_GENERICPROJECT+4)

// Help strings for enums and other constants

// Help strings for DsWindowState
#define IDS_HS_DSWINDOWSTATE                                    IDS_HS_GENERICPROJECT_max
#define IDS_HS_DSWINDOWSTATE_MAXIMIZED                  (IDS_HS_DSWINDOWSTATE+1)
#define IDS_HS_DSWINDOWSTATE_MINIMIZED                  (IDS_HS_DSWINDOWSTATE+2)
#define IDS_HS_DSWINDOWSTATE_NORMAL                     (IDS_HS_DSWINDOWSTATE+3)
#define IDS_HS_DSWINDOWSTATE_max                                (IDS_HS_DSWINDOWSTATE+4)


// Help strings for DsArrangeStyle
#define IDS_HS_DSARRANGESTYE                                    IDS_HS_DSWINDOWSTATE_max
#define IDS_HS_DSARRANGESTYE_MINIMIZE                   (IDS_HS_DSARRANGESTYE+1)
#define IDS_HS_DSARRANGESTYE_TILEHORIZONTAL     (IDS_HS_DSARRANGESTYE+2)
#define IDS_HS_DSARRANGESTYE_TILEVERTICAL               (IDS_HS_DSARRANGESTYE+3)
#define IDS_HS_DSARRANGESTYE_CASCADE                    (IDS_HS_DSARRANGESTYE+4)
#define IDS_HS_DSARRANGESTYE_max                                (IDS_HS_DSARRANGESTYE+5)


// Help strings for DsButtonType 
#define IDS_HS_DSBUTTONTYPE                                     IDS_HS_DSARRANGESTYE_max
#define IDS_HS_DSBUTTONTYPE_GLYPH                               (IDS_HS_DSBUTTONTYPE+1)
#define IDS_HS_DSBUTTONTYPE_TEXT                                (IDS_HS_DSBUTTONTYPE+2)
#define IDS_HS_DSBUTTONTYPE_max                                 (IDS_HS_DSBUTTONTYPE+3)

// Help string for DsSaveChanges
#define IDS_HS_DSSAVECHANGES                                    IDS_HS_DSBUTTONTYPE_max
#define IDS_HS_DSSAVECHANGES_YES                                (IDS_HS_DSSAVECHANGES+1)
#define IDS_HS_DSSAVECHANGES_NO                                 (IDS_HS_DSSAVECHANGES+2)
#define IDS_HS_DSSAVECHANGES_PROMPT                     (IDS_HS_DSSAVECHANGES+3)
#define IDS_HS_DSSAVECHANGES_max                                (IDS_HS_DSSAVECHANGES+4)

// Help string for DsSaveStatus
#define IDS_HS_DSAVESTATUS                                              IDS_HS_DSSAVECHANGES_max
#define IDS_HS_DSAVESTATUS_SUCCEEDED                    (IDS_HS_DSAVESTATUS+1)
#define IDS_HS_DSAVESTATUS_CANCELED                     (IDS_HS_DSAVESTATUS+2)
#define IDS_HS_DSAVESTATUS_max                                  (IDS_HS_DSAVESTATUS+3)

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Command ID ranges

//  Shell
#define MIN_SHELL_COMMAND_ID    0x8000
#define MAX_SHELL_COMMAND_ID    0x83FF
//  Build package
#define MIN_BUILD_COMMAND_ID    0x8400
#define MAX_BUILD_COMMAND_ID    0x85FF
//  Res package
#define MIN_RES_COMMAND_ID      0x8600
#define MAX_RES_COMMAND_ID      0x86FF
//  Src
#define MIN_SOURCE_COMMAND_ID   0x8700
#define MAX_SOURCE_COMMAND_ID   0x88FF
//  ClassView/Wizards
#define MIN_CLASSVIEW_COMMAND_ID   0x8900
#define MAX_CLASSVIEW_COMMAND_ID   0x89FF
//  MSIN
#define MIN_MSIN_COMMAND_ID     0x8A00
#define MAX_MSIN_COMMAND_ID     0x8BFF
//  Proj
#define MIN_WORKSPACE_COMMAND_ID 0x8C00
#define MAX_WORKSPACE_COMMAND_ID 0x8FFF
//  HTML
#define MIN_HTML_COMMAND_ID  0x9000
#define MAX_HTML_COMMAND_ID  0x900F
//  BIN
#define MIN_BIN_COMMAND_ID  0x9010
#define MAX_BIN_COMMAND_ID  0x901F
//  IMG
#define MIN_IMG_COMMAND_ID  0x9020
#define MAX_IMG_COMMAND_ID  0x907F
//  ODL
#define MIN_ODL_COMMAND_ID  0x9080
#define MAX_ODL_COMMAND_ID  0x909F
// IV
#define MIN_IV_COMMAND_ID   0x90A0
#define MAX_IV_COMMAND_ID   0x9FFF
//  Fortran
#define MIN_FORTRAN_COMMAND_ID  0xA000
#define MAX_FORTRAN_COMMAND_ID  0xA1FF
//  Test
#define MIN_TEST_COMMAND_ID     0xA200
#define MAX_TEST_COMMAND_ID     0xA3FF
//  Espresso
#define MIN_ESPRESSO_COMMAND_ID 0xA400
#define MAX_ESPRESSO_COMMAND_ID 0xA5FF
//  Enterprise Edition (Galileo)
#define MIN_ENT_COMMAND_ID      0xA600
#define MAX_ENT_COMMAND_ID      0xA67F
// Jakarta
#define MIN_JAVA_COMMAND_ID     0xA681
#define MAX_JAVA_COMMAND_ID     0xA6FF
//  Jakarta Image Editor
#define MIN_IMAGE_COMMAND_ID    0xA700
#define MAX_IMAGE_COMMAND_ID    0xA7FF
//  LangCPP
#define MIN_LANGCPP_COMMAND_ID  0xA800
#define MAX_LANGCPP_COMMAND_ID  0xA8FF

#define MIN_ADD_TO_PROJECT		0xA900
#define MAX_ADD_TO_PROJECT		0xA9FF
#define ID_ADD_TO_PROJECT_FIRST	0xA900
#define ID_ADD_TO_PROJECT_LAST	0xA9FF

//  SPACE 0xAA00-0xAEFF

//  Sample Package
#define MIN_SAMPLE_COMMAND_ID   0xAF00
#define MAX_SAMPLE_COMMAND_ID   0xAFFF
//  NuMega Package
#define MIN_NUMEGA_COMMAND_ID   0xB000
#define MAX_NUMEGA_COMMAND_ID   0xB1FF
//  Package Partner Package (define one min/max set per partner--512 IDs)
//  Partners use 'PARTNER' IDs until a specific range is established for them.
#define MIN_PARTNER_COMMAND_ID  0xB200
#define MAX_PARTNER_COMMAND_ID  0xB3FF

//  SPACE 0xB400-0xB78F 

// IATOOL (Interactive Authoring Tool Package)
#define MIN_IATOOL_COMMAND_ID   0xB400
#define MAX_IATOOL_COMMAND_ID   0xB5FF
// NCB (No Compile Browser)
// !!!!!This is an internal package.
#define MIN_NCB_COMMAND_ID      0xB600
#define MAX_NCB_COMMAND_ID      0xB625
// IST (Internet Studio)    
#define MIN_IST_COMMAND_ID      0xB650
#define MAX_IST_COMMAND_ID      0xB6FF
// LANGJVA (Java Language Package)
#define MIN_LANGJVA_COMMAND_ID  0xB700
#define MAX_LANGJVA_COMMAND_ID  0xB78F
// OLE Automation 1 (DevAUT1.pkg)
#define MIN_AUT1_COMMAND_ID     0xB790
#define MAX_AUT1_COMMAND_ID     0xC78F
// OLE Automation 2 (DevAUT2.pkg)
#define MIN_AUT2_COMMAND_ID     0xC790
#define MAX_AUT2_COMMAND_ID     0xC8EF
// HTM
#define MIN_HTM_COMMAND_ID      0xC8F0
#define MAX_HTM_COMMAND_ID      0xCAEF
// DTG
#define MIN_DTG_COMMAND_ID      0xCAF0
#define MAX_DTG_COMMAND_ID      0xCCEF
// DDK
#define MIN_DDK_COMMAND_ID      0xCCF0
#define MAX_DDK_COMMAND_ID      0xCCFF

//  BlueSky Package
#define MIN_BLUESKY_COMMAND_ID  0xCD00
#define MAX_BLUESKY_COMMAND_ID  0xCDFF
//  Rational Package
#define MIN_RATIONAL_COMMAND_ID 0xCE00
#define MAX_RATIONAL_COMMAND_ID 0xCEFF
//  RST Package
#define MIN_RST_COMMAND_ID      0xCF00
#define MAX_RST_COMMAND_ID      0xCFFF
//  StingRay Package
#define MIN_STINGRAY_COMMAND_ID 0xD000
#define MAX_STINGRAY_COMMAND_ID 0xD0FF


//  SPACE 0xD100-0xDFFF

// MFC
#define MIN_MFC_COMMAND_ID      0xE000
#define MAX_MFC_COMMAND_ID      0xFFFF

/////////////////////////////////////////////////////////////////////
// Shell Commands
//
#define IDM_TOOLBAR_EDIT                0x8000
#define IDM_CUSTOMIZE                   0x8001
#define ID_WINDOW_CLOSE_ALL             0x8002
#define IDM_OPTIONS                     0x8003
#define IDM_TOGGLE_MDI                  0x8004
#define IDM_TOOLBAR_CUSTOMIZE           0x8005
#define ID_WINDOW_FULLSCREEN            0x8006
#define ID_FILE_SAVE_ALL_EXIT           0x8007
#define IDM_CONTEXT_SEPARATOR           0x8008
#define IDM_TOOLBARS_LIST               0x8009
#define IDM_TOOLBARS_CUSTOMIZE          0x800a

// These two only used for tooltips
#define IDM_BUTTON_DOCK_CLOSE           0x800b
#define IDM_BUTTON_DOCK_MAX             0x800c
#define IDM_BUTTON_DOCK_RESTORE         0x800d

// Window commands
#define IDM_WINDOW_CLOSE                                0x800e
#define IDM_WINDOW_NEXT                                 0x800f
#define IDM_WINDOW_PREVIOUS                             0x8010

#define IDM_REFRESH                                             0x8011
#define IDM_FILE_NEW_IN_PROJ            0x8012

// This command group is special becuase it is enabled even when the toolbar
// customisation window is visible. martynl 25Mar96
#define IDM_BUTTON_BASE                 0x8080
#define IDM_BUTTON_COPYIMAGE                    0x8080
#define IDM_BUTTON_PASTEIMAGE                   0x8081
#define IDM_BUTTON_RESETIMAGE           0x8082
#define IDM_BUTTON_CHOOSEAPPEARANCE     0x8083
#define IDM_BUTTON_RESET                                0x8084
#define IDM_BUTTON_DELETE                               0x8085
#define IDM_BUTTON_IMAGEONLY                    0x8086
#define IDM_BUTTON_TEXTONLY                             0x8088
#define IDM_BUTTON_IMAGETEXT                    0x8089
#define IDM_BUTTON_GROUPSTART                   0x808a
#define IDM_BUTTON_LAST                 0x808a

// READ THIS IF YOU ARE MODIFYING MENU IDS:
// Menus are now commands, and as such, have command ids like everything else in the world.
// Code elsewhere (entry.cpp) relies on the order of these, and the fact that they increase 
// monotonically, so change with caution.
// THIS MEANS THAT EACH OF THESE SHOULD BE ONE MORE THAN THE PREVIOUS ONE. NO SPACES ALLOWED

#define IDM_MENU_FIRSTMAIN                              0x8100
#define IDM_MENU_BAR                                    0x8100
#define IDM_MENU_FILE                                   0x8101
#define IDM_MENU_EDIT                                   0x8102
#define IDM_MENU_VIEW                                   0x8103
#define IDM_MENU_INSERT                                 0x8104
#define IDM_MENU_BUILD_DEBUG            0x8105
#define IDM_MENU_BUILD                                  0x8106
#define IDM_MENU_IMAGE                  0x8107
#define IDM_MENU_LAYOUT_ALIGN                   0x8108
#define IDM_MENU_LAYOUT_SPACE           0x8109
#define IDM_MENU_LAYOUT_CENTER          0x810a
#define IDM_MENU_LAYOUT_ARRANGE         0x810b
#define IDM_MENU_LAYOUT_SAMESIZE        0x810c
#define IDM_MENU_LAYOUT_AUTO                    0x810d
#define IDM_MENU_LAYOUT                                 0x810e
#define IDM_MENU_TEST                                   0x810f
#define IDM_MENU_CODECOVERAGE                   0x8110
#define IDM_MENU_DEBUG                  0x8111
#define IDM_MENU_TOOLS_SCC              0x8112
#define IDM_MENU_TOOLS                                  0x8113
#define IDM_MENU_WINDOW                 0x8114
#define IDM_MENU_HELP                   0x8115
#define IDM_MENU_INTERNAL_DEBUG         0x8116
#define IDM_MENU_INTERNAL_PERF          0x8117
#define IDM_MENU_VIEW_TOOLBARS                          0x8118
#define IDM_MENU_OLEBAR                          0x8119
#define IDM_MENU_HELP_MSONTHEWEB                0x811a
#define IDM_MENU_FILEMRU                                0x811b
#define IDM_MENU_WKSMRU                                 0x811c
#define IDM_MENU_DEBUG_WINDOWS                  0x811d
#define IDM_MENU_PROJECT                                0x811e
#define IDM_MENU_PROJECT_LIST                           0x811f
#define IDM_MENU_PROJECT_ADD                            0x8120
#define IDM_MENU_LASTMAIN                               0x8120

#define IDM_DOCKSHOW_BASE               0x8200
#define IDM_DOCKSHOW_LAST               0x8218  //24 possible show commands for all dockable windows.

// Custom menu ids for 'new' menus generated and managed by the shell
#define IDM_CUSTOMMENU_BASE                             0x8240

#define IDM_CUSTOMMENU_0                (IDM_CUSTOMMENU_BASE + 0)
#define IDM_CUSTOMMENU_1                (IDM_CUSTOMMENU_BASE + 1)
#define IDM_CUSTOMMENU_2                (IDM_CUSTOMMENU_BASE + 2)
#define IDM_CUSTOMMENU_3                (IDM_CUSTOMMENU_BASE + 3)
#define IDM_CUSTOMMENU_4                (IDM_CUSTOMMENU_BASE + 4)
#define IDM_CUSTOMMENU_5                (IDM_CUSTOMMENU_BASE + 5)
#define IDM_CUSTOMMENU_6                (IDM_CUSTOMMENU_BASE + 6)
#define IDM_CUSTOMMENU_7                (IDM_CUSTOMMENU_BASE + 7)
#define IDM_CUSTOMMENU_8                (IDM_CUSTOMMENU_BASE + 8)
#define IDM_CUSTOMMENU_9                (IDM_CUSTOMMENU_BASE + 9)
#define IDM_CUSTOMMENU_10               (IDM_CUSTOMMENU_BASE + 10)
#define IDM_CUSTOMMENU_11               (IDM_CUSTOMMENU_BASE + 11)
#define IDM_CUSTOMMENU_12               (IDM_CUSTOMMENU_BASE + 12)
#define IDM_CUSTOMMENU_13               (IDM_CUSTOMMENU_BASE + 13)
#define IDM_CUSTOMMENU_14               (IDM_CUSTOMMENU_BASE + 14)
#define IDM_CUSTOMMENU_15               (IDM_CUSTOMMENU_BASE + 15)
#define IDM_CUSTOMMENU_LAST                             0x824f

#define IDM_ACTIVATE_PROJECT_WORKSPACE  0x8280
#define IDM_TOGGLE_PROJECT_WORKSPACE    0x8281

#define IDMX_IDPROP                     0x8290
#define IDMX_PROPGENERAL                0x8291
#define IDMX_PROPSTYLES                 0x8292
#define ID_FASTHELP                     0x8298

#define IDM_USERTOOLS_BASE              0x82A0
#define IDM_USERTOOLS_0                 (IDM_USERTOOLS_BASE + 0)
#define IDM_USERTOOLS_1                 (IDM_USERTOOLS_BASE + 1)
#define IDM_USERTOOLS_2                 (IDM_USERTOOLS_BASE + 2)
#define IDM_USERTOOLS_3                 (IDM_USERTOOLS_BASE + 3)
#define IDM_USERTOOLS_4                 (IDM_USERTOOLS_BASE + 4)
#define IDM_USERTOOLS_5                 (IDM_USERTOOLS_BASE + 5)
#define IDM_USERTOOLS_6                 (IDM_USERTOOLS_BASE + 6)
#define IDM_USERTOOLS_7                 (IDM_USERTOOLS_BASE + 7)
#define IDM_USERTOOLS_8                 (IDM_USERTOOLS_BASE + 8)
#define IDM_USERTOOLS_9                 (IDM_USERTOOLS_BASE + 9)
#define IDM_USERTOOLS_10                (IDM_USERTOOLS_BASE + 10)
#define IDM_USERTOOLS_11                (IDM_USERTOOLS_BASE + 11)
#define IDM_USERTOOLS_12                (IDM_USERTOOLS_BASE + 12)
#define IDM_USERTOOLS_13                (IDM_USERTOOLS_BASE + 13)
#define IDM_USERTOOLS_14                (IDM_USERTOOLS_BASE + 14)
#define IDM_USERTOOLS_15                (IDM_USERTOOLS_BASE + 15)
#define IDM_USERTOOLS_LAST              0x82AF  //16 user tools on Tools menu

#define IDM_WINDOWS_BASE              0x82B0
#define IDM_WINDOWS_0                 (IDM_WINDOWS_BASE + 0)
#define IDM_WINDOWS_1                 (IDM_WINDOWS_BASE + 1)
#define IDM_WINDOWS_2                 (IDM_WINDOWS_BASE + 2)
#define IDM_WINDOWS_3                 (IDM_WINDOWS_BASE + 3)
#define IDM_WINDOWS_4                 (IDM_WINDOWS_BASE + 4)
#define IDM_WINDOWS_5                 (IDM_WINDOWS_BASE + 5)
#define IDM_WINDOWS_6                 (IDM_WINDOWS_BASE + 6)
#define IDM_WINDOWS_7                 (IDM_WINDOWS_BASE + 7)
#define IDM_WINDOWS_8                 (IDM_WINDOWS_BASE + 8)
#define IDM_WINDOWS_9                 (IDM_WINDOWS_BASE + 9)
#define IDM_WINDOWS_10                (IDM_WINDOWS_BASE + 10)
#define IDM_WINDOWS_11                (IDM_WINDOWS_BASE + 11)
#define IDM_WINDOWS_12                (IDM_WINDOWS_BASE + 12)
#define IDM_WINDOWS_13                (IDM_WINDOWS_BASE + 13)
#define IDM_WINDOWS_14                (IDM_WINDOWS_BASE + 14)
#define IDM_WINDOWS_15                (IDM_WINDOWS_BASE + 15)
#define IDM_WINDOWS_16                (IDM_WINDOWS_BASE + 16)
#define IDM_WINDOWS_17                (IDM_WINDOWS_BASE + 17)
#define IDM_WINDOWS_18                (IDM_WINDOWS_BASE + 18)
#define IDM_WINDOWS_19                (IDM_WINDOWS_BASE + 19)
#define IDM_WINDOWS_20                (IDM_WINDOWS_BASE + 20)
#define IDM_WINDOWS_21                (IDM_WINDOWS_BASE + 21)
#define IDM_WINDOWS_22                (IDM_WINDOWS_BASE + 22)
#define IDM_WINDOWS_23                (IDM_WINDOWS_BASE + 23)
#define IDM_WINDOWS_24                (IDM_WINDOWS_BASE + 24)
#define IDM_WINDOWS_25                (IDM_WINDOWS_BASE + 25)
#define IDM_WINDOWS_26                (IDM_WINDOWS_BASE + 26)
#define IDM_WINDOWS_27                (IDM_WINDOWS_BASE + 27)
#define IDM_WINDOWS_28                (IDM_WINDOWS_BASE + 28)
#define IDM_WINDOWS_29                (IDM_WINDOWS_BASE + 29)
#define IDM_WINDOWS_30                (IDM_WINDOWS_BASE + 30)
#define IDM_WINDOWS_31                (IDM_WINDOWS_BASE + 31)
#define IDM_WINDOWS_LAST              0x82CF  //32 items max on windows menu
#define ID_WINDOW_LIST_DOCS           0x82D0

#define ID_PROJ_MRU_FIRST                               0x82E0
#define ID_PROJ_MRU_FILE1               (ID_PROJ_MRU_FIRST)
#define ID_PROJ_MRU_FILE2               (ID_PROJ_MRU_FIRST + 1)
#define ID_PROJ_MRU_FILE3               (ID_PROJ_MRU_FIRST + 2)
#define ID_PROJ_MRU_FILE4               (ID_PROJ_MRU_FIRST + 3)
#define ID_PROJ_MRU_FILE5               (ID_PROJ_MRU_FIRST + 4)
#define ID_PROJ_MRU_FILE6               (ID_PROJ_MRU_FIRST + 5)
#define ID_PROJ_MRU_FILE7               (ID_PROJ_MRU_FIRST + 6)
#define ID_PROJ_MRU_FILE8               (ID_PROJ_MRU_FIRST + 7)
#define ID_PROJ_MRU_FILE9               (ID_PROJ_MRU_FIRST + 8)
#define ID_PROJ_MRU_FILE10              (ID_PROJ_MRU_FIRST + 9)
#define ID_PROJ_MRU_FILE11              (ID_PROJ_MRU_FIRST + 10)
#define ID_PROJ_MRU_FILE12              (ID_PROJ_MRU_FIRST + 11)
#define ID_PROJ_MRU_FILE13              (ID_PROJ_MRU_FIRST + 12)
#define ID_PROJ_MRU_FILE14              (ID_PROJ_MRU_FIRST + 13)
#define ID_PROJ_MRU_FILE15              (ID_PROJ_MRU_FIRST + 14)
#define ID_PROJ_MRU_FILE16              (ID_PROJ_MRU_FIRST + 15)
#define ID_PROJ_MRU_LAST                                0x82EF

#ifdef _DEBUG
#define IDM_OPENLAYOUT                  0x8300
#endif

#define IDM_INSERT_FILES                0x8301
#define ID_TOOLS_RECORD                 0x8302
#define ID_TOOLS_PLAYBACK               0x8303
#define ID_FILE_NEW_SOURCE              0x8304
#define ID_FILE_NEW_RC                  0x8305
#define IDM_WINDOW_SHOWPROPERTIES       0x8306
#define IDM_WINDOW_HSPROPERTIES         0x8307
#define ID_WINDOW_HIDE                  0x8308
#define ID_WINDOW_POPUP_HIDE            0x8309

#define ID_PROP_PUSHPIN                 0x4edf
#define MIN_ID_REPEAT_CMD               0x8321
#define ID_REPEAT_CMD                   0x8321
#define ID_REPEAT_CMD0                  0x8322
#define ID_REPEAT_CMD1                  0x8323
#define ID_REPEAT_CMD2                  0x8324
#define ID_REPEAT_CMD3                  0x8325
#define ID_REPEAT_CMD4                  0x8326
#define ID_REPEAT_CMD5                  0x8327
#define ID_REPEAT_CMD6                  0x8328
#define ID_REPEAT_CMD7                  0x8329
#define ID_REPEAT_CMD8                  0x832A
#define ID_REPEAT_CMD9                  0x832B
#define MAX_ID_REPEAT_CMD               0x832B

#define ID_DRAG_MOVE                    0x8330
#define ID_DRAG_COPY                    0x8331
#define ID_DRAG_CANCEL                  0x8332

#define ID_CANCEL                       0x8333
#define ID_MDI_CYCLE_FORWARD            0x8334
#define ID_PROJECT_COMBO                0x8335

#define ID_GRD_NEW                      0x4ed0
#define ID_GRD_DELETE                   0x4ed1
#define ID_GRD_MOVEUP                   0x4ed2
#define ID_GRD_MOVEDOWN                 0x4ed3

// IDs reserved for internal developer utilities
#define ID_INTERNAL_FIRST               0x8350
#ifdef  PERF_MEASUREMENTS
#define ID_DUMP_MEM                     (ID_INTERNAL_FIRST+0)
#endif
#define ID_INTERNAL_LAST                0x835f

// Shell groups

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Command Group IDs

// Below are groups that commands may go in.  (These used to be called "minor
//  groups"-- note that the IDs relating to "major groups" have been removed.)
//  Each command or menu is a member of one of the following groups. If the command 
//  or menu appears on a menu, its location is determined by the group's location (on 
//  which menu, and where in that menu).  See ide\exes\dev\entry.cpp.
// The Tools.Customize dialog lists commands by "category".  The category
//  is determined by which of the following groups the command is a member of,
//  and then what main menu that group is a part of.  Note that you can
//  have hidden menus, which allow you to group your commands in the
//  Tools.Customize.Keyboard dialog any way you like, without affecting the
//  visible menu structure.  To have a hidden menu, simply make sure that
//  all commands in all groups on that menu have the CT_NOMENU flag.
//  (See ide\include\cmdtable.h.)  This is set in your .cmd file, or
//  as an argument to CTheApp::AddCommand.

// Packages are free to create their own groups. The group ids should be in the same range as
// the packages command ids, though the two sets of numbers may overlap.

// If a group has its low bit set, then no separator will appear after it. Otherwise, a 
// separator will be inserted

// Currently groups for all packages appear here. Gradually, some of these group ids can be
// exported to other places

#define IDG_FILEOPS                  0x8000
#define IDG_FILESAVE                 0x8002
#define IDG_PRINT                    0x8005
#define IDG_MRU                      0x8006
#define IDG_EXIT                     0x8008
#define IDG_PROJ_MRU_MENU            0x800c
#define IDG_MSINOPEN                 0x8010
#define IDG_PRINTMAIN                        0x8012
#define IDG_MRU_MENU                             0x8014
#define IDG_PROJ_MRU                 0x8016
#define IDG_FILE_RENAME                          0x8018
#define IDG_FILE_NEW                 0x8019

#define IDG_UNDO                     0x8020
#define IDG_FILE_NEW_IN_PROJ         0x8021
#define IDG_CLIPBOARD                0x8023
#define IDG_FIND_1                   0x8025
#define IDG_FIND_COMBO               0x8026
#define IDG_GOTO                     0x8029
#define IDG_BOOKMARK                 0x802a
#define IDG_EDITPROPS                0x802c
#define IDG_BREAKPOINTS              0x8030
#define IDG_FORMATEDITOR             0x8032
#define IDG_SELECTALL                            0x8034
#define IDG_FIND_2                   0x8036
#define IDG_CLIPBOARD_2                          0x8038
#define IDG_GOTO_2                                   0x803b

#define IDG_PROJECTOPS               0x8040
#define IDG_BUILD                    0x8042
#define IDG_ERROR                    0x8044
#define IDG_DEPENDENCIES             0x8046
#define IDG_PROJECTCLOSE             0x8048
#define IDG_PROJECTOPS_STD           0x804a
#define IDG_PROJECT_ADD              0x804c
#define IDG_PROJECTOPS_ADD           0x804e
#define IDG_PROJECTOPS_SCAN          0x8050
#define IDG_PROJECTOPS_SCHM          0x8052

#define IDG_RESOURCENEW			     0x8060
#define IDG_RESOURCEIMPORTEXPORT     0x8062
#define IDG_SYMBOLS					 0x8064
#define IDG_VIEW_AS_POPUP            0x8066
#define IDG_NEWRES                   0x8068
#define IDG_CLASSWIZARD              0x806a
#define IDG_LAYOUTOPT                0x806c
#define IDG_WORKSPACEOPS             0x806e

#define IDG_DEBUGRUN                 0x8080
#define IDG_DEBUGSHOW                0x8082
#define IDG_DEBUGTHREADS             0x8084
#define IDG_CODECOVER                0x8086
#define IDG_COVERITEMS               0x8088
#define IDG_DEBUGOPTS                0x808a
#define IDG_DEBUGACTIVE              0x808c
// This group id is deliberately odd, because it is a no-break group
#define IDG_DEBUGSTEP1               0x808f 
#define IDG_DEBUGSTEP2               0x8090

#define IDG_TOOLOPS                  0x80a0
#define IDG_TOOLS_PROFILE                        0x80a1         // WinslowF Adding profiler
#define IDG_TOOLS_PACKAGES           0x80a2
#define IDG_OPTIONS                  0x80a4
#define IDG_TOOLS_SEARCH             0x80a6
#define IDG_USERTOOLS                0x80a8
#define IDG_MACROS                   0x80aa
#define IDG_BUTTONOPS                0x80ac     // Button customize context menu.
#define IDG_TOOLS_RUN                            0x80ae
#define IDG_TOOLS_RESOURCE                       0x80b0
#define IDG_TOOLS_WEB_1              0x80b3
#define IDG_TOOLS_WEB_2              0x80b4

#define IDG_WINDOWOPS                0x80c0
#define IDG_WINDOWALL                0x80c3
#define IDG_WINDOW_WINDOWS           0x80c4
#define IDG_WINDOWLIST               0x80c6
#define IDG_WINDOWNEXT                           0x80c8

#define IDG_HELPGENERAL              0x80e0
#define IDG_HELPDOCS                 0x80e1
#define IDG_MSONTHEWEB               0x80e4
#define IDG_HELPINSTALLED            0x80e6
#define IDG_HELP_SUBSETCOMBO         0x80e8
#define IDG_HELPABOUT                0x80ea
#define IDG_HELP_OPENCOMBO           0x80ec
#define IDG_HELP_CONTENTS            0x80f0
#define IDG_HELP_BROWSE              0x80f2
#define IDG_HELP_MSONTHEWEB_NEWS         0x80f4         // Items on the mow submenu
#define IDG_HELP_MSONTHEWEB_INFO         0x80f6
#define IDG_HELP_MSONTHEWEB_HOME         0x80f8
#define IDG_HELP_KEYBOARD            0x80fa
#define IDG_HELP_TOPLEVEL                0x80fc

#define IDG_IMAGEOPS                 0x8100
#define IDG_IMAGECOLORS              0x8102
#define IDG_IMAGEGRID                0x8104
#define IDG_IMAGETOOLS               0x8106
#define IDG_IMAGE_DEVICE             0x8108

#define IDG_LAYOUTOPS_1              0x8121
#define IDG_LAYOUTGRID               0x8122
#define IDG_LAYOUTTAB                0x8124
#define IDG_RULER_CONTEXT            0x8126
#define IDG_ALIGNEDGE                0x8128
#define IDG_ALIGNCENTER              0x812a
#define IDG_SPACEEVENLY              0x812c
#define IDG_CENTERDIALOG             0x812e
#define IDG_ARRANGEBUTTONS           0x8130
#define IDG_SAMESIZE                 0x8132
#define IDG_LAYOUTGROUP                          0x8134
#define IDG_LAYOUTZORDER             0x8136
#define IDG_LAYOUTGUIDES             0x8138
#define IDG_LAYOUTTEST               0x813a
#define IDG_LAYOUTAUTO               0x813c
#define IDG_LAYOUTOPS_2                          0x813e

#define IDG_BROWSE                  0x8140
#define IDG_KEY                     0x8142

// InfoViewer Groups 0x815x to 0x817F

#define IDG_IV_FILE_OPS             0x8150
#define IDG_IV_FILE_NEW_NOSEP       0x8151
#define IDG_IV_FILE_SAVE            0x8152
#define IDG_IV_FILE_PRINT           0x8154
#define IDG_IV_FILE_MISC            0x8156
#define IDG_IV_FILE_LAST            0x8157
#define IDG_IV_EDIT_OPS             0x815a
#define IDG_IV_EDIT_FIND            0x815c
#define IDG_IV_EDIT_BOOKMARKS       0x815c
#define IDG_IV_EDIT_BOOKMARKS_NOSEP 0x815d
#define IDG_IV_EDIT_LAST            0x815f
#define IDG_IV_VIEW_HOME            0x8160
#define IDG_IV_VIEW_LISTS           0x8162
#define IDG_IV_VIEW_LIST_OPS        0x8164
#define IDG_IV_VIEW_LAST            0x8167
#define IDG_IV_VIEW_WINDOWS         0x8169
#define IDG_IV_TOOLS_SEARCH         0x816a
#define IDG_IV_TOOLS_SYNC           0x816c
#define IDG_IV_TOOLS_SYNC_NOSEP     0x816d
#define IDG_IV_TOOLS_SUBSETS        0x816e
#define IDG_IV_TOOLS_LAST           0x816f
#define IDG_IV_WINDOW_MISC          0x8170
#define IDG_IV_WINDOW_CONTENTS      0x8172
#define IDG_IV_WINDOW_TOPIC         0x8174
#define IDG_IV_WINDOW_LAST          0x8177
#define IDG_IV_HELP_FIRST           0x817a
#define IDG_IV_HELP_CONTENTS        0x817a
#define IDG_IV_HELP_MISC            0x817c
#define IDG_IV_HELP_ABOUT           0x817f

#define IDG_PROJECT                 0x8180
#define IDG_PRJSYS_OPS              0x8182
#define IDG_BLDSYS_TOOL             0x8184
#define IDG_TARGET_COMBO            0x8186
#define IDG_BUILD_1                 0x8188
#define IDG_BUILD_DEBUGMENU         0x818a
#define IDG_BUILD_2                 0x818c
#define IDG_CONFIG_COMBO            0x818e

#define IDG_VIEW_WINDOWS            0x81a1
#define IDG_VIEW_MSIN1              0x81a2
#define IDG_VIEW_MSIN2              0x81a4
#define IDG_VIEW_MSIN3              0x81a6
#define IDG_MSIN_PRINT              0x81a8
#define IDG_VIEW_WINDOWS_2                      0x81aa
#define IDG_VIEW_WEB                0x81ad
#define IDG_VIEW_WEB_2              0x81ae
#define IDG_VIEW_DB                                 0x81b1
#define IDG_VIEW_DB_2               0x81b3
#define IDG_VIEW_DB_3               0x81b4
#define IDG_VIEW_REFRESH            0x81b6
#define IDG_VIEW_DEBUG_WINDOWS          0x81ba

#define IDG_SCC_POPUP               0x81c0
#define IDG_SCC_OPS1                0x81c2
#define IDG_SCC_OPS2                0x81c4
#define IDG_SCC_OPS3                0x81c6
#define IDG_SCC_RUN                 0x81c8

#define IDG_BROWSE_OPS              0x81e0
#define IDG_WIZBARU                 0x81e4
#define IDG_WIZBARU_CTL             0x81e6
#define IDG_WIZBARU_CMDS            0x81e8
#define IDG_AUTO_COMPLETE			0x81eA

#define IDG_INTERNAL_UTILS          0x8202
// the edit advanced submenu
#define IDG_MENU_EDIT_ADVANCED      0x8204
// Items on the edit advanced submenu
#define IDG_EDIT_ADVANCED           0x8206

#define IDG_TOOLBARS_LIST           0x8220
#define IDG_TOOLBARS_CUSTOMIZE      0x8222

#define IDG_VIEW_FULLSCREEN         0x8240
#define IDG_INSERT_0                0x8241
#define IDG_INSERT_1                0x8243
#define IDG_INSERT_2                0x8244
#define IDG_INSERT_3                0x8246
#define IDG_INSERT_4                0x8248
#define IDG_BUILD_SETTINGS          0x824a
#define IDG_VIEW_TOOLBARS           0x824c
#define IDG_TOOLS_REMOTE            0x824e
#define IDG_INSERT_FILE                         0x8250
#define IDG_DEBUG_GO                0x8251
#define IDG_DEBUG_STEPINTO          0x8253
#define IDG_DEBUG_RUNTOCURSOR       0x8255
#define IDG_DEBUG_ATTACH            0x8257
#define IDG_INSERT_RESOURCE			0x8258

// These groups dictate the ordering of the menus within the main menu bar
// Keep them numerically adjacent, so the popup main menu bar is drawn without separators
#define IDG_MENUBAR_FILE            0x8261
#define IDG_MENUBAR_EDIT            0x8263
#define IDG_MENUBAR_VIEW            0x8265
#define IDG_MENUBAR_INSERT          0x8267
#define IDG_MENUBAR_BUILD           0x8269     // includes build and debug
#define IDG_MENUBAR_TEST            0x826b
#define IDG_MENUBAR_RESOURCE        0x826d
#define IDG_MENUBAR_TOOLS           0x826f
#define IDG_MENUBAR_WINDOW          0x8271
#define IDG_MENUBAR_HELP            0x8273
#define IDG_MENUBAR_INTERNAL        0x8275
#define IDG_MENUBAR_PROJECT         0x8277

// Special kinds of menus
#define IDG_CUSTOMMENU              0x8280
#define IDG_MENUBAR_BAR             0x8282      // group containing the main menu bar. Shouldn't be used outside of shell

#define IDG_DEBUG_1                 0x82a0
#define IDG_DEBUG_2                 0x82a2
#define IDG_DEBUG_3                 0x82a4
#define IDG_DEBUG_4                 0x82a6
#define IDG_DEBUG_5                 0x82a8
#define IDG_DEBUG_6                 0x82aa
#define IDG_DEBUG_7                 0x82ac
#define IDG_DEBUG_8                 0x82ae
#define IDG_DEBUG_PERF              0x82b0

#define IDG_PROJECT_MODIFY                      0x82c0
#define IDG_PROJECT_WEB                         0x82c2
#define IDG_PROJECT_SCCS                        0x82c4
#define IDG_PROJECT_SETTINGS            0x82c6
#define IDG_PROJECT_INSERT                      0x82c8
#define IDG_PROJECT_LIST                        0x82ca
#define IDG_PROJECT_ADD_NEW         0x82cc
#define IDG_PROJECT_ADD_INSERT          0x82ce
#define IDG_PROJECT_ADD_DB                      0x82d0
#define IDG_PROJECT_ADD_COMPONENT   0x82d2
#define IDG_PROJECT_ADD_PROJECT                 0x82d4


// Espresso special command groups
// REVIEW(BrendanX): Better location for non-DevStudio EXE commands.
// Martynl: Can now move out to packages, since they can own their own menus.
#define IDG_GLOSSARY_MISC_OPS               0x8300
#define IDG_GLOSSARY_OPS                    0x8302
#define IDG_TRANSLATE_APPROVALSTATEFAILED   0x8304
#define IDG_TRANSLATE_APPROVALSTATE         0x8306
#define IDG_TRANSLATE_USERFLAG              0x8308
#define IDG_TRANSLATE_TESTWHAT              0x830a
#define IDG_TRANSLATE_ITEM                  0x830c
#define IDG_TRANSLATE_STATE                 0x830e
#define IDG_TRANSLATE_AUTO                  0x8310
#define IDG_TRANSLATE_TEST                  0x8312
#define IDG_DATABASEOPS                     0x8314
#define IDG_VIEW_EDITORS                    0x8316
#define IDG_VIEW_REPORTS                    0x8318
#define IDG_VIEW_REPORT_FILES               0x8320
#define IDG_HELPESPRESSOABOUT               0x8322
#define IDG_ESPRESSO_LAYOUT                 0x8324
#define IDG_ESPRESSO_TOOLS                  0x8326
#define IDG_ESPRESSO_MISC1                  0x8328
#define IDG_ESPRESSO_MISC2                  0x832a
#define IDG_ESPRESSO_MISC3                  0x832c
#define IDG_IMPORT_PROJECT                  0x832e
#define IDG_ESPESSO_DB                      0x8330
#define IDG_AUTOLAYOUT                      0x8332
#define IDG_REVERT                          0x8334
#define IDG_DIALOGLAYOUT                    0x8336
#define IDG_HELPESPRESSO                    0x8338
#define IDG_VIEW_EDITORWALK                 0x833a

#define IDG_TEST_DEBUG              0x8380
#define IDG_TEST_RECORD             0x8382
#define IDG_TEST_SCREENS            0x8384
#define IDG_TEST_DIALOGS            0x8386
#define IDG_TEST_DRIVER             0x8388

// HTM Insert.Html group
#define IDG_INSERTHTML                                          0x8389

/////////////////////////////////////////////////////////////////////
// VPROJ Commands
//
#define IDM_PROJITEM_FIRST              0x8400
#define IDM_PROJITEM_NEW                0x8400
#define IDM_PROJITEM_MOVE               0x8401
#define IDM_PROJITEM_COPY               0x8402
#define IDM_PROJITEM_DELETE             0x8403
#define IDM_PROJITEM_OPEN               0x8404
#define IDM_PROJITEM_ADD_RC             0x8416
#define IDM_PROJITEM_ADD_GROUP          0x8407
#define IDM_PROJITEM_ADD_SUBPROJ        0x8408
#define IDM_PROJITEM_SCAN               0x8409
#define IDM_PROJECT_SCAN_ALL            0x840a
#define IDM_PROJITEM_BUILD              0x840b
#define IDM_PROJITEM_COMPILE            0x840c
#define IDM_PROJITEM_PRINT              0x840d
#define IDM_PROJITEM_REBUILD_ALL        0x840e
#define IDM_PROJECT_EXECUTE             0x840f
#define IDM_PROJITEM_BATCH_BUILD        0x8410
#define IDM_PROJITEM_BUILD_NO_DEPS      0x8411
#define IDM_PROJITEM_CBUILD             0x841c
#define IDM_PROJITEM_CBUILD_NO_DEPS     0x841d
#define IDM_PROJITEM_CCOMPILE           0x841f
#define IDM_PROJECT_SET_AS_DEFAULT      0x8420
#define IDM_PROJECT_OPENDIALOG          0x8221
#define IDM_PROJITEM_LAST               0x84ff
#define IDM_PROJ_FIRST                  0x8500
#define IDM_PROJECT_SETTINGS            0x8500
#define IDM_PROJECT_STOP_BUILD          0x8501
#define IDM_PROJECT_CLOSEBSC            0x8502
#define IDM_PROJECT_TOOL_MECR           0x8503
#define IDM_PROJECT_CONFIGURATIONS      0x8504
#define IDM_PROJECT_TOOL_CMD_FIRST      0x8510
#define IDM_PROJECT_TOOL_CMD0           0x8510
#define IDM_PROJECT_TOOL_CMD1           0x8511
#define IDM_PROJECT_TOOL_CMD2           0x8512
#define IDM_PROJECT_TOOL_CMD3           0x8513
#define IDM_PROJECT_TOOL_CMD4           0x8514
#define IDM_PROJECT_TOOL_CMD5           0x8515
#define IDM_PROJECT_TOOL_CMD6           0x8516
#define IDM_PROJECT_TOOL_CMD_LAST       0x8516
#define IDM_PROJECT_CLEAN_ACTIVE        0x8518
#define IDM_PROJECT_TOOLS               0x8519
#define IDM_PROJECT_SETTINGS_POP        0x851a
#define IDM_PROJECT_BUILDGRAPH          0x851b
#define IDM_PROJECT_CLEAN               0x851c
#define IDM_DEBUG_SETTINGS              0x851d

// adding user-defined messages for PostMessage()
// this is to avoid buttons being depressed during duration of the cmd.
#define ID_PROJITEM_COMPILE             0x851e
#define ID_PROJITEM_BUILD               0x851f 
#define ID_PROJITEM_REBUILD_ALL         0x8520
#define ID_PROJITEM_BATCH_BUILD         0x8521

#define ID_TARGET_COMBO                 0x8522
#define IDM_PROJECT_SUBPROJECTS         0x8523
#define IDM_PROJECT_EXPORT              0x8524
#define IDM_PROJECT_WRITE               0x8525
#define ID_CONFIG_COMBO                 0x8526

#define IDM_PROJECT_LOAD                0x8527
#define IDM_PROJECT_UNLOAD              0x8528
#define IDM_BUILD_TOGGLE                                0x8529
#define ID_PROJITEM_CLEAN               0x852a
#define IDM_PROJITEM_CLEAN              0x852b
#define IDM_PROJECT_DEFAULT_PROJ        0x852c
#define IDM_PROJECT_SCC_GET             0x852D
#define IDM_DUMP_HEAP               	0x852E

#define IDM_PROJECT_EDIT               	0x852f

// Menu ids for exe finder tool in debug general page
#define IDM_BROWSE_FOR_DEBUG_EXE		0x8530
#define IDM_USE_TESTCONTAINER			0x8531
#define IDM_USE_WEBBROWSER				0x8532

// -> custom build tool output specification macros ->
#define IDM_BLDTOOL_DIRMACRO1           0x8550
#define IDM_BLDTOOL_DIRMACRO2           0x8551
#define IDM_BLDTOOL_DIRMACRO3           0x8552
#define IDM_BLDTOOL_DIRMACRO4           0x8553
#define IDM_BLDTOOL_DIRMACRO5           0x8554
#define IDM_BLDTOOL_DIRMACRO6           0x8555
#define IDM_BLDTOOL_DIRMACRO7           0x8556
#define IDM_BLDTOOL_DIRMACRO8           0x8557
#define IDM_BLDTOOL_FILEMACRO1          0x8558
#define IDM_BLDTOOL_FILEMACRO2          0x8559
#define IDM_BLDTOOL_FILEMACRO3          0x855a
#define IDM_BLDTOOL_FILEMACRO4          0x855b
#define IDM_BLDTOOL_FILEMACRO5          0x855c
#define IDM_BLDTOOL_FILEMACRO6          0x855d

#define IDM_BLDTOOL_FIRSTMACRO          IDM_BLDTOOL_DIRMACRO1
#define IDM_BLDTOOL_LASTMACRO           IDM_BLDTOOL_FILEMACRO6
// <- custom build tool output specification macros <-

#define IDM_INSERT_FILES_POP                    0x855e
#define IDM_CTL_PROJ_TARGET                     0x85F0
#define IDM_PROJ_LAST                           0x85FF

/////////////////////////////////////////////////////////////////////
// VRES Commands
//
#define IDM_FILE_SETINCLUDES        0x8600
#define IDM_FILE_INSTALLCONTROLS    0x8601
#define IDM_EDIT_SYMBOLS            0x8602
#define IDM_RESOURCE_NEW            0x8603
#define IDM_RESOURCE_OPEN           0x8604
#define IDM_RESOURCE_OPENBINARY     0x8605
#define IDM_RESOURCE_IMPORT         0x8606
#define IDM_RESOURCE_EXPORT         0x8607
#define IDMB_NEW_DIALOG             0x860A
#define IDMB_NEW_CURSOR             0x860B
#define IDMB_NEW_ICON               0x860C
#define IDMB_NEW_BITMAP             0x860D
#define IDMB_NEW_MENUBAR            0x860E
#define IDMB_NEW_STRINGTABLE        0x860F
#define IDMB_NEW_ACCTABLE           0x8610
#define IDMB_NEWRC                  0x8611
#define IDMB_NEW_VERSION            0x8612
#define ID_CANCEL_EDIT              0x8613
#define IDM_RESOURCE_ADD            0x8614
#define IDM_RESOURCE_NEWCOPY        0x8615
#define IDMB_NEW_TOOLBAR            0x8616
#define IDM_RESOURCENODE_SAVE       0x8617

// special commands for specific VRES resource types
#define IDM_FIND_STRING             0x8650
#define IDM_FIND_NEXT               0x8651
#define IDM_NEW_STRING              0x8652
#define IDM_NEW_ACCELERATOR         0x8653
#define IDM_NEW_STRINGBLOCK         0x8654
#define IDM_DELETE_STRINGBLOCK      0x8655
#define IDMY_TEST                   0x8656
#define IDM_SETTABORDER             0x8657
#define IDMY_ALIGNLEFT              0x8658
#define IDMY_ALIGNRIGHT             0x8659
#define IDMY_ALIGNTOP               0x865A
#define IDMY_ALIGNBOTTOM            0x865B
#define IDMY_SPACEACROSS            0x865C
#define IDMY_SPACEDOWN              0x865D
#define IDM_BUTTONRIGHT             0x865E
#define IDM_BUTTONBOTTOM            0x865F
#define IDMY_MAKESAMEWIDTH          0x8660
#define IDMY_MAKESAMEHEIGHT         0x8661
#define IDMY_MAKESAMEBOTH           0x8662
#define IDM_SIZETOTEXT              0x8663
#define IDM_GRIDSETTINGS            0x8664
#define IDMB_SHOWGRID               0x8665
#define IDM_VIEWASPOPUP             0x8666

#ifndef IMAGE_EDITOR_BROKEN_OUT
#define ID_FILE_NEW_BITMAP                      0x830A
#define IDM_TOOLBAR_WIZARD                      0x8608
#define IDMY_PICKCOLOR              0x8667
#define IDM_FLIPBSHH                0x8668
#define IDM_FLIPBSHV                0x8669
#define IDM_MAGNIFY                 0x866A
#define IDM_NEWDEVIMG               0x866B
#define IDM_OPENDEVIMG              0x866C
#define IDM_DELDEVIMG               0x866D
#define IDM_INVERTCOLORS            0x866E
#define IDM_TGLOPAQUE               0x866F
#define IDM_EDITCOLORS              0x8670
#define IDM_LOADCOLORS              0x8671
#define IDM_SAVECOLORS              0x8672
#define IDM_SETHOTSPOT              0x8673
#define IDM_ALIGNCVERT              0x8674
#define IDM_ALIGNCHORZ              0x8675
#define IDMY_CENTERVERT             0x8676
#define IDMY_CENTERHORZ             0x8677
#define IDM_INSTALLCONTROLS         0x8678
#define IDMY_CHECKHOTKEYS           0x8679
#define IDMZ_IMGSEPARATOR           0x8680
#define IDMX_ZOOMIN                 0x8681
#define IDMX_ZOOMOUT                0x8682
#define IDMX_SMALLERBRUSH           0x8683
#define IDMX_LARGERBRUSH            0x8684
#define IDMB_RECTTOOL               0x8685
#define IDMB_FRECTTOOL              0x8686
#define IDMB_ELLIPSETOOL            0x8687
#define IDMB_FELLIPSETOOL           0x8688
#define IDMZ_BRUSHTOOL              0x8689
#define IDMB_FILLTOOL               0x868A
#define IDMB_LINETOOL               0x868B
#define IDMB_PICKTOOL               0x868C
#define IDMX_PREVCOLOR              0x868D
#define IDMX_NEXTCOLOR              0x868E
#define IDMX_PREVECOLOR             0x868F
#define IDMB_3DRECTTOOL             0x8690
#define IDMX_NEXTECOLOR             0x8691
#define IDMX_TEXTTOOL               0x8692
#define IDMB_CURVETOOL              0x8693
#define IDMB_ORECTTOOL              0x8694
#define IDMB_ORNDRECTTOOL           0x8695
#define IDMB_OELLIPSETOOL           0x8696
#define IDMB_POLYGONTOOL            0x8697
#define IDMB_FPOLYGONTOOL           0x8698
#define IDMB_OPOLYGONTOOL           0x8699
#define IDMB_PICKRGNTOOL            0x869A
#define IDMX_OUTLINEBSH             0x869B
#define IDMB_AIRBSHTOOL             0x869C
#define IDMX_SMALLBRUSH             0x869D
#define IDMX_SEL2BSH                0x869E
#define IDMB_RNDRECTTOOL            0x869F
#define IDMB_FRNDRECTTOOL           0x86A1
#define IDMB_PENCILTOOL             0x86A2
#define IDMB_ERASERTOOL             0x86A3
#define IDMB_ZOOMTOOL               0x86A4
#define IDMB_CBRUSHTOOL             0x86A5
#define IDM_GRIDOPT                 0x86A6
#define IDMX_SHOWTILEGRID           0x86A7
#define IDM_ROT90                   0x86A8
#define IDMX_OTHERPANE              0x86A9
#define IDM_SHOWIMAGEGRID           0x86AA
#define IDM_WINDOWRECTTOOL          0x86B3
#endif  // IMAGE_EDITOR_BROKEN_OUT

#define IDMB_SHOWRULER              0x86AB
#define IDMB_TOGGLEGUIDES           0x86AC
#define IDM_GUIDE_ATTACH_LEFT       0x86AD
#define IDM_GUIDE_ATTACH_RIGHT      0x86AE
#define IDM_GUIDE_ATTACH_PROPORTIONAL 0x86AF
#define IDM_GUIDE_CLEAR             0x86B0
#define IDM_GUIDE_CLEAR_ALL         0x86B1
#define IDMY_FLIPDIALOG             0x86B4

#define IDM_CTL_POSFIRST                        0x86B5
    #define IDM_CTL_MOVELEFT                        0x86B5
    #define IDM_CTL_MOVEUP                          0x86B6
    #define IDM_CTL_MOVERIGHT                       0x86B7
    #define IDM_CTL_MOVEDOWN                        0x86B8
#define IDM_CTL_SIZEFIRST                       0x86B9
    #define IDM_CTL_SIZELEFT                        0x86B9
    #define IDM_CTL_SIZEUP                          0x86BA
    #define IDM_CTL_SIZERIGHT                       0x86BB
    #define IDM_CTL_SIZEDOWN                        0x86BC
#define IDM_CTL_POSLAST                         0x86BC

#define IDM_CTL_HOTSPOT_LOC         0x86F0
#define IDM_CTL_INSERT              0x86F1

// Espresso commands for dialog editor
#define IDM_REVERT_SIZES            0x86F2
#define IDM_REVERT_COORDINATES      0x86F3
#define IDM_REVERT_STYLES           0x86F4
#define IDMY_AUTOLAYOUT_GROW        0x86F5
#define IDMY_AUTOLAYOUT_OPTIMIZE    0x86F6
#define IDMY_AUTOLAYOUT_NORESIZE    0x86F7

// Wizard Bar New Event Handler command for Dialog controls
#define IDM_WIZBARU_EVENTS                      0x86F8
// Wizard Bar Help Id (Generic) here because makehm needs non-releaive hex number bobz
#define IDM_WIZBARU_HELP                        0x86F9

// insert Activex control w/out wrapper class - dialog editor only
#define IDM_CTL_INSERT_RES           0x86FA

/////////////////////////////////////////////////////////////////////
// VCPP Commands
//
// Many of the standard ones (FileOpen, etc) now use the MFC
// predefined IDs.
//
// REVIEW: This block is a hodge-podge of Edit, Debug, Browse,
//         Shell, Output window, obsolete, and shared commands.
//
#define IDM_FILE_SAVE_ALL               0x8700
#define ID_EDIT_FIND_DLG        0x8701 // invoke find dialog
#define IDM_EDIT_GREP                   0x8702
//#define IDM_EDIT_FINDMATCH              0x8703  note: now in pkgs\edit\resource.h
#define IDM_EDIT_READONLY               0x8704
#define ID_CUT_LINE             0x8705
#define ID_EDIT_FIND_NEXT       0x8706
#define ID_EDIT_FIND_PREV       0x8707
#define ID_EDIT_FIND_COMBO      0x8708 // activate find combo
#define IDM_ADDTOPROJECT                0x8709
#define IDM_EDIT_CLEAR_OUTPUT   0x870A
#define IDM_FILE_SEND                   0x870B
#define IDM_FILE_REVERT                 0x870C
#define IDM_FILE_LIST_DOCS              0x870D
#define IDM_EDIT_TOGGLE_CASE            0x870E
#define IDM_EDIT_TOGGLE_RE              0x870F
#define IDM_EDIT_TOGGLE_WORD            0x8710
#define IDM_EDIT_TOGGLE_SEARCH_ALL_DOCS 0x872E

// DEBUG
#define IDM_GOTO_LINE                   0x8711
#define IDM_VIEW_MIXEDMODE              0x8712
#define IDM_GOTO_DISASSY                0x8713
#define IDM_GOTO_SOURCE                 0x8714

// OUTPUT
#define IDM_GOTO_CURRENTERROR   0x8715
#define IDM_VIEW_NEXTERROR              0x8716
#define IDM_VIEW_PREVIOUSERROR  0x8717

// EDIT
#define IDM_VIEW_TOGGLETAG              0x8718
#define IDM_VIEW_NEXTTAG                0x8719
#define IDM_VIEW_PREVIOUSTAG    0x871A
#define IDM_VIEW_CLEARALLTAGS   0x871B
#define IDM_GOTO_FILE                   0x871C

// OUTPUT
#define IDM_VIEW_OUTTOOLS               0x871D

// EDIT (for now)
#define IDM_BOOKMARKS                   0x871E

// ??
#define IDM_GOTO_CODE                   0x871F

// BROWSE
#define IDM_BROWSE_GOTOREF              0x8720
#define IDM_BROWSE_GOTODEF              0x8721
#define IDM_BROWSE_NEXT                 0x8722
#define IDM_BROWSE_PREVIOUS             0x8723
#define IDM_BROWSE_OPEN                 0x8724
#define IDM_BROWSE_POP                  0x8725
#define IDM_BROWSE_SYM                  0x8726
#define IDM_BROWSE_OUTLINE              0x8727
#define IDM_BROWSE_FGRAPH               0x8728
#define IDM_BROWSE_CGRAPH               0x8729
#define IDM_BROWSE_FGRAPH_R             0x872A
#define IDM_BROWSE_CGRAPH_R             0x872B
#define IDM_BROWSE_QUERY                0x872C
#define IDM_BROWSE_FILES                0x872D
// note: 0x872E is used above by the shell

// DEBUG
#define IDM_DEBUG_RADIX                 0x872F
#define IDM_RUN_GO                      0x8730
#define IDM_RUN_RESTART                 0x8731
#define IDM_RUN_STOPDEBUGGING           0x8732
#define IDM_RUN_BREAK                   0x8733
#define IDM_RUN_TOCURSOR                0x8734
#define IDM_RUN_TRACEINTO               0x8735
#define IDM_RUN_STEPOVER                0x8736
#define IDM_RUN_STOPAFTERRETURN         0x8737
#define IDM_RUN_CALLS                   0x8738
#define IDM_RUN_SETBREAK                0x8739
#define IDM_RUN_QUICKWATCH              0x873A
#define IDM_RUN_EXCEPTIONS              0x873B
#define IDM_RUN_THREADS                 0x873C
//      IDM_RUN_FIBERS                          0x876B
#define IDM_RUN_ATTACH_TO_ACTIVE        0x873D
#define IDM_RUN_COVER_TOGGLE            0x873E
#define IDM_RUN_COVER_CLEAR             0x873F
#define IDM_RUN_SETNEXTSTMT             0x8740
#define IDM_RUN_CLEARALLBREAK           0x8741
#define IDM_RUN_SRC_TRACEINTO           0x8742
#define IDM_RUN_SRC_STEPOVER            0x8743
#define IDM_RUN_FLIPHACK                0x8744 // not used in release build
#define IDM_RUN_REMOTEDEBUGGER          0x8745
#define IDM_RUN_JIT_GO                  0x8746
#define IDM_DEBUG_ACTIVE                0x8747
#define IDM_DEBUG_UPDATEIMAGE			0x8748
#define IDM_RUN_TRACEFUNCTION           0x8749
#define IDM_RUN_SRC_TRACEFUNCTION       0x874A
#define IDM_RUN_ENABLEBREAK             0x874B
#define IDM_RUN_TOGGLEBREAK             0x874C
#define IDM_RUN_DISABLEALLBREAK         0x874D
#define IDM_MEMORY_NEXTFORMAT           0x874E
#define IDM_MEMORY_PREVFORMAT           0x874F
#define IDM_RUN_PROFILE                 0x8750  // WinslowF Adding profiler

// OUTPUT
#define IDM_OUTPUT_EXECTOOL             0x8750
#define IDM_OUTPUT_CLEAR                0x8751
#define IDM_OUTPUT_GOTO_TAG             0x8752

// DEBUG
#define IDM_DEBUG_SHOWIP                0x8753
#define IDM_DEBUG_GOTOSOURCE    0x8754
#define IDM_DBGSHOW_FLOAT               0x8755
#define IDM_DBGSHOW_STACKTYPES  0x8756
#define IDM_DBGSHOW_STACKVALUES 0x8757
#define IDM_DBGSHOW_SOURCE              0x8758
#define IDM_DBGSHOW_MEMBYTE             0x8759
#define IDM_DBGSHOW_MEMSHORT    0x875A
#define IDM_DBGSHOW_MEMLONG             0x875B
#define IDM_DBGSHOW_CODEBYTES           0x875C
#define IDM_DBGSHOW_MEMORY_BAR          0x875F

// EDIT
#define IDM_INDENT              0x8760
#define IDM_UNINDENT            0x8761

// DEBUG
#define IDM_DBGSHOW_VARS_BAR    0x8762

// EDIT
#define IDM_KEYBOARDINDENT      0x8763
#define IDM_KEYBOARDUNINDENT    0x8764

// SHELL
#define IDM_TOOL_STOP           0x8765
#define IDM_FIRST_TOOL          0x8766

// EDIT
#define IDM_MACRO_RECORD                0x8767
#define IDM_MACRO_PLAY                  0x8768
#define IDM_MACRO_PAUSE                 0x8769
#define IDM_MACRO_STOP                  0x876A

#define IDM_RUN_FIBERS                                  0x876B
// OUTPUT
//#define IDM_WINDOW_ERRORS               0x876C // AXE(CFlaat) doesn't appear to be needed

// ??
#define IDM_WINDOWCHILD         0x876D

// SHELL
#define IDM_HELP_MSPROD                         0x8774
#define IDM_HELP_CONTENTS                       0x8776
#define IDM_HELP_INDEX                          0x8777
#define IDM_HELP_SEARCH                         0x8778
#define IDM_HELP_EXTHELP                        0x8779
#define IDM_HELP_TIPOFTHEDAY            0x877b

// EDIT
#define IDM_REFORMAT                                    0x877f
#define IDM_UPPERCASE                   0x8780
#define IDM_NEWLINE                     0x8781
#define IDM_NEXTLINE                    0x8782
#define IDM_NEWLINEABOVE                0x8783
#define IDM_LOWERCASE                   0x8784
#define IDM_CLEARSELECTION              0x8785
#define IDM_SCROLL_LINEUP               0x8786
#define IDM_SCROLL_LINEDOWN             0x8787
#define ID_EDIT_FIND_NEXTCURRENTWORD    0x8788
#define ID_EDIT_FIND_PREVCURRENTWORD    0x8789
#define IDM_EDIT_DELETEWORDRIGHT        0x878a
#define IDM_EDIT_DELETELEFT             0x878b
#define IDM_FIND_IFDEF_UP               0x878c
#define IDM_FIND_IFDEF_DOWN             0x878d
#define IDM_EXTEND_FIND_IFDEF_UP        0x878e
#define IDM_EXTEND_FIND_IFDEF_DOWN      0x878f
#define IDM_CURSOR_STARTNEXTLINE        0x8790
#define ID_COL_SELECT                   0x8791
#define IDM_INSERT                                              0x8792

// DEBUG
#define ID_RUN_GO                               0x8793
#define ID_RUN_RESTART                          0x8794
#define ID_RUN_TOCURSOR                         0x8795
#define ID_RUN_TRACEINTO                        0x8796
#define ID_RUN_STEPOVER                         0x8797
#define ID_RUN_STOPAFTERRETURN                  0x8798
#define ID_RUN_SRC_TRACEINTO                    0x8799
#define ID_RUN_SRC_STEPOVER                     0x879a
#define ID_RUN_STOPDEBUGGING                    0x879b

// EDIT
#define IDM_VIEW_EPSDROPBOOKMARK                0x879c
#define IDM_VIEW_EPSNEXTBOOKMARK                0x879d
#define IDM_VIEW_BRF_DROPBOOKMARK1              0x879e
#define IDM_VIEW_BRF_DROPBOOKMARK2              0x879f
#define IDM_VIEW_BRF_DROPBOOKMARK3              0x87a0
#define IDM_VIEW_BRF_DROPBOOKMARK4              0x87a1
#define IDM_VIEW_BRF_DROPBOOKMARK5              0x87a2
#define IDM_VIEW_BRF_DROPBOOKMARK6              0x87a3
#define IDM_VIEW_BRF_DROPBOOKMARK7              0x87a4
#define IDM_VIEW_BRF_DROPBOOKMARK8              0x87a5
#define IDM_VIEW_BRF_DROPBOOKMARK9              0x87a6
#define IDM_VIEW_BRF_DROPBOOKMARK10             0x87a7
#define ID_RUN_TRACEFUNCTION                    0x87a8
#define ID_RUN_SRC_TRACEFUNCTION                0x87a9

// DEBUG
#define IDM_ACTIVATE_SRCFIRST                                   0x87B0
#define IDM_ACTIVATE_OUTPUT                     0x87B0
#define IDM_ACTIVATE_WATCH                      0x87B1
#define IDM_ACTIVATE_VARIABLES                  0x87B2
#define IDM_ACTIVATE_REGISTERS                  0x87B3
#define IDM_ACTIVATE_MEMORY                     0x87B4
#define IDM_ACTIVATE_CALLSTACK                  0x87B5
#define IDM_ACTIVATE_DISASSEMBLY                0x87B6
#define IDM_ACTIVATE_SRCLAST                                    0x87BF

#define IDM_TOGGLE_SRCFIRST                                             0x87B0
#define IDM_TOGGLE_OUTPUT                       0x87C0
#define IDM_TOGGLE_WATCH                        0x87C1
#define IDM_TOGGLE_VARIABLES                    0x87C2
#define IDM_TOGGLE_REGISTERS                    0x87C3
#define IDM_TOGGLE_MEMORY                       0x87C4
#define IDM_TOGGLE_CALLSTACK                    0x87C5
#define IDM_TOGGLE_DISASSEMBLY                  0x87C6
#define IDM_TOGGLE_SRCLAST                                              0x87CF

// Shell
#define IDM_HELP_MOW_FIRST                                              0x87D0
#define IDM_HELP_MOW_FREESTUFF                                  0x87D0
#define IDM_HELP_MOW_PRODUCTNEWS                                0x87D1
#define IDM_HELP_MOW_FAQ                                                0x87D2
#define IDM_HELP_MOW_ONLINESUPPORT                              0x87D3
#define IDM_HELP_MOW_DEVONLYHOMEPAGE                    0x87D4
#define IDM_HELP_MOW_SENDFEEDBACK                               0x87D5
#define IDM_HELP_MOW_BESTOFTHEWEB                               0x87D6
#define IDM_HELP_MOW_SEARCHTHEWEB                               0x87D7
#define IDM_HELP_MOW_WEBTUTORIAL                                0x87D8
#define IDM_HELP_MOW_MSHOMEPAGE                                 0x87D9
#define IDM_HELP_MOW_LAST                                               0x87DF

// DEBUG
#define IDM_CTL_LOCAL_FUNCS                     0x6400
#define IDM_CTL_WATCH_EDIT                      0x6401
#define IDM_CTL_WATCH_DELETE                    0x6402

// BROWSE
#define IDM_CTL_BROWSE_CLASS                    0x6403
#define IDM_CTL_BROWSE_FUNCMEM                  0x6404
#define IDM_CTL_BROWSE_DATAMEM                  0x6405
#define IDM_CTL_BROWSE_MACRO                    0x6406
#define IDM_CTL_BROWSE_TYPE                     0x6407
#define IDM_CTL_BROWSE_FUNCS                    0x6408
#define IDM_CTL_BROWSE_DATA                     0x6409
#define IDM_CTL_MEMORY_EDIT                     0x640A
#define IDM_CTL_VARS_FUNCS                      0x640B

// PARSER
#define IDM_PARSER_PARSE                0x87F0
#define IDM_PARSER_INFO                 0x87F1
#define IDM_PARSER_PARSEPROJ    0x87F2
#define IDM_PARSER_OPENDBASE    0x87F3
#define IDM_PARSER_CLOSEDBASE   0x87F4
#define IDM_PARSER_COPYTOFILE	0x87F5

// EDIT
#define ID_DELETE_LINE                  0x8800
#define ID_EDIT_REPLACE_DLG             0x8801
#define ID_NEXT_WINDOW          0x8802
#define ID_PREV_WINDOW          0x8803
#define ID_TOGGLE_RESTORE               0x8806
#define ID_SEARCH_AGAIN                 0x8807

#define IDM_CURSOR_CHARLEFT     0x8808
#define IDM_CURSOR_CHARRIGHT    0x8809
#define IDM_CURSOR_LINEUP               0x880A
#define IDM_CURSOR_LINEDOWN             0x880B
#define IDM_CURSOR_WORDLEFT             0x880C
#define IDM_CURSOR_WORDRIGHT    0x880D
#define IDM_CURSOR_SCREENUP             0x880E
#define IDM_CURSOR_SCREENDOWN   0x880F
#define IDM_EPSILON_ESCAPE              0x8810
#define IDM_CURSOR_LINESTART    0x8811
#define IDM_CURSOR_LINEEND              0x8812
#define IDM_CURSOR_FILESTART    0x8813
#define IDM_CURSOR_FILEEND              0x8814
#define IDM_CURSOR_BEGINPANE    0x8815
#define IDM_CURSOR_ENDPANE              0x8816
#define IDM_CURSOR_LEFTPANE             0x8817
#define IDM_CURSOR_RIGHTPANE    0x8818
#define IDM_CURSOR_BEGINLINE    0x8819
#define IDM_CURSOR_BRIEFHOME    0x881A
#define IDM_CURSOR_BRIEFEND             0x881B
#define IDM_TABIFYREGION        0x881C
#define IDM_UNTABIFYREGION      0x881D
#define IDM_HIGHLIGHTREGION             0x881E
#define IDM_EPSILON_INDENT_REGION       0x881F
#define IDM_EPSILON_KILLREGION  0x8820
#define IDM_EDIT_DELETEBRIEFEOLN        0x8821
#define IDM_EPSILON_FWDPARAGRAPH        0x8822
#define IDM_EPSILON_BCKPARAGRAPH        0x8823
#define IDM_EPSILON_FWDSENTENCE 0x8824
#define IDM_EPSILON_BCKSENTENCE 0x8825
#define IDM_EPSILON_DELSENTENCE 0x8826
#define IDM_EDIT_DELETEBRIEFBOLN        0x8827
#define IDM_EPSILON_TRANSPOSECH 0x8828
#define IDM_EPSILON_TRANSPOSEWD 0x8829
#define IDM_EPSILON_TRANSPOSELN 0x882A
#define IDM_EPSILON_CAPWORD             0x882B
#define IDM_EPSILON_LOWERWORD   0x882C
#define IDM_EPSILON_UPPERWORD   0x882D
#define ID_CMD_INVALID                  0x882E
#define IDM_NEWLINEBELOW                0x882F
#define ID_EDIT_REDOCHANGES             0x8830
#define ID_EDIT_UNDOCHANGES             0x8831
#define IDM_EPSILON_MARKPARAGRAPH       0x8832
#define IDM_EPSILON_CENTERWINDOW        0x8833
#define IDM_EPSILON_DELHORZSPACE        0x8834
#define IDM_EPSILON_DELBLANKLINES       0x8835
#define IDM_EPSILON_APPENDNEXTKILL      0x8839
#define IDM_EPSILON_TOINDENT    0x883A
#define IDM_FORMAT_REGION               0x883B
#define IDM_INDENT_UNDER                0x883C
#define IDM_REGEX_SRCH_FWD              0x883E
#define IDM_REGEX_SRCH_BCK              0x883F

// DEBUG
#define IDM_BP_CURRENT_ADDR                             0x8840
#define IDM_BP_CURRENT_LINE                             0x8841
#define IDM_BP_CURRENT_FUNC                             0x8842
#define IDM_BP_ADVANCED                                 0x8843

// EDIT
#define ID_EDIT_RE_REPLACE                              0x8844
#define IDM_SEARCH_ISEARCH_FWD                  0x8845
#define IDM_SEARCH_ISEARCH_BACK                 0x8846
#define IDM_SEARCH_ISEARCH_REFWD        0x8847
#define IDM_SEARCH_ISEARCH_REBACK       0x8848
#define IDM_ISEARCH_ISEARCH_FWD         0x8849
#define IDM_ISEARCH_ISEARCH_BACK        0x884A
#define IDM_ISEARCH_TOGGLE_CASE         0x884B
#define IDM_ISEARCH_TOGGLE_WORD         0x884C
#define IDM_ISEARCH_TOGGLE_REGEX        0x884D
#define ID_QUOTED_INSERT                                0x884E
#define ID_CHANGE_PANE_UP                               0x884F
#define ID_CHANGE_PANE_DOWN                             0x8850
#define ID_CHANGE_PANE_LEFT                             0x8851
#define ID_CHANGE_PANE_RIGHT                    0x8852
#define ID_DELETE_ROW_UP                                0x8853
#define ID_DELETE_ROW_DOWN                              0x8854
#define ID_DELETE_COL_LEFT                              0x8855
#define ID_DELETE_COL_RIGHT                             0x8856
#define ID_LINE_TO_TOP                                  0x8857
#define ID_LINE_TO_BOTTOM                               0x8858

// SHELL??
// UNUSED	0x8859 - 0x8868

// EDIT
#define ID_EDIT_BACKWARD_LEVEL                  0x8869
#define ID_EDIT_FORWARD_LEVEL                   0x8870
#define ID_EDIT_KILL_LEVEL                      0x8871
#define ID_EDIT_BACKWARD_KILL_LEVEL             0x8872

// RES??
#define IDM_STRING_TO_RESOURCE                  0x8873

// EDIT ("Advanced" menu)
#define IDM_MENU_EDIT_ADVANCED                                  0x8874

// HTM commands squatting in the edit package
#define IDM_HTMLX_SCUSEWIZARD                                   0x8875
#define IDM_HTMLX_SCINSERTCONTROL                               0x8876
#define IDM_HTMLX_SCINSERTHTMLLAYOUT                    0x8877
#define IDM_HTMLX_SCSCRIPTWIZARD                                0x8878
#define IDM_HTMLX_SCEDITCONTROL                         0x8879
#define IDM_HTMLX_SCEDITDESIGNCONTROL                   0x887a
#define IDM_HTMLX_SCEDITHTMLLAYOUT                              0x887b
#define IDM_HTMLX_INTOHTMLSUBMENU                               0x887c

// AUTO_COMPLETE command squatting in the edit package
#define ID_LIST_MEMBERS					0x8880
#define ID_PARAMETER_INFO				0x8881
#define ID_TYPE_INFO					0x8882
#define ID_COMPLETE_WORD				0x8883

// DEBUG (Edit and Continue)
#define IDM_DEBUG_UPDATEIMAGE_TOGGLE	0x8890

/////////////////////////////////////////////////////////////////////
// MSIN Commands
//
#define IDM_MSIN                        0x8a00
#define ID_SEARCH                       0x8a01
#define ID_QUERY_RESULTS                0x8a02
#define ID_SELECT_TITLE                 0x8a10
#define ID_SELECT_SUBSET                0x8a11
#define ID_FIND_FIND                    0x8a12
#define ID_FIND_KEYWORD                 0x8a13
#define ID_RESULTS_FIND                 0x8a14
#define ID_RESULTS_HISTORY              0x8a15
#define ID_BACK                         0x8a16
#define ID_NEXT                         0x8a17
#define ID_PREV                         0x8a18
#define ID_FIND_NEXT                    0x8a19
#define ID_BOOK_NEXT                    0x8a1a
#define ID_FIND_PREV                    0x8a1b
#define ID_BOOK_PREV                    0x8a1c
#define ID_PRINT_TOPIC                  0x8a1d
#define ID_PRINT_TOC                    0x8a1e
#define ID_SYNC                         0x8a1f
#define ID_TOGGLE_HIGHLIGHTS            0x8a20
#define ID_ADD_BOOKMARK                 0x8a21
#define ID_EDIT_BOOKMARK                0x8a22
#define ID_WHATSNEW                     0x8a24
#define ID_COPYTOPIC                    0x8a25
#define ID_SELECTION_RIGHT              0x8a26
#define ID_SELECTION_LEFT               0x8a27
#define ID_SELECTION_UP                 0x8a28
#define ID_SELECTION_DOWN               0x8a29
#define ID_SELECTION_ENTIRE             0x8a2a
#define ID_SEEALSO                      0x8a2b
#define ID_FIRST_TOPIC                  0x8a2c
#define ID_HOME_SCREEN                  0x8a2d
#define ID_EDIT_HELPBOOKMARKS           0x8a2e
#define ID_EDIT_ANNOTATION              0x8a30
#define ID_ADD_ANNOTATION               0x8a31
#define ID_NEXT_BOOKMARK                0x8a32
#define ID_PREV_BOOKMARK                0x8a33
#define ID_NEXT_LIST                    0x8a34
#define ID_PREV_LIST                    0x8a35
#define ID_SHOWBAR                      0x8a37
#define ID_SHOWNSR                      0x8a38
#define ID_DISPLAY_OPTIONS              0x8a39
#define ID_FIND_OPTIONS                 0x8a3a
#define ID_DEFINE_SUBSETS               0x8a3b
#define ID_SWITCHMVB                    0x8a3c
#define ID_NEXT_BOOKMARK_LIST           0x8a3d
#define ID_PREV_BOOKMARK_LIST           0x8a3e
#define ID_HELP_QUERY_RESULTS           0x8a3f
#define ID_HELP_BOOKMARK_LIST           0x8a40
#define ID_NEXT_JUMP                    0x8a41
#define ID_SEEALSOP                     0x8a42
#define ID_BACKP                        0x8a43
#define ID_HELPP                        0x8a44
#define ID_TOPIC                        0x8a45
#define ID_MSIN_OPEN                    0x8a46
#define ID_MSIN_ABOUT                   0x8a47
#define ID_NEXT_HIGHLIGHT               0x8b48
#define ID_PREV_HIGHLIGHT               0x8b49
#define ID_MSIN_PRINT_SETUP             0x8b4a
#define ID_HELP_KEYBOARD                0x8a4b
#define IDM_TOGGLE_TOPICWIN             0x8a4c
#define ID_SET_SUBSET                   0x8a4d
#define ID_EDIT_SELECTALL_POPUP         0x8a4e
#define ID_PREV_JUMP                    0x8a4f
#define ID_HELP_WEB                     0x8a50
#define ID_HELP_ESPRESSO                0x8a51

//////////////////////////////////////////////////////////
//
// New IV command defines here.
//
#define ID_IV_HOME                         MIN_IV_COMMAND_ID + 1
#define ID_IV_URL_SF                       MIN_IV_COMMAND_ID + 2
#define ID_IV_OPEN_URL                     MIN_IV_COMMAND_ID + 3 
#define ID_IV_SYNC                         MIN_IV_COMMAND_ID + 4 
#define ID_IV_BACK                         MIN_IV_COMMAND_ID + 5 
#define ID_IV_NEXT                         MIN_IV_COMMAND_ID + 6 
#define ID_IV_PREV                         MIN_IV_COMMAND_ID + 7
#define ID_IV_FORWARD                      MIN_IV_COMMAND_ID + 8
#define ID_IV_REFRESH                      MIN_IV_COMMAND_ID + 9
#define ID_IV_STOP_DOWNLOAD                MIN_IV_COMMAND_ID + 10
#define ID_IV_LIST_BOOKMARKS               MIN_IV_COMMAND_ID + 11
#define ID_IV_ADD_BOOKMARK                 MIN_IV_COMMAND_ID + 12
#define ID_IV_PREV_BOOKMARK                MIN_IV_COMMAND_ID + 13
#define ID_IV_NEXT_BOOKMARK                MIN_IV_COMMAND_ID + 14
#define ID_IV_DEF_SUBSET                   MIN_IV_COMMAND_ID + 15
#define ID_IV_SELECT_SUBSET                MIN_IV_COMMAND_ID + 16
#define ID_IV_SELECT_TOC_SUBSET_COMBO      MIN_IV_COMMAND_ID + 17
#define ID_IV_SEARCH                       MIN_IV_COMMAND_ID + 18
#define ID_IV_RESULTS_LIST                 MIN_IV_COMMAND_ID + 19
#define ID_IV_TOPIC_WINDOW                 MIN_IV_COMMAND_ID + 20
#define ID_IV_NEXT_LISTITEM                MIN_IV_COMMAND_ID + 21
#define ID_IV_PREV_LISTITEM                MIN_IV_COMMAND_ID + 22
#define ID_IV_QUERY                        MIN_IV_COMMAND_ID + 23
#define ID_IV_KEYWORD_INDEX                MIN_IV_COMMAND_ID + 24
#define ID_IV_SEARCH_LIST                  MIN_IV_COMMAND_ID + 25
#define ID_IV_NEXT_SEARCH                  MIN_IV_COMMAND_ID + 26
#define ID_IV_PREV_SEARCH                  MIN_IV_COMMAND_ID + 27
#define ID_IV_QUERY_LIST                   MIN_IV_COMMAND_ID + 28
#define ID_IV_NEXT_QUERY                   MIN_IV_COMMAND_ID + 29
#define ID_IV_PREV_QUERY                   MIN_IV_COMMAND_ID + 30
#define ID_IV_RESULTS_LIST_SHOWDETAIL      MIN_IV_COMMAND_ID + 31
#define ID_IV_RESULTS_LIST_SHOWLIST        MIN_IV_COMMAND_ID + 32
#define ID_IV_PRINT_TOC                    MIN_IV_COMMAND_ID + 33
#define ID_IV_HELP_CONTENTS                MIN_IV_COMMAND_ID + 34
#define ID_IV_HELP_ABOUT                   MIN_IV_COMMAND_ID + 35
#define ID_IV_DOWNLOADCOMPLETE             MIN_IV_COMMAND_ID + 36
#define ID_IV_FONT_SIZE                    MIN_IV_COMMAND_ID + 37
#ifdef __DEVHELP98__
#define ID_HELP98_KEYWORDS					MIN_IV_COMMAND_ID + 38
#endif

//////////////////////////////////////////////////////////////
//
// The following commands are handled by the MSIN macro system
//
#define IDM_MSIN_MACRO_FIRST    0x8b00  //ID of first one
#define IDM_NEXT_HIGHLIGHT              0x8b00
#define IDM_PREV_HIGHLIGHT              0x8b01
#define IDM_ANNOTATION                  0x8b02
#define IDM_MSIN_MACRO_LAST             0x8b02  //ID of last one

/////////////////////////////////////////////////////////////////////
// Classview/Wizard Commands
//
// TODO(davidga): reconcile this with the CLASSVIEW_COMMAND_ID range
// possible creating a CLASSVIEW_STRING_ID range
#define IDM_CLASSVIEW_FIRST             (MIN_CLASSVIEW_COMMAND_ID)
#define IDM_ADDCLASS                    (IDM_CLASSVIEW_FIRST+0)
#define IDM_ADDATLCOMP                  (IDM_CLASSVIEW_FIRST+1)
#define IDM_GALLERY                     (IDM_CLASSVIEW_FIRST+2)
#define IDMY_CLASSWIZARD                (IDM_CLASSVIEW_FIRST+3)
#define IDM_CLASSVIEW_LAST              (IDM_CLASSVIEW_FIRST+3)


#define IDM_WIZBARU_CLASS               (IDM_CLASSVIEW_LAST+1)
#define IDM_WIZBARU_OBJECT              (IDM_CLASSVIEW_LAST+2)
#define IDM_WIZBARU_TARGET              (IDM_CLASSVIEW_LAST+3)
#define IDM_WIZBARU_ACTION              (IDM_CLASSVIEW_LAST+4)

//  wizard bar commands that can be assigned to keys
#define ID_WIZBARU_COMBO_CLASS          (IDM_CLASSVIEW_LAST+5)  // activate the class combo
#define ID_WIZBARU_COMBO_OBJECT         (IDM_CLASSVIEW_LAST+6)
#define ID_WIZBARU_COMBO_TARGET         (IDM_CLASSVIEW_LAST+7)
#define ID_WIZBARU_CTL_ACTION           (IDM_CLASSVIEW_LAST+8)
 
#define IDM_WIZBARU_COMMANDS            (IDM_CLASSVIEW_LAST+9)  // menu entry for tools customize

#define IDM_WIZBARU_CMD_FIRST            (IDM_CLASSVIEW_LAST+10)    // first generic or shared WB command

#define IDM_WIZBARU_DEFAULT               (IDM_WIZBARU_CMD_FIRST)  // current appropriate default action for client

#define IDM_WIZBARNCB_GOFUNCIMPL          (IDM_WIZBARU_CMD_FIRST + 1)
#define IDM_WIZBARNCB_GOFUNCNEXT          (IDM_WIZBARU_CMD_FIRST + 2)
#define IDM_WIZBARNCB_GOFUNCPREV          (IDM_WIZBARU_CMD_FIRST + 3)
#define IDM_WIZBARNCB_DELETE              (IDM_WIZBARU_CMD_FIRST + 4)
#define IDM_WIZBARNCB_CREATE              (IDM_WIZBARU_CMD_FIRST + 5)
#define IDM_WIZBARNCB_NEW_FUNC            (IDM_WIZBARU_CMD_FIRST + 6)
#define IDM_WIZBARNCB_GOCLASSDEF          (IDM_WIZBARU_CMD_FIRST + 7)

#define IDM_WIZBARU_CMD_LAST              (IDM_WIZBARNCB_GOCLASSDEF)    // last generic or shared WB command 
// NOTE: non-default Wizard Bar command ids added by clients to menus  are defined in the client's package

// note: Gallery/Explorer uses MANY id's. It uses them for its context menu's which
//        it builds from the Explorer/Shell's menus.

#define IDM_CREATENEWCLASS              (IDM_WIZBARU_CMD_LAST + 1)
#define IDM_NEWATLCOMP                  (IDM_WIZBARU_CMD_LAST + 2)
#define IDM_CREATENEWFORM               (IDM_WIZBARU_CMD_LAST + 3)
#define IDM_ADDINTERFACE				(IDM_WIZBARU_CMD_LAST + 4)
#define IDM_ADDCONNECTPOINT				(IDM_WIZBARU_CMD_LAST + 5)
#define IDM_ADDEVENT					(IDM_WIZBARU_CMD_LAST + 6)
#define IDM_CLVW_PLACEHOLDER			(IDM_WIZBARU_CMD_LAST + 7)
#define IDM_CLVW_PLACEHOLDER2			(IDM_WIZBARU_CMD_LAST + 8)
#define IDM_ADDFORM                     (IDM_WIZBARU_CMD_LAST + 9)

#define IDM_GALLERY_FIRST               (IDM_ADDFORM + 1)
#define IDM_GAL_ADDTOPROJECT            (IDM_GALLERY_FIRST+ 1)
#define IDM_GAL_RUN                     (IDM_GAL_ADDTOPROJECT+ 1)
#define IDM_GAL_OPEN                    (IDM_GAL_RUN+ 1)
#define IDM_GAL_INSTALL                 (IDM_GAL_OPEN+ 1)
#define IDM_GAL_SETPATH                 (IDM_GAL_INSTALL+ 1)
#define IDM_GAL_NEWFOLDER               (IDM_GAL_SETPATH+ 1)
#define IDM_GAL_CUT                     (IDM_GAL_NEWFOLDER+ 1)
#define IDM_GAL_COPY                    (IDM_GAL_CUT+ 1)
#define IDM_GAL_PASTE                   (IDM_GAL_COPY+ 1)
#define IDM_GAL_DISCONNECT              (IDM_GAL_PASTE+ 1)
#define IDM_GAL_CREATESHORTCUT          (IDM_GAL_DISCONNECT+ 1)
#define IDM_GAL_DELETE                  (IDM_GAL_CREATESHORTCUT+ 1)
#define IDM_GAL_RENAME                  (IDM_GAL_DELETE+ 1)
#define IDM_GAL_HELP                    (IDM_GAL_RENAME+ 1)
#define IDM_GAL_SEARCH                  (IDM_GAL_HELP+ 1)
#define IDM_EXPLORERCONTEXTMENU_BASE    (IDM_GAL_SEARCH + 1)
#define IDM_EXPLORERCONTEXTMENU_0       (IDM_EXPLORERCONTEXTMENU_BASE + 0)

// ... go all the way to end
// stolen id's for gallery
#define IDM_GAL_LAST                    (MAX_CLASSVIEW_COMMAND_ID)
#define IDM_EXPLORERCONTEXTMENU_LAST    (MAX_CLASSVIEW_COMMAND_ID)

// Commands which are dynamically bound to actions via the CDynCT instance in CTheApp.
#define IDM_DYNAMIC_BASE                0x8d00
#define IDM_DYNAMIC_LAST                0x8eff

/////////////////////////////////////////////////////////////////////
// Project Window Commands
//
#define IDM_PROJECT_FIRST                               0x8c00
#define IDM_PROJECT_LAST                                0x8cff

#define IDM_PRJWIN_FIRST            0x8f00
#define IDM_ADDFOLDER               (IDM_PRJWIN_FIRST+0)
#define IDM_ADDSUBFOLDER            (IDM_PRJWIN_FIRST+1)
#define IDM_ADDMENU                 (IDM_PRJWIN_FIRST+2)
#define IDM_ADDSELECTEDNODE         (IDM_PRJWIN_FIRST+3)
#define IDM_ADDPANE                 (IDM_PRJWIN_FIRST+4)
#define IDM_DELETEPANE              (IDM_PRJWIN_FIRST+5)
#define IDM_ADDPROVIDEDNODE1        (IDM_PRJWIN_FIRST+6)
#define IDM_ADDPROVIDEDNODE2        (IDM_PRJWIN_FIRST+7)
#define IDM_ADDPROVIDEDNODE3        (IDM_PRJWIN_FIRST+8)
#define IDM_ADDPROVIDEDNODE4        (IDM_PRJWIN_FIRST+9)
#define IDM_ADDPROVIDEDNODE5        (IDM_PRJWIN_FIRST+10)
#define IDM_ADDPROVIDEDNODE6        (IDM_PRJWIN_FIRST+11)
#define IDM_ADDPROVIDEDNODE7        (IDM_PRJWIN_FIRST+12)
#define IDM_ADDPROVIDEDNODE8        (IDM_PRJWIN_FIRST+13)
#define IDM_ADDPROVIDEDNODE9        (IDM_PRJWIN_FIRST+14)
#define IDM_ADDPROVIDEDNODE10       (IDM_PRJWIN_FIRST+15)
#define IDM_ADDPROVIDEDNODE11       (IDM_PRJWIN_FIRST+16)
#define IDM_ADDPROVIDEDNODE12       (IDM_PRJWIN_FIRST+17)
#define IDM_ADDPROVIDEDNODE13       (IDM_PRJWIN_FIRST+18)
#define IDM_ADDPROVIDEDNODE14       (IDM_PRJWIN_FIRST+19)
#define IDM_ADDPROVIDEDNODE15       (IDM_PRJWIN_FIRST+20)
#define IDM_ADDPROVIDEDNODE16       (IDM_PRJWIN_FIRST+21)
#define IDM_SLOB_DEFAULTCMD         (IDM_PRJWIN_FIRST+22)
#define IDM_TOGGLE_TITLETIPS        (IDM_PRJWIN_FIRST+23)
#define IDM_OPEN_WORKSPACE          (IDM_PRJWIN_FIRST+24)
#define IDM_CLOSE_WORKSPACE         (IDM_PRJWIN_FIRST+25)
#define IDM_TOGGLE_SHOWTITLE        (IDM_PRJWIN_FIRST+26)
#define IDM_PANE1_VISIBLE           (IDM_PRJWIN_FIRST+27)
#define IDM_PANE2_VISIBLE           (IDM_PRJWIN_FIRST+28)
#define IDM_PANE3_VISIBLE           (IDM_PRJWIN_FIRST+29)
#define IDM_PANE4_VISIBLE           (IDM_PRJWIN_FIRST+30)
#define IDM_PANE5_VISIBLE           (IDM_PRJWIN_FIRST+31)
#define IDM_PANE6_VISIBLE           (IDM_PRJWIN_FIRST+32)
#define IDM_PANE7_VISIBLE           (IDM_PRJWIN_FIRST+33)
#define IDM_PANE8_VISIBLE           (IDM_PRJWIN_FIRST+34)
#define IDM_PANE9_VISIBLE           (IDM_PRJWIN_FIRST+35)
#define IDM_INSERT_PROJECTS         (IDM_PRJWIN_FIRST+36)
#define IDM_SAVE_WORKSPACE          (IDM_PRJWIN_FIRST+37)
#define IDM_FILE_RENAME             (IDM_PRJWIN_FIRST+38)       // Rename command
#define IDM_PROJECT_COMBO_ALIAS     (IDM_PRJWIN_FIRST+39)       // Activates Proj Combo
#define IDM_ADD_NEW_PROJECT         (IDM_PRJWIN_FIRST+40)

// Begin Class View specific
#define IDM_CLSVIEW_GOTO_DECL       (IDM_PRJWIN_FIRST+50)
#define IDM_CLSVIEW_DERIVED_CLASSES (IDM_PRJWIN_FIRST+51)
#define IDM_CLSVIEW_BASE_CLASSES    (IDM_PRJWIN_FIRST+52)
#define IDM_CLSVIEW_REFS            (IDM_PRJWIN_FIRST+53)
#define IDM_CLSVIEW_CALLED_FUNCS    (IDM_PRJWIN_FIRST+54)
#define IDM_CLSVIEW_CALLING_FUNCS   (IDM_PRJWIN_FIRST+55)
#define IDM_CLSVIEW_ADD_MF          (IDM_PRJWIN_FIRST+56)
#define IDM_CLSVIEW_ADD_MV          (IDM_PRJWIN_FIRST+57)
#define IDM_GROUPBYACCESS           (IDM_PRJWIN_FIRST+58)
#define IDM_CLSVIEW_ADD_TO_GALLERY  (IDM_PRJWIN_FIRST+59)
#define IDM_CLSVIEW_ADD_IPROP       (IDM_PRJWIN_FIRST+60)
#define IDM_CLSVIEW_ADD_IMETH       (IDM_PRJWIN_FIRST+61)
#define IDM_CLSVIEW_ADD_IEVENT      (IDM_PRJWIN_FIRST+62)
// End Class View specific
// Begin Source Control specific
#define IDM_SCC_FIRST               (IDM_PRJWIN_FIRST+80)
#define IDM_SCC_POP                 (IDM_SCC_FIRST+0)
#define IDM_SCC_GET                 (IDM_SCC_FIRST+1)
#define IDM_SCC_OUT                 (IDM_SCC_FIRST+2)
#define IDM_SCC_IN                  (IDM_SCC_FIRST+3)
#define IDM_SCC_UNOUT               (IDM_SCC_FIRST+4)
#define IDM_SCC_ADD                 (IDM_SCC_FIRST+5)
#define IDM_SCC_REMOVE              (IDM_SCC_FIRST+6)
#define IDM_SCC_HISTORY             (IDM_SCC_FIRST+7)
#define IDM_SCC_DIFF                (IDM_SCC_FIRST+8)
#define IDM_SCC_PROPS               (IDM_SCC_FIRST+9)
#define IDM_SCC_SHARE               (IDM_SCC_FIRST+10)
#define IDM_SCC_ADMIN               (IDM_SCC_FIRST+11)
#define IDM_SCC_REFRESH             (IDM_SCC_FIRST+12)
#define IDM_SCC_LAST                (IDM_SCC_FIRST+19) // (IDM_PRJWIN_FIRST+99)
// End Source Control specific
#define IDM_PRJWIN_LAST                 0x8FFF  // (IDM_PRJWIN_FIRST + 255)

// Fortran commands
#define ID_FORMAT_EDITOR            MIN_FORTRAN_COMMAND_ID

// LangCPP commands
#define IDM_WIZBARCPP_CMD_FIRST           (MIN_LANGCPP_COMMAND_ID)    // first in range of wizard bar actions

#define IDM_WIZBARCPP_GOFUNCDECL          (IDM_WIZBARCPP_CMD_FIRST+0)
#define IDM_WIZBARCPP_GOFUNCIMPLBASE      (IDM_WIZBARCPP_CMD_FIRST+1)
#define IDM_WIZBARCPP_GOFUNCDECLBASE      (IDM_WIZBARCPP_CMD_FIRST+2)
#define IDM_WIZBARCPP_NEW_WINMSG          (IDM_WIZBARCPP_CMD_FIRST+3)
#define IDM_WIZBARCPP_NEW_VIRT            (IDM_WIZBARCPP_CMD_FIRST+4)
#define IDM_WIZBARCPP_DELETE_FUNCDEF      (IDM_WIZBARCPP_CMD_FIRST+5)
#define IDM_WIZBARCPP_GODIALOG            (IDM_WIZBARCPP_CMD_FIRST+6) 

// editor specific commands
#define IDM_WIZBARCPP_OPEN_INCLUDE        (IDM_WIZBARCPP_CMD_FIRST+7)

#define IDM_WIZBARCPP_CMD_LAST            (IDM_WIZBARCPP_OPEN_INCLUDE + 1)    // last in range of wizard bar actions (plus buffer)

#define IDM_ADD_MF                        (IDM_WIZBARCPP_CMD_LAST+1)
#define IDM_ADD_MV                        (IDM_WIZBARCPP_CMD_LAST+2)
#define IDM_ADD_TO_GALLERY                (IDM_WIZBARCPP_CMD_LAST+3)

// Aut1 commands
// For Tools | Macro Dialog
#define IDM_TOOLS_MACRO						(MIN_AUT1_COMMAND_ID + 1)
// For Recording Macros
#define IDM_RECORD_STOP						(MIN_AUT1_COMMAND_ID + 2)
#define IDM_RECORD_PAUSE					(MIN_AUT1_COMMAND_ID + 3)
// For Context Menu editting Macro Files
#define IDM_ADD_MACRO						(MIN_AUT1_COMMAND_ID + 4)
#define IDM_RELOAD_MACRO_FILE				(MIN_AUT1_COMMAND_ID + 5)
#define IDM_RELOAD_MACROS					(MIN_AUT1_COMMAND_ID + 6)
#define IDM_REMOVE_MACRO_COMMANDS			(MIN_AUT1_COMMAND_ID + 7)

/////////////////////////////////////////////////////////////////////////////
// Dynamic commands added by macros and add-ins
// Submenu of file menu on which the two categories "Macros" and "Addins" reside
#define IDM_MACRO_DYNCMDS					(MIN_AUT1_COMMAND_ID + 8)
#define IDM_ADDIN_DYNCMDS					(MIN_AUT1_COMMAND_ID + 9)
#define IDG_MACRODYNCMDS					(MIN_AUT1_COMMAND_ID + 10)
#define IDG_ADDINDYNCMDS					(MIN_AUT1_COMMAND_ID + 11)

//For Temporary Macros
#define IDM_RECTEMPMACRO					(MIN_AUT1_COMMAND_ID + 12)
#define IDM_PLAYTEMPMACRO					(MIN_AUT1_COMMAND_ID + 13)


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//  Status Bar Indicator IDs
//      Note that several "standard" ones (ID_INDICATOR_OVR, ID_INDICATOR_CAPS,
//      ID_INDICATOR_NUM) are defined by MFC.

#define ID_INDICATOR_LNCOL              20192
#define ID_INDICATOR_READ               20193
#define ID_INDICATOR_POSITION           20194
#define ID_INDICATOR_SIZE               20195
#define ID_INDICATOR_ZOOM               20196
#define ID_INDICATOR_CLOCK              20197
#define ID_INDICATOR_OFFSET             20198
#define ID_INDICATOR_EXTENT             20199

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Help IDs and ranges

#ifndef HID_BASE_RESOURCE
#define HID_BASE_RESOURCE           0x20000 // Afx Standard
#endif

// SRC
#define HID_WND_TEXT                    0x28001
#define HID_WND_OUTPUT                  0x28002
#define HID_WND_MEMORY                  0x28003
#define HID_WND_VARS                    0x28004
#define HID_WND_WATCH                   0x28005
#define HID_WND_CALL_STACK              0x28006
#define HID_WND_REGISTERS               0x28007
#define HID_WND_DISASSEMBLY             0x28008
// SRC range
#define MIN_SOURCE_WINDOW_ID            HID_WND_TEXT
#define MAX_SOURCE_WINDOW_ID            HID_WND_DISASSEMBLY

// RES
#define HID_WND_RESOURCE_BROWSER        0x28010
#define HID_WND_DIALOG                  0x28011
#define HID_WND_MENU                    0x28012
#define HID_WND_BITMAP                  0x28013
#define HID_WND_CURSOR                  0x28014
#define HID_WND_ICON                    0x28015
#define HID_WND_STRING                  0x28016
#define HID_WND_ACCEL                   0x28017
#define HID_WND_VERSION                 0x28018
#define HID_WND_BINARY                  0x28019
#define HID_WND_IMAGE_TOOLS             0x2801a
#define HID_WND_DIALOG_TOOLS            0x2801b
#define HID_WND_BITMAP_TEXTTOOL         0x2801c
#define HID_WND_TOOLBAR_EDIT            0x2801d
#define HID_WND_RESOURCEVIEW            0x2801e
// RES range
#define MIN_RES_WINDOW_ID               HID_WND_RESOURCE_BROWSER
#define MAX_RES_WINDOW_ID               HID_WND_RESOURCEVIEW

// PRJ
#define HID_WND_PROJECT_WORKSPACE       0x28020
// PRJ range
#define MIN_WORKSPACE_WINDOW_ID         HID_WND_PROJECT_WORKSPACE
#define MAX_WORKSPACE_WINDOW_ID         HID_WND_PROJECT_WORKSPACE

// MSIN
#define HID_WND_TOPIC                   0x28030
// MSIN range
#define MIN_MSIN_WINDOW_ID              HID_WND_TOPIC
#define MAX_MSIN_WINDOW_ID              HID_WND_TOPIC

// IV
#define HID_WND_TOPIC2                  0x28035
#define HID_WND_INFOVIEW                0x28036
// IV range
#define MIN_IV_WINDOW_ID                HID_WND_TOPIC2
#define MAX_IV_WINDOW_ID                HID_WND_INFOVIEW

// MS-Test
#define HID_WND_TEST_LOCALS             0x28040
#define HID_WND_TEST_WATCH              0x28041
// MS-Test range
#define MIN_TEST_WINDOW_ID              HID_WND_TEST_LOCALS
#define MAX_TEST_WINDOW_ID              HID_WND_TEST_WATCH

// BLD
#define HID_WND_FILEVIEW                0x28050
// BLD range
#define MIN_BLD_WINDOW_ID               HID_WND_FILEVIEW
#define MAX_BLD_WINDOW_ID               HID_WND_FILEVIEW

// ClassView
#define HID_WND_CLASSVIEW               0x28060
// ClassView range
#define MIN_CLASSVIEW_WINDOW_ID         HID_WND_CLASSVIEW
#define MAX_CLASSVIEW_WINDOW_ID         HID_WND_CLASSVIEW

// DTG/ENT
#define HID_WND_DATAVIEW                0x28070
// DTG/ENT range
#define MIN_DTGENT_WINDOW_ID            HID_WND_DATAVIEW
#define MAX_DTGENT_WINDOW_ID            HID_WND_DATAVIEW

// In a number of places, several dialogs share a single dialog template.
// The following dialog IDs are used solely for generating separate help
// contexts for such cases, and do not actually have any dialog templates
// associated with them.
//
#define IDD_REGISTER_CONTROL        0x3990
#define IDD_PROFILE_BROWSE          0x2010
#define IDD_GOTOADDR                0x6978
#define IDD_FILE_SAVE_AS            0x7005
#define IDD_FILE_NEW                0x7801

#define IDD_IMAGE_NEWICONIMAGE      0x0F20
#define IDD_IMAGE_NEWCURSORIMAGE    0x0F21
#define IDD_IMAGE_OPENICONIMAGE     0x0F22
#define IDD_IMAGE_OPENCURSORIMAGE   0x0F23
#define IDD_IMAGE_COLOR             0x0F30
#define IDD_DIR_PICKER              0x0F50
#define IDD_DIR_FINDFILE            0x0F51
#define IDD_ADDTOOL_FILE_OPEN       0x0F60
#define IDD_PROJ_ADDDEP             0x0F70
#define IDD_PROJ_SELECT_TARGET      0x0F71

#define IDDP_BROWSE_DEFREF          0x8101
#define IDDP_BROWSE_FILEOUTLINE     0x8102
#define IDDP_BROWSE_BASEGRAPH       0x8103
#define IDDP_BROWSE_DERVGRAPH       0x8104
#define IDDP_BROWSE_CALLGRAPH       0x8105
#define IDDP_BROWSE_REVCALLGRAPH    0x8106

#define IDDP_RESOURCE_CURSOR        0x8121
#define IDDP_RESOURCE_ICON          0x8122

//  Needed for compatibility when converting 4.x .mdp files
//
//  Streams used in a project's MSF file.  Since we use 512 byte pages, these
//  numbers must be  between 0 and 256
//
#define PROJ_MSF_PROJINTINFO    0   // Internal project info
#define PROJ_MSF_PROJEXTINFO    1   // External project info
#define PROJ_MSF_WORKSPACE      2
#define PROJ_MSF_VCPPINFO       3
#define PROJ_MSF_BRSINFO        4
#define PROJ_MSF_PROJSYSINFO    5   // new project system info
#define PROJ_MSF_PROJSCCINFO    6   // project source control info
#define PROJ_MSF_HELPINFO       7   // help system info
#define PROJ_MSF_ENT                    9       // Enterprise package info
#define PROJ_MSF_DBASE          50  // no-compile browse info
				    // this must be the last one
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//  Package notification IDs.
//      REVIEW(BrendanX):  Use GUIDs.

#define PN_NEW_PROJECT          1
#define PN_OPEN_PROJECT         2
#define PN_CLOSE_PROJECT        3
#define PN_BEGIN_BUILD          4
#define PN_END_BUILD            5
#define PN_START_BSCMAKE        6
#define PN_END_BSCMAKE          7
#define PN_CHANGE_PROJECT       8
#define PN_CHANGE_BSC           9
#define PN_CHANGE_CONFIG        10
#define PN_CHANGE_PLATFORM      11
#define PN_FULL_SCREEN          12
#define PN_FULL_SCREEN_END      13
#define PN_ADD_FILE             14
#define PN_DEL_FILE             15
#define PN_ADD_TARGET           16
#define PN_DEL_TARGET           17
#define PN_DBASE_CHANGE         18
#define PN_WORKSPACE_INIT       19
#define PN_WORKSPACE_CLOSE      20
#define PN_BEGIN_MULTIOP        21
#define PN_END_MULTIOP          22
#define PN_ADD_FILESET          23
#define PN_DEL_FILESET          24
#define PN_ADD_RCFILE           25
#define PN_DEL_RCFILE           26
#define PN_EXTCHNG_PROJECT      27
#define PN_BLD_SETTINGS_CHANGED 29
#define PN_END_PARSEPROJECT     30
#define PN_BEGIN_SCCOP          31
#define PN_END_SCCOP            32
#define PN_DIRECTORIES_CHANGED  33
#define PN_RENAME_FILE          34
#define PN_DEL_PROJECTDEP       35
#define PN_DOCKMGR_LOADED       36

// Notifcatations after PN_FIRST_VETO are actaully queries to the 
// packages. If any package returns FALSE for these notfications,
// the sender should abort whatever its trying to do.  For example,
// the build systems sends PN_QUERY_BEGIN_BUILD before starting
// a build.  The debugger, when it gets this and debugging is active
// brings up its warning message box and returns false if the user
// hits cancel

#define PN_FIRST_VETO               100
#define PN_QUERY_NEW_PROJECT        100
#define PN_QUERY_OPEN_PROJECT       101
#define PN_QUERY_CLOSE_PROJECT      102
#define PN_QUERY_BEGIN_BUILD        103
#define PN_QUERY_CHANGE_CONFIG      104
#define PN_QUERY_NEW_WORKSPACE      105
#define PN_QUERY_OPEN_WORKSPACE     106
#define PN_QUERY_CLOSE_WORKSPACE    107
#define PN_QUERY_DEL_FILE           108
#define PN_QUERY_DEL_FILESET        109
#define PN_BLD_INIT_COMPLETE        110
#define PN_QUERY_RENAME_FILE        111

#endif  // __IDS_H__
