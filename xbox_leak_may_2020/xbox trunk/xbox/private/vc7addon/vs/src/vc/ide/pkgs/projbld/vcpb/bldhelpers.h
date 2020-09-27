#ifndef __BLDHELPERS_H__
#define __BLDHELPERS_H__

#pragma once

#include <vccolls.h>
#include <path2.h>
#include <VCFileClasses.h>
#include "BuildLogging.h"
#include "bldspawner.h"

// classes in this header
class CTempMaker;
class CBldSpawner;
class CVCBuildStringCollection;
class CBldOptionList;

// classes used by this header 
class CBldFileRegSet;		// requires FileRegistry.h anywhere we actually use this...
class CDynamicBuildEngine;	// requires BuildEngine.h anywhere we actually use this...

#define FILE_RESPONSE	0
#define FILE_BATCH		1
#define FILE_TEMP		2
#define FILE_XML		3

// creating and deleting a list of temporary files
class CTempMaker
{
public:
	// FUTURE: pick better size? (prime)
	CTempMaker() : m_bMustBeNew(false) { m_Files.InitHashTable(17); }
	~CTempMaker();	

	BOOL CreateTempFile(CVCStdioFile*& pFile, CPathW& rPath, IVCBuildErrorContext* pEC, CBldCfgRecord* pCfgRecord,
		int fFileType = FILE_RESPONSE);
	BOOL CreateTempFileFromString(CStringW& str, CPathW& rPath, CDynamicBuildEngine* pBuildEngine, IVCBuildErrorContext* pEC, 
		CBldCfgRecord* pCfgRecord, int fFileType = FILE_RESPONSE);

	// Remove all the files from disk:
	void NukeFiles(IVCBuildErrorContext* pEC = NULL);

private:
	CMapPathWToPtr m_Files;
	CDirW m_DirTempPath;
	bool m_bMustBeNew;
	static unsigned long s_ulSeed;
};

///////////////////////////////////////////////////////////////////////////////
//
//	Class for spawning a process and invoking a tool.  Also, this is the global
//	reposititory about building/spawning state information:  
//
class CBldSpawner 
{
public:
	CBldSpawner();

	BOOL CanSpawn();

	//	Calls InitSpawn down in the IDE
	HRESULT InitSpawn(BOOL bHidden = FALSE);
	int DoSpawn(IVCCommandLineList* pCmds, CDirW& dir, CDynamicBuildEngine* pBldEngine, CBldCfgRecord* pCfgRecord, 
		BOOL fCheckForComspec = FALSE, BOOL fIgnoreErrors = FALSE, IVCBuildErrorContext* pEC = NULL, BOOL fAsyncSpawn = TRUE);

	void TermSpawn();
	void DoSpawnCancel();
	void ReInitSpawn();

	__inline BOOL SpawnActive() {return m_InitCount > 0 && !m_bHidden;}

	__inline BOOL HiddenSpawnActive() {return m_InitCount > 0 && m_bHidden;}

	BOOL UserStoppedBuild();

	UINT GetCodePage(IVCBuildErrorContext* pEC);

private:
	int WaitForSpawnTerm(BOOL bPump, BOOL *pbUserCancel);
	void FormatError(int nIDError, CStringW& strCmdLine, CStringW& rstrError);
	void IdleAndPump(HANDLE hEventEarlyOut, BOOL fIdleAndPump);
	BOOL OkToSpawn(IVCCommandLineList* pCmds);
	void BuildCommandLineList(IVCCommandLineList* pCmds, CVCStringWList& slCmds, CVCStringWList& slDesc);
	int HandleUserCancel(CDynamicBuildEngine* pBldEngine);
	void CreateSpawner(CDirW &dir);
	void ClearSpawner();
	void HandleCheckForComspec(CBldCfgRecord* pCfgRecord, CStringW& strCmdLine, int& nRetVal, int& iCmd);
	void DoActualSpawn(CVCStringWList& slSpawnCmds, CDynamicBuildEngine* pBldEngine, BOOL fUseConsoleCP, int& nRetval, 
		int& iCmd, BOOL& bUserCancel, BOOL& bSpawnFailed);
	int DetermineSpawnRetval(CVCStringWList& slSpawnCmds, CDynamicBuildEngine* pBldEngine, CBldCfgRecord* pCfgRecord, int& nRetval, 
		int& iCmd, BOOL& bUserCancel, BOOL& bSpawnFailed);
	
private:
	// OS command shell specification
	CStringW m_strComspec;

	BOOL m_bBuildTerminated;
	BOOL m_fUserStoppedBuild;

	// Count of requests for InitSpawn
	int	m_InitCount;

	// spawner object (defined in shell) used to actually run the build
	CBldConsoleSpawner *m_pSpawner;
	DWORD m_dwFileWarnings;
	DWORD m_dwFileErrors;
	CComPtr<IVCBuildErrorContext> m_pEC;

	// m_bHidden implies a spawn that is not noticeable by the user
	// and is automatically terminated before a regular spawn occurs
	// A hidden spawn is useful for Edit and Continue performance,
	// as it allows the compiler to preload the PCH while the user is
	// editing a file during a debugging session.
	BOOL m_bHidden;
};

/////////////////////////////////////////////////////////////////////////////
// CVCBuildStringCollection

class CVCBuildStringCollection : 
	IVCBuildStringCollection,
	public CComObjectRoot
{
public:
	CVCBuildStringCollection() : m_nIndex(0) {}
	static HRESULT CreateInstance(IVCBuildStringCollection** ppCollection, CVCBuildStringCollection** ppCollectionObj = NULL);

BEGIN_COM_MAP(CVCBuildStringCollection)
	COM_INTERFACE_ENTRY(IUnknown)
	COM_INTERFACE_ENTRY(IVCBuildStringCollection)
END_COM_MAP()
DECLARE_NOT_AGGREGATABLE(CVCBuildStringCollection) 

// IVCBuildStringCollection
public:
	STDMETHOD(Reset)();
	STDMETHOD(get_Count)(long *pVal);
	STDMETHOD(Next)(BSTR* pbstrElem);
	STDMETHOD(Add)(BSTR bstrVal);
	STDMETHOD(AddAtLocation)(long nLocation, BSTR bstrVal);	// add the element at the specified index; 1-based
	STDMETHOD(Remove)(long nLocation);	// remove the element at the specified index; 1-based
	STDMETHOD(Clear)();	// remove all elements
	STDMETHOD(RemoveDuplicates)(VARIANT_BOOL bIsPaths, VARIANT_BOOL bIsCaseSensitive);

public:
	CVCStringWArray m_strStrings;

protected:
	int m_nIndex;
};

class CBldTurnRegRecalcOff
{
public:
	CBldTurnRegRecalcOff(CBldFileRegSet* pRegSet);
	~CBldTurnRegRecalcOff();
protected:
	CBldFileRegSet* m_pRegSet;
	BOOL m_fOldOn;
};

// NOTE: no duplicates are allowed in the option list

typedef struct tagBldOptEl {
	BYTE cref;		// ref. count
	int cch;		// # of chars. in pch (incl. '\0')
	wchar_t pch[1];	// space for single terminator
} BldOptEl;

class  CBldOptionList
{
public:
	// ConstrucTORs
	CBldOptionList(wchar_t chJoin = L',', BOOL fUseDefJoins = TRUE, BOOL fAllowDupes = FALSE)
	{
		m_cchStr = 0;
		m_bAllowDuplicates = fAllowDupes; 
		SetJoinUsage(chJoin, fUseDefJoins);
	}

	CBldOptionList(const wchar_t * pch, wchar_t chJoin = L',', BOOL fUseDefJoins = TRUE, BOOL fAllowDupes = FALSE)
	{
		m_cchStr = 0;
		m_bAllowDuplicates = fAllowDupes; 
		SetJoinUsage(chJoin, fUseDefJoins); 
		SetString(pch);
	}

	__inline void SetJoinUsage(wchar_t chJoin = L',', BOOL fUseDefJoins = TRUE)
	{
		m_strJoin = (CStringW)chJoin + (fUseDefJoins ? L",;\t" : L"");
	}

	// DestrucTOR
	virtual ~CBldOptionList()
		{ Empty(); }

	void Empty();										// empty
	void GetString(CStringW& str);						// conversion
	void SetString(const wchar_t* pch);					// conversion
	__inline const CBldOptionList& operator=(CBldOptionList& optlst)	// assignment
	{
		Empty(); 
		Append(optlst); 
		return *this;
	}	
	void Append(const wchar_t* pch, int cch);			// element append
	void Append(CBldOptionList& optlst);				// list append
	__inline BOOL Exists(const wchar_t* pch)			// existence?
	{
		VCPOSITION posOurWord; 
		return m_bAllowDuplicates? FALSE : m_mapStrToPtr.Lookup(pch, (void *&)posOurWord);
	}

	// enumeration
	__inline VCPOSITION GetHeadPosition()
		{ return m_lstStr.GetHeadPosition(); }
	__inline wchar_t * GetNext(VCPOSITION & pos)
		{ return ((BldOptEl *)m_lstStr.GetNext(pos))->pch; }
	__inline int GetCount()
		{ return (int) m_lstStr.GetCount(); }

protected:
	// addition/deletion of elements using ref. counting
	void Add(BldOptEl* poptel);

	CStringW m_strJoin;					// our join chars
	CVCMapStringWToPtr	m_mapStrToPtr;	// our hash lookup
	CVCPtrList m_lstStr;				// our actual list
	int m_cchStr;						// total length of string (bytes, incl. join char. seps.)
	BOOL m_bAllowDuplicates;
};


extern BOOL GetItemOutputFile(IVCBuildAction* pBldAction, IUnknown* pItem, long nFileNameID,
	BldFileRegHandle& frh, const wchar_t * pcDefExtension, const CDirW* pBaseDir, BOOL bIsDefinitelyDir = FALSE);
extern BOOL GetItemOutputFile(IVCBuildAction* pBldAction, IUnknown* pItem, long nFileNameID,
	BldFileRegHandle& frh, const wchar_t * pcDefExtension, BSTR bstrDir, BOOL bIsDefinitelyDir = FALSE);

extern BOOL GetQuotedString(const wchar_t*& pBase, const wchar_t*& pEnd); 
	// Look through the string to find a quoted substring. On return
	// pBase points to first char after first pEnd points to last quote.
	// Return FALSE if < 2 quotes or 0 length substring.
extern BOOL BldFileNameMatchesExtension(LPCOLESTR szPath, const  wchar_t* pExtensions);
extern BOOL BldFileNameMatchesExtension(const CPathW* pPath, const wchar_t* pExtensions);

// some useful macros
#define CHECK_REQUIRED_VT(vt, vttype) \
	if ((vt) != (vttype)) \
	{	\
		VSASSERT(FALSE, "Wrong vt type on VARIANT"); \
		continue; \
	}
#define CHECK_REQUIRED_VT2(vt, vttype1, vttype2) \
	if ((vt) != (vttype1) && (vt) != (vttype2)) \
	{	\
		VSASSERT(FALSE, "Wrong vt type on VARIANT"); \
		continue; \
	}
#define CONTINUE_ON_NULL(val) \
	if ((val) == NULL) \
	{ \
		VSASSERT(FALSE, "NULL pointer in enumeration"); \
		continue; \
	}
#define VALID_CONTINUE_ON_NULL(val) \
	if ((val) == NULL) \
		continue; 
#define CONTINUE_ON_FAIL_OR_NULL(ptr, hr) \
	if ((val) == NULL || FAILED(hr)) \
	{ \
		VSASSERT(val != NULL, "NULL pointer in enumeration"); \
		VSASSERT(SUCCEEDED(hr), "Failed Next in enumeration"); \
		continue; \
	}
#define CONTINUE_ON_FAIL(hr) \
	if (FAILED(hr)) \
	{ \
		VSASSERT(FALSE, "Failed Next in enumeration"); \
		continue; \
	}
#define VOID_RETURN_ON_FAIL_OR_NULL(ptr, hr) \
	if ((ptr) == NULL || FAILED(hr)) \
	{ \
		VSASSERT(ptr != NULL, "Unexpected NULL pointer"); \
		VSASSERT(SUCCEEDED(hr), "Unexpected failure code"); \
		return; \
	}
#define VOID_RETURN_ON_NULL(ptr) \
	if ((ptr) == NULL) \
	{ \
		VSASSERT(FALSE, "Unexpected NULL pointer"); \
		return; \
	}

#define HR_RETURN_ON_FAIL_OR_NULL(ptr, hr) \
	if ((ptr) == NULL || FAILED(hr)) \
	{ \
		VSASSERT(ptr != NULL, "Unexpected NULL pointer"); \
		VSASSERT(SUCCEEDED(hr), "Unexpected failure code"); \
		return (hr); \
	}

#define HR_RETURN_ON_NULL(ptr, hr) \
	if ((ptr) == NULL) \
	{ \
		VSASSERT(FALSE, "Unexpected NULL pointer"); \
		return (hr); \
	}

#endif	// __BLDHELPERS_H__
