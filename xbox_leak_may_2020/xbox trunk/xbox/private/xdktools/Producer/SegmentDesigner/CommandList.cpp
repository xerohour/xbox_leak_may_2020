//  CommandList.cpp

// This class loads an CommandList chunk from a IMA 25 design-time template file.
// It also will be able to write out a DirectMusic Command track.

#include "stdafx.h"

#include "CommandList.h"
#include "FileStructs.h"
#include "RiffStructs.h"
#include <RiffStrm.h>
#include "Track.h"
#include <DMusProd.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define IMA_PPQ 192

inline DWORD ClocksPerBeat(DMUS_TIMESIGNATURE& TimeSig)
{ return (DMUS_PPQ * 4) / TimeSig.bBeat; }

inline DWORD ClocksPerMeasure(DMUS_TIMESIGNATURE& TimeSig)
{ return ClocksPerBeat(TimeSig) * TimeSig.bBeatsPerMeasure; }

inline WORD ClocksToMeasure(DWORD dwTotalClocks, DMUS_TIMESIGNATURE& TimeSig)
{ return (WORD) (dwTotalClocks / ClocksPerMeasure(TimeSig)); }

CCommandList::CCommandList()
{
	m_nLastImportantMeasure = 0;
	m_lClocksPerMeasure = 0;
	m_pIFramework = NULL;

	// Fill in the time sig event with default values (4/4, 16th note resolution)
	m_TimeSig.mtTime = 0;
	m_TimeSig.bBeatsPerMeasure = 4;
	m_TimeSig.bBeat = 4;
	m_TimeSig.wGridsPerBeat = 4;
}

CCommandList::~CCommandList()
{
	RemoveAll();
}

HRESULT CCommandList::IMA_AddSectionCommand( IStream* pIStream, long lRecSize )
{
	DMUS_IO_COMMAND		*pDMCommand;
	IMASectionCommandExt imaSectionCommandExt;

	_LARGE_INTEGER	liTemp;

	HRESULT			hr;
	ULONG			ulBytesRead;

	if ( lRecSize < sizeof( IMASectionCommandExt ) )
	{
		ZeroMemory( &imaSectionCommandExt, sizeof( IMASectionCommandExt ) );
	}

	hr = pIStream->Read(&imaSectionCommandExt, lRecSize, &ulBytesRead);
	if(hr != S_OK || ulBytesRead != (ULONG) lRecSize)
	{
		if(FAILED(hr))
		{
			hr = E_FAIL;
		}
		return hr;
	}

	if ( lRecSize > sizeof( IMASectionCommandExt ) )
	{
		liTemp.LowPart = lRecSize - sizeof( IMASectionCommandExt );
		liTemp.HighPart = 0;
    	pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
    }

	pDMCommand = new DMUS_IO_COMMAND;
	ASSERT( pDMCommand != NULL );
	if ( pDMCommand == NULL )
	{
		return E_OUTOFMEMORY;
	}

	pDMCommand->mtTime = imaSectionCommandExt.lTime * DMUS_PPQ / IMA_PPQ;
	pDMCommand->wMeasure = WORD (pDMCommand->mtTime / ClocksPerMeasure(m_TimeSig));
	pDMCommand->bBeat = 0;

	switch ( imaSectionCommandExt.dwCommand )
	{
	case PF_FILL:
		pDMCommand->bCommand = DMUS_COMMANDT_FILL;
		pDMCommand->bGrooveLevel = 0;
		break;
	case PF_INTRO:
		pDMCommand->bCommand = DMUS_COMMANDT_INTRO;
		pDMCommand->bGrooveLevel = 0;
		break;
	case PF_BREAK:
		pDMCommand->bCommand = DMUS_COMMANDT_BREAK;
		pDMCommand->bGrooveLevel = 0;
		break;
	case PF_END:
		pDMCommand->bCommand = DMUS_COMMANDT_END;
		pDMCommand->bGrooveLevel = 0;
		break;
	case PF_A:
		pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
		pDMCommand->bGrooveLevel = 12;
		break;
	case PF_B:
		pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
		pDMCommand->bGrooveLevel = 37;
		break;
	case PF_C:
		pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
		pDMCommand->bGrooveLevel = 62;
		break;
	case PF_D:
		pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
		pDMCommand->bGrooveLevel = 87;
		break;
	default:  // default to a Groove with level 0 (interpretation: use previous groove level)
		// This happens in the measure following an embellishment
		pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
		pDMCommand->bGrooveLevel = 0;
	}

	m_lstDMCommands.AddTail( pDMCommand );

	return hr;
}

HRESULT CCommandList::IMA_AddTemplateCommand( IStream* pIStream, long lRecSize )
{
	DMUS_IO_COMMAND		*pDMCommand;
	DMUS_IO_SIGNPOST	*pDMSignPost;
	IMATemplateCommandExt imaCommandExt;

	_LARGE_INTEGER	liTemp;

	HRESULT			hr;
	ULONG			ulBytesRead;

	if ( lRecSize < sizeof( IMATemplateCommandExt ) )
	{
		ZeroMemory( &imaCommandExt, sizeof( IMATemplateCommandExt ) );
	}

	hr = pIStream->Read(&imaCommandExt, lRecSize, &ulBytesRead);
	if(hr != S_OK || ulBytesRead != (ULONG) lRecSize)
	{
		if(FAILED(hr))
		{
			hr = E_FAIL;
		}
		return hr;
	}

	if ( lRecSize > sizeof( IMATemplateCommandExt ) )
	{
		liTemp.LowPart = lRecSize - sizeof( IMATemplateCommandExt );
		liTemp.HighPart = 0;
    	pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
    }

	// Check for a Command in imaCommandExt
	if ( imaCommandExt.dwCommand != 0 )
	{
		pDMCommand = new DMUS_IO_COMMAND;
		ASSERT( pDMCommand != NULL );
		if ( pDMCommand == NULL )
		{
			return E_OUTOFMEMORY;
		}

		pDMCommand->mtTime = ClocksPerMeasure(m_TimeSig) * imaCommandExt.nMeasure;
		pDMCommand->wMeasure = imaCommandExt.nMeasure;
		pDMCommand->bBeat = 0;

		switch ( imaCommandExt.dwCommand )
		{
		case PF_FILL:
			pDMCommand->bCommand = DMUS_COMMANDT_FILL;
			pDMCommand->bGrooveLevel = 0;
			break;
		case PF_INTRO:
			pDMCommand->bCommand = DMUS_COMMANDT_INTRO;
			pDMCommand->bGrooveLevel = 0;
			break;
		case PF_BREAK:
			pDMCommand->bCommand = DMUS_COMMANDT_BREAK;
			pDMCommand->bGrooveLevel = 0;
			break;
		case PF_END:
			pDMCommand->bCommand = DMUS_COMMANDT_END;
			pDMCommand->bGrooveLevel = 0;
			break;
		case PF_A:
			pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
			pDMCommand->bGrooveLevel = 12;
			break;
		case PF_B:
			pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
			pDMCommand->bGrooveLevel = 37;
			break;
		case PF_C:
			pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
			pDMCommand->bGrooveLevel = 62;
			break;
		case PF_D:
			pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
			pDMCommand->bGrooveLevel = 87;
			break;
		default:  // default to a Groove with level 0 (interpretation: use previous groove level)
			// This happens in the measure following an embellishment
			pDMCommand->bCommand = DMUS_COMMANDT_GROOVE;
			pDMCommand->bGrooveLevel = 0;
		}

		m_lstDMCommands.AddTail( pDMCommand );
	}

	// Check for a Signpost in imaCommandExt
	if ( imaCommandExt.dwSignPost != 0 )
	{
		pDMSignPost = new DMUS_IO_SIGNPOST;
		ASSERT( pDMSignPost != NULL );
		if ( pDMSignPost == NULL )
		{
			return E_OUTOFMEMORY;
		}

		pDMSignPost->mtTime = ClocksPerMeasure(m_TimeSig) * imaCommandExt.nMeasure;
		pDMSignPost->dwChords = imaCommandExt.dwSignPost;
		pDMSignPost->wMeasure = imaCommandExt.nMeasure;

		m_lstDMSignPosts.AddTail( pDMSignPost );
	}

	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandList::CreateCommandTrack()

HRESULT CCommandList::CreateCommandTrack( class CTrack** ppTrack )
{
	if( m_lstDMCommands.IsEmpty() )
	{
		return E_FAIL;
	}

	if ( ppTrack == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( m_pIFramework != NULL );
	if ( m_pIFramework == NULL )
	{
		return E_UNEXPECTED;
	}

	*ppTrack = NULL;
	CTrack *pNewTrack = new CTrack;
	if ( pNewTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream *pStream = NULL;
	
	HRESULT hr = m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream );
	if ( FAILED( hr ) )
	{
		delete pNewTrack;
		return hr;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( hr = AllocRIFFStream( pStream, &pIRiffStream ) ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	hr = DM_SaveCommandList( pIRiffStream );
	pIRiffStream->Release();
	pIRiffStream = NULL;
	if ( FAILED( hr ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	LARGE_INTEGER	liTemp;
	liTemp.QuadPart = 0;
	pStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

	memcpy( &pNewTrack->m_guidClassID, &CLSID_DirectMusicCommandTrack, sizeof( GUID ) );
	// Let the Segment fill m_guidEditorID in.
	//memcpy( &pNewTrack->m_guidEditorID, &CLSID_CommandMgr, sizeof( GUID ) );
	//pNewTrack->dwPosition = 0;
	pNewTrack->m_dwGroupBits = 0x00000001;
	//pNewTrack->punkStripMgr = NULL;
	pNewTrack->m_ckid = DMUS_FOURCC_COMMANDTRACK_CHUNK;
	//pNewTrack->m_fccType = 0;
	pNewTrack->SetStream( pStream );
	pStream->Release(); // Release our reference to the stream

	*ppTrack = pNewTrack;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandList::CreateSignPostTrack()

HRESULT CCommandList::CreateSignPostTrack( class CTrack** ppTrack )
{
	if( m_lstDMSignPosts.IsEmpty() )
	{
		return E_FAIL;
	}

	if ( ppTrack == NULL )
	{
		return E_INVALIDARG;
	}

	ASSERT( m_pIFramework != NULL );
	if ( m_pIFramework == NULL )
	{
		return E_UNEXPECTED;
	}

	*ppTrack = NULL;
	CTrack *pNewTrack = new CTrack;
	if ( pNewTrack == NULL )
	{
		return E_OUTOFMEMORY;
	}

	IStream *pStream = NULL;
	HRESULT hr = m_pIFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream );
	if ( FAILED( hr ) )
	{
		delete pNewTrack;
		return hr;
	}

	IDMUSProdRIFFStream* pIRiffStream;
	if( FAILED( hr = AllocRIFFStream( pStream, &pIRiffStream ) ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	hr = DM_SaveSignPostList( pIRiffStream );
	pIRiffStream->Release();
	pIRiffStream = NULL;
	if ( FAILED( hr ) )
	{
		pStream->Release();
		delete pNewTrack;
		return hr;
	}

	LARGE_INTEGER	liTemp;
	liTemp.QuadPart = 0;
	pStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning

	memcpy( &pNewTrack->m_guidClassID, &CLSID_DirectMusicSignPostTrack, sizeof( GUID ) );
	// Let the Segment fill m_guidEditorID in.
	//memcpy( &pNewTrack->m_guidEditorID, &CLSID_SignPostMgr, sizeof( GUID ) );
	//pNewTrack->dwPosition = 0;
	pNewTrack->m_dwGroupBits = 0x00000001;
	//pNewTrack->punkStripMgr = NULL;
	pNewTrack->m_ckid = DMUS_FOURCC_SIGNPOST_TRACK_CHUNK;
	//pNewTrack->m_fccType = 0;
	pNewTrack->SetStream( pStream );
	pStream->Release(); // Release our reference to the stream

	*ppTrack = pNewTrack;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CCommandList::DM_SaveCommandList()

HRESULT CCommandList::DM_SaveCommandList( interface IDMUSProdRIFFStream* pIRIFFStream )
{
	ASSERT( pIRIFFStream != NULL );
	if ( pIRIFFStream == NULL )
	{
		return E_INVALIDARG;
	}


	MMCKINFO ck;
	HRESULT hr;
	ck.ckid = DMUS_FOURCC_COMMANDTRACK_CHUNK;
	hr = pIRIFFStream->CreateChunk( &ck, 0 );
	if( hr != S_OK )
	{
		return hr;
	}

	IStream *pIStream;
    pIStream = pIRIFFStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		pIRIFFStream->Ascend( &ck, 0 );
		return E_INVALIDARG;
	}

	DWORD			cb;
	DWORD			dwSize;
	DMUS_IO_COMMAND	*pCommand;
	POSITION		position;

    dwSize = sizeof(DMUS_IO_COMMAND);
    hr = pIStream->Write(&dwSize, sizeof(dwSize), &cb);
    if(FAILED(hr) || cb != sizeof(dwSize))
    {
		pIRIFFStream->Ascend( &ck, 0 );
		pIStream->Release();
        return hr;
    }

	BYTE bLastGroove = 62; 
	position = m_lstDMCommands.GetHeadPosition();
	while(position)
	{
		pCommand = m_lstDMCommands.GetNext(position);

		// fix groove levels in the command list
		if (pCommand->bGrooveLevel == 0)
		{
			pCommand->bGrooveLevel = bLastGroove;
		}
		else bLastGroove = pCommand->bGrooveLevel;

		if( FAILED(pIStream->Write( pCommand, sizeof(DMUS_IO_COMMAND), &cb ) ) || 
			cb != sizeof(DMUS_IO_COMMAND) )
		{
			hr = E_FAIL;
			break;
		}
    }

	if( pIRIFFStream->Ascend( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	pIStream->Release();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandList::DM_SaveSignPostList()

HRESULT CCommandList::DM_SaveSignPostList( interface IDMUSProdRIFFStream* pIRIFFStream )
{
	ASSERT( pIRIFFStream != NULL );
	if ( pIRIFFStream == NULL )
	{
		return E_INVALIDARG;
	}

    MMCKINFO ck;
	HRESULT hr;
	ck.ckid = DMUS_FOURCC_SIGNPOST_TRACK_CHUNK;
	hr = pIRIFFStream->CreateChunk( &ck, 0 );
	if( hr != S_OK )
	{
		return hr;
	}

	IStream *pIStream;
    pIStream = pIRIFFStream->GetStream();
	ASSERT( pIStream != NULL );
	if ( pIStream == NULL )
	{
		pIRIFFStream->Ascend( &ck, 0 );
		return E_INVALIDARG;
	}

	DWORD			cb;
	DWORD			dwSize;
	DMUS_IO_SIGNPOST	*pSignPost;
	POSITION		position;

    dwSize = sizeof(DMUS_IO_SIGNPOST);
    hr = pIStream->Write(&dwSize, sizeof(dwSize), &cb);
    if(FAILED(hr) || cb != sizeof(dwSize))
    {
		pIRIFFStream->Ascend( &ck, 0 );
		pIStream->Release();
        return hr;
    }

	position = m_lstDMSignPosts.GetHeadPosition();
	while(position)
	{
		pSignPost = m_lstDMSignPosts.GetNext(position);
		if( FAILED(pIStream->Write( pSignPost, sizeof(DMUS_IO_SIGNPOST), &cb ) ) || 
			cb != sizeof(DMUS_IO_SIGNPOST) )
		{
			hr = E_FAIL;
			break;
		}
    }

	if( pIRIFFStream->Ascend( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
	}

	pIStream->Release();

    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CCommandList::LoadCommandList()
/*
HRESULT CCommandList::LoadCommandList( interface IDMUSProdRIFFStream* pIRIFFStream )
{
	ASSERT( m_lClocksPerMeasure != 0 );
	if ( m_lClocksPerMeasure == 0 )
	{
		return E_FAIL;
	}

    DWORD			cb;
    WORD			wSize;
    ioCommand		oCommand;
    CCommandStruct	*pCommand;
	HRESULT			hr = S_OK;
	LARGE_INTEGER	liTemp;

    hr = pIStream->Read(&wSize, sizeof(wSize), &cb);
    if(FAILED(hr) || cb != sizeof(wSize))
    {
        return E_FAIL; 
    }
	if(wSize > sizeof(ioCommand))
	{
		liTemp.QuadPart = wSize - sizeof(ioCommand);
	}
	else
	{
		liTemp.QuadPart = 0;
	}

	// Remove any commandstructs which don't have commands
	RemoveAll();

	hr = pIStream->Read(&oCommand, wSize, &cb);
	while(hr == S_OK && cb == wSize)
	{
		pCommand = new CCommandStruct();
		if(pCommand == NULL)
		{
			RemoveAll();
			return E_OUTOFMEMORY;
		}
		pCommand->m_nMeasure = oCommand.lTime / m_lClocksPerMeasure;
		pCommand->m_dwCommand = oCommand.dwCommand;
		pCommand->m_dwSignPost = 0;

		m_lstCommands.AddTail(pCommand);
		pIStream->Seek(liTemp, STREAM_SEEK_CUR, NULL);
		hr = pIStream->Read(&oCommand, wSize, &cb);
	}
	if(FAILED(hr))
	{
		RemoveAll();
		return E_FAIL;
	}

	*//*
    CCommandStruct	*pCommandTemp;
	CommandList		CommandList;
	POSITION		position, posTemp;
	posTemp = CommandList.m_list.GetHeadPosition();
	position = m_commandList.m_list.GetHeadPosition();
	if(posTemp != NULL)
	{
		pCommandTemp = CommandList.m_list.GetNext(posTemp);
	}
	else
	{
		pCommandTemp = NULL;
	}
	if(position != NULL)
	{
		pCommand = m_commandList.m_list.GetNext(position);
	}
	else
	{
		pCommand = NULL;
	}

	while(pCommandTemp != NULL && pCommand != NULL)
	{
		if(pCommand->m_nMeasure < pCommandTemp->m_nMeasure)
		{
			if(position == NULL)
			{
				// This is the last command in our list, and everything left in CommandList goes after it.
				pCommand = NULL;
				break;
			}
			pCommand = m_commandList.m_list.GetNext(position);
			continue;
		}
		// Remove pCommandTemp from the list.
		if(posTemp == NULL)
		{
			// Remove the tail
			CommandList.m_list.RemoveTail();
		}
		else
		{
			POSITION posBefore;

			// Set posBefore to point to pCommandTemp and remove it.
			posBefore = posTemp;
			CommandList.m_list.GetPrev(posBefore);
			CommandList.m_list.RemoveAt(posBefore);
		}
		if(pCommand->m_nMeasure == pCommandTemp->m_nMeasure)
		{
			pCommand->m_dwCommand = pCommandTemp->m_dwCommand;
			if(position != NULL)
			{
				pCommand = m_commandList.m_list.GetNext(position);
			}
			delete pCommandTemp;
		}
		else if(pCommand->m_nMeasure > pCommandTemp->m_nMeasure)
		{
			if(position == NULL)
			{
				position = m_commandList.m_list.GetTailPosition();
			}
			else
			{
				m_commandList.m_list.GetPrev(position);
			}
			m_commandList.m_list.InsertBefore(position, pCommandTemp);
			m_commandList.m_list.GetNext(position);
		}
		if(posTemp != NULL)
		{
			pCommandTemp = CommandList.m_list.GetNext(posTemp);
		}
		else
		{
			pCommandTemp = NULL;
		}
	}
	if(pCommandTemp != NULL)
	{
		ASSERT(pCommand == NULL);
		m_commandList.m_list.AddTail(&CommandList.m_list);
	}
	CommandList.m_list.RemoveAll();
	*//*
    return S_OK;
}
*/
void CCommandList::RemoveAll()
{
	if ( !m_lstDMCommands.IsEmpty() )
	{
		DMUS_IO_COMMAND	*pCommand;
		while( !m_lstDMCommands.IsEmpty() )
		{
			pCommand = m_lstDMCommands.RemoveHead();
			delete pCommand;
		}
	}
	if ( !m_lstDMSignPosts.IsEmpty() )
	{
		DMUS_IO_SIGNPOST	*pSignPost;
		while( !m_lstDMSignPosts.IsEmpty() )
		{
			pSignPost = m_lstDMSignPosts.RemoveHead();
			delete pSignPost;
		}
	}
}

/*
HRESULT CCommandList::SendCommandListToCommandMgr( LPUNKNOWN punkCommandMgr )
{
	if ( punkCommandMgr == NULL )
	{
		return E_INVALIDARG;
	}

	HRESULT			hr = E_FAIL;
	IPersistStream	*pIPS;
	IStream			*pStream;
	LARGE_INTEGER	liTemp;

	liTemp.QuadPart = 0;
	if(SUCCEEDED(CreateStreamOnHGlobal(NULL, TRUE, &pStream)))
	{
		if(SUCCEEDED(punkCommandMgr->QueryInterface(IID_IPersistStream, (void**)&pIPS)))
		{
			if(SUCCEEDED(SaveCommandList(pStream)))
			{
				pStream->Seek(liTemp, STREAM_SEEK_SET, NULL); //seek to beginning
				if(SUCCEEDED(pIPS->Load(pStream)))
				{
					hr = S_OK;
					//if(m_pSegmentDlg)
					//{
					//	m_pSegmentDlg->RefreshStrip(STRIP_COMMAND);
					//}
				}
			}
			pIPS->Release();
		}
		pStream->Release();
	}
	
	return hr;
}
*/