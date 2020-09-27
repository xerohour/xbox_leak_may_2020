/*
 * DirectSound DirectMediaObject base classes 
 *
 * Copyright (c) 1999 Microsoft Corporation.  All Rights Reserved.  
 */
#ifndef _DsDmoBase_
#define _DsDmoBase_

#define DMO_NOATL

#include <objbase.h>
#include <dmobase.h>
#include <medparam.h>

#ifndef RELEASE
#define RELEASE(x) { if (x) (x)->Release(); x = NULL; }
#endif

class CDirectSoundDMO :
      public CPCMDMO,
      public IPersistStream,
      public IMediaObjectInPlace
{
public:
    CDirectSoundDMO();
    virtual ~CDirectSoundDMO();

    /* IPersist */
    STDMETHODIMP GetClassID                 (THIS_ CLSID *pClassID);
    
    /* IPersistStream */
    STDMETHODIMP IsDirty                    (THIS);
    STDMETHODIMP Load                       (THIS_ IStream *pStm); 
    STDMETHODIMP Save                       (THIS_ IStream *pStm, BOOL fClearDirty);
    STDMETHODIMP GetSizeMax                 (THIS_ ULARGE_INTEGER *pcbSize);

    /* IMediaObjectInPlace */
    STDMETHODIMP Process                    (THIS_ ULONG ulSize, BYTE *pData, REFERENCE_TIME rtStart, DWORD dwFlags);
    STDMETHODIMP GetLatency                 (THIS_ REFERENCE_TIME *prt);

protected:
    // Process in place
    //
    virtual HRESULT ProcessInPlace(ULONG ulQuanta, LPBYTE pcbData, REFERENCE_TIME rtStart, DWORD dwFlags) = 0;
};

#endif
