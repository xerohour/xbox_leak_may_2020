/***************************************************************************
 *
 *  Copyright (C) 12/10/2001 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       waveldr.cpp
 *  Content:    Wave file XMO wrappers.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  12/10/2001   dereks  Created.
 *
 ****************************************************************************/

#include "dsoundi.h"


/****************************************************************************
 *
 *  CWaveFileMediaObject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFileMediaObject::CWaveFileMediaObject"

CWaveFileMediaObject::CWaveFileMediaObject
(
    void
)
{
    DPF_ENTER();

    //
    // Initialize defaults
    //
    
    m_dwReadOffset = 0;

    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CWaveFileMediaObject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFileMediaObject::~CWaveFileMediaObject"

CWaveFileMediaObject::~CWaveFileMediaObject
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info structure.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFileMediaObject::GetInfo"

HRESULT
CWaveFileMediaObject::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();

    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
    pInfo->dwMaxLookahead = 0;
    pInfo->dwInputSize = 0;
    pInfo->dwOutputSize = m_pwfxFormat->nBlockAlign;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Submits buffers to the stream.
 *
 *  Arguments:
 *      LPCXMEDIAPACKET  [in]: input buffer.
 *      LPCXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFileMediaObject::Process"

HRESULT
CWaveFileMediaObject::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    LPBYTE                  pbDst;
    DWORD                   cbDst;
    HRESULT                 hr;

    DPF_ENTER();

    ASSERT(m_pwfxFormat);
    
    //
    // We're an output-only XMO, so we only use pxmbDest.  We'll just assume
    // that it's valid and pxmbSource isn't.
    //

    ASSERT(!pxmbSource);
    ASSERT(pxmbDest);
    ASSERT(pxmbDest->pvBuffer);
    ASSERT(pxmbDest->dwMaxSize);

    pbDst = (LPBYTE)pxmbDest->pvBuffer;
    cbDst = pxmbDest->dwMaxSize;

    //
    // Block-align the size
    //

    cbDst /= m_pwfxFormat->nBlockAlign;
    cbDst *= m_pwfxFormat->nBlockAlign;

    //
    // Initialize packet output parameters
    //

    XMOAcceptPacket(pxmbDest);

    //
    // Read from the file
    //

    hr = CWaveFile::ReadSample(m_dwReadOffset, pbDst, cbDst, &cbDst);

    //
    // Update the read offset
    //

    if(SUCCEEDED(hr))
    {
        m_dwReadOffset += cbDst;
    }

    //
    // Complete the packet
    //

    if(SUCCEEDED(hr))
    {
        XMOCompletePacket(pxmbDest, cbDst);
    }
    else
    {
        XMOCompletePacket(pxmbDest, 0, NULL, NULL, XMEDIAPACKET_STATUS_FAILURE);
    }

    DPF_LEAVE_HRESULT(hr);

    return hr;
}


/***************************************************************************
 *
 *  Seek
 *
 *  Description:
 *      Sets the current read or write position in the stream.
 *
 *  Arguments:
 *      LONG [in]: relative offset.
 *      DWORD [in]: offset origin.
 *      LPDWORD [out]: absolute stream position.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ***************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveFileMediaObject::Seek"

HRESULT
CWaveFileMediaObject::Seek
(
    LONG                    lOffset,
    DWORD                   dwOrigin,
    LPDWORD                 pdwAbsolute
)
{
    DPF_ENTER();
    
    switch(dwOrigin)
    {
        case FILE_BEGIN:
            m_dwReadOffset = 0;
            break;

        case FILE_END:
            m_dwReadOffset = m_DataChunk.GetDataSize();
            break;
    }

    m_dwReadOffset += lOffset;

    if(pdwAbsolute)
    {
        *pdwAbsolute = m_dwReadOffset;
    }

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  CFileMediaObject
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFileMediaObject::CFileMediaObject"

CFileMediaObject::CFileMediaObject
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  ~CFileMediaObject
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFileMediaObject::~CFileMediaObject"

CFileMediaObject::~CFileMediaObject
(
    void
)
{
    DPF_ENTER();
    DPF_LEAVE_VOID();
}


/****************************************************************************
 *
 *  GetInfo
 *
 *  Description:
 *      Gets information about the data the object supports.
 *
 *  Arguments:
 *      LPXMEDIAINFO [in/out]: info structure.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFileMediaObject::GetInfo"

HRESULT
CFileMediaObject::GetInfo
(
    LPXMEDIAINFO            pInfo
)
{
    DPF_ENTER();

    ASSERT(pInfo);
    
    pInfo->dwFlags = XMO_STREAMF_FIXED_SAMPLE_SIZE;
    pInfo->dwMaxLookahead = 0;
    pInfo->dwInputSize = 1;
    pInfo->dwOutputSize = 1;

    DPF_LEAVE_HRESULT(DS_OK);

    return DS_OK;
}


/****************************************************************************
 *
 *  Process
 *
 *  Description:
 *      Submits buffers to the stream.
 *
 *  Arguments:
 *      LPCXMEDIAPACKET  [in]: input buffer.
 *      LPCXMEDIAPACKET  [in]: output buffer.
 *
 *  Returns:  
 *      HRESULT: COM result code.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CFileMediaObject::Process"

HRESULT
CFileMediaObject::Process
(
    LPCXMEDIAPACKET         pxmbSource, 
    LPCXMEDIAPACKET         pxmbDest 
)
{
    HRESULT                 hr      = E_FAIL;
    LPBYTE                  pbSrc;
    DWORD                   cbSrc;
    LPBYTE                  pbDst;
    DWORD                   cbDst;

    DPF_ENTER();

    ASSERT(pxmbSource || pxmbDest);
    ASSERT(!(pxmbSource && pxmbDest));
    
    if(pxmbSource)
    {
        ASSERT(pxmbSource->pvBuffer);
        ASSERT(pxmbSource->dwMaxSize);

        pbSrc = (LPBYTE)pxmbSource->pvBuffer;
        cbSrc = pxmbSource->dwMaxSize;

        XMOAcceptPacket(pxmbSource);

        hr = Write(pbSrc, cbSrc, &cbSrc);

        if(SUCCEEDED(hr))
        {
            XMOCompletePacket(pxmbSource, cbSrc);
        }
        else
        {
            XMOCompletePacket(pxmbSource, 0, NULL, NULL, XMEDIAPACKET_STATUS_FAILURE);
        }
    }
    else if(pxmbDest)
    {
        ASSERT(pxmbDest->pvBuffer);
        ASSERT(pxmbDest->dwMaxSize);

        pbDst = (LPBYTE)pxmbDest->pvBuffer;
        cbDst = pxmbDest->dwMaxSize;

        XMOAcceptPacket(pxmbDest);

        hr = Read(pbDst, cbDst, &cbDst);
        
        if(SUCCEEDED(hr))
        {
            XMOCompletePacket(pxmbDest, cbDst);
        }
        else
        {
            XMOCompletePacket(pxmbDest, 0, NULL, NULL, XMEDIAPACKET_STATUS_FAILURE);
        }
    }
    
    DPF_LEAVE_HRESULT(hr);

    return hr;
}


