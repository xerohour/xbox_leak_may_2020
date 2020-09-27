// PfnDatabase.cpp: implementation of the CPfnDatabase class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PfnDatabase.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPfnDatabase::CPfnDatabase() :
    m_NumberOfPages(0),
    m_pfn(NULL)
{
}

CPfnDatabase::~CPfnDatabase()
{
    if (m_pfn) {
        _ASSERTE(LocalSize(m_pfn) == sizeof(MMPFN) * m_NumberOfPages);
        LocalFree(m_pfn);
    }
}

bool CPfnDatabase::Grow(ULONG NumberOfPages)
{
    PMMPFN ppfn;

    if (m_NumberOfPages >= NumberOfPages) {
        return true;
    }

    if (m_pfn) {
        ppfn = (PMMPFN)LocalReAlloc(m_pfn, NumberOfPages * sizeof(MMPFN), LPTR);
    } else {
        ppfn = (PMMPFN)LocalAlloc(LPTR, NumberOfPages * sizeof(MMPFN));
    }

    if (!ppfn) {
        return false;
    }

    m_pfn = ppfn;
    m_NumberOfPages = NumberOfPages;
    return true;
}

bool CPfnDatabase::MakeCopy(PMMPFN ppfn, ULONG NumberOfPages)
{
    if (!Grow(NumberOfPages)) {
        return false;
    }

    memcpy(m_pfn, ppfn, sizeof(MMPFN)*NumberOfPages);
    return true;
}

bool CPfnDatabase::IsLockedPage(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);

    if (m_pfn[Page].Pte.Hard.Valid != 0 || m_pfn[Page].Busy.Busy == 0 || \
        m_pfn[Page].Busy.LockCount == 0) {
        return false;
    }

    return true;
}

bool CPfnDatabase::IsFreePage(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return m_pfn[Page].Busy.Busy == 0;
}

bool CPfnDatabase::IsPhysicallyMappedPage(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return m_pfn[Page].Pte.Hard.Valid != 0;
}

const MMPTE* CPfnDatabase::GetPte(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return &m_pfn[Page].Pte;
}

ULONG CPfnDatabase::GetPteIndex(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return m_pfn[Page].Busy.PteIndex;
}

ULONG CPfnDatabase::GetPfnDatabaseSize() const
{
    return m_NumberOfPages;
}

ULONG CPfnDatabase::GetBusyType(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return m_pfn[Page].Busy.BusyType;   
}

ULONG CPfnDatabase::GetLockCount(ULONG Page) const
{
    _ASSERTE(Page < m_NumberOfPages);
    return m_pfn[Page].Busy.LockCount >> 1;
}
