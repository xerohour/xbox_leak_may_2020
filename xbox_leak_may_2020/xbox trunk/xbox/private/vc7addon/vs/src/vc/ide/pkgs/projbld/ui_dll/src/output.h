#pragma once

#include "filechng.h"	// for CTestableSection

/////////////////////////////////////////////////////////////////////////////
// CVCBuildOutput 
const int MAXTHREADS = 3;

class CVCBuildOutput : 
	public IVCBuildOutput,
	public CComObjectRoot
{
// Construction
public:
	CVCBuildOutput();   // standard constructor
	~CVCBuildOutput();
	static HRESULT CreateInstance(IVCBuildOutput** ppOutput, IUnknown* pUnkWindowPane);
BEGIN_COM_MAP(CVCBuildOutput)
	COM_INTERFACE_ENTRY(IVCBuildOutput)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCBuildOutput) 

protected:
	HRESULT InitInstance(IUnknown* pUnkWindowPane);

// IVCBuildOutput
public:
	STDMETHOD(FlushToTaskList)(); // TODO: DRA add thread id to all other calls if we need to use these again
	STDMETHOD(AddString)(BSTR str, BOOL bAddCRLF);
	STDMETHOD(EnsureVisible)();
	STDMETHOD(Clear)();
	STDMETHOD(Activate)();
	STDMETHOD(Create)();
	STDMETHOD(Close)();
	STDMETHOD(CloseWithCount)(int *pCount);
	STDMETHOD(CloseAll)();
	STDMETHOD(get_OutputWindowPane)(IUnknown** pOutWindowPane);
	STDMETHOD(OutputTaskItemString)(LPCOLESTR pszOutputString, LONG /*VSTASKPRIORITY*/ nPriority, 
		LONG /*VSTASKCATEGORY*/ nCategory, LPCOLESTR pszSubcategory, LONG /*VSTASKBITMAP*/ nBitmap, 
		LPCOLESTR pszFilename, ULONG nLineNum, LPCOLESTR pszTaskItemText);
	STDMETHOD(SameWindowPane)(IUnknown* pUnkPane);

	static CTestableSection s_sectionOutWindowPane;

// Implementation
protected:
	IVsOutputWindowPane* GetOutWindowPane();
	IStream *m_pStreamOutPane;
	CComPtr<IUnknown> m_spUnkWindowPane;
	CVCMap< int, int, IVsOutputWindowPane *, IVsOutputWindowPane *> m_rgpVsOutputWindowPane;
};
