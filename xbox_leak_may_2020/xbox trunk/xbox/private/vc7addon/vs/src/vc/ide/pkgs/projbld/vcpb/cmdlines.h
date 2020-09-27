#pragma once

#include "vccolls.h"

// classes in this header
class CVCCommandLineList;
class CVCCommandLine;
class CBldCmdSet;

/////////////////////////////////////////////////////////////////////////////
// CVCCommandLineList

class CVCCommandLineList : 
	public IDispatchImpl<IVCCommandLineList, &IID_IVCCommandLineList, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot
{
public:
	static HRESULT CreateInstance(IVCCommandLineList** ppCmdLineList);
	CVCCommandLineList() : m_posEnumerator(NULL), m_bUseConsoleCodePageForSpawner(VARIANT_FALSE) {}
	~CVCCommandLineList()
	{
		RemoveAll();
	}

BEGIN_COM_MAP(CVCCommandLineList)
	COM_INTERFACE_ENTRY(IVCCommandLineList)
 	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCCommandLineList) 

// IVCCommandLineList
public:
	STDMETHOD(Add)(IVCCommandLine* pCmdLine, BOOL bAddFront);
	STDMETHOD(Reset)();
	STDMETHOD(get_Count)(long *pVal);
	STDMETHOD(Next)(IVCCommandLine** ppCmdLine);
	STDMETHOD(AddList)(IVCCommandLineList* pCmdLineList, BOOL bAddFront);
	STDMETHOD(RemoveAll)();
	STDMETHOD(get_UseConsoleCodePageForSpawner)(VARIANT_BOOL* pbConsoleCP);
	STDMETHOD(put_UseConsoleCodePageForSpawner)(VARIANT_BOOL bConsoleCP);

// IVCCommandLineList
protected:
	CVCPtrList m_commands;
	VCPOSITION m_posEnumerator;
	VARIANT_BOOL m_bUseConsoleCodePageForSpawner;
};

/////////////////////////////////////////////////////////////////////////////
// CVCCommandLine

class CVCCommandLine : 
	public IDispatchImpl<IVCCommandLine, &IID_IVCCommandLine, &LIBID_VCProjectEnginePrivateLibrary, PrivateProjBuildTypeLibNumber,0, CVsTypeInfoHolder>, 
	public CComObjectRoot
{
public:
	static HRESULT CreateInstance(IVCCommandLine** ppCmdLine);
	CVCCommandLine() : m_nProcessors(1) {}
BEGIN_COM_MAP(CVCCommandLine)
	COM_INTERFACE_ENTRY(IVCCommandLine)
 	COM_INTERFACE_ENTRY(IDispatch)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCCommandLine) 

// IVCCommandLine
public:
	STDMETHOD(get_CommandLineContents)(BSTR *pVal);
	STDMETHOD(put_CommandLineContents)(BSTR newVal);
	STDMETHOD(get_Description)(BSTR *pVal);
	STDMETHOD(put_Description)(BSTR newVal);
	STDMETHOD(get_NumberOfProcessors)(long* pnProcessors);
	STDMETHOD(put_NumberOfProcessors)(long nProcessors);

// IVCCommandLine
protected:
	CComBSTR m_bstrCommandLineContents;
	CComBSTR m_bstrDescription;
	long m_nProcessors;
};

class CBldCmdSet
{
public:
	CBldCmdSet() : m_bMakeFirst(FALSE), m_compileAs(compileAsCPlusPlus) {}

	CStringW m_strCommandLine;
	CVCStringWList m_strFilesList;
	BOOL m_bMakeFirst;
	CompileAsOptions m_compileAs;
};