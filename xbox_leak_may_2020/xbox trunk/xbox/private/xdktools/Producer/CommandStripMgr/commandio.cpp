#include "stdafx.h"
#include "ChordIO.h"



CChordItem::CChordItem()

{
	m_lDrawPosition = 0;
	m_dwIndex = 0;
	m_fSelected = FALSE;
}

CChordItem::CChordItem(const CChordItem& chord)
{
	m_lDrawPosition = chord.m_lDrawPosition;
	m_dwIndex = chord.m_dwIndex;
	m_fSelected = chord.m_fSelected;
	*((CPropChord*)this) = chord;
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
		if (pChord->m_fSelected)
		{
			if (dwSelectCount == 0)
			{
				*pPropChord = *pChord;
				pPropChord->m_dwUndetermined = 0;
			}
			else
			{
				pChord->CopyToPropChord(pPropChord);
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
		if (pChord->m_fSelected)
		{
			pChord->CopyFromPropChord(pPropChord);
			dwSelectCount++;
		}
	}
	return dwSelectCount;
}

void CChordList::ClearSelections( )

{
	CChordItem* pChord = GetHead();
	for (;pChord != NULL; pChord = pChord->GetNext())
	{
		pChord->m_fSelected = FALSE;
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


void CChordList::InsertByAscendingTime(CChordItem* pChord)
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
	}
	else
	{
		AddTail(pChord);
	}
}

HRESULT CChordList::Load( LPSTREAM pStream )
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

        WideCharToMultiByte( CP_ACP, 0, iChordSelection.wstrName, -1, pChord->m_szName, sizeof( pChord->m_szName ), NULL, NULL );
        if( iChordSelection.fCSFlags & CSF_FLAT )
        {
            pChord->m_bFlat = 1;
        }
		if( iChordSelection.fCSFlags & CSF_SELECTED)
		{
			pChord->m_fSelected = 1;
		}
        pChord->m_bBeat = iChordSelection.bBeat;
        pChord->m_nMeasure = iChordSelection.wMeasure;
        pChord->m_dwChordPattern = iChordSelection.aChord[0].lChordPattern;
        pChord->m_dwScalePattern = iChordSelection.aChord[0].lScalePattern;
		pChord->m_bRoot = iChordSelection.aChord[0].bRoot;
        pChord->SetBits();
		pChord->m_dwIndex = dwIndex++;
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
    ioChordSelection iChordSelection;
    CChordItem*   pChord;
    int         i;

    wSize = sizeof( ioChordSelection );
//	FixBytes( FBT_SHORT, &wSize );
    hr = pStream->Write( &wSize, sizeof( wSize ), &cb );
    if( FAILED( hr ) || cb != sizeof( wSize ) )
    {
        return E_FAIL;
    }

	
    for( pChord = GetHead() ; pChord != NULL; pChord = pChord->GetNext() )
    {
        memset( &iChordSelection, 0, sizeof( iChordSelection ) );
        MultiByteToWideChar( CP_ACP, 0, pChord->m_szName, -1, iChordSelection.wstrName, sizeof( iChordSelection.wstrName ) / sizeof( wchar_t ) );
        if( pChord->m_bFlat != 0 )
        {
            iChordSelection.fCSFlags |= CSF_FLAT;
        }
		if( pChord->m_fSelected)
		{
			iChordSelection.fCSFlags |= CSF_SELECTED;
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
        iChordSelection.bBeat = pChord->m_bBeat;
        iChordSelection.wMeasure = pChord->m_nMeasure;
        for( i = 0 ; i < 4 ; ++i )
        {
            iChordSelection.aChord[i].lChordPattern = pChord->m_dwChordPattern;
            iChordSelection.aChord[i].lScalePattern = pChord->m_dwScalePattern;
/* not available in CChordItem
            iChordSelection.aChord[i].lInvertPattern = ~0;
*/
            iChordSelection.aChord[i].bRoot = pChord->m_bRoot;
        }
//		FixBytes( FBT_IOCHORDSELECTION, &iChordSelection );
        if( FAILED( pStream->Write( &iChordSelection, sizeof( iChordSelection), &cb ) ) ||
            cb != sizeof( iChordSelection ) )
        {
			hr = E_FAIL;
			break;
        }
    }
	return hr;
}
