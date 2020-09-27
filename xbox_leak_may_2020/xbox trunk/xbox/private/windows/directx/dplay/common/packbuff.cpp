/*==========================================================================
 *
 *  Copyright (C) 1998-2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       PackBuff.cpp
 *  Content:	Packed Buffers
 *
 *
 *  History:
 *   Date		By		Reason
 *   ====		==		======
 *	11/01/00	mjn		Created
 *  06/15/2000  rmt     Added func to add string to packbuffer
 ***************************************************************************/

#include "commoni.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_COMMON

//**********************************************************************
// Constant definitions
//**********************************************************************

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

//**********************************************************************
// Variable definitions
//**********************************************************************

//**********************************************************************
// Function prototypes
//**********************************************************************

//**********************************************************************
// Function definitions
//**********************************************************************


//**********************************************************************
// ------------------------------
// CPackedBuffer::Initialize
//
// Entry:		void *const	pvBuffer		- Buffer to fill up (may be NULL)
//				const DWORD	dwBufferSize	- Size of buffer (may be 0)
//
// Exit:		Nothing
// ------------------------------

#undef DPF_MODNAME
#define DPF_MODNAME "CPackedBuffer::Initialize"
void CPackedBuffer::Initialize(void *const pvBuffer,
								  const DWORD dwBufferSize)
{
	if (pvBuffer == NULL || dwBufferSize == 0)
	{
		m_pStart = NULL;
		m_pHead = NULL;
		m_pTail = NULL;
		m_dwRemaining = 0;
		m_bBufferTooSmall = TRUE;
	}
	else
	{
		m_pStart = reinterpret_cast<BYTE*>(pvBuffer);
		m_pHead = m_pStart;
		m_pTail = m_pStart + dwBufferSize;
		m_dwRemaining = dwBufferSize;
		m_bBufferTooSmall = FALSE;
	}
	m_dwRequired = 0;
}


//**********************************************************************
// ------------------------------
// CPackedBuffer::AddToFront
//
// Entry:		void *const	pvBuffer		- Buffer to add (may be NULL)
//				const DWORD	dwBufferSize	- Size of buffer (may be 0)
//
// Exit:		Error Code:	DPN_OK					if able to add
//							DPNERR_BUFFERTOOSMALL	if buffer is full
// ------------------------------

#undef DPF_MODNAME
#define DPF_MODNAME "CPackedBuffer::AddToFront"
HRESULT CPackedBuffer::AddToFront(const void *const pvBuffer,
								  const DWORD dwBufferSize)
{
	m_dwRequired += dwBufferSize;
	if (!m_bBufferTooSmall)
	{
		if (m_dwRemaining >= dwBufferSize)
		{
			if (pvBuffer)
			{
				memcpy(m_pHead,pvBuffer,dwBufferSize);
			}
			m_pHead += dwBufferSize;
			m_dwRemaining -= dwBufferSize;
		}
		else
		{
			m_bBufferTooSmall = TRUE;
		}
	}

	if (m_bBufferTooSmall)
		return(DPNERR_BUFFERTOOSMALL);

	return(DPN_OK);
}

//**********************************************************************
// ------------------------------
// CPackedBuffer::AddWCHARStringToBack
//
// Entry:		const wchar_t * const pwszString - String to add (may be NULL)
//
// Exit:		Error Code:	DPN_OK					if able to add
//							DPNERR_BUFFERTOOSMALL	if buffer is full
// ------------------------------

#undef DPF_MODNAME
#define DPF_MODNAME "CPackedBuffer::AddWCHARStringToBack"
HRESULT CPackedBuffer::AddWCHARStringToBack( const wchar_t * const pwszString )
{
    return AddToBack( pwszString, sizeof( wchar_t ) * (wcslen( pwszString )+1) );
}

//**********************************************************************
// ------------------------------
// CPackedBuffer::AddToBack
//
// Entry:		void *const	pvBuffer		- Buffer to add (may be NULL)
//				const DWORD	dwBufferSize	- Size of buffer (may be 0)
//
// Exit:		Error Code:	DPN_OK					if able to add
//							DPNERR_BUFFERTOOSMALL	if buffer is full
// ------------------------------

#undef DPF_MODNAME
#define DPF_MODNAME "CPackedBuffer::AddToBack"
HRESULT CPackedBuffer::AddToBack(const void *const pvBuffer,
								 const DWORD dwBufferSize)
{
	m_dwRequired += dwBufferSize;
	if (!m_bBufferTooSmall)
	{
		if (m_dwRemaining >= dwBufferSize)
		{
			m_pTail -= dwBufferSize;
			m_dwRemaining -= dwBufferSize;
			if (pvBuffer)
			{
				memcpy(m_pTail,pvBuffer,dwBufferSize);
			}
		}
		else
		{
			m_bBufferTooSmall = TRUE;
		}
	}

	if (m_bBufferTooSmall)
		return(DPNERR_BUFFERTOOSMALL);

	return(DPN_OK);
}

