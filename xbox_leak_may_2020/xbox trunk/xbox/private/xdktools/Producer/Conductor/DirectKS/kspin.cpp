// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft Corporation, 1996 - 2000.  All rights reserved.
//
// File Name:
//	kspin.cpp
//
// Abstract:
//  Class implementation of CKsPin.
//
// -------------------------------------------------------------------------------

#include "stdafx.h"
#include "kslibp.h"

// ------------------------------------------------------------------------
// CKsPin::CKsPin
//      copy constructor
// ------------------------------------------------------------------------
CKsPin::CKsPin
(
    CKsFilter*  pFilter,
    CKsPin*     pkspCopy
) : CKsIrpTarget(INVALID_HANDLE_VALUE),
    m_pFilter(pFilter),
    m_pksPinCreate(NULL),
    m_pksDataFormat(NULL),
    m_pbStreamData(NULL),
    m_cbStreamData(0L),
    m_dwAlignment(0),
    m_cFramesUsed(1)
{
    if (pkspCopy)
    {
        m_nId = pkspCopy->m_nId;
        m_ksState = pkspCopy->m_ksState;         
        m_nStackwiseFlow = pkspCopy->m_nStackwiseFlow;
        CopyMemory(&m_Descriptor, &pkspCopy->m_Descriptor, sizeof(PIN_DESCRIPTOR));
    }

    ZeroMemory(m_ov, 10 * sizeof(OVERLAPPED));
    ZeroMemory(m_ksStreamHeader, 10 * sizeof(KSSTREAM_HEADER));

    // create IRP-completion event
    for (int i = 0; i < 10; i++)
        m_ov[i].hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    m_szFriendlyName[0] = 0;
    m_UserDef = NULL;
}

// ------------------------------------------------------------------------
// CKsPin::CKsPin
//      constructor
// ------------------------------------------------------------------------
CKsPin::CKsPin
(
    CKsFilter*  pFilter,
    ULONG       nId
) : CKsIrpTarget(INVALID_HANDLE_VALUE),
    m_pFilter(pFilter),
    m_nId(nId),
    m_ksState(KSSTATE_STOP),
    m_pksPinCreate(NULL),
    m_pksDataFormat(NULL),
    m_pbStreamData(NULL),
    m_cbStreamData(0L),
    m_nStackwiseFlow(render),
    m_dwAlignment(0),
    m_cFramesUsed(1)
{
    ZeroMemory(&m_Descriptor, sizeof(PIN_DESCRIPTOR));

    ZeroMemory(m_ov, 10 * sizeof(OVERLAPPED));
    ZeroMemory(m_ksStreamHeader, 10 * sizeof(KSSTREAM_HEADER));

    // create IRP-completion event
    for (int i = 0; i < 10; i++)
        m_ov[i].hEvent = CreateEvent(NULL, TRUE, TRUE, NULL);

    m_szFriendlyName[0] = 0;
    m_UserDef = NULL;
}

// ------------------------------------------------------------------------
// CKsPin::~CKsPin
// ------------------------------------------------------------------------
CKsPin::~CKsPin
(
    void
)
{
    ClosePin();
    for (int i = 0; i < 10; i++)
        SafeCloseHandle(m_ov[i].hEvent);

    SafeLocalFree(m_pksPinCreate);
    m_pbStreamData -= m_dwAlignment;
    SafeLocalFree(m_pbStreamData);
}

LPCSTR gpstrState[] = { "KSSTATE_STOP", "KSSTATE_ACQUIRE", "KSSTATE_PAUSE", "KSSTATE_RUN" };
LPCSTR gpstrPinType[] = { "render", "capture" };

// ----------------------------------------------------------------------------------
// CKsPin::Instantiate
//  instantiates m_handle
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::Instantiate
(
    BOOL    fLooped
)
{
    if (!m_pksPinCreate)
        return FALSE;

    HRESULT hRes;

    m_fLooped = fLooped;
    m_pksPinCreate->Interface.Id = 
        m_fLooped ? KSINTERFACE_STANDARD_LOOPED_STREAMING : KSINTERFACE_STANDARD_STREAMING;

    if (fxnKsCreatePin)
    {
        hRes =
            fxnKsCreatePin
            (
                m_pFilter->m_handle,
                m_pksPinCreate,
                GENERIC_WRITE | GENERIC_READ,
                &m_handle
            );
    }
    else
    {
        LOG(eWarn1, "CKsPin::Instantiate.  Error:  fxnKsCreatePin = NULL");
        return FALSE;
    }

    if (hRes == 0)
    {
        LOG(eInfo2, "Successfully instantiated %s pin", m_nStackwiseFlow == render ? "Render" : "Capture");
        return TRUE;
    }
    else
    {
        DWORD dwLastError = GetLastError();
        
        LOG(eWarn2, "CKsPin::Instantiate.  Failed to instantiate %s pin!\nKsCreatePin returned = 0x%08x", m_nStackwiseFlow == render ? "Render" : "Capture", hRes);
        s_iLog->LogStdErrorString(dwLastError);
        return FALSE;
    }
}

// ----------------------------------------------------------------------------------
// CKsPin::ClosePin
//  closes m_handle
// ----------------------------------------------------------------------------------
void
CKsPin::ClosePin
(
    void
)
{
    SetState(KSSTATE_STOP);
    //LOG(eBlab2, "Closing %s pin", gpstrPinType[(UINT)m_nStackwiseFlow]);
    m_pbStreamData -= m_dwAlignment;
    m_dwAlignment = 0; // Ensure that we do not use the same alignment in Destructor.
    SafeLocalFree(m_pbStreamData);
    SafeCloseHandle(m_handle);
}

// ----------------------------------------------------------------------------------
// CKsPin::Wait
//  waits for m_ov[nFrame].hEvent
// ----------------------------------------------------------------------------------
BOOL
CKsPin::Wait
(
    DWORD   dwMilliseconds,
    ULONG   nFrame
)
{
    if (!IsValidHandle(m_ov[nFrame].hEvent))
        return FALSE;

    DWORD   dwWait = WaitForSingleObject(m_ov[nFrame].hEvent, dwMilliseconds);
    BOOL    fRes = (WAIT_OBJECT_0 == dwWait);

    if (!fRes && s_iLog)  LOG(eWarn2, "IRP completion timed out...");

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsPin::Wait
//  waits for all m_ov[n].hEvent
// ----------------------------------------------------------------------------------
BOOL
CKsPin::Wait
(
    DWORD   dwMilliseconds
)
{
    ULONG   nFrame;
    for (nFrame = 0; nFrame < m_cFramesUsed; nFrame++)
        if (! Wait(dwMilliseconds, nFrame))
            return FALSE;

    return TRUE;
}

// ----------------------------------------------------------------------------------
// CKsPin::IsSignaled
//  check state of m_ov[nFrame].hEvent
// ----------------------------------------------------------------------------------
BOOL
CKsPin::IsSignaled
(
    ULONG   nFrame
)
{
    if (!IsValidHandle(m_ov[nFrame].hEvent))
        return FALSE;

    return (WAIT_OBJECT_0 == WaitForSingleObject(m_ov[nFrame].hEvent, 0));
}

// ----------------------------------------------------------------------------------
// CKsPin::IsSignaled
//  check state of all m_ov[n].hEvent
// ----------------------------------------------------------------------------------
BOOL
CKsPin::IsSignaled
(
    void
)
{
    ULONG   nFrame;
    for (nFrame = 0; nFrame < m_cFramesUsed; nFrame++)
        if (! IsSignaled(nFrame))
            return FALSE;

    return TRUE;
}

// ----------------------------------------------------------------------------------
// CKsPin::GetOverlapped
//  Returns the overlapped structure. transparent to others.
// ----------------------------------------------------------------------------------
OVERLAPPED 
&CKsPin::GetOverlapped
(
    UINT unIndex /* = 0 */
)
{
    return m_ov[unIndex];
} // GetOverlapped

// ----------------------------------------------------------------------------------
// CKsPin::GetOverlapped
//  Returns the stream header structure. transparent to others.
// ----------------------------------------------------------------------------------
KSSTREAM_HEADER 
&CKsPin::GetStreamHeader
(
    UINT unIndex /* = 0 */
)
{
    return m_ksStreamHeader[unIndex];
} // GetStreamHeader

// ----------------------------------------------------------------------------------
// CKsPin::SetState
//  set KSSTATE of pin
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::GetState
(
    PKSSTATE pksState
)
{
    KSPROPERTY    Property;
    ULONG         cbReturned;
    BOOL          fRes;
    KSSTATE       ksState;

    fRes =
        (m_nStackwiseFlow == 0 || m_nStackwiseFlow == 1) &&
        (pksState) &&
        (IsValidHandle(m_handle));

    if (!fRes)
        return FALSE;

    Property.Set = KSPROPSETID_Connection;
    Property.Id = KSPROPERTY_CONNECTION_STATE;
    Property.Flags = KSPROPERTY_TYPE_GET;

    fRes = 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &Property,
            sizeof(KSPROPERTY),
            &ksState,
            sizeof(KSSTATE),
            &cbReturned 
        );

    if (fRes)
    {
        CopyMemory(pksState, &ksState, sizeof(KSSTATE));
    }

    return fRes;
} // GetState

// ----------------------------------------------------------------------------------
// CKsPin::SetState
//  set KSSTATE of pin
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::SetState
(
    KSSTATE ksState
)
{
    KSPROPERTY    Property;
    ULONG         cbReturned;
    BOOL          fRes;

    fRes =
        (m_nStackwiseFlow == 0 || m_nStackwiseFlow == 1) &&
        (ksState == KSSTATE_STOP || ksState == KSSTATE_RUN || ksState == KSSTATE_PAUSE || ksState == KSSTATE_ACQUIRE) &&
        (IsValidHandle(m_handle));

    if (!fRes)
        return FALSE;

    LOG(eInfo3, "Setting %s pin to state %s", gpstrPinType[(UINT)m_nStackwiseFlow], gpstrState[(UINT)ksState]);

    m_ksState = ksState;

    Property.Set = KSPROPSETID_Connection;
    Property.Id = KSPROPERTY_CONNECTION_STATE;
    Property.Flags = KSPROPERTY_TYPE_SET;

    fRes = 
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_PROPERTY,
            &Property,
            sizeof(KSPROPERTY),
            &ksState,
            sizeof(KSSTATE),
            &cbReturned 
        );

    if (!fRes && s_iLog)  
        LOG(eWarn2, "Failed to set %s pin to state %s", gpstrPinType[(UINT)m_nStackwiseFlow], gpstrState[(UINT)ksState]);

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsPin::Reset
//  dump some data (namely m_pbStreamData) on the pin
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::Reset
(
    void
)
{
    ULONG   ulIn;
    ULONG   ulBytesReturned;
    BOOL    fResult;
        
    LOG(eInfo3, "Resetting Pin state: IOCTL_KS_RESET_STATE (KSRESET_BEGIN)");
    ulIn = KSRESET_BEGIN;
    fResult =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_RESET_STATE,
            &ulIn,
            sizeof(ULONG),
            NULL,
            0,
            &ulBytesReturned
        );
    if (!fResult)
    {
        LOG(eWarn2, "IOCTL_KS_RESET_STATE failed");
        s_iLog->LogStdErrorString(GetLastError());
    }

    LOG(eInfo3, "Resetting Pin state: IOCTL_KS_RESET_STATE (KSRESET_END)");
    ulIn = KSRESET_END;
    fResult =
        SyncIoctl
        (
            m_handle,
            IOCTL_KS_RESET_STATE,
            &ulIn,
            sizeof(ULONG),
            NULL,
            0,
            &ulBytesReturned
        );
    if (!fResult)
    {
        LOG(eWarn2, "IOCTL_KS_RESET_STATE failed");
        s_iLog->LogStdErrorString(GetLastError());
    }

    return fResult;
}

// ----------------------------------------------------------------------------------
// CKsPin::WriteData
//  dump some data (namely m_pbStreamData) on the pin using only the nFrame-th frame
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::WriteData
(
    ULONG   nFrame,
	BOOL	fPause
)
{
    DWORD   dwError;
    DWORD   cbReturned;
    BOOL    fRes = TRUE;

    ZeroMemory(&m_ksStreamHeader[nFrame], sizeof(KSSTREAM_HEADER));
    m_ksStreamHeader[nFrame].Size = sizeof(KSSTREAM_HEADER);
    m_ksStreamHeader[nFrame].Data = m_pbStreamData;
    m_ksStreamHeader[nFrame].FrameExtent = m_cbStreamData;
    m_ksStreamHeader[nFrame].DataUsed = m_cbStreamData;
    m_ksStreamHeader[nFrame].PresentationTime.Time          = 0;
    m_ksStreamHeader[nFrame].PresentationTime.Numerator     = 1;
    m_ksStreamHeader[nFrame].PresentationTime.Denominator   = 1;
    m_ksStreamHeader[nFrame].OptionsFlags = m_fLooped ? KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA : 0;

	if( fPause )
	{
		fRes = SetState(KSSTATE_PAUSE);
	}

    if (fRes)
    {
        // submit the data
        LOG(eInfo3, "Submitting IOCTL_KS_WRITE_STREAM IRP ...");
        ResetEvent(m_ov[nFrame].hEvent);
        fRes = 
            DeviceIoControl
            ( 
                m_handle,
                IOCTL_KS_WRITE_STREAM,
                NULL,
                0,
                &m_ksStreamHeader[nFrame],
                sizeof(KSSTREAM_HEADER),
                &cbReturned,
                &m_ov[nFrame]
            );
    }

    // Since we are paused, this should return FALSE
    if (fRes)
    {
        LOG(eWarn2, "DeviceIoControl returned TRUE even though the pin is paused\n");
    }
    else
    {
        // if it did return FALSE, then GetLastError should return ERROR_IO_PENDING
        dwError = GetLastError();
        if (ERROR_IO_PENDING == dwError)
        {
            LOG(eBlab2, "IRP is pending...");
            fRes = TRUE;
        }
        else
        {
            LOG(eWarn2, "GetLastError did not return ERROR_IO_PENDING even though the pin is paused\n");
            s_iLog->LogStdErrorString(dwError);
        }
    }

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsPin::WriteData
//  dump some data (namely m_pbStreamData) on the pin.  Use all frames
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::WriteData
(
    void
)
{
    BOOL    fRes = TRUE;
    ULONG   nFrame;

    for (nFrame = 0; nFrame < m_cFramesUsed; nFrame++)
        fRes == fRes && WriteData(nFrame, TRUE);

    return fRes;
}

// ----------------------------------------------------------------------------------
// CKsPin::ReadData
//  read some data (into m_pbStreamData) from the pin
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::ReadData
(
    ULONG   nFrame
)
{
    DWORD   dwError;
    DWORD   cbReturned;
    BOOL    fRes = TRUE;

    ZeroMemory(&m_ksStreamHeader[nFrame], sizeof(KSSTREAM_HEADER));
    m_ksStreamHeader[nFrame].Size = sizeof(KSSTREAM_HEADER);
    m_ksStreamHeader[nFrame].Data = m_pbStreamData;
    m_ksStreamHeader[nFrame].FrameExtent = CalculateFrameSize(nFrame, m_cFramesUsed, m_cbStreamData);
    m_ksStreamHeader[nFrame].DataUsed = 0;
    m_ksStreamHeader[nFrame].PresentationTime.Time          = 0;
    m_ksStreamHeader[nFrame].PresentationTime.Numerator     = 1;
    m_ksStreamHeader[nFrame].PresentationTime.Denominator   = 1;
    m_ksStreamHeader[nFrame].OptionsFlags = m_fLooped ? KSSTREAM_HEADER_OPTIONSF_LOOPEDDATA : 0;

    fRes = SetState(KSSTATE_PAUSE);

    if (fRes)
    {
        // submit the data
        LOG(eInfo3, "Submitting IOCTL_KS_WRITE_STREAM IRP ...");
        ResetEvent(m_ov[nFrame].hEvent);
        fRes = 
            DeviceIoControl
            ( 
                m_handle,
                IOCTL_KS_READ_STREAM,
                NULL,
                0,
                &m_ksStreamHeader[nFrame],
                sizeof(KSSTREAM_HEADER),
                &cbReturned,
                &m_ov[nFrame]
            );
    }

    // Since we are paused, this should return FALSE
    if (fRes)
    {
        LOG(eWarn2, "Warning: DeviceIoControl returned TRUE even though the pin is paused\n");
    }
    else
    {
        // if it did return FALSE, then GetLastError should return ERROR_IO_PENDING
        dwError = GetLastError();
        if (ERROR_IO_PENDING == dwError)
        {
            LOG(eBlab2, "IRP is pending...");
            fRes = TRUE;
        }
        else
        {
            LOG(eWarn2, "Warning: GetLastError did not return ERROR_IO_PENDING even though the pin is paused\n");
            s_iLog->LogStdErrorString(dwError);
        }
    }

    if (fRes)
    {
        ASSERT(WAIT_TIMEOUT == WaitForSingleObject(m_ov[nFrame].hEvent, 0));
    }

    return fRes;
}


// ----------------------------------------------------------------------------------
// CKsPin::ReadData
//  dump some data (namely m_pbStreamData) on the pin.  Use all frames
// ----------------------------------------------------------------------------------
BOOL    
CKsPin::ReadData
(
    void
)
{
    BOOL    fRes = TRUE;
    ULONG   nFrame;

    for (nFrame = 0; nFrame < m_cFramesUsed; nFrame++)
        fRes == fRes && ReadData(nFrame);

    return fRes;
}

