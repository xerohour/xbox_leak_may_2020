// PChannelGroup.cpp : implementation file
//

#include "stdafx.h"
#include "ToolGraphDesignerDLL.h"
#include "Graph.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// CPChannelGroup Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPChannelGroup::CPChannelGroup( void )
{
	m_dwNbrPChannels = 0;
	m_pdwPChannel = NULL;
}

CPChannelGroup::~CPChannelGroup()
{
	if( m_pdwPChannel )
	{
		delete [] m_pdwPChannel;
	}
	m_dwNbrPChannels = 0;

	// Cleanup references
	CTool* pTool;
	while( !m_lstTools.IsEmpty() )
	{
		pTool = m_lstTools.RemoveHead();
		RELEASE( pTool );
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelGroup::FormatPChannelText

void CPChannelGroup::FormatPChannelText( CString& strText )
{
	strText.Empty();

	ASSERT( m_dwNbrPChannels > 0 );

	DWORD dwLastPChannel = m_pdwPChannel[0];
	bool fInRange = false;

	TCHAR achTemp2[20];
	TCHAR achNum[10];

	_itot( m_pdwPChannel[0] + 1, achNum, 10 );
	strText = achNum;

	for( DWORD i = 1 ;  i < m_dwNbrPChannels ;  i++ )
	{
		if( (dwLastPChannel + 1) == m_pdwPChannel[i] )
		{
			dwLastPChannel++;
			fInRange = true;
		}
		else
		{
			if( fInRange )
			{
				// No longer in range
				_itot( dwLastPChannel + 1, achNum, 10 );
				_tcscpy( achTemp2, "-" );
				_tcscat( achTemp2, achNum );
				_tcscat( achTemp2, ", " );
				_itot( m_pdwPChannel[i] + 1, achNum, 10 );
				_tcscat( achTemp2, achNum );
				strText += achTemp2;
				dwLastPChannel = m_pdwPChannel[i];
				fInRange = false;
			}
			else
			{
				// No longer in range
				_itot( m_pdwPChannel[i] + 1, achNum, 10 );
				_tcscpy( achTemp2, ", " );
				_tcscat( achTemp2, achNum );
				strText += achTemp2;
				dwLastPChannel = m_pdwPChannel[i];
			}
		}
	}

	if( fInRange )
	{
		_itot( dwLastPChannel + 1, achNum, 10 );
		_tcscpy( achTemp2, "-" );
		_tcscat( achTemp2, achNum );
		strText += achTemp2;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CPChannelGroup::InsertTool

void CPChannelGroup::InsertTool( CDirectMusicGraph* pGraph, CTool* pToolToInsert )
{
	if( pGraph == NULL 
	||  pToolToInsert == NULL )
	{
		ASSERT( FALSE );
		return;
	}

	ASSERT( m_dwNbrPChannels > 0 );
	ASSERT( m_lstTools.Find( pToolToInsert ) == NULL );

	int nIndexToolToInsert = pGraph->ToolToIndex( pToolToInsert );

	CTool* pTool;
	POSITION posCurrent, posNext = m_lstTools.GetHeadPosition();

	while( posNext )
	{
		posCurrent = posNext;
		pTool = m_lstTools.GetNext( posNext );

		if( pGraph->ToolToIndex(pTool) > nIndexToolToInsert )
		{
			pToolToInsert->AddRef();
			m_lstTools.InsertBefore( posCurrent, pToolToInsert );
			return;
		}
	}

	pToolToInsert->AddRef();
	m_lstTools.AddTail( pToolToInsert );
}
