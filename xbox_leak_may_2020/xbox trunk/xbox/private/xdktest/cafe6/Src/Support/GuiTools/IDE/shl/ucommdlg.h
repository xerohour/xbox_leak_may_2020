///////////////////////////////////////////////////////////////////////////////
//	UCOMMDLG.H
//
//	Created by :			Date :
//		DavidGa					10/21/93
//
//	Description :
//		Declaration of the Common Dialog Utility classes
//

#ifndef __UICOMMDLG_H__
#define __UICOMMDLG_H__

#include "..\..\udialog.h"
#include "guiv1.h"
#include "wbutil.h"
#include "Strings.h"

#include "shlxprt.h"

#ifndef __UIDIALOG_H__
	#error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//	control IDs, obtained from Spy++

#define IDC_FILE_FILENAME		0x480	// edt1
#define IDC_FILE_FILENAMELIST	0x460	// lst1
#define IDC_FILE_CURDIR			0x440	// stc1
#define IDC_FILE_DIRECTORIES		0x461	// lst2
#define IDC_FILE_FILETYPES		0x470	// cmb1
#define IDC_FILE_DRIVES			0x471	// cmb2
#define IDC_FILE_HELP			0xE145
#define IDC_FILE_NETWORK			0x40D	// btn13
#define IDC_FILE_READONLY		0x410	// chk1
#define IDC_FILE_OPENAS			0x5033
//#define IDC_FILE_OK		IDOK
//#define IDC_FILE_CANCEL	CANCEL
#define IDC_FONT_NAME			0x470	// cmb1
#define IDC_FONT_STYLE			0x471	// cmb2
#define IDC_FONT_SIZE			0x472	// cmb3

///////////////////////////////////////////////////////////////////////////////
//	UIFileDlg class

// BEGIN_CLASS_HELP
// ClassName: UIFileDlg
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIFileDlg : public UIDialog
{
	UIWND_COPY_CTOR(UIFileDlg, UIDialog);

// Utilities
public:
	void SetName(LPCSTR sz);
	CString GetName();
	void SetPath(LPCSTR sz);
	BOOL OK(BOOL bOverwrite);

	virtual HWND GetDlgItem(UINT id) const;
	virtual LPCSTR GetLabel(UINT id) const;

	// get the extension dialog for this common dialog, if it exists
	virtual HWND GetExtensionDialog(void) const;
};

///////////////////////////////////////////////////////////////////////////////
//	UIFileDlg class

// BEGIN_CLASS_HELP
// ClassName: UIFileSaveAsDlg
// BaseClass: UIFileDlg
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIFileSaveAsDlg : public UIFileDlg
{
	UIWND_COPY_CTOR(UIFileSaveAsDlg, UIFileDlg);

// Utilities
public:
	virtual BOOL VerifyUnique(void) const;
	virtual CString ExpectedTitle(void) const
		{	return GetLocString(IDSS_FSA_TITLE); }
};

///////////////////////////////////////////////////////////////////////////////
//	UIFileOpenDlg class

enum EOpenAs { OA_AUTO, OA_TEXT, OA_BINARY, OA_MAKEFILE };

// BEGIN_CLASS_HELP
// ClassName: UIFileOpenDlg
// BaseClass: UIFileDlg
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIFileOpenDlg : public UIFileDlg
{
	UIWND_COPY_CTOR(UIFileOpenDlg, UIFileDlg);

// Utilities
public:
	HWND Display();
	virtual BOOL VerifyUnique(void) const;
	virtual CString ExpectedTitle(void) const
		{	return GetLocString(IDSS_FO_TITLE); }
	void SetEditor(EOpenAs oa);
	void SetReadOnly(BOOL b);
};

///////////////////////////////////////////////////////////////////////////////
//	UIFontDlg class

enum { FONT_NORMAL = 1, FONT_ITALIC, FONT_BOLD, FONT_BOLDITALIC };

// BEGIN_CLASS_HELP
// ClassName: UIFontDlg
// BaseClass: UIDialog
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS UIFontDlg : public UIDialog
{
	UIWND_COPY_CTOR(UIFontDlg, UIDialog);

// Utilities
public:
	virtual BOOL VerifyUnique(void) const;
	virtual CString ExpectedTitle(void) const
		{	return GetLocString(IDSS_FD_TITLE, GetSysLang()); }	// no template, so title depends on system
	void SetName(LPCSTR szName);
	void SetSize(int nSize);
	void SetStyle(int nStyle);
};

#endif //__UICOMMDLG_H__
