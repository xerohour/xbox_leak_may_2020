// meminfo.h: interface for the CXboxMemoryInfo class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMINFO_H__CF13C9F7_ECBF_42FE_B045_C5F358849597__INCLUDED_)
#define AFX_MEMINFO_H__CF13C9F7_ECBF_42FE_B045_C5F358849597__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DmConnection.h"
#include "PfnDatabase.h"	// Added by ClassView
#include "vad.h"	// Added by ClassView

class CXboxMemoryInfo : public CDmConnection  
{
public:
	CXboxMemoryInfo();
	virtual ~CXboxMemoryInfo();

	HRESULT DownloadVad();
	HRESULT DownloadMmGlobal();
	HRESULT DownloadPfnDatabase();

    const CPfnDatabase& GetPfnDatabase() const;
    const CVadArray& GetVad() const;
	
    ULONG GetAvailablePages() const;
    ULONG GetTotalNumberOfPhysicalPages() const;
    ULONG GetAllocatedPagesByUsage(ULONG Usage) const;
	ULONG GetAllocatedKBytesByUsage(ULONG Usage) const;

	bool IsValidHeap(PVOID Va) const;

protected:
	CVadArray m_vad;
	CPfnDatabase m_pfn;
	MMGLOBALDATA m_MmGlobalData;
	ULONG m_HighestPhysicalPage;
    ULONG m_MmNumberOfPhysicalPages;
    ULONG m_MmAvailablePages;
	ULONG m_AllocatedPagesByUsage[MmMaximumUsage];
    const PMMPFN m_PfnDatabaseAddr;
};

#endif // !defined(AFX_MEMINFO_H__CF13C9F7_ECBF_42FE_B045_C5F358849597__INCLUDED_)
