#include "stdafx.h"
#pragma hdrstop

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

// Ugly casting away of const. Use at your own risk :-)
#define CHARSTAR (char *)(const char *)

// Windows / Element pointer cache
struct FormatCache
{
	FMT_WINDOW *pfmtcat;
	FMT_ELEMENT  *pfmtel;
};
#define INITIALIZE_FORMAT_CACHE(x,y) x.pfmtcat = m_FormatInfo.GetWindow(y); x.pfmtel = x.pfmtcat->rgElements;
FormatCache fmtcacheCpuWindow;
FormatCache fmtcacheMemoryWindow;
FormatCache fmtcacheCallsWindow;
FormatCache fmtcacheVarsWindow;
FormatCache fmtcacheDisassembly;
FormatCache fmtcacheWatchWindow;

FMT_ELEMENT *pfmtelFromDocType(DOCTYPE doctype)
{
	switch (doctype)
	{
		case MEMORY_WIN:		return fmtcacheMemoryWindow.pfmtel;
		case CALLS_WIN:			return fmtcacheCallsWindow.pfmtel;
		case DISASSY_WIN:		return fmtcacheDisassembly.pfmtel;
		case CPU_WIN:			return fmtcacheCpuWindow.pfmtel;

		default:
            ASSERT(FALSE);
            return( NULL );
	}
}

FMT_WINDOW *pfmtcatFromDocType(DOCTYPE doctype)
{
	switch (doctype)
	{
		case MEMORY_WIN:		return fmtcacheMemoryWindow.pfmtcat;
		case CALLS_WIN:			return fmtcacheCallsWindow.pfmtcat;
		case DISASSY_WIN:		return fmtcacheDisassembly.pfmtcat;
		case CPU_WIN:			return fmtcacheCpuWindow.pfmtcat;
		default:
            ASSERT(FALSE);
            return( NULL );
	}
}

// Since the watch and vars windows are not CTextView's we have seperate
// fncs for them.
FMT_ELEMENT *pfmtelForVarsWnd(UINT nID)
{
	switch(nID)
	{
		case IDDW_VCPP_WATCH_WIN:		return fmtcacheWatchWindow.pfmtel;
		case IDDW_VCPP_VARIABLES_WIN:	return fmtcacheVarsWindow.pfmtel;	
		default:
            ASSERT(FALSE);
            return fmtcacheVarsWindow.pfmtel;
	}
}


FMT_WINDOW *pfmtcatForVarsWnd(UINT nID)
{
	switch(nID)
	{
		case IDDW_VCPP_WATCH_WIN:		return fmtcacheWatchWindow.pfmtcat;
		case IDDW_VCPP_VARIABLES_WIN:	return fmtcacheVarsWindow.pfmtcat;	
		default:
            ASSERT(FALSE);
            return fmtcacheVarsWindow.pfmtcat;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	Fonts/Colors Interface
void CDebugPackage::InitColors(void)
{
	GetDefaultFormatInfo (m_FormatInfo);
	m_FormatInfo.UpdateFromRegistry();

#define LOADCSTRING(x,y) CString x; VERIFY(x.LoadString(y));

	LOADCSTRING(strCpuWindow, DLG_Cols_CpuWindow);
	LOADCSTRING(strMemoryWindow, DLG_Cols_MemoryWindow);
	LOADCSTRING(strVarsWindow, DLG_Cols_VarsWindow);
	LOADCSTRING(strCallsWindow, DLG_Cols_CallsWindow);
	LOADCSTRING(strDisassembly, DLG_Cols_DisassemblyWindow);
	LOADCSTRING(strWatchWindow, DLG_Cols_WatchWindow);

	if ( gpIBldSys )
	{
		INITIALIZE_FORMAT_CACHE(fmtcacheCpuWindow,		strCpuWindow);
		INITIALIZE_FORMAT_CACHE(fmtcacheMemoryWindow,	strMemoryWindow);
		INITIALIZE_FORMAT_CACHE(fmtcacheCallsWindow,	strCallsWindow);
		INITIALIZE_FORMAT_CACHE(fmtcacheVarsWindow,		strVarsWindow);
		INITIALIZE_FORMAT_CACHE(fmtcacheDisassembly,	strDisassembly);
		INITIALIZE_FORMAT_CACHE(fmtcacheWatchWindow,	strWatchWindow);
	}
}

CFormatInfo * CDebugPackage::GetFormatInfo(void)
{
	return( &m_FormatInfo );
}

void CDebugPackage::GetDefaultFormatInfo (CFormatInfo& fi)
{
	if (!m_FormatInfoDefault.IsEmpty())
	{
		fi = m_FormatInfoDefault;
		return;
	}

	LOADCSTRING(strText, DLG_Cols_SourceWindow);
	LOADCSTRING(strSelection, DLG_Cols_Selection);
	LOADCSTRING(strRegister, DLG_Cols_Register);
	LOADCSTRING(strValue, DLG_Cols_Value);
	LOADCSTRING(strMemory, SYS_MemoryWin_Title);
	LOADCSTRING(strMemoryChanged, DLG_Cols_MemoryHighlight);
	LOADCSTRING(strRegisterChanged, DLG_Cols_CpuWindowHighlight);
	LOADCSTRING(strCallsHighlight, DLG_Cols_CallsHighlight);
	LOADCSTRING(strCallsSuperceded, DLG_Cols_Superceded);
	LOADCSTRING(strCurrentLine, DLG_Cols_CurrentLine);
	LOADCSTRING(strBreakpointLine, DLG_Cols_BreakpointLine);
	LOADCSTRING(strAssembly, DLG_Cols_Disassembly);
	LOADCSTRING(strWatchChanged, DLG_Cols_WatchHighlight);
	LOADCSTRING(strVarsChanged, DLG_Cols_VarsHighlight);

	const FMT_ELEMENT rgElementsRegisters[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText, 			RGB_BLACK, 		RGB_WHITE,		AUTO_TEXT },
		{ CHARSTAR strSelection,		RGB_WHITE, 		RGB_BLACK,		AUTO_SELECTION },
		{ CHARSTAR strRegisterChanged,	RGB_RED,   		RGB_WHITE,		NOTAUTO_REF(0) }
	};

	const FMT_ELEMENT rgElementsMemory[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText,				RGB_BLACK, 		RGB_WHITE,		AUTO_TEXT },		
		{ CHARSTAR strSelection,		RGB_WHITE, 		RGB_BLACK,		AUTO_SELECTION },
		{ CHARSTAR strMemoryChanged,	RGB_RED,   		RGB_WHITE,		NOTAUTO_REF(0) }	
	};

	const FMT_ELEMENT rgElementsCallStack[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText,				RGB_BLACK, 		RGB_WHITE,		AUTO_TEXT },		
		{ CHARSTAR strSelection,		RGB_WHITE, 		RGB_BLACK,		AUTO_SELECTION },
		{ CHARSTAR strCallsHighlight,	RGB_BLACK, 		RGB_GREEN,		NOTAUTO_REF(0) },	
		{ CHARSTAR strCallsSuperceded,	RGB_GRAY,	 	RGB_WHITE,		NOTAUTO_REF(0) }	
	};

	const FMT_ELEMENT rgElementsVars[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText,				RGB_BLACK, 		RGB_WHITE,		AUTO_TEXT },		
		{ CHARSTAR strSelection,		RGB_WHITE, 		RGB_BLACK,		AUTO_SELECTION },	
		{ CHARSTAR strVarsChanged,	    RGB_RED,   		RGB_WHITE,		NOTAUTO_REF(0) },	
	};

	const FMT_ELEMENT rgElementsDisAsm[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText,				RGB_GRAY,		RGB_WHITE,		AUTO_TEXT },		
		{ CHARSTAR strSelection,		RGB_WHITE,		RGB_BLACK,		AUTO_SELECTION },	
		{ CHARSTAR strAssembly,			RGB_GRAY,		RGB_WHITE,		NOTAUTO_REF(0) },	
	};

	const FMT_ELEMENT rgElementsWatchWindow[]=
	{//	  Name							Foreground		Background		Automatic info
		{ CHARSTAR strText,				RGB_BLACK,		RGB_WHITE,		AUTO_TEXT },		
		{ CHARSTAR strSelection,		RGB_WHITE,		RGB_BLACK,		AUTO_SELECTION },	
		{ CHARSTAR strWatchChanged,	    RGB_RED,   		RGB_WHITE,		NOTAUTO_REF(0) },	
	};

	// Create the categories for the elements
	LOGFONT lfMonospace, lfProportional;

	memcpy(&lfMonospace, GetStdLogfont(6), sizeof(LOGFONT));
	memcpy(&lfProportional, GetStdLogfont(0), sizeof(LOGFONT));

	LOADCSTRING(strCpuWindow,    DLG_Cols_CpuWindow);
	LOADCSTRING(strMemoryWindow, DLG_Cols_MemoryWindow);
	LOADCSTRING(strVarsWindow,   DLG_Cols_VarsWindow);
	LOADCSTRING(strCallsWindow,  DLG_Cols_CallsWindow);
	LOADCSTRING(strDisassembly,  DLG_Cols_DisassemblyWindow);
	LOADCSTRING(strWatchWindow,  DLG_Cols_WatchWindow);

	const FMT_WINDOW rgDebuggerWindows [] =
	{//	  Name						Registry Entry			Changed	Monospace	Font							 pt  PLOGFONT #Elements						Elements
		{ CHARSTAR strCpuWindow,	"Registers Window",		FALSE,	TRUE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsRegisters),	(FMT_ELEMENT*)rgElementsRegisters },
		{ CHARSTAR strMemoryWindow,	"Memory Window",		FALSE,	TRUE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsMemory),		(FMT_ELEMENT*)rgElementsMemory },
		{ CHARSTAR strCallsWindow,	"Calls Window",			FALSE,	TRUE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsCallStack),	(FMT_ELEMENT*)rgElementsCallStack },
		{ CHARSTAR strVarsWindow,	"Variables Window",		FALSE,	FALSE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsVars),		(FMT_ELEMENT*)rgElementsVars },
		{ CHARSTAR strWatchWindow,	"Watch Window",			FALSE,	FALSE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsWatchWindow),(FMT_ELEMENT*)rgElementsWatchWindow },
		{ CHARSTAR strDisassembly,	"Disassembly Window",	FALSE,	TRUE,		CHARSTAR lfMonospace.lfFaceName, 10, NULL,	FMT_ELEMENTS(rgElementsDisAsm),		(FMT_ELEMENT*)rgElementsDisAsm }
	};

	LOADCSTRING(strDebuggerWindows, DLG_Cols_DebuggerWindows);

	const FMT_WINGROUP DebuggerWinGroup = { FMT_WINDOWS(rgDebuggerWindows), FALSE, CHARSTAR strDebuggerWindows, (FMT_WINDOW*)rgDebuggerWindows };

    fi += DebuggerWinGroup;

	m_FormatInfoDefault = fi;
}

void CDebugPackage::OnFormatChanged(void)
{
	FMT_WINDOW *pfmtcat;
#define NOTIFYCHANGE(x,y) if (((pfmtcat = x.pfmtcat)->bChanged) && (y::g_cRef != 0)) { y::g_pFontInfo->ChangeFontInfo(*(pfmtcat->pLogFont)); }

	if ( gpIBldSys != NULL )
	{
		NOTIFYCHANGE(fmtcacheCpuWindow, CCpuView);
		NOTIFYCHANGE(fmtcacheMemoryWindow, CMemoryView);
		NOTIFYCHANGE(fmtcacheCallsWindow, CCallsView);
		NOTIFYCHANGE(fmtcacheDisassembly, CDAMView);
		NOTIFYCHANGE(fmtcacheWatchWindow, CGridWatchView);
		NOTIFYCHANGE(fmtcacheVarsWindow,  CVarsView);
	}

	if ( g_pWatchView != NULL )
		g_pWatchView->OnFontColorChange();

	if ( g_pVarsView != NULL )
		g_pVarsView->OnFontColorChange();

}


///// CDebugPackage::OnFormatChanged
//
//	The system has notified us that the pool of font resources
//	has been changed.  Tell our FormatInfo instance to
//	rebuild all the LOGFONTs.
//
void CDebugPackage::OnFontChange()
{
	m_FormatInfo.UpdateAllLogFonts ();
	OnFormatChanged ();
}

///// CDebugPackage::OnColorChanged
//
//	The system has notified us that the system colors
//	have been changed.  Tell our FormatInfo instance to
//	redisplay everything.
//
void CDebugPackage::OnColorChange ()
{
	m_FormatInfo.UpdateAllAutoColors ();
	OnFormatChanged ();
}

