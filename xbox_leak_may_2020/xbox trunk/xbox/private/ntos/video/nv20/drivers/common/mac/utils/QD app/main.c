#include <Types.h>
#include <Quickdraw.h>
#include <Devices.h>
#include <Dialogs.h>
#include <Fonts.h>
#include <LowMem.h>
#include <Menus.h>
#include <QDOffscreen.h>
#include <Resources.h>
#include <Scrap.h>
#include <Sound.h>
#include <TextUtils.h>
#include <ToolUtils.h>
#include <Windows.h>

#include "debug.h"

#if DEBUG_BUILD

#define	kludgeKeyMap(i)		(*((/*volatile*/ unsigned long*) (0x174+(i*4))))
#define	OptionIsDown		((kludgeKeyMap(1)&0x0004)!=0)
#define	ShiftIsDown			((kludgeKeyMap(1)&0x0001)!=0)
#define	CapsLockIsDown		((kludgeKeyMap(1)&0x0002)!=0)
#define	ControlIsDown		((kludgeKeyMap(1)&0x0008)!=0)
#define	CmdIsDown			((kludgeKeyMap(1)&0x8000)!=0)

#endif

/* Miscellaneous definitions */

#define RECTINSET		48
#define RRECT_CURVATURE	50
#define ARC_STARTANGLE	135
#define ARC_DEGREES		90

#define DESTWIND_ID			128
#define SOURCEWIND_ID		129
#define STATUSWIND_ID		130
#define DESTWINDREFCON		DESTWIND_ID
#define SRCWINDREFCON		SOURCEWIND_ID
#define STATUSWINDREFCON	STATUSWIND_ID
#define ERRORALERT_ID		128
#define ABOUTDLOG_ID		129
#define BOUNDRECTDLOG_ID	130
#define OTHERRECTDLOG_ID	131
enum {
	OK_BRDITM=1,
	DEFLTBTN_BRDITM,
	TITLE_BRDITM,
	HORIZ_BRDITM,
	HTITLE_BRDITM,
	VERT_BRDITM,
	VTITLE_BRDITM
};

/* Error values, closely tied to application error string indices */
#define NONQDMISC_ERR			1

#define ERRORSTR_ID			128		// Error strings
#define NONQDMISC_NDX			NONQDMISC_ERR
#define CANTGETWIND_NDX			2
#define CANTGETMENU_NDX			3
#define CANTGETDLOG_NDX			4
#define CANTGETPICT_NDX			5
#define CANTGETCTABLE_NDX		6
#define CANTGETGWORLD_NDX		7
#define CANTGETREGION_NDX		8
#define CANTGETPIXPAT_NDX		9
#define CANTGETNVBOARD_NDX		10
#define NEEDMILLIONS_NDX		11
#define DESTNOTONNV_NDX			12

#define PARAMSTR_ID			130		// Parameterized strings
#define DESTLOC_NDX				1
#define SOURCELOC_NDX			2
#define WINDTITLE_NDX			3
#define UNNAMEDPICT_NDX			4
#define UNNAMEDPIXPAT_NDX		5
#define TIMING_NDX				6
#define ADDR_NDX				7

#define PICT_BASEID			128		// Base resource IDs for PICTs

/* Definitions for the main application menus */

#define APPLEMENU_ID		128
enum {
	ABOUT_ITEM = 1
};
static MenuHandle gAppleMenu;

#define FILEMENU_ID			129
enum {
	QUIT_ITEM = 1
};
static MenuHandle gFileMenu;

#define EDITMENU_ID			130
enum {
	UNDO_ITEM = 1,
	EDITSEP1_ITEM,
	CUT_ITEM,
	COPY_ITEM,
	PASTE_ITEM,
	CLEAR_ITEM
};
static MenuHandle gEditMenu;

#define TESTMENU_ID			131
enum {
	RUN_ITEM = 1,
	NOREFRESH_ITEM,
	NOCURSORHIDE_ITEM,
	TESTSEP1_ITEM,
	COPYBITS_ITEM,
	LINES_ITEM,
	TEXT_ITEM,
	FRAMESHAPE_ITEM,
	PAINTSHAPE_ITEM,
	ERASESHAPE_ITEM,
	INVERTSHAPE_ITEM,
	PATFILLSHAPE_ITEM,
	TESTSEP2_ITEM,
	RECTANGLE_ITEM,
	REGION_ITEM,
	OVAL_ITEM,
	ROUNDRECT_ITEM,
	ARC_ITEM,
	POLYGON_ITEM,
	TESTSEP3_ITEM,
	BOUNDRECT_ITEM,
	RGNSHAPE_ITEM
};
static MenuHandle gTestMenu;
static UInt8 gNoRefresh = false;
static UInt8 gNoCursorHide = false;
static UInt8 gTest = PAINTSHAPE_ITEM;
static UInt8 gShape = RECTANGLE_ITEM;

#define DESTMENU_ID			132
enum {
	DESTLOCATION_ITEM = 1,
	DESTDIM_ITEM,
	DESTIMAGE_ITEM,
	DESTDITHER_ITEM,
	DESTSEP1_ITEM,
	FGCOLOR_ITEM,
	BGCOLOR_ITEM,
	SWAPFGBG_ITEM,
	RESETCOLORS_ITEM,
	DESTSEP2_ITEM,
	PENSIZE_ITEM,
	PENMODE_ITEM,
	PENPATTERN_ITEM,
	BGPATTERN_ITEM,
	FILLPATTERN_ITEM,
	DESTSEP3_ITEM,
	USECLIPRGN_ITEM,
	CLIPRGN_ITEM
};
static MenuHandle gDestMenu;
static UInt8 gDitherDest = false;
static UInt8 gSwapFgBg = false;
static UInt8 gUseClipRgn = false;

#define COPYBITSMENU_ID		133
enum {
	USESEPSRC_ITEM = 1,
	SRCLOCATION_ITEM,
	SRCDIM_ITEM,
	SRCIMAGE_ITEM,
	SRCDITHER_ITEM,
	COPYBITSSEP1_ITEM,
	TRANSFERMODE_ITEM,
	SOURCERECT_ITEM,
	DESTRECT_ITEM,
	USEMASKRGN_ITEM,
	MASKRGN_ITEM
};
static MenuHandle gCopyBitsMenu;
static UInt8 gSepSrc = false;
static UInt8 gDitherSrc = false;
static UInt8 gUseMaskRgn = false;

#define LINESMENU_ID		134
enum {
	HORIZONTAL_ITEM = 1,
	VERTICAL_ITEM,
	DIAGONAL_ITEM,
	SLANTED_ITEM
};
static MenuHandle gLinesMenu;
static UInt8 gLines = HORIZONTAL_ITEM;

#define TEXTMENU_ID			135
enum {
	TEXTMODE_ITEM = 1,
	STRING_ITEM,
	TEXTSEP1_ITEM,
	FONT_ITEM,
	SIZE_ITEM
};
static MenuHandle gTextMenu;

#define EXPMENU_ID			136
enum {
	LUTDITHER_ITEM = 1,
	SPATIALDITHER_ITEM
};
static MenuHandle gExpMenu;

#define RGNSHAPEMENU_ID		140
// RgnShape, ClipRgn, and MaskRgn menus use identical items
enum {
	TRIOVALRGN_ITEM = 1,
	RRECTRGN_ITEM,
	RECTHOLERGN_ITEM,
	MEDRECTRGN_ITEM,
	SMALLRECTRGN_ITEM,
	EMPTYRGN_ITEM,
	NUM_REGIONS
};
static MenuHandle gRgnShapeMenu;
static UInt8 gRgnShape = TRIOVALRGN_ITEM;

#define CLIPRGNMENU_ID		141
// RgnShape, ClipRgn, and MaskRgn menus use identical items
static MenuHandle gClipRgnMenu;
static UInt8 gClipRgn = RRECTRGN_ITEM;

#define MASKRGNMENU_ID		142
// RgnShape, ClipRgn, and MaskRgn menus use identical items
static MenuHandle gMaskRgnMenu;
static UInt8 gMaskRgn = RECTHOLERGN_ITEM;

/* Definitions for the hierarchical menus */

#define DESTLOCMENU_ID		150
// DestLoc and SrcLoc menus use identical items
enum {
	WINDOW_ITEM = 1,
	OFFSCRN1BW_ITEM,
	OFFSCRN8G_ITEM,
	OFFSCRN8C_ITEM,
	OFFSCRN16C_ITEM,
	OFFSCRN24C_ITEM
};
static MenuHandle gDestLocMenu;
static UInt8 gDestLoc = WINDOW_ITEM;

#define DESTDIMMENU_ID		151
// DestDim and SrcDim menus use identical items
enum {
	DIM_512X384_ITEM = 1,
	DIM_600X450_ITEM,
	DIM_640X480_ITEM,
	DIM_1024X768_ITEM
};
static MenuHandle gDestDimMenu;
static UInt8 gDestDim = DIM_600X450_ITEM;

#define DESTIMAGEMENU_ID	152
// DestImage and SrcImage menus use identical items (from PICT resources)
static MenuHandle gDestImageMenu;
static UInt16 gDestImage = 1;	// First picture is default

#define FGCOLORMENU_ID		153
// FgColor and BgColor menus use identical items
enum {
	BLACK_ITEM = 1,
	WHITE_ITEM,
	RED_ITEM,
	GREEN_ITEM,
	BLUE_ITEM,
	CYAN_ITEM,
	MAGENTA_ITEM,
	YELLOW_ITEM,
	GRAY53_ITEM,
	HILITE_ITEM,
	RANDOM_ITEM,
	OTHERCOLOR_ITEM
};
static MenuHandle gFgColorMenu;
static UInt8 gFgColor = BLACK_ITEM;
static RGBColor gFgOtherRGB = { 0, 0, 0 };
static RGBColor gFgSaveRGB = { 0, 0, 0 };

#define BGCOLORMENU_ID		154
// FgColor and BgColor menus use identical items
static MenuHandle gBgColorMenu;
static UInt8 gBgColor = WHITE_ITEM;
static RGBColor gBgOtherRGB = { 0xFFFF, 0xFFFF, 0xFFFF };
static RGBColor gBgSaveRGB = { 0xFFFF, 0xFFFF, 0xFFFF };

#define PENSIZEMENU_ID		155
enum {
	PS_1X1_ITEM = 1,
	PS_2X2_ITEM,
	PS_4X4_ITEM,
	PS_8X8_ITEM,
	PS_16X16_ITEM,
	PS_OTHER_ITEM
};
static MenuHandle gPenSizeMenu;
static UInt8 gPenSize = PS_1X1_ITEM;

#define PENMODEMENU_ID		156
enum {
	PATCOPY_ITEM = 1,
	PATOR_ITEM,
	PATXOR_ITEM,
	PATBIC_ITEM,
	NOTPATCOPY_ITEM,
	NOTPATOR_ITEM,
	NOTPATXOR_ITEM,
	NOTPATBIC_ITEM,
	PATCOPYPLUSTRANSP_ITEM,
	PENMODESEP1_ITEM,
	PM_PLUSHILITE_ITEM
};
static MenuHandle gPenModeMenu;
static UInt8 gPenMode = PATCOPY_ITEM;
static UInt8 gPMPlusHilite = false;

#define PENPATMENU_ID		157
// PenPat, BgPat, and FillPat menus use identical items
enum {
	BLACKPAT_ITEM = 1,
	WHITEPAT_ITEM,
	GRAY50PAT_ITEM,
	LTGRAYPAT_ITEM,
	DKGRAYPAT_ITEM,
	HSTRIPESPAT_ITEM,
	VSTRIPESPAT_ITEM,
	DSTRIPESPAT_ITEM,
	NUMNIBBLESPAT_ITEM,
	PATSEP1_ITEM,
	FIRSTPIXPAT_ITEM
};
static MenuHandle gPenPatMenu;
static UInt8 gPenPat = BLACKPAT_ITEM;
static UInt8 gPixPat = FIRSTPIXPAT_ITEM;

#define BGPATMENU_ID		158
// PenPat, BgPat, and FillPat menus use identical items
static MenuHandle gBgPatMenu;
static UInt8 gBgPat = WHITEPAT_ITEM;

#define FILLPATMENU_ID		159
// PenPat, BgPat, and FillPat menus use identical items
static MenuHandle gFillPatMenu;
static UInt8 gFillPat = GRAY50PAT_ITEM;

#define SRCLOCMENU_ID		170
// DestLoc and SrcLoc menus use identical items
static MenuHandle gSrcLocMenu;
static UInt8 gSrcLoc = WINDOW_ITEM;

#define SRCDIMMENU_ID		171
// DestDim and SrcDim menus use identical items
static MenuHandle gSrcDimMenu;
static UInt8 gSrcDim = DIM_600X450_ITEM;

#define SRCIMAGEMENU_ID		172
// DestImage and SrcImage menus use identical items (from PICT resources)
static MenuHandle gSrcImageMenu;
static UInt16 gSrcImage = 2;	// First picture is default

#define XFERMODEMENU_ID		173
enum {
	X_SRCCOPY_ITEM = 1,
	X_SRCOR_ITEM,
	X_SRCXOR_ITEM,
	X_SRCBIC_ITEM,
	X_NOTSRCCOPY_ITEM,
	X_NOTSRCOR_ITEM,
	X_NOTSRCXOR_ITEM,
	X_NOTSRCBIC_ITEM,
	X_ADDOVER_ITEM,
	X_ADDPIN_ITEM,
	X_SUBOVER_ITEM,
	X_SUBPIN_ITEM,
	X_ADMAX_ITEM,
	X_ADMIN_ITEM,
	X_BLEND_ITEM,
	X_TRANSPARENT_ITEM,
	X_HILITE_ITEM,
	XFERMODESEP1_ITEM,
	X_PLUSDITHER_ITEM
};
static MenuHandle gXferModeMenu;
static UInt8 gXferMode = X_SRCCOPY_ITEM;
static UInt8 gXPlusDither = false;

#define SRCRECTMENU_ID		174
// SrcRect and DestRect menus use identical items
enum {
	TOP50_ITEM = 1,
	BOTTOM50_ITEM,
	LEFT50_ITEM,
	RIGHT50_ITEM,
	TOP99_ITEM,
	BOTTOM99_ITEM,
	LEFT99_ITEM,
	RIGHT99_ITEM,
	FULLSIZE_ITEM,
	OTHERRECT_ITEM
};
static MenuHandle gSrcRectMenu;
static UInt8 gSrcRect = TOP50_ITEM;
static Rect gSrcOtherRect = { 100, 100, 300, 300 };	// Should be safe

#define DESTRECTMENU_ID		175
static MenuHandle gDestRectMenu;
static UInt8 gDestRect = BOTTOM50_ITEM;
static Rect gDestOtherRect = { 100, 100, 300, 300 };	// Should be safe

#define TEXTMODEMENU_ID		190
enum {
	T_SRCCOPY_ITEM = 1,
	T_SRCOR_ITEM,
	T_SRCXOR_ITEM,
	T_SRCBIC_ITEM,
	T_NOTSRCCOPY_ITEM,
	T_NOTSRCOR_ITEM,
	T_NOTSRCXOR_ITEM,
	T_NOTSRCBIC_ITEM,
	T_ADDOVER_ITEM,
	T_ADDPIN_ITEM,
	T_SUBOVER_ITEM,
	T_SUBPIN_ITEM,
	T_ADMAX_ITEM,
	T_ADMIN_ITEM,
	T_BLEND_ITEM,
	T_TRANSPARENT_ITEM,
	T_GRAYISHTEXTOR_ITEM,
	TEXTMODESEP1_ITEM,
	T_PLUSHILITE_ITEM,
	T_PLUSMASK_ITEM
};
static MenuHandle gTextModeMenu;
static UInt8 gTextMode = T_SRCOR_ITEM;
static UInt8 gTPlusHilite = false;
static UInt8 gTPlusMask = false;


/* Miscellaneous definitions */

#define MAX_PIXPATS		40		/* Maximum number of PixPats we handle */
#define BEEPTICKS		3		/* Number of ticks for beep duration */
#define TESTTICKS		4*60	/* Number of ticks to run each test */

#define UNIMPTRAP		0xA89F
#define NQDMISCTRAP		0xABC3


/* Other global variables visible in this file only */

static UInt8 gQuitFlag = false;	// Set to true when the user wants to quit
static WindowPtr gDestWind;	// Pointer to destination window 
static GWorldPtr gDestPictBufGW = nil;	// Pointer to dest pict buffer GWorld
static GWorldPtr gDestGW = nil;	// Pointer to dest offscreen GWorld
static WindowPtr gSrcWind;	// Pointer to separate source window
static GWorldPtr gSrcPictBufGW = nil;	// Pointer to source pict buffer GWorld
static GWorldPtr gSrcGW = nil;	// Pointer to source offscreen GWorld
static WindowPtr gStatusWind;	// Pointer to Status window
static CTabHandle gCTab1BW;	// Handle to 1-bit black & white color table
static CTabHandle gCTab8G;	// Handle to 8-bit gray-scale color table
static CTabHandle gCTab8C;	// Handle to standard 8-bit color table
static UInt32 gLastTest = 0;	// Last test number that was run
static UInt32 gTestBeginTicks = 0;	// Tick count at beginning of a test
static UInt32 gTestEndTicks = 0;	// Tick count at end of a test
static UInt32 gTestIterations = 0;	// Number of times a test was performed
static UInt32 gTestDestAddr = 0;	// Address of 0,0 in destination memory
static UInt32 gTestSrcAddr = 0;	// Address of 0,0 in source memory
static UInt32 gBoundRectHInset = RECTINSET;	// Horizontal inset for shape bounding rect
static UInt32 gBoundRectVInset = RECTINSET;	// Vertical inset for shape bounding rect
static RgnHandle gRgnh[NUM_REGIONS];	// Handles to the various regions
static PixPatHandle gPixPath[MAX_PIXPATS];	// Handles to the PixPats

/* Procedures visible in this file only */
static void doeventloop(void);
static void updatedestwind(WindowPtr wind);
static SInt32 initdest(void);
static void cleanupdest(void);
static SInt32 initsource(void);
static void cleanupsrc(void);
static SInt32 initstatus(void);
static void cleanupstatus(void);
static SInt32 initregions(void);
static void setlocdimtitle(WindowPtr wind);
static SInt32 allocgworld(UInt32 item, Rect *rectp, GWorldPtr *gwpp);
static void rectitemtorect(UInt32 rectitem, Rect *inr, Rect *otherr, Rect *outr);
static void copyGWtoGW(GWorldPtr srcgwp, GWorldPtr destgwp, UInt32 dither);
static SInt32 drawpicttogworld(UInt32 pictitem, GWorldPtr gwp);
static void invalwind(WindowPtr wind);
static void dimtowh(UInt32 dimitem, UInt32 *widthp, UInt32 *heightp);
static void runtest(UInt32 test);
static void sint32todstr(SInt32 num, unsigned char *strp);
static void uint32todstr(UInt32 num, unsigned char *strp);
static void uint32tohstr(UInt32 num, unsigned char *strp);
static void paramstr(unsigned char *dest, SInt32 index, unsigned char *sub0, unsigned char *sub1,
						unsigned char *sub2, unsigned char *sub3);

static void erroralert(SInt32 strindex)
{
	unsigned char str[256];

	GetIndString(str, ERRORSTR_ID, strindex);
	ParamText(str, nil, nil, nil);
	NoteAlert(ERRORALERT_ID, nil);
}

static void keepwindowonmain(WindowPtr wind)
{
	SInt32 x, y, yextra;
	GrafPtr oldport;
	Point topleft, btmright;

#define EDGEFACTOR	2	// Leave this many pixels between screen edge & window frame
	GetPort(&oldport);
	SetPort(wind);
	topleft = *(Point *)&wind->portRect.top;
	btmright = *(Point *)&wind->portRect.bottom;
	LocalToGlobal(&topleft);
	LocalToGlobal(&btmright);
	SetPort(oldport);

	yextra = GetMBarHeight() + 20;	/* Menu bar height + title bar */
	x = topleft.h + EDGEFACTOR;
	if (btmright.h > qd.screenBits.bounds.right)
		x = qd.screenBits.bounds.right -
			(btmright.h - topleft.h) - EDGEFACTOR-2;
	if (topleft.h < qd.screenBits.bounds.left + EDGEFACTOR)
		x = qd.screenBits.bounds.left + EDGEFACTOR+1;
	y = topleft.v;
	if (btmright.v > qd.screenBits.bounds.bottom)
		y = qd.screenBits.bounds.bottom -
			(btmright.v - topleft.v) - EDGEFACTOR-2;
	if (topleft.v < qd.screenBits.bounds.top + yextra + EDGEFACTOR)
		y = qd.screenBits.bounds.top + yextra + EDGEFACTOR+1;
	if (x != topleft.h || y != topleft.v)
		MoveWindow(wind, x, y, false);
}

static SInt32 checkconfig(void)
{
	if (NGetTrapAddress(NQDMISCTRAP, ToolTrap) == NGetTrapAddress(UNIMPTRAP, ToolTrap))
		return(NONQDMISC_ERR);
	return(0);
}

static SInt32 setupmenus(void)
{
	SInt16 resID;
	SInt32 i, pictcount, pixpatcount;
	Handle h;
	ResType restype;
	unsigned char str[256], countstr[16];
	/* Set up the main menus */

	if ((gAppleMenu = GetMenu(APPLEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	AppendResMenu(gAppleMenu, 'DRVR');	/* Add items to the Apple menu */
	InsertMenu(gAppleMenu, 0);

	if ((gFileMenu = GetMenu(FILEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gFileMenu, 0);

	if ((gEditMenu = GetMenu(EDITMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gEditMenu, 0);

	if ((gTestMenu = GetMenu(TESTMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gTestMenu, 0);
	CheckItem(gTestMenu, NOREFRESH_ITEM, gNoRefresh);
	CheckItem(gTestMenu, NOCURSORHIDE_ITEM, gNoCursorHide);
	CheckItem(gTestMenu, gTest, true);
	CheckItem(gTestMenu, gShape, true);

	if ((gDestMenu = GetMenu(DESTMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gDestMenu, 0);
	CheckItem(gDestMenu, DESTDITHER_ITEM, gDitherDest);
	CheckItem(gDestMenu, SWAPFGBG_ITEM, gSwapFgBg);
	CheckItem(gDestMenu, USECLIPRGN_ITEM, gUseClipRgn);

	if ((gCopyBitsMenu = GetMenu(COPYBITSMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gCopyBitsMenu, 0);
	CheckItem(gCopyBitsMenu, USESEPSRC_ITEM, gSepSrc);
	CheckItem(gCopyBitsMenu, SRCDITHER_ITEM, gDitherSrc);
	CheckItem(gCopyBitsMenu, USEMASKRGN_ITEM, gUseMaskRgn);
	if (gTest != COPYBITS_ITEM)	// Need to disable entire menu
		DisableItem(gCopyBitsMenu, 0);

	if ((gLinesMenu = GetMenu(LINESMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gLinesMenu, 0);
	CheckItem(gLinesMenu, gLines, true);
	if (gTest != LINES_ITEM)	// Need to disable entire menu
		DisableItem(gLinesMenu, 0);

	if ((gTextMenu = GetMenu(TEXTMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gTextMenu, 0);
	if (gTest != TEXT_ITEM)	// Need to disable entire menu
		DisableItem(gTextMenu, 0);

	if ((gExpMenu = GetMenu(EXPMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gExpMenu, 0);

	/* Set up the hierarchical menus */

	if ((gRgnShapeMenu = GetMenu(RGNSHAPEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gRgnShapeMenu, -1);
	CheckItem(gRgnShapeMenu, gRgnShape, true);

	if ((gClipRgnMenu = GetMenu(CLIPRGNMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gClipRgnMenu, -1);
	CheckItem(gClipRgnMenu, gClipRgn, true);

	if ((gMaskRgnMenu = GetMenu(MASKRGNMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gMaskRgnMenu, -1);
	CheckItem(gMaskRgnMenu, gMaskRgn, true);

	if ((gDestLocMenu = GetMenu(DESTLOCMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gDestLocMenu, -1);
	CheckItem(gDestLocMenu, gDestLoc, true);

	if ((gDestDimMenu = GetMenu(DESTDIMMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gDestDimMenu, -1);
	CheckItem(gDestDimMenu, gDestDim, true);

	/* The dest and source image selection menus are done together,
		since they both are populated from the resource names of
		the PICT resources. */
	if ((gDestImageMenu = GetMenu(DESTIMAGEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	if ((gSrcImageMenu = GetMenu(SRCIMAGEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	if ((pictcount = Count1Resources('PICT')) == 0)
		return(CANTGETPICT_NDX);
	for (i = 1; i <= pictcount; i++) {
		SetResLoad(false);
		h = Get1Resource('PICT', PICT_BASEID - 1 + i);
		SetResLoad(true);
		if (h != nil) {
			GetResInfo(h, &resID, &restype, str);
			if (ResError() == noErr) {
				if (str[0] == 0) {
					/* The resource had no name, so we'll create one like
						'Unnamed Picture 1'. */
					sint32todstr(i, countstr);
					paramstr(str, UNNAMEDPICT_NDX, countstr, nil, nil, nil);
				}
				/* Now we append an empty name as the menu item,
					then we do a SetMenuItemText in case the picture name
					contains any metacharacters.  This makes it look clean. */
				AppendMenu(gDestImageMenu, "\p ");
				SetMenuItemText(gDestImageMenu, i, str);
				AppendMenu(gSrcImageMenu, "\p ");
				SetMenuItemText(gSrcImageMenu, i, str);
			}
		}
	}
	InsertMenu(gDestImageMenu, -1);
	if (gDestImage > pictcount)
		gDestImage = pictcount;
	CheckItem(gDestImageMenu, gDestImage, true);
	InsertMenu(gSrcImageMenu, -1);
	if (gSrcImage > pictcount)
		gSrcImage = pictcount;
	CheckItem(gSrcImageMenu, gSrcImage, true);

	if ((gFgColorMenu = GetMenu(FGCOLORMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gFgColorMenu, -1);
	CheckItem(gFgColorMenu, gFgColor, true);

	if ((gBgColorMenu = GetMenu(BGCOLORMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gBgColorMenu, -1);
	CheckItem(gBgColorMenu, gBgColor, true);

	if ((gPenSizeMenu = GetMenu(PENSIZEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gPenSizeMenu, -1);
	CheckItem(gPenSizeMenu, gPenSize, true);

	if ((gPenModeMenu = GetMenu(PENMODEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gPenModeMenu, -1);
	CheckItem(gPenModeMenu, gPenMode, true);
	CheckItem(gPenModeMenu, PM_PLUSHILITE_ITEM, gPMPlusHilite);

	// Get all three pattern menus
	if ((gPenPatMenu = GetMenu(PENPATMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	if ((gBgPatMenu = GetMenu(BGPATMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	if ((gFillPatMenu = GetMenu(FILLPATMENU_ID)) == nil)
		return(CANTGETMENU_NDX);

	// See how many PixPats we have in our resources
	if ((pixpatcount = Count1Resources('ppat')) == 0)
		return(CANTGETPIXPAT_NDX);
	if (pixpatcount > MAX_PIXPATS)
		pixpatcount = MAX_PIXPATS;

	// Add the PixPats to the end of all three pattern menus
	for (i = 1; i <= pixpatcount; i++) {
		SetResLoad(false);
		h = Get1IndResource('ppat', i);
		SetResLoad(true);
		if (h == nil)
			return(CANTGETPIXPAT_NDX);
		GetResInfo(h, &resID, &restype, str);
		if (ResError() == noErr) {
			if (str[0] == 0) {
				/* The resource had no name, so we'll create one like
					'Unnamed PixPat 1'. */
				sint32todstr(i, countstr);
				paramstr(str, UNNAMEDPIXPAT_NDX, countstr, nil, nil, nil);
			}
			/* Now we append an empty name as the menu item,
				then we do a SetMenuItemText in case the picture name
				contains any metacharacters.  This makes it look clean. */
			AppendMenu(gPenPatMenu, "\p ");
			SetMenuItemText(gPenPatMenu, i+FIRSTPIXPAT_ITEM-1, str);
			AppendMenu(gBgPatMenu, "\p ");
			SetMenuItemText(gBgPatMenu, i+FIRSTPIXPAT_ITEM-1, str);
			AppendMenu(gFillPatMenu, "\p ");
			SetMenuItemText(gFillPatMenu, i+FIRSTPIXPAT_ITEM-1, str);
		} else {
			DBUGSTR("ResError() returned error after getting PixPat ResInfo");
		}
		h = (Handle)GetPixPat(resID);
		if (h == nil)
			return(CANTGETPIXPAT_NDX);
		gPixPath[i-1] = (PixPatHandle)h;
	}
	InsertMenu(gPenPatMenu, -1);
	if (gPenPat > pixpatcount)
		gPenPat = pixpatcount + FIRSTPIXPAT_ITEM - 1;
	CheckItem(gPenPatMenu, gPenPat, true);

	InsertMenu(gBgPatMenu, -1);
	if (gBgPat > pixpatcount)
		gBgPat = pixpatcount + FIRSTPIXPAT_ITEM - 1;
	CheckItem(gBgPatMenu, gBgPat, true);

	InsertMenu(gFillPatMenu, -1);
	if (gFillPat > pixpatcount)
		gFillPat = pixpatcount + FIRSTPIXPAT_ITEM - 1;
	CheckItem(gFillPatMenu, gFillPat, true);

	if ((gSrcLocMenu = GetMenu(SRCLOCMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gSrcLocMenu, -1);
	CheckItem(gSrcLocMenu, gSrcLoc, true);

	if ((gSrcDimMenu = GetMenu(SRCDIMMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gSrcDimMenu, -1);
	if (!gSepSrc)	// Sync source & dest dimensions unless activated
		gSrcDim = gDestDim;
	CheckItem(gSrcDimMenu, gSrcDim, true);

	// SrcImage menu already set up with DestImage menu

	if ((gXferModeMenu = GetMenu(XFERMODEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gXferModeMenu, -1);
	CheckItem(gXferModeMenu, gXferMode, true);
	CheckItem(gXferModeMenu, X_PLUSDITHER_ITEM, gXPlusDither);

	if ((gSrcRectMenu = GetMenu(SRCRECTMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gSrcRectMenu, -1);
	CheckItem(gSrcRectMenu, gSrcRect, true);

	if ((gDestRectMenu = GetMenu(DESTRECTMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gDestRectMenu, -1);
	CheckItem(gDestRectMenu, gDestRect, true);

	if ((gTextModeMenu = GetMenu(TEXTMODEMENU_ID)) == nil)
		return(CANTGETMENU_NDX);
	InsertMenu(gTextModeMenu, -1);
	CheckItem(gTextModeMenu, gTextMode, true);
	CheckItem(gTextModeMenu, T_PLUSHILITE_ITEM, gTPlusHilite);
	CheckItem(gTextModeMenu, T_PLUSMASK_ITEM, gTPlusMask);

	return(0);
}

int main(void)
{
	SInt32 err;

	/* The usual invocations */
	InitGraf(&qd.thePort);	/* Initialize Quickdraw */
	InitFonts();			/* Font Manager */
	InitWindows();			/* Window Manager */
	InitMenus();			/* Menu Manager */
	TEInit(); 				/* TextEdit */
	InitDialogs(nil);		/* Dialog Manager (no resume procedure) */
	InitCursor(); 			/* Initialize the cursor to an arrow */
	FlushEvents(everyEvent, 0);	/* Clear the event queue of all events */

	/* Do the application-specific setup */
	if ((err = checkconfig()) != 0) {
		erroralert(err);
		return(err);
	}
	if ((err = setupmenus()) != 0) {
		erroralert(err);
		return(err);
	}
	if ((err = initdest()) != 0 || (err = initsource()) != 0) {
		erroralert(err);
		return(err);
	}
	if ((err = initstatus()) != 0) {
		erroralert(err);
		return(err);
	}
	if ((err = initregions()) != 0) {
		erroralert(err);
		return(err);
	}
	/* Let there be menus and windows */
	DrawMenuBar();
	ShowWindow(gDestWind);
	if (gSepSrc && gSrcLoc == WINDOW_ITEM)
		ShowWindow(gSrcWind);
	ShowWindow(gStatusWind);

	doeventloop();	// Main event dispatching

	cleanupdest();
	cleanupsrc();
	cleanupstatus();

	return(0);
}

static void uint32tohstr(UInt32 num, unsigned char *strp)
{
	SInt32 i, digit;
	char numbuf[20];

	i = 0;
	do {	/* Convert the number to a reversed ASCII string */
		digit = num & 0x0F;
		if (digit >= 10)
			digit += ('A' - 10);
		else
			digit += '0';
		numbuf[i++] = digit;
	} while ((num >>= 4) > 0);
	*strp++ = i;		/* Copy the length byte */
	while (--i >= 0)	/* Copy the string forwards to its destination */
		*strp++ = numbuf[i];
}

static void uint32todstr(UInt32 num, unsigned char *strp)
{
	SInt32 i;
	char numbuf[20];

	i = 0;
	do {	/* Convert the number to a reversed ASCII string */
		numbuf[i++] = num % 10 + '0';
	} while ((num /= 10) > 0);
	*strp++ = i;		/* Copy the length byte */
	while (--i >= 0)	/* Copy the string forwards to its destination */
		*strp++ = numbuf[i];
}

static void sint32todstr(SInt32 num, unsigned char *strp)
{
	Boolean neg;
	SInt32 i;
	char numbuf[20];

	if (num < 0) {
		neg = true;
		num = -num;
	} else {
		neg = false;
	}
	i = 0;
	do {	/* Convert the number to a reversed ASCII string */
		numbuf[i++] = num % 10 + '0';
	} while ((num /= 10) > 0);
	if (neg)
		numbuf[i++] = '-';
	*strp++ = i;		/* Copy the length byte */
	while (--i >= 0)	/* Copy the string forwards to its destination */
		*strp++ = numbuf[i];
}

/* Note: all of these are Pascal strings */
static void paramstr(unsigned char *dest, SInt32 index, unsigned char *sub0, unsigned char *sub1,
			unsigned char *sub2, unsigned char *sub3)
{
	SInt32 srclen, substlen, destindex;
	unsigned char str[256], *source, *substitute;

	GetIndString(str, PARAMSTR_ID, index);
	source = str;

	srclen = *source++;
	destindex = 0;

	while (srclen > 0) {
		if (*source == '^') {	/* Substitution marker */
			if (srclen == 1) {	/* Last character in string was '^' */
				goto storedest;
			} else {
				switch (*(source+1)) {
				case '0':
					substitute = sub0;
					break;
				case '1':
					substitute = sub1;
					break;
				case '2':
					substitute = sub2;
					break;
				case '3':
					substitute = sub3;
					break;
				default:
					goto storedest;
				}
			}
			source += 2;	/* Skip over the '^n' characters */
			srclen -= 2;
			if (substitute != nil) {	/* Guard against NIL pointers */
				substlen = *substitute++;
				while (substlen) {
					destindex++;
					dest[destindex] = *substitute++;
					substlen--;
				}
			}
		} else {	/* Not doing a substitution, so copy a byte from the source */
storedest:
			destindex++;
			dest[destindex] = *source++;
			srclen--;
		}
	}

	dest[0] = destindex;	/* Finally, fill in the length byte */
}

static void coloritemtoRGB(UInt32 item, RGBColor *otherp, RGBColor *colorp)
{
	switch (item) {
	case BLACK_ITEM:
		colorp->red = 0;
		colorp->green = 0;
		colorp->blue = 0;
		break;
	case WHITE_ITEM:
		colorp->red = 0xFFFF;
		colorp->green = 0xFFFF;
		colorp->blue = 0xFFFF;
		break;
	case RED_ITEM:
		colorp->red = 0xFFFF;
		colorp->green = 0;
		colorp->blue = 0;
		break;
	case GREEN_ITEM:
		colorp->red = 0;
		colorp->green = 0xFFFF;
		colorp->blue = 0;
		break;
	case BLUE_ITEM:
		colorp->red = 0;
		colorp->green = 0;
		colorp->blue = 0xFFFF;
		break;
	case CYAN_ITEM:
		colorp->red = 0;
		colorp->green = 0xFFFF;
		colorp->blue = 0xFFFF;
		break;
	case MAGENTA_ITEM:
		colorp->red = 0xFFFF;
		colorp->green = 0;
		colorp->blue = 0xFFFF;
		break;
	case YELLOW_ITEM:
		colorp->red = 0xFFFF;
		colorp->green = 0xFFFF;
		colorp->blue = 0;
		break;
	case GRAY53_ITEM:
		colorp->red = 0x8888;
		colorp->green = 0x8888;
		colorp->blue = 0x8888;
		break;
	case HILITE_ITEM:
		LMGetHiliteRGB(colorp);
		break;
	case RANDOM_ITEM:
		colorp->red = Random();
		colorp->green = Random();
		colorp->blue = Random();
		break;
	case OTHERCOLOR_ITEM:
		*colorp = *otherp;
		break;
	}
}

static UInt32 windpttoaddr(WindowPtr wind, Point *originp)
{
	UInt32 v, h, addr;
	GrafPtr oldport;
	GDPtr gdp;
	GDHandle gdh;
	PixMapPtr pmp;
	Point pt;

	GetPort(&oldport);
	SetPort(wind);
	pt = *originp;	// Get top-left origin point
	LocalToGlobal(&pt);
	SetPort(oldport);

	for (gdh = GetDeviceList(); gdh != nil; gdh = GetNextDevice(gdh)) {
		if (!TestDeviceAttribute(gdh, screenDevice) ||
				!TestDeviceAttribute(gdh, screenActive))
			continue;
		gdp = *gdh;
		/* See if the window is on this GDevice */
		if (PtInRect(pt, &gdp->gdRect))
			goto gotgdev;
	}
	return(0);

gotgdev:
	v = pt.v - gdp->gdRect.top;
	h = pt.h - gdp->gdRect.left;
	pmp = *gdp->gdPMap;
	addr = (UInt32)pmp->baseAddr + v * ((UInt32)pmp->rowBytes & 0x3FFF);
	addr += h << (pmp->pixelSize >> 4);
	return(addr);
}

static UInt32 gwpixmappttoaddr(PixMapHandle pmh, Point *originp)
{
	UInt32 v, h, addr;
	PixMapPtr pmp;
	Point pt;

	addr = (UInt32)GetPixBaseAddr(pmh);
	pt = *originp;
	pmp = *pmh;
	v = pt.v - pmp->bounds.top;
	h = pt.h - pmp->bounds.left;
	addr += v * ((UInt32)pmp->rowBytes & 0x3FFF);
	addr += h << (pmp->pixelSize >> 4);
	return(addr);
}

static void docopybitstest(WindowPtr wind)
{
	UInt32 mode, width, height;
	UInt32 iter, beginticks, curticks, stopticks;
	BitMapPtr srcbits, destbits;
	CGrafPtr oldport;
	GDHandle oldgdh;
	PixMapHandle destpmh, srcpmh;
	RgnHandle maskrgnh;
	Rect srcrect, destrect, portrect, r;
	RGBColor foreRGB, backRGB, opRGB;

	switch (gXferMode) {
	case X_SRCCOPY_ITEM:
		mode = srcCopy;
		break;
	case X_SRCOR_ITEM:
		mode = srcOr;
		break;
	case X_SRCXOR_ITEM:
		mode = srcXor;
		break;
	case X_SRCBIC_ITEM:
		mode = srcBic;
		break;
	case X_NOTSRCCOPY_ITEM:
		mode = notSrcCopy;
		break;
	case X_NOTSRCOR_ITEM:
		mode = notSrcOr;
		break;
	case X_NOTSRCXOR_ITEM:
		mode = notSrcXor;
		break;
	case X_NOTSRCBIC_ITEM:
		mode = notSrcBic;
		break;
	case X_ADDOVER_ITEM:
		mode = addOver;
		break;
	case X_ADDPIN_ITEM:
		mode = addPin;
		opRGB.red = 0xFFFF;
		opRGB.green = 0xFFFF;
		opRGB.blue = 0xFFFF;
		OpColor(&opRGB);
		break;
	case X_SUBOVER_ITEM:
		mode = subOver;
		break;
	case X_SUBPIN_ITEM:
		mode = subPin;
		opRGB.red = 0;
		opRGB.green = 0;
		opRGB.blue = 0;
		OpColor(&opRGB);
		break;
	case X_ADMAX_ITEM:
		mode = adMax;
		break;
	case X_ADMIN_ITEM:
		mode = adMin;
		break;
	case X_BLEND_ITEM:
		mode = blend;
		opRGB.red = 0x4040;
		opRGB.green = 0x4040;
		opRGB.blue = 0x4040;
		OpColor(&opRGB);
		break;
	case X_TRANSPARENT_ITEM:
		mode = transparent;
		break;
	case X_HILITE_ITEM:
		mode = hilitetransfermode;
		break;
	}
	if (gXPlusDither)
		mode += ditherCopy;

	/* Get the source and destination rectangles */
	r.top = 0;
	r.left = 0;
	dimtowh(gSrcDim, &width, &height);
	r.bottom = height;
	r.right = width;
	rectitemtorect(gSrcRect, &r, &gSrcOtherRect, &srcrect);
	dimtowh(gDestDim, &width, &height);
	r.bottom = height;
	r.right = width;
	rectitemtorect(gDestRect, &r, &gDestOtherRect, &destrect);

	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen
		GetGWorld(&oldport, &oldgdh);
		SetGWorld(gDestGW, nil);
		destpmh = GetGWorldPixMap(gDestGW);
		LockPixels(destpmh);
		destbits = &((GrafPtr)gDestGW)->portBits;
		portrect = ((GrafPtr)gDestGW)->portRect;
		gTestDestAddr = gwpixmappttoaddr(destpmh, (Point *)&destrect);
	} else {	// Destination is onscreen
		// The current port is already set to the window
		destbits = &wind->portBits;
		portrect = wind->portRect;
		gTestDestAddr = windpttoaddr(wind, (Point *)&destrect);
	}
	if (gSepSrc) {
		if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen
			srcpmh = GetGWorldPixMap(gSrcGW);
			LockPixels(srcpmh);
			srcbits = &((GrafPtr)gSrcGW)->portBits;
			gTestSrcAddr = gwpixmappttoaddr(srcpmh, (Point *)&srcrect);
		} else {	// Source is onscreen
			srcbits = &gSrcWind->portBits;
			gTestSrcAddr = windpttoaddr(gSrcWind, (Point *)&srcrect);
		}
	} else {	// No separate source, so source loc = destination
		srcbits = destbits;
		if (gDestLoc != WINDOW_ITEM)	// Destination is offscreen
			gTestSrcAddr = gwpixmappttoaddr(destpmh, (Point *)&srcrect);
		else	// Destination is onscreen
			gTestSrcAddr = windpttoaddr(wind, (Point *)&srcrect);
	}
	if (gUseClipRgn)
		SetClip(gRgnh[gClipRgn]);
	else
		ClipRect(&portrect);

	/* Set up any custom colors we might have */
	coloritemtoRGB(gFgColor, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(gBgColor, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	/* Grab the mask region, if it's being used */
	maskrgnh = nil;
	if (gUseMaskRgn)
		maskrgnh = gRgnh[gMaskRgn];

	/* Perform the actual test, gathering timing information */
	beginticks = TickCount();
	stopticks = beginticks + TESTTICKS;
	for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
		CopyBits(srcbits, destbits, &srcrect, &destrect, mode, maskrgnh);
	}
	gTestBeginTicks = beginticks;
	gTestEndTicks = curticks;
	gTestIterations = iter;

	/* Restore the foreground and background colors to black & white */
	coloritemtoRGB(BLACK_ITEM, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(WHITE_ITEM, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	ClipRect(&portrect);	// Restore the full clipping

	if (gDestLoc != WINDOW_ITEM) {	// Destination was offscreen
		SetGWorld(oldport, oldgdh);
		UnlockPixels(destpmh);
	}
	if (gSepSrc && gSrcLoc != WINDOW_ITEM)	// Source was offscreen
		UnlockPixels(srcpmh);
}

static void dotexttest(WindowPtr wind)
{
	UInt32 mode, width, height, fontheight, fontascent, v;
	UInt32 iter, beginticks, curticks, stopticks;
	BitMapPtr destbits;
	CGrafPtr oldport;
	GDHandle oldgdh;
	PixMapHandle destpmh;
	Rect destrect, portrect, r;
	RGBColor foreRGB, backRGB, opRGB;
	FontInfo fi;

	switch (gTextMode) {
	case T_SRCCOPY_ITEM:
		mode = srcCopy;
		break;
	case T_SRCOR_ITEM:
		mode = srcOr;
		break;
	case T_SRCXOR_ITEM:
		mode = srcXor;
		break;
	case T_SRCBIC_ITEM:
		mode = srcBic;
		break;
	case T_NOTSRCCOPY_ITEM:
		mode = notSrcCopy;
		break;
	case T_NOTSRCOR_ITEM:
		mode = notSrcOr;
		break;
	case T_NOTSRCXOR_ITEM:
		mode = notSrcXor;
		break;
	case T_NOTSRCBIC_ITEM:
		mode = notSrcBic;
		break;
	case T_ADDOVER_ITEM:
		mode = addOver;
		break;
	case T_ADDPIN_ITEM:
		mode = addPin;
		opRGB.red = 0xFFFF;
		opRGB.green = 0xFFFF;
		opRGB.blue = 0xFFFF;
		OpColor(&opRGB);
		break;
	case T_SUBOVER_ITEM:
		mode = subOver;
		break;
	case T_SUBPIN_ITEM:
		mode = subPin;
		opRGB.red = 0;
		opRGB.green = 0;
		opRGB.blue = 0;
		OpColor(&opRGB);
		break;
	case T_ADMAX_ITEM:
		mode = adMax;
		break;
	case T_ADMIN_ITEM:
		mode = adMin;
		break;
	case T_BLEND_ITEM:
		mode = blend;
		opRGB.red = 0x4040;
		opRGB.green = 0x4040;
		opRGB.blue = 0x4040;
		OpColor(&opRGB);
		break;
	case T_TRANSPARENT_ITEM:
		mode = transparent;
		break;
	case T_GRAYISHTEXTOR_ITEM:
		mode = grayishTextOr;
		break;
	}
	if (gTPlusHilite)
		mode += hilitetransfermode;
	if (gTPlusMask)
		mode += 64;

	/* Get the destination rectangle */
	r.top = 0;
	r.left = 0;
	dimtowh(gDestDim, &width, &height);
	r.bottom = height;
	r.right = width;
	rectitemtorect(gDestRect, &r, &gDestOtherRect, &destrect);

	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen
		GetGWorld(&oldport, &oldgdh);
		SetGWorld(gDestGW, nil);
		destpmh = GetGWorldPixMap(gDestGW);
		LockPixels(destpmh);
		destbits = &((GrafPtr)gDestGW)->portBits;
		portrect = ((GrafPtr)gDestGW)->portRect;
		gTestDestAddr = gwpixmappttoaddr(destpmh, (Point *)&destrect);
	} else {	// Destination is onscreen
		// The current port is already set to the window
		destbits = &wind->portBits;
		portrect = wind->portRect;
		gTestDestAddr = windpttoaddr(wind, (Point *)&destrect);
	}
	gTestSrcAddr = 0;
	if (gUseClipRgn)
		SetClip(gRgnh[gClipRgn]);
	else
		ClipRect(&portrect);

	/* Set the Text parameters */
	TextFont(0);
	TextSize(20);
	TextFace(0);
	TextMode(mode);
	GetFontInfo(&fi);
	fontascent = fi.ascent;
	fontheight = fontascent + fi.descent;

	/* Set up any custom colors we might have */
	coloritemtoRGB(gFgColor, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(gBgColor, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	v = 0;

	/* Perform the actual test, gathering timing information */
	beginticks = TickCount();
	stopticks = beginticks + TESTTICKS;
	for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
		MoveTo(0, v + fontascent);
		DrawString("\pThe quick red fox jumped over the lazy brown dog.");
		v += fontascent;
		if (v >= destrect.bottom)
			v = 0;
	}
	gTestBeginTicks = beginticks;
	gTestEndTicks = curticks;
	gTestIterations = iter;

	/* Restore the foreground and background colors to black & white */
	coloritemtoRGB(BLACK_ITEM, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(WHITE_ITEM, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	/* Restore the transfer mode */
	TextMode(srcOr);

	ClipRect(&portrect);	// Restore the full clipping

	if (gDestLoc != WINDOW_ITEM) {	// Destination was offscreen
		SetGWorld(oldport, oldgdh);
		UnlockPixels(destpmh);
	}
}

static void patitemtopat(UInt32 patitem, PatPtr patptr)
{
	static Pattern gHStripes =
		{ 0xFF, 0x00, 0xFF, 0x00, 0x00, 0xFF, 0xFF, 0x00 };
	static Pattern gVStripes =
		{ 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6, 0xA6 };
	static Pattern gDStripes =
		{ 0x84, 0x42, 0x21, 0x90, 0x48, 0x24, 0x12, 0x09 };
	static Pattern gNumNibbles =
		{ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF };

	switch (patitem) {
	case BLACKPAT_ITEM:
		*patptr = qd.black;
		break;
	case WHITEPAT_ITEM:
		*patptr = qd.white;
		break;
	case GRAY50PAT_ITEM:
		*patptr = qd.gray;
		break;
	case LTGRAYPAT_ITEM:
		*patptr = qd.ltGray;
		break;
	case DKGRAYPAT_ITEM:
		*patptr = qd.dkGray;
		break;
	case HSTRIPESPAT_ITEM:
		*patptr = gHStripes;
		break;
	case VSTRIPESPAT_ITEM:
		*patptr = gVStripes;
		break;
	case DSTRIPESPAT_ITEM:
		*patptr = gDStripes;
		break;
	case NUMNIBBLESPAT_ITEM:
		*patptr = gNumNibbles;
		break;
	default:
		DBUGPRINTF(("Unknown patitem %d", patitem));
		break;
	}
}

static void pensizeitemtopt(UInt32 pensizeitem, Point *ptp)
{
	switch (pensizeitem) {
	case PS_1X1_ITEM:
		ptp->v = 1;
		ptp->h = 1;
		break;
	case PS_2X2_ITEM:
		ptp->v = 2;
		ptp->h = 2;
		break;
	case PS_4X4_ITEM:
		ptp->v = 4;
		ptp->h = 4;
		break;
	case PS_8X8_ITEM:
		ptp->v = 8;
		ptp->h = 8;
		break;
	case PS_16X16_ITEM:
		ptp->v = 16;
		ptp->h = 16;
		break;
	}
}

static void doshapetest(WindowPtr wind, UInt32 test)
{
	UInt32 mode, h, v, width, height;
	UInt32 iter, beginticks, curticks, stopticks;
	CGrafPtr oldport;
	GDHandle oldgdh;
	PixMapHandle destpmh;
	RgnHandle rgnh;
	PixPatHandle pixpath;
	Point pensize;
	Rect destrect, portrect;
	Pattern pat;
	RGBColor foreRGB, backRGB;

	switch (gPenMode) {
	case PATCOPY_ITEM:
		mode = patCopy;
		break;
	case PATOR_ITEM:
		mode = patOr;
		break;
	case PATXOR_ITEM:
		mode = patXor;
		break;
	case PATBIC_ITEM:
		mode = patBic;
		break;
	case NOTPATCOPY_ITEM:
		mode = notPatCopy;
		break;
	case NOTPATOR_ITEM:
		mode = notPatOr;
		break;
	case NOTPATXOR_ITEM:
		mode = notPatXor;
		break;
	case NOTPATBIC_ITEM:
		mode = notPatBic;
		break;
	case PATCOPYPLUSTRANSP_ITEM:
		mode = patCopy + transparent;
		break;
	}
	if (gPMPlusHilite)
		mode += hilitetransfermode;

	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen
		GetGWorld(&oldport, &oldgdh);
		SetGWorld(gDestGW, nil);
		destpmh = GetGWorldPixMap(gDestGW);
		LockPixels(destpmh);
		destrect = ((GrafPtr)gDestGW)->portRect;
		portrect = destrect;
		InsetRect(&destrect, gBoundRectHInset, gBoundRectVInset);
		gTestDestAddr = gwpixmappttoaddr(destpmh, (Point *)&destrect);
	} else {	// Destination is onscreen
		// The current port is already set to the window
		destrect = wind->portRect;
		portrect = destrect;
		InsetRect(&destrect, gBoundRectHInset, gBoundRectVInset);
		gTestDestAddr = windpttoaddr(wind, (Point *)&destrect);
	}
	gTestSrcAddr = 0;
	if (gUseClipRgn)
		SetClip(gRgnh[gClipRgn]);
	else
		ClipRect(&portrect);

	switch (test) {
	case ERASESHAPE_ITEM:
		if (gBgPat >= FIRSTPIXPAT_ITEM) {
			/* It's a PixPat */
			BackPixPat(gPixPath[gBgPat-FIRSTPIXPAT_ITEM]);
		} else {
			/* Set the old-style background pattern */
			patitemtopat(gBgPat, &pat);
			BackPat(&pat);
		}
		break;
	case INVERTSHAPE_ITEM:
		break;	// Don't need to set the pen parameters
	case PATFILLSHAPE_ITEM:
		if (gFillPat >= FIRSTPIXPAT_ITEM) {
			/* It's a PixPat */
			pixpath = gPixPath[gFillPat-FIRSTPIXPAT_ITEM];
		} else {
			/* Set the old-style background pattern */
			patitemtopat(gFillPat, &pat);
			pixpath = nil;
		}
		break;
	default:
		/* Set the pen mode */
		PenMode(mode);
		/* Set the pen pixpat or pattern */
		if (gPenPat >= FIRSTPIXPAT_ITEM) {
			/* It's a PixPat */
			PenPixPat(gPixPath[gPenPat-FIRSTPIXPAT_ITEM]);
		} else {
			/* Set the old-style pen pattern */
			patitemtopat(gPenPat, &pat);
			PenPat(&pat);
		}
		/* Set the pen size */
		pensizeitemtopt(gPenSize, &pensize);
		PenSize(pensize.h, pensize.v);
		break;
	}

	/* Set up any custom colors we might have */
	coloritemtoRGB(gFgColor, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(gBgColor, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	if (gShape == REGION_ITEM)
		rgnh = gRgnh[gRgnShape];

	/* Perform the actual test, gathering timing information */
	beginticks = TickCount();
	stopticks = beginticks + TESTTICKS;

	switch (test) {
	case LINES_ITEM:
		switch (gLines) {
		case HORIZONTAL_ITEM:
			h = destrect.left;
			width = destrect.right - h;
			v = destrect.top;
			height = destrect.bottom - v;
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				MoveTo(h, v);
				LineTo(h + width, v);
				v += pensize.v + 1;
				if (v >= destrect.bottom)
					v = destrect.top;
			}
			break;
		case VERTICAL_ITEM:
			h = destrect.left;
			width = destrect.right - h;
			v = destrect.top;
			height = destrect.bottom - v;
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				MoveTo(h, v);
				LineTo(h, v + height);
				h += pensize.h + 1;
				if (h >= destrect.right)
					h = destrect.left;
			}
			break;
		case DIAGONAL_ITEM:
			h = destrect.left;
			width = destrect.right - h;
			v = destrect.top;
			height = destrect.bottom - v;
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				MoveTo(h, v);
				LineTo(h + height, v + height);
			}
			break;
		case SLANTED_ITEM:
			break;
		}
		break;
	case FRAMESHAPE_ITEM:
		switch (gShape) {
		case RECTANGLE_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				FrameRect(&destrect);
			}
			break;
		case REGION_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				FrameRgn(rgnh);
			}
			break;
		case OVAL_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				FrameOval(&destrect);
			}
			break;
		case ROUNDRECT_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				FrameRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE);
			}
			break;
		case ARC_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				FrameArc(&destrect, ARC_STARTANGLE, ARC_DEGREES);
			}
			break;
		default:
			DBUGPRINTF(("Unimplemented Frame Shape %d", gShape));
			break;
		}
		break;
	case PAINTSHAPE_ITEM:
		switch (gShape) {
		case RECTANGLE_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				PaintRect(&destrect);
			}
			break;
		case REGION_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				PaintRgn(rgnh);
			}
			break;
		case OVAL_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				PaintOval(&destrect);
			}
			break;
		case ROUNDRECT_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				PaintRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE);
			}
			break;
		case ARC_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				PaintArc(&destrect, ARC_STARTANGLE, ARC_DEGREES);
			}
			break;
		default:
			DBUGPRINTF(("Unimplemented Paint Shape %d", gShape));
			break;
		}
		break;
	case ERASESHAPE_ITEM:
		switch (gShape) {
		case RECTANGLE_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				EraseRect(&destrect);
			}
			break;
		case REGION_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				EraseRgn(rgnh);
			}
			break;
		case OVAL_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				EraseOval(&destrect);
			}
			break;
		case ROUNDRECT_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				EraseRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE);
			}
			break;
		case ARC_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				EraseArc(&destrect, ARC_STARTANGLE, ARC_DEGREES);
			}
			break;
		default:
			DBUGPRINTF(("Unimplemented Erase Shape %d", gShape));
			break;
		}
		break;
	case INVERTSHAPE_ITEM:
		switch (gShape) {
		case RECTANGLE_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				InvertRect(&destrect);
			}
			break;
		case REGION_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				InvertRgn(rgnh);
			}
			break;
		case OVAL_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				InvertOval(&destrect);
			}
			break;
		case ROUNDRECT_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				InvertRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE);
			}
			break;
		case ARC_ITEM:
			for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
				InvertArc(&destrect, ARC_STARTANGLE, ARC_DEGREES);
			}
			break;
		default:
			DBUGPRINTF(("Unimplemented Invert Shape %d", gShape));
			break;
		}
		break;
	case PATFILLSHAPE_ITEM:
		switch (gShape) {
		case RECTANGLE_ITEM:
			if (pixpath == nil) {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillRect(&destrect, &pat);
				}
			} else {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillCRect(&destrect, pixpath);
				}
			}
			break;
		case REGION_ITEM:
			if (pixpath == nil) {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillRgn(rgnh, &pat);
				}
			} else {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillCRgn(rgnh, pixpath);
				}
			}
			break;
		case OVAL_ITEM:
			if (pixpath == nil) {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillOval(&destrect, &pat);
				}
			} else {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillCOval(&destrect, pixpath);
				}
			}
			break;
		case ROUNDRECT_ITEM:
			if (pixpath == nil) {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE, &pat);
				}
			} else {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillCRoundRect(&destrect, RRECT_CURVATURE, RRECT_CURVATURE, pixpath);
				}
			}
			break;
		case ARC_ITEM:
			if (pixpath == nil) {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillArc(&destrect, ARC_STARTANGLE, ARC_DEGREES, &pat);
				}
			} else {
				for (iter = 0; (curticks = TickCount()) < stopticks; iter++) {
					FillCArc(&destrect, ARC_STARTANGLE, ARC_DEGREES, pixpath);
				}
			}
			break;
		default:
			DBUGPRINTF(("Unimplemented Pattern Fill Shape %d", gShape));
			break;
		}
		break;
	}

	gTestBeginTicks = beginticks;
	gTestEndTicks = curticks;
	gTestIterations = iter;

	/* Restore the foreground and background colors to black & white */
	coloritemtoRGB(BLACK_ITEM, &gFgOtherRGB, &foreRGB);
	coloritemtoRGB(WHITE_ITEM, &gBgOtherRGB, &backRGB);
	RGBForeColor(&foreRGB);
	RGBBackColor(&backRGB);

	/* Restore the default pen settings */
	PenNormal();

	ClipRect(&portrect);	// Restore the full clipping

	if (gDestLoc != WINDOW_ITEM) {	// Destination was offscreen
		SetGWorld(oldport, oldgdh);
		UnlockPixels(destpmh);
	}
}

static void runtest(UInt32 test)
{
	GrafPtr oldport;
	WindowPtr wind;

	if (gDestPictBufGW == nil || (gDestLoc != WINDOW_ITEM && gDestGW == nil)) {
		erroralert(CANTGETGWORLD_NDX);
		return;
	}
	if (gTest == COPYBITS_ITEM && gSepSrc) {
		if (gSrcPictBufGW == nil || (gSrcLoc != WINDOW_ITEM && gSrcGW == nil)) {
			erroralert(CANTGETGWORLD_NDX);
			return;
		}
	}

	if (!gNoRefresh && gDestLoc != WINDOW_ITEM)	// Destination is offscreen GWorld
		copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);
	if (!gNoCursorHide)
		HideCursor();

	GetPort(&oldport);
	wind = gDestWind;
	SetPort(wind);
	if (!gNoRefresh || gDestLoc != WINDOW_ITEM)
		updatedestwind(wind);

	switch (test) {
	case COPYBITS_ITEM:
		docopybitstest(wind);
		break;
	case TEXT_ITEM:
		dotexttest(wind);
		break;
	case LINES_ITEM:
	case FRAMESHAPE_ITEM:
	case PAINTSHAPE_ITEM:
	case ERASESHAPE_ITEM:
	case INVERTSHAPE_ITEM:
	case PATFILLSHAPE_ITEM:
		doshapetest(wind, test);
		break;
	default:
		test = 0;
		break;
	}

	if (gDestLoc != WINDOW_ITEM)	// Destination is offscreen GWorld
		updatedestwind(wind);	// Show the changed pixels

	SetPort(oldport);
	if (!gNoCursorHide)
		ShowCursor();

	gLastTest = test;
	invalwind(gStatusWind);	// Update the timing statistics
}

static void showdialog(SInt32 resid)
{
	short itemhit;
	DialogPtr dlog;

	if ((dlog = GetNewDialog(resid, nil, (WindowPtr)-1)) == nil) {
		erroralert(CANTGETDLOG_NDX);
		return;
	}
	ModalDialog(nil, &itemhit);	/* Get all events until an active item is hit */
	DisposeDialog(dlog);		/* Get rid of the dialog */
}

static void setitemstate(MenuHandle menu, UInt32 item, UInt32 enable)
{
	if (enable)
		EnableItem(menu, item);
	else
		DisableItem(menu, item);
}

static void configmenus(void)
{
	UInt32 i, enable;
	WindowPtr wind;

	// Edit menu
	if ((wind = FrontWindow()) && ((WindowPeek)wind)->windowKind < 0) {
		// It's a DA window
		enable = true;
	} else {
		// We won't allow editing commands
		enable = false;
	}
	setitemstate(gEditMenu, UNDO_ITEM, enable);
	setitemstate(gEditMenu, CUT_ITEM, enable);
	setitemstate(gEditMenu, COPY_ITEM, true);
	setitemstate(gEditMenu, PASTE_ITEM, enable);
	setitemstate(gEditMenu, CLEAR_ITEM, enable);

	//  Test menu
	for (i = RECTANGLE_ITEM; i <= ARC_ITEM; i++)
		setitemstate(gTestMenu, i, gTest >= FRAMESHAPE_ITEM);
	enable = true;
	if (gTest == COPYBITS_ITEM || (gTest >= FRAMESHAPE_ITEM && gShape == REGION_ITEM))
		enable = false;
	setitemstate(gTestMenu, BOUNDRECT_ITEM, enable);
	enable = false;
	if (gTest >= FRAMESHAPE_ITEM && gShape == REGION_ITEM)
		enable = true;
	setitemstate(gTestMenu, RGNSHAPE_ITEM, enable);

	// Destination menu
	enable = true;
	if (gTest == INVERTSHAPE_ITEM || gTest == PATFILLSHAPE_ITEM)
		enable = false;
	setitemstate(gDestMenu, FGCOLOR_ITEM, enable);
	setitemstate(gDestMenu, BGCOLOR_ITEM, enable);
	setitemstate(gDestMenu, SWAPFGBG_ITEM, enable);
	setitemstate(gDestMenu, RESETCOLORS_ITEM, enable);
	enable = false;
	if (gTest == LINES_ITEM || gTest == FRAMESHAPE_ITEM)
		enable = true;
	setitemstate(gDestMenu, PENSIZE_ITEM, enable);
	enable = false;
	if (gTest == LINES_ITEM || gTest == FRAMESHAPE_ITEM ||
			gTest == PAINTSHAPE_ITEM)
		enable = true;
	setitemstate(gDestMenu, PENMODE_ITEM, enable);
	setitemstate(gDestMenu, PENPATTERN_ITEM, enable);
	setitemstate(gDestMenu, BGPATTERN_ITEM, gTest == ERASESHAPE_ITEM);
	setitemstate(gDestMenu, FILLPATTERN_ITEM, gTest == PATFILLSHAPE_ITEM);
	setitemstate(gDestMenu, CLIPRGN_ITEM, gUseClipRgn);

	// CopyBits menu
	for (i = SRCLOCATION_ITEM; i <= SRCDITHER_ITEM; i++)
		setitemstate(gCopyBitsMenu, i, gSepSrc);
	setitemstate(gCopyBitsMenu, MASKRGN_ITEM, gUseMaskRgn);
}

static void doapplemenu(UInt32 item)
{
	GrafPtr oldport;
	unsigned char str[256];

	if (item == ABOUT_ITEM) {
		showdialog(ABOUTDLOG_ID);
	} else {
		GetMenuItemText(gAppleMenu, item, str);	/* Get the DA name */
		GetPort(&oldport);		/* Preserve the current port in case the DA doesn't */
		OpenDeskAcc(str);		/* Open the DA */
		SetPort(oldport);		/* Restore the port */
	}
}

static void dofilemenu(UInt32 item)
{
	switch (item) {
	case QUIT_ITEM:
		gQuitFlag = true;	/* Set the flag so we leave the main event loop */
		break;
	}
}

static void docopy(void)
{

	SInt32 err;
	GrafPtr oldport;
	PicHandle pic;
	OpenCPicParams cpp;

	cpp.srcRect.top = 0;
	cpp.srcRect.left = 0;
	cpp.srcRect.bottom = 100;
	cpp.srcRect.right = 100;
	cpp.hRes = 0x00480000;
	cpp.vRes = 0x00480000;
	cpp.version = -2;
	cpp.reserved1 = 0;
	cpp.reserved2 = 0;
	GetPort(&oldport);
	SetPort(gDestWind);
	if ((pic = OpenCPicture(&cpp)) == nil) {
		DBUGSTR("OpenCPicture() failed");
		SetPort(oldport);	/* Restore the port */
		return;
	}
	ClipRect(&cpp.srcRect);
	/* Copy from the grafport to itself in order to record it */
	CopyBits((BitMap *)&gDestWind->portBits,
			 (BitMap *)&gDestWind->portBits,
			 &cpp.srcRect, &cpp.srcRect, srcCopy, nil);
#if 0
	MoveTo(2, 20);
DBUGSTR("About to call DrawString");
	DrawString("\pTest of a string!");
#endif
	ClosePicture();
	ClipRect(&gDestWind->portRect);

	if (*pic == nil) {
		DBUGSTR("Closed picture has null size");
		SetPort(oldport);	/* Restore the port */
		return;
	}

	HLock((Handle)pic);
	if (EmptyRect(&(*pic)->picFrame)) {	/* Did we run out of memory? */
		DBUGSTR("Closed picture has empty frame rectangle");
		KillPicture(pic);
		SetPort(oldport);	/* Restore the port */
		return;
	}
	ZeroScrap();
	err = PutScrap(GetHandleSize((Handle)pic), 'PICT', (Ptr)*pic);
	KillPicture(pic);
	if (err) {
		DBUGSTR("Error %d from PutScrap()");
		ZeroScrap();
	}
	SetPort(oldport);	/* Restore the port */
	return;
}

static void doeditmenu(UInt32 item)
{
	switch (item) {
	case COPY_ITEM:
		docopy();
		break;
	default:
		break;
	}
}

static SInt32 strtouint32(unsigned char *strp, UInt32 *valptr)
{
	UInt32 len, i, digits;
	long val;

	len = strp[0];
	strp++;
	i = 0;
	while (i < len) {	/* Ignore leading spaces */
		if (*strp != ' ')
			break;
		*strp++;
		i++;
	}
	digits = 0;
	val = 0;
	while (i < len && *strp >= '0' && *strp <= '9') {
		val = val * 10 + (*strp - '0');
		digits++;
		strp++;
		i++;
	}
	while (i < len) {	/* Ignore trailing spaces */
		if (*strp != ' ')
			break;
		*strp++;
		i++;
	}
	if (i < len || digits == 0 || digits > 9) {
		*valptr = 0;  /* Zero the number by default with errors!!! */
		return(1);
	} else {
		*valptr = val;
		return(0);
	}
}

static void doboundrectdlog(void)
{
	SInt16 itemhit, itemtype;
	UInt32 val, width, height;
	GrafPtr oldport;
	DialogPtr dlog;
	Handle horizitemh, vertitemh;
	Rect r;
	unsigned char str[256];

	if ((dlog = GetNewDialog(BOUNDRECTDLOG_ID, nil, (WindowPtr)-1)) == nil) {
		erroralert(CANTGETDLOG_NDX);
		return;
	}

	GetPort(&oldport);
	SetPort(dlog);

	dimtowh(gDestDim, &width, &height);

	GetDialogItem(dlog, HORIZ_BRDITM, &itemtype, &horizitemh, &r);
	GetDialogItem(dlog, VERT_BRDITM, &itemtype, &vertitemh, &r);
	uint32todstr(gBoundRectHInset, str);
	SetDialogItemText(horizitemh, str);
	uint32todstr(gBoundRectVInset, str);
	SetDialogItemText(vertitemh, str);
	/* Select the first editText item */
	SelectDialogItemText(dlog, HORIZ_BRDITM, 0, 32767);

	ShowWindow(dlog);

	do {
		ModalDialog(nil, &itemhit);
		switch (itemhit) {
		case OK_BRDITM:
			GetDialogItemText(horizitemh, str);
			if (strtouint32(str, &val) || val > (width/2 - 1)) {
				SysBeep(BEEPTICKS);
				uint32todstr(RECTINSET, str);
				SetDialogItemText(horizitemh, str);
				SelectDialogItemText(dlog, HORIZ_BRDITM, 0, 32767);
				break;
			}
			gBoundRectHInset = val;
			GetDialogItemText(vertitemh, str);
			if (strtouint32(str, &val) || val > (height/2 - 1)) {
				SysBeep(BEEPTICKS);
				uint32todstr(RECTINSET, str);
				SetDialogItemText(vertitemh, str);
				SelectDialogItemText(dlog, VERT_BRDITM, 0, 32767);
				break;
			}
			gBoundRectVInset = val;
			break;
		}
	} while (itemhit != OK_BRDITM);

	SetPort(oldport);
	DisposeDialog(dlog);
}

static void dotestmenu(UInt32 item)
{
	MenuHandle menu1, menu2;

	switch (item) {
	case RUN_ITEM:
		runtest(gTest);
		break;
	case NOREFRESH_ITEM:
		gNoRefresh = !gNoRefresh;
		CheckItem(gTestMenu, item, gNoRefresh);
		break;
	case NOCURSORHIDE_ITEM:
		gNoCursorHide = !gNoCursorHide;
		CheckItem(gTestMenu, item, gNoCursorHide);
		break;
	case COPYBITS_ITEM:
	case LINES_ITEM:
	case TEXT_ITEM:
	case FRAMESHAPE_ITEM:
	case PAINTSHAPE_ITEM:
	case ERASESHAPE_ITEM:
	case INVERTSHAPE_ITEM:
	case PATFILLSHAPE_ITEM:
		if (item == gTest)	// No change, we're done
			break;
		/* We're choosing a new test */
		CheckItem(gTestMenu, gTest, false);
		CheckItem(gTestMenu, item, true);
		menu1 = nil;
		if (gTest == COPYBITS_ITEM) {
			menu1 = gCopyBitsMenu;
		} else if (gTest == LINES_ITEM) {
			menu1 = gLinesMenu;
		} else if (gTest == TEXT_ITEM) {
			menu1 = gTextMenu;
		}
		if (menu1)
			DisableItem(menu1, 0);

		menu2 = nil;
		if (item == COPYBITS_ITEM) {
			menu2 = gCopyBitsMenu;
		} else if (item == LINES_ITEM) {
			menu2 = gLinesMenu;
		} else if (item == TEXT_ITEM) {
			menu2 = gTextMenu;
		}
		if (menu2)
			EnableItem(menu2, 0);
		if (menu1 || menu2)
			InvalMenuBar();
		gTest = item;
		break;
	case RECTANGLE_ITEM:
	case REGION_ITEM:
	case OVAL_ITEM:
	case ROUNDRECT_ITEM:
	case ARC_ITEM:
	case POLYGON_ITEM:
		if (item == gShape)	// No change, we're done
			break;
		CheckItem(gTestMenu, gShape, false);
		CheckItem(gTestMenu, item, true);
		gShape = item;
		break;
	case BOUNDRECT_ITEM:
		doboundrectdlog();
		break;
	}
}

static void dodestmenu(UInt32 item)
{
	UInt32 saveitem;

	switch (item) {
	case DESTDITHER_ITEM:
		gDitherDest = !gDitherDest;
		CheckItem(gDestMenu, DESTDITHER_ITEM, gDitherDest);
		if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
			if (gDestPictBufGW == nil || gDestGW == nil)
				break;
			copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);
		}
		invalwind(gDestWind);	// Invalidate so it redraws
		break;
	case SWAPFGBG_ITEM:
		gSwapFgBg = !gSwapFgBg;
		CheckItem(gDestMenu, SWAPFGBG_ITEM, gSwapFgBg);
		CheckItem(gFgColorMenu, gFgColor, false);
		CheckItem(gBgColorMenu, gBgColor, false);
		saveitem = gFgColor;
		gFgColor = gBgColor;
		gBgColor = saveitem;
		CheckItem(gFgColorMenu, gFgColor, true);
		CheckItem(gBgColorMenu, gBgColor, true);
		/* Keep 'Other' RGB values properly maintained */
		if (gFgColor == OTHERCOLOR_ITEM) {
			if (gSwapFgBg) {
				gFgSaveRGB = gFgOtherRGB;
				gFgOtherRGB = gBgOtherRGB;
			} else {
				gFgOtherRGB = gFgSaveRGB;
			}
		}
		if (gBgColor == OTHERCOLOR_ITEM) {
			if (gSwapFgBg) {
				gBgSaveRGB = gBgOtherRGB;
				gBgOtherRGB = gFgOtherRGB;
			} else {
				gBgOtherRGB = gBgSaveRGB;
			}
		}
		break;
	case RESETCOLORS_ITEM:
		gSwapFgBg = false;
		CheckItem(gDestMenu, SWAPFGBG_ITEM, false);
		CheckItem(gFgColorMenu, gFgColor, false);
		CheckItem(gBgColorMenu, gBgColor, false);
		gFgColor = BLACK_ITEM;
		gBgColor = WHITE_ITEM;
		CheckItem(gFgColorMenu, BLACK_ITEM, true);
		CheckItem(gBgColorMenu, WHITE_ITEM, true);
		break;
	case USECLIPRGN_ITEM:
		gUseClipRgn = !gUseClipRgn;
		CheckItem(gDestMenu, USECLIPRGN_ITEM, gUseClipRgn);
		break;
	case CLIPRGN_ITEM:
		DBUGPRINTF(("CLIPRGN_ITEM: supposed to be a hierarchical menu!!"));
		break;
	}
}

static void docopybitsmenu(UInt32 item)
{
	SInt32 err;
	UInt32 width, height;
	Rect r;

	switch (item) {
	case USESEPSRC_ITEM:
		gSepSrc = !gSepSrc;
		CheckItem(gCopyBitsMenu, USESEPSRC_ITEM, gSepSrc);
		if (gSepSrc) {
			dimtowh(gSrcDim, &width, &height);
			SizeWindow(gSrcWind, width, height, false);
			r = gSrcWind->portRect;
			/* Allocate the GWorld in main memory (keepLocal) */
			if ((err = NewGWorld(&gSrcPictBufGW, 32, &r, nil, nil, keepLocal)) != noErr) {
				erroralert(CANTGETGWORLD_NDX);
				break;
			}
			if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
				if ((err = allocgworld(gSrcLoc, &r, &gSrcGW)) != 0) {
					erroralert(err);
					break;
				}
			}
			if ((err = drawpicttogworld(gSrcImage, gSrcPictBufGW)) != 0) {
				erroralert(err);
				break;
			}
			if (gSrcLoc != WINDOW_ITEM)	// Source is offscreen GWorld
				copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);
			ShowWindow(gSrcWind);
		} else {
			if (gSrcGW) {
				DisposeGWorld(gSrcGW);
				gSrcGW = nil;
			}
			if (gSrcPictBufGW) {	// Dispose of source picture buffer GWorld
				DisposeGWorld(gSrcPictBufGW);
				gSrcPictBufGW = nil;
			}
			HideWindow(gSrcWind);
		}
		break;
	case SRCDITHER_ITEM:
		gDitherSrc = !gDitherSrc;
		CheckItem(gCopyBitsMenu, SRCDITHER_ITEM, gDitherSrc);
		if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
			if (gSrcPictBufGW == nil || gSrcGW == nil)
				break;
			copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);
		}
		invalwind(gSrcWind);	// Invalidate so it redraws
		break;
	case USEMASKRGN_ITEM:
		gUseMaskRgn = !gUseMaskRgn;
		CheckItem(gCopyBitsMenu, USEMASKRGN_ITEM, gUseMaskRgn);
		break;
	}
}

static void dolinesmenu(UInt32 item)
{
	if (item == gLines)	// No change, we're done
		return;
	/* We're choosing a new line type */
	CheckItem(gLinesMenu, gLines, false);
	CheckItem(gLinesMenu, item, true);
	gLines = item;
}

static void dotextmenu(UInt32 item)
{
	switch (item) {
	case STRING_ITEM:
		break;
	}
}

// Return non-zero if two Pascal-style strings are equal
static int pstrequal(unsigned char *strp1, unsigned char *strp2)
{
	int len, i;

	len = *strp1;	// Length byte
	for (i = 0; i <= len; i++) {	// Make sure the length bytes match, too
		if (strp1[i] != strp2[i])	// If they don't match, return zero
			return(0);
	}
	return(1);	// The length and the string characters are the same, return non-zero
}

static void dolutdither(void)
{
	int drvrrefnum;
	GrafPtr oldport;
	GDHandle gdh;
	GDPtr gdp;
	PixMapPtr pmp;
	unsigned int utableindex;
	AuxDCEHandle *unittable;
	AuxDCEPtr dcep;
	DRVRHeaderPtr drvrhdrp;
	Rect wr;
	static unsigned char *drivername = "\p.Display_NV";

	for (gdh = GetDeviceList(); gdh != nil; gdh = GetNextDevice(gdh)) {
		drvrrefnum = (*gdh)->gdRefNum;	// Signed number (should always be negative)
		utableindex = ~drvrrefnum;		// Convert negative refnum to positive Unit Table index
		unittable = (AuxDCEHandle *)LMGetUTableBase();	// Get the base address of the Unit Table
		dcep = *(unittable[utableindex]);	// Index into the table to get the handle to our AuxDCE
		drvrhdrp = (DRVRHeaderPtr)dcep->dCtlDriver;	// Get a pointer or handle to our driver header
		if (dcep->dCtlFlags & dRAMBasedMask)		// Means it was a handle, not a pointer
			drvrhdrp = *(DRVRHeaderHandle)drvrhdrp;	// De-reference to get the pointer
		if (pstrequal(drvrhdrp->drvrName, drivername))
			goto gotgdev;
	}
	erroralert(CANTGETNVBOARD_NDX);
	return;

gotgdev:
	pmp = *(*gdh)->gdPMap;
	if (pmp->pixelSize != 32) {
		erroralert(NEEDMILLIONS_NDX);
		return;
	}
	wr = gDestWind->portRect;
	GetPort(&oldport);
	SetPort(gDestWind);
	LocalToGlobal((Point *)&wr.top);
	LocalToGlobal((Point *)&wr.bottom);
	SetPort(oldport);
	// See if the window's global content rect is contained within the NV gdRect
	gdp = *gdh;
	if (wr.top < gdp->gdRect.top || wr.left < gdp->gdRect.left ||
			wr.bottom > gdp->gdRect.bottom || wr.right > gdp->gdRect.right) {
		erroralert(DESTNOTONNV_NDX);
		return;
	}
	// Now we have the destination window entirely on the 32-bit NVIDIA screen.
	
}

static void dospatialdither(void)
{
}

static void doexpmenu(UInt32 item)
{
	switch (item) {
	case LUTDITHER_ITEM:
		dolutdither();
		break;
	case SPATIALDITHER_ITEM:
		dospatialdither();
		break;
	}
}

static void dorgnshapemenu(UInt32 item)
{
	if (item == gRgnShape)	// No change, we're done
		return;
	/* We're choosing a new region */
	CheckItem(gRgnShapeMenu, gRgnShape, false);
	CheckItem(gRgnShapeMenu, item, true);
	gRgnShape = item;
}

static void docliprgnmenu(UInt32 item)
{
	if (item == gClipRgn)	// No change, we're done
		return;
	/* We're choosing a new region */
	CheckItem(gClipRgnMenu, gClipRgn, false);
	CheckItem(gClipRgnMenu, item, true);
	gClipRgn = item;
}

static void domaskrgnmenu(UInt32 item)
{
	if (item == gMaskRgn)	// No change, we're done
		return;
	/* We're choosing a new region */
	CheckItem(gMaskRgnMenu, gMaskRgn, false);
	CheckItem(gMaskRgnMenu, item, true);
	gMaskRgn = item;
}

static void dodestlocmenu(UInt32 item)
{
	SInt32 err;

	if (item == gDestLoc)	// No change, we're done
		return;
	/* We're choosing a new dest image location */
	CheckItem(gDestLocMenu, gDestLoc, false);
	CheckItem(gDestLocMenu, item, true);
	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
		if (gDestGW) {
			DisposeGWorld(gDestGW);
			gDestGW = nil;
		}
	}
	if (gDestPictBufGW == nil)
		return;
	if (item != WINDOW_ITEM) {	// New destination is offscreen GWorld
		if ((err = allocgworld(item, &gDestPictBufGW->portRect, &gDestGW)) != 0) {
			erroralert(err);
			return;
		}
		copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);
	}
	gDestLoc = item;
	/* Invalidate the destination window so it will redraw */
	invalwind(gDestWind);
	setlocdimtitle(gDestWind);
}

static void dodestdimmenu(UInt32 item)
{
	SInt32 err;
	UInt32 width, height;
	Rect r;

	if (item == gDestDim)	// No change, we're done
		return;
	/* We're choosing new dest image dimensions */
	CheckItem(gDestDimMenu, gDestDim, false);
	CheckItem(gDestDimMenu, item, true);
	gDestDim = item;
	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
		if (gDestGW) {
			DisposeGWorld(gDestGW);
			gDestGW = nil;
		}
	}
	if (gDestPictBufGW) {	// Dispose of destination picture buffer GWorld
		DisposeGWorld(gDestPictBufGW);
		gDestPictBufGW = nil;
	}
	dimtowh(gDestDim, &width, &height);
	r.top = 0;
	r.left = 0;
	r.bottom = height;
	r.right = width;
	/* Allocate the GWorld in main memory (keepLocal) */
	if ((err = NewGWorld(&gDestPictBufGW, 32, &r, nil, nil, keepLocal)) != 0) {
		erroralert(CANTGETGWORLD_NDX);
		return;
	}
	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
		if ((err = allocgworld(gDestLoc, &r, &gDestGW)) != 0) {
			erroralert(err);
			return;
		}
	}
	if ((err = drawpicttogworld(gDestImage, gDestPictBufGW)) != 0) {
		erroralert(err);
		return;
	}
	if (gDestLoc != WINDOW_ITEM)	// Destination is offscreen GWorld
		copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);

	SizeWindow(gDestWind, width, height, false);
	/* Invalidate the destination window so it will redraw */
	invalwind(gDestWind);
	setlocdimtitle(gDestWind);
}

static void dodestimagemenu(UInt32 item)
{
	SInt32 err;

	if (item == gDestImage)	// No change, we're done
		return;
	/* We're choosing a new dest image */
	CheckItem(gDestImageMenu, gDestImage, false);
	CheckItem(gDestImageMenu, item, true);
	gDestImage = item;
	if (gDestPictBufGW == nil)
		return;
	if ((err = drawpicttogworld(gDestImage, gDestPictBufGW)) != 0) {
		erroralert(err);
		return;
	}
	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
		if (gDestGW == nil)
			return;
		copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);
	}
	/* Invalidate the destination window so it will redraw */
	invalwind(gDestWind);
}

static void dofgcolormenu(UInt32 item)
{
	if (item == OTHERCOLOR_ITEM) {	// Do a new custom color
	} else if (item == gFgColor) {	// No change, we're done
		return;
	}
	gSwapFgBg = false;
	CheckItem(gDestMenu, SWAPFGBG_ITEM, false);
	/* We've chosen a new foreground color item */
	CheckItem(gFgColorMenu, gFgColor, false);
	CheckItem(gFgColorMenu, item, true);
	gFgColor = item;
}

static void dobgcolormenu(UInt32 item)
{
	if (item == OTHERCOLOR_ITEM) {	// Do a new custom color
	} else if (item == gBgColor) {	// No change, we're done
		return;
	}
	gSwapFgBg = false;
	CheckItem(gDestMenu, SWAPFGBG_ITEM, false);
	/* We've chosen a new background color item */
	CheckItem(gBgColorMenu, gBgColor, false);
	CheckItem(gBgColorMenu, item, true);
	gBgColor = item;
}

static void dopensizemenu(UInt32 item)
{
	if (item == gPenSize)	// No change, we're done
		return;
	/* We're choosing a new pen size */
	CheckItem(gPenSizeMenu, gPenSize, false);
	CheckItem(gPenSizeMenu, item, true);
	gPenSize = item;
}

static void dopenmodemenu(UInt32 item)
{
	switch (item) {
	case PATCOPY_ITEM:
	case PATOR_ITEM:
	case PATXOR_ITEM:
	case PATBIC_ITEM:
	case NOTPATCOPY_ITEM:
	case NOTPATOR_ITEM:
	case NOTPATXOR_ITEM:
	case NOTPATBIC_ITEM:
	case PATCOPYPLUSTRANSP_ITEM:
		if (item == gPenMode) // No change, we're done
			break;
		/* We're choosing a new pen mode */
		CheckItem(gPenModeMenu, gPenMode, false);
		CheckItem(gPenModeMenu, item, true);
		gPenMode = item;
		break;
	case PM_PLUSHILITE_ITEM:
		gPMPlusHilite = !gPMPlusHilite;
		CheckItem(gPenModeMenu, PM_PLUSHILITE_ITEM, gPMPlusHilite);
		break;
	}
}

static void dopenpatmenu(UInt32 item)
{
	if (item == gPenPat) // No change, we're done
		return;
	/* We're choosing a new pen pattern */
	CheckItem(gPenPatMenu, gPenPat, false);
	CheckItem(gPenPatMenu, item, true);
	gPenPat = item;
}

static void dobgpatmenu(UInt32 item)
{
	if (item == gBgPat) // No change, we're done
		return;
	/* We're choosing a new background pattern */
	CheckItem(gBgPatMenu, gBgPat, false);
	CheckItem(gBgPatMenu, item, true);
	gBgPat = item;
}

static void dofillpatmenu(UInt32 item)
{
	if (item == gFillPat) // No change, we're done
		return;
	/* We're choosing a new fill pattern */
	CheckItem(gFillPatMenu, gFillPat, false);
	CheckItem(gFillPatMenu, item, true);
	gFillPat = item;
}

static void dosrclocmenu(UInt32 item)
{
	SInt32 err;

	if (item == gSrcLoc) // No change, we're done
		return;
	/* We're choosing a new source image location */
	CheckItem(gSrcLocMenu, gSrcLoc, false);
	CheckItem(gSrcLocMenu, item, true);
	if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
		if (gSrcGW) {
			DisposeGWorld(gSrcGW);
			gSrcGW = nil;
		}
	}
	if (gSrcPictBufGW == nil)
		return;
	if (item != WINDOW_ITEM) {	// New source is offscreen GWorld
		if ((err = allocgworld(item, &gSrcPictBufGW->portRect, &gSrcGW)) != 0) {
			erroralert(err);
			return;
		}
		copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);
	}
	gSrcLoc = item;
	/* Invalidate the source window so it will redraw */
	invalwind(gSrcWind);
	setlocdimtitle(gSrcWind);
}

static void dosrcdimmenu(UInt32 item)
{
	SInt32 err;
	UInt32 width, height;
	Rect r;

	if (item == gSrcDim) // No change, we're done
		return;
	/* We're choosing new source image dimensions */
	CheckItem(gSrcDimMenu, gSrcDim, false);
	CheckItem(gSrcDimMenu, item, true);
	gSrcDim = item;

	if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
		if (gSrcGW) {
			DisposeGWorld(gSrcGW);
			gSrcGW = nil;
		}
	}
	if (gSrcPictBufGW) {	// Dispose of source picture buffer GWorld
		DisposeGWorld(gSrcPictBufGW);
		gSrcPictBufGW = nil;
	}
	dimtowh(gSrcDim, &width, &height);
	r.top = 0;
	r.left = 0;
	r.bottom = height;
	r.right = width;
	/* Allocate the GWorld in main memory (keepLocal) */
	if ((err = NewGWorld(&gSrcPictBufGW, 32, &r, nil, nil, keepLocal)) != 0) {
		erroralert(CANTGETGWORLD_NDX);
		return;
	}
	if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
		if ((err = allocgworld(gSrcLoc, &r, &gSrcGW)) != 0) {
			erroralert(err);
			return;
		}
	}
	if ((err = drawpicttogworld(gSrcImage, gSrcPictBufGW)) != 0) {
		erroralert(err);
		return;
	}
	if (gSrcLoc != WINDOW_ITEM)	// Source is offscreen GWorld
		copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);

	SizeWindow(gSrcWind, width, height, false);
	/* Invalidate the source window so it will redraw */
	invalwind(gSrcWind);
	setlocdimtitle(gSrcWind);
}

static void dosrcimagemenu(UInt32 item)
{
	SInt32 err;

	if (item == gSrcImage) // No change, we're done
		return;
	/* We're choosing a new source image */
	CheckItem(gSrcImageMenu, gSrcImage, false);
	CheckItem(gSrcImageMenu, item, true);
	gSrcImage = item;
	if (gSrcPictBufGW == nil)
		return;
	if ((err = drawpicttogworld(gSrcImage, gSrcPictBufGW)) != 0) {
		erroralert(err);
		return;
	}
	if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
		if (gSrcGW == nil)
			return;
		copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);
	}
	/* Invalidate the source window so it will redraw */
	invalwind(gSrcWind);
}

static void doxfermodemenu(UInt32 item)
{
	switch (item) {
	case X_SRCCOPY_ITEM:
	case X_SRCOR_ITEM:
	case X_SRCXOR_ITEM:
	case X_SRCBIC_ITEM:
	case X_NOTSRCCOPY_ITEM:
	case X_NOTSRCOR_ITEM:
	case X_NOTSRCXOR_ITEM:
	case X_NOTSRCBIC_ITEM:
	case X_ADDOVER_ITEM:
	case X_ADDPIN_ITEM:
	case X_SUBOVER_ITEM:
	case X_SUBPIN_ITEM:
	case X_ADMAX_ITEM:
	case X_ADMIN_ITEM:
	case X_BLEND_ITEM:
	case X_TRANSPARENT_ITEM:
	case X_HILITE_ITEM:
		if (item == gXferMode) // No change, we're done
			break;
		/* We're choosing a new transfer mode */
		CheckItem(gXferModeMenu, gXferMode, false);
		CheckItem(gXferModeMenu, item, true);
		gXferMode = item;
		break;
	case X_PLUSDITHER_ITEM:
		gXPlusDither = !gXPlusDither;
		CheckItem(gXferModeMenu, X_PLUSDITHER_ITEM, gXPlusDither);
		break;
	}
}

static void dosrcrectmenu(UInt32 item)
{
	if (item == OTHERRECT_ITEM) {	/* Do a new custom rectangle */
	}
	if (item == gSrcRect) // No change, we're done
		return;
	/* We've chosen a new source rect */
	CheckItem(gSrcRectMenu, gSrcRect, false);
	CheckItem(gSrcRectMenu, item, true);
	gSrcRect = item;
}

static void dodestrectmenu(UInt32 item)
{
	if (item == OTHERRECT_ITEM) {	/* Do a new custom rectangle */
	}
	if (item == gDestRect) // No change, we're done
		return;
	/* We've chosen a new dest rect */
	CheckItem(gDestRectMenu, gDestRect, false);
	CheckItem(gDestRectMenu, item, true);
	gDestRect = item;
}

static void dotextmodemenu(UInt32 item)
{
	switch (item) {
	case T_SRCCOPY_ITEM:
	case T_SRCOR_ITEM:
	case T_SRCXOR_ITEM:
	case T_SRCBIC_ITEM:
	case T_NOTSRCCOPY_ITEM:
	case T_NOTSRCOR_ITEM:
	case T_NOTSRCXOR_ITEM:
	case T_NOTSRCBIC_ITEM:
	case T_ADDOVER_ITEM:
	case T_ADDPIN_ITEM:
	case T_SUBOVER_ITEM:
	case T_SUBPIN_ITEM:
	case T_ADMAX_ITEM:
	case T_ADMIN_ITEM:
	case T_BLEND_ITEM:
	case T_TRANSPARENT_ITEM:
	case T_GRAYISHTEXTOR_ITEM:
		if (item == gTextMode) // No change, we're done
			break;
		/* We're choosing a new transfer mode */
		CheckItem(gTextModeMenu, gTextMode, false);
		CheckItem(gTextModeMenu, item, true);
		gTextMode = item;
		break;
	case T_PLUSHILITE_ITEM:
		gTPlusHilite = !gTPlusHilite;
		CheckItem(gTextModeMenu, T_PLUSHILITE_ITEM, gTPlusHilite);
		break;
	case T_PLUSMASK_ITEM:
		gTPlusMask = !gTPlusMask;
		CheckItem(gTextModeMenu, T_PLUSMASK_ITEM, gTPlusMask);
		break;
	}
}


static void menucmd(long menuitem, short what)	/* Execute the appropriate routine */
{
#pragma unused(what)
	UInt32 menu, item;		/* Menu ID and item number */

	menu = menuitem >> 16;
	item = (UInt16)menuitem; //# check generated code for this

	switch (menu) {
	case APPLEMENU_ID:
		doapplemenu(item);
		break;
	case FILEMENU_ID:
		dofilemenu(item);
		break;
	case EDITMENU_ID:
		doeditmenu(item);
		break;
	case TESTMENU_ID:
		dotestmenu(item);
		break;
	case DESTMENU_ID:
		dodestmenu(item);
		break;
	case COPYBITSMENU_ID:
		docopybitsmenu(item);
		break;
	case LINESMENU_ID:
		dolinesmenu(item);
		break;
	case TEXTMENU_ID:
		dotextmenu(item);
		break;
	case EXPMENU_ID:
		doexpmenu(item);
		break;
	case RGNSHAPEMENU_ID:
		dorgnshapemenu(item);
		break;
	case CLIPRGNMENU_ID:
		docliprgnmenu(item);
		break;
	case MASKRGNMENU_ID:
		domaskrgnmenu(item);
		break;
	case DESTLOCMENU_ID:
		dodestlocmenu(item);
		break;
	case DESTDIMMENU_ID:
		dodestdimmenu(item);
		break;
	case DESTIMAGEMENU_ID:
		dodestimagemenu(item);
		break;
	case FGCOLORMENU_ID:
		dofgcolormenu(item);
		break;
	case BGCOLORMENU_ID:
		dobgcolormenu(item);
		break;
	case PENSIZEMENU_ID:
		dopensizemenu(item);
		break;
	case PENMODEMENU_ID:
		dopenmodemenu(item);
		break;
	case PENPATMENU_ID:
		dopenpatmenu(item);
		break;
	case BGPATMENU_ID:
		dobgpatmenu(item);
		break;
	case FILLPATMENU_ID:
		dofillpatmenu(item);
		break;
	case SRCLOCMENU_ID:
		dosrclocmenu(item);
		break;
	case SRCDIMMENU_ID:
		dosrcdimmenu(item);
		break;
	case SRCIMAGEMENU_ID:
		dosrcimagemenu(item);
		break;
	case XFERMODEMENU_ID:
		doxfermodemenu(item);
		break;
	case SRCRECTMENU_ID:
		dosrcrectmenu(item);
		break;
	case DESTRECTMENU_ID:
		dodestrectmenu(item);
		break;
	case TEXTMODEMENU_ID:
		dotextmodemenu(item);
		break;
	}

	HiliteMenu(0);			/* Invert the menu title back to normal */
}

static void closewind(WindowPtr wind)
{
	if (wind == nil)	/* No visible windows */
		return;
	if (((WindowPeek)wind)->windowKind < 0) {	/* Negative windowKind: it's a DA */
		CloseDeskAcc(((WindowPeek)wind)->windowKind);	/* windowKind = DA's refnum */
		return;
	}
	HideWindow(wind);	// Could dispose of it if we're sure we don't need it again
}

static void mouseclick(EventRecord *eventp)
{
	short part;
	WindowPtr wind;

	part = FindWindow(eventp->where, &wind);
	switch (part) {		/* Switch on the "window part" */
	case inMenuBar:
		configmenus();	/* Make sure the appropriate items are selected */
		menucmd(MenuSelect(eventp->where), eventp->what);	/* Process the selection */
		break;
	case inSysWindow:	/* DA or other system window */
		SystemClick(eventp, wind);
		break;
	case inContent:		/* Click in the content area of some window */
		if (wind != FrontWindow())	/* Is the window clicked on in front? */
			SelectWindow(wind); 		/* If not, bring it forward now */
		break;
	case inDrag:
		/* screenBits.bounds for the rect constrains to all GDevices */
		DragWindow(wind, eventp->where, &qd.screenBits.bounds);
		break;
	case inGoAway:
		if (TrackGoAway(wind, eventp->where))
			closewind(wind);
		break;
	}
}

static void keypress(EventRecord *eventp)	/* Handles command-key keyboard events */
{
	short key;

	key = eventp->message & charCodeMask;
	if (eventp->modifiers & cmdKey) {	/* Is the command key down? */
		configmenus();	/* Make sure the appropriate items are selected */
		/* Process the menu equivalent of that command key (if any) */
		menucmd(MenuKey(key), eventp->what);
	} else {	/* Not a command key */
	}
}

static void dimtowh(UInt32 dimitem, UInt32 *widthp, UInt32 *heightp)
{
	switch (dimitem) {
	case DIM_512X384_ITEM:
		*widthp = 512;
		*heightp = 384;
		break;
	case DIM_600X450_ITEM:
		*widthp = 600;
		*heightp = 450;
		break;
	case DIM_640X480_ITEM:
		*widthp = 640;
		*heightp = 480;
		break;
	case DIM_1024X768_ITEM:
		*widthp = 1024;
		*heightp = 768;
		break;
	}
}

static void rectitemtorect(UInt32 rectitem, Rect *inr, Rect *otherr, Rect *outr)
{
	*outr = *inr;	// Copy the whole input rect as default

	switch (rectitem) {
	case TOP50_ITEM:
		outr->bottom = (inr->top + inr->bottom) / 2;
		break;
	case BOTTOM50_ITEM:
		outr->top = (inr->top + inr->bottom) / 2;
		break;
	case LEFT50_ITEM:
		outr->right = (inr->left + inr->right) / 2;
		break;
	case RIGHT50_ITEM:
		outr->left = (inr->left + inr->right) / 2;
		break;
	case TOP99_ITEM:
		outr->bottom = inr->bottom - 1;
		break;
	case BOTTOM99_ITEM:
		outr->top = inr->top + 1;
		break;
	case LEFT99_ITEM:
		outr->right = inr->right - 1;
		break;
	case RIGHT99_ITEM:
		outr->left = inr->left + 1;
		break;
	case FULLSIZE_ITEM:
		// *outr = *inr
		break;
	case OTHERRECT_ITEM:
		*outr = *otherr;
		/* Constrain the 'other' rectangle to legal boundaries */
		if (outr->top < inr->top || outr->top >= inr->bottom)
			outr->top = inr->top;
		if (outr->left < inr->left || outr->left >= inr->right)
			outr->left = inr->left;
		if (outr->bottom > inr->bottom || outr->bottom <= inr->top)
			outr->bottom = inr->bottom;
		if (outr->right > inr->right || outr->right <= inr->left)
			outr->right = inr->right;
		break;
	}
}

static SInt32 allocgworld(UInt32 item, Rect *rectp, GWorldPtr *gwpp)
{
	SInt32 err;
	UInt32 depth;
	CTabHandle cth;

	switch (item) {
	case OFFSCRN1BW_ITEM:
		depth = 1;
		cth = gCTab1BW;
		break;
	case OFFSCRN8G_ITEM:
		depth = 8;
		cth = gCTab8G;
		break;
	case OFFSCRN8C_ITEM:
		depth = 8;
		cth = gCTab8C;
		break;
	case OFFSCRN16C_ITEM:
		depth = 16;
		cth = nil;
		break;
	case OFFSCRN24C_ITEM:
		depth = 32;
		cth = nil;
		break;
	}
	/* Allocate the GWorld in main memory (keepLocal) */
	if ((err = NewGWorld(gwpp, depth, rectp, cth, nil, keepLocal)) != 0) {
		return(CANTGETGWORLD_NDX);
	}
	return(0);
}

static SInt32 drawpicttogworld(UInt32 pictitem, GWorldPtr gwp)
{
	SInt32 err;
	CGrafPtr oldport;
	GDHandle oldgdh;
	PixMapHandle pmh;
	PicHandle pich;

	err = 0;
	if ((pich = (PicHandle)Get1Resource('PICT', PICT_BASEID - 1 + pictitem)) == nil)
		err = CANTGETPICT_NDX;
	GetGWorld(&oldport, &oldgdh);
	/* Draw the picture into the GWorld */
	SetGWorld(gwp, nil);
	pmh = GetGWorldPixMap(gwp);
	if (LockPixels(pmh)) {	// Drawing with the PixMap is OK
		if (pich) {
			EraseRect(&gwp->portRect);	// Set to a known state
			DrawPicture(pich, &gwp->portRect);
		} else {	// No picture, so fill it with something
			FillRect(&gwp->portRect, &qd.ltGray);
		}
		UnlockPixels(pmh);
	}
	SetGWorld(oldport, oldgdh);
	ReleaseResource((Handle)pich);
	return(err);
}

static void copyGWtoGW(GWorldPtr srcgwp, GWorldPtr destgwp, UInt32 dither)
{
	UInt32 mode;
	CGrafPtr oldport;
	GDHandle oldgdh;
	PixMapHandle srcpmh, destpmh;

	mode = srcCopy;
	if (dither)
		mode = srcCopy+ditherCopy;
	GetGWorld(&oldport, &oldgdh);
	/* Draw the picture into the GWorld */
	SetGWorld(destgwp, nil);
	srcpmh = GetGWorldPixMap(srcgwp);
	destpmh = GetGWorldPixMap(destgwp);
	if (LockPixels(srcpmh)) {
		if (LockPixels(destpmh)) {
			CopyBits(&((GrafPtr)srcgwp)->portBits, &((GrafPtr)destgwp)->portBits,
					&srcgwp->portRect, &destgwp->portRect, mode, nil);
			UnlockPixels(destpmh);
		}
		UnlockPixels(srcpmh);
	}
	SetGWorld(oldport, oldgdh);
}

static void setlocdimtitle(WindowPtr wind)
{
	unsigned char str[256], locstr[128], menuitemstr[128];

	if (wind == gSrcWind) {
		if (gSrcLoc == WINDOW_ITEM) {
			GetMenuItemText(gSrcLocMenu, WINDOW_ITEM, locstr);
		} else {
			GetMenuItemText(gSrcLocMenu, gSrcLoc, menuitemstr);
			paramstr(locstr, SOURCELOC_NDX, menuitemstr, nil, nil, nil);
		}
		GetMenuItemText(gSrcDimMenu, gSrcDim, menuitemstr);
	} else {	// Must be destination
		if (gDestLoc == WINDOW_ITEM) {
			GetMenuItemText(gDestLocMenu, WINDOW_ITEM, locstr);
		} else {
			GetMenuItemText(gDestLocMenu, gDestLoc, menuitemstr);
			paramstr(locstr, DESTLOC_NDX, menuitemstr, nil, nil, nil);
		}
		GetMenuItemText(gDestDimMenu, gDestDim, menuitemstr);
	}
	paramstr(str, WINDTITLE_NDX, locstr, menuitemstr, nil, nil);
	SetWTitle(wind, str);
}

static SInt32 initdest(void)
{
	SInt32 err;
	UInt32 width, height;
	Rect r;

	/* We initially bring up the window in a hidden state */
	if ((gDestWind = (GrafPtr)GetNewCWindow(DESTWIND_ID, 0L, (WindowPtr)-1L)) == nil)
		return(CANTGETWIND_NDX);
	dimtowh(gDestDim, &width, &height);
	SizeWindow(gDestWind, width, height, false);
	keepwindowonmain(gDestWind);	// Make sure it's not off the main display
	setlocdimtitle(gDestWind);

	if ((gCTab1BW = GetCTable(33)) == nil)	// 1-bit black & white table
		return(CANTGETCTABLE_NDX);
	if ((gCTab8G = GetCTable(40)) == nil)	// 8-bit gray-scale table
		return(CANTGETCTABLE_NDX);
	if ((gCTab8C = GetCTable(72)) == nil)	// 8-bit standard color table
		return(CANTGETCTABLE_NDX);

	r = gDestWind->portRect;
	/* Allocate the GWorld in main memory (keepLocal) */
	if ((err = NewGWorld(&gDestPictBufGW, 32, &r, nil, nil, keepLocal)) != 0) {
		return(CANTGETGWORLD_NDX);
	}
	if (gDestLoc != WINDOW_ITEM) {	// Destination is offscreen GWorld
		if ((err = allocgworld(gDestLoc, &r, &gDestGW)) != 0)
			return(err);
	}
	if ((err = drawpicttogworld(gDestImage, gDestPictBufGW)) != 0)
		return(err);
	if (gDestLoc != WINDOW_ITEM)	// Destination is offscreen GWorld
		copyGWtoGW(gDestPictBufGW, gDestGW, gDitherDest);
	return(0);
}

static void cleanupdest(void)
{
	if (gDestGW) {
		DisposeGWorld(gDestGW);
		gDestGW = nil;
	}
	if (gDestPictBufGW) {	// Dispose of destination picture buffer GWorld
		DisposeGWorld(gDestPictBufGW);
		gDestPictBufGW = nil;
	}
	DisposeCTable(gCTab8C);	// Dispose of the color tables we allocated
	DisposeCTable(gCTab8G);
	DisposeCTable(gCTab1BW);
	DisposeWindow(gDestWind);
}

static SInt32 initsource(void)
{
	SInt32 err;
	UInt32 width, height;
	Rect r;

	/* We initially bring up the window in a hidden state */
	if ((gSrcWind = (GrafPtr)GetNewCWindow(SOURCEWIND_ID, 0L, (WindowPtr)0L)) == nil)
		return(CANTGETWIND_NDX);
	dimtowh(gSrcDim, &width, &height);
	SizeWindow(gSrcWind, width, height, false);
	keepwindowonmain(gSrcWind);	// Make sure it's not off the main display
	setlocdimtitle(gSrcWind);

	if (!gSepSrc)	// No separate source, so we're done
		return(0);

	r = gSrcWind->portRect;
	/* Allocate the GWorld in main memory (keepLocal) */
	if ((err = NewGWorld(&gSrcPictBufGW, 32, &r, nil, nil, keepLocal)) != 0) {
		return(CANTGETGWORLD_NDX);
	}
	if (gSrcLoc != WINDOW_ITEM) {	// Source is offscreen GWorld
		if ((err = allocgworld(gSrcLoc, &r, &gSrcGW)) != 0)
			return(err);
	}
	if ((err = drawpicttogworld(gSrcImage, gSrcPictBufGW)) != 0)
		return(err);
	if (gSrcLoc != WINDOW_ITEM)	// Source is offscreen GWorld
		copyGWtoGW(gSrcPictBufGW, gSrcGW, gDitherSrc);
	return(0);
}

static void cleanupsrc(void)
{
	if (gSrcGW) {
		DisposeGWorld(gSrcGW);
		gSrcGW = nil;
	}
	if (gSrcPictBufGW) {	// Dispose of source picture buffer GWorld
		DisposeGWorld(gSrcPictBufGW);
		gSrcPictBufGW = nil;
	}
	DisposeWindow(gSrcWind);
}

static SInt32 initstatus(void)
{
	/* We initially bring up the window in a hidden state */
	if ((gStatusWind = (GrafPtr)GetNewCWindow(STATUSWIND_ID, 0L, (WindowPtr)0L)) == nil)
		return(CANTGETWIND_NDX);
	keepwindowonmain(gStatusWind);	// Make sure it's not off the main display
	return(0);
}

static void cleanupstatus(void)
{
	DisposeWindow(gStatusWind);
}

static SInt32 initregions(void)
{
	SInt32 err;
#define NUMPOLYPTS	17
	static SInt16 polypts[NUMPOLYPTS] = {
		0x0022, 0x001E, 0x001E, 0x0172, 0x023A, 0x0172, 0x001E, 0x0172,
		0x023A, 0x001E, 0x01B3, 0x0168, 0x012C, 0x001E, 0x00A5, 0x0172,
		0x001E
	};
	Rect maskrgnovalrect;
	static Rect bigrect = { 50, 50, 400, 550 };
	static Rect medrect = { 100, 100, 350, 500 };
	static Rect smallrect = { 150, 150, 300, 450 };
	PolyHandle polyh;
	RgnHandle rgnh, rgn2h;

	maskrgnovalrect = gDestWind->portRect;
	InsetRect(&maskrgnovalrect, 40, 140);

	/* Set up triangles and oval region */
	if ((polyh = (PolyHandle)NewHandle(NUMPOLYPTS*sizeof(SInt16))) == nil)
		return(CANTGETREGION_NDX);
	BlockMove((Ptr)polypts, (Ptr)*polyh, NUMPOLYPTS*sizeof(SInt16));
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FramePoly(polyh);
	FrameOval(&maskrgnovalrect);
	CloseRgn(rgnh);
	err = QDError();
	DisposeHandle((Handle)polyh);
	if (err)
		return(CANTGETREGION_NDX);
	gRgnh[TRIOVALRGN_ITEM] = rgnh;

	/* Set up rounded rectangle region */
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FrameRoundRect(&bigrect, RRECT_CURVATURE, RRECT_CURVATURE);
	CloseRgn(rgnh);
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	gRgnh[RRECTRGN_ITEM] = rgnh;

	/* Set up rectangle with hole region */
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FrameRect(&bigrect);
	CloseRgn(rgnh);
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	if ((rgn2h = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FrameRect(&medrect);
	CloseRgn(rgn2h);
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	DiffRgn(rgnh, rgn2h, rgnh);
	DisposeRgn(rgn2h);
	gRgnh[RECTHOLERGN_ITEM] = rgnh;

	/* Set up the medium rectangular region */
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FrameRect(&medrect);
	CloseRgn(rgnh);
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	gRgnh[MEDRECTRGN_ITEM] = rgnh;

	/* Set up the small rectangular region */
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	FrameRect(&smallrect);
	CloseRgn(rgnh);
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	gRgnh[SMALLRECTRGN_ITEM] = rgnh;

	/* Set up the empty region */
	if ((rgnh = NewRgn()) == nil)
		return(CANTGETREGION_NDX);
	OpenRgn();
	CloseRgn(rgnh);	// Open and Close Region with no other operations makes an empty region
	err = QDError();
	if (err)
		return(CANTGETREGION_NDX);
	gRgnh[EMPTYRGN_ITEM] = rgnh;

	return(0);
}

static void updatedestwind(WindowPtr wind)
{
	UInt32 mode;
	GWorldPtr gwp;
	PixMapHandle pmh;

	mode = srcCopy;	// Default
	if (gDestLoc == WINDOW_ITEM) {
		if (gDitherDest)
			mode = srcCopy+ditherCopy;
		gwp = gDestPictBufGW;
	} else {	// Offscreen GWorld
		gwp = gDestGW;
	}
	if (gwp == nil) {	// Must have run out of memory
		FillRect(&wind->portRect, &qd.ltGray);
		return;
	}
	pmh = GetGWorldPixMap(gwp);
	if (LockPixels(pmh)) {	// Drawing with the PixMap is OK
		CopyBits(&((GrafPtr)gwp)->portBits, &wind->portBits,
				&gwp->portRect, &wind->portRect, mode, nil);
		UnlockPixels(pmh);
	}
//InvertRect(&wind->portRect);
}

static void updatesrcwind(WindowPtr wind)
{
	UInt32 mode;
	GWorldPtr gwp;
	PixMapHandle pmh;

	mode = srcCopy;	// Default
	if (gSrcLoc == WINDOW_ITEM) {
		if (gDitherSrc)
			mode = srcCopy+ditherCopy;
		gwp = gSrcPictBufGW;
	} else {	// Offscreen GWorld
		gwp = gSrcGW;
	}
	if (gwp == nil) {	// Must have run out of memory
		FillRect(&wind->portRect, &qd.ltGray);
		return;
	}
	pmh = GetGWorldPixMap(gwp);
	if (LockPixels(pmh)) {	// Drawing with the PixMap is OK
		CopyBits(&((GrafPtr)gwp)->portBits, &wind->portBits,
				&gwp->portRect, &wind->portRect, mode, nil);
		UnlockPixels(pmh);
	}
}

static void updatestatuswind(WindowPtr wind)
{
	Rect r1, r2;
	unsigned char str[256], teststr[128], numstr1[16], numstr2[16];

	if (gLastTest == 0) {	// No tests have been run yet
		EraseRect(&wind->portRect);
		return;
	}
	r1 = wind->portRect;
	r2 = r1;
	r1.bottom = (r1.top + r1.bottom) / 2;
	r2.top = r1.bottom;
	GetMenuItemText(gTestMenu, gLastTest, teststr);
	sint32todstr(gTestEndTicks-gTestBeginTicks, numstr1);
	sint32todstr(gTestIterations, numstr2);
	paramstr(str, TIMING_NDX, teststr, numstr1, numstr2, nil);
	TETextBox(&str[1], str[0], &r1, teFlushDefault);
	uint32tohstr(gTestDestAddr, numstr1);
	uint32tohstr(gTestSrcAddr, numstr2);
	paramstr(str, ADDR_NDX, numstr1, numstr2, nil, nil);
	TETextBox(&str[1], str[0], &r2, teFlushDefault);
}

static void updatewind(WindowPtr wind)
{
	UInt32 refcon;
	GrafPtr oldport;

	GetPort(&oldport);
	SetPort(wind);
	BeginUpdate(wind);

	refcon = GetWRefCon(wind);
	if (refcon == DESTWINDREFCON)
		updatedestwind(wind);
	else if (refcon == SRCWINDREFCON)
		updatesrcwind(wind);
	else if (refcon == STATUSWINDREFCON)
		updatestatuswind(wind);

	EndUpdate(wind);
	SetPort(oldport);
}

static void invalwind(WindowPtr wind)
{
	GrafPtr oldport;

	GetPort(&oldport);
	SetPort(wind);
	InvalRect(&wind->portRect);
	SetPort(oldport);
}

static void activatewind(WindowPtr wind, short activate)
{
#pragma unused(wind, activate)
}

static void doeventloop(void)
{
	Boolean isourevent;
	EventRecord mainevent;

	do {
		isourevent = WaitNextEvent(everyEvent, &mainevent, 45, nil);
		if (isourevent) {
			switch (mainevent.what) {
			case mouseDown:
				mouseclick(&mainevent);
				break;
			case keyDown:
			case autoKey: 
				keypress(&mainevent);
				break;
			case updateEvt:
				updatewind((WindowPtr)mainevent.message);
				break;
			case activateEvt:
				activatewind((WindowPtr)mainevent.message,
								(mainevent.modifiers & activeFlag) != 0);
				break;
			}
		}
	} while (!gQuitFlag);
}
