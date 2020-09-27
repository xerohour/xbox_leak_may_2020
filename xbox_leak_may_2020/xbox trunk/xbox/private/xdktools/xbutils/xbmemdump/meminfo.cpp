// meminfo.cpp: implementation of the CXboxMemoryInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "meminfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CXboxMemoryInfo::CXboxMemoryInfo() :
    m_HighestPhysicalPage(0),
    m_PfnDatabaseAddr(MM_PFN_DATABASE),
    m_MmNumberOfPhysicalPages(0),
    m_MmAvailablePages(0)
{
    ZeroMemory(&m_MmGlobalData, sizeof(m_MmGlobalData));
    ZeroMemory(m_AllocatedPagesByUsage, 
        sizeof(m_AllocatedPagesByUsage));
}

CXboxMemoryInfo::~CXboxMemoryInfo()
{
}

const CPfnDatabase& CXboxMemoryInfo::GetPfnDatabase() const
{
    return m_pfn;
}

const CVadArray& CXboxMemoryInfo::GetVad() const
{
    return m_vad;
}

ULONG CXboxMemoryInfo::GetAllocatedPagesByUsage(ULONG Usage) const
{
    _ASSERTE(Usage < MmMaximumUsage);
    return m_AllocatedPagesByUsage[Usage];
}

ULONG CXboxMemoryInfo::GetAllocatedKBytesByUsage(ULONG Usage) const
{
    return GetAllocatedPagesByUsage(Usage) * 4096 / 1024;
}

ULONG CXboxMemoryInfo::GetTotalNumberOfPhysicalPages() const
{
    return m_MmNumberOfPhysicalPages;
}

ULONG CXboxMemoryInfo::GetAvailablePages() const
{
    return m_MmAvailablePages;
}

HRESULT CXboxMemoryInfo::DownloadMmGlobal()
{
    HRESULT hr;
    CHAR szResp[256];
    SIZE_T Size = sizeof(szResp);

    hr = SendCommand("mmglobal");

    if (hr != XBDM_MULTIRESPONSE) {
        return E_UNEXPECTED;
    }

    for (;;) {
        
        Size = sizeof(szResp);
        hr = ReceiveSocketLine(szResp, &Size);
        
        if (FAILED(hr) || szResp[0] == '.') {
            break;
        }

        GetParam(szResp, "MmHighestPhysicalPage", &m_HighestPhysicalPage);
        GetParam(szResp, "RetailPfnRegion", (PULONG)&m_MmGlobalData.RetailPfnRegion);
        GetParam(szResp, "SystemPteRange", (PULONG)&m_MmGlobalData.SystemPteRange);
        GetParam(szResp, "AvailablePages",
            (LPDWORD)&m_MmGlobalData.AvailablePages);
        GetParam(szResp, "AllocatedPagesByUsage",
            (LPDWORD)&m_MmGlobalData.AllocatedPagesByUsage);
        GetParam(szResp, "AddressSpaceLock",
            (LPDWORD)&m_MmGlobalData.AddressSpaceLock);
        GetParam(szResp, "VadRoot", (LPDWORD)&m_MmGlobalData.VadRoot);
        GetParam(szResp, "VadHint", (LPDWORD)&m_MmGlobalData.VadHint);
        GetParam(szResp, "VadFreeHint", (LPDWORD)&m_MmGlobalData.VadFreeHint);
        GetParam(szResp, "MmNumberOfPhysicalPages", (LPDWORD)&m_MmNumberOfPhysicalPages);
        GetParam(szResp, "MmAvailablePages", (LPDWORD)&m_MmAvailablePages);
        GetParam(szResp, "PfnDatabase", (LPDWORD)&m_PfnDatabaseAddr);
    }

    if (m_MmGlobalData.AllocatedPagesByUsage) {
        Size = sizeof(m_AllocatedPagesByUsage);
        hr = DmGetMemory(m_MmGlobalData.AllocatedPagesByUsage, Size,
                m_AllocatedPagesByUsage, &Size);
    }

    return hr;
}

HRESULT CXboxMemoryInfo::DownloadPfnDatabase()
{
    DWORD cbRead;
    PMMPFN PfnDatabaseLocal;
    SIZE_T SizeOfPfnDatabase;
    HRESULT hr;

    hr = UseSharedConnection();

    if (FAILED(hr)) {
        return hr;
    }

    if (!m_HighestPhysicalPage) {
        hr = DownloadMmGlobal();
        if (FAILED(hr)) {
            return hr;
        }
    }

    if (m_HighestPhysicalPage == 0) {
        return E_FAIL;
    }

    ULONG HighestPhysicalPage = m_HighestPhysicalPage + 1;

    SizeOfPfnDatabase = HighestPhysicalPage * sizeof(MMPFN);

    PfnDatabaseLocal = (PMMPFN)LocalAlloc(LPTR, SizeOfPfnDatabase);

    if (PfnDatabaseLocal == NULL) {
        return E_OUTOFMEMORY;
    }

#if 0
    hr = DmGetMemory(m_PfnDatabase, SizeOfPfnDatabase, PfnDatabaseLocal, &cbRead);
#else
    ULONG CurrentPage;
    ULONG EntriesToRead;

    EntriesToRead = 100;

    for (CurrentPage = 0; CurrentPage < HighestPhysicalPage; CurrentPage += EntriesToRead) {

        if ((HighestPhysicalPage - CurrentPage) < EntriesToRead) {
            EntriesToRead = HighestPhysicalPage - CurrentPage;
        }

        hr = DmGetMemory(&m_PfnDatabaseAddr[CurrentPage], 
                EntriesToRead * sizeof(MMPFN),
                &PfnDatabaseLocal[CurrentPage],
                &cbRead);

        if (FAILED(hr)) {
            break;
        }
    }
#endif

    if (SUCCEEDED(hr)) {
        if (!m_pfn.MakeCopy(PfnDatabaseLocal, HighestPhysicalPage)) {
            hr = E_OUTOFMEMORY;
        }
    }

    LocalFree(PfnDatabaseLocal);
    return hr;
}

HRESULT CXboxMemoryInfo::DownloadVad()
{
    HRESULT hr = S_OK;
    PMMADDRESS_NODE VadToDump;
    PMMADDRESS_NODE Next;
    PMMADDRESS_NODE Parent;
    PMMADDRESS_NODE First;
    PMMADDRESS_NODE Left;
    MMVAD CurrentVad;
    ULONG Done;
    ULONG Result;

    if (!m_MmGlobalData.VadRoot && FAILED(hr = DownloadMmGlobal())) {
        return hr;
    }

    //
    // Suspend all XTL threads so that VAD list is not changed.
    //

    if (FAILED(hr = DmStop())) {
        return hr;
    }

    m_vad.ResetContent();

    if (FAILED(hr = DmGetMemory(m_MmGlobalData.VadRoot, sizeof(PMMVAD), 
        &VadToDump, &Result))) {
        goto cleanup;
    }

    //VadToDump = (PMMADDRESS_NODE)m_MmGlobalData.VadRoot;

    First = VadToDump;

    if (First == NULL) {
        goto cleanup;
    }

    RtlZeroMemory(&CurrentVad, sizeof(MMVAD));

    if (FAILED(hr = DmGetMemory(First, sizeof(MMVAD), &CurrentVad, &Result))) {
        goto cleanup;
    }

    while (CurrentVad.LeftChild != NULL) {

        First = CurrentVad.LeftChild;

        if (FAILED(hr = DmGetMemory(First, sizeof(MMVAD), &CurrentVad, &Result))) {
            goto cleanup;
        }
    }

    m_vad.Add(&CurrentVad);

    Next = First;

    while (Next != NULL) {

        if (CurrentVad.RightChild == NULL) {

            Done = TRUE;

            while ((Parent = CurrentVad.Parent) != NULL) {

                //
                // Locate the first ancestor of this node of which this
                // node is the left child of and return that node as the
                // next element.
                //

                if (FAILED(hr = DmGetMemory(Parent, sizeof(MMVAD), 
                    &CurrentVad, &Result))) {
                    goto cleanup;
                }

                if (CurrentVad.LeftChild == Next) {
                    Next = Parent;
                    m_vad.Add(&CurrentVad);
                    Done = FALSE;
                    break;
                }
                Next = Parent;
            }
            if (Done) {
                Next = NULL;
                break;
            }
        } else {

            //
            // A right child exists, locate the left most child of that right child.
            //

            Next = CurrentVad.RightChild;

            if (FAILED(hr = DmGetMemory(Next, sizeof(MMVAD), &CurrentVad, 
                &Result))) {
                goto cleanup;
            }

            while ((Left = CurrentVad.LeftChild) != NULL) {
                Next = Left;
                if (FAILED(hr = DmGetMemory(Next, sizeof(MMVAD), &CurrentVad, 
                    &Result))) {
                    goto cleanup;
                }
            }

            m_vad.Add(&CurrentVad);
        }
    }

cleanup:

    DmGo();
    return hr;
}

bool CXboxMemoryInfo::IsValidHeap(PVOID Va) const
{
    HEAP Heap;
    ULONG Result;

    if (FAILED(DmGetMemory((PVOID)Va, sizeof(HEAP), &Heap, &Result))) {
        return false;
    }

    return Heap.Signature == HEAP_SIGNATURE;
}
