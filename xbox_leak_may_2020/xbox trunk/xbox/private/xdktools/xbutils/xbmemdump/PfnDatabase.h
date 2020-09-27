// PfnDatabase.h: interface for the CPfnDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PFNDATABASE_H__ECC446E6_D544_48EF_8EC3_16917F87E20B__INCLUDED_)
#define AFX_PFNDATABASE_H__ECC446E6_D544_48EF_8EC3_16917F87E20B__INCLUDED_

#include "DmConnection.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CPfnDatabase
{
public:
	CPfnDatabase();
	virtual ~CPfnDatabase();

	bool MakeCopy(PMMPFN ppfn, ULONG NumberOfPages);

	bool IsLockedPage(ULONG Page) const;
	bool IsFreePage(ULONG Page) const ;
	bool IsPhysicallyMappedPage(ULONG Page) const;

	ULONG GetPfnDatabaseSize() const;
	ULONG GetLockCount(ULONG Page) const;
	ULONG GetBusyType(ULONG Page) const;
    ULONG GetPteIndex(ULONG Page) const;
	const MMPTE* GetPte(ULONG Page) const;

protected:
	bool Grow(ULONG NumberOfPages);

	PMMPFN m_pfn;
	ULONG m_NumberOfPages;

private:
    CPfnDatabase& operator=(const CPfnDatabase& rhs);
};

#endif // !defined(AFX_PFNDATABASE_H__ECC446E6_D544_48EF_8EC3_16917F87E20B__INCLUDED_)
