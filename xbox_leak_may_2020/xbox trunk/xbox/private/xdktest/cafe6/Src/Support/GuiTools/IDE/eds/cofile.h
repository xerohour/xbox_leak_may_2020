///////////////////////////////////////////////////////////////////////////////
//	COFILE.H
//
//	Created by :			Date :
//		DavidGa					1/10/94
//
//	Description :
//		Declaration of the COFile component object class
//

#ifndef __COFILE_H__
#define __COFILE_H__

#include "ueditor.h"
#include "..\shl\ucommdlg.h"

#include "edsxprt.h"

#define ERROR_ERROR -1

// use to tell Create() which type of file to create.
enum FILE_TYPE {
		FILE_ACTIVE_SERVER_PAGE	= IDSS_NEW_ACTIVE_SERVER_PAGE,	
		FILE_BINARY				= IDSS_NEW_BINARY,			
		FILE_BITMAP				= IDSS_NEW_BITMAP,
		FILE_HEADER				= IDSS_NEW_HEADER,
		FILE_SOURCE				= IDSS_NEW_CPP_SOURCE,
		FILE_CURSOR				= IDSS_NEW_CURSOR, 
		FILE_HTML_PAGE			= IDSS_NEW_HTML_PAGE,			  	
		FILE_ICON				= IDSS_NEW_ICON,
		FILE_MACRO				= IDSS_NEW_MACRO,
		FILE_ODBC_SCRIPT		= IDSS_NEW_ODBC_SCRIPT,
		FILE_RESOURCE_SCRIPT	= IDSS_NEW_RESOURCE_SCRIPT,
		FILE_RESOURCE_TEMPLATE	= IDSS_NEW_RESOURCE_TEMPLATE,
		FILE_TEXT				= IDSS_NEW_TEXT
};

///////////////////////////////////////////////////////////////////////////////
// COFile class

// BEGIN_CLASS_HELP
// ClassName: COFile
// BaseClass: none
// Category: Editors
// END_CLASS_HELP
class EDS_CLASS COFile
{
public:
	COFile();

// Data
protected:
	CString m_strFileName;
	UIEditor m_editor;

// Operations
public:
	int Create(UINT idsType, LPCSTR szSaveAs = NULL, LPCSTR szProject = NULL);
	virtual int AttachActive(void);
	virtual int Open(LPCSTR szFileName, LPCSTR szChDir = NULL, EOpenAs oa = OA_AUTO);
	virtual int Save(void);
	virtual int SaveAs(LPCSTR szFileName, BOOL bOverWrite = FALSE);
	virtual int Close(BOOL bSaveChanges = FALSE);
	virtual inline BOOL AttachActiveEditor(void)
		{	return m_editor.AttachActive(); }

// Generic features
public:
	virtual LPCSTR Cut(CString* pstr = NULL);
	virtual LPCSTR Copy(CString* pstr = NULL);
	virtual void Paste(void);
	virtual void Delete(void);
	virtual void Undo(void);
	virtual void Redo(void);

// Attributes
	LPCSTR GetFileName(void);
};

#endif //__COFILE_H__
