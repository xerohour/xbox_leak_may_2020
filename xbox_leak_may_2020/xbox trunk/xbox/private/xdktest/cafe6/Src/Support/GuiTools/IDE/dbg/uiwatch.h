///////////////////////////////////////////////////////////////////////////////
//  UIWATCH.H
//
//  Created by :            Date :
//      MichMa              	1/17/94
//
//  Description :
//		Declaration of the UIWatch class
//

#ifndef __UIWATCH_H__
#define __UIWATCH_H__

#include "uieewnd.h"

#ifndef __UIEEWND_H__
	#error include 'uieewnd.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIWatch class

#define NAME_NOT_EDITABLE  0
#define NAME_IS_EDITABLE  -1

#define PANE_WATCH1	IDSS_WATCH_PANE_WATCH1
#define PANE_WATCH2	IDSS_WATCH_PANE_WATCH2
#define PANE_WATCH3	IDSS_WATCH_PANE_WATCH3
#define PANE_WATCH4	IDSS_WATCH_PANE_WATCH4

// BEGIN_CLASS_HELP
// ClassName: UIWatch
// BaseClass: UIEEWindow
// Category: Debug
// END_CLASS_HELP
class DBG_CLASS UIWatch : public UIEEWindow
	
	{
	UIWND_COPY_CTOR(UIWatch, UIEEWindow);

	virtual BOOL Activate(void);
	virtual UINT GetID(void) const {return IDW_WATCH_WIN;}

	// Public utilities
	public:

		int SetPane(int pane);
		int SetName(LPCSTR name, int row = ROW_CURRENT);

		int SetValue(LPCSTR value, int row = ROW_CURRENT, 
					 int name_editability = NAME_IS_EDITABLE);

		int Delete(int start_row = ROW_CURRENT, int total_rows = 1);
		BOOL SelectWatch(LPCSTR szWatch);
		int GetWatchCount(void);
	};

#endif //__UIWATCH_H__
