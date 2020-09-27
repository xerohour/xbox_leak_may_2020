/////////////////////////////////////////////////////////////////////////////
// toolset.h
//
// email	date		change
// briancr	11/04/94	created
//
// copyright 1994 Microsoft

// Interface of the CToolset class

#ifndef __TOOLSET_H__
#define __TOOLSET_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#pragma warning(disable: 4251)

#include "log.h"
#include "toolxprt.h"

#include "platform.h"


class CIDE;		// Forward declarations.
class CTarget;
class CSettings;
class CCompilerDriver;
class CExecutable;

/////////////////////////////////////////////////////////////////////////////
// CToolset

typedef CTypedPtrList<CPtrList, CTarget*> CToolPtrList;


class TOOLSET_CLASS CToolset
{
// ctor/dtor
public:

	enum BUILD_TYPE {Debug, Retail};

	CToolset(BUILD_TYPE nBuildType, HANDLE hOutput);
	virtual ~CToolset();

protected:
	CToolset() { ASSERT(FALSE); }  // not to be used

// data types
public:
	enum ToolsetLangs { TL_ENGLISH, TL_GERMAN, TL_JAPANESE };

// access attributes
public:
	virtual BOOL SetLanguage(ToolsetLangs lang);
	virtual ToolsetLangs GetLanguage(void);

// operations
public:
	// initialize the toolset
	virtual BOOL Initialize(CString strBaseKey) = 0;

	// display the options tabbed dialog
	virtual BOOL ToolsetOptions(void);

	// register tools
	virtual BOOL AddTool(CTarget *pTarget);

	// apply settings in the given object to the toolset
	virtual BOOL ApplySettings(CSettings* pSettings);

	CSettings* GetToolsetSettings(void)				{ ASSERT(m_psettingsToolset); return m_psettingsToolset; }
	CSettings* GetEnvSettings(void)					{ ASSERT(m_psettingsEnv); return m_psettingsEnv; }

	// set the log the toolset will use
	virtual BOOL SetLog(CLog* pLog);
  virtual CLog* GetLog();

  const CToolPtrList& GetToolList();

// attributes
public:
	// static data about the toolset
/*	static CString m_strToolsets[];
	static CString m_strToolsetSwitches[];
	static CString m_strLanguage[];*/
	static CString m_strBuildType[];
	static CString m_strEnvvars[];

// operations (internal)
protected:
	// initialize toolset settings
	virtual BOOL InitializeSettings(void);
	virtual BOOL InitializeEnvSettings(void);
	virtual BOOL InitializeToolsetSettings(void);
	
// data
protected:
	CToolPtrList m_listTools;
	CSettings* m_psettingsEnv;
	CSettings* m_psettingsToolset;
	CString m_strBaseKey;
	CString m_strLang;
	CString m_strDebug;
	HANDLE m_hOutput;
	BUILD_TYPE m_nBuildType;
	
	// the log the toolset will use
	CLog* m_pLog;
	// toolset's language
	ToolsetLangs m_lang;
	// handle to the dll that contains the localized ide strings.
	HINSTANCE m_hStringsDll;
};


/*  toolset creation */

TOOLSET_API CToolset * CreateToolset(CPlatform::COS *pOS, CPlatform::CLanguage *pLang, CToolset::BUILD_TYPE, HANDLE hOutput);




/////////////////////////////////////////////////////////////////////////////
// CDevTools

class TOOLSET_CLASS CDevTools: public CToolset
{
// ctor/dtor
public:
	CDevTools(BUILD_TYPE nBuildType, HANDLE hOutput);
	~CDevTools();

protected:
	CDevTools();

// operations
public:
	virtual BOOL Initialize(CString strBaseKey) = 0;

// data
protected:
	CIDE* m_pIDE;
  CCompilerDriver *m_pCompilerDriver;
  CExecutable *m_pExecutable;
};


/////////////////////////////////////////////////////////////////////////////
// CVCTools

const CString settingPath = "Path";
const CString settingLib = "Lib";
const CString settingInclude = "Include";

class TOOLSET_CLASS CVCTools: public CDevTools
{
// ctor/dtor
public:
	CVCTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CDevTools(nBuildType, hOutput) {}
	virtual ~CVCTools() {}

protected:
	CVCTools() {}

// operations
public:
	virtual BOOL Initialize(CString strBaseKey) = 0;

	
	virtual CIDE* GetIDE(void)	{ return m_pIDE; } // get a pointer to the IDE
	virtual CCompilerDriver* GetCompilerDriver(void)	{ return m_pCompilerDriver; }
	virtual CExecutable* GetExecutable(LPCSTR szExeName);

// operations (internal)
protected:

// data
protected:
//	CIDE* m_pIDE;
};


/////////////////////////////////////////////////////////////////////////////
// CX86Tools

const CString strX86ToolsKey = "X86 Toolset";

class TOOLSET_CLASS CX86Tools: public CVCTools
{
// ctor/dtor
public:
	CX86Tools(BUILD_TYPE nBuildType, HANDLE hOutput) : CVCTools(nBuildType, hOutput) { m_pIDE = 0; }  // for now
	virtual ~CX86Tools() { }

protected:
  CX86Tools() {}

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};


/////////////////////////////////////////////////////////////////////////////
// CWin32sTools

const CString strWin32sToolsKey = "Win32s Toolset";

class TOOLSET_CLASS CWin32sTools: public CX86Tools
{
// ctor/dtor
public:
	CWin32sTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CX86Tools(nBuildType, hOutput) { }  
	virtual ~CWin32sTools() { }

protected:
  CWin32sTools() {}

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};

/////////////////////////////////////////////////////////////////////////////
// C68KTools

const CString str68KToolsKey = "68K Toolset";

class TOOLSET_CLASS C68KTools: public CX86Tools
{
// ctor/dtor
public:
	C68KTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CX86Tools(nBuildType, hOutput) { }
	virtual ~C68KTools() { }

protected:
	C68KTools() { }

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};

/////////////////////////////////////////////////////////////////////////////
// CPMacTools

const CString strPMacToolsKey = "PowerMac Toolset";

class TOOLSET_CLASS CPMacTools: public CX86Tools
{
// ctor/dtor
public:
	CPMacTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CX86Tools(nBuildType, hOutput)  { }
	virtual ~CPMacTools() { }

protected:
	CPMacTools() { }

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};

/////////////////////////////////////////////////////////////////////////////
// CMIPSTools

const CString strMIPSToolsKey = "MIPS Toolset";

class TOOLSET_CLASS CMIPSTools: public CVCTools
{
// ctor/dtor
public:
	CMIPSTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CVCTools(nBuildType, hOutput)  { }
	virtual ~CMIPSTools() { }

protected:
	CMIPSTools() { }

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};

/////////////////////////////////////////////////////////////////////////////
// CAlphaToolset

const CString strAlphaToolsKey = "Alpha Toolset";

class TOOLSET_CLASS CAlphaTools: public CVCTools
{
// ctor/dtor
public:
	CAlphaTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CVCTools(nBuildType, hOutput) { }
	virtual ~CAlphaTools() { }

protected:
	CAlphaTools() { }

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};

/////////////////////////////////////////////////////////////////////////////
// CPowerPCToolset

const CString strPowerPCToolsKey = "PowerPC Toolset";

class TOOLSET_CLASS CPowerPCTools: public CVCTools
{
// ctor/dtor
public:
	CPowerPCTools(BUILD_TYPE nBuildType, HANDLE hOutput) : CVCTools(nBuildType, hOutput) { }
	virtual ~CPowerPCTools() { }

protected:
	CPowerPCTools() { }

// operations
public:
	virtual BOOL Initialize(CString strBaseKey);
};


#endif // __TOOLSET_H__
