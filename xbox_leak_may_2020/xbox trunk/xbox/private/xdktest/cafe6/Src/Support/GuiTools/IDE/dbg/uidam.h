///////////////////////////////////////////////////////////////////////////////
//  UIDAM.H
//
//  Created by :            Date :
//      WayneBr             1/14/94
//
//  Description :
//      Declaration of the UIDAM class
//

#ifndef __UIDAM_H__
#define __UIDAM_H__

#include "dbgxprt.h"

#include "..\SYM\qcqp.h"
#include "..\SHL\udockwnd.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

enum DAM_MENU_ITEM{
	GO_TO_SOURCE,	
	SHOW_NEX_STATEMENT,
	INSERT_REMOVE_BREAK_POINT,
	TOGGLE_BREAKPOINT,
	RUN_TO_CURSOR,
	SET_NEXT_STATEMENT,
	SOURCE_ANNOTATION,
	CODE_BYTES,
	DOCKING_VIEW,
	CLOSE_HIDE
} ;


///////////////////////////////////////////////////////////////////////////////
//  UIDAM class

// BEGIN_CLASS_HELP
// ClassName: UIDAM
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIDAM : public UIDockWindow
	{
	UIWND_COPY_CTOR(UIDAM, UIDockWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const
		{	return IDW_DISASSY_WIN; }

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
		BOOL GotoSymbol(LPCSTR symbol, BOOL bCloseAfter = TRUE);
		BOOL GotoAddress(int address, BOOL bCloseAfter = TRUE);
		CString GetInstruction(int offset=0,int range=1, BOOL bLocateCaretAtLeftTop = FALSE);
		BOOL SwitchBetweenASMAndSRC(BOOL bUsingContextMenu = FALSE);
		BOOL EnableDockingView(BOOL bEnable);
		BOOL IsDockingViewEnabled(void);
		BOOL ChooseContextMenuItem(DAM_MENU_ITEM DamMenuItem);

		
	};

#endif
