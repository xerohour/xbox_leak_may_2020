
#ifndef __BLDTOOLS_H
#define __BLDTOOLS_H

#include "target.h"

#ifdef EXPORT_BLDTOOLS
  #define BLDTOOLS_CLASS AFX_EXT_CLASS
#else
  #define BLDTOOLS_CLASS __declspec(dllimport)
	#ifdef _DEBUG
		#pragma comment(lib, "bldtoold.lib")
	#else
		#pragma comment(lib, "bldtools.lib")
	#endif
#endif


/////////////////////////////////////////////////////////////////////////////
// CConsoleTarget

class BLDTOOLS_CLASS CConsoleTarget: public CTarget
{
// ctor/dtor
public:
	CConsoleTarget(HANDLE hOutput) { m_hViewport = hOutput; }
	virtual ~CConsoleTarget() {}

// operations
public:

  virtual DWORD GetExitCode(void) { return m_dwExitCode; }
  virtual LPCSTR GetLastOutput(void);
  virtual LPCSTR GetLastCmdLine(void);

  //review(CFlaat): should these be declared at the CGUITarget level instead of at CTarget?
	virtual BOOL Attach(void) { ASSERT(FALSE); return FALSE; }
	
	// exit the target
	virtual BOOL Exit(void) { ASSERT(FALSE); return FALSE; }

// attributes
public:
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey) = 0;

	virtual BOOL ApplySettings(CSettings* pSettings) { return TRUE; UNREFERENCED_PARAMETER(pSettings); }

// operations (internal)
protected:
	virtual BOOL InitializeSettings(void) { return TRUE; }

  // create the target's process
  virtual BOOL CreateTargetProcess(LPCSTR szCmdLine, DWORD dwCreationFlags, LPCSTR szEnvBlock, LPCSTR szWorkDir);

// buffer declarations
protected:

  class COutputBuffer
  {
  public:
    LPCSTR GetAsSZ(void);
    BOOL Append(LPCSTR szText, UINT cchBytes);
    void ResetContents(void);
    BOOL IsEmpty(void);

  protected:
    enum { BufSize = (2 * 1024) };
    char m_acBuf[BufSize];
    char *m_pcNext;
    BOOL m_bFull;
  };

// data
protected:
  HANDLE m_hViewport, m_hCapture;
  DWORD m_dwExitCode;        // holds exit code of last invocation
  COutputBuffer m_bufOutput;
};



/////////////////////////////////////////////////////////////////////////////
// CCompilerDriver

class CLinker;

const CString strCompilerTargetKey("CompilerDriver");


class BLDTOOLS_CLASS CCompilerDriver: public CConsoleTarget
{
// ctor/dtor
public:
	CCompilerDriver(HANDLE hOutput) : CConsoleTarget(hOutput)	{ }
	virtual ~CCompilerDriver()									{ }

// operations
public:
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);

	virtual BOOL CompileOnly(LPCSTR szSrcFile, LPCSTR szCompileArgs=0);                         // CL -c [...]
	virtual BOOL CompileAndLink(LPCSTR szSrcFile, LPCSTR szCompileArgs=0, LPCSTR szLinkArgs=0); // CL [...] -link [...]
  virtual BOOL LinkOnly(LPCSTR szObjFile, LPCSTR szLinkArgs=0);                               // CL -link [...]

// testing operations
public:

// operations (internal)
protected:
	virtual BOOL InitializeSettings(void);

// data
protected:
  CLinker *m_pLinker;
};


class BLDTOOLS_CLASS CX86CompilerDriver: public CCompilerDriver
{
// ctor/dtor
public:
	CX86CompilerDriver(HANDLE hOutput) : CCompilerDriver(hOutput)	{ }
	virtual ~CX86CompilerDriver()									{}

// operations
public:
//	virtual BOOL Initialize(CString strKey);
};



/////////////////////////////////////////////////////////////////////////////
// CExecutable

const CString strExecManagerKey("ExecManager");

class BLDTOOLS_CLASS CExecutable : public CConsoleTarget
{
// ctor/dtor
public:
	CExecutable(HANDLE hOutput) : CConsoleTarget(hOutput)	{ }
	virtual ~CExecutable()									{ }

// operations
public:
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
	virtual BOOL ApplySettings(CSettings* pSettings);

  virtual void SetFileName(LPCSTR szExeFile);

	virtual BOOL Execute(void) = 0;

// testing operations
public:

// operations (internal)
protected:
	virtual BOOL InitializeSettings(void);

// data
protected:

};


class BLDTOOLS_CLASS CX86Executable: public CExecutable
{
// ctor/dtor
public:
	CX86Executable(HANDLE hOutput) : CExecutable(hOutput)	{ }
	virtual ~CX86Executable()	{}

// operations
public:
	virtual BOOL Execute(void);
	virtual BOOL Initialize(CToolset::BUILD_TYPE, CString strKey);
};


#endif __BLDTOOLS_H

