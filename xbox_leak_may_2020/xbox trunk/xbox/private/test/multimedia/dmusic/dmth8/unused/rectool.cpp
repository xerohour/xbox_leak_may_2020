//-----------------------------------------------------------------------------
//	File: RecTool.cpp
//
//	Implementation of the Recording Tool.  See RecTool.h for more info and a
//	description of the expected tool behavior.
//
//	History:
//	05/08/1998 - jimmo - Created
//-----------------------------------------------------------------------------


// #include "tdmusic.h"
#include "RecTool.h"
#include <stdio.h>
#include <stddef.h>
#include "assert.h"
#include "diffinfo.h"
#include "RecTake.h"
#include "Factory.h"

HRESULT PrintPMsg (DMUS_PMSG* pPMsg);
class CThreadSafe
{
public :
	CThreadSafe (CRITICAL_SECTION* pCS) : m_pCS (pCS)
	{
		EnterCriticalSection (m_pCS);
	}
	~CThreadSafe ()
	{
		LeaveCriticalSection (m_pCS);
	}

private :
	CRITICAL_SECTION* m_pCS;

};

// {903ACE7E-2EAD-45dc-BD63-38E9C5601C97}
static const GUID IID_ITDMRECTOOL =
{ 0x903ace7e, 0x2ead, 0x45dc, { 0xbd, 0x63, 0x38, 0xe9, 0xc5, 0x60, 0x1c, 0x97 } };

static CFactory::CRegister var (IID_ITDMRECTOOL, CRecordTool::Create);

struct msgType
{
	DMUS_PMSGT_TYPES			type;
	LPCSTR						pName;
};

static const msgType msgTypes[] =
{
	{ DMUS_PMSGT_MIDI,				"DMUS_PMSGT_MIDI",				},	// MIDI short message
	{ DMUS_PMSGT_NOTE,				"DMUS_PMSGT_NOTE",				},	// Interactive Music Note
	{ DMUS_PMSGT_SYSEX,				"DMUS_PMSGT_SYSEX",				},	// MIDI long message (system exclusive message)
	{ DMUS_PMSGT_NOTIFICATION,		"DMUS_PMSGT_NOTIFICATION",		},	// Notification message
	{ DMUS_PMSGT_TEMPO,				"DMUS_PMSGT_TEMPO",				},	// Tempo message
	{ DMUS_PMSGT_CURVE,				"DMUS_PMSGT_CURVE",				},	// Control change / pitch bend, etc. curve
	{ DMUS_PMSGT_TIMESIG,			"DMUS_PMSGT_TIMESIG",			},	// Time signature
	{ DMUS_PMSGT_PATCH,				"DMUS_PMSGT_PATCH",				},	// Patch changes
	{ DMUS_PMSGT_TRANSPOSE,			"DMUS_PMSGT_TRANSPOSE",			},	// Transposition messages
	{ DMUS_PMSGT_CHANNEL_PRIORITY,	"DMUS_PMSGT_CHANNEL_PRIORITY",	},	// Channel priority
	{ DMUS_PMSGT_STOP,				"DMUS_PMSGT_STOP",				},	// Stop message
	{ DMUS_PMSGT_DIRTY,				"DMUS_PMSGT_DIRTY",				},	// Tells Tools that cache GetParam() info to refresh
	{ DMUS_PMSGT_WAVE,				"DMUS_PMSGT_WAVE",				},	// Carries control information for playing a wave.
	{ DMUS_PMSGT_LYRIC,				"DMUS_PMSGT_LYRIC",				},	// Lyric message from lyric track.
	{ DMUS_PMSGT_SCRIPTLYRIC,		"DMUS_PMSGT_SCRIPTLYRIC",		},	// Lyric message sent by a script with the Trace function.
	{ DMUS_PMSGT_USER,				"DMUS_PMSGT_USER",				},	// User message
};

//
//	Basic Constructure
//
CRecordTool::CRecordTool()
{
	m_cRef				= 1;
	m_dwNumElements 	= sizeof (msgTypes) / sizeof (msgTypes[0]);

	m_fRecording		= FALSE;

	m_dwTakeCount		= 0;
	m_dwCurTake			= -1;
	m_pCurTake			= NULL;

	InitializeCriticalSection(&m_CrSec);
}

//
//	Standard Destructor
//
CRecordTool::~CRecordTool()
{
	Erase ();
	DeleteCriticalSection (&m_CrSec);
}


/////////////////////////////////////////////////////////////////
// IUnknown
STDMETHODIMP CRecordTool::QueryInterface
(
	const IID &iid,
	void **ppv
)
{
	if (iid == IID_IUnknown || iid == IID_IDirectMusicTool)
	{
		*ppv = static_cast<IDirectMusicTool*>(this);
	}
	//This IID is useful if you get back the tool as an IDirectMusicTool pointer
	//you can QI for IID_ITESTTOOL to get access to your tools methods
	else if (iid == IID_ITDMRECTOOL)
	{
		*ppv = static_cast<CRecordTool*>(this);
	}
	else
	{
		*ppv = NULL;
		return E_NOINTERFACE;
	}

	AddRef();

	return S_OK;
}

STDMETHODIMP_(ULONG) CRecordTool::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG) CRecordTool::Release()
{
	if( 0 == InterlockedDecrement(&m_cRef) )
	{
		delete this;
		return 0;
	}

	return m_cRef;
}

/////////////////////////////////////////////////////////////////
// IDirectMusicTool

HRESULT STDMETHODCALLTYPE CRecordTool::Init
(
	IDirectMusicGraph* pGraph
)
{
	return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE CRecordTool::GetMsgDeliveryType
(
	DWORD* pdwDeliveryType
)
{
	*pdwDeliveryType = DMUS_PMSGF_TOOL_IMMEDIATE;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRecordTool::GetMediaTypeArraySize
(
	DWORD* pdwNumElements
)
{
	//Number of elements that are retrieved from GetMediaTypes
	*pdwNumElements = m_dwNumElements;

	return S_OK;
}

HRESULT STDMETHODCALLTYPE CRecordTool::GetMediaTypes
(
	DWORD** padwMediaTypes,
	DWORD	dwNumElements
)
{
	// Fill in the array padwMediaTypes with the type of
	// messages this tool wants to process. dwNumElements is
	// the number requestes (should be the same as what was handed out
	// in the GetMediaTypeArraySize() method, above.

	if( m_dwNumElements != dwNumElements )
	{
		return E_INVALIDARG;
	}

	// set the elements in the array to DMUS_PMSGT_NOTE,
	// DMUS_PMSGT_MIDI, and DMUS_PMSGT_PATCH
	for (int i=0; i < m_dwNumElements; ++i)
	{
		(*padwMediaTypes)[i] = msgTypes[i].type;
	}


	return S_OK;
}


HRESULT STDMETHODCALLTYPE CRecordTool::ProcessPMsg
(
	IDirectMusicPerformance* pPerf,
	DMUS_PMSG*				 pPMsg
)
{
	HRESULT hr		= E_FAIL;

	if(( NULL == pPMsg->pGraph ) ||
		FAILED(pPMsg->pGraph->StampPMsg(pPMsg)))
	{
		return DMUS_S_FREE;
	}

	AddPMsg (pPMsg);

	// return DMUS_S_REQUEUE so the original message is requeued
	return DMUS_S_REQUEUE;
}


HRESULT STDMETHODCALLTYPE CRecordTool::Flush
(
	IDirectMusicPerformance* pPerf,
	DMUS_PMSG*				 pDMUS_PMSG,
	REFERENCE_TIME			 rt
)
{
	return E_NOTIMPL;
}

GUID CRecordTool::GetIID()
{
	return (IID_ITDMRECTOOL);
}

GUID CRecordTool::GetCLSID()
{
	return (IID_ITDMRECTOOL);
}

HRESULT CRecordTool::Create(IUnknown** ppv)
{
	if (!ppv)
	{
		return (E_INVALIDARG);
	}

	*ppv = static_cast <IUnknown*> (new CRecordTool);

	if (!*ppv)
	{
		return (E_OUTOFMEMORY);
	}

	return (S_OK);
}

HRESULT CRecordTool::Erase()
{
	CThreadSafe ts (&m_CrSec);

	try
	{
		for (iterator itt = m_Takes.begin ();itt != m_Takes.end ();itt++)
		{
			delete itt->pTake;
		}

		m_Takes.clear ();
		m_dwTakeCount = 0;
	}
	catch (...)
	{
		return (E_FAIL);
	}

	return (S_OK);
}

//////////////////////////////////////////////////////////////////////////
//
//	SetRecordMode:
//
//	This function is called to set the record mode.
//	If fOnOff == TRUE, a new RECTOOL_TAKE_LIST element is created and
//	added to the list of Takes, pointed to by m_pTakeList.
//
//	History:
//		05.09.1999 - jimmo - Created
//
//////////////////////////////////////////////////////////////////////////
HRESULT CRecordTool::SetRecordMode
(
	BOOL	fOnOff,
	DWORD	dwTake
)
{
	CThreadSafe ts (&m_CrSec);
	m_dwCurTake = dwTake;

	m_pCurTake = NULL;

	if (TRUE == fOnOff)
	{
		// If a take already exists for this Take number, remove it
		if (NULL != GetTake(dwTake))
		{
			RemoveTake(dwTake);
		}

		RECTOOL_TAKE take;
		take.dwIndex = dwTake;
		take.fRecord = MSG_ALL;
		take.pTake = CRecTake::Create ();

		if (NULL == take.pTake)
		{
			printf("!!!! [RecTool::SetRecordMode] Failed to create new Take[%d], Out of Memory?\n", take.dwIndex);
			return E_OUTOFMEMORY;
		}

		m_Takes.push_back (take);
		m_pCurTake = --(m_Takes.end ());

		m_dwTakeCount++;
	}

	m_fRecording = fOnOff;

	return S_OK;
}


//////////////////////////////////////////////////////////////////////////
//
//	SetRecordFlags:
//
//	This function is called to control the messages recorded by this tool
//		if the take number does not exist, a new one is created.
//
//	History:
//		08.01.2000 - ruswoods - Created
//
//////////////////////////////////////////////////////////////////////////
HRESULT CRecordTool::SetRecordFlags(MESSAGES_FLAGS fFlags, DWORD dwTake)
{
	HRESULT hr = S_OK;

	if (m_pCurTake == NULL || (m_pCurTake != NULL && 
		m_pCurTake->dwIndex != dwTake))
	{
		SetRecordMode (true, dwTake);
	}

	if (hr != S_OK)
	{
		return (hr);
	}

	m_pCurTake->fRecord = fFlags;

	return (hr);
}

//////////////////////////////////////////////////////////////////////////
//
//	CompareTwoTakes:
//
//	This function compares the two takes, given by dwTake1 and dwTake2
//
//	Return Value:
//	   -1	if Error occurred or invalid params
//		0  if dwTake1 != dwTake2
//		1  if dwTake1 == dwTake2
//
//	History:
//		05.20.1999 - jimmo - Created
//
//////////////////////////////////////////////////////////////////////////
HRESULT CRecordTool::CompareTwoTakes
(
	DWORD	dwTake1,
	DWORD	dwTake2
)
{
	CThreadSafe ts (&m_CrSec);

	HRESULT hr = E_INVALIDARG;

	CRecTake* pLhs = GetTake (dwTake1);
	CRecTake* pRhs = GetTake (dwTake2);

	if (pLhs && pRhs)
	{
		hr = pLhs->Compare (pRhs);
	}

	return hr;
}


//////////////////////////////////////////////////////////////////////////
//
//	RemoveTake:
//
//	This function removes a take from the take list.
//
//	Return Value:
//		HRESULT
//
//	Params:
//		DWORD dwTake -- ID of take
//
//	History:
//		06.27.1999 - jimmo - Created
//
/////////////////////////////////////////////////////////////////////////
HRESULT CRecordTool::RemoveTake
(
	DWORD	dwTake
)
{
	CThreadSafe ts (&m_CrSec);

	for (iterator itt = m_Takes.begin ();itt != m_Takes.end ();++itt)
	{
		if (itt->dwIndex == dwTake)
		{
			delete itt->pTake;
			m_Takes.erase (itt);
			--m_dwTakeCount;

			if (dwTake == m_dwCurTake)
			{
				m_pCurTake = NULL;
			}

			return (S_OK);
		}
	}

	// If there was no matching take, return a success code
	return S_FALSE;
}

//////////////////////////////////////////////////////////////////////////
//
//	GetTake:
//
//	This function returns a RECTOOL_TAKE_LIST* for the take who's index
//	is given as dwTake
//
//	Return Value:
//		RECTOOL_TAKE_LIST* -- pointer to take or NULL if dwTake not found
//
//	Params:
//		DWORD dwTake -- ID of take
//
//	History:
//		06.22.1999 - jimmo - Created
//
//////////////////////////////////////////////////////////////////////////
CRecTake* CRecordTool::GetTake
(
	DWORD	dwTake
) const
{
	CRecTake* pTake = NULL;

	for (const_iterator itt = m_Takes.begin ();!pTake && itt != m_Takes.end ();++itt)
	{
		if (itt->dwIndex == dwTake)
		{
			pTake = itt->pTake;
		}
	}

	return pTake;
}

void CRecordTool::AddPMsg
(
	DMUS_PMSG *pPMsg
)
{
	BOOL bAdd = false;

	if (m_fRecording && m_pCurTake != NULL)
	{
		// Check to see if the flag for this message is set
		// Since the user message doesn't work with the flags, check it separately
		if (pPMsg->dwType == DMUS_PMSGT_USER)
		{
			bAdd |= m_pCurTake->fRecord & MSG_USER;
		}
		else
		{
			bAdd |= m_pCurTake->fRecord & (1 << pPMsg->dwType);
		}

		// If the type was user and the user flag was set or
		// if the flag was set for any other type, add the message
		if (bAdd)
		{
			CThreadSafe ts (&m_CrSec);

			m_pCurTake->pTake->AddMsg (pPMsg);
		}
	}
}

HRESULT CRecordTool::SaveTake(DWORD dwTake, LPCSTR pFilename)
{
	CRecTake::list	messages;
	FILE*			pFile = NULL;

	if (!pFilename)
	{
		return (E_INVALIDARG);
	}

	CRecTake* pTake = GetTake (dwTake);
	if (!pTake)
	{
		return (E_INVALIDARG);
	}

	// Attempt to open the file
	pFile = fopen (pFilename,"wb");
	if (!pFile)
	{
		return (E_FAIL);
	}

	EnterCriticalSection (&m_CrSec);
	messages = pTake->GetMessages ();
	LeaveCriticalSection (&m_CrSec);

	// File structure -
	// Number of elements
	// each element
	//		PMSG index
	//		PMSG data

	HRESULT hr = E_FAIL;

	do
	{
		int size = 0;
		// Any failure will cause a break
		// This will prevent the return of S_OK
		int nElements = messages.size ();
		if (fwrite (&nElements, sizeof (nElements), 1, pFile) != 1)
		{
			break;
		}
		size += sizeof (nElements);

		for (CRecTake::iterator itt = messages.begin ();itt != messages.end ();++itt)
		{
			CRecTake::RECTOOL_PMSG const& msg = *itt;
			if (fwrite (&msg.dwIndex, sizeof (msg.dwIndex), 1, pFile) != 1)
			{
				break;
			}
			size += sizeof (msg.dwIndex);

			if (fwrite (msg.pPMsg, 1, msg.pPMsg->dwSize, pFile) != msg.pPMsg->dwSize)
			{
				break;
			}

			size += msg.pPMsg->dwSize;
		}

		// If everything was processed, we're OK
		if (itt == messages.end ())
		{
			hr = S_OK;
		}

	} while (0);


	fclose (pFile);

	if (FAILED (hr))
	{
//		unlink (pFilename);
	}

	return (hr);
}

HRESULT CRecordTool::ReadTake(DWORD dwTake, int resource)
{
	// If a take already exists for this Take number, remove it
	if (NULL != GetTake(dwTake))
	{
		RemoveTake(dwTake);
	}

	RECTOOL_TAKE take;
	take.dwIndex = dwTake;
	take.pTake = CRecTake::Create ();

	if (NULL == take.pTake)
	{
		printf("!!!! [RecTool::SetRecordMode] Failed to create new Take[%d], Out of Memory?\n", take.dwIndex);
		return E_OUTOFMEMORY;
	}

	m_Takes.push_back (take);

	++m_dwTakeCount;
	take.pTake->ReadTake (resource);

	return (S_OK);
}

int CRecordTool::GetMessageCount (DWORD type) const
{
	CThreadSafe		ts (&m_CrSec);
	CRecTake::list	messages;
	int 			nMessages = 0;

	CRecTake* pTake = GetTake(m_dwCurTake);


	if (NULL == pTake)
	{
		return (nMessages);
	}

	if (type)
	{
		messages = pTake->GetMessages (type);
		nMessages = messages.size ();
	}
	else
	{
		nMessages = pTake->GetMessageCount ();
	}

	return (nMessages);
}

void CRecordTool::PrintMessages()
{
	CRecTake*					pTake		= NULL;
	CRecTake::list				messages;
	CRecTake::const_iterator	cit;


	// Set this block asside for the critical section
	{
		CThreadSafe		ts (&m_CrSec);
		pTake = GetTake(m_dwCurTake);
		messages = pTake->GetMessages ();
	}

	for (cit = messages.begin (); cit != messages.end (); ++cit)
	{
		CRecTake::PrintPMsg (cit->pPMsg);
	}
}
