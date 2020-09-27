// CJazzUndoNode implementation file

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxctl.h>         // MFC support for ActiveX Controls
#include <afxcmn.h>

#include "DMUSProd.h"
#include "JazzUndoMan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CJazzUndoNode::CJazzUndoNode()
{
	// initialize member variables
	m_pPrev = NULL;
	m_pNext = NULL;
	m_pIMemStream = NULL;
	m_nUndoLevel = DEFAULT_UNDO_LEVEL;

}
CJazzUndoNode::~CJazzUndoNode()
{
	// release the memory stream
	if( m_pIMemStream )  {
		m_pIMemStream->Release();
}	}

CJazzUndoNode  *CJazzUndoNode::SaveState(IDMUSProdFramework* pFramework, 
										 FileType ftFileType,
										 GUID& guidDataFormat,
										 IPersistStream *pIPStream, char *pszText )
{
	CJazzUndoNode  *pDel, *pPrev;
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
	m_pNext = new CJazzUndoNode();

	if( m_pNext )
	{
		// attach the new node to the list
		m_pNext->m_pPrev = this;
		m_pNext->m_pNext = NULL;

		// copy the text string
		lstrcpyn( m_pNext->m_szMenuText, pszText, sizeof(m_szMenuText) - 1 );

		// set the undo level to the same
		m_pNext->m_nUndoLevel = m_nUndoLevel;

		// create a memory stream using state of previous stream
		if( SUCCEEDED( pFramework->AllocMemoryStream(ftFileType, guidDataFormat, &m_pNext->m_pIMemStream)))
		{
			// write the current data to it, leave it dirty
			pIPStream->Save( m_pNext->m_pIMemStream, FALSE );
			return m_pNext;
		}
		delete m_pNext;
	}
	return this;
}

CJazzUndoNode  *CJazzUndoNode::Undo(IDMUSProdFramework* pFramework, FileType ft, GUID& guid, IPersistStream *pIPStream )
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
		SaveState(pFramework, ft, guid, pIPStream, "!" );
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

CJazzUndoNode  *CJazzUndoNode::Redo( IPersistStream *pIPStream )
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

BOOL  CJazzUndoNode::GetUndo( char *pszText, int nMaxCount )
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

BOOL  CJazzUndoNode::GetRedo( char *pszText, int nMaxCount )
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

void  CJazzUndoNode::Destroy()
{
	CJazzUndoNode  *pDel;

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

void  CJazzUndoNode::SetUndoLevel( long nUndoLevel )
{
	CJazzUndoNode  *pScan = m_pNext;

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

