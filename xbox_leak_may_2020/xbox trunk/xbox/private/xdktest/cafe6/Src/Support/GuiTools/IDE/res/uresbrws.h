///////////////////////////////////////////////////////////////////////////////
//	URESBRWS.H
//
//	Created by :			Date :
//		DavidGa					3/25/94
//
//	Description :
//		Declaration of the UIResBrowser class
//

#ifndef __UIRESBRWS_H__
#define __UIRESBRWS_H__

#include "urestabl.h"

#include "export.h"

#ifndef __UIRESTABL_H__
	#error include 'urestabl.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// UIResBrowser class

// BEGIN_CLASS_HELP
// ClassName: UIResBrowser
// BaseClass: UIResTableEditor
// Category: Editors
// END_CLASS_HELP
class RES_CLASS UIResBrowser : public UIResTableEditor
{
public:
	UIWND_COPY_CTOR(UIResBrowser, UIResTableEditor);

// Attributes
public:
	virtual BOOL IsValid(void) const;

// Utilities
public:	
	int SelectFolder(int nFolder);	// ED_*
	int SelectFolder(LPCSTR szFolder);
	int OpenFolder(int nOpen);
	int FindResource(LPCSTR szId, int nFolder);
	int FindResource(LPCSTR szId, LPCSTR szFolder = NULL);
};

enum { CLOSE_FOLDER, OPEN_FOLDER, OPEN_FOLDER_ALL };

#endif //__UIRESBRWS_H__
