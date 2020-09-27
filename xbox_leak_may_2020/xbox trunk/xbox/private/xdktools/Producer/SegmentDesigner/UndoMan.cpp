// CUndoNode implementation file

#include "stdafx.h"
#include "UndoMan.h"
#include "DMUSProd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CUndoNode::CUndoNode()
{
	// initialize member variables
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pIMemStream = NULL;
	m_nUndoLevel = DEFAULT_UNDO_LEVEL;
}
CUndoNode::~CUndoNode()
{
	// release the memory stream
	if( m_pIMemStream )  {
		m_pIMemStream->Release();
}	}

CUndoNode  *CUndoNode::SaveState( IPersistStream *pIPStream, const char *pszText, IDMUSProdFramework *pIFramework )
{
	CUndoNode  *pDel, *pPrev;
	short n;

	// delete all redo nodes from this point
	while( m_pNext )
	{
		pDel = m_pNext;
		m_pNext = m_pNext->m_pNext;
		delete pDel;
	}

	// scan the list backward until no more nodes or undo level is reached
	pPrev = m_pPrev;
	n = 0;
	while( pPrev  &&  n < m_nUndoLevel )
	{
		pPrev = pPrev->m_pPrev;
		n++;
	}
	// terminate the new head
	if( pPrev )  {
		if( pPrev->m_pNext )  {
			pPrev->m_pNext->m_pPrev = NULL;
	}	}

	// delete oldest undo nodes past max level
	while( pPrev )
	{
		pDel = pPrev;
		pPrev = pPrev->m_pPrev;
		delete pDel;
	}

	// create a new node
	m_pNext = new CUndoNode;

	if( m_pNext )
	{
		// attach the new node to the list
		m_pNext->m_pPrev = this;
		m_pNext->m_pNext = NULL;

		// copy the text string
		lstrcpyn( m_pNext->m_szMenuText, pszText, sizeof(m_szMenuText) - 1 );

		// set the undo level to the same
		m_pNext->m_nUndoLevel = m_nUndoLevel;

		// create a memory stream
		if( pIFramework &&
			SUCCEEDED( pIFramework->AllocMemoryStream(FT_DESIGN, GUID_CurrentVersion, &m_pNext->m_pIMemStream) ) )
		{
			// write the current data to it, leave it dirty
			pIPStream->Save( m_pNext->m_pIMemStream, FALSE );
			return m_pNext;
		}

		// Either no framework pointer, or unable to allocate stream with framework
		if( SUCCEEDED( CreateStreamOnHGlobal( NULL, TRUE, &m_pNext->m_pIMemStream ) ) )
		{
			// write the current data to it, leave it dirty
			pIPStream->Save( m_pNext->m_pIMemStream, FALSE );
			return m_pNext;
		}
		delete m_pNext;
	}
	return this;
}

CUndoNode  *CUndoNode::Undo( IPersistStream *pIPStream, IDMUSProdFramework *pIFramework )
{
	// check for valid undo
#ifdef _DEBUG
	ASSERT(m_pPrev);
#endif
	if( m_pPrev == NULL )  {
		return this;
	}

	// save the current state if there is no redo
	if( m_pNext == NULL )  {
		SaveState( pIPStream, "!", pIFramework );
	}

	// rewind the memory stream and load it
	LARGE_INTEGER largeint;
	largeint.LowPart = 0;
	largeint.HighPart = 0;
	m_pIMemStream->Seek( largeint, STREAM_SEEK_SET, NULL );
	if( SUCCEEDED(pIPStream->Load( m_pIMemStream )) )
	{
		// return the previous node
		return m_pPrev;
	}
	return this;
}

CUndoNode  *CUndoNode::Redo( IPersistStream *pIPStream )
{
	// check for a valid redo node
#ifdef _DEBUG
	ASSERT(m_pNext);
	ASSERT(m_pNext->m_pNext);
	ASSERT(pIPStream);
#endif
	if( m_pNext == NULL )  {
		return this;
	}
	if( m_pNext->m_pNext == NULL )  {
		return this;
	}

	LARGE_INTEGER largeint;
	largeint.LowPart = 0;
	largeint.HighPart = 0;

	// must skip next, which is the action we just undid
	m_pNext->m_pNext->m_pIMemStream->Seek( largeint, STREAM_SEEK_SET, NULL );
	if( SUCCEEDED(pIPStream->Load( m_pNext->m_pNext->m_pIMemStream )) )
	{
		// return the next node
		return m_pNext;
	}
	return this;
}

BOOL  CUndoNode::GetUndo( char *pszText, int nMaxCount )
{
	if( m_pPrev == NULL )
	{
		// nothing to undo
		return FALSE;
	}
	// copy the undo text
	lstrcpyn( pszText, m_szMenuText, nMaxCount );
	return TRUE;
}

BOOL  CUndoNode::GetRedo( char *pszText, int nMaxCount )
{
	if( m_pNext )
	{
		// check the next of next for validity
		if( m_pNext->m_pNext )  {
			lstrcpyn( pszText, m_pNext->m_szMenuText, nMaxCount );
			return TRUE;
	}	}
	// nothing to redo
	return FALSE;
}

void  CUndoNode::Destroy()
{
	CUndoNode  *pDel;

	// delete all redo nodes from this
	while( m_pNext )
	{
		pDel = m_pNext;
		m_pNext = m_pNext->m_pNext;
		delete pDel;
	}

	// delete all undo nodes from this
	while( m_pPrev )
	{
		pDel = m_pPrev;
		m_pPrev = m_pPrev->m_pPrev;
		delete pDel;
	}
	delete this;
}

void  CUndoNode::SetUndoLevel( long nUndoLevel )
{
	CUndoNode  *pScan = m_pNext;

	// set level for all redo nodes from this
	while( pScan )  {
		pScan->m_nUndoLevel = nUndoLevel;
		pScan = pScan->m_pNext;
	}
	// set level for all undo nodes from this
	pScan = m_pPrev;
	while( pScan )  {
		pScan->m_nUndoLevel = nUndoLevel;
		pScan = pScan->m_pPrev;
	}
	m_nUndoLevel = nUndoLevel;
}

