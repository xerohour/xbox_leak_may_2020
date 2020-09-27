/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	GPMixbin.cpp

Abstract:

	CGPMixbin module

Author:

	Robert Heitkamp (robheit) 04-May-2001

Revision History:

	04-May-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include "dspbuilder.h"
#include "GPMixbin.h"
#include "Cell.h"
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
//	CGPMixbin::CGPMixbin
//------------------------------------------------------------------------------
CGPMixbin::CGPMixbin(
					 IN int		id,
					 IN CGrid*	pGrid,
					 IN LPCTSTR	name
					 ) : CModule(pGrid, name)
/*++

Routine Description:

	Constructor

Arguments:

	IN id -		Mixbin id [0, 31]
	IN pGrid -	Grid this mixbin belongs to
	IN name -	Name of mixbin

Return Value:

	None

--*/
{
	int		i;
	CSize	size;

	m_id		= id;
	m_width		= 9;
	m_height	= 3;
	m_pCells	= new CCell* [m_width * m_height];

	for(i=m_width*m_height-1; i>=0; --i)
	{
		m_pCells[i] = new CCell(m_pGrid);
		m_pCells[i]->SetModule(this);
	}

	// This modules has one patch
	m_patchCell = GetCell(0, m_height/2);
	m_patchCell->SetPatchLeft(TRUE);

	// Set the pen & brush
	m_pen.CreatePen(PS_SOLID, 3, RGB(128, 90, 0));
	m_brush.CreateSolidBrush(RGB(255, 200, 100));
}

//------------------------------------------------------------------------------
//	CGPMixbin::~CGPMixbin
//------------------------------------------------------------------------------
CGPMixbin::~CGPMixbin(void)
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
//	CGPMixbin::Draw
//------------------------------------------------------------------------------
void
CGPMixbin::Draw(
				IN CDC* pDC
				)
/*++

Routine Description:

	Draws the mixbin

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	// Draw the module
	CModule::Draw(pDC);

	// Only one cell really matters (and needs to be drawn)
	m_patchCell->Draw(pDC);	
}

//------------------------------------------------------------------------------
//	CGPMixbin::GetId
//------------------------------------------------------------------------------
int
CGPMixbin::GetId(void) const
/*++

Routine Description:

	Returns the mixbin ID

Arguments:

	None

ReturnValue:

	The mixbin id [0, 31]

--*/
{
	return m_id;
}

//------------------------------------------------------------------------------
//	CGPMixbin::AreInputsPatched
//------------------------------------------------------------------------------
BOOL 
CGPMixbin::AreInputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the inputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the inputs are patched

--*/
{
	return m_pGrid->IsPatched(m_patchCell);
}

//------------------------------------------------------------------------------
//	CGPMixbin::AreOutputsPatched
//------------------------------------------------------------------------------
BOOL 
CGPMixbin::AreOutputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the outputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the outputs are patched

--*/
{
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGPMixbin::DisconnectInputs
//------------------------------------------------------------------------------
void 
CGPMixbin::DisconnectInputs(void)
/*++

Routine Description:

	Disconnects all inputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	m_pGrid->Disconnect(m_patchCell);
}
