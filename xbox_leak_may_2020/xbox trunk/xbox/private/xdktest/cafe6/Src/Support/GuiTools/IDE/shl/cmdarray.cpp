///////////////////////////////////////////////////////////////////////////////
//  CMDARRAY.CPP
//
//  Created by :            Date :
//      DavidGa                 9/16/93
//
//  Description :
//      CMD_STRUCT array for DoCommand function in the Workbench
//
// changes for new manubar 1/30/95 GeorgeCh - File/Edit/View/Insert/Tools/Window

#include "stdafx.h"
#include "uwbframe.h"
#include "..\sym\cmdids.h"
#include "Strings.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

CMD_STRUCT UIWBFrame::m_aCmdTable[] =
{
//	idCommand,				{ idMenu1,			idMenu2,			idMenu3 },			szMnemonic,	szAccel,

	// file commands

	ID_FILE_NEW,			{ IDS_FILEMENU,		IDS_NEW,			0 },				"%fn",		"^n",
    ID_FILE_OPEN,			{ IDS_FILEMENU,		IDS_OPEN,			0 },				"%fo",		"^o",
    ID_FILE_CLOSE,			{ IDS_FILEMENU,		IDS_CLOSE,			0 },				"%fc",      NULL,
    ID_FILE_SAVE,			{ IDS_FILEMENU,		IDS_SAVE,			0 },				"%fs",		"^s",
    ID_FILE_SAVE_AS,		{ IDS_FILEMENU,		IDS_SAVEAS,			0 },				"%fa",		"{f12}",
    IDM_OPEN_WORKSPACE,		{ IDS_FILEMENU,		IDS_OPENWRK,		0 },				"%fw",		NULL,
	IDM_SAVE_WORKSPACE,		{ IDS_FILEMENU,		IDS_SAVEWRK,		0 },				"%fv",		NULL,
    IDM_CLOSE_WORKSPACE,	{ IDS_FILEMENU,		IDS_CLOSEWRK,		0 },				"%fk",		NULL,
    IDM_FILE_SAVE_ALL,		{ IDS_FILEMENU,		IDS_SAVEALL,		0 },				"%fl",		NULL,
    IDM_EDIT_GREP,			{ IDS_FILEMENU,		IDS_FINDINFILES,	0 },				"%ff",		NULL,
    ID_FILE_PAGE_SETUP,		{ IDS_FILEMENU,		IDS_PAGESETUP,		0 },				"%fu",		NULL,
    ID_FILE_PRINT,			{ IDS_FILEMENU,		IDS_PRINT,			0 },				"%fp",		"^p",
    ID_APP_EXIT,			{ IDS_FILEMENU,		IDS_EXIT,			0 },				"%fx",		"%{f4}",

	// edit commands

    ID_EDIT_UNDO,			{ IDS_EDITMENU,		IDS_UNDO,			0 },				"%eu",		"^z",
    ID_EDIT_REDO,			{ IDS_EDITMENU,		IDS_REDO,			0 },				"%er",		"^a",
    ID_EDIT_CUT,			{ IDS_EDITMENU,		IDS_CUT,			0 },				"%et",		"^(x)",
    ID_EDIT_COPY,			{ IDS_EDITMENU,		IDS_COPY,			0 },				"%ec",		"^c",
    ID_EDIT_PASTE,			{ IDS_EDITMENU,		IDS_PASTE,			0 },				"%ep",		"^v",
    ID_EDIT_CLEAR,			{ IDS_EDITMENU,		IDS_CLEAR,			0 },				"%ed",		"{DEL}",
    ID_EDIT_SELECT_ALL,		{ IDS_EDITMENU,		IDS_SELECTALL,		0 },				"%el",		NULL,
    ID_EDIT_FIND_DLG,		{ IDS_EDITMENU,		IDS_FIND,			0 },				"%ef",		"%{f3}",
    ID_EDIT_REPLACE,		{ IDS_EDITMENU,		IDS_REPLACE,		0 },				"%ee",		NULL,
    IDM_GOTO_LINE,			{ IDS_EDITMENU,		IDS_GOTO,			0 },				"%eg",		"^(g)",
	IDM_RUN_SETBREAK,		{ IDS_EDITMENU,		IDS_BREAKPOINTS,	0 },				"%ek",		"^(b)",
    IDM_BOOKMARKS,			{ IDS_EDITMENU,		IDS_BOOKMARK,		0 },				"%eb",		NULL,

    IDM_WINDOW_SHOWPROPERTIES,{ IDS_EDITMENU,	IDS_PROPERTIES,		0 },				"%eo",		"%({ENTER})",

	// view commands

    IDM_EDIT_SYMBOLS,		{ IDS_VIEWMENU,		IDS_RESSYMBOLS,		0 },				"%vy",		NULL,
    IDM_FILE_SETINCLUDES,	{ IDS_VIEWMENU,		IDS_RESINCLUDES,	0 },				"%vn",		NULL,
    ID_WINDOW_FULLSCREEN,	{ IDS_VIEWMENU,		IDS_FULLSCREEN,		0 },				"%vu",		NULL,
    IDM_TOOLBAR_EDIT,		{ IDS_VIEWMENU,		IDS_TOOLBARS,		0 },				"%vt",		NULL,
    IDM_WINDOW_ERRORS,		{ IDS_VIEWMENU,		IDS_OUTPUT,			0 },				"%vo",		"%2",
    IDMY_CLASSWIZARD,		{ IDS_VIEWMENU,		IDS_CLASSWIZARD,	0 },				"%vw",		"^w",
    IDM_VIEW_WORKSPACE,		{ IDS_VIEWMENU,		IDS_WORKSPACE,		0 },				"%vp",		"%0", 
    IDM_VIEW_GLOBALWORKSPACE, { IDS_VIEWMENU,	IDS_GLOBALWORKSPACE, 0 },				"%vg",		"%+0", 
	IDM_VIEW_WATCH,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_WATCH		},				"%vda",		"%3",
    IDM_VIEW_VARS,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_VARIABLES	},				"%vdv",		"%4",
    IDM_VIEW_REGS,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_REGISTERS	},				"%vdr",		"%5",
    IDM_VIEW_MEM,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_MEMORY		},				"%vdm",		"%6",
    IDM_VIEW_STACK,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_CALLSTACK	},				"%vdc",		"%7",
    IDM_VIEW_ASM,			{ IDS_VIEWMENU,		IDS_VIEWDEBUGWNDS,			IDS_DISASSEMBLY	},				"%vdd",		"%8",
	IDM_VIEW_TOPIC,			{ IDS_VIEWMENU,		IDS_TOPICWND,		0 },				"%vt",		"%1",
	IDM_VIEW_RESULTS,		{ IDS_VIEWMENU,		IDS_RESULTWND,		0 },				"%vr",		NULL,

	// insert commands

    IDM_GALLERY,			{ IDS_PROJECTMENU,	IDS_ADDTOPROJECT,	IDS_GALLERY },				"%ic",		NULL,

	// project commands

	IDM_ADDMENU,			{ IDS_PROJECTMENU,	IDS_ADDTOPROJECT,	0 },				"%ii",		NULL,
    IDM_PROJITEM_ADD_FILES,	{ IDS_PROJECTMENU,	IDS_ADDTOPROJECT,	IDS_ADDFILES },				"%ii",		NULL,
    IDM_PROJ_ADD_DATASOURCE,{ IDS_PROJECTMENU,	IDS_ADDTOPROJECT,	IDS_DATACON },				"%pad",		NULL,
    IDM_PROJ_ACTIVEPROJMENU,{ IDS_PROJECTMENU,	IDS_SETACTIVEPROJ,	0 },				"%pv",		NULL,
    IDM_PROJECT_TARGET,		{ IDS_INSERTMENU,	IDS_TARGET,			0 },				"%ip",		NULL,
    IDM_INSERT_FROM_FILE,	{ IDS_INSERTMENU,	IDS_INSERTFILE,		0 },				"%if",		NULL,
    IDM_PROJITEM_COMPILE,	{ IDS_BUILDMENU,	IDS_COMPILE,		0 },				"%bc",		"^({F8})",
    IDM_PROJITEM_BUILD,		{ IDS_BUILDMENU,	IDS_BUILD,			0 },				"%bb",		"+({F8})",
    IDM_PROJITEM_REBUILD_ALL,{ IDS_BUILDMENU,	IDS_REBUILDALL,		0 },				"%br",		"%({F8})",
    IDM_PROJECT_STOP_BUILD,	{ IDS_BUILDMENU,	IDS_STOPBUILD,		0 },				"%bb",		"^({BREAK})",
    IDM_PROJECT_EXECUTE,	{ IDS_BUILDMENU,	IDS_EXECUTE,		0 },				"%bx",		"^({F5})",
    IDM_PROJECT_SETTINGS,	{ IDS_PROJECTMENU,	IDS_SETTINGS,		0 },				"%bs",		NULL,
    IDM_PROJECT_CONFIGURATIONS, { IDS_BUILDMENU,IDS_DEFALTCONFIG,   0 },                            "%bf",          NULL,

    IDM_PROJECT_SCAN_ALL,	{ IDS_BUILDMENU,	IDS_UPDATEDEP,		0 },				"%ba",		NULL,
    IDM_PROJECT_EXPORT_MAKEFILE,{ IDS_BUILDMENU,        IDS_EXPORTMAK,          0 },                            "%bk",          NULL,
    IDM_RUN_PROFILE,		{ IDS_TOOLSMENU,	IDS_PROFILE,		0 },				"%tf",		NULL,

	// resource commands

    IDM_NEW_ATL_OBJECT,		{ IDS_INSERTMENU,	IDS_NEW_ATL_OBJECT, 0 },				"%ia",		NULL,
	IDM_RESOURCE_NEW,		{ IDS_INSERTMENU,	IDS_NEWRESOURCE,	0 },				"%ir",		"^r",
	IDMY_TEST,				{ IDS_LAYOUTMENU,	IDS_TESTDLG,		0 },				"%lt",		"^t",
	IDM_VIEWASPOPUP,		{ 0,				0,					0 },				"^+{f10}{DOWN 4}{ENTER}",NULL,
    IDM_NEW_STRING,			{ IDS_INSERTMENU,	IDS_NEWSTRING,		0 },				"%iw",		"{insert}",
    IDM_NEW_ACCELERATOR,	{ IDS_INSERTMENU,	IDS_NEWACCELERATOR,	0 },				"%ia",		"{insert}",
    IDM_INVERTCOLORS,		{ IDS_IMAGEMENU,	IDS_INVERTCOLORS,	0 },				"%ii",		NULL,
    IDM_FLIPBSHH,			{ IDS_IMAGEMENU,	IDS_FLIPHORIZ,		0 },				"%ih",		"x",
    IDM_FLIPBSHV,			{ IDS_IMAGEMENU,	IDS_FLIPVERT,		0 },				"%iv",		"y",
    IDM_ROT90,				{ IDS_IMAGEMENU,	IDS_ROTATE90,		0 },				"%i9",		"z",
    IDM_NEW_STRINGBLOCK,	{ IDS_INSERTMENU,	IDS_NEWSTRINGBLOCK,	0 },				"%iw",		NULL,
    IDM_DELETE_STRINGBLOCK,	{ IDS_INSERTMENU,	IDS_DELSTRINGBLOCK,	0 },				"%id",		NULL,
	IDM_GRIDSETTINGS,		{ IDS_LAYOUTMENU,	IDS_GUIDESETTINGS,	0 },				"%lg",		NULL,
	IDM_RESOURCE_EXPORT,	{ 0,				0,					0 },			"+({F10}e",		NULL,
	IDM_NEWDEVIMG,			{ IDS_IMAGEMENU,	IDS_NEWDEVIMG,		0 },				"%mn",		"{insert}",
	IDM_TOOLBAR_WIZARD,		{ IDS_IMAGEMENU,	IDS_TOOLBAR_EDITOR, 0 },				"%it",		NULL,				
	
	// debug commands

    IDM_BUILD_DEBUG_GO,		{ IDS_BUILDMENU,	IDS_DEBUG,			IDS_GO },			"%bdg",		"{f5}",
    IDM_BUILD_DEBUG_TRACEINTO,{ IDS_BUILDMENU,	IDS_DEBUG,			IDS_STEPINTO },		"%bdi",		"{f8}",
    IDM_BUILD_DEBUG_TOCURSOR,{ IDS_BUILDMENU,	IDS_DEBUG,			IDS_STEPTOCURSOR },	"%bdc",		"{f7}",
    IDM_BUILD_DEBUG_ATTACH, { IDS_BUILDMENU,	IDS_DEBUG,			IDS_ATTACHTOPROCESS},"%bda",	NULL,
    IDM_CONNECTION,			{ IDS_BUILDMENU,	IDS_CONNECTION,		0 },				"%tn",		NULL,
    IDM_RUN_GO,				{ IDS_DEBUGMENU,	IDS_GO,				0 },				"%dg",		"{f5}",
    IDM_RUN_RESTART,		{ IDS_DEBUGMENU,	IDS_RESTART,		0 },				"%dr",		"+({f5})",
    IDM_RUN_STOPDEBUGGING,	{ IDS_DEBUGMENU,	IDS_STOPDEBUGGING,	0 },				"%dd",		"%({f5})",
    IDM_RUN_BREAK,			{ IDS_DEBUGMENU,	IDS_BREAK,			0 },				"%db",		NULL,
    IDM_RUN_TOCURSOR,		{ IDS_DEBUGMENU,	IDS_STEPTOCURSOR,	0 },				"%dc",		"{f7}",
    IDM_RUN_TRACEINTO,		{ IDS_DEBUGMENU,	IDS_STEPINTO,		0 },				"%di",		"{f8}",
    IDM_RUN_STEPOVER,		{ IDS_DEBUGMENU,	IDS_STEPOVER,		0 },				"%do",		"{f10}",
    IDM_RUN_STOPAFTERRETURN,{ IDS_DEBUGMENU,	IDS_STOPRETURN,		0 },				"%du",		"+({f7})",
    IDM_RUN_QUICKWATCH,		{ IDS_DEBUGMENU,	IDS_QUICKWATCH,		0 },				"%dq",		"+({f9})",
    IDM_RUN_EXCEPTIONS,		{ IDS_DEBUGMENU,	IDS_EXCEPTIONS,		0 },				"%de",		NULL,
    IDM_RUN_THREADS,		{ IDS_DEBUGMENU,	IDS_THREADS,		0 },				"%dt",		NULL,

	// tools commands

    IDM_CUSTOMIZE,			{ IDS_TOOLSMENU,	IDS_CUSTOMIZE,		0 },				"%tc",		NULL,
    IDM_OPTIONS,			{ IDS_TOOLSMENU,	IDS_OPTIONS,		0 },				"%to",		NULL,
    ID_TOOLS_RECORD,		{ IDS_TOOLSMENU,	IDS_RECORD,			0 },				"%tr",		"^+R",
    ID_TOOLS_PLAYBACK,		{ IDS_TOOLSMENU,	IDS_PLAYBACK,		0 },				"%tp",		"^+P",
    IDM_PROJECT_TOOL_MECR,	{ IDS_TOOLSMENU,	IDS_UPDATEREMOTE,	0 },				"%tu",		NULL,
    IDM_BROWSE_QUERY,		{ IDS_TOOLSMENU,	IDS_BROWSE,			0 },				"%tw",		"^{F11}",       
    IDM_MACROS,				{ IDS_TOOLSMENU,	IDS_MACROS,			0 },				"%tm",		NULL,
    IDM_TOOLS_RUNMENU,		{ IDS_TOOLSMENU,	IDS_TOOLSRUN,		0 },				"%tu",		"NULL",
	IDM_QUERY_GROUPBYMENU,	{ IDS_QUERYMENU,	IDS_GROUPBY,		0 },				"%qg",		"NULL",
	IDM_QUERY_VERIFYMENU,	{ IDS_QUERYMENU,	IDS_VERIFYSQL,		0 },				"%qv",		"NULL",
	// window commands

    ID_WINDOW_NEW,			{ IDS_WINDOWMENU,	IDS_NEWWINDOW,		0 },				"%wn",		NULL,   
    ID_WINDOW_SPLIT,		{ IDS_WINDOWMENU,	IDS_SPLITWINDOW,	0 },				"%wp",		NULL,
    ID_WINDOW_HIDE,			{ IDS_WINDOWMENU,	IDS_HIDEWINDOW,		0 },				"%wd",		"+({ESC})",
    ID_WINDOW_CASCADE,		{ IDS_WINDOWMENU,	IDS_CASCADE,		0 },				"%wc",		NULL,
    ID_WINDOW_TILE_HORZ,	{ IDS_WINDOWMENU,	IDS_TILEHORIZ,		0 },				"%wh",		NULL,
    ID_WINDOW_TILE_VERT,	{ IDS_WINDOWMENU,	IDS_TILEVERT,		0 },				"%wt",		NULL,
    ID_WINDOW_CLOSE_ALL,	{ IDS_WINDOWMENU,	IDS_CLOSEALL,		0 },				"%wl",		NULL,

	// help commands

	IDM_HELP_TIPOFTHEDAY,	{ IDS_HELPMENU,		IDS_TIPOFTHEDAY,	0 },				"%hp",		NULL,
	IDM_HELP_IVBOOKMARKS,	{ IDS_HELPMENU,		IDS_IVBOOKMARKS,	0 },				"%hm",		NULL,
	IDM_HELP_DEFINESUBSETS,	{ IDS_HELPMENU,		IDS_DEFINESUBSETS,	0 },				"%hd",		"^(+d)",
	IDM_HELP_SELECTSUBSETS,	{ IDS_HELPMENU,		IDS_SELECTSUBSETS,	0 },				"%hb",		NULL,
	IDM_HELP_SEARCH,		{ IDS_HELPMENU,		IDS_SEARCH,			0 },				"%hs",		NULL,

	// popup menus, without commands

    MENU_FILE,				{ IDS_FILEMENU,		0,					0 },				"%f",		NULL,
    MENU_EDIT,				{ IDS_EDITMENU,		0,					0 },				"%e",		NULL,
    MENU_VIEW,				{ IDS_VIEWMENU,		0,					0 },				"%v",		NULL,
	MENU_INSERT,			{ IDS_INSERTMENU,	0,					0 },				"%i",		NULL,
    MENU_PROJECT,			{ IDS_PROJECTMENU,	0,					0 },				"%p",		NULL,
    MENU_BUILD,				{ IDS_BUILDMENU,	0,					0 },				"%b",		NULL,
    MENU_DEBUG,				{ IDS_DEBUGMENU,	0,					0 },				"%d",		NULL,
    MENU_TOOLS,				{ IDS_TOOLSMENU,	0,					0 },				"%t",		NULL,
    MENU_WINDOW,			{ IDS_WINDOWMENU,	0,					0 },				"%w",		NULL,
    MENU_HELP,				{ IDS_HELPMENU,		0,					0 },				"%h",		NULL,
    MENU_IMAGE,				{ IDS_IMAGEMENU,	0,					0 },				"%m",		NULL,
    MENU_LAYOUT,			{ IDS_LAYOUTMENU,	0,					0 },				"%l",		NULL,

// keep this line last!!!
    0,						{ 0,				0,					0 },				NULL,		NULL
};
