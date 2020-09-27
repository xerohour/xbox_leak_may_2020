///////////////////////////////////////////////////////////////////////////////
//	UMAINFRM.H
//
//	Created by :			Date :
//		DavidGa					9/20/93
//
//	Description :
//		Declaration of the UIMainFrm class
//
//		A Common subclass of UIWindow, UIMainFrm is designed as the basis of any
//		utility class associated with an application's main frame window
//		(i.e. the one with the menubar and possibly an MDI client area.
//

#ifndef __UIMAINFRM_H__
#define __UIMAINFRM_H__

#include "..\..\uwindow.h"

#include "shlxprt.h"

///////////////////////////////////////////////////////////////////////////////
//	DoCommand structure, enums and globals

// command access methods
enum DC_METHOD { DC_MESSAGE, DC_MENU, DC_MNEMONIC, DC_ACCEL };
	// REVIEW(davidga) additional method possibilities: MENUX, TOOLBAR

struct CMD_STRUCT									  
{	
	UINT idCommand;		// menu id
	UINT idMenu[3];		// string id for up to three menu items
	LPCSTR szMnemonic;	// string of keys that will access menu mnemonics
	LPCSTR szAccel;		// accelerator string
};


///////////////////////////////////////////////////////////////////////////////
//UIWindow class

// BEGIN_CLASS_HELP
// ClassName: UIMainFrm
// BaseClass: UIWindow
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIMainFrm : public UIWindow
{
public:
	UIWND_COPY_CTOR( UIMainFrm, UIWindow);
	virtual void OnUpdate(void);

// Data
protected:
	CMD_STRUCT* m_pCmdTable;

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:
	BOOL DoCommand(UINT id, DC_METHOD method);
	BOOL IsCommandEnabled(UINT id);
};

#endif //__UIMAINFRM_H__
