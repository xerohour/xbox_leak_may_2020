//-----------------------------------------------------------------------------
//  SpyxxHk.h
//
//  Copyright (C) 1993, Microsoft Corporation
//
//  Purpose:
//
//  Spy++ message hook DLL header file.
//
//  Revision History:
//
//  04/26/93 byrond	 Created.
//
//-----------------------------------------------------------------------------

#ifndef __SPYXXHK_H__
#define __SPYXXHK_H__

//
// This header is used by both C and C++ programs.  All interfaces
// must therefore be in C.
//
#ifdef __cplusplus
extern "C" {
#endif


#define PUBLIC
#ifndef _DEBUG
#define PRIVATE static
#else
#define PRIVATE
#endif

#ifdef SPYXXHK_DLL
#define SPYXXHK_API		__declspec(dllexport)
#define SPYXXHK_SHDATA	__declspec(dllexport)
#else
#define SPYXXHK_API		__declspec(dllimport)
#define SPYXXHK_SHDATA	__declspec(dllimport)
#endif


//
// Signature for the main window in Spy++.  This is part of the
// mechanism that ensures there is only one copy of Spy++ running.
// Versions of Spy++ that use the same signature will not allow
// themselves to be run simultaneously.
//
#define SPYXX_WINDOW_SIGNATURE		0x12feeb76

//
// The startup mutex is the other part of the mechanism that ensures
// that only one compatible copy of Spy++ is running.  It should
// be changed if and only if the window signature above is also
// changed.
//
#define STARTUPMUTEXNAME			"SpyxxStartupSyncMutex"


//
// Object names that are in the global namespace and are used
// to communicate between the hook (running in many different
// processes) and the Spy++ application.  These objects should
// be renamed for a different version of Spy++ only if running
// other versions simultaneously with the new version is
// allowed.  Or, to put it another way, these object names
// can remain the same only if the next version of Spy++
// will not be allowed to run at the same time.
//
#define WRITERMUTEXNAME				"Spy++ Writer Mutex"
#define ACCESSMUTEXNAME				"Spy++ Access Mutex"
#define READEVENTNAME				"Spy++ Read Event"
#define WRITTENEVENTNAME			"Spy++ Written Event"
#define OTHERPROCESSDATAEVENTNAME	"Spy++ Other Process Data Event"


#define POSTTYPE_POSTED			0	// Msg was posted
#define POSTTYPE_SENT			1	// Msg was sent
#define POSTTYPE_SENTNORETURN	2	// Msg was sent (no return available)
#define POSTTYPE_RETURN			3	// Return value from a send



//
// Maximum characters of a string that is a parameter that will be
// passed back in the queue.  Strings longer than this will be
// truncated when displayed.  Buffers allocated using this constant
// should add 1 for the null terminator.
//
#define CCHMAXPARMSTRING		50


//
// Maximum number of ints that will be passed back in an int array.
// Some messages (like LB_GETSELITEMS) have lParams that point to
// an array of ints.  We want to limit the max number of these ints
// that will be passed in the queue to Spy++ to display.
//
#define CMAXINTARRAYITEMS		25


//
// Message encoding table structure
//
typedef struct
{
	WORD fEncoding;			// Encoding flags (MET_*)
	WORD cbExtraData;		// Optional size of extra data
} MSGENCODETABLE;


//
// Message encoding table flags.  These describe what extra data needs
// to be grabbed by the hook and where it is located.
//
// Some messages only have valid data after the call (on the return).
// These types of messages will specify "RET" at the end to signify
// this.  The hook will only grab the data on return type messages.
// An example of this is WM_GETTEXT, where the buffer pointed to by
// lParam contains garbage until the callee fills it in with the string
// on the return, for instance.
//
// Unless specified otherwise, all these types access data pointed to
// by lParam.
//
#define MET_NOEXTRADATA			0	// No extra data in wParam/lParam
#define MET_STRUCTRET			1	// Pointer to struct (only valid on ret)
#define MET_STRUCTBOTH			2	// Pointer to struct (valid both ways)
#define MET_STRINGRET			3	// Pointer to string (only valid on ret)
#define MET_STRINGBOTH			4	// Pointer to string (valid both ways)
#define MET_LBODSTRINGBOTH		5	// Listbox owner-draw string message
#define MET_CBODSTRINGBOTH		6	// Combobox owner-draw string message
#define MET_LBSTRINGORDRET		7	// Listbox str or ord (only valid on ret)
#define MET_CBSTRINGORDRET		8	// Combobox str or ord (only valid on ret)
#define MET_WM_NCCALCSIZEBOTH	9	// WM_NCCALCSIZE message
#define MET_GLOBALSTRUCTBOTH	10	// Pointer to struct (use GlobalLock)
#define MET_CREATESTRUCTBOTH	11	// CREATESTRUCT struct (valid both ways)
#define MET_MDICREATESTRUCTBOTH	12	// MDICREATESTRUCT struct (valid both ways)
#define MET_CLASSBOTH			13	// Get class in hook (valid both ways)
#define MET_WPLPDWORDSRET		14	// wp and lp can point to dwords (on ret)
#define MET_EM_GETLINE			15	// EM_GETLINE message
#define MET_INTARRAYBOTH		16	// lParam is ptr to int array (valid both ways)
#define MET_INTARRAYRET			17	// lParam is ptr to int array on return
#define MET_FONTBOTH			18	// lParam is hfont (valid both ways)
#define MET_FONTRET				19	// lResult is hfont (only valid on ret)



//
// Class atom types.  CLS_UNKNOWN is a special value if the class
// is not one of the known types.  The other entries are indices
// into the class atom array.
//
#define CLS_UNKNOWN		999

#define CLS_BUTTON		0
#define CLS_COMBOBOX	1
#define CLS_EDIT		2
#define CLS_LISTBOX		3
#define CLS_MDICLIENT	4
#define CLS_SCROLLBAR	5
#define CLS_STATIC		6
#define CLS_DIALOG		7
#ifndef DISABLE_WIN95_MESSAGES
#define CLS_ANIMATE		8
#define CLS_HEADER		9
#define CLS_TOOLBAR		10
#define CLS_TOOLTIP		11
#define CLS_STATUSBAR	12
#define CLS_TRACKBAR	13
#define CLS_UPDOWN		14
#define CLS_PROGRESS	15
#define CLS_HOTKEY		16
#define CLS_LISTVIEW	17
#define CLS_TREEVIEW	18
#define CLS_TABCTRL		19
#endif	// DISABLE_WIN95_MESSAGES

//
// Number of entries in the class atom array.  This must be
// updated if more CLS_* class atom types are defined!
// Also, the code that initializes this array must be updated
// if new types are defined as well.
//
#ifndef DISABLE_WIN95_MESSAGES
#define MAXCLASSATOMS	20
#else	// DISABLE_WIN95_MESSAGES
#define MAXCLASSATOMS	8
#endif	// DISABLE_WIN95_MESSAGES


//
// Maximum reserved messages in windows.
//
#define MAX_MESSAGES	WM_USER

//
// Maximum messages in windows.
//
#define MAX_DLG_MESSAGES	2
#ifndef DISABLE_WIN95_MESSAGES
#define MAX_ANI_MESSAGES	3
#define MAX_HDR_MESSAGES	7
#define MAX_TB_MESSAGES		45
#define MAX_TT_MESSAGES		15
#define MAX_STAT_MESSAGES	10
#define MAX_TRK_MESSAGES	29
#define MAX_UD_MESSAGES		10
#define MAX_PROG_MESSAGES	5
#define MAX_HK_MESSAGES		3
#define MAX_LV_MESSAGES		53
#define MAX_TV_MESSAGES		24
#define MAX_TAB_MESSAGES	49
#endif	// DISABLE_WIN95_MESSAGES

//
// Message type flags used when classifying a message for printing.
//
enum MSGTYPE
{
	MT_NORMAL,			// A normal message (found in the table).
	MT_DIALOG,			// dialog message (DM_GETDEFID, DM_SETDEFID)
#ifndef DISABLE_WIN95_MESSAGES
	MT_ANIMATE,			// Chicago animation control
	MT_HEADER,			// Chicago header control
	MT_HOTKEY,			// Chicago hotkey control
	MT_LISTVIEW,		// Chicago list-view control
	MT_PROGRESS,		// Chicago progress gauge control
	MT_STATUSBAR,		// Chicago status-bar control
	MT_TOOLBAR,			// Chicago tool-bar control
	MT_TRACKBAR,		// Chicago track-bar control
	MT_TABCTRL,			// Chicago tab control
	MT_TOOLTIP,			// Chicago tool-tip control
	MT_TREEVIEW,		// Chicago tree-view control
	MT_UPDOWN,			// Chicago up-down (spinner) control
#endif	// DISABLE_WIN95_MESSAGES
	MT_USER,			// A WM_USER message.
	MT_UNDOCUMENTED,	// An unknown message (below WM_USER).
	MT_REGISTERED		// A message in the registered msg range.
};

//
// Structure that contains the message data sent from the hook
// to the app to display.  This structure is what is packed into
// the queue by the hook, and the data remains in this form until
// it is actually logged in a message stream view, at which time
// it gets converted to a MSGSTREAMDATA2 structure.  Note that
// this means that all the message and return crackers deal with
// the data in this form (but not the parameter crackers).
//
// The Spy++ app code that receives the hook packets explicitly
// looks for the cb field as the first DWORD in the packet, so
// that field MUST remain as the first DWORD in this structure!
//
typedef struct
{
	DWORD cb;			// Total size, including extra data
	UINT fPostType;
	INT nLevel;
	HWND hwnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
	LRESULT lResult;
	DWORD time;
	LONG ptX;			// component part of POINT portion of MSG structure
	LONG ptY;			// component part of POINT portion of MSG structure
	//
	// Message specific data may follow.
	//
} MSGSTREAMDATA, *PMSGSTREAMDATA;


//
// Structure that describes a message once the message has been
// logged in a message stream view.  Note that this means that all
// the message parameter crackers deal with the data in this form.
//
typedef struct
{
	UINT fPostType;
	INT nLevel;
	HWND hwnd;
	UINT msg;
	WPARAM wParam;
	LPARAM lParam;
	LRESULT lResult;
	enum MSGTYPE mtMsgType;
	DWORD cbExtraData;	// Size of any extra data (or 0)
	PBYTE pbExtraData;	// Pointer to extra data (or NULL)
} MSGSTREAMDATA2, *PMSGSTREAMDATA2;


//
// Maximum size of a message packet
//
#define CBMSGPACKETMAX	(sizeof(MSGSTREAMDATA) + 260)


//
// Structure used to obtain data from within the context of another
// process
//
typedef struct
{
	HWND hwnd;				// Window we are interested in
	WNDPROC pfnWndProc;		// Window proc for hwnd
	BOOL fValidWC;			// TRUE if the wc data is valid
	WNDCLASS wc;			// WNDCLASS from GetClassInfo
	CHAR szMenuName[128];	// Menu name from GetClassInfo
} OTHERPROCESSDATA, *POTHERPROCESSDATA;



//
// Following are structures used for packing parameters into the
// queue for different messages that have extra data.
//

//
// Used for WM_NCCALCSIZE.  It starts with room for a NCCALCSIZE_PARAMS
// structure. This structure has a pointer to a WINDOWPOS structure in it,
// so we need room to tack that on the end as well.
//
typedef struct
{
	NCCALCSIZE_PARAMS nccp;
	WINDOWPOS wp;
} PACK_NCCALCSIZE, *PPACK_NCCALCSIZE;

//
// Used for strings that can be ordinals as well.  This structure can
// be overlaid on a string.  If the dwMarker field is 0xFFFFFFFF, then
// the dwOrdinal value is the ordinal.  Otherwise, the buffer it was
// overlaid on is really a null-terminated string.
//
typedef struct
{
	DWORD dwMarker;
	DWORD dwOrdinal;
} PACK_STRINGORD, *PPACK_STRINGORD;

//
// Used for WM_CREATE.  It starts with room for a CREATESTRUCT
// structure. This structure has two string pointers in it, to
// the text and the class.  Allow the max string room for these
// as well.
//
typedef struct
{
	CREATESTRUCT cs;
	BOOL fValidName;
	CHAR szName[CCHMAXPARMSTRING + 1];
	BOOL fValidClass;
	CHAR szClass[CCHMAXPARMSTRING + 1];
} PACK_CREATESTRUCT, *PPACK_CREATESTRUCT;

//
// Used for WM_MDICREATE.  It starts with room for a MDICREATESTRUCT
// structure. This structure has two string pointers in it, to
// the text and the class.  Allow the max string room for these
// as well.
//
typedef struct
{
	MDICREATESTRUCT mdic;
	BOOL fValidClass;
	CHAR szClass[CCHMAXPARMSTRING + 1];
	BOOL fValidTitle;
	CHAR szTitle[CCHMAXPARMSTRING + 1];
} PACK_MDICREATESTRUCT, *PPACK_MDICREATESTRUCT;

//
// Used for messages that need to know the class of the window.
// The class needs to be queried in the hook (otherwise the
// window may disappear by the time the Spy++ app decoding
// code gets called).  The nClass field will contain one of
// the CLS_* defines.
//
typedef struct
{
	UINT nClass;
} PACK_CLASS, *PPACK_CLASS;

//
// Used for messages where wParam and lParam can be null or can point
// to dwords.  This includes SBM_GETRANGE and CB_GETEDITSEL.
//
typedef struct
{
	BOOL fValidWP;
	DWORD nWP;
	BOOL fValidLP;
	DWORD nLP;
} PACK_WPLPDWORDS, *PPACK_WPLPDWORDS;

//
// Used for messages that set/get the font.  This includes WM_GETFONT
// and WM_SETFONT.  The nHeight field will be -1 if the structure
// contents are invalid.  The nHeight field is not the actual point
// size, but rather it is the pixel height of the font that can be used
// later to calculate the point size using the following formula:
// nPointSize = MulDiv(nHeight, 72, cyLogPixelsPerInch);
//
typedef struct
{
	INT nHeight;
	CHAR szFaceName[LF_FACESIZE];
} PACK_FONT, *PPACK_FONT;



extern SPYXXHK_SHDATA BOOL gfOnChicago;				// TRUE if the application is running on Chicago
extern SPYXXHK_SHDATA BOOL gfOnDaytona;				// TRUE if the application is running on Daytona
extern SPYXXHK_SHDATA HHOOK ghhkMsgHook;			// hook from SetWindowsHookEx(WH_GETMESSAGE)
extern SPYXXHK_SHDATA HHOOK ghhkCallHook;			// hook from SetWindowsHookEx(WH_CALLWNDPROC)
extern SPYXXHK_SHDATA HHOOK ghhkRetHook;			// hook from SetWindowsHookEx(WH_CALLWNDPROCRET) (Chicago only)
extern SPYXXHK_SHDATA BOOL gfHookEnabled;			// TRUE if the hook is enabled
extern SPYXXHK_SHDATA BOOL gfEnableSubclass;		// TRUE if subclassing is enabled
extern SPYXXHK_SHDATA BOOL gfDecodeStructs;			// TRUE if structs/strings are cracked
extern SPYXXHK_SHDATA UINT gcSubclass;				// Count of subclassed messages
extern SPYXXHK_SHDATA DWORD gpidSpyxx;				// Process ID of the Spy++ app
extern SPYXXHK_SHDATA DWORD gtidSpyxx;				// Thread ID of Spy++'s main thread
extern SPYXXHK_SHDATA DWORD goffRead;				// Read offset
extern SPYXXHK_SHDATA DWORD goffWrite;				// Write offset
extern SPYXXHK_SHDATA UINT gcMsgPackets;			// Count of message packets
extern SPYXXHK_SHDATA ATOM gaaClasses[];			// Class atom array
extern SPYXXHK_SHDATA BYTE gabMsgBuf[];				// Message buffer area
extern SPYXXHK_SHDATA UINT gmsgOtherProcessData;	// Registered message
extern SPYXXHK_SHDATA OTHERPROCESSDATA gopd;		// Structure for other process data
extern SPYXXHK_SHDATA MSGENCODETABLE gmet[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetDlg[];		// Message encoding table
#ifndef DISABLE_WIN95_MESSAGES
extern SPYXXHK_SHDATA MSGENCODETABLE gmetAni[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetBL[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetHdr[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetTB[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetTT[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetStat[];	// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetTrk[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetUpDn[];	// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetProg[];	// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetHK[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetLV[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetTV[];		// Message encoding table
extern SPYXXHK_SHDATA MSGENCODETABLE gmetTab[];		// Message encoding table
#endif	// DISABLE_WIN95_MESSAGES


SPYXXHK_API LRESULT CALLBACK SpyxxGetMsgProc(INT hc, WPARAM wParam, LPARAM lParam);
SPYXXHK_API LRESULT CALLBACK SpyxxCallWndProc(INT hc, WPARAM wParam, LPARAM lParam);
SPYXXHK_API LRESULT CALLBACK SpyxxCallWndRetProc(INT hc, WPARAM wParam, LPARAM lParam);

SPYXXHK_API UINT GetWindowClass(HWND hwnd);

#ifdef __cplusplus
}
#endif

#endif	// __SPYXXHK_H__
