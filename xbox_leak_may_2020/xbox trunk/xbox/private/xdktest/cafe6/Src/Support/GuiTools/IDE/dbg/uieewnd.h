///////////////////////////////////////////////////////////////////////////////
//  UIEEWND.H
//
//  Created by :            Date :
//      MichMa              	10/20/94
//
//  Description :
//		Declaration of the UIEEWindow class
//

#ifndef __UIEEWND_H__
#define __UIEEWND_H__

#include "..\shl\udockwnd.h"
#include "dbgxprt.h"
#include "mstwrap.h"
#include "..\..\testutil.h"
#include "..\sym\cmdids.h"
#include "..\shl\uwbframe.h"
#include "..\sym\qcqp.h"
#include "..\shl\wbutil.h"
#include "Strings.h"
#include "testxcpt.h"
#include "guiv1.h"

#ifndef __UIDOCKWND_H__
	#error include 'udockwnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIEEWindow class

#define ERROR_ERROR -1

#define ROW_CURRENT		0
#define ROW_LAST		-1
#define ROW_ALL			-2
#define ROW_NEXT		-3
#define ROW_PREVIOUS	-4

typedef enum {NOT_EXPANDABLE, EXPANDED, COLLAPSED} EXPR_STATE;
typedef enum {METHOD_WND_SEL, METHOD_PROP_PAGE} EE_METHOD_TYPE;

class DBG_CLASS EXPR_INFO
	{
	public:
		int		state;
		CString	type;
		CString	name;
		CString	value;
	};

// BEGIN_CLASS_HELP
// ClassName: UIEEWindow
// BaseClass: UIDockWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIEEWindow : public UIDockWindow
	
	{
	UIWND_COPY_CTOR(UIEEWindow, UIDockWindow);

	// subclasses must override these functions
	virtual UINT GetID(void) {return 0;}

	// Public utilities
	public:

		// subclasses must override these function
		virtual BOOL Activate(void) {return FALSE;}
		virtual int SetPane(int pane){return ERROR_ERROR;}
		virtual int SetValue(void) {return 0;}

		int SelectRows(int start_row = ROW_CURRENT, int total_rows = 1);
		int GetState(int row = ROW_CURRENT);

		CString GetType(int row = ROW_CURRENT);
		//CString GetName(int row = ROW_CURRENT);
		CString GetName(int row = ROW_CURRENT, EE_METHOD_TYPE method = METHOD_WND_SEL);
		CString GetValue(int row = ROW_CURRENT);
		
		int GetAllFields(EXPR_INFO* expr_info, int start_row = ROW_CURRENT,
						 int total_rows = 1, EE_METHOD_TYPE method = METHOD_PROP_PAGE);

		int Expand(int row = ROW_CURRENT);
		int Collapse(int row = ROW_CURRENT);
	};

#endif //__UIEEWND_H__
