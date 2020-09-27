#ifndef __CODEGEN_H__
#define __CODEGEN_H__
#include "customaw.h"		// Has declaration of OutputStream
#include <fcntl.h>			// Has declaration of _O_BINARY


/////////////////////////////////////////////////////////////////////////////
// codegen.h, codegen.cpp: Code-generator engine


/////////////////////////////////////////////////////////////////////////////
// "OutputStream" structs to handle output

class ListStream : public OutputStream
{
public:
    void WriteLine(LPCTSTR psz)
        {
			LPCTSTR lpchBegin = psz;
			LPCTSTR lpchEnd;
			while ((lpchEnd = _tcschr(lpchBegin, '\n')) != NULL)
			{
				CString strLine(lpchBegin, lpchEnd-lpchBegin);
            	list.AddTail(strLine);
				lpchBegin = _tcsinc(lpchEnd);
			}
			list.AddTail(lpchBegin);
        }
	void WriteBlock(LPCTSTR pb, DWORD dwSize)
		{
			// AppWizard will never call a WriteBlock on an output stream tied
			//  to the New Project Information edit control.  However, a deranged
			//  extension might want to, so we'll grudgingly support it.
			ASSERT(FALSE);
			CString strBlock(pb, dwSize);
			list.AddTail(strBlock);
		}

    CStringList list;
};

class FileStream : public OutputStream
{
public:
    FileStream(FILE* pfile)
        : file(pfile)
        { pOpenFile = pfile; }
	// Normally, we'll close the file ourselves, checking for errors.  When
	//  we do this, we set pOpenFile to NULL.  If an exception is thrown, we don't
	//  have a chance to do this.  Thus, the destructor takes care of it.
	virtual ~FileStream()
		{ if (pOpenFile) fclose(pOpenFile); }
	FILE* pOpenFile;
    void WriteLine(const char* psz)
        {
            file.WriteString(psz);
            file.WriteString("\n");
        }
    void WriteBlock(LPCTSTR pb, DWORD dwSize)
        {
			VERIFY(_setmode(_fileno(pOpenFile), _O_BINARY) != -1);
            file.WriteHuge(pb, dwSize);
        }

private:
    CStdioFile file;        // output only - usually does not close
};

/* void GenFile(const char* szResName, OutputStream& output,
				HINSTANCE hMainRsc, HINSTANCE hLocRsc, CMapStringToString* pDictionary);*/

// Used internally by codegen.
extern CString strFileToOpen;

// Error reporting functions
void ReportErrorExt(UINT nIDP, LPCTSTR lpszArg = NULL);
void ReportAndThrowExt(UINT nIDP, LPCTSTR lpszArg = NULL);
void ReportAndThrow(UINT nIDP, LPCTSTR lpszArg);


/////////////////////////////////////////////////////////////////////////////
// State classes to handle nested $$IF's.

#define MAXSTATE        10       // nested limit of $$IF's.

class State
{
friend class CCodeGen;
public:
    State();
    BOOL Process(LPCTSTR lpch, LPCTSTR *plpchNext);
    BOOL IsOutput()
            { return m_pCurState->m_bOutput; }

protected:
    enum Mode
    {
        idle,
        inIf,
        inElse
    };

    struct IFSTATE
    {
        BOOL m_bOutput;			// currently output
        Mode m_mode;			// control bOutput
        BOOL m_bOneOutput;		// set once if output during IF/ELIF/ELSE
        DWORD m_nLine;		    // record the line # for better error output on IF
    };
    IFSTATE* m_pCurState;		// current state

    int m_iCurState;			// current state 0->MAXSTATE-1
    IFSTATE m_states[MAXSTATE];	// state stack
	CCodeGen* m_pCodeGen;		// code generator it's assoc. w/
};

/////////////////////////////////////////////////////////////////////////////
// One CCodeGen instance is created to generate each file.  CCodeGen's
//  are typically created by the master generator CProjGen below

class CCodeGen
{
friend class State;
friend class CProjGen;
public:
	CCodeGen();

	void Go(LPCTSTR lpszInput, DWORD dwSize, OutputStream* pOutput, CMapStringToString* pSymbols);

protected:
	BOOL InLoop();
	void ScanToAfterEndloop(LPCTSTR& lpch);
	BOOL ProcessControl(LPCTSTR& lpch, LPCTSTR lpchEnd);
	void Translate(CString& pchOut, LPCTSTR lpch);
	void GetLoopKey(LPCTSTR lpszKey, CString& rStrLoopKey);
	BOOL DoesSymbolExist(LPCTSTR lpszKey);
	BOOL GetSymbol(LPCTSTR lpszKey, CString& rStrValue);
	void ReportAndThrow(UINT nIDP, LPCTSTR lpszArg = NULL);

	CMapStringToString* m_pSymbols;
	OutputStream* m_pOutput;
	State m_state;

	BOOL m_bGeneratingWorklist;
	BOOL m_bLoopInProjGen;
	int m_nBeginLoop;
	LPCTSTR m_lpszBeginLoop;
	int m_nIteration, m_nTotalIterations;

	// We keep these around for error reporting
	CString m_strTemplateName;		// Name of template we're parsing
	int m_nLine;					// Current line number we're at
};

/////////////////////////////////////////////////////////////////////////////
// There is at most one CProjGen instance at any time.  It handles creating
//  CCodeGen's, creating the worklist, and parsing through that worklist
//  to drive the generation process.

class CProjGen
{
public:
	CProjGen()
		{ SetCurrent(this); m_nIteration = -1; m_bGeneratingWorklist = FALSE;
		  m_pProjStatus = NULL; m_pFileStatus = NULL; }
	~CProjGen()
		{ SetCurrent(NULL); }
		
	void Go(CWnd* pProjStatus, CWnd* pFileStatus);
	void SetTemplateName(LPCTSTR szTemplateName)	// Called by CCodeGen while processing
		{ m_strTemplateName = szTemplateName; }		//  $$INCLUDE
		
	static void InitCodeGen(CCodeGen* pCodeGen);
	static CProjGen* GetCurrent();
	void SetCurrentIteration(int i)
		{ ASSERT (m_nIteration == -1); m_nIteration = i; }
	void ResetCurrentIteration()
		{ ASSERT (m_nIteration != -1); m_nIteration = -1; }

protected:
	static void SetCurrent(CProjGen* pProjGen);
	BOOL InLoop();
	void GenerateFromWorkList(CStringList& list);

	CString m_strTemplateName;		// Name of template we're currently parsing
	BOOL m_bGeneratingWorklist;
	CWnd* m_pProjStatus;
	CWnd* m_pFileStatus;
	int m_nIteration;
};

// Keeps track of all the files currently on the include stack.  This exists
//  solely to check for recursive includes that would cause a stack overflow.
class IncludeStack
{
public:
	inline void Clear()
		{ m_IncludeStack.RemoveAll(); }
	inline void Add(LPCTSTR szTemplateName)
		{ 
			CString strEntry = szTemplateName;
			strEntry.MakeLower();
			m_IncludeStack[strEntry] = NULL;
		}
	inline BOOL Remove(LPCTSTR szTemplateName)
		{ 
			CString strEntry = szTemplateName;
			strEntry.MakeLower();
			return m_IncludeStack.RemoveKey(strEntry);
		}
	inline BOOL Lookup(LPCTSTR szTemplateName)
		{
			void* p;
			CString strEntry = szTemplateName;
			strEntry.MakeLower();
			return m_IncludeStack.Lookup(strEntry, p);
		}

protected:
	
	CMapStringToPtr m_IncludeStack;
};
extern IncludeStack g_IncludeStack;
extern AppWizTermStatus g_TermStatus;

void LoadAndProcessTemplate(LPCTSTR szTemplateName, OutputStream* pOutput, BOOL bBinary = FALSE);


#endif // __CODEGEN_H__
