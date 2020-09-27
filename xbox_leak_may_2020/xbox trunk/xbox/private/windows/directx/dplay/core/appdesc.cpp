/*==========================================================================
 *
 *  Copyright (C) 2000 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       AppDesc.cpp
 *  Content:    Application Description Object
 *@@BEGIN_MSINTERNAL
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  09/02/00	mjn		Created
 *  09/14/2000	rmt		Bug #44625 - CORE: Multihomed machines are not always enumerable (extra spew)
 *@@END_MSINTERNAL
 *
 ***************************************************************************/

#include "dncorei.h"

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_CORE

//**********************************************************************
// Constant definitions
//**********************************************************************

#define UUID_TIME_HIGH_MASK    0x0FFF
#define UUID_VERSION           0x1000
#define RAND_UUID_VERSION      0x4000
#define UUID_RESERVED          0x80
#define UUID_CLOCK_SEQ_HI_MASK 0x3F

//**********************************************************************
// Macro definitions
//**********************************************************************

//**********************************************************************
// Structure definitions
//**********************************************************************

// This is the "true" OSF DCE format for Uuids.
//
typedef struct _UUID_GENERATE
{
    unsigned long  TimeLow;
    unsigned short TimeMid;
    unsigned short TimeHiAndVersion;
    unsigned char  ClockSeqHiAndReserved;
    unsigned char  ClockSeqLow;
    unsigned char  NodeId[6];
} UUID_GENERATE;


//**********************************************************************
// Variable definitions
//**********************************************************************

// Random seed -- remove when we pick a new random number generator
//
unsigned long g_ulSeed = 0;

//**********************************************************************
// Function prototypes
//**********************************************************************

HRESULT XUuidCreate( GUID *Uuid );
int NewGenRandom( unsigned char **ppbRandSeed, unsigned long *pcbRandSeed, unsigned char *pbBuffer, unsigned long dwLength );
unsigned long IRand( void );

//**********************************************************************
// Function definitions
//**********************************************************************


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDesc::Initialize"

HRESULT CApplicationDesc::Initialize( void )
{
	if (!DNInitializeCriticalSection(&m_cs))
	{
		return( DPNERR_OUTOFMEMORY );
	}

	m_dwFlags = 0;

	m_dwMaxPlayers = 0;
	m_dwCurrentPlayers = 0;

	m_pwszSessionName = NULL;
	m_dwSessionNameSize = 0;

	m_pwszPassword = NULL;
	m_dwPasswordSize = 0;

	m_pvReservedData = NULL;
	m_dwReservedDataSize = 0;

	m_pvApplicationReservedData = NULL;
	m_dwApplicationReservedDataSize = 0;

	memset(&m_guidInstance,0,sizeof(GUID));
	memset(&m_guidApplication,0,sizeof(GUID));

	return( DPN_OK );
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDesc::Deinitialize"

void CApplicationDesc::Deinitialize( void )
{
	if (m_pwszSessionName)
	{
		DNFree(m_pwszSessionName);
		m_pwszSessionName = NULL;
		m_dwSessionNameSize = 0;
	}

	if (m_pwszPassword)
	{
		DNFree(m_pwszPassword);
		m_pwszPassword = NULL;
		m_dwPasswordSize = 0;
	}

	if (m_pvReservedData)
	{
		DNFree(m_pvReservedData);
		m_pvReservedData = NULL;
		m_dwReservedDataSize = 0;
	}

	if (m_pvApplicationReservedData)
	{
		DNFree(m_pvApplicationReservedData);
		m_pvApplicationReservedData = NULL;
		m_dwApplicationReservedDataSize = 0;
	}

	DNDeleteCriticalSection(&m_cs);
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDesc::Pack"

HRESULT	CApplicationDesc::Pack(CPackedBuffer *const pPackedBuffer,
							   const DWORD dwFlags)
{
	HRESULT		hResultCode;
	DPN_APPLICATION_DESC	*pdpnAppDesc;

	DPF(6,"Parameters: pPackedBuffer [0x%p], dwFlags [0x%lx]",pPackedBuffer,dwFlags);

	Lock();

	//
	//	Add structure
	//
	pdpnAppDesc = reinterpret_cast<DPN_APPLICATION_DESC*>(pPackedBuffer->GetHeadAddress());
	hResultCode = pPackedBuffer->AddToFront(NULL,sizeof(DPN_APPLICATION_DESC));

	//
	//	Add session name
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME) && (m_dwSessionNameSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pwszSessionName,m_dwSessionNameSize);
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pwszSessionName = static_cast<WCHAR*>(pPackedBuffer->GetTailAddress());
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pwszSessionName = NULL;
		}
	}

	//
	//	Add password
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_PASSWORD) && (m_dwPasswordSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pwszPassword,m_dwPasswordSize);
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pwszPassword = static_cast<WCHAR*>(pPackedBuffer->GetTailAddress());
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pwszPassword = NULL;
		}
	}

	//
	//	Add reserved data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA) && (m_dwReservedDataSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pvReservedData,m_dwReservedDataSize);
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pvReservedData = pPackedBuffer->GetTailAddress();
			pdpnAppDesc->dwReservedDataSize = m_dwReservedDataSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pvReservedData = NULL;
			pdpnAppDesc->dwReservedDataSize = 0;
		}
	}

	//
	//	Add app reserved data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA) && (m_dwApplicationReservedDataSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pvApplicationReservedData,m_dwApplicationReservedDataSize);
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pvApplicationReservedData = pPackedBuffer->GetTailAddress();
			pdpnAppDesc->dwApplicationReservedDataSize = m_dwApplicationReservedDataSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pdpnAppDesc->pvApplicationReservedData = NULL;
			pdpnAppDesc->dwApplicationReservedDataSize = 0;
		}
	}

	//
	//	Fill in remainder of structure (if space)
	//
	if (hResultCode == DPN_OK)
	{
		pdpnAppDesc->dwSize = sizeof(DPN_APPLICATION_DESC);
		pdpnAppDesc->dwFlags = m_dwFlags;
		pdpnAppDesc->dwMaxPlayers = m_dwMaxPlayers;
		pdpnAppDesc->dwCurrentPlayers = m_dwCurrentPlayers;
		memcpy(&pdpnAppDesc->guidInstance,&m_guidInstance,sizeof(GUID));
		memcpy(&pdpnAppDesc->guidApplication,&m_guidApplication,sizeof(GUID));
	}

	Unlock();

	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDesc::PackInfo"

HRESULT CApplicationDesc::PackInfo(CPackedBuffer *const pPackedBuffer,
								   const DWORD dwFlags)
{
	HRESULT		hResultCode;
	DPN_APPLICATION_DESC_INFO	*pInfo;

	DPF(6,"Parameters: pPackedBuffer [0x%p], dwFlags [0x%lx]",pPackedBuffer,dwFlags);

	//
	//	Add structure
	//
	pInfo = reinterpret_cast<DPN_APPLICATION_DESC_INFO*>(pPackedBuffer->GetHeadAddress());
	hResultCode = pPackedBuffer->AddToFront(NULL,sizeof(DPN_APPLICATION_DESC_INFO));

	Lock();

	//
	//	Add session name
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME) && (m_dwSessionNameSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pwszSessionName,m_dwSessionNameSize);
		if (hResultCode == DPN_OK)
		{
			pInfo->dwSessionNameOffset = pPackedBuffer->GetTailOffset();
			pInfo->dwSessionNameSize = m_dwSessionNameSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pInfo->dwSessionNameOffset = 0;
			pInfo->dwSessionNameSize = 0;
		}
	}

	//
	//	Add password
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_PASSWORD) && (m_dwPasswordSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pwszPassword,m_dwPasswordSize);
		if (hResultCode == DPN_OK)
		{
			pInfo->dwPasswordOffset = pPackedBuffer->GetTailOffset();
			pInfo->dwPasswordSize = m_dwPasswordSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pInfo->dwPasswordOffset = 0;
			pInfo->dwPasswordSize = 0;
		}
	}

	//
	//	Add reserved data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA) && (m_dwReservedDataSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pvReservedData,m_dwReservedDataSize);
		if (hResultCode == DPN_OK)
		{
			pInfo->dwReservedDataOffset = pPackedBuffer->GetTailOffset();
			pInfo->dwReservedDataSize = m_dwReservedDataSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pInfo->dwReservedDataOffset = 0;
			pInfo->dwReservedDataSize = 0;
		}
	}

	//
	//	Add app reserved data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA) && (m_dwApplicationReservedDataSize > 0))
	{
		hResultCode = pPackedBuffer->AddToBack(m_pvApplicationReservedData,m_dwApplicationReservedDataSize);
		if (hResultCode == DPN_OK)
		{
			pInfo->dwApplicationReservedDataOffset = pPackedBuffer->GetTailOffset();
			pInfo->dwApplicationReservedDataSize = m_dwApplicationReservedDataSize;
		}
	}
	else
	{
		if (hResultCode == DPN_OK)
		{
			pInfo->dwApplicationReservedDataOffset = 0;
			pInfo->dwApplicationReservedDataSize = 0;
		}
	}

	//
	//	Fill in remainder of structure (if space)
	//
	if (hResultCode == DPN_OK)
	{
		pInfo->dwFlags = m_dwFlags;
		pInfo->dwMaxPlayers = m_dwMaxPlayers;
		pInfo->dwCurrentPlayers = m_dwCurrentPlayers;
		memcpy(&pInfo->guidInstance,&m_guidInstance,sizeof(GUID));
		memcpy(&pInfo->guidApplication,&m_guidApplication,sizeof(GUID));
	}

	Unlock();

	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDecs::UnpackInfo"

HRESULT CApplicationDesc::UnpackInfo(DPN_APPLICATION_DESC_INFO *const pdnAppDescInfo,
									 void *const pBufferStart,
									 const DWORD dwFlags)
{
	HRESULT		hResultCode;
	WCHAR		*pwszSessionName;
	WCHAR		*pwszPassword;
	void		*pvReservedData;
	void		*pvApplicationReservedData;

	DPF(6,"Parameters: pdnAppDescInfo [0x%p], pBufferStart [0x%p], dwFlags [0x%lx]",pdnAppDescInfo,pBufferStart,dwFlags);

	pwszSessionName = NULL;
	pwszPassword = NULL;
	pvReservedData = NULL;
	pvApplicationReservedData = NULL;

	//
	//	We will allocate the required memory for the data fields first.
	//	If everything succeeds, we will update the object
	//

	//
	//	Session Name
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME) && (pdnAppDescInfo->dwSessionNameSize))
	{
		if ((pwszSessionName = static_cast<WCHAR*>(DNMalloc(pdnAppDescInfo->dwSessionNameSize))) == NULL)
		{
			DPFERR("Could not allocate application desc session name");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pwszSessionName,
				static_cast<BYTE*>(pBufferStart) + pdnAppDescInfo->dwSessionNameOffset,
				pdnAppDescInfo->dwSessionNameSize);
	}

	//
	//	Password
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_PASSWORD) && (pdnAppDescInfo->dwPasswordSize))
	{
		if ((pwszPassword = static_cast<WCHAR*>(DNMalloc(pdnAppDescInfo->dwPasswordSize))) == NULL)
		{
			DPFERR("Could not allocate application desc password");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pwszPassword,
				static_cast<BYTE*>(pBufferStart) + pdnAppDescInfo->dwPasswordOffset,
				pdnAppDescInfo->dwPasswordSize);
	}

	//
	//	Reserved Data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA) && (pdnAppDescInfo->dwReservedDataSize))
	{
		if ((pvReservedData = DNMalloc(pdnAppDescInfo->dwReservedDataSize)) == NULL)
		{
			DPFERR("Could not allocate application desc reserved data");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pvReservedData,
				static_cast<BYTE*>(pBufferStart) + pdnAppDescInfo->dwReservedDataOffset,
				pdnAppDescInfo->dwReservedDataSize);
	}

	//
	//	Application Reserved Data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA) && (pdnAppDescInfo->dwApplicationReservedDataSize))
	{
		if ((pvApplicationReservedData = DNMalloc(pdnAppDescInfo->dwApplicationReservedDataSize)) == NULL)
		{
			DPFERR("Could not allocate application desc app reserved data");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pvApplicationReservedData,
				static_cast<BYTE*>(pBufferStart) + pdnAppDescInfo->dwApplicationReservedDataOffset,
				pdnAppDescInfo->dwApplicationReservedDataSize);
	}


	//
	//	Replace old values in object
	//

	Lock();

	//
	//	Session Name
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME)
	{
		if (m_pwszSessionName)
		{
			DNFree(m_pwszSessionName);
			m_pwszSessionName = NULL;
			m_dwSessionNameSize = 0;
		}
		m_pwszSessionName = pwszSessionName;
		m_dwSessionNameSize = pdnAppDescInfo->dwSessionNameSize;
	}

	//
	//	Password
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_PASSWORD)
	{
		if (m_pwszPassword)
		{
			DNFree(m_pwszPassword);
			m_pwszPassword = NULL;
			m_dwPasswordSize = 0;
		}
		m_pwszPassword = pwszPassword;
		m_dwPasswordSize = pdnAppDescInfo->dwPasswordSize;
	}

	//
	//	Reserved Data
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA)
	{
		if (m_pvReservedData)
		{
			DNFree(m_pvReservedData);
			m_pvReservedData = NULL;
			m_dwReservedDataSize = 0;
		}
		m_pvReservedData = pvReservedData;
		m_dwReservedDataSize = pdnAppDescInfo->dwReservedDataSize;
	}

	//
	//	Application Reserved Data
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA)
	{
		if (m_pvApplicationReservedData)
		{
			DNFree(m_pvApplicationReservedData);
			m_pvApplicationReservedData = NULL;
			m_dwApplicationReservedDataSize = 0;
		}
		m_pvApplicationReservedData = pvApplicationReservedData;
		m_dwApplicationReservedDataSize = pdnAppDescInfo->dwApplicationReservedDataSize;
	}

	//
	//	Remaining fields
	//
	m_dwMaxPlayers = pdnAppDescInfo->dwMaxPlayers;
	m_dwFlags = pdnAppDescInfo->dwFlags;
	memcpy(&m_guidInstance,&pdnAppDescInfo->guidInstance,sizeof(GUID));
	memcpy(&m_guidApplication,&pdnAppDescInfo->guidApplication,sizeof(GUID));

	Unlock();

	hResultCode = DPN_OK;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pwszSessionName)
	{
		DNFree(pwszSessionName);
		pwszSessionName = NULL;
	}
	if (pwszPassword)
	{
		DNFree(pwszPassword);
		pwszPassword = NULL;
	}
	if (pvReservedData)
	{
		DNFree(pvReservedData);
		pvReservedData = NULL;
	}
	if (pvApplicationReservedData)
	{
		DNFree(pvApplicationReservedData);
		pvApplicationReservedData = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDecs::Update"

HRESULT CApplicationDesc::Update(const DPN_APPLICATION_DESC *const pdnAppDesc,
								 const DWORD dwFlags)
{
	HRESULT		hResultCode;
	WCHAR		*pwszSessionName;
	DWORD		dwSessionNameSize;
	WCHAR		*pwszPassword;
	DWORD		dwPasswordSize;
	void		*pvReservedData;
	void		*pvApplicationReservedData;

	DPF(6,"Parameters: pdnAppDesc [0x%p], dwFlags [0x%lx]",pdnAppDesc,dwFlags);

	pwszSessionName = NULL;
	dwSessionNameSize = 0;
	pwszPassword = NULL;
	dwPasswordSize = 0;
	pvReservedData = NULL;
	pvApplicationReservedData = NULL;

	//
	//	We will allocate the required memory for the data fields first.
	//	If everything succeeds, we will update the object
	//

	//
	//	Session Name
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME) && (pdnAppDesc->pwszSessionName))
	{
		dwSessionNameSize = (wcslen(pdnAppDesc->pwszSessionName) + 1) * sizeof(WCHAR);
		if ((pwszSessionName = static_cast<WCHAR*>(DNMalloc(dwSessionNameSize))) == NULL)
		{
			DPFERR("Could not allocate application desc session name");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pwszSessionName,
				pdnAppDesc->pwszSessionName,
				dwSessionNameSize);
	}

	//
	//	Password
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_PASSWORD) && (pdnAppDesc->pwszPassword))
	{
		dwPasswordSize = (wcslen(pdnAppDesc->pwszPassword) + 1) * sizeof(WCHAR);
		if ((pwszPassword = static_cast<WCHAR*>(DNMalloc(dwPasswordSize))) == NULL)
		{
			DPFERR("Could not allocate application desc password");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pwszPassword,
				pdnAppDesc->pwszPassword,
				dwPasswordSize);
	}

	//
	//	Reserved Data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA) && (pdnAppDesc->pvReservedData))
	{
		if ((pvReservedData = DNMalloc(pdnAppDesc->dwReservedDataSize)) == NULL)
		{
			DPFERR("Could not allocate application desc reserved data");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pvReservedData,
				pdnAppDesc->pvReservedData,
				pdnAppDesc->dwReservedDataSize);
	}

	//
	//	Application Reserved Data
	//
	if ((dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA) && (pdnAppDesc->pvApplicationReservedData))
	{
		if ((pvApplicationReservedData = DNMalloc(pdnAppDesc->dwApplicationReservedDataSize)) == NULL)
		{
			DPFERR("Could not allocate application desc app reserved data");
			hResultCode = DPNERR_OUTOFMEMORY;
			goto Failure;
		}
		memcpy(	pvApplicationReservedData,
				pdnAppDesc->pvApplicationReservedData,
				pdnAppDesc->dwApplicationReservedDataSize);
	}


	//
	//	Replace old values in object
	//

	//
	//	Session Name
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_SESSIONNAME)
	{
		if (m_pwszSessionName)
		{
			DNFree(m_pwszSessionName);
			m_pwszSessionName = NULL;
			m_dwSessionNameSize = 0;
		}
		m_pwszSessionName = pwszSessionName;
		m_dwSessionNameSize = dwSessionNameSize;
	}

	//
	//	Password
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_PASSWORD)
	{
		if (m_pwszPassword)
		{
			DNFree(m_pwszPassword);
			m_pwszPassword = NULL;
			m_dwPasswordSize = 0;
		}
		m_pwszPassword = pwszPassword;
		m_dwPasswordSize = dwPasswordSize;
	}

	//
	//	Reserved Data
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_RESERVEDDATA)
	{
		if (m_pvReservedData)
		{
			DNFree(m_pvReservedData);
			m_pvReservedData = NULL;
			m_dwReservedDataSize = 0;
		}
		m_pvReservedData = pvReservedData;
		m_dwReservedDataSize = pdnAppDesc->dwReservedDataSize;
	}

	//
	//	Application Reserved Data
	//
	if (dwFlags & DN_APPDESCINFO_FLAG_APPRESERVEDDATA)
	{
		if (m_pvApplicationReservedData)
		{
			DNFree(m_pvApplicationReservedData);
			m_pvApplicationReservedData = NULL;
			m_dwApplicationReservedDataSize = 0;
		}
		m_pvApplicationReservedData = pvApplicationReservedData;
		m_dwApplicationReservedDataSize = pdnAppDesc->dwApplicationReservedDataSize;
	}

	//
	//	Remaining fields
	//
	m_dwMaxPlayers = pdnAppDesc->dwMaxPlayers;
	m_dwFlags = pdnAppDesc->dwFlags;
	memcpy(&m_guidInstance,&pdnAppDesc->guidInstance,sizeof(GUID));
	memcpy(&m_guidApplication,&pdnAppDesc->guidApplication,sizeof(GUID));

	hResultCode = DPN_OK;

Exit:
	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);

Failure:
	if (pwszSessionName)
	{
		DNFree(pwszSessionName);
		pwszSessionName = NULL;
	}
	if (pwszPassword)
	{
		DNFree(pwszPassword);
		pwszPassword = NULL;
	}
	if (pvReservedData)
	{
		DNFree(pvReservedData);
		pvReservedData = NULL;
	}
	if (pvApplicationReservedData)
	{
		DNFree(pvApplicationReservedData);
		pvApplicationReservedData = NULL;
	}
	goto Exit;
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDecs::CreateNewInstanceGuid"

HRESULT	CApplicationDesc::CreateNewInstanceGuid( void )
{
	HRESULT		hResultCode;

	hResultCode = XUuidCreate(&m_guidInstance);

	return(hResultCode);
}

#undef DPF_SUBCOMP
#define DPF_SUBCOMP DN_SUBCOMP_CORE

#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDecs::IncPlayerCount"

HRESULT	CApplicationDesc::IncPlayerCount(const BOOL fCheckLimit)
{
	HRESULT		hResultCode;

	Lock();

	if ((fCheckLimit) && (m_dwMaxPlayers) && (m_dwCurrentPlayers >= m_dwMaxPlayers))
	{
		hResultCode = DPNERR_SESSIONFULL;
	}
	else
	{
		m_dwCurrentPlayers++;
		hResultCode = DPN_OK;
	}

	Unlock();

	return(hResultCode);
}


#undef DPF_MODNAME
#define DPF_MODNAME "CApplicationDecs::DecPlayerCount"

void CApplicationDesc::DecPlayerCount( void )
{
	Lock();
	m_dwCurrentPlayers--;
	Unlock();
}


#undef DPF_MODNAME
#define DPF_MODNAME "DNProcessUpdateAppDesc"

HRESULT	DNProcessUpdateAppDesc(DIRECTNETOBJECT *const pdnObject,
							   DPN_APPLICATION_DESC_INFO *const pv)
{
	HRESULT				hResultCode;

	DPF(6,"Parameters: pv [0x%p]",pv);

	DNASSERT(pdnObject != NULL);
	DNASSERT(pv != NULL);

	hResultCode = pdnObject->ApplicationDesc.UnpackInfo(pv,
														pv,
														DN_APPDESCINFO_FLAG_SESSIONNAME | DN_APPDESCINFO_FLAG_PASSWORD |
														DN_APPDESCINFO_FLAG_RESERVEDDATA | DN_APPDESCINFO_FLAG_APPRESERVEDDATA);

	hResultCode = DNUserUpdateAppDesc(pdnObject);

	DPF(6,"Returning: [0x%lx]",hResultCode);
	return(hResultCode);
}


// *****************************************************
// XUuidCreate()
//
// Generate an UUID with RC4 algorithm.
//
// *****************************************************

HRESULT XUuidCreate( GUID *Uuid )
{
    UUID_GENERATE *UuidGen = (UUID_GENERATE *) Uuid;
    BYTE newSeed[256];

    // Get a new seed if it hasn't been set yet
    //
    if ( g_ulSeed == 0 )
    {
        g_ulSeed = NtGetTickCount();
    }

    // Calculate a 256 byte seed key for the RC4 algorithm
    //
    NewGenRandom( NULL, NULL, (unsigned char *) Uuid, sizeof(GUID) );

    // Overwrite some bits of the UUID (not sure why -- it's like that in the W2k source)
    //
    UuidGen->TimeHiAndVersion = (UuidGen->TimeHiAndVersion & UUID_TIME_HIGH_MASK) | RAND_UUID_VERSION;
    UuidGen->ClockSeqHiAndReserved = (UuidGen->ClockSeqHiAndReserved & UUID_CLOCK_SEQ_HI_MASK) | UUID_RESERVED;

    return S_OK;
}

// *****************************************************
// NewGenRandom()
//
// Fill a buffer with random data up to dwLength size.
//
// ASSUMPTION:  dwLength is a multiple of 4 bytes.  We
// can assume this because UuidCreate is our only
// client.  When we switch to a real NewGenRandom()
// function (the RC4 one), then this will not matter.
//
// *****************************************************

int NewGenRandom( unsigned char **ppbRandSeed, unsigned long *pcbRandSeed, unsigned char *pbBuffer, unsigned long dwLength )
{
    int i, nMax;

    nMax = dwLength / sizeof(unsigned long);

    for ( i = 0; i < nMax; i += 1 )
    {
        ((unsigned long *)pbBuffer)[i] = IRand();
    }

    return 1;
}

// *****************************************************
// IRand()
//
// Generate a random unsigned long.  Will be replaced
// when real RC4 NewGenRandom is used.
//
// *****************************************************

unsigned long IRand( void )
{
    g_ulSeed =  1664525 * g_ulSeed + 1013904223;

    return g_ulSeed;
}
