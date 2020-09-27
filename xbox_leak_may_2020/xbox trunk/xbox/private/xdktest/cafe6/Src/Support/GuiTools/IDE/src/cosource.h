///////////////////////////////////////////////////////////////////////////////
//	COSOURCE.H
//
//	Created by :			Date :
//		DavidGa					1/10/94
//
//	Description :
//		Declaration of the COSource component object class
//

#ifndef __COSOURCE_H__
#define __COSOURCE_H__

#include "..\eds\cofile.h"

#include "export.h"
#include "..\shl\uioptdlg.h"

#define ERROR_ERROR	-1	// REVIEW: define this in a better place

///////////////////////////////////////////////////////////////////////////////
// SOURCE_CREATE_STRUCT class

struct SOURCE_CREATE_STRUCT		// REVIEW: what else?
{
	LPCSTR szFileName;
};

typedef enum { SELECT_NORMAL, SELECT_COLUMN, SELECT_BRIEF_COLUMN } SelectionType;

///////////////////////////////////////////////////////////////////////////////
// COSource class

// BEGIN_CLASS_HELP
// ClassName: COSource
// BaseClass: COFile
// Category: Editors
// END_CLASS_HELP
class SRC_CLASS COSource : public COFile
{
public:
// data types
	enum FindDirection { FD_UP, FD_DOWN };
	enum ReplaceScope { RS_WHOLEFILE, RS_SELECTION };
// defines for Compatibility choices

public:
	COSource();

// Data
protected:

// File Operations
public:
	int Create(LPCSTR szSaveAs = NULL);
	virtual int Open(LPCSTR szFileName, LPCSTR szChDir = NULL, EOpenAs oa = OA_AUTO);

// Editor Attributes
public:
	BOOL GetReadOnly(void);
	int GetCurrentLine(void);
	int GetCurrentColumn(void);
	CString GetSelectedText(void);

// Selection Operations
public:
	void SelectText(int line1, int column1, int line2, int column2, BOOL bColumnSelect = SELECT_NORMAL);
	void SelectLines(int startline = 0, int count = 1);
	void SetCursor(int line, int column);
	void InsertText(LPCSTR szText, int line, int column, BOOL bLiteral=FALSE);
	void TypeTextAtCursor(LPCSTR szText,BOOL bLiteral=FALSE);

// operations
public:
	BOOL GoToLine(LPCSTR szLine, BOOL bCloseAfter = TRUE);
	BOOL GoToLine(int nLine, BOOL bCloseAfter = TRUE);

	BOOL SetFont(LPCSTR pszFontName = NULL, int iFontSize = 0, LPCSTR pszFontStyle = NULL);

// Emulation Selection
	BOOL SelectEmulations(EmulationType eEM);

// search and replace operations
	BOOL Find(LPCSTR szFind, BOOL bMatchWord = FALSE, BOOL bMatchCase = FALSE, BOOL bRegExpr = FALSE, int fd = FD_DOWN);
	BOOL Replace(LPCSTR szFind, LPCSTR szReplace, BOOL bMatchWord = FALSE, BOOL bMatchCase = FALSE, BOOL bRegExpr = FALSE, ReplaceScope rs = RS_WHOLEFILE, BOOL bAll = FALSE);

//	TODO: Bookmark functions, if anyone needs them

};

enum FIND_DIRECTION {FIND_UP = COSource::FD_UP, FIND_DOWN = COSource::FD_DOWN };

#endif //__COSOURCE_H__
