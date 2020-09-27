/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Cell.cpp

Abstract:

	Cell class

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "dspbuilder.h"
#include "Cell.h"
#include "Module.h"
#include "Grid.h"

//------------------------------------------------------------------------------
//	Stuff
//------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------
//	Static Member Variables
//------------------------------------------------------------------------------
CPen CCell::m_invalidPen(PS_SOLID, 1, RGB(255, 0, 0));
CPen CCell::m_noSignelPen(PS_SOLID, 1, RGB(0, 0, 255));
CPen CCell::m_unknownPen(PS_SOLID, 1, RGB(0, 255, 0));

//------------------------------------------------------------------------------
//	CCell::CCell
//------------------------------------------------------------------------------
CCell::CCell(
			 IN CGrid*	pGrid
			 )
/*++

Routine Description:

	Constructor

Arguments:

	IN pGrid -	Grid this cell belongs to

Return Value:

	None

--*/
{
	m_pGrid			= pGrid;
	m_patchTop		= FALSE;
	m_patchBottom	= FALSE;
	m_patchLeft		= FALSE;
	m_patchRight	= FALSE;
	m_pModule		= NULL;
	m_join			= FALSE;
	m_patchCount	= 0;
	m_state			= CCell::UNKNOWN;
	m_stateV		= CCell::UNKNOWN;
}

//------------------------------------------------------------------------------
//	CCell::CCell
//------------------------------------------------------------------------------
CCell::CCell(
			 IN CGrid*	pGrid,
			 IN int		xLoc,
			 IN int		yLoc
			 )
/*++

Routine Description:

	Constructor

Arguments:

	IN pGrid -	Grid this cell belongs to
	IN xLoc -	X grid location
	IN yLoc -	Y grid location

Return Value:

	None

--*/
{
	m_pGrid			= pGrid;
	m_patchTop		= FALSE;
	m_patchBottom	= FALSE;
	m_patchLeft		= FALSE;
	m_patchRight	= FALSE;
	m_pModule		= NULL;
	m_join			= FALSE;
	m_patchCount	= 0;
	m_state			= CCell::UNKNOWN;
	m_stateV		= CCell::UNKNOWN;
	SetLocation(xLoc, yLoc);
}

//------------------------------------------------------------------------------
//	CCell::~CCell
//------------------------------------------------------------------------------
CCell::~CCell(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CCell::SetPatchTop
//------------------------------------------------------------------------------
void 
CCell::SetPatchTop(
				   IN BOOL	flag
				   )
/*++

Routine Description:

	Sets the ability of the cell to accept a patch on its top

Arguments:

	IN flag -	TRUE to allow patching, FALSE otherwise

Return Value:

	None

--*/
{
	if(m_patchTop == flag)
		return;

	m_patchTop = flag;
	if(m_patchTop)
		++m_patchCount;
	else
		--m_patchCount;
}

//------------------------------------------------------------------------------
//	CCell::SetPatchBottom
//------------------------------------------------------------------------------
void 
CCell::SetPatchBottom(
					  IN BOOL	flag
					  )
/*++

Routine Description:

	Sets the ability of the cell to accept a patch on its bottom

Arguments:

	IN flag -	TRUE to allow patching, FALSE otherwise

Return Value:

	None

--*/
{
	if(m_patchBottom == flag)
		return;

	m_patchBottom = flag;
	if(m_patchBottom)
		++m_patchCount;
	else
		--m_patchCount;
}

//------------------------------------------------------------------------------
//	CCell::SetPatchLeft
//------------------------------------------------------------------------------
void 
CCell::SetPatchLeft(
					IN BOOL	flag
					)
/*++

Routine Description:

	Sets the ability of the cell to accept a patch on its left

Arguments:

	IN flag -	TRUE to allow patching, FALSE otherwise

Return Value:

	None

--*/
{
	if(m_patchLeft == flag)
		return;

	m_patchLeft = flag;
	if(m_patchLeft)
		++m_patchCount;
	else
		--m_patchCount;
}

//------------------------------------------------------------------------------
//	CCell::SetPatchRight
//------------------------------------------------------------------------------
void 
CCell::SetPatchRight(
					 IN BOOL	flag
					 )
/*++

Routine Description:

	Sets the ability of the cell to accept a patch on its right.
	
Arguments:

	IN flag -	TRUE to allow patching, FALSE otherwise

Return Value:

	None

--*/
{
	if(m_patchRight == flag)
		return;

	m_patchRight = flag;
	if(m_patchRight)
		++m_patchCount;
	else
		--m_patchCount;
}

//------------------------------------------------------------------------------
//	CCell::IsPatched
//------------------------------------------------------------------------------
BOOL
CCell::IsPatched(void) const
/*++

Routine Description:

	Returns TRUE if any patch cords are visible
	
Arguments:

	None

Return Value:

	TRUE if any patch cords are visible

--*/
{
	return (m_patchLeft || m_patchRight || m_patchTop || m_patchBottom);
}

//------------------------------------------------------------------------------
//	SetLocation
//------------------------------------------------------------------------------
void
CCell::SetLocation(
				   IN int	xLoc,
				   IN int	yLoc
			       )
/*++

Routine Description:

	Sets the dimensions of the cell

Arguments:

	IN xLoc -	X location
	IN yLoc -	Y location

Return Value:

	None

--*/
{
	m_xLoc		= xLoc;
	m_yLoc		= yLoc;
	m_left		= m_pGrid->GetXOffset() + (xLoc * m_pGrid->GetCellSize());
	m_right		= m_left + m_pGrid->GetCellSize() - 1;
	m_top		= m_pGrid->GetYOffset() + (yLoc * m_pGrid->GetCellSize());
	m_bottom	= m_top +  m_pGrid->GetCellSize() - 1;
	m_mx		= (m_right - m_left) / 2 + m_left;
	m_my		= (m_bottom - m_top) / 2 + m_top;
}

//------------------------------------------------------------------------------
//	CCell::Draw
//------------------------------------------------------------------------------
void 
CCell::Draw(
			IN CDC*	pDC
			)
/*++

Routine Description:

	Draws the cell

Arguments:

	IN pDC -		Device context

Return Value:

	None

--*/
{
	CPen*	oldPen		= pDC->GetCurrentPen();

	// Draw the patch cords
	if(m_patchLeft)
	{
		SetPenForState(pDC, m_state);
		pDC->MoveTo(m_left, m_my);
		pDC->LineTo(m_mx+1, m_my);
	}
	if(m_patchRight)
	{
		SetPenForState(pDC, m_state);
		pDC->MoveTo(m_right, m_my);
		pDC->LineTo(m_mx-1, m_my);
	}
	if(m_patchTop)
	{
		SetPenForState(pDC, m_stateV);
		pDC->MoveTo(m_mx, m_top);
		pDC->LineTo(m_mx, m_my+1);
	}
	if(m_patchBottom)
	{
		SetPenForState(pDC, m_stateV);
		pDC->MoveTo(m_mx, m_bottom);
		pDC->LineTo(m_mx, m_my-1);
	}

	// If any lines cross, are they joined
	if(m_join && 
	   ((m_patchLeft && m_patchRight && (m_patchTop || m_patchBottom)) ||
	    (m_patchTop && m_patchBottom && (m_patchLeft || m_patchRight))))
	{
		pDC->FillSolidRect(m_mx-2, m_my-2, 5, 5, RGB(0, 0, 0));
	}

	// Restore
	pDC->SelectObject(oldPen);
}

//------------------------------------------------------------------------------
//	CCell::SetPenForState
//------------------------------------------------------------------------------
void
CCell::SetPenForState(
					  IN CDC*			pDC,
					  IN CCell::State	state
					  )
/*++

Routine Description:

	Sets the pen based on the state

Arguments:

	IN pDC -	Device context
	IN state -	State

Return Value:

	None

--*/
{
	switch(state)
	{
	case CCell::OK:
		pDC->SelectStockObject(BLACK_PEN);
		break;
	case CCell::INVALID:
		pDC->SelectObject(&m_invalidPen);
		break;
	case CCell::NO_SIGNAL:
		pDC->SelectObject(&m_noSignelPen);
		break;
	default:
		pDC->SelectObject(&m_unknownPen);
		break;
	}
}

