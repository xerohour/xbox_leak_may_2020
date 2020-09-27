// SharedPattern.cpp : implementation file
//

#include "stdafx.h"
#include "Pattern.h"
#include "SharedPattern.h"
#include "DMUSProd.h"
#include "RiffStrm.h"

// Variation Choices bit associated with DirectMusic buttons
const static short sga_DM_AssociatedBit[DM_NBR_COLUMNS] =
		{ 0,7,14, 1,8,15, 2,9,16, 3,10,17, 4,11,18, 5,12,19, 6,13,20, 21, 22, 23, 24, 25, 26, 27, 28, 30 };

// Variation Choices bit associated with IMA buttons
const static short sga_IMA_AssociatedBit[IMA_NBR_COLUMNS] =
		{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 13, 14, 11, 12 };

const short* sga_nBtnBit[NBR_ROWTYPES] = {					// Associated dwVariationChoices bits
	&sga_IMA_AssociatedBit[0],
	&sga_DM_AssociatedBit[0] };

IStream *CreateStreamForVarChoices( IDMUSProdFramework *pIDMUSProdFramework, DWORD adwVariationChoices[NBR_VARIATIONS] )
{
	// Validate the parameter
	ASSERT( adwVariationChoices );
	ASSERT( pIDMUSProdFramework );
	if( adwVariationChoices == NULL
	||	pIDMUSProdFramework == NULL )
	{
		return NULL;
	}

	// Try and allocate the stream
	IStream *pStream = NULL;
	if( SUCCEEDED( pIDMUSProdFramework->AllocMemoryStream( FT_DESIGN, GUID_CurrentVersion, &pStream ) ) )
	{
		IDMUSProdRIFFStream* pIRiffStream;
		if( SUCCEEDED( AllocRIFFStream( pStream, &pIRiffStream ) ) )
		{
			// Prepare the chunk headers
			MMCKINFO ckMain, ckSubChunk;
			ckMain.fccType = DM_FOURCC_VARCHOICES_FORM;
			ckSubChunk.ckid = DM_FOURCC_VARCHOICES_CHUNK;

			// Prepare ioVarChoices structure
			ioVarChoices oVarChoices;
			memset( &oVarChoices, 0, sizeof(ioVarChoices) );
			memcpy( oVarChoices.m_dwVariationChoices, adwVariationChoices, sizeof(DWORD) * NBR_VARIATIONS );

			// Write out the data
			DWORD dwBytesWritten;
			if( pIRiffStream->CreateChunk( &ckMain, MMIO_CREATERIFF ) != 0
			||	pIRiffStream->CreateChunk( &ckSubChunk, 0 ) != 0
			||	FAILED( pStream->Write( &oVarChoices, sizeof(ioVarChoices), &dwBytesWritten) )
			||	sizeof(ioVarChoices) != dwBytesWritten
			||	pIRiffStream->Ascend( &ckSubChunk, 0 ) != 0
			||	pIRiffStream->Ascend( &ckMain, 0 ) != 0 )
			{
				pStream->Release();
				pStream = NULL;
			}
			pIRiffStream->Release();
		}
		else
		{
			pStream->Release();
			pStream = NULL;
		}
	}

	return pStream;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicStyleCurve::SetDefaultResetValues

void CDirectMusicStyleCurve::SetDefaultResetValues( MUSIC_TIME mtLength  )
{
	m_bFlags &= ~DMUS_CURVE_RESET;	
	m_nResetValue = 0;	
	m_mtResetDuration = 0;
	
	switch( m_bEventType )
	{
		case DMUS_CURVET_PBCURVE:
			m_bFlags |= DMUS_CURVE_RESET;	
			m_nResetValue = 8192;	
			m_mtResetDuration = mtLength;
			break;
		
		case DMUS_CURVET_CCCURVE:
			switch( m_bCCData )
			{
				case 1:		// Mod Wheel
				case 2:		// Breath Controller
				case 4:		// Foot Controller
				case 5:		// Portamento Time
				case 12:	// FX1
				case 13:	// FX2
				case 64:	// Sustain Pedal
				case 65:	// Portamento On/Off
				case 66:	// Sostenuto Pedal
				case 67:	// Soft Pedal
				case 68:	// Legato Footswitch
				case 69:	// Hold 2
					m_bFlags |= DMUS_CURVE_RESET;	
					m_nResetValue = 0;	
					m_mtResetDuration = mtLength;
					break;
			}
			break;

		case DMUS_CURVET_PATCURVE:
		case DMUS_CURVET_MATCURVE:
			m_bFlags |= DMUS_CURVE_RESET;	
			m_nResetValue = 0;	
			m_mtResetDuration = mtLength;
			break;

		case DMUS_CURVET_RPNCURVE:
			switch( m_wParamType )
			{
			case 0: // Pitch Bend range
				m_bFlags |= DMUS_CURVE_RESET;
				m_nResetValue = 2 << 7; // Want MSB set to 2 (Whole step)
				m_mtResetDuration = mtLength;
				break;
			/* No acceptable default values for any other RPN types
			default:
				break;
			*/
			}
			break;

		/* No acceptable default values
		case DMUS_CURVET_NRPNCURVE:
			break;
		*/
	}
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::IsVarChoicesRowDisabled

bool CDirectMusicPart::IsVarChoicesRowDisabled( short nRow )
{
	short nRowType = GetVarChoicesRowType( nRow );
    short nNbrColumns = GetVarChoicesNbrColumns( nRow );
	DWORD dwOffset;

	for( short nColumn = 0 ;  nColumn < nNbrColumns ;  nColumn++ )
    {
		dwOffset = sga_nBtnBit[nRowType][nColumn];

		if( m_dwVariationChoices[nRow] & (1 << dwOffset) )
		{
			return false;
		}
    }

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetVarChoicesRowType

short CDirectMusicPart::GetVarChoicesRowType( short nRow )
{
	return (short)((m_dwVariationChoices[nRow] & DM_VF_MODE_BITS) >> 29);
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPart::GetVarChoicesNbrColumns

short CDirectMusicPart::GetVarChoicesNbrColumns( short nRow )
{
	short nNbrColumns = 0;
	
	switch( GetVarChoicesRowType(nRow) )
	{
		case IMA_ROWTYPE:
			nNbrColumns = IMA_NBR_COLUMNS;
			break;

		case DM_ROWTYPE:
			nNbrColumns = DM_NBR_COLUMNS;
			break;
	}

	ASSERT( nNbrColumns != 0 );
	return nNbrColumns;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPartRef::DM_SavePartRefInfoList

HRESULT CDirectMusicPartRef::DM_SavePartRefInfoList( IDMUSProdRIFFStream* pIRiffStream ) const
{
	if( m_strName.IsEmpty() )
	{
		return S_OK;
	}

 	IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	HRESULT hr = S_OK;
    MMCKINFO ckMain;
    MMCKINFO ck;

	// Write INFO LIST header
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write PartRef name
	if( !m_strName.IsEmpty() )
	{
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::IsDirty

HRESULT CDirectMusicPattern::IsDirty()
{
	AFX_MANAGE_STATE(AfxGetStaticModuleState( ));

	if( m_fModified )
	{
		return S_OK;
	}

    return S_FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern IPersistStream::GetSizeMax

HRESULT CDirectMusicPattern::GetSizeMax( ULARGE_INTEGER FAR* pcbSize )
{
//	AFX_MANAGE_STATE(_afxModuleAddrThis);
	UNREFERENCED_PARAMETER(pcbSize);

    return E_NOTIMPL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::AllocPartRef

CDirectMusicPartRef* CDirectMusicPattern::AllocPartRef( void )
{
	CDirectMusicPartRef* pPartRef = new CDirectMusicPartRef( this );

	if( pPartRef )
	{
		// Add PartRef to Pattern's list of PartRefs
		m_lstPartRefs.AddTail( pPartRef );
	}

	return pPartRef;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DeletePartRef

void CDirectMusicPattern::DeletePartRef( CDirectMusicPartRef* pPartRef )
{
	ASSERT( pPartRef != NULL );

	// Remove PartRef from Pattern's PartRef list
	POSITION pos = m_lstPartRefs.Find( pPartRef );

	if( pos )
	{
		m_lstPartRefs.RemoveAt( pos );
	}

	pPartRef->SetPart( NULL );
	delete pPartRef;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SavePatternRhythm

HRESULT CDirectMusicPattern::DM_SavePatternRhythm( IDMUSProdRIFFStream* pIRiffStream )
{
	// Ensure we have a rhythm map to save
	if( m_pRhythmMap == NULL )
	{
		return S_FALSE;
	}

    IStream* pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Rhythm chunk header
    HRESULT hr = S_OK;
	MMCKINFO ck;
	ck.ckid = DMUS_FOURCC_RHYTHM_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Now save the RhythmMap for each measure
	DWORD dwBytesWritten;
	int i;
	for( i = 0 ;  i < m_wNbrMeasures ;  i++ )
	{
		// Write Rhythm chunk data
		hr = pIStream->Write( &m_pRhythmMap[i], sizeof(DWORD), &dwBytesWritten);
		if( FAILED( hr )
		||  dwBytesWritten != sizeof(DWORD) )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SavePatternInfoList

HRESULT CDirectMusicPattern::DM_SavePatternInfoList( IDMUSProdRIFFStream* pIRiffStream )
{
	if( m_strName.IsEmpty() )
	{
		return S_OK;
	}

	IStream* pIStream;
    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write INFO LIST header
	HRESULT hr = S_OK;
    MMCKINFO ckMain;
	ckMain.fccType = DMUS_FOURCC_UNFO_LIST;
	if( pIRiffStream->CreateChunk(&ckMain, MMIO_CREATELIST) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Write Pattern name
	if( !m_strName.IsEmpty() )
	{
		MMCKINFO ck;
		ck.ckid = DMUS_FOURCC_UNAM_CHUNK;
		if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}

		hr = SaveMBStoWCS( pIStream, &m_strName );
		if( FAILED( hr ) )
		{
			goto ON_ERROR;
		}

		if( pIRiffStream->Ascend(&ck, 0) != 0 )
		{
			hr = E_FAIL;
			goto ON_ERROR;
		}
	}

	if( pIRiffStream->Ascend(&ckMain, 0) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
    pIStream->Release();
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::DM_SaveMotifSettingsChunk

HRESULT CDirectMusicPattern::DM_SaveMotifSettingsChunk( IDMUSProdRIFFStream* pIRiffStream )
{
    IStream* pIStream;
    HRESULT hr;
    MMCKINFO ck;
	DWORD dwBytesWritten;
	DMUS_IO_MOTIFSETTINGS dmusMotifSettingsIO;

	// Only save if this Pattern is a Motif
	if( !(m_wEmbellishment & EMB_MOTIF) )
	{
		return S_OK;
	}

    pIStream = pIRiffStream->GetStream();
	ASSERT( pIStream != NULL );

	// Write Motif Settings chunk header
    ck.ckid = DMUS_FOURCC_MOTIFSETTINGS_CHUNK;
    if( pIRiffStream->CreateChunk( &ck, 0 ) != 0 )
	{
		hr = E_FAIL;
		goto ON_ERROR;
	}

	// Prepare DMUS_IO_MOTIFSETTINGS
	memset( &dmusMotifSettingsIO, 0, sizeof(DMUS_IO_MOTIFSETTINGS) );

	ASSERT( m_mtLoopStart >= 0 );  
	ASSERT( m_mtLoopEnd > m_mtLoopStart );  
	ASSERT( m_mtLoopEnd <= CalcLength() );  

	dmusMotifSettingsIO.dwRepeats = m_dwRepeats;
	dmusMotifSettingsIO.mtPlayStart = m_mtPlayStart;
	dmusMotifSettingsIO.mtLoopStart = m_mtLoopStart;
	dmusMotifSettingsIO.mtLoopEnd = m_mtLoopEnd;
	dmusMotifSettingsIO.dwResolution = m_dwResolution;

	// Write Motif Settings chunk data
	hr = pIStream->Write( &dmusMotifSettingsIO, sizeof(DMUS_IO_MOTIFSETTINGS), &dwBytesWritten);
	if( FAILED( hr )
	||  dwBytesWritten != sizeof(DMUS_IO_MOTIFSETTINGS) )
	{
        hr = E_FAIL;
        goto ON_ERROR;
	}
	
	if( pIRiffStream->Ascend( &ck, 0 ) != 0 )
	{
 		hr = E_FAIL;
		goto ON_ERROR;
	}

ON_ERROR:
	RELEASE( pIStream );
    return hr;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::FindPart

CDirectMusicPart* CDirectMusicPattern::FindPart( DWORD dwChannelID ) const
{
    POSITION pos = m_lstPartRefs.GetHeadPosition();

    while( pos )
    {
        CDirectMusicPartRef *pPartRef = m_lstPartRefs.GetNext( pos );

		if( dwChannelID == pPartRef->m_dwPChannel )
		{
			return pPartRef->m_pDMPart;
		}
    }

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CDirectMusicPattern::FindPartRefByPChannel

CDirectMusicPartRef* CDirectMusicPattern::FindPartRefByPChannel( DWORD dwPChannel ) const
{
	POSITION pos = m_lstPartRefs.GetHeadPosition();

	while( pos )
	{
		CDirectMusicPartRef* pPartRef = m_lstPartRefs.GetNext( pos );

		if( pPartRef->m_dwPChannel == dwPChannel )
		{
			return pPartRef;
		}
	}

	return NULL;
}
