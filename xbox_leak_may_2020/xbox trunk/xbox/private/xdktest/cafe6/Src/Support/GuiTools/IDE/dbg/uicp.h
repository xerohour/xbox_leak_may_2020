///////////////////////////////////////////////////////////////////////////////
//  UICP.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UICoProcessor class
//

#ifndef __UICP_H__
#define __UICP_H__

#include "..\shl\udockwnd.h"

#include "dbgxprt.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UICoProcessor symbols

// enums must start at 1

// Intel Specific
//enum {ST0=1, ST1, ST2, ST3, ST4, ST5, ST6, ST7, CTRL, STAT, TAGS, IP, CS, DO, DS};
enum {rST0=1, rST1, rST2, rST3, rST4, rST5, rST6, rST7, rCTRL, rSTAT, rTAGS, rIP, rCS, rDO, rDS};
//const MaxReg=14;
#define MaxReg 14
#define REG_OFFSET 24


///////////////////////////////////////////////////////////////////////////////
//  UICoProcessor class

// BEGIN_CLASS_HELP
// ClassName: UICoProcessor
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UICoProcessor : public UIDockWindow
	{

	UIWND_COPY_CTOR(UICoProcessor, UIDockWindow);

	virtual BOOL AttachActive(void);
	virtual BOOL Find(void);

	// Initialization
	public:

	// Data
	private:
		int m_option;

	// UIWindow overrides
	public:
		virtual BOOL IsValid(void) const;
		virtual BOOL IsActive(void);	// derived classes must provide their own Activate (usually a DoCommand)


	// Utilities
	public:
		CString GetCPRegister(int reg);
		BOOL SetCPRegister(int reg, LPCSTR value);
	};

#endif
