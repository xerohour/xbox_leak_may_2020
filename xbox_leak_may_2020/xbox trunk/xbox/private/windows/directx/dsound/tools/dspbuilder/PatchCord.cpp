/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	PatchCord.cpp

Abstract:

	Patch Cord. This object connects modules

Author:

	Robert Heitkamp (robheit) 05-Oct-2001

Revision History:

	05-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "PatchCord.h"
#include "Graph.h"
#include "Module.h"
#include "Jack.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------
//	CPatchCord::CPatchCord
//------------------------------------------------------------------------------
CPatchCord::CPatchCord(
					   IN CJack*	pJack
					   ) : CUnique()
/*++

Routine Description:

	Constructor

Arguments:

	IN pJack -	Jack

Return Value:

	None

--*/
{
	m_pPatchNode		= NULL;
	m_pJack				= pJack;
	m_bInvalid			= FALSE;
	m_lastPointOn		= CPatchCord::POINTON_NONE;
	m_bBadData			= FALSE;
	m_bMark				= FALSE;
	m_bHighlight		= FALSE;
	m_bOverwrite		= FALSE;
	m_bOverwriteEnable	= FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::~CPatchCord
//------------------------------------------------------------------------------
CPatchCord::~CPatchCord(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;

	if(!m_bBadData)
		ClearPatch();
	else
	{
		while(m_pPatchNode)
		{
			// Clear the node
			pNode = m_pPatchNode->pNext;
			delete m_pPatchNode;
			m_pPatchNode = pNode;
		}
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::Draw
//------------------------------------------------------------------------------
void
CPatchCord::Draw(
				 IN CDC*	pDC
				 )
/*++

Routine Description:

	Draws the patch cord

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;
	CPen*					oldPen = NULL;
	CPen					pen;

	// Setup
	if(m_bInvalid)
		pen.CreatePen(PS_SOLID, 1, RGB(255, 0, 0));
	else if(m_bHighlight)
		pen.CreatePen(PS_SOLID, 1, RGB(100, 200, 100));
	else if(m_bOverwrite)
		pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
	else
		pen.CreatePen(PS_SOLID, 1, RGB(0, 0, 0));

	oldPen = (CPen*)pDC->SelectObject(&pen);
	pDC->Polyline(m_points.GetData(), m_points.GetSize());

	// If either end is patched to a non-jack patchcord, draw a block
	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
	{
		if(pNode->bEnd && (pNode->pPatchCord->GetJack() == NULL))
		{
			pDC->FillSolidRect(m_points[m_points.GetSize()-1].x-2, 
							   m_points[m_points.GetSize()-1].y-2, 5, 5, 
							   RGB(0, 0, 0));
			break;
		}
	}
	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
	{
		if(!pNode->bEnd && (pNode->pPatchCord->GetJack() == NULL))
		{
			pDC->FillSolidRect(m_points[0].x-2, m_points[0].y-2, 5, 5, 
							   RGB(0, 0, 0));
			break;
		}
	}

	if(m_bInvalid)
		pDC->SelectObject(oldPen);
}

//------------------------------------------------------------------------------
//	CPatchCord::DrawXOR
//------------------------------------------------------------------------------
void
CPatchCord::DrawXOR(
					IN CDC*	pDC
					)
/*++

Routine Description:

	Draws the patch cord in XOR mode

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	CPen*		oldPen;
	int			oldROP;

	// Setup
	oldPen	= (CPen*)pDC->SelectStockObject(WHITE_PEN);
	oldROP	= pDC->SetROP2(R2_XORPEN);

	Draw(pDC);

	// Restore
	pDC->SetROP2(oldROP);
	pDC->SelectObject(oldPen);
}

//------------------------------------------------------------------------------
//	CPatchCord::Split
//------------------------------------------------------------------------------
CPatchCord*
CPatchCord::Split(
				  IN const CPoint&	point
				  )
/*++

Routine Description:

	Splits the patch cord in two and returns the end split.
	A patch cord that is part of a jack cannot be split.

Arguments:

	IN point -	Point to use as split point

Return Value:

	New patch cord or NULL if this patch cord is part of a module

--*/
{
	int						i;
	CPatchCord*				pNewCord;
	CArray<CPoint, CPoint&>	points;
	CPatchCord::PatchNode*	pNode;
	CPatchCord::PatchNode*	pNext;

	// If this patch cord belongs to a Jack, it cannot be split
	ASSERT(m_pJack == NULL);

	// Build the new cord
	pNewCord = new CPatchCord;
	pNewCord->SetInvalid(m_bInvalid);

	// Is the point the last point on the cord?
	if(point == m_points[m_points.GetSize()-1])
		pNewCord->GetPoints().Add((CPoint&)point);

	// Or somewhere else
	else
	{
		points.Copy(m_points);

		// Find the place to insert the point
		for(i=1; i<m_points.GetSize(); ++i)
		{
			// Horizontal segment?
			if(m_points[i-1].y == m_points[i].y)
			{
				// Is the split point on the segment?
				if(m_points[i].y == point.y)
				{
					// Duplicate point?
					if(point.x == m_points[i-1].x)
					{
						m_points.RemoveAt(i, m_points.GetSize()-i);
						points.RemoveAt(0, i-1);
						pNewCord->SetPoints(points);
						break;
					}

					// Between the two points?
					else if(((point.x > m_points[i-1].x) && (point.x < m_points[i].x)) ||
							((point.x < m_points[i-1].x) && (point.x > m_points[i].x)))
					{
						m_points.RemoveAt(i, m_points.GetSize()-i);
						m_points.Add((CPoint&)point);
						points.RemoveAt(0, i);
						points.InsertAt(0, (CPoint&)point);
						pNewCord->SetPoints(points);
						break;
					}
				}
			}

			// Vertical segment
			else
			{
				// Is the split point on the segment?
				if(m_points[i].x == point.x)
				{
					// Duplicate point?
					if(point.y == m_points[i-1].y)
					{
						m_points.RemoveAt(i, m_points.GetSize()-i);
						points.RemoveAt(0, i-1);
						pNewCord->SetPoints(points);
						break;
					}

					// Between the two points?
					else if(((point.y > m_points[i-1].y) && (point.y < m_points[i].y)) ||
							((point.y < m_points[i-1].y) && (point.y > m_points[i].y)))
					{
						m_points.RemoveAt(i, m_points.GetSize()-i);
						m_points.Add((CPoint&)point);
						points.RemoveAt(0, i);
						points.InsertAt(0, (CPoint&)point);
						pNewCord->SetPoints(points);
						break;
					}
				}
			}
		}
	}

	pNewCord->AddPatchCord(this, FALSE);

	// Remove all end patches and add them to the new cord
	for(pNode=m_pPatchNode, pNext=(pNode ? pNode->pNext : NULL); pNode; pNode=pNext)
	{
		pNext = pNode->pNext;
		if(pNode->bEnd)
		{
			pNode->pPatchCord->SwapPatchCord(this, pNewCord);
			pNewCord->AddPatchCord(pNode->pPatchCord, TRUE);

			if(pNode->pPrev)
				pNode->pPrev->pNext = pNext;
			else
				m_pPatchNode = pNext;

			if(pNode->pNext)
				pNode->pNext->pPrev = pNode->pPrev;

			delete pNode;
		}
	}

	// Add the new cord as the end patch
	AddPatchCord(pNewCord, TRUE);

	// Update overwrite info
#if 0
	pNewCord->SetOverwriteEnable();
	pNewCord->SetOverwrite(pNewCord->GetOverwriteEnable() && m_bOverwrite);
	SetOverwriteEnable();
	SetOverwrite(m_bOverwriteEnable && m_bOverwrite);
#else
//	pNewCord->SetOverwriteEnable(m_bOverwriteEnable);
//	pNewCord->SetOverwrite(m_bOverwrite);
#endif

	CalcRect();
	pNewCord->CalcRect();

	return pNewCord;
}

//------------------------------------------------------------------------------
//	CPatchCord::Join
//------------------------------------------------------------------------------
void
CPatchCord::Join(
				 IN OUT CPatchCord*&	pPatchCord
				 )
/*++

Routine Description:

	Joins another patchcord to this and deletes the other cord. The last point
	of this must be the same as the first point of pPatch. There can also not be

Arguments:

	IN OUT pPatch -	Other cord to join that is deleted upon return.

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;
	int						numStart	= 0;
	int						numEnd		= 0;
	int						i;

	// Strict error checking
	ASSERT((m_pJack == NULL) && (pPatchCord->GetJack() == NULL));
	ASSERT(IsPatched(pPatchCord));

	// How are they connected? End - Beginning?
	if(m_points[m_points.GetSize()-1] == pPatchCord->GetPoints()[0])
	{
		// More strict error checking
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
				++numEnd;
		}
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
				++numStart;
		}
		ASSERT((numEnd == 1) && (numStart == 1));

		// Remove and add points
		m_points.RemoveAt(m_points.GetSize()-1);
		m_points.Append(pPatchCord->GetPoints());

		// Add the patches
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
			{
				AddPatchCord(pNode->pPatchCord, TRUE);
				pNode->pPatchCord->SwapPatchCord(pPatchCord, this);
			}
		}
	}

	// How are they connected? End - End?
	else if(m_points[m_points.GetSize()-1] == pPatchCord->GetPoints()[pPatchCord->GetPoints().GetSize()-1])
	{
		// More strict error checking
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
				++numEnd;
		}
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
				++numStart;
		}
		ASSERT((numEnd == 1) && (numStart == 1));

		// Remove and add points
		m_points.RemoveAt(m_points.GetSize()-1);

		// Add the points in reverse order
		for(i=pPatchCord->GetPoints().GetSize()-1; i>=0; --i)
			m_points.Add(pPatchCord->GetPoints()[i]);

		// Add the patches
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
			{
				AddPatchCord(pNode->pPatchCord, TRUE);
				pNode->pPatchCord->SwapPatchCord(pPatchCord, this);
			}
		}
	}

	// How are they connected? Beginning - End?
	else if(m_points[0] == pPatchCord->GetPoints()[pPatchCord->GetPoints().GetSize()-1])
	{
		// More strict error checking
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
				++numEnd;
		}
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
				++numStart;
		}
		ASSERT((numEnd == 1) && (numStart == 1));

		// Remove and add points
		m_points.RemoveAt(0);

		// Add the points in reverse order
		for(i=pPatchCord->GetPoints().GetSize()-1; i>=0; --i)
			m_points.InsertAt(0, pPatchCord->GetPoints()[i]);

		// Add the patches
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
			{
				AddPatchCord(pNode->pPatchCord, FALSE);
				pNode->pPatchCord->SwapPatchCord(pPatchCord, this);
			}
		}
	}

	// How are they connected? Beginning - Beginning?
	else if(m_points[0] == pPatchCord->GetPoints()[0])
	{
		// More strict error checking
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
				++numEnd;
		}
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
				++numStart;
		}
		ASSERT((numEnd == 1) && (numStart == 1));

		// Remove and add points
		m_points.RemoveAt(0);

		// Add the points
		for(i=0; i<pPatchCord->GetPoints().GetSize(); ++i)
			m_points.InsertAt(0, pPatchCord->GetPoints()[i]);

		// Add the patches
		for(pNode=pPatchCord->GetPatchNode(); pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
			{
				AddPatchCord(pNode->pPatchCord, FALSE);
				pNode->pPatchCord->SwapPatchCord(pPatchCord, this);
			}
		}
	}

	else
		ASSERT(0);

	// Remove duplicate points
	for(i=1; i<m_points.GetSize(); )
	{
		if(m_points[i] == m_points[i-1])
			m_points.RemoveAt(i);
		else
			++i;
	}

	// Remove redundent points
	if(m_points.GetSize() >= 3)
	{
		for(i=2; i<m_points.GetSize(); )
		{
			if(((m_points[i].x == m_points[i-1].x) && (m_points[i].x == m_points[i-2].x)) ||
			   ((m_points[i].y == m_points[i-1].y) && (m_points[i].y == m_points[i-2].y)))
			   m_points.RemoveAt(i-1);
			else
				++i;
		}
	}

	// Remove the references to each other
	RemovePatchCord(pPatchCord);
	pPatchCord->ClearPatch();

	// Update the overwrite enable flag
//	SetOverwriteEnable();

	// Update the overwrite flag
//	SetOverwrite(m_bOverwriteEnable && (m_bOverwrite || pPatchCord->GetOverwrite()));

	delete pPatchCord;
	pPatchCord = NULL;
	
	CalcRect();
}

//------------------------------------------------------------------------------
//	CPatchCord::Connect
//------------------------------------------------------------------------------
CPatchCord* 
CPatchCord::Connect(
					IN OUT CPatchCord*&	pPatchCord,
					IN const CPoint&	point,
					IN BOOL				bEnd
					)
/*++

Routine Description:

	Connects another patch cord to this, splitting this if necessary and 
	performing a join

Arguments:

	IN OUT pPatchCord -	Patch cord to join
	IN point -			Point to connect
	IN bEnd -			TRUE if the connection is at the end of pPatchCord
						FALSE if the connection is at the beginning of pPatchCord

Return Value:

	The new split patch cord if one is created

--*/
{
	CPatchCord*				pSplit;
	CPatchCord::PatchNode*	pNode;

	// Is the point at the beginning?
	if(point == m_points[0])
	{
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(!pNode->bEnd)
			{
				pNode->pPatchCord->AddPatchCord(pPatchCord, point);
				pPatchCord->AddPatchCord(pNode->pPatchCord, bEnd);
			}
		}
		AddPatchCord(pPatchCord, FALSE);
		pPatchCord->AddPatchCord(this, bEnd);
		return NULL;
	}

	// Or at the end?
	else if(point == m_points[m_points.GetSize()-1])
	{
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(pNode->bEnd)
			{
				pNode->pPatchCord->AddPatchCord(pPatchCord, point);
				pPatchCord->AddPatchCord(pNode->pPatchCord, bEnd);
			}
		}
		AddPatchCord(pPatchCord, TRUE);
		pPatchCord->AddPatchCord(this, bEnd);
		return NULL;
	}

	// Or in the middle?
	// Split this at the point
	pSplit = Split(point);

	// Add the new patch cord at the point
	AddPatchCord(pPatchCord, point);

	// Add the point to the new (split) patch cord
	pSplit->AddPatchCord(pPatchCord, point);

	// Add this to the splitting patch cord
	pPatchCord->AddPatchCord(this, bEnd);

	// Add the new (split) to the splitting patch cord
	pPatchCord->AddPatchCord(pSplit, bEnd);

	return pSplit;
}

//------------------------------------------------------------------------------
//	CPatchCord::SetPoints
//------------------------------------------------------------------------------
void
CPatchCord::SetPoints(
					  IN const CArray<CPoint, CPoint&>&	points
					  )
/*++

Routine Description:

	Sets the points for the patch cord line

Arguments:

	IN points -	Points

Return Value:

	None

--*/
{	
	int	i;

	m_points.RemoveAll();
	m_points.Copy(points);

	// Remove duplicate points
	for(i=1; i<m_points.GetSize(); )
	{
		if(m_points[i] == m_points[i-1])
			m_points.RemoveAt(i);
		else
			++i;
	}

	// Remove useless points
	if(m_points.GetSize() >= 3)
	{
		for(i=0; i<m_points.GetSize()-2; ++i)
		{
			// Horizontal or vertical 3 point segment?
			if(((m_points[i].y == m_points[i+1].y) &&
			    (m_points[i].y == m_points[i+2].y)) ||
			   ((m_points[i].x == m_points[i+1].x) &&
				(m_points[i].x == m_points[i+2].x)))
			{
				m_points.RemoveAt(i+1);
			}
			else
				++i;
		}
	}

	CalcRect();
}

//------------------------------------------------------------------------------
//	CPatchCord::AddPatchCord
//------------------------------------------------------------------------------
void
CPatchCord::AddPatchCord(
						 IN CPatchCord*		pPatchCord,
						 IN const CPoint&	point
						 )
/*++

Routine Description:

	Adds a patch cord to a patch node list. The patch cord can only be added
	to an endpoint

Arguments:

	IN pPatch -	Patch cord to add to list
	IN point -	Point where patch is being added

Return Value:

	None

--*/
{
	// Strict error checking
	ASSERT(m_points.GetSize() >= 2);
	ASSERT((point == m_points[0]) || (point == m_points[m_points.GetSize()-1]));

	AddPatchCord(pPatchCord, (point == m_points[0]) ? FALSE : TRUE);
}

//------------------------------------------------------------------------------
//	CPatchCord::AddPatchCord
//------------------------------------------------------------------------------
void
CPatchCord::AddPatchCord(
						 IN CPatchCord*	pPatchCord,
						 IN BOOL		bEnd
						 )
/*++

Routine Description:

	Adds a patch cord to a patch node list. The patch cord can only be added
	to an endpoint

Arguments:

	IN pPatch -	Patch cord to add to list
	IN bEnd -	TRUE to add to end, FALSE to add to beginning

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode	= new CPatchCord::PatchNode;
	pNode->pPatchCord				= pPatchCord;
	pNode->pNext					= m_pPatchNode;
	pNode->pPrev					= NULL;
	pNode->bEnd						= bEnd;
	if(m_pPatchNode)
		m_pPatchNode->pPrev = pNode;
	m_pPatchNode = pNode;
}

//------------------------------------------------------------------------------
//	CPatchCord::RemovePatchCord
//------------------------------------------------------------------------------
void 
CPatchCord::RemovePatchCord(
							IN CPatchCord*	pPatchCord
							)
/*++

Routine Description:

	Removes a patch cord from the patch node list

Arguments:

	IN pPatch -	Patch cord to remove

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode = m_pPatchNode;

	while(pNode && pNode->pPatchCord != pPatchCord)
		pNode = pNode->pNext;

	if(pNode)
	{
		if(pNode->pPrev)
			pNode->pPrev->pNext = pNode->pNext;
		else
			m_pPatchNode = pNode->pNext;

		if(pNode->pNext)
			pNode->pNext->pPrev = pNode->pPrev;

		delete pNode;
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::SwapPatchCord
//------------------------------------------------------------------------------
void 
CPatchCord::SwapPatchCord(
						  IN CPatchCord*	pOld,
						  IN CPatchCord*	pNew
						  )
/*++

Routine Description:

	Swaps two patches

Arguments:

	IN pOld -	Patch to remove
	IN pNew -	Patch to add

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode = m_pPatchNode;

	while(pNode && pNode->pPatchCord != pOld)
		pNode = pNode->pNext;

	if(pNode)
		pNode->pPatchCord = pNew;
}

//------------------------------------------------------------------------------
//	CPatchCord::ClearPatch
//------------------------------------------------------------------------------
void 
CPatchCord::ClearPatch(void)
/*++

Routine Description:

	Removes all connections

Arguments:

	None

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode = m_pPatchNode;

	while(pNode)
	{
		// Disconnect
		pNode->pPatchCord->RemovePatchCord(this);

		// Clear the node
		m_pPatchNode = pNode->pNext;
		delete pNode;
		pNode = m_pPatchNode;
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::Write
//------------------------------------------------------------------------------
void 
CPatchCord::Write(
				  IN CFile&	file
				  )
/*++

Routine Description:

	Writes the patch cord to a save file

Arguments:

	IN file -	File to write to

Return Value:

	None

--*/
{
	WORD					word;
	int						i;
	CPatchCord::PatchNode*	pNode;
	DWORD					id;

	try
	{
		// Write the unique id
		CUnique::Write(file);

		// Write the invalid state
		file.Write(&m_bInvalid, sizeof(m_bInvalid));

		// Write the rect
		CalcRect();
		file.Write((LPRECT)m_rect, sizeof(RECT));

		// Write the jack (id)
		id = m_pJack ? m_pJack->GetId() : 0;
		file.Write(&id, sizeof(id));

		// Overwrite enable
		file.Write(&m_bOverwriteEnable, sizeof(m_bOverwriteEnable));

		// Overwrite
		file.Write(&m_bOverwrite, sizeof(m_bOverwrite));

		// Write the points
		word = (WORD)m_points.GetSize();
		file.Write(&word, sizeof(word));
		for(i=0; i<m_points.GetSize(); ++i)
		{
			file.Write(&m_points[i].x, sizeof(m_points[i].x));
			file.Write(&m_points[i].y, sizeof(m_points[i].y));
		}

		// Count the patches
		for(word=0, pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
			++word;

		// Write the patches
		file.Write(&word, sizeof(word));
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{	
			file.Write(&pNode->bEnd, sizeof(pNode->bEnd));
			id = pNode->pPatchCord->GetId();
			file.Write(&id, sizeof(id));
		}
	}
	catch(...)
	{
		throw;	}
}

//------------------------------------------------------------------------------
//	CPatchCord::IsPatched
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IsPatched(
					  IN const CPatchCord*	pPatchCord
					  ) const
/*++

Routine Description:

	Returns TRUE if this is patched to pPatch

Arguments:

	IN pPatch -		Patch cord to check connectivity
					if NULL, will return TRUE if patched to anything

Return Value:

	TRUE if this is patched to pPatch, FALSE if not connected

--*/
{
	CPatchCord::PatchNode*	pNode;

	if(pPatchCord == NULL)
		return (m_pPatchNode != NULL);

	// Check this connected to pPatch
	for(pNode=m_pPatchNode; pNode && pNode->pPatchCord != pPatchCord; pNode = pNode->pNext);

	// Check pPatch connected to this
	if(pNode)
	{
		for(pNode=pPatchCord->GetPatchNode(); pNode && pNode->pPatchCord != this; pNode = pNode->pNext);
		ASSERT(pNode != NULL);
		return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::IsPatchedAtEnds
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IsPatchedAtEnds(void) const
/*++

Routine Description:

	Returns TRUE if this is patched at both ends to anything

Arguments:

	None

Return Value:

	TRUE if this is patched at both ends, FALSE if not connected

--*/
{
	BOOL					start	= FALSE;
	BOOL					end		= FALSE;
	CPatchCord::PatchNode*	pNode;

	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
	{
		if(pNode->bEnd)
			end = TRUE;
		else
			start = TRUE;
	}

	return (start && end);
}

//------------------------------------------------------------------------------
//	CPatchCord::IsPatchedToModule
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IsPatchedToModule(
							  IN const CModule*	pModule
							  ) const
/*++

Routine Description:

	Determines if this is directly connected to the module pModule

Arguments:

	IN pModule -	Module to check

Return Value:

	TRUE if connected, FALSE otherwise

--*/
{
	CPatchCord::PatchNode*	pNode;

	// Check this connected to pPatch
	for(pNode=m_pPatchNode; pNode; pNode = pNode->pNext)
	{
		if(pNode->pPatchCord->GetJack() &&
		   (pNode->pPatchCord->GetJack()->GetModule() == pModule))
		{
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::IsPatchedToEffect
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IsPatchedToEffect(
							  IN const CModule* pEffect
							  ) const
/*++

Routine Description:

	Returns TRUE if the patch cord is patched to a particular effect.
	If pEffect is NULL, this returns TRUE is the patch cord is patched to any
	effect (not a mixbin)

Arguments:

	IN pEffect -	Effect to check (can be NULL)

Return Value:

	TRUE if patched, FALSE otherwise

--*/
{
	CPatchCord::PatchNode*	pNode;

	if(pEffect)
		return IsPatchedToModule(pEffect);

	// Check this connected to pPatch
	for(pNode=m_pPatchNode; pNode; pNode = pNode->pNext)
	{
		if(pNode->pPatchCord->GetJack() &&
		   pNode->pPatchCord->GetJack()->GetModule() &&
		   pNode->pPatchCord->GetJack()->GetModule()->GetMixbin() < 0)
		{
			return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::IntersectRect
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IntersectRect(
						  IN const CRect& rect
						  ) const
/*++

Routine Description:

	Returns TRUE if this intersects a rect by checking each line segment.

Arguments:

	IN rect -	Rect to check for intersection with

Return Value:

	TRUE if this intersects the rect, FALSE otherwise

--*/
{
	int	i;
	int	xy0;
	int	xy1;

	// Quick error check
	if(m_points.GetSize() < 2)
		return FALSE;

	for(i=1; i<m_points.GetSize(); ++i)
	{
		// Is the segment horizontal?
		if(m_points[i].y == m_points[i-1].y)
		{
			if(m_points[i].x < m_points[i-1].x)
			{
				xy0 = m_points[i].x;
				xy1 = m_points[i-1].x;
			}
			else
			{
				xy0 = m_points[i-1].x;
				xy1 = m_points[i].x;
			}

			if((m_points[i].y >= rect.top) && (m_points[i].y <= rect.bottom) &&
			   (xy0 <= rect.right) && (xy1 >= rect.left))
			   return TRUE;
		}

		// Otherwise, vertical
		else
		{
			if(m_points[i].y < m_points[i-1].y)
			{
				xy0 = m_points[i].y;
				xy1 = m_points[i-1].y;
			}
			else
			{
				xy0 = m_points[i-1].y;
				xy1 = m_points[i].y;
			}

			if((m_points[i].x >= rect.left) && (m_points[i].x <= rect.right) &&
			   (xy0 <= rect.bottom) && (xy1 >= rect.top))
			   return TRUE;
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::IsPointOnLine
//------------------------------------------------------------------------------
BOOL 
CPatchCord::IsPointOnLine(
						  IN const CPoint&	point,
						  IN int			epsilon
						  )
/*++

Routine Description:

	Returns TRUE if the point is on the segment within a given epsilon.

Arguments:

	IN point -		Point to check
	IN epsilon -	Number of pixels within the line

Return Value:

	TRUE if the point is on the line segment, FALSE otherwise

--*/
{
	CRect	rect;
	int		i;

	// Check for the point on a point
	for(i=0; i<m_points.GetSize(); ++i)
	{
		if((abs(point.x - m_points[i].x) <= epsilon) &&
		   (abs(point.y - m_points[i].y) <= epsilon))
		{
			m_lastPointOn	= CPatchCord::POINTON_POINT;
			m_lastPoint		= m_points[i];
			return TRUE;
		}
	}

	// Then check the line segments
	for(i=1; i<m_points.GetSize(); ++i)
	{
		rect.left	= MIN(m_points[i].x, m_points[i-1].x) - epsilon;
		rect.right	= MAX(m_points[i].x, m_points[i-1].x) + epsilon;
		rect.top	= MIN(m_points[i].y, m_points[i-1].y) - epsilon;
		rect.bottom	= MAX(m_points[i].y, m_points[i-1].y) + epsilon;
		if(::IsPointInRect(point.x, point.y, rect))
		{
			if(m_points[i].x == m_points[i-1].x)
			{
				m_lastPointOn	= CPatchCord::POINTON_VERTICAL;
				m_lastPoint.x	= m_points[i].x;
				m_lastPoint.y	= point.y;
			}
			else
			{
				m_lastPointOn = CPatchCord::POINTON_HORIZONTAL;
				m_lastPoint.x	= point.x;
				m_lastPoint.y	= m_points[i].y;
			}
			return TRUE;
		}
	}

	m_lastPointOn = CPatchCord::POINTON_NONE;
	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::FixPointers
//------------------------------------------------------------------------------
void
CPatchCord::FixPointers(
						IN const CGraph*	pGraph
						)
/*++

Routine Description:

	Fixes the pointers loaded from a file read

Arguments:

	IN pGraph -	Graph to correct pointers with

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;

	if(m_pJack)
		m_pJack = pGraph->GetJackFromId((DWORD)m_pJack);

	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		pNode->pPatchCord = pGraph->GetPatchCordFromId((DWORD)pNode->pPatchCord);

	m_bBadData = FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::Read
//------------------------------------------------------------------------------
BOOL
CPatchCord::Read(
 			     IN CFile&	file,
				 IN BYTE	version
			     )
/*++

Routine Description:

	Constructor. Reads from a file

Arguments:

	IN file -		File to read from
	IN version -	File version

Return Value:

	None

--*/
{
	BOOL					ret = TRUE;
	WORD					word;
	CPoint					point;
	CPatchCord::PatchNode*	pNode;
	int						i;

	m_bBadData = TRUE;

	// Read the unique id
	if(!CUnique::Read(file, version))
		return FALSE;

	// Read the invalid state
	if(file.Read(&m_bInvalid, sizeof(m_bInvalid)) != sizeof(m_bInvalid))
		return FALSE;

	// Read the rect
	if(file.Read((LPRECT)m_rect, sizeof(RECT)) != sizeof(RECT))
		return FALSE;

	// Read the jack (id)
	if(file.Read(&m_pJack, sizeof(DWORD)) != sizeof(DWORD))
		return FALSE;

	if(version >= 4)
	{
		// Overwrite enable
		if(file.Read(&m_bOverwriteEnable, sizeof(m_bOverwriteEnable)) != sizeof(m_bOverwriteEnable))
			return FALSE;

		// Overwrite
		if(file.Read(&m_bOverwrite, sizeof(m_bOverwrite)) != sizeof(m_bOverwrite))
			return FALSE;
	}

	// Read the points
	if(file.Read(&word, sizeof(word)) != sizeof(word))
		return FALSE;
	for(i=0; ret && i<word; ++i)
	{
		ret = (file.Read(&point.x, sizeof(point.x)) == sizeof(point.x));
		if(ret)
		{
			ret = (file.Read(&point.y, sizeof(point.y)) == sizeof(point.y));
			if(ret)
				m_points.Add(point);
		}
	}

	// Sanity check
	CalcRect();

	// Read the patches
	if(ret)
		ret = (file.Read(&word, sizeof(word)) == sizeof(word));
	for(i=0; ret && i<word; ++i)
	{
		pNode = new CPatchCord::PatchNode;
		ret = (file.Read(&pNode->bEnd, sizeof(pNode->bEnd)) == sizeof(pNode->bEnd));
		if(ret)
		{
			ret = (file.Read(&pNode->pPatchCord, sizeof(DWORD)) == sizeof(DWORD));
			if(ret)
			{
				pNode->pNext	= m_pPatchNode;
				pNode->pPrev	= NULL;
				if(m_pPatchNode)
					m_pPatchNode->pPrev = pNode;
				m_pPatchNode = pNode;
			}
		}
	}

	if(ret)
		return TRUE;

	// Error cleanup
	m_points.RemoveAll();
	while(m_pPatchNode)
	{
		pNode = m_pPatchNode->pNext;
		delete m_pPatchNode;
		m_pPatchNode = pNode;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CPatchCord::GetConnections
//------------------------------------------------------------------------------
void 
CPatchCord::GetConnections(
						   IN OUT CArray<CJack*, CJack*>& list
						   )
/*++

Routine Description:

	Builds a list of all jacks connected to this. The list is not cleared
	before adding.

Arguments:

	IN OUT list -	List of jacks connected to this

Return Value:

	None

--*/
{
	int						i;
	CPatchCord::PatchNode*	pNode;

	// If this patch cord is marked, just return
	if(m_bMark)
		return;
	
	// Otherwise, if this is a jack, add to the list if not already there
	if(m_pJack)
	{
		for(i=0; i<list.GetSize(); ++i)
		{
			if(list[i] == m_pJack)
				return;
		}
		list.Add(m_pJack);
	}

	// Otherwise, traverse all other connected patch cords
	else
	{
		// Mark this (never mark a jack's patch cord)
		m_bMark = TRUE;

		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
			pNode->pPatchCord->GetConnections(list);
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::GetPatchCords
//------------------------------------------------------------------------------
void 
CPatchCord::GetPatchCords(
						  IN OUT CArray<CPatchCord*, CPatchCord*>& list
						  )
/*++

Routine Description:

	Builds a list of all patch cords connected to this, including those 
	indirectly connected and jacks.
	The list is not cleared before adding.

Arguments:

	IN OUT list -	List of patch cords connected to this

Return Value:

	None

--*/
{
	int						i;
	CPatchCord::PatchNode*	pNode;

	// If this patch cord is marked, just return
	if(m_bMark)
		return;

	// Add to the list if not already there
	for(i=0; i<list.GetSize(); ++i)
	{
		if(list[i] == this)
			return;
	}
	list.Add(this);

	// Mark this (never mark a jack's patch cord)
	m_bMark = TRUE;

	// Traverse all other connected patch cords
	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		pNode->pPatchCord->GetPatchCords(list);
}

//------------------------------------------------------------------------------
//	CPatchCord::Invalidate
//------------------------------------------------------------------------------
void
CPatchCord::Invalidate(void)
/*++

Routine Description:

	Recursive method to invalidate all patch cords connected to this. This
	will not invalidate patch cords belonging to jacks.

Arguments:

	None

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;

	if(m_bInvalid || m_bMark || m_pJack)
		return;

	else
	{
		m_bMark		= TRUE;
		m_bInvalid	= TRUE;

		// Invalidate all connections
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
			pNode->pPatchCord->Invalidate();
	
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::SetOverwriteEnabled
//------------------------------------------------------------------------------
void 
CPatchCord::SetOverwriteEnable(void)
/*++

Routine Description:

	Determines if overwrite enable should be set. For it to be set, the patch
	cord must be connected to an output mixbin or another patch cord connected
	to an output mixbin

Arguments:

	None

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*	pNode;

	m_bOverwriteEnable = FALSE;
	for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
	{
		if(pNode->pPatchCord &&
		   (pNode->pPatchCord->GetOverwriteEnable() ||
		    (pNode->pPatchCord->GetJack() &&
 			 pNode->pPatchCord->GetJack()->GetInput() &&
			 pNode->pPatchCord->GetJack()->GetModule() &&
			 pNode->pPatchCord->GetJack()->GetModule()->GetMixbin() >= 0)))
		{
			m_bOverwriteEnable = TRUE;
			break;
		}
	}
}

//------------------------------------------------------------------------------
//	CPatchCord::SetOverwrite
//------------------------------------------------------------------------------
RECT
CPatchCord::SetOverwrite(
						 IN BOOL bFlag
						 )
/*++

Routine Description:

	Enables or disables overwritting

Arguments:

	IN bFlag -	TRUE to enable overwrite, FALSE to disable

Return Value:

	None

--*/
{
	CPatchCord::PatchNode*				pNode;
	CArray<CPatchCord*, CPatchCord*>	patchCordList;
	int									i;
	CRect								rect(0,0,0,0);
	CPatchCord*							pMixbin	= NULL;

	if(bFlag == m_bOverwrite)
		return rect;

	// Get the list of all connections
	GetPatchCords(patchCordList);
	for(i=patchCordList.GetSize()-1; i>=0; --i)
		patchCordList[i]->Mark(FALSE);

	// The list cannot include an input mixbin or any effect inputs,
	// and must contain at least one output mixbin
	for(i=patchCordList.GetSize()-1; i>=0; --i)
	{
		if(patchCordList[i]->GetJack())
		{
			if(patchCordList[i]->GetJack()->GetModule())
			{
				// Is this a mixbin
				if(patchCordList[i]->GetJack()->GetModule()->GetMixbin() >= 0)
				{
					// Output mixbin?
					if(patchCordList[i]->GetJack()->GetInput())
					{
						// There cannot be more than one output
						if(pMixbin)
							return rect;
						pMixbin = patchCordList[i];
					}

					// Or input
					else
						return rect;
				}

				// Cannot be an effect input
				else if(patchCordList[i]->GetJack()->GetInput())
				   return rect;
			}
		}
	}

	// No output?
	if(!pMixbin)
		return rect;

	// Is this patch cord connected to an effect?
	if(IsPatchedToEffect(NULL))
	{
		// Find the effect
		for(pNode=m_pPatchNode; pNode; pNode=pNode->pNext)
		{
			if(pNode->pPatchCord && pNode->pPatchCord->GetJack() &&
			   pNode->pPatchCord->GetJack()->GetModule() &&
			   pNode->pPatchCord->GetJack()->GetModule()->GetMixbin() < 0)
			{
				// Clear the list
				patchCordList.RemoveAll();				
				CGraph::ShortestPath(pNode->pPatchCord->GetJack(), 
									 pMixbin->GetJack(), patchCordList);
				break;
			}
		}

	}

	// Set the flags for all the patch cords and unmark them
	for(i=patchCordList.GetSize()-1; i>=0; --i)
	{
		if(patchCordList[i]->GetJack())
		{
			patchCordList[i]->GetJack()->SetOverwrite(bFlag);
			rect.UnionRect(rect, 
						   patchCordList[i]->GetJack()->GetModule()->GetRect());
		}
		else
		{
			patchCordList[i]->m_bOverwrite = bFlag;
			rect.UnionRect(rect, patchCordList[i]->GetRect());
		}
	}

	return rect;
}

//------------------------------------------------------------------------------
//	CPatchCord::CalcRect
//------------------------------------------------------------------------------
void 
CPatchCord::CalcRect(void)
/*++

Routine Description:

	Calculates the rect for the patch cord

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	if(m_points.GetSize())
	{
		m_rect.left		= m_points[0].x;
		m_rect.right	= m_points[0].x;
		m_rect.top		= m_points[0].y;
		m_rect.bottom	= m_points[0].y;
		for(i=1; i<m_points.GetSize(); ++i)
		{
			if(m_points[i].x < m_rect.left)
				m_rect.left = m_points[i].x;
			if(m_points[i].x > m_rect.right)
				m_rect.right = m_points[i].x;
			if(m_points[i].y < m_rect.top)
				m_rect.top = m_points[i].y;
			if(m_points[i].y > m_rect.bottom)
				m_rect.bottom = m_points[i].y;
		}

		// Adjust
		++m_rect.right;
		++m_rect.bottom;
	}
	else
	{
		m_rect.left		= 0;
		m_rect.right	= 0;
		m_rect.top		= 0;
		m_rect.bottom	= 0;
	}
}
