/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Module.cpp

Abstract:

	Module composed of multiple cells

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
#include "Module.h"
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
//	Static Member Variables:
//------------------------------------------------------------------------------
CFont* CModule::m_stateFont = NULL;
int CModule::m_stateFontRef	= 0;

//------------------------------------------------------------------------------
//	CModule::CModule
//------------------------------------------------------------------------------
CModule::CModule(
				 IN CGrid*				pGrid,
				 IN int					id,
				 IN CModule::Type		type,
				 IN LPCTSTR				pName,
				 IN int					dspCycles,
				 IN int					yMemSize,
				 IN LPCTSTR				dspCode,
				 IN LPCTSTR				stateFilename,
				 IN const CStringArray*	pInputs,
				 IN const CStringArray*	pOutputs
				 )
/*++

Routine Description:

	Constructor

Arguments:

	IN pGrid -			Grid this module will belong to
	IN id -				ID of module
	IN type -			Type of module
	IN pName -			Name of module
	IN dspCycles -		DSP Cycles
	IN yMemSize -		Y mem size
	IN dspCode -		DSP filename (NULL for none)
	IN stateFilename -	State filename (NULL for none)
	IN pInputs -		List of input names (NULL terminated)
	IN pOutputs -		List of ouput names (NULL terminated)

Return Value:

	None

--*/
{
	int	i;

	if(!m_stateFont)
	{
		m_stateFont = new CFont;
		m_stateFont->CreatePointFont(160, "Arial");
	}
	++m_stateFontRef;

	m_id			= id;
	m_type			= type;
	m_state			= CModule::OK;
	m_pGrid			= pGrid;
	m_xLoc			= 0;
	m_yLoc			= 0;
	m_name			= pName;
	m_dspCode		= dspCode;
	m_stateFilename	= stateFilename;
	m_dspCycles		= dspCycles;
	m_yMemSize		= yMemSize;

	if(m_type == CModule::EFFECT)
	{
		m_stats.Format(_T("DSP Cycles: %d   Y Mem Size: %d"), 
					   m_dspCycles, m_yMemSize);
	}

	// Num inputs
	if(pInputs)
		m_numInputs = pInputs->GetSize();
	else
		m_numInputs = 0;

	// Num outputs
	if(pOutputs)
		m_numOutputs = pOutputs->GetSize();
	else
		m_numOutputs = 0;

	// Build the module based on type
	switch(m_type)
	{
	case CModule::VPMIXBIN:
		m_width		= 9;
		m_height	= 3;
		break;

	case CModule::GPMIXBIN:
		m_width		= 9;
		m_height	= 3;
		break;

	case CModule::EFFECT:
		m_width		= 16;
		m_height	= (m_numInputs > m_numOutputs) ? 
					  (m_numInputs > 1 ? m_numInputs + 2 : 3) :
					  (m_numOutputs > 1 ? m_numOutputs + 2 : 3);
		break;
	}
	CreatePenAndBrush();

	// Create the cells
	m_pCells = new CCell* [m_width * m_height];
	for(i=m_width*m_height-1; i>=0; --i)
	{
		m_pCells[i] = new CCell(m_pGrid);
		m_pCells[i]->SetModule(this);
	}

	// Inputs
	if(m_numInputs)
	{
		m_inputs = new CModule::Connection [m_numInputs];
		for(i=0; i<m_numInputs; ++i)
		{
			m_inputs[i].name	= (*pInputs)[i];
			m_inputs[i].pCell	= GetCell(0, i+1);
			m_inputs[i].pCell->SetPatchLeft(TRUE);
			m_inputs[i].pCell->SetState(CCell::OK);
			m_inputs[i].pCell->SetStateV(CCell::OK);
		}
	}
	else
		m_inputs = NULL;

	// Outputs
	if(m_numOutputs)
	{
		m_outputs = new CModule::Connection [m_numOutputs];
		for(i=0; i<m_numOutputs; ++i)
		{
			m_outputs[i].name	= (*pOutputs)[i];
			m_outputs[i].pCell	= GetCell(m_width-1, i+1);
			m_outputs[i].pCell->SetPatchRight(TRUE);
			m_outputs[i].pCell->SetState(CCell::OK);
			m_outputs[i].pCell->SetStateV(CCell::OK);
		}
	}
	else
		m_outputs = NULL;
}

//------------------------------------------------------------------------------
//	CModule::CModule
//------------------------------------------------------------------------------
CModule::CModule(
				 IN const CModule&	module
				 )
/*++

Routine Description:

	Copy constructor.

Arguments:

	IN module -	Module to copy

Return Value:

	None

--*/
{
	int			i;

	if(!m_stateFont)
	{
		m_stateFont = new CFont;
		m_stateFont->CreatePointFont(160, "Arial");
	}
	++m_stateFontRef;

	m_id			= module.m_id;
	m_type			= module.m_type;
	m_state			= module.m_state;
	m_pGrid			= module.m_pGrid;
	m_xLoc			= module.m_xLoc;
	m_yLoc			= module.m_yLoc;
	m_name			= module.m_name;
	m_stats			= module.m_stats;
	m_numInputs		= module.m_numInputs;
	m_numOutputs	= module.m_numOutputs;
	m_width			= module.m_width;
	m_height		= module.m_height;
	m_dspCode		= module.m_dspCode;
	m_stateFilename	= module.m_stateFilename;
	m_dspCycles		= module.m_dspCycles;
	m_yMemSize		= module.m_yMemSize;	

	CreatePenAndBrush();

	// Create the cells
	m_pCells = new CCell* [m_width * m_height];
	for(i=m_width*m_height-1; i>=0; --i)
	{
		m_pCells[i] = new CCell(m_pGrid);
		m_pCells[i]->SetModule(this);
	}

	// Inputs
	if(m_numInputs)
	{
		m_inputs = new CModule::Connection [m_numInputs];
		for(i=0; i<m_numInputs; ++i)
		{
			m_inputs[i].name	= module.m_inputs[i].name;
			m_inputs[i].pCell	= GetCell(0, i+1);
			m_inputs[i].pCell->SetPatchLeft(TRUE);
			m_inputs[i].pCell->SetState(module.m_inputs[i].pCell->GetState());
			m_inputs[i].pCell->SetStateV(module.m_inputs[i].pCell->GetStateV());
		}
	}
	else
		m_inputs = NULL;

	// Outputs
	if(m_numOutputs)
	{
		m_outputs = new CModule::Connection [m_numOutputs];
		for(i=0; i<m_numOutputs; ++i)
		{
			m_outputs[i].name	= module.m_outputs[i].name;
			m_outputs[i].pCell	= GetCell(m_width-1, i+1);
			m_outputs[i].pCell->SetPatchRight(TRUE);
			m_outputs[i].pCell->SetState(module.m_outputs[i].pCell->GetState());
			m_outputs[i].pCell->SetStateV(module.m_outputs[i].pCell->GetStateV());
		}
	}
	else
		m_outputs = NULL;

	// Set the location
	SetLocation(module.m_xLoc, module.m_yLoc);
}

//------------------------------------------------------------------------------
//	CModule::~CModule
//------------------------------------------------------------------------------
CModule::~CModule(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Free the cells
	if(m_pCells)
	{
		for(i=m_width*m_height-1; i>=0; --i)
			delete m_pCells[i];
		delete [] m_pCells;
	}

	// Inputs and Outputs
	if(m_inputs)
		delete [] m_inputs;
	if(m_outputs)
		delete [] m_outputs;

	// Delete the font?
	--m_stateFontRef;
	if(m_stateFontRef == 0)
	{
		delete m_stateFont;
		m_stateFont = NULL;
	}
}

//------------------------------------------------------------------------------
//	CModule::SetLocation
//------------------------------------------------------------------------------
void
CModule::SetLocation(
					 IN	int	xLoc,
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
	int	x;
	int	y;
	int	xoff	= m_pGrid->GetXOffset() + (xLoc * m_pGrid->GetCellSize());
	int	yoff	= m_pGrid->GetYOffset() + (yLoc * m_pGrid->GetCellSize());
	int	size	= m_pGrid->GetCellSize();

	m_xLoc			= xLoc;
	m_yLoc			= yLoc;
	m_rect.left		= xoff;
	m_rect.right	= xoff + (m_width * m_pGrid->GetCellSize());
	m_rect.top		= yoff;
	m_rect.bottom	= yoff + (m_height * m_pGrid->GetCellSize());
	
	// Calculate the name rectangle
	m_nameRect.top		= m_rect.top;
	m_nameRect.left		= m_rect.left;
	m_nameRect.bottom	= m_nameRect.top + m_pGrid->GetCellSize();
	m_nameRect.right	= m_rect.right;

	if(m_pCells)
	{
		for(x=0; x<m_width; ++x)
		{
			for(y=0; y<m_height; ++y)
				GetCell(x, y)->SetLocation(xLoc+x, yLoc+y);
		}
	}

	// Calculate the stats rectangle
	m_statsRect.top		= m_rect.bottom - size;
	m_statsRect.left	= m_rect.left;
	m_statsRect.bottom	= m_rect.bottom;
	m_statsRect.right	= m_rect.right;

	// Calculate the rectangles for the connection labels
	if(m_inputs)
	{
		for(y=m_rect.top+size, i=0; i<m_numInputs; ++i, y+=size)
		{
			m_inputs[i].rect.top	= y;
			m_inputs[i].rect.bottom	= m_inputs[i].rect.top + size;
			m_inputs[i].rect.left	= m_rect.left + (size / 2) + 2;
			m_inputs[i].rect.right	= m_rect.right - (size / 2) - 3;
		}
	}
	if(m_outputs)
	{
		for(y=m_rect.top+size, i=0; i<m_numOutputs; ++i, y+=size)
		{
			m_outputs[i].rect.top		= y;
			m_outputs[i].rect.bottom	= m_outputs[i].rect.top + size;
			m_outputs[i].rect.left		= m_rect.left + (size / 2) + 2;
			m_outputs[i].rect.right		= m_rect.right - (size / 2) - 3;
		}
	}
}

//------------------------------------------------------------------------------
//	CModule::GetCell
//------------------------------------------------------------------------------
CCell*
CModule::GetCell(
				 IN	int	x,
				 IN	int	y
				 )
/*++

Routine Description:

	Returns the cell at the given x, y location (not pixels)

Arguments:

	IN x -	X location of cell [0 - m_width-1]
	IN y -	Y location of cell [0 - m_height-1]

Return Value:

	The cell at the given location.

--*/
{
	if(m_pCells && (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height))
		return m_pCells[x * m_height + y];
	return NULL;
}

//------------------------------------------------------------------------------
//	CModule::GetWidth
//------------------------------------------------------------------------------
int 
CModule::GetWidth(void) const
/*++

Routine Description:

	Returns the width (in cells) of the module

Arguments:

	None

Return Value:

	The width in cells of the module

--*/
{
	return m_width;
}

//------------------------------------------------------------------------------
//	CModule::GetHeight
//------------------------------------------------------------------------------
int 
CModule::GetHeight(void) const
/*++

Routine Description:

	Returns the height (in cells) of the module

Arguments:

	None

Return Value:

	The height in cells of the module

--*/
{
	return m_height;
}

//------------------------------------------------------------------------------
//	CModule::GetLeft
//------------------------------------------------------------------------------
int
CModule::GetLeft(void) const
/*++

Routine Description:

	Returns the leftmost cell index of the module

Arguments:

	None

ReturnValue:

	The leftmost cell index of the module

--*/
{
	return m_xLoc;
}

//------------------------------------------------------------------------------
//	CModule::GetRight
//------------------------------------------------------------------------------
int
CModule::GetRight(void) const
/*++

Routine Description:

	Returns the rightmost cell index of the module

Arguments:

	None

ReturnValue:

	The rightmost cell index of the module

--*/
{
	return m_xLoc + m_width - 1;
}

//------------------------------------------------------------------------------
//	CModule::GetTop
//------------------------------------------------------------------------------
int
CModule::GetTop(void) const
/*++

Routine Description:

	Returns the topmost cell index of the module

Arguments:

	None

ReturnValue:

	The topmost cell index of the module

--*/
{
	return m_yLoc;
}

//------------------------------------------------------------------------------
//	CModule::GetBottom
//------------------------------------------------------------------------------
int
CModule::GetBottom(void) const
/*++

Routine Description:

	Returns the bottommost cell index of the module

Arguments:

	None

ReturnValue:

	The bottommost cell index of the module

--*/
{
	return m_yLoc + m_height - 1;
}

//------------------------------------------------------------------------------
//	CModule::Draw
//------------------------------------------------------------------------------
void
CModule::Draw(
			  IN CDC*	pDC
			  )
/*++

Routine Description:

	Draws the modules.
	Draws the outline, the filled interior and the name(s) of the module

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	int				i;
	CPen*			oldPen;
	CBrush*			oldBrush;
	static CPoint	point(10, 10);

	// Setup to draw
	oldPen		= pDC->SelectObject(&m_pen);
	if(m_state != CModule::OK)
		oldBrush = (CBrush*)pDC->SelectStockObject(NULL_BRUSH);
	else
		oldBrush = pDC->SelectObject(&m_brush);

	// Draw
	pDC->RoundRect(m_rect, point);

	// Draw the state
	DrawState(pDC);

	// Draw the name
	pDC->SetTextColor(RGB(0,0,0));
	pDC->SetBkMode(TRANSPARENT);
	pDC->DrawText(m_name, m_nameRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Draw the connections
	for(i=0; i<m_numInputs; ++i)
	{
		m_inputs[i].pCell->Draw(pDC);
		pDC->DrawText(m_inputs[i].name, m_inputs[i].rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	for(i=0; i<m_numOutputs; ++i)
	{
		m_outputs[i].pCell->Draw(pDC);
		pDC->DrawText(m_outputs[i].name, m_outputs[i].rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}

	// Draw the statistics
	pDC->DrawText(m_stats, m_statsRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

	// Restore the graphics state
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

//------------------------------------------------------------------------------
//	CModule::DrawState
//------------------------------------------------------------------------------
void
CModule::DrawState(
				   IN CDC*	pDC
				   )
/*++

Routine Description:

	Draws the state of the module

Arguments:

	IN pDC -	Device context

Return Value:

	None

--*/
{
	switch(m_state)
	{
	case CModule::INVALID: // Invalid
		{
		int		offset = m_pGrid->GetCellSize() / 2;
		CPen	pen(PS_SOLID, 7, RGB(255, 0, 0));
		CPen*	oldPen = pDC->SelectObject(&pen);

		pDC->MoveTo(m_rect.left + offset, m_rect.top + offset);
		pDC->LineTo(m_rect.right - offset, m_rect.bottom - offset);
		pDC->MoveTo(m_rect.left + offset, m_rect.bottom - offset);
		pDC->LineTo(m_rect.right - offset, m_rect.top + offset);
		
		pDC->SelectObject(oldPen);
		}
		break;

	case CModule::INSERT: // Insert
		{
		CFont*	oldFont = pDC->SelectObject(m_stateFont);
		pDC->SetTextColor(RGB(0,0,255));
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(_T("<Insert>"), m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(oldFont);
		}
		break;

	case CModule::SWAP:	// Swap
		{
		CFont*	oldFont = pDC->SelectObject(m_stateFont);
		pDC->SetTextColor(RGB(0, 100, 0));
		pDC->SetBkMode(TRANSPARENT);
		pDC->DrawText(_T("<Swap>"), m_rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
		pDC->SelectObject(oldFont);
		}
		break;

	default:	// OK
		break;
	}
}

//------------------------------------------------------------------------------
//	CModule::AreInputsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::AreInputsPatched(void) const
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
		if(m_pGrid->IsPatched(m_inputs[i].pCell))
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::AreOutputsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::AreOutputsPatched(void) const
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
		if(m_pGrid->IsPatched(m_outputs[i].pCell))
			return TRUE;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CModule::IsPatched
//------------------------------------------------------------------------------
BOOL 
CModule::IsPatched(void) const
/*++

Routine Description:

	Returns TRUE if the module is patched

Arguments:

	None

ReturnValue:

	TRUE if the module is patched

--*/
{
	return AreInputsPatched() || AreOutputsPatched();
}

//------------------------------------------------------------------------------
//	CModule::Disconnect
//------------------------------------------------------------------------------
void 
CModule::Disconnect(void)
/*++

Routine Description:

	Disconnect all connections

Arguments:

	None

ReturnValue:

	None

--*/
{
	DisconnectInputs();
	DisconnectOutputs();
}

//------------------------------------------------------------------------------
//	CModule::DisconnectInputs
//------------------------------------------------------------------------------
void 
CModule::DisconnectInputs(void)
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
		m_pGrid->Disconnect(m_inputs[i].pCell);
}

//------------------------------------------------------------------------------
//	CModule::DisconnectOutputs
//------------------------------------------------------------------------------
void 
CModule::DisconnectOutputs(void)
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
		m_pGrid->Disconnect(m_outputs[i].pCell);
}

//------------------------------------------------------------------------------
//	CModule::SetState
//------------------------------------------------------------------------------
void 
CModule::SetState(
				  IN CModule::State	state
				  )
/*++

Routine Description:

	Sets the state of the module

Arguments:

	IN state -	State

Return Value:

	None

--*/
{
	m_state = state;
}

//------------------------------------------------------------------------------
//	CModule::GetState
//------------------------------------------------------------------------------
CModule::State 
CModule::GetState(void) const
/*++

Routine Description:

	Returns the state of the module

Arguments:

	None

Return Value:

	The state of the module

--*/
{
	return m_state;
}

//------------------------------------------------------------------------------
//	CModule::SetType
//------------------------------------------------------------------------------
void 
CModule::SetType(
				 IN CModule::Type type
				 )
/*++

Routine Description:

	Sets the type of the module

Arguments:

	IN type -	Type

Return Value:

	None

--*/
{
	m_type = type;
}

//------------------------------------------------------------------------------
//	CModule::GetType
//------------------------------------------------------------------------------
CModule::Type 
CModule::GetType(void) const
/*++

Routine Description:

	Returns the type of the module

Arguments:

	None

Return Value:

	The type of the module

--*/
{
	return m_type;
}

//------------------------------------------------------------------------------
//	CModule::GetId
//------------------------------------------------------------------------------
int	
CModule::GetId(void) const 
/*++

Routine Description:

	Returns the id of the module

Arguments:

	None

Return Value:

	The module's id

--*/
{
	return m_id; 
}

//------------------------------------------------------------------------------
//	CModule::GetRect
//------------------------------------------------------------------------------
const CRect&
CModule::GetRect(void) const
/*++

Routine Description:

	Returns the visible rectangle of the module

Arguments:

	None

Return Value:

	The rectangle (in pixels) of the module

--*/
{
	return m_rect;
}

//------------------------------------------------------------------------------
//	CModule::CreatePenAndBrush
//------------------------------------------------------------------------------
void 
CModule::CreatePenAndBrush(void)
/*++

Routine Description:

	Creates a pen and brush based on the type of module

Arguments:

	None

Return Value:

	None

--*/
{
	switch(m_type)
	{
	case CModule::VPMIXBIN:
		m_pen.CreatePen(PS_SOLID, 1, RGB(128, 64, 0));
		if(m_id < 6)
			m_brush.CreateSolidBrush(RGB(255, 100, 90));
		else
			m_brush.CreateSolidBrush(RGB(255, 180, 90));
		break;

	case CModule::GPMIXBIN:
		m_pen.CreatePen(PS_SOLID, 1, RGB(128, 90, 0));
		if(m_id < 6)
			m_brush.CreateSolidBrush(RGB(255, 120, 120));
		else
			m_brush.CreateSolidBrush(RGB(255, 200, 100));
		break;

	case CModule::EFFECT:
		m_pen.CreatePen(PS_SOLID, 1, RGB(0, 64, 128));
		m_brush.CreateSolidBrush(RGB(0, 180, 255));
		break;
	}
}

//------------------------------------------------------------------------------
//	CModule::IsConnected
//------------------------------------------------------------------------------
BOOL 
CModule::IsConnected(void) const
/*++

Routine Description:

	Returns TRUE if the module is connected to a valid input and output

Arguments:

	None

Return Value:

	TRUE if the module is connected to a valid input and output, FALSE otherwise

--*/
{
	int		i;
	BOOL	in	= FALSE;
	BOOL	out	= FALSE;

	for(i=0; i<m_numInputs; ++i)
	{
		if(m_inputs[i].pCell->GetState() == CCell::OK)
		{
			in = TRUE;
			break;
		}
	}
	for(i=0; i<m_numOutputs; ++i)
	{
		if(m_outputs[i].pCell->GetState() == CCell::OK)
		{
			out = TRUE;
			break;
		}
	}
	return (in && out);
}

