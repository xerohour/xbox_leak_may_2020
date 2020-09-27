// vad.cpp: implementation of the CVpn class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "vad.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CVadArray::CVadArray() :
    m_arraySize(0),
    m_numberOfVad(0),
    m_vadArray(NULL)
{
}

CVadArray::~CVadArray()
{
    if (m_vadArray) {
        _ASSERTE(LocalSize(m_vadArray) == sizeof(MMVAD) * m_arraySize);
        LocalFree(m_vadArray);
    }
}

bool CVadArray::GrowBy(size_t elements)
{
    PMMVAD vadArray;
    size_t NewSize;

    NewSize = (m_arraySize + elements) * sizeof(MMVAD);

    if (m_vadArray) {
        vadArray = (PMMVAD)LocalReAlloc(m_vadArray, NewSize, LPTR);
    } else {
        vadArray = (PMMVAD)LocalAlloc(LPTR, NewSize);
    }

    if (!vadArray) {
        return false;
    }

    m_vadArray = vadArray;
    m_arraySize = m_arraySize + elements;

    return true;
}

bool CVadArray::Add(const MMVAD* vad)
{
    if (m_numberOfVad == m_arraySize && !GrowBy(256)) {
        return false;
    }

    m_vadArray[m_numberOfVad] = *vad;
    m_numberOfVad++;

    return true;
}

ULONG CVadArray::GetStartingVpn(size_t element) const
{
    _ASSERTE(element < m_numberOfVad);
    return m_vadArray[element].StartingVpn;
}

ULONG CVadArray::GetEndingVpn(size_t element) const
{
    _ASSERTE(element < m_numberOfVad);
    return m_vadArray[element].EndingVpn;
}

ULONG CVadArray::GetAllocationProtecion(size_t element) const
{
    _ASSERTE(element < m_numberOfVad);
    return m_vadArray[element].AllocationProtect;
}

void CVadArray::ResetContent()
{
    m_numberOfVad = 0;
}

size_t CVadArray::GetArraySize() const
{
    return m_numberOfVad;
}
