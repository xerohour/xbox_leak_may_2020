// RectList.cpp: implementation of the CRectList class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "midistripmgr.h"
#include "RectList.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CRectList::CRectList()
{
	m_pList = NULL;
	m_pRect = NULL;
}

CRectList::~CRectList()
{
	RemoveAll();
	ASSERT( m_pList == NULL );
}

void CRectList::RemoveAll( void )
{
	while(m_pList != NULL)
	{
		RectElem* pRectElem = m_pList->next;
		delete m_pList;
		m_pList = pRectElem;
	}
}

void CRectList::InsertRect(RECT rect)
{
	if (m_pList != NULL)
	{
		RectElem *pRect, *opRect;
		pRect = m_pList;
		opRect = NULL;
		while ( (pRect != NULL) && (pRect->rect.right < rect.right) )
		{
			opRect = pRect;
			pRect = pRect->next;
		}
		if ( opRect == NULL )
		{	// inserting first element;
			opRect = new RectElem;
			opRect->rect = rect;
			opRect->next = pRect;
			m_pList = opRect;
		}
		else
		{	// inserting elsewhere in the list
			opRect->next = new RectElem;
			opRect->next->rect = rect;
			opRect->next->next = pRect;
		}
	}
	else
	{
		m_pList = new RectElem;
		m_pList->rect = rect;
		m_pList->next = NULL;
	}
}

int CRectList::RemoveBefore(RECT rect)
{
	int iRes = 0;
	if (m_pList != NULL)
	{
		while ((m_pList != NULL) && (m_pList->rect.right < rect.left))
		{
			RectElem* pRect = m_pList->next;
			delete m_pList;
			m_pList = pRect;
			iRes++;
		}
	}
	return iRes;
}

const RECT* CRectList::GetFirst()
{
	m_pRect = m_pList;
	return &(m_pRect->rect);
}

const RECT* CRectList::GetNext()
{
	if( m_pRect == NULL )
	{
		return NULL;
	}

	// go to the next rectangle
	m_pRect = m_pRect->next;

	return &(m_pRect->rect);
}
