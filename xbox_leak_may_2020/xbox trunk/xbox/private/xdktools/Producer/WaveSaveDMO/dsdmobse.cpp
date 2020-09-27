/*
 * DirectSound DirectMediaObject base classes 
 *
 * Copyright (c) 1999-2000 Microsoft Corporation.  All Rights Reserved.  
 */

#include "DsDmoBse.h"

// XXX C1in1out calls InitializeCriticalSection in a constructor with
// no handler.
//

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::CDirectSoundDMO
//
CDirectSoundDMO::CDirectSoundDMO()
{
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::~CDirectSoundDMO
//
CDirectSoundDMO::~CDirectSoundDMO() 
{
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::GetClassID
//
// This should always return E_NOTIMPL
//
STDMETHODIMP CDirectSoundDMO::GetClassID(THIS_ CLSID *pClassID)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::IsDirty
//
// Override if doing something other than just standard save.
//
STDMETHODIMP CDirectSoundDMO::IsDirty(THIS)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::Load
//
// Override if doing something other than just standard load.
//
STDMETHODIMP CDirectSoundDMO::Load(THIS_ IStream *pStm) 
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::Save
//
// Override if doing something other than just standard save.
//
STDMETHODIMP CDirectSoundDMO::Save(THIS_ IStream *pStm, BOOL fClearDirty)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::GetSizeMax
//
// Override if doing something other than just standard save.
//
STDMETHODIMP CDirectSoundDMO::GetSizeMax(THIS_ ULARGE_INTEGER *pcbSize)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::Process
//
STDMETHODIMP CDirectSoundDMO::Process(THIS_ ULONG ulSize, BYTE *pData, REFERENCE_TIME rtStart, DWORD dwFlags)
{
    DMO_MEDIA_TYPE *pmt = InputType();
    if (pmt == NULL)
        return E_FAIL;

    assert(pmt->formattype == FORMAT_WaveFormatEx);
    ulSize /= LPWAVEFORMATEX(pmt->pbFormat)->nBlockAlign;
    return ProcessInPlace(ulSize, pData, rtStart, dwFlags);
}

//////////////////////////////////////////////////////////////////////////////
//
// CDirectSoundDMO::GetLatency
//
STDMETHODIMP CDirectSoundDMO::GetLatency(THIS_ REFERENCE_TIME *prt)
{
    *prt = 0;
    return S_OK;
}
