/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Effect.cpp

Abstract:

	CEffect module

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
#include "Effect.h"
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
//	CEffect::CEffect
//------------------------------------------------------------------------------
CEffect::CEffect(
				 IN CGrid*			pGrid,
				 IN CEffect::Info&	info
				 ) : CModule(pGrid, info.m_name)
/*++

Routine Description:

	Constructor

Arguments:

	IN pGrid -	Grid this mixbin belongs to
	IN name -	Name of mixbin
	IN info -	Information describing the effect

Return Value:

	None

--*/
{
	int		i;

	// Inputs & Outputs
	m_numInputs		= info.m_inputs.GetSize();
	m_numOutputs	= info.m_outputs.GetSize();

	m_width		= 16;
	m_height	= (m_numInputs > m_numOutputs) ? 
				  (m_numInputs > 1 ? m_numInputs + 2 : 3) :
				  (m_numOutputs > 1 ? m_numOutputs + 2 : 3);
	m_pCells	= new CCell* [m_width * m_height];

	for(i=m_width*m_height-1; i>=0; --i)
	{
		m_pCells[i] = new CCell(m_pGrid);
		m_pCells[i]->SetModule(this);
	}

	// Inputs
	if(m_numInputs != 0)
	{
		m_inputs = new CEffect::Connection [m_numInputs];
		for(i=0; i<m_numInputs; ++i)
		{
			m_inputs[i].m_name = info.m_inputs[i];
			m_inputs[i].m_pCell = GetCell(0, i+1);
			m_inputs[i].m_pCell->SetPatchLeft(TRUE);
		}
	}
	else
		m_inputs = NULL;

	// Outputs
	if(m_numOutputs != 0)
	{
		m_outputs = new CEffect::Connection [m_numOutputs];
		for(i=0; i<m_numOutputs; ++i)
		{
			m_outputs[i].m_name = info.m_outputs[i];
			m_outputs[i].m_pCell = GetCell(m_width-1, i+1);
			m_outputs[i].m_pCell->SetPatchRight(TRUE);
		}
	}
	else
		m_outputs = NULL;

	// Fake statistics for now
	m_stats = info.m_stats;

	// Set the pen & brush
	m_pen.CreatePen(PS_SOLID, 3, RGB(0, 64, 128));
	m_brush.CreateSolidBrush(RGB(0, 180, 255));
}

//------------------------------------------------------------------------------
//	CEffect::~CEffect
//------------------------------------------------------------------------------
CEffect::~CEffect(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_inputs)
		delete [] m_inputs;
	if(m_outputs)
		delete [] m_outputs;
}

//------------------------------------------------------------------------------
//	CEffect::Draw
//------------------------------------------------------------------------------
void
CEffect::Draw(
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
	int	i;

	// Draw the module
	CModule::Draw(pDC);

	// Draw the connections
	for(i=0; i<m_numInputs; ++i)
	{
		m_inputs[i].m_pCell->Draw(pDC);
		pDC->DrawText(m_inputs[i].m_name, m_inputs[i].m_rect, DT_LEFT | DT_VCENTER);
	}
	for(i=0; i<m_numOutputs; ++i)
	{
		m_outputs[i].m_pCell->Draw(pDC);
		pDC->DrawText(m_outputs[i].m_name, m_outputs[i].m_rect, DT_RIGHT | DT_VCENTER);
	}

	// Draw the statistics
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_stats, m_statsRect, DT_CENTER | DT_VCENTER);
}

//------------------------------------------------------------------------------
//	CEffect::SetLocation
//------------------------------------------------------------------------------
void
CEffect::SetLocation(
					 IN int	xLoc,
					 IN int	yLoc
					 )
/*++

Routine Description:

	Sets the location of the module

Arguments:

	IN xLoc -		X location in cells
	IN yLoc -		Y location in cells

Return Value:

	None

--*/
{
	int	i;
	int	y;
	int	size = m_pGrid->GetCellSize();
	
	CModule::SetLocation(xLoc, yLoc);

	// Calculate the stats rectangle
	m_statsRect.top		= m_bottom - size - 1;
	m_statsRect.left	= m_left;
	m_statsRect.bottom	= m_bottom;
	m_statsRect.right	= m_right;

	// Calculate the rectangles for the connection labels
	if(m_inputs)
	{
		for(y=m_top+size, i=0; i<m_numInputs; ++i, y+=size)
		{
			m_inputs[i].m_rect.top		= y;
			m_inputs[i].m_rect.bottom	= m_inputs[i].m_rect.top + size;
			m_inputs[i].m_rect.left		= m_left + (size / 2) + 2;
			m_inputs[i].m_rect.right	= m_right - (size / 2) - 3;
		}
	}
	if(m_outputs)
	{
		for(y=m_top+size, i=0; i<m_numOutputs; ++i, y+=size)
		{
			m_outputs[i].m_rect.top		= y;
			m_outputs[i].m_rect.bottom	= m_outputs[i].m_rect.top + size;
			m_outputs[i].m_rect.left	= m_left + (size / 2) + 2;
			m_outputs[i].m_rect.right	= m_right - (size / 2) - 3;
		}
	}
}

//------------------------------------------------------------------------------
//	CEffect::AreInputsPatched
//------------------------------------------------------------------------------
BOOL 
CEffect::AreInputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the inputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the inputs are patched

--*/
{
	int	i;

	for(i=0; i<m_numInputs; ++i)
	{
		if(m_pGrid->IsPatched(m_inputs[i].m_pCell))
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CEffect::AreOutputsPatched
//------------------------------------------------------------------------------
BOOL 
CEffect::AreOutputsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the outputs are patched

Arguments:

	None

ReturnValue:

	TRUE if the outputs are patched

--*/
{
	int	i;

	for(i=0; i<m_numOutputs; ++i)
	{
		if(m_pGrid->IsPatched(m_outputs[i].m_pCell))
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CEffect::DisconnectInputs
//------------------------------------------------------------------------------
void 
CEffect::DisconnectInputs(void)
/*++

Routine Description:

	Disconnects all inputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	int	i;

	for(i=0; i<m_numInputs; ++i)
		m_pGrid->Disconnect(m_inputs[i].m_pCell);
}

//------------------------------------------------------------------------------
//	CEffect::DisconnectOutputs
//------------------------------------------------------------------------------
void 
CEffect::DisconnectOutputs(void)
/*++

Routine Description:

	Disconnects all outputs

Arguments:

	None

ReturnValue:

	None

--*/
{
	int	i;

	for(i=0; i<m_numOutputs; ++i)
		m_pGrid->Disconnect(m_outputs[i].m_pCell);
}
