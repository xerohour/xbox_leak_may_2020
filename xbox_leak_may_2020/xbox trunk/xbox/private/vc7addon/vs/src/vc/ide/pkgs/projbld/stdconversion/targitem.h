//
//	CTargetItem
//
//	This is a projitem that represents a target, that is something
//	which is buildable, e.g. an exe, dll, or lib.
//
//	[colint]
//				   

#ifndef _TARGETITEM_H										
#define _TARGETITEM_H
#include "projitem.h"

///////////////////////////////////////////////////////////////////////////////
class CTargetItem : public CProjContainer 
{
//
//	Represents a target. It is the same as a CProjContainer
//	except that conceptually it can have operations applied
//	to it that are normally only applied to the CProject,
//	e.g. build.
//
	DECLARE_SERIAL (CTargetItem)

protected:

public:
	CTargetItem ();
	virtual ~CTargetItem ();

	virtual	GPT GetIntProp(UINT idProp, int& val);
	virtual GPT GetStrProp(UINT idProp, CString &str);
	virtual BOOL SetStrProp(UINT idProp,  const CString &str);

	// allowing/disallowing global registry access to this target registry
	virtual BOOL PreMoveItem(CSlob * & pContainer, BOOL fFromPrj, BOOL fToPrj, BOOL fPrjChanged);

	// .MAK reading
	virtual BOOL ReadFromMakeFile(CMakFileReader& mr, const CDir &BaseDir);

	virtual BOOL CanContain(CSlob* pSlob);
	virtual void FlattenQuery(int fo, BOOL& fAddContent, BOOL& fAddItem);
	
	__inline const CString& GetTargDir() { return m_strTargDir; }
	__inline void SetTargDir(CString &strTargDir) { m_strTargDir = strTargDir; }
	__inline const CString & GetTargetName() { return m_strTargetName; }
	__inline void SetTargetName(const CString & str) { m_strTargetName = str; }

public:
	// Makefile reading/writing support
	static const TCHAR *pcNamePrefix;

	BOOL IsNameComment(CObject*& pObject);

	// Properties
	CString m_strTargetName;
	CString m_strTargDir;                   // save per target directory

	DECLARE_SLOBPROP_MAP()
};

#endif // _TARGETITEM_H
