///////////////////////////////////////////////////////////////////////////////
//	UIMEM.H
//
//	Created by :			Date :
//		MichMa					1/14/94
//
//	Description :
//		Declaration of the UIMemory class
//

#ifndef __UIMEM_H__
#define __UIMEM_H__

#include "dbgxprt.h"
#include "..\SYM\qcqp.h"
#include "..\SHL\udockwnd.h"

#include "..\SHL\uwbframe.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

// memory format options
/*
typedef enum {	MEM_FORMAT_ASCII = 0,
				MEM_FORMAT_WCHAR,
				MEM_FORMAT_BYTE,
				MEM_FORMAT_SHORT,
				MEM_FORMAT_SHORT_HEX,
				MEM_FORMAT_SHORT_UNSIGNED,
				MEM_FORMAT_LONG,
				MEM_FORMAT_LONG_HEX,
				MEM_FORMAT_LONG_UNSIGNED,
				MEM_FORMAT_REAL,
				MEM_FORMAT_REAL_LONG,
				MEM_FORMAT_INT64,
				MEM_FORMAT_INT64_HEX,
				MEM_FORMAT_INT64_UNSIGNED
			} MEM_FORMAT;
			


*/
///////////////////////////////////////////////////////////////////////////////
//	UIMemory class

// BEGIN_CLASS_HELP
// ClassName: UIMemory
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP

#define ERROR_ERROR -1

class DBG_CLASS UIMemory : public UIDockWindow
	
	{
	UIWND_COPY_CTOR(UIMemory, UIDockWindow);

	//Data
	// stores format currently used by Memory window. 
	// used by UIMemory and COMemory utility classes.
	// use UIMemory::SetMemoryFormat() to set it, and
	// UIMemory::GetMemoryFormat() to get it.
	MEM_FORMAT	m_MemoryFormat;

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_MEMORY_WIN; }

	virtual BOOL AttachActive(void);
	virtual BOOL Find(void);

	// UIWindow overrides
	public:
		virtual BOOL IsValid(void) const;
		virtual BOOL IsActive(void);	// derived classes must provide their own Activate (usually a DoCommand)

	// Utilities
	public:
		
		CString GetCurrentAddress(void);
		CString GetCurrentData(int count = 1, int start = 0);
		CString GetCurrentChars(int count = 1, int start = 0);
		CString GetColumns(void);
		CString GetCurrentSymbol(void);
		
		BOOL SetCurrentData(LPCSTR data, int start = 0);
		BOOL SetCurrentChars(LPCSTR chars, int start = 0);
		BOOL SetCurrentChars(char chr, int start = 0);
		BOOL SetMemoryFormat(MEM_FORMAT format);
		BOOL EnableToolBar(BOOL bEnable);
		BOOL ToogleReEvaluation(void);
		BOOL SetColumns(CString Columns);
		BOOL CycleMemoryFormat(int count);
		BOOL EnableDockingView(BOOL bEnable);
		BOOL IsDockingViewEnabled(void);
		BOOL ToggleDockingView(void);

		int GoToAddress(LPCSTR symbol);
		int GetFormatWidth(int format = 0);
		int GetMemoryWidth(void);

		MEM_FORMAT GetMemoryFormat(void);

	};

#endif //__UIMEM_H__
