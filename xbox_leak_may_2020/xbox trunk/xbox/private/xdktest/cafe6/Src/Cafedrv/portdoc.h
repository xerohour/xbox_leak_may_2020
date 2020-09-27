/////////////////////////////////////////////////////////////////////////////
// portdoc.h
//
// email	date		change
// briancr	10/25/94	created
//
// copyright 1994 Microsoft

// Interface of the CViewportDoc class

#ifndef __PORTDOC_H__
#define __PORTDOC_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#undef NAMED_PIPES

#include "settings.h"
#include "logview.h"

/////////////////////////////////////////////////////////////////////////////
// CViewportDoc

class CViewportDoc : public CDocument
{
protected: // create from serialization only
	CViewportDoc();
	DECLARE_DYNCREATE(CViewportDoc)

// Overrides
	virtual void OnCloseDocument();

// operations
public:

	CString GetLine(int line);
	COLORREF GetColor(int line);
	COLORREF GetSelBkColor();

	int GetNumLines() const		{ return m_aFilteredLines.GetSize(); }
	int GetWidth() const		{ return m_nWidth; }

#ifdef NAMED_PIPES
	CString GetPipeName(void)	{ return m_pipeViewport; }
#endif //#else
	HANDLE GetWriteHandle(void)	{ return m_hPipeWrite; }
//#endif

	BOOL GetFilter(UINT nId);
	BOOL SetFilter(UINT nId, BOOL bFilter);

  void SetGenericTitle(LPCSTR szTitleRoot, LPCSTR szNumber);

public:

// operations (internal)
protected:
	BOOL InsertLine(int nLine, CString strLine);
	BOOL AddLine(CString strLine);
	BOOL InitializeSettings(void);
	NodeTypes LineHasKeyWord(CString strLine) ;

	CString GetUniqueName(void);

// Tag operations (internal)
protected:
	int GetTagIndex(int nLine);
	CString GetLineText(int nLine);

// Filter operations (internal)
protected:
	BOOL FilterOneLine(int nLine);
	BOOL FilterAllLines(void);

// Pipe operations (internal)
protected:
	static UINT WrapReadPipe(LPVOID pParam);
	UINT ReadPipe(void);
	BOOL ReadPipeData(CString &strBuf, CString &strRemain, int nLine = -1);

// File operations (internal)
protected:
	BOOL ReadFile(CStdioFile* pfile, int nLine = -1);
	BOOL WriteFile(CStdioFile* pfile, int nBegLine = -1, int nEndLine = -1);


// data types (internal)
protected:
	struct LineInfoType {
		char tag;
		COLORREF color;
		LPCSTR lpszSetting;
	};

// data
protected:
	static LineInfoType m_LineInfo[];

// Implementation
public:
	virtual ~CViewportDoc();
	virtual void Serialize(CArchive& ar);	// overridden for document i/o
#ifdef _DEBUG
	virtual	void AssertValid() const;
	virtual	void Dump(CDumpContext& dc) const;
#endif
protected:
	virtual	BOOL OnNewDocument();
	virtual	void DeleteContents();
	virtual	BOOL OnOpenDocument(const char* pszPathName);
	virtual	BOOL OnSaveDocument(const char* pszPathName);

// data
public:

	CLogView * m_pLogViewTree;

// Data
protected:
	CStringArray m_aLines;
	CArray<int, int> m_aFilteredLines;
	HANDLE m_hthreadPipe;
	CString szLogTreeLine ;
	CSettings* m_psettingsDoc;
	int	m_nWidth;
	int m_nCount;
	int m_nId;
#ifdef NAMED_PIPES
	CString m_pipeViewport;
	CString m_eventTerminate;
	HANDLE m_hTerminate;
#endif //#else
	HANDLE m_hPipeRead, m_hPipeWrite;
//#endif
	CString m_eventOverlapped;

	static int nCount;

  char m_cCurrentAttribute;

// Generated message map functions
protected:
	//{{AFX_MSG(CViewportDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif //__PORTDOC_H__
