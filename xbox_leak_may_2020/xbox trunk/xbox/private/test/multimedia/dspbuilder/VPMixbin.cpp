/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	VPMixbin.cpp

Abstract:

	CVPMixbin module

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
#include "VPMixbin.h"
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
//	CVPMixbin::CVPMixbin
//------------------------------------------------------------------------------
CVPMixbin::CVPMixbin(
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
	m_patchCell = GetCell(m_width-1, m_height/2);
	m_patchCell->SetPatchRight(TRUE);

	// Set the pen & brush
	m_pen.CreatePen(PS_SOLID, 3, RGB(128, 64, 0));
	m_brush.CreateSolidBrush(RGB(255, 180, 90));
}

//------------------------------------------------------------------------------
//	CVPMixbin::~CVPMixbin
//------------------------------------------------------------------------------
CVPMixbin::~CVPMixbin(void)
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
//	CVPMixbin::Draw
//------------------------------------------------------------------------------
void
CVPMixbin::Draw(
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
//	CVPMixbin::GetId
//------------------------------------------------------------------------------
int
CVPMixbin::GetId(void) const
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
//	CVPMixbin::AreInputsPatched
//------------------------------------------------------------------------------
BOOL 
CVPMixbin::AreInputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the inputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the inputs are patched

--*/
{
	return FALSE;
}

//------------------------------------------------------------------------------
//	CVPMixbin::AreOutputsPatched
//------------------------------------------------------------------------------
BOOL 
CVPMixbin::AreOutputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the outputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the outputs are patched

--*/
{
	return m_pGrid->IsPatched(m_patchCell);
}

//------------------------------------------------------------------------------
//	CVPMixbin::DisconnectOutputs
//------------------------------------------------------------------------------
void 
CVPMixbin::DisconnectOutputs(void)
/*++

Routine Description:

	Disconnects all outputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	m_pGrid->Disconnect(m_patchCell);
}
