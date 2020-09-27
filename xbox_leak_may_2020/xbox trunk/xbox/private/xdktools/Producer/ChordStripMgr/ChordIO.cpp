#include "stdafx.h"
#include "ChordIO.h"
#include "riffstrm.h"
#include "chord.h"
#include <dmusici.h>

CChordItem::CChordItem()

{
//	TRACE("New ChordItem, addr = %p\n");
	m_lDrawPosition = 0;
	m_dwIndex = 0;
	m_fSelected = 0;
}

CChordItem::CChordItem(const CChordItem& chord)
{
//	TRACE("New ChordItem, addr = %p\n");
	m_lDrawPosition = chord.m_lDrawPosition;
	m_dwIndex = chord.m_dwIndex;
	m_fSelected = chord.m_fSelected;
	*((CPropChord*)this) = chord;
}

CChordItem::CChordItem(const ChordEntry& chord)
{
//	TRACE("New ChordItem, addr = %p\n");
	m_lDrawPosition = 0;
	m_dwIndex = 0;
	m_fSelected = 0;
	SetSelected(const_cast<ChordEntry&>(chord).m_chordsel.KeyDown());
	CPropChord* pProp = dynamic_cast<CPropChord*>(this);
	const_cast<ChordEntry&>(chord).SaveToPropChord(*pProp);
}

CChordItem::CChordItem(const ChordSelection& chord)
{
//	TRACE("New ChordItem, addr = %p\n");
	m_lDrawPosition = 0;
	m_dwIndex = 0;
	m_fSelected = 0;
	ChordEntry chordentry;
	chordentry.m_chordsel = chord;
	SetSelected(const_cast<ChordSelection&>(chord).KeyDown());
	CPropChord* pProp = dynamic_cast<CPropChord*>(this);
	const_cast<ChordEntry&>(chordentry).SaveToPropChord(*pProp);
}


DWORD CChordList::PreparePropChord( CPropChord *pPropChord )

/*	Scan through the chord list and combine all chords that have
	m_fSelected set into the one chord for the property page.
*/

{
	DWORD dwSelectCount = 0;	// How many are selected.
	memset( (void *) pPropChord, 0, sizeof (CPropChord));
	CChordItem* pChord = GetHead();
	for (;pChord != NULL; pChord = pChord->GetNext())
	{
		if (pChord->IsSelected())
		{
			if (dwSelectCount == 0)
			{
				*pPropChord = *pChord;
				pPropChord->SetUndetermined(-1, 0);
			}
			else
			{
				pChord->CopyToPropChord(pPropChord, CPropChord::ALL);
			}
			dwSelectCount++;
		}
	}
	return dwSelectCount;
}

DWORD CChordList::RetrievePropChord( CPropChord *pPropChord )

{
	DWORD dwSelectCount = 0;	// How many are selected.
	CChordItem* pChord = GetHead();
	for (;pChord != NULL; pChord = pChord->GetNext())
	{
		if (pChord->IsSelected())
		{
			pChord->CopyFromPropChord(pPropChord, CPropChord::ALL);
			dwSelectCount++;
		}
	}
	return dwSelectCount;
}

void CChordList::InsertByAscendingTime(CChordItem* pChord, bool bReplaceCoincident)
{
	ASSERT(pChord);
	CChordItem* pItem = GetHead();
	while(pItem && *pItem < *pChord)
	{
		pItem = pItem->GetNext();
	}
	if(pItem)
	{
		InsertBefore(pItem, pChord);
		if(bReplaceCoincident && pItem->IsCoincident(*pChord))
		{
			Remove(pItem);
			delete pItem;
		}
	}
	else
	{
		AddTail(pChord);
	}
}

void CChordList::ClearSelections( )

{
	CChordItem* pChord = GetHead();
	for (;pChord != NULL; pChord = pChord->GetNext())
	{
		pChord->SetSelected(FALSE);
	}
}

void CChordList::ClearList()
{
	CChordItem* pChord;
	while( pChord = RemoveHead() )
	{
		delete pChord;
	}
}


HRESULT CChordList::Load( LPSTREAM pStream )
{
	HRESULT		hr = S_OK;
	DWORD		cb;
	DWORD		cSize;
    CChordItem*		pChord;
    ioOldChordSelection iChordSelection;
    WORD        wSizeChord;
	STATSTG		statstg;
	DWORD		dwIndex = 0;

	if( NULL == pStream )
	{
		return E_POINTER;
	}
	if( hr = FAILED( pStream->Stat( &statstg, STATFLAG_NONAME ) ))
	{
		return hr;
	}

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{//Changed ECW 4/24/98
		pPersistInfo->GetStreamInfo( &StreamInfo );
		if(pPersistInfo)
		{
			ftFileType = StreamInfo.ftFileType;
			guidDataFormat = StreamInfo.guidDataFormat;
			pPersistInfo->Release();
		}
	}


	if( IsEqualGUID( guidDataFormat, GUID_CurrentVersion ))
	{
		return	DMLoad(pStream);
	}

	ClearList();
	cSize = statstg.cbSize.LowPart;

//    cSize = pck->cksize;
//	*plstChord = NULL;
	// load size of chord structure
    hr = pStream->Read( &wSizeChord, sizeof( wSizeChord ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSizeChord ) )
	{
        hr = E_FAIL;
		goto ON_ERR;
	}
	cSize -= cb;
	while( cSize )
	{
        if( wSizeChord > sizeof( ioOldChordSelection ) )
        {
            hr = pStream->Read( &iChordSelection, sizeof( ioOldChordSelection ), &cb );
            if( FAILED( hr ) || cb != sizeof( ioOldChordSelection ) )
            {
				hr = E_FAIL;
				break;
            }
            StreamSeek( pStream, wSizeChord - sizeof( ioOldChordSelection ), STREAM_SEEK_CUR );
        }
        else
        {
            hr = pStream->Read( &iChordSelection, wSizeChord, &cb );
            if( FAILED( hr ) || cb != wSizeChord )
            {
				hr = E_FAIL;
				break;
            }
        }
        cSize -= wSizeChord;

        pChord = new CChordItem;

        if( pChord == NULL )
		{
            hr = E_OUTOFMEMORY;
            goto ON_ERR;
        }

		memset(pChord->Name(), 0, DMPolyChord::MAX_NAME);
        WideCharToMultiByte( CP_ACP, 0, iChordSelection.wstrName, -1, pChord->Name(), DMPolyChord::MAX_NAME, NULL, NULL );
        if( iChordSelection.fCSFlags & CSF_FLAT )
        {
            pChord->Base()->UseFlat() = 1;
        }
		if( iChordSelection.fCSFlags & CSF_SELECTED)
		{
			pChord->SetSelected(TRUE);
		}
		else
		{
			pChord->SetSelected(FALSE);
		}
		if( iChordSelection.fCSFlags & CSF_SIGNPOST)
		{
			pChord->SetSignPost(TRUE);
		}
		else
		{
			pChord->SetSignPost(FALSE);
		}
		pChord->Beat() = iChordSelection.bBeat;
		pChord->Measure() = iChordSelection.wMeasure;
		pChord->Base()->ChordPattern() = iChordSelection.aChord[0].lChordPattern;
		pChord->Base()->ScalePattern() = iChordSelection.aChord[0].lScalePattern;
		pChord->Base()->InvertPattern() = iChordSelection.aChord[0].lInvertPattern;
		pChord->Base()->Flags() = iChordSelection.aChord[0].wCFlags;
		pChord->Base()->ChordRoot() = iChordSelection.aChord[0].bRoot;
		pChord->Base()->ScaleRoot() = pChord->Base()->ChordRoot();
        pChord->SetBits(pChord->RootIndex());
		pChord->m_dwIndex = dwIndex++;
		pChord->PropagateAll();	// legacy: set all subchords to base chord
		AddTail(pChord);
    }

ON_ERR:
    return hr;
}

// Save the chordlist to a normal stream
HRESULT CChordList::Save( LPSTREAM pStream, BOOL bClearDirty )
{
    HRESULT     hr;
    DWORD       cb;
    WORD        wSize;
    ioOldChordSelection iChordSelection;
    CChordItem*   pChord;
    int         i;

	DMUSProdStreamInfo	StreamInfo;//Added ECW 4/24/98
	FileType ftFileType = FT_RUNTIME;
	GUID guidDataFormat = GUID_CurrentVersion;

	// Get additional stream information
	IDMUSProdPersistInfo* pPersistInfo;

	if( SUCCEEDED ( pStream->QueryInterface( IID_IDMUSProdPersistInfo, (void **)&pPersistInfo ) ) )
	{//Changed ECW 4/24/98
		pPersistInfo->GetStreamInfo( &StreamInfo );
		if(pPersistInfo)
		{
			ftFileType = StreamInfo.ftFileType;
			guidDataFormat = StreamInfo.guidDataFormat;
			pPersistInfo->Release();
		}
	}


	if( IsEqualGUID( guidDataFormat, GUID_CurrentVersion ))
	{
		return	DMSave(pStream, bClearDirty);
	}

    wSize = sizeof( ioOldChordSelection );
    hr = pStream->Write( &wSize, sizeof( wSize ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSize ) )
    {
        return E_FAIL;
    }

	
    for( pChord = GetHead() ; pChord != NULL; pChord = pChord->GetNext() )
    {
        memset( &iChordSelection, 0, sizeof( ioOldChordSelection ) );
        MultiByteToWideChar( CP_ACP, 0, pChord->Name(), -1, iChordSelection.wstrName, sizeof( iChordSelection.wstrName ) / sizeof( wchar_t ) );
        if( pChord->Base()->UseFlat() != 0 )
        {
            iChordSelection.fCSFlags |= CSF_FLAT;
        }
		if( pChord->IsSelected())
		{
			iChordSelection.fCSFlags |= CSF_SELECTED;
		}
		if( pChord->IsSignPost())
		{
			iChordSelection.fCSFlags |= CSF_SIGNPOST;
		}
/* not available in CChordItem
        if( pChord->inscale != 0 )
        {
            iChordSelection.fCSFlags |= CSF_INSCALE;
        }
        if( pChord->keydown != 0 )
        {
            iChordSelection.fCSFlags |= CSF_KEYDOWN;
        }
        if( ( pChord->bits & CHORD_SIMPLE ) != 0 )
        {
            iChordSelection.fCSFlags |= CSF_SIMPLE;
        }
*/
        iChordSelection.bBeat = pChord->Beat();
        iChordSelection.wMeasure = pChord->Measure();
        for( i = 0 ; i < 4 ; ++i )
        {
            iChordSelection.aChord[i].lChordPattern = pChord->Base()->ChordPattern();
            iChordSelection.aChord[i].lScalePattern = pChord->Base()->ScalePattern();
/* not available in CChordItem
            iChordSelection.aChord[i].lInvertPattern = ~0;
*/
            iChordSelection.aChord[i].bRoot = pChord->Base()->ChordRoot();
        }
        if( FAILED( pStream->Write( &iChordSelection, sizeof( iChordSelection), &cb ) ) ||
            cb != sizeof( iChordSelection ) )
        {
			hr = E_FAIL;
			break;
        }
    }
	return hr;
}


void CChordList::SetPatternEditorMode( BYTE bPatternEditorMode )
{
	m_bPatternEditorMode = bPatternEditorMode;
}


HRESULT CChordList::DMLoad(IStream* pStream)
{
	HRESULT		hr = S_OK;
	DWORD		cb;
	DWORD		cSize;
    CChordItem*		pChord;
    ioChordSelection iChordSelection;
    WORD        wSizeChord;
	STATSTG		statstg;
	DWORD		dwIndex = 0;

	if( NULL == pStream )
	{
		return E_POINTER;
	}
	if( hr = FAILED( pStream->Stat( &statstg, STATFLAG_NONAME ) ))
	{
		return hr;
	}

	ClearList();
	cSize = statstg.cbSize.LowPart;

//    cSize = pck->cksize;
//	*plstChord = NULL;
	// load size of chord structure
    hr = pStream->Read( &wSizeChord, sizeof( wSizeChord ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSizeChord ) )
	{
        hr = E_FAIL;
		goto ON_ERR;
	}
	cSize -= cb;
	while( cSize )
	{
        if( wSizeChord > sizeof( ioChordSelection ) )
        {
            hr = pStream->Read( &iChordSelection, sizeof( ioChordSelection ), &cb );
            if( FAILED( hr ) || cb != sizeof( ioChordSelection ) )
            {
				hr = E_FAIL;
				break;
            }
            StreamSeek( pStream, wSizeChord - sizeof( ioChordSelection ), STREAM_SEEK_CUR );
        }
        else
        {
            hr = pStream->Read( &iChordSelection, wSizeChord, &cb );
            if( FAILED( hr ) || cb != wSizeChord )
            {
				hr = E_FAIL;
				break;
            }
        }
        cSize -= wSizeChord;

        pChord = new CChordItem;

        if( pChord == NULL )
		{
            hr = E_OUTOFMEMORY;
            goto ON_ERR;
        }

		memset(pChord->Name(), 0, DMPolyChord::MAX_NAME);		
        WideCharToMultiByte( CP_ACP, 0, iChordSelection.wstrName, -1, pChord->Name(), DMPolyChord::MAX_NAME, NULL, NULL );

		if( iChordSelection.bKeydown)
		{
			pChord->SetSelected(TRUE);
		}
		else
		{
			pChord->SetSelected(FALSE);
		}
		if( iChordSelection.fCSFlags & CSF_SIGNPOST)
		{
			pChord->SetSignPost(TRUE);
		}
		else
		{
			pChord->SetSignPost(FALSE);
		}

		pChord->m_dwIndex = dwIndex++;
		pChord->Beat() = iChordSelection.bBeat;
		pChord->Measure() = iChordSelection.nMeasure;
		pChord->Flags() = iChordSelection.bFlags;

		if( m_bPatternEditorMode != 0xFF )
		{
			// DMUS_CHORDKEYF_SILENT does not apply to chords in pattern editor
			pChord->Flags() &= ~DMUS_CHORDKEYF_SILENT;
		}

		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			DMChord* pDM = pChord->SubChord(i);
			pDM->ChordPattern() = iChordSelection.aChord[i].dwChordPattern;
			pDM->ScalePattern() = iChordSelection.aChord[i].dwScalePattern;
			pDM->InvertPattern() = iChordSelection.aChord[i].dwInvertPattern;
			pDM->ChordRoot() = iChordSelection.aChord[i].bChordRoot;
			pDM->ScaleRoot() = iChordSelection.aChord[i].bScaleRoot;
			pDM->Flags() = iChordSelection.aChord[i].wFlags;
			pDM->Levels() = iChordSelection.aChord[i].dwLevels;
			pDM->UseFlat() = iChordSelection.aChord[i].bFlat;
			pDM->InUse() = iChordSelection.aChord[i].bInUse;
			pDM->Bits() = iChordSelection.aChord[i].bBits;
		}
		
		AddTail(pChord);
    }

ON_ERR:
    return hr;
}

HRESULT CChordList::DMSave(IStream* pStream, BOOL bClearDirty)
{
	UNREFERENCED_PARAMETER(bClearDirty);
    HRESULT     hr;
    DWORD       cb;
    WORD        wSize;
    ioChordSelection iChordSelection;
    CChordItem*   pChord;
    int         i;

    wSize = sizeof( ioChordSelection );
    hr = pStream->Write( &wSize, sizeof( wSize ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSize ) )
    {
        return E_FAIL;
    }

	
    for( pChord = GetHead() ; pChord != NULL; pChord = pChord->GetNext() )
    {
        memset( &iChordSelection, 0, sizeof( iChordSelection ) );
        MultiByteToWideChar( CP_ACP, 0, pChord->Name(), -1, iChordSelection.wstrName, sizeof( iChordSelection.wstrName ) / sizeof( wchar_t ) );
        if( pChord->Base()->UseFlat() != 0 )
        {
            iChordSelection.fCSFlags |= CSF_FLAT;
        }
		if( pChord->IsSelected())
		{
			iChordSelection.bKeydown = TRUE;
		}
		else
		{
			iChordSelection.bKeydown = FALSE;
		}
		if( pChord->IsSignPost())
		{
			iChordSelection.fCSFlags |= CSF_SIGNPOST;
		}
        iChordSelection.bBeat = pChord->Beat();
        iChordSelection.nMeasure = pChord->Measure();
        iChordSelection.bFlags = pChord->Flags();
        for( i = 0 ; i < DMPolyChord::MAX_POLY ; ++i )
        {
			DMChord* pDM = pChord->SubChord(i);
            iChordSelection.aChord[i].dwChordPattern = pDM->ChordPattern();
            iChordSelection.aChord[i].dwScalePattern = pDM->ScalePattern();
			iChordSelection.aChord[i].dwInvertPattern = pDM->InvertPattern();
            iChordSelection.aChord[i].bChordRoot = pDM->ChordRoot();
			iChordSelection.aChord[i].bScaleRoot = pDM->ScaleRoot();
			iChordSelection.aChord[i].dwLevels = pDM->Levels();
			iChordSelection.aChord[i].bFlat = pDM->UseFlat();
			iChordSelection.aChord[i].bInUse = pDM->InUse();
			iChordSelection.aChord[i].wFlags = pDM->Flags();
			iChordSelection.aChord[i].bBits = pDM->Bits();
        }
        if( FAILED( pStream->Write( &iChordSelection, sizeof( iChordSelection), &cb ) ) ||
            cb != sizeof( iChordSelection ) )
        {
			hr = E_FAIL;
			break;
        }
    }
	return hr;
}

void CChordList::Transpose(short deltaKey)
{
	CChordItem* pchord = 0;
	for(pchord=GetHead(); pchord; pchord = pchord->GetNext())
	{
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			short root = pchord->SubChord(i)->ChordRoot() + deltaKey;
			if(root < 0)
				root += 12;
			if(root > 23)
				root -= 12;
			pchord->SubChord(i)->ChordRoot() = static_cast<unsigned char>(root);
			pchord->SubChord(i)->ScalePattern() = Rotate24(pchord->SubChord(i)->ScalePattern(), deltaKey);
		}
	}
}

void CChordList::UseFlats(bool bUseFlats)
{
	CChordItem* pchord = 0;
	for(pchord=GetHead(); pchord; pchord = pchord->GetNext())
	{
		for(int i = 0; i < DMPolyChord::MAX_POLY; i++)
		{
			pchord->SubChord(i)->UseFlat() = bUseFlats;
		}

	}
}

