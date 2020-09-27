/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Grid.cpp

Abstract:

	Grid class for keeping track of CCells

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robhei
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "stdafx.h"
#include <afxtempl.h>
#include "dspbuilder.h"
#include "Grid.h"
#include "Cell.h"
#include "Module.h"
#include "parser.h"

//------------------------------------------------------------------------------
//	Defines:
//------------------------------------------------------------------------------
#define ENV_VAR_INI_PATH		"_XGPIMAGE_INI_PATH"
#define ENV_VAR_DSP_CODE_PATH	"_XGPIMAGE_DSP_CODE_PATH"
#define SECTION_MAX_SIZE		4096

//------------------------------------------------------------------------------
//	Stuff:
//------------------------------------------------------------------------------
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//------------------------------------------------------------------------------
//	Structures
//------------------------------------------------------------------------------
struct EffectChainBucket
{
	CArray<CModule*, CModule*>	effects;
	CArray<int, int>			inputs;	 // Negative values are temp mixbins
	CArray<int, int>			outputs; // Negative values are temp mixbins

	EffectChainBucket& operator = (const EffectChainBucket& ecb)
	{
		effects.Copy(ecb.effects);
		inputs.Copy(ecb.inputs);
		outputs.Copy(ecb.outputs);
		return *this;
	}
};

//------------------------------------------------------------------------------
//	CGrid::CGrid
//------------------------------------------------------------------------------
CGrid::CGrid(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	m_pCells			= NULL;
	m_numCells			= 0;
	m_width				= 0;
	m_height			= 0;
	m_drawGrid			= FALSE;
	m_mixbinWidth		= 0;
	m_mixbinWidth		= 0;
	m_pMoveModule		= NULL;
	m_pTempModule		= NULL;
	m_leftMouseDown		= 0;
	m_drawLine			= FALSE;
	m_drawLeftRight		= FALSE;
	m_numEffects		= NULL;
	m_numEffectsInList	= NULL;
	m_pEffectList		= NULL;
	m_insertEffect		= FALSE;
	m_bChangesMade		= FALSE;
	m_numPatches		= 0;

	for(i=0; i<CGridNumMixbins; ++i)
	{
		m_pVPMixbins[i] = NULL;
		m_pGPMixbins[i] = NULL;
	}
	for(i=0; i<CGridMaxNumEffects; ++i)
		m_pEffects[i] = NULL;

	SetCellSize(13);
}

//------------------------------------------------------------------------------
//	CGrid::~CGrid
//------------------------------------------------------------------------------
CGrid::~CGrid(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	Cleanup();
}

//------------------------------------------------------------------------------
//	CGrid::Create
//------------------------------------------------------------------------------
void 
CGrid::Create(
			  IN CWnd*	pParent
			  )
/*++

Routine Description:

	Creates the grid with width x height empty cells

Arguments:

	IN pParent -	Parent window

Return Value:

	None

--*/ 
{
	int				i;
	CStringArray	inputs;
	CStringArray	outputs;
	LPCTSTR			mixbinNames[CGridNumMixbins] = 
	{
		_T("FRONTLEFT"),
		_T("FRONTRIGHT"),
		_T("FRONTCENTER"),
		_T("LFE"),
		_T("BACKLEFT"),
		_T("BACKRIGHT"),
		_T("FXSEND0"),
		_T("FXSEND1"),
		_T("FXSEND2"),
		_T("FXSEND3"),
		_T("FXSEND4"),
		_T("FXSEND5"),
		_T("FXSEND6"),
		_T("FXSEND7"),
		_T("FXSEND8"),
		_T("FXSEND9"),
		_T("FXSEND10"),
		_T("FXSEND11"),
		_T("FXSEND12"),
		_T("FXSEND13"),
		_T("FXSEND14"),
		_T("FXSEND15"),
		_T("FXSEND16"),
		_T("FXSEND17"),
		_T("FXSEND18"),
		_T("FXSEND19")
	};
	srand(time(NULL));
	Cleanup();

	// Parent window
	m_pParent = pParent;

	// Load all the effects
	if(!LoadEffects())
	{
		AfxMessageBox("Unable to load effects.", 
					  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		exit(1);
	}

	// Create the mixbin menu
	if(m_mixbinMenu.CreatePopupMenu())
	{
		m_mixbinMenu.AppendMenu(MF_STRING, ID_MIXBIN_MOVE_UP, _T("Move &Up"));
		m_mixbinMenu.AppendMenu(MF_STRING, ID_MIXBIN_MOVE_DOWN, _T("Move &Down"));
		m_mixbinMenu.AppendMenu(MF_STRING, ID_MIXBIN_DISCONNECT, _T("&Disconnect"));
	}

	// Create the effects menu
	if(m_effectsMenu.CreatePopupMenu())
	{
		m_effectsMenu.AppendMenu(MF_STRING, ID_EFFECTS_PROPERTIES, _T("&Properties"));
		m_effectsMenu.AppendMenu(MF_STRING, ID_EFFECTS_DELETE, _T("&Delete"));
		m_effectsMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_ALL, _T("Disconnect &All"));
		m_effectsMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_INPUTS, _T("Disconnect &Inputs"));
		m_effectsMenu.AppendMenu(MF_STRING, ID_EFFECTS_DISCONNECT_OUTPUTS, _T("Disconnect &Outputs"));
	}

	// Patch cord menu
	if(m_patchCordMenu.CreatePopupMenu())
		m_patchCordMenu.AppendMenu(MF_STRING, ID_PATCH_DISCONNECT, _T("&Disconnect"));

	// Insert effects menu
	if(m_insertEffectMenu.CreatePopupMenu())
	{
		// Add all the effects to the insert effects menu
		for(i=0; i<m_numEffectsInList; ++i)
			m_insertEffectMenu.AppendMenu(MF_STRING, m_pEffectList[i]->GetId(), m_pEffectList[i]->GetName());
		
		// Grid menu (root menu)
		if(m_gridMenu.CreatePopupMenu())
		{
			m_gridMenu.AppendMenu(MF_POPUP, (UINT)m_insertEffectMenu.Detach(), _T("&Insert Effect"));
			m_gridMenu.AppendMenu(MF_STRING, ID_GRID_DISCONNECT_ALL, _T("Disconnect &All"));
			m_gridMenu.AppendMenu(MF_STRING, ID_GRID_DELETE_ALL, _T("&Delete All"));
			m_gridMenu.AppendMenu(MF_STRING, ID_GRID_SHOW_GRID, _T("&Show Grid"));
		}
		else
		{
			AfxMessageBox("Unable to create root popup menu.", 
						  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			exit(1);
		}
	}
	else
	{
		AfxMessageBox("Unable to create Insert Effect popup menu.", 
					  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		exit(1);
	}
	
	// Create the VP Mixbins
	inputs.SetSize(1);
	for(i=0; i<CGridNumMixbins; ++i)
		m_pVPMixbins[i] = new CModule(this, i, CModule::VPMIXBIN, mixbinNames[i], 0, 0, NULL, NULL, NULL, &inputs);

	// Create the GP Mixbins
	for(i=0; i<CGridNumMixbins; ++i)
		m_pGPMixbins[i] = new CModule(this, i, CModule::GPMIXBIN, mixbinNames[i], 0, 0, NULL, NULL, &inputs, NULL);

	// All mixbins are the same dimension
	m_mixbinWidth	= m_pVPMixbins[0]->GetWidth();
	m_mixbinHeight	= m_pVPMixbins[0]->GetHeight();
	
	// Create the font
	m_font.CreatePointFont(80, _T("Arial"));

	// Start it up...
	New(88, (m_mixbinHeight * CGridNumMixbins) + (CGridNumMixbins - 1));
}

//------------------------------------------------------------------------------
//	CGrid::Cleanup
//------------------------------------------------------------------------------
void
CGrid::Cleanup(void)
/*++

Routine Description:

	Frees all resources associated with the grid

Arguments:

	None

Return Value:

	None

--*/
{
	int	i;

	// Delete the cells
	if(m_pCells)
	{
		for(i=0; i<m_numCells; ++i)
			SetCell(i, NULL);
		delete [] m_pCells;
		m_pCells		= NULL;
		m_numPatches	= 0;
	}
	
	// Delete the VP Mixbins
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(m_pVPMixbins[i])
			delete m_pVPMixbins[i];
		m_pVPMixbins[i] = NULL;
	}

	// Delete the GP Mixbins
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(m_pGPMixbins[i])
			delete m_pGPMixbins[i];
		m_pGPMixbins[i] = NULL;
	}

	// Delete the Effects
	for(i=0; i<m_numEffects; ++i)
	{
		if(m_pEffects[i])
			delete m_pEffects[i];
		m_pEffects[i] = NULL;
	}

	// Delete the Effect List
	if(m_pEffectList)
	{
		for(i=0; i<m_numEffectsInList; ++i)
		{
			if(m_pEffectList[i])
				delete m_pEffectList[i];
			m_pEffectList[i] = NULL;
		}
		delete [] m_pEffectList;
		m_pEffectList = NULL;
	}

	m_numEffects		= 0;
	m_numEffectsInList	= 0;
	m_numCells			= 0;
	m_width				= 0;
	m_height			= 0;
}

//------------------------------------------------------------------------------
//	SetCellSize
//------------------------------------------------------------------------------
void
CGrid::SetCellSize(
				   IN int	size
				   )
/*++

Routine Description:

	Sets the size of a cell

Arguments:

	IN size -	Width & height of a cell in pixels

Return Value:

	None

--*/
{
	m_cellSize = size;
}

//------------------------------------------------------------------------------
//	GetCellSize
//------------------------------------------------------------------------------
int
CGrid::GetCellSize(void) const
/*++

Routine Description:

	Returns the cell size

Arguments:

	None

Return Value:

	The size of the cell in pixels (width & height)

--*/
{
	return m_cellSize;
}

//------------------------------------------------------------------------------
//	CGrid::Draw
//------------------------------------------------------------------------------
void
CGrid::Draw(
			IN const CRect&	rect,
			IN CDC*			pDC
			)
/*++

Routine Description:

	Draws the grid and its contents

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	CFont*	oldFont;
	int		x;
	int		y;
	int		mx;
	int		my;
	BOOL	deleteDC;
	CRect	gridRect((rect.left - GetXOffset()) / m_cellSize, 
					 (rect.top - GetYOffset()) / m_cellSize,
					 (rect.right - GetXOffset()) / m_cellSize + 1,
					 (rect.bottom - GetYOffset()) / m_cellSize + 1);

	// Create a device context?
	if(pDC == NULL)
	{
		pDC = new CClientDC(m_pParent);
		deleteDC = TRUE;

		// If no DC is specified, clear the region manually
		pDC->FillSolidRect(rect, RGB(255, 255, 255));
	}
	else
		deleteDC = FALSE;

	// Select the font
	oldFont = pDC->SelectObject(&m_font);

	// Draw the grid?
	if(m_drawGrid)
	{
		for(x=gridRect.left, mx = (x * m_cellSize + GetXOffset()) + (m_cellSize / 2) ; x<=gridRect.right; ++x, mx += m_cellSize)
		{
			for(y=gridRect.top, my = (y * m_cellSize + GetYOffset()) + (m_cellSize / 2); y<=gridRect.bottom; ++y, my += m_cellSize)
			{
				if(IsValidIndex(x, y))
					pDC->SetPixel(mx, my, RGB(0, 128, 128));
			}
		}
	}

	// Draw all the cells
	for(x=gridRect.left; x<=gridRect.right; ++x)
	{
		for(y=gridRect.top; y<=gridRect.bottom; ++y)
		{
			if(IsValidIndex(x, y))
			{
				i = GetCellIndex(x, y);
				if(m_pCells[i])
					m_pCells[i]->Draw(pDC);
			}
		}
	}

	// Draw the VP Mixbins
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(IsModuleInRect(m_pVPMixbins[i], rect))
			m_pVPMixbins[i]->Draw(pDC);
	}

	// Draw the GP Mixbins
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(IsModuleInRect(m_pGPMixbins[i], rect))
			m_pGPMixbins[i]->Draw(pDC);
	}

	// Draw the effects
	for(i=0; i<m_numEffects; ++i)
	{
		if(IsModuleInRect(m_pEffects[i], rect))
			m_pEffects[i]->Draw(pDC);
	}

	// DEBUG: Fill NULL grid cells
#if 0
	if(m_drawGrid)
	{
		for(i=0; i<m_numCells; ++i)
		{
			if(!m_pCells[i])
			{
				pDC->FillSolidRect((i / m_height) * m_cellSize + GetXOffset() + 1, 
								   (i % m_height) * m_cellSize + GetYOffset() + 1, 
								   m_cellSize-2, m_cellSize-2, RGB(220, 220, 220));
			}
		}
	}
#endif

	// Draw a moving module?
	if(m_pTempModule)
		m_pTempModule->Draw(pDC);

	// Restore
	pDC->SelectObject(oldFont);

	// Delete the dc?
	if(deleteDC)
		delete pDC;
}
//------------------------------------------------------------------------------
//	CGrid::GetCell
//------------------------------------------------------------------------------
CCell*
CGrid::GetCellAlloc(
					IN int	x,
					IN int	y
					)
/*++

Routine Description:

	Returns a cell at a given grid location. If no cell exists, one will be
	allocated

Arguments:

	IN x -		X location of cell
	IN y -		Y location of cell

Return Value:

	The cell at the given location

--*/
{
	if(IsValidIndex(x, y))
	{
		int index = GetCellIndex(x, y);
		if(m_pCells[index] == NULL)
		{
			m_pCells[index] = new CCell(this, x, y);
			++m_numPatches;
		}
		return m_pCells[index];
	}
	return NULL;
}

//------------------------------------------------------------------------------
//	CGrid::GetCell
//------------------------------------------------------------------------------
CCell*
CGrid::GetCell(
			   IN int	x,
			   IN int	y
			   ) const
/*++

Routine Description:

	Returns a cell at a given grid location.

Arguments:

	IN x -		X location of cell
	IN y -		Y location of cell

Return Value:

	The cell at the given location, or NULL

--*/
{
	if(IsValidIndex(x, y))
		return m_pCells[GetCellIndex(x, y)];
	return NULL;
}

//------------------------------------------------------------------------------
//	CGrid::IsPlacementValid
//------------------------------------------------------------------------------
BOOL 
CGrid::IsPlacementValid(
						IN const CModule*	pModule,
						IN int				left,
						IN int				top
						)
/*++

Routine Description:

	Determines if the placement of a module is valid. A module that
	will be moved cannot be placed within 1 cell of any other modules.

Arguments:

	IN pModule -	Module to test
	IN left -		Leftmost cell of module
	IN top -		Topmost cell of module

ReturnValue:

	TRUE if the module can be placed at the given location, FALSE otherwise

--*/
{
	int		i;
	CRect	r0(left, top, pModule->GetWidth() + left - 1, pModule->GetHeight() + top - 1);
	CRect	r1;
	CRect	r2;

	// Bounds check the placement. The module must be placed horizontally
	// between the VP mixbins and the GP mixbins, and vertically within 
	// the grid
	if((left <= m_mixbinWidth) || 
	   ((left + pModule->GetWidth()) >= (m_width - m_mixbinWidth)) ||
	   (top < 0) || ((top + pModule->GetHeight()) > m_height))
	   return FALSE;

	// Check to see if the module would be within 1 cell of any other
	// module (except itself)
	for(i=0; i<m_numEffects; ++i)
	{
		// Ignore the module. This situation should only occur when 
		// a module is being moved.
		if(m_pEffects[i] != pModule)
		{
			r1.left		= m_pEffects[i]->GetLeft() - 2;
			r1.right	= m_pEffects[i]->GetRight() + 2;
			r1.top		= m_pEffects[i]->GetTop() - 2;
			r1.bottom	= m_pEffects[i]->GetBottom() + 2;

			if(r2.IntersectRect(r0, r1))
				return FALSE;
		}
	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::PlaceModuleIfValid
//------------------------------------------------------------------------------
BOOL
CGrid::PlaceModuleIfValid(
						  IN CModule*	pModule,
						  IN int		left,
						  IN int		top
						  )
/*++

Routine Description:

	Places a module on the grid.

Arguments:

	IN pModule -	Module to place
	IN left -		Leftmost cell of module
	IN top -		Topmost cell of module

ReturnValue:

	TRUE if the module was placed successfully, FALSE otherwise

--*/
{
	int		x;
	int		y;
	int		right;
	int		bottom;
	int		l;
	int		t;
	int		r;
	int		b;

	// Check for a valid placement
	if(IsPlacementValid(pModule, left, top))
	{
		right	= left + pModule->GetWidth() - 1;
		bottom	= top + pModule->GetHeight() - 1;
		l		= left > 0 ? left-1 : 0;
		t		= top > 0 ? top-1 : 0;
		r		= right < m_width-1 ? right + 1 : m_width-1;
		b		= bottom < m_height-1 ? bottom + 1 : m_height-1;

		// Clear the module's space and the bounding cells
		for(x=l; x<=r; ++x)
		{
			for(y=t; y<=b; ++y)
				SetCell(x, y, NULL);
		}

		// Insert the module
		PlaceModule(pModule, left, top);

		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::InBox
//------------------------------------------------------------------------------
BOOL
CGrid::InBox(
			 IN	int	x,
			 IN int	y,
			 IN	int	left,
			 IN int	right,
			 IN int	top,
			 IN int	bottom
			 ) const
/*++

Routine Description:

	Returns TRUE if the point (x, y) is within the box

Arguments:

	IN x -		Point
	IN y -		Point
	IN left -	Left side of box
	IN right -	Right side of box
	IN top -	Top of box
	IN bottom -	Bottom of box

ReturnValue:

	TRUE if the point is within the box, FALSE otherwise

--*/
{
	return ((x >= left) && (x <= right) && (y >= top) && (y <= bottom));
}

//------------------------------------------------------------------------------
//	CGrid::SetCell
//------------------------------------------------------------------------------
void
CGrid::SetCell(
			   IN int		x,
			   IN int		y,
			   IN CCell*	pCell
			   )
/*++

Routine Description:

	Sets a cell. If the index is already occupied, the exising cell is 
	deleted (if not part of a module)

Arguments:

	IN x -		X location
	IN y -		Y location
	IN pCell -	Cell to set

ReturnValue:

	None

--*/
{
	SetCell(GetCellIndex(x, y), pCell);
}

//------------------------------------------------------------------------------
//	CGrid::SetCell
//------------------------------------------------------------------------------
void
CGrid::SetCell(
			   IN int		index,
			   IN CCell*	pCell
			   )
/*++

Routine Description:

	Sets a cell. If the index is already occupied, the exising cell is 
	deleted (if not part of a module)

Arguments:

	IN index -	X location
	IN pCell -	Cell to set

ReturnValue:

	None

--*/
{
	if(m_pCells[index] && !m_pCells[index]->GetModule())
	{
		delete m_pCells[index];
		--m_numPatches;
	}
	m_pCells[index] = pCell;
}

//------------------------------------------------------------------------------
//	CGrid::IsValidIndex
//------------------------------------------------------------------------------
BOOL
CGrid::IsValidIndex(
					IN int	x,
					IN int	y
					) const
/*++

Routine Description:

	Returns TRUE if the coordinate is a valid index

Arguments:

	IN x -	X location
	IN y -	Y location

ReturnValue:

	TRUE if the coordinate is valid

--*/
{
	return ((m_pCells != NULL) && (x >= 0) && (x < m_width) && (y >= 0) && (y < m_height));
}

//------------------------------------------------------------------------------
//	CGrid::PlaceModule
//------------------------------------------------------------------------------
void
CGrid::PlaceModule(
				   IN CModule*	pModule,
				   IN int		left,
				   IN int		top
				   )
/*++

Routine Description:

	Places a module on the grid. This method assume the placement is valid
	and does not clear existing cells. See PlaceModuleIfValid().

Arguments:

	IN pModule -	Module to place
	IN left -		Leftmost cell of module
	IN top -		Topmost cell of module

ReturnValue:

	None

--*/
{
	int		x;
	int		y;
	int		right;
	int		bottom;

	right	= left + pModule->GetWidth() - 1;
	bottom	= top + pModule->GetHeight() - 1;

	// Set the location of the module
	pModule->SetLocation(left, top);

	// Install the module
	for(x=left; x<=right; ++x)
	{
		for(y=top; y<=bottom; ++y)
			SetCell(x, y, pModule->GetCell(x-left, y-top));
	}
}

//------------------------------------------------------------------------------
//	CGrid::RemoveModule
//------------------------------------------------------------------------------
void
CGrid::RemoveModule(
				    IN const CModule*	pModule
				    )
/*++

Routine Description:

	Nulls the cells under the module

Arguments:

	IN pModule -	Module to place

ReturnValue:

	None

--*/
{
	int		x;
	int		y;

	// Install the module
	for(x=pModule->GetLeft(); x<=pModule->GetRight(); ++x)
	{
		for(y=pModule->GetTop(); y<=pModule->GetBottom(); ++y)
			SetCell(x, y, NULL);
	}
}

//------------------------------------------------------------------------------
//	CGrid::GetXYFromPoint
//------------------------------------------------------------------------------
BOOL 
CGrid::GetXYFromPoint(
					  IN const CPoint&	point, 
					  OUT int&			x, 
					  OUT int&			y
					  ) const
/*++

Routine Description:

	Returns an xy grid location from a point

Arguments:

	IN point -	Point (pixels)
	OUT x -		X grid location
	OUT y -		Y grid location

ReturnValue:

	Returns TRUE if the point is within the grid, FALSE otherwise

--*/
{
	// XY location
	x = (point.x - GetXOffset()) / m_cellSize;
	y = (point.y - GetYOffset()) / m_cellSize;

	// Is the point outside if the grid (1 cell boundary)
	if((point.x < GetXOffset()) || (point.x >= (m_cellSize * m_width + GetXOffset())) ||
	   (point.y < GetYOffset()) || (point.y >= (m_cellSize * m_height + GetYOffset())))
	   return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::OnLButtonDown
//------------------------------------------------------------------------------
void
CGrid::OnLButtonDown(
					 IN UINT	nFlags,
					 IN CPoint	point
					 )
/*++

Routine Description:

	Left mouse down:
		Select Module
		Drag Module
		Draw Patch Cord
		Connect / Disconnect Patch Cord
		Drop Module

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	int			x;
	int			y;
	CCell*		pCell	= NULL;
	CModule*	pModule	= NULL;

	m_leftMouseDown = TRUE;

	// If any other button is down besides the left mouse, ignore
	if((nFlags & MK_MBUTTON) || (nFlags & MK_RBUTTON))
		return;

	// Get the cell the mouse is within
	if(!GetXYFromPoint(point, x, y))
		return;

	// Get the cell that was hit
	pCell = GetCell(x, y);
	if(pCell)
		pModule = pCell->GetModule();

	// Insert an effect?
	if(m_insertEffect)
	{
		if(m_pTempModule->GetState() == CModule::OK)
		{
			ResetRect();
			AdjustRect(m_pTempModule);
			PlaceModule(m_pTempModule, m_pTempModule->GetLeft(), m_pTempModule->GetTop());

			m_pEffects[m_numEffects++]	= m_pTempModule;
			m_pMoveModule				= NULL;
			m_pTempModule				= NULL;
			m_insertEffect				= FALSE;
			m_bChangesMade				= TRUE;

			ValidateGrid();
			Draw(m_rect);
		}
	}

	// Empty grid node or a patched cell?
	else if((pCell && pCell->IsPatched()) || 
	   (!pCell && (x >= m_mixbinWidth) && (x < (m_width - m_mixbinWidth)) && (y >= 0) && (y < m_height)))
	{
		if(pCell && pCell->GetModule())
			m_drawLeftRight = TRUE;
		else
			m_drawLeftRight = FALSE;
		m_drawLine	= TRUE;
		m_moveX		= x;
		m_moveY		= y;
		m_moveDx	= x;
		m_moveDy	= y;
	}

	// Was a module selected?
	else if(pModule)
	{
		m_pMoveModule	= pModule;
		m_pTempModule	= new CModule(*m_pMoveModule);
		m_moveX			= x;
		m_moveY			= y;
		m_moveDx		= x - m_pMoveModule->GetLeft();
		m_moveDy		= y - m_pMoveModule->GetTop();
		m_pMoveModule->SetState(CModule::MOVING);
	}
}

//------------------------------------------------------------------------------
//	CGrid::OnLButtonUp
//------------------------------------------------------------------------------
void
CGrid::OnLButtonUp(
				   IN UINT		nFlags,
				   IN CPoint	point
				   )
/*++

Routine Description:

	Left mouse up:
		Drop module

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	int			i;
	int			from;
	int			to;
	int			x;
	int			y;
	CRect		rect;
	CCell*		pCell		= NULL;
	CModule*	pModule		= NULL;
	BOOL		disconnect	= FALSE;

	m_leftMouseDown = FALSE;

	// Drawing a line?
	if(m_drawLine)
	{
		DrawLine();
		m_drawLine = FALSE;

		// If the point up is outside the client rect, just return
		m_pParent->GetClientRect(rect);
		if((point.x < rect.left) || (point.x > rect.right) || (point.y < rect.top) || (point.y > rect.bottom))
		   return;

		// If the line crosses a module, just return
		for(i=0; i<m_numEffects; ++i)
		{
			// Horizontal
			if(m_moveDy == m_moveY)
			{
				if(m_moveX <= m_moveDx)
				{
					from	= m_moveX;
					to		= m_moveDx;
				}
				else
				{
					from	= m_moveDx;
					to		= m_moveX;
				}
				if((m_moveY >= m_pEffects[i]->GetTop()) && (m_moveY <= m_pEffects[i]->GetBottom()) &&
				   (from <= m_pEffects[i]->GetLeft()) && (to >= m_pEffects[i]->GetRight()))
				   return;
			}

			// Vertical
			else if(m_moveDx == m_moveX)
			{
				if(m_moveY <= m_moveDy)
				{
					from	= m_moveY;
					to		= m_moveDy;
				}
				else
				{
					from	= m_moveDy;
					to		= m_moveY;
				}
				if((m_moveX >= m_pEffects[i]->GetLeft()) && (m_moveX <= m_pEffects[i]->GetRight()) &&
				   (from <= m_pEffects[i]->GetTop()) && (to >= m_pEffects[i]->GetBottom()))
				   return;
			}

			// Should never happen
			else
				return;
		}

		// Create a patch cord?
		if((m_moveDx != m_moveX) || (m_moveDy != m_moveY))
		{
			pCell = GetCell(m_moveDx, m_moveDy);

			// Only patch if drawn to a valid cell (NULL or a patchable one)
			if((pCell && pCell->IsPatched()) || (!pCell && (m_moveDx >= m_mixbinWidth) && 
			   (m_moveDx < (m_width - m_mixbinWidth)) && (m_moveDy >= 0) && (m_moveDy < m_height)))
			{
				DrawPatchCords(m_moveX, m_moveY, m_moveDx, m_moveDy, (nFlags & MK_CONTROL) ? TRUE : FALSE);
				m_bChangesMade = TRUE;
			}
		}

		// If the point is in the same location and the control key was down, join / disconnect 
		else if(nFlags & MK_CONTROL)
		{
			pCell = GetCell(m_moveDx, m_moveDy);
			if(pCell && (pCell->GetPatchCount() == 4))
			{
				pCell->SetJoin(!pCell->IsJoined());
				ResetRect();
				AdjustRect(pCell);
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
			}
		}
	}

	// Was a module being moved?
	else if(m_pMoveModule)
	{
		// If the control key is not down, disconnect
		if(!(nFlags & MK_CONTROL))
			disconnect = TRUE;

		ResetRect();
		AdjustRect(m_pTempModule);

		GetXYFromPoint(point, x, y);

		// Module Type
		switch(m_pMoveModule->GetType())
		{
		case CModule::VPMIXBIN:
			if(m_pTempModule->GetState() == CModule::INSERT)
			{
				// These two calls will be valid
				pCell	= GetCell(x, y+1);
				pModule	= pCell->GetModule();
				from	= GetVPMixbinIndex(m_pMoveModule);
				to		= GetVPMixbinIndex(pModule);
				x		= m_pMoveModule->GetLeft();
				y		= m_pMoveModule->GetTop();
				AdjustRect(m_pMoveModule);
				AdjustRect(pModule);

				if(from > to)
				{
					for(i=from-1; i>=to; --i)
					{
						if(disconnect)
							m_pVPMixbins[i]->Disconnect();
						m_pVPMixbins[i+1] = m_pVPMixbins[i];
						PlaceModule(m_pVPMixbins[i+1], x, y);
						y -= (m_mixbinHeight + 1);
					}
					m_pVPMixbins[i+1] = m_pMoveModule;
					PlaceModule(m_pMoveModule, x, y);
					m_bChangesMade = TRUE;
				}
				else if(to > from)
				{
					for(i=from+1; i<to; ++i)
					{
						if(disconnect)
							m_pVPMixbins[i]->Disconnect();
						m_pVPMixbins[i-1] = m_pVPMixbins[i];
						PlaceModule(m_pVPMixbins[i-1], x, y);
						y += (m_mixbinHeight + 1);
					}
					m_pVPMixbins[i-1] = m_pMoveModule;
					PlaceModule(m_pMoveModule, x, y);
					m_bChangesMade = TRUE;
				}
			}
			else if(m_pTempModule->GetState() == CModule::SWAP)
			{
				// These two calls will be valid
				pCell	= GetCell(x, y);
				pModule	= pCell->GetModule();
				if(pModule != m_pMoveModule)
				{
					AdjustRect(m_pMoveModule);
					AdjustRect(pModule);
					if(disconnect)
					{
						m_pMoveModule->Disconnect();
						pModule->Disconnect();
					}
					x	= m_pMoveModule->GetLeft();
					y	= m_pMoveModule->GetTop();
					PlaceModule(m_pMoveModule, pModule->GetLeft(), pModule->GetTop());
					PlaceModule(pModule, x, y);
					from	= GetVPMixbinIndex(m_pMoveModule);
					to		= GetVPMixbinIndex(pModule);
					m_pVPMixbins[from]	= pModule;
					m_pVPMixbins[to]	= m_pMoveModule;
					m_bChangesMade = TRUE;
				}
			}
			break;

		case CModule::GPMIXBIN:
			if(m_pTempModule->GetState() == CModule::INSERT)
			{
				// These two calls will be valid
				pCell	= GetCell(x, y+1);
				pModule	= pCell->GetModule();
				from	= GetGPMixbinIndex(m_pMoveModule);
				to		= GetGPMixbinIndex(pModule);
				x		= m_pMoveModule->GetLeft();
				y		= m_pMoveModule->GetTop();
				AdjustRect(m_pMoveModule);
				AdjustRect(pModule);

				if(from > to)
				{
					for(i=from-1; i>=to; --i)
					{
						if(disconnect)
							m_pGPMixbins[i]->Disconnect();
						m_pGPMixbins[i+1] = m_pGPMixbins[i];
						PlaceModule(m_pGPMixbins[i+1], x, y);
						y -= (m_mixbinHeight + 1);
					}
					m_pGPMixbins[i+1] = m_pMoveModule;
					PlaceModule(m_pMoveModule, x, y);
					m_bChangesMade = TRUE;
				}
				else if(to > from)
				{
					for(i=from+1; i<to; ++i)
					{
						if(disconnect)
							m_pGPMixbins[i]->Disconnect();
						m_pGPMixbins[i-1] = m_pGPMixbins[i];
						PlaceModule(m_pGPMixbins[i-1], x, y);
						y += (m_mixbinHeight + 1);
					}
					m_pGPMixbins[i-1] = m_pMoveModule;
					PlaceModule(m_pMoveModule, x, y);
					m_bChangesMade = TRUE;
				}
			}
			else if(m_pTempModule->GetState() == CModule::SWAP)
			{
				// These two calls will be valid
				pCell	= GetCell(x, y);
				pModule	= pCell->GetModule();
				if(pModule != m_pMoveModule)
				{
					AdjustRect(m_pMoveModule);
					AdjustRect(pModule);
					if(disconnect)
					{
						m_pMoveModule->Disconnect();
						pModule->Disconnect();
					}
					x	= m_pMoveModule->GetLeft();
					y	= m_pMoveModule->GetTop();
					PlaceModule(m_pMoveModule, pModule->GetLeft(), pModule->GetTop());
					PlaceModule(pModule, x, y);
					from	= GetGPMixbinIndex(m_pMoveModule);
					to		= GetGPMixbinIndex(pModule);
					m_pGPMixbins[from]	= pModule;
					m_pGPMixbins[to]	= m_pMoveModule;
					m_bChangesMade = TRUE;
				}
			}
			break;

		case CModule::EFFECT:
			// Is the current position valid to drop the module?
			if((m_pTempModule->GetState() == CModule::OK) &&
			   ((m_pTempModule->GetLeft() != m_pMoveModule->GetLeft()) ||
			    (m_pTempModule->GetTop() != m_pMoveModule->GetTop())))
			{
				AdjustRect(m_pMoveModule);
				RemoveModule(m_pMoveModule);
				if(disconnect)
					m_pMoveModule->Disconnect();
				PlaceModule(m_pMoveModule, m_pTempModule->GetLeft(), m_pTempModule->GetTop());
				m_bChangesMade = TRUE;
			}
			break;
		}
		m_pMoveModule->SetState(CModule::OK);
		m_pMoveModule = NULL;
		delete m_pTempModule;
		m_pTempModule = NULL;
		ValidateGrid();
		Draw(m_rect);
	}
}

//------------------------------------------------------------------------------
//	CGrid::OnRButtonDown
//------------------------------------------------------------------------------
void
CGrid::OnRButtonDown(
					 IN UINT	nFlags,
					 IN CPoint	point
					 )
/*++

Routine Description:

	Right Mouse down: Popup menu

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

ReturnValue:

	None

--*/
{
	int			i;
	int			x;
	int			y;
	int			nCmd;
	CCell*		pCell		= NULL;
	CModule*	pModule		= NULL;
	CModule*	otherModule	= NULL;

	// If any other button is down besides the left mouse or any key is down,
	// ignore
	if((nFlags & MK_MBUTTON) || (nFlags & MK_LBUTTON) || (nFlags & MK_CONTROL) ||
		(nFlags & MK_SHIFT))
		return;
	
	// Get the cell the mouse is within
	if(!GetXYFromPoint(point, x, y))
		return;

	// Get the module selected
	pCell = GetCell(x, y);
	if(pCell)
		pModule = pCell->GetModule();

	m_pParent->ClientToScreen(&point);

	// Patch cord menu?
	if(pCell && pCell->IsPatched())
	{
		m_patchCordMenu.EnableMenuItem(ID_PATCH_DISCONNECT, IsPatched(pCell) ? MF_ENABLED : MF_GRAYED);
		nCmd = m_patchCordMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
											  TPM_RIGHTBUTTON | TPM_RETURNCMD |
											  TPM_NONOTIFY,
											  point.x, point.y, m_pParent);
		switch(nCmd)
		{
		case ID_PATCH_DISCONNECT:
			ResetRect();
			Disconnect(pCell);
			m_bChangesMade = TRUE;
			ValidateGrid();
			Draw(m_rect);
			break;
		}
	}

	// Was a module selected?
	else if(pModule)
	{
		// Otherwise if the module a mixbin?
		if((pModule->GetType() == CModule::VPMIXBIN) || (pModule->GetType() == CModule::GPMIXBIN))
		{
			m_mixbinMenu.EnableMenuItem(ID_MIXBIN_MOVE_UP, pModule->GetTop() > 0 ? MF_ENABLED : MF_GRAYED);
			m_mixbinMenu.EnableMenuItem(ID_MIXBIN_MOVE_DOWN, pModule->GetBottom() < m_height-1 ? MF_ENABLED : MF_GRAYED);
			m_mixbinMenu.EnableMenuItem(ID_MIXBIN_DISCONNECT, pModule->IsPatched() ? MF_ENABLED : MF_GRAYED);
			nCmd = m_mixbinMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
											   TPM_RIGHTBUTTON | TPM_RETURNCMD | 
											   TPM_NONOTIFY, 
											   point.x, point.y, m_pParent);
			switch(nCmd)
			{
			case ID_MIXBIN_MOVE_UP:
				otherModule = GetCell(x, y - (m_mixbinHeight + 1))->GetModule(); // This should never fail
				if(!otherModule)
					return;
				ResetRect();
				AdjustRect(pModule);
				AdjustRect(otherModule);
				pModule->Disconnect();
				otherModule->Disconnect();
				PlaceModule(pModule, pModule->GetLeft(), pModule->GetTop() - (m_mixbinHeight + 1));
				PlaceModule(otherModule, otherModule->GetLeft(), otherModule->GetTop() + (m_mixbinHeight + 1));
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);

				// Adjust the mixbin list
				if(pModule->GetType() == CModule::VPMIXBIN)
				{
					i = GetVPMixbinIndex(pModule);
					m_pVPMixbins[i]		= m_pVPMixbins[i-1];
					m_pVPMixbins[i-1]	= pModule;
				}
				else
				{
					i = GetGPMixbinIndex(pModule);
					m_pGPMixbins[i]		= m_pGPMixbins[i-1];
					m_pGPMixbins[i-1]	= pModule;
				}
				break;

			case ID_MIXBIN_MOVE_DOWN:
				otherModule = GetCell(x, y + (m_mixbinHeight + 1))->GetModule(); // This should never fail
				if(!otherModule)
					return;
				ResetRect();
				AdjustRect(pModule);
				AdjustRect(otherModule);
				pModule->Disconnect();
				otherModule->Disconnect();
				PlaceModule(pModule, pModule->GetLeft(), pModule->GetTop() + (m_mixbinHeight + 1));
				PlaceModule(otherModule, otherModule->GetLeft(), otherModule->GetTop() - (m_mixbinHeight + 1));
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);

				// Adjust the mixbin list
				if(pModule->GetType() == CModule::VPMIXBIN)
				{
					i = GetVPMixbinIndex(pModule);
					m_pVPMixbins[i]		= m_pVPMixbins[i+1];
					m_pVPMixbins[i+1]	= pModule;
				}
				else
				{
					i = GetGPMixbinIndex(pModule);
					m_pGPMixbins[i]		= m_pGPMixbins[i+1];
					m_pGPMixbins[i+1]	= pModule;
				}

				break;

			case ID_MIXBIN_DISCONNECT:
				ResetRect();
				pModule->Disconnect();
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;
			}
		}

		// Or is it an effect
		else if(pModule->GetType() == CModule::EFFECT)
		{
			m_effectsMenu.EnableMenuItem(ID_EFFECTS_PROPERTIES, MF_GRAYED);
			m_effectsMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_ALL, pModule->IsPatched() ? MF_ENABLED : MF_GRAYED);
			m_effectsMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_INPUTS, pModule->AreInputsPatched() ? MF_ENABLED : MF_GRAYED);
			m_effectsMenu.EnableMenuItem(ID_EFFECTS_DISCONNECT_OUTPUTS, pModule->AreOutputsPatched() ? MF_ENABLED : MF_GRAYED);
			nCmd = m_effectsMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
												TPM_RIGHTBUTTON | TPM_RETURNCMD |
												TPM_NONOTIFY,
												point.x, point.y, m_pParent);
			switch(nCmd)
			{
			case ID_EFFECTS_PROPERTIES:
				break;
			case ID_EFFECTS_DELETE:
				ResetRect();
				AdjustRect(pModule);
				DeleteEffect(pModule);
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;
			case ID_EFFECTS_DISCONNECT_ALL:
				ResetRect();
				pModule->Disconnect();
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;
			case ID_EFFECTS_DISCONNECT_INPUTS:
				ResetRect();
				pModule->DisconnectInputs();
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;
			case ID_EFFECTS_DISCONNECT_OUTPUTS:
				ResetRect();
				pModule->DisconnectOutputs();
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;
			}
		}
	}

	// Or just the root window
	else 
	{
		m_gridMenu.CheckMenuItem(ID_GRID_SHOW_GRID, m_drawGrid ? MF_CHECKED : MF_UNCHECKED);
		m_gridMenu.EnableMenuItem(ID_GRID_DELETE_ALL, m_numEffects > 0 ? MF_ENABLED : MF_GRAYED);
		m_gridMenu.EnableMenuItem(ID_GRID_DISCONNECT_ALL, IsPatched() ? MF_ENABLED : MF_GRAYED);
		m_gridMenu.EnableMenuItem(ID_GRID_INSERT_EFFECT, MF_ENABLED);
		nCmd = m_gridMenu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
										 TPM_RIGHTBUTTON | TPM_RETURNCMD |
										 TPM_NONOTIFY,
										 point.x, point.y, m_pParent);

		// Insert an effect?
		if(nCmd < 0)
			InsertEffect(point, -(nCmd+1));

		// Otherwise, do something else
		else
		{
			switch(nCmd)
			{
			case ID_GRID_INSERT_EFFECT:
				break;

			case ID_GRID_DISCONNECT_ALL:
				ResetRect();
				for(i=0; i<m_numCells; ++i)
				{
					AdjustRect(m_pCells[i]);
					SetCell(i, NULL);
				}
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;

			case ID_GRID_DELETE_ALL:
				ResetRect();
				for(i=m_numEffects-1; i>=0; --i)
				{
					AdjustRect(m_pEffects[i]);
					DeleteEffect(m_pEffects[i]);
				}
				m_bChangesMade = TRUE;
				ValidateGrid();
				Draw(m_rect);
				break;

			case ID_GRID_SHOW_GRID:
				m_pParent->GetClientRect(m_rect);
				m_drawGrid = !m_drawGrid;
				Draw(m_rect);
				break;
			}
		}
	}
}

//------------------------------------------------------------------------------
//	CGrid::OnMove
//------------------------------------------------------------------------------
void 
CGrid::OnMouseMove(
				   IN UINT		nFlags,
				   IN CPoint	point
				   )
/*++

Routine Description:

	Moves a module, or draws a patch cord

Arguments:

	IN nFlags -	Flags
	IN point -	Point where mouse was clicked

Return Value:

	None

--*/
{
	int				x;
	int				y;
	int				nx;
	int				ny;
	CModule::State	state;
	CRect			rect;

	// If the left mouse is not down, just return
	if(!m_leftMouseDown && !m_insertEffect)
		return;

	// Drawing a line?
	if(m_drawLine)
	{
		// If the point is outside the client rect, just return
		m_pParent->GetClientRect(rect);
		if((point.x < rect.left) || (point.x > rect.right) || (point.y < rect.top) || (point.y > rect.bottom))
		   return;
		if(GetXYFromPoint(point, x, y))
		{
			if((x != m_moveDx) || (y != m_moveDy))
			{
				DrawLine();

				if(m_drawLeftRight || (abs(x - m_moveX) >= abs(y - m_moveY)))
				{
					m_moveDx = x;
					m_moveDy = m_moveY;
				}
				else
				{
					m_moveDx = m_moveX;
					m_moveDy = y;
				}

				DrawLine();
			}
		}
	}

	// Moving a module?
	else if(m_pMoveModule)
	{
		// Get the grid cell
		state = GetXYFromPoint(point, x, y) ? CModule::OK : CModule::INVALID;

		// Define the invalidation region
		m_rect = m_pTempModule->GetRect();

		// Update the position of the temp module
		nx		= m_pTempModule->GetLeft() + (x - m_moveX);
		ny		= m_pTempModule->GetTop() + (y - m_moveY);

		// Will the module move?
		if((nx != m_pTempModule->GetLeft()) || (ny != m_pTempModule->GetTop()))
		{
			m_moveX	= x;
			m_moveY	= y;
			m_pTempModule->SetLocation(nx, ny);

			// Update the state of the temp module
			if(state == CModule::OK)
			{
				switch(m_pMoveModule->GetType())
				{
				case CModule::VPMIXBIN:
					if(x >= m_mixbinWidth)
						state = CModule::INVALID;
					else if(GetCell(x, y))
						state = (GetCell(x, y)->GetModule() == m_pMoveModule) ? state : CModule::SWAP;
					else 
						state = CModule::INSERT;
					break;

				case CModule::GPMIXBIN:
					if(x <= m_width - m_mixbinWidth)
						state = CModule::INVALID;
					else if(GetCell(x, y))
						state = (GetCell(x, y)->GetModule() == m_pMoveModule) ? state : CModule::SWAP;
					else 
						state = CModule::INSERT;
					break;

				case CModule::EFFECT:
					state = IsPlacementValid(m_pMoveModule, nx, ny) ? state : CModule::INVALID;
				}
			}
			m_pTempModule->SetState(state);

			// Draw
			Draw(m_rect);
		}
	}
}

//------------------------------------------------------------------------------
//	CGrid::Disconnect
//------------------------------------------------------------------------------
void
CGrid::Disconnect(
				  IN CCell*				pCell,
				  IN CGrid::Direction	dir
				  )
/*++

Routine Description:

	Disconnects a cell or series of cells. The cell and any other cells patched
	to the cell will be disconnected up to a joined cell.

Arguments:

	IN pCell -	Cell to disconnect
	IN dir -	Direction the disconnnect came from

ReturnValue:

	None

--*/
{
	CCell*			nextCell;
	int				x	= pCell->GetXLoc();	
	int				y	= pCell->GetYLoc();

	// Intitialize the stack
	if(dir == CGrid::DIR_NONE)
		m_si = 0;

	// Has this cell already been processed (check for overflow)?
	if(IsCellInStack(pCell) || (m_si == 512))
		return;

	// Insert the cell into the stack
	m_stack[m_si++] = pCell;
	
	// Is the cell patched?
	if(pCell->IsPatched())
	{
		// Is the cell joined?
		if(pCell->IsJoined())
		{
			// If it was entered from any side, just unpatch that side and return
			if(dir == CGrid::DIR_LEFT)
			{
				AdjustRect(pCell);
				pCell->SetPatchLeft(FALSE);
				if(pCell->GetPatchCount() < 3)
					pCell->SetJoin(FALSE);
				return;
			}
			else if(dir == CGrid::DIR_RIGHT)
			{
				AdjustRect(pCell);
				pCell->SetPatchRight(FALSE);
				if(pCell->GetPatchCount() < 3)
					pCell->SetJoin(FALSE);
				return;
			}
			else if(dir == CGrid::DIR_TOP)
			{
				AdjustRect(pCell);
				pCell->SetPatchTop(FALSE);
				if(pCell->GetPatchCount() < 3)
					pCell->SetJoin(FALSE);
				return;
			}
			else if(dir == CGrid::DIR_BOTTOM)
			{
				AdjustRect(pCell);
				pCell->SetPatchBottom(FALSE);
				if(pCell->GetPatchCount() < 3)
					pCell->SetJoin(FALSE);
				return;
			}
		}

		// If all 4 edges are patched and a direction other than DIR_NONE is specified, 
		// Move in the one direction left, and just unpatch instead of deleting
		if((pCell->GetPatchCount() == 4) && (dir != CGrid::DIR_NONE))
		{
			if(dir == CGrid::DIR_LEFT)
			{
				AdjustRect(pCell);
				nextCell = GetCell(x+1, y);
				if(nextCell)
					Disconnect(nextCell, CGrid::DIR_LEFT);
				pCell->SetPatchLeft(FALSE);
				pCell->SetPatchRight(FALSE);
				return;
			}
			else if(dir == CGrid::DIR_RIGHT)
			{
				AdjustRect(pCell);
				nextCell = GetCell(x-1, y);
				if(nextCell)
					Disconnect(nextCell, CGrid::DIR_RIGHT);
				pCell->SetPatchLeft(FALSE);
				pCell->SetPatchRight(FALSE);
				return;
			}
			else if(dir == CGrid::DIR_TOP)
			{
				AdjustRect(pCell);
				nextCell = GetCell(x, y+1);
				if(nextCell)
					Disconnect(nextCell, CGrid::DIR_TOP);
				pCell->SetPatchTop(FALSE);
				pCell->SetPatchBottom(FALSE);
				return;
			}
			else //if(dir == CGrid::DIR_BOTTOM)
			{
				AdjustRect(pCell);
				nextCell = GetCell(x, y-1);
				if(nextCell)
					Disconnect(nextCell, CGrid::DIR_BOTTOM);
				pCell->SetPatchTop(FALSE);
				pCell->SetPatchBottom(FALSE);
				return;
			}
		}

		// If the cell is patched left, should that cell be disconnected as well
		if(pCell->GetPatchLeft() && (dir != CGrid::DIR_LEFT))
		{
			nextCell = GetCell(x-1, y);
			if(nextCell)
				Disconnect(nextCell, CGrid::DIR_RIGHT);
		}

		// If the cell is patched right, should that cell be disconnected as well
		if(pCell->GetPatchRight() && (dir != CGrid::DIR_RIGHT))
		{
			nextCell = GetCell(x+1, y);
			if(nextCell)
				Disconnect(nextCell, CGrid::DIR_LEFT);
		}

		// If the cell is patched top, should that cell be disconnected as well
		if(pCell->GetPatchTop() && (dir != CGrid::DIR_TOP))
		{
			nextCell = GetCell(x, y-1);
			if(nextCell)
				Disconnect(nextCell, CGrid::DIR_BOTTOM);
		}

		// If the cell is patched bottom, should that cell be disconnected as well
		if(pCell->GetPatchBottom() && (dir != CGrid::DIR_BOTTOM))
		{
			nextCell = GetCell(x, y+1);
			if(nextCell)
				Disconnect(nextCell, CGrid::DIR_TOP);
		}
	}	
	
	// Delete the cell if it is not part of a module
	if(!pCell->GetModule())
	{
		AdjustRect(pCell);
		SetCell(x, y, NULL);
	}
}

//------------------------------------------------------------------------------
//	CGrid::IsPatched
//------------------------------------------------------------------------------
BOOL
CGrid::IsPatched(
				 IN const CCell*	pCell
				 )
/*++

Routine Description:

	Returns TRUE if the cell is connected to any other cells

Arguments:

	IN pCell -	Cell to check

ReturnValue:

	TRUE if the cell is patched, FALSE otherwise

--*/
{
	CCell*	pLeftCell	= GetCell(pCell->GetXLoc()-1, pCell->GetYLoc());
	CCell*	pRightCell	= GetCell(pCell->GetXLoc()+1, pCell->GetYLoc());
	CCell*	pTopCell	= GetCell(pCell->GetXLoc(), pCell->GetYLoc()-1);
	CCell*	pBottomCell	= GetCell(pCell->GetXLoc(), pCell->GetYLoc()+1);

	return ((pCell->GetPatchLeft() && pLeftCell) ||
			(pCell->GetPatchRight() && pRightCell) ||
			(pCell->GetPatchTop() && pTopCell) ||
			(pCell->GetPatchBottom() && pBottomCell));
}

//------------------------------------------------------------------------------
//	CGrid::GetXOffset
//------------------------------------------------------------------------------
int 
CGrid::GetXOffset(void) const
/*++

Routine Description:

	Returns the grid offset in pixels

Arguments:

	None

ReturnValue:

	The X grid offset in pixels

--*/
{
	return m_cellSize;
}

//------------------------------------------------------------------------------
//	CGrid::GetYOffset
//------------------------------------------------------------------------------
int 
CGrid::GetYOffset(void) const
/*++

Routine Description:

	Returns the grid offset in pixels

Arguments:

	None

ReturnValue:

	The Y grid offset in pixels

--*/
{
	return m_cellSize;
}

//------------------------------------------------------------------------------
//	CGrid::DeleteEffect
//------------------------------------------------------------------------------
void
CGrid::DeleteEffect(
					IN CModule* pEffect
					)
/*++

Routine Description:

	Deletes an effect

Arguments:

	IN pEffect -	Effect to delete

ReturnValue:

	None

--*/
{
	int	x;
	int	y;
	
	// First disconnect the effect
	pEffect->Disconnect();

	// Set the grid to NULL where the effect sits
	for(x=pEffect->GetLeft(); x<=pEffect->GetRight(); ++x)
	{
		for(y=pEffect->GetTop(); y<=pEffect->GetBottom(); ++y)
			SetCell(x, y, NULL);
	}

	// Remove the effect from the effects list
	for(x=0; x<m_numEffects; ++x)
	{
		if(m_pEffects[x] == pEffect)
		{
			for(; x<m_numEffects-1; ++x)
				m_pEffects[x] = m_pEffects[x+1];
			m_pEffects[x]	= NULL;
			m_numEffects	= x;
		}
	}

	// Delete the effect
	delete pEffect;
}

//------------------------------------------------------------------------------
//	CGrid::ResetRect
//------------------------------------------------------------------------------
void
CGrid::ResetRect(void)
/*++

Routine Description:

	Resets the rectangle to invalid value

Arguments:

	None

Return Value:

	None

--*/
{
	m_rect.left		= INT_MAX;
	m_rect.right	= INT_MIN;
	m_rect.top		= INT_MAX;
	m_rect.bottom	= INT_MIN;
}

//------------------------------------------------------------------------------
//	CGrid::AdjustRect
//------------------------------------------------------------------------------
void
CGrid::AdjustRect(
				  IN const CCell*	pCell
				  )
/*++

Routine Description:

	Adjust the rectangle to include the cell

Arguments:

	IN pCell -	Cell to include in the rectangle

Return Value:

	None

--*/
{
	int	v;

	if(!pCell)
		return;

	v = pCell->GetXLoc() * m_cellSize + GetXOffset();
	if(v < m_rect.left)
		m_rect.left = v;

	v += m_cellSize;
	if(v > m_rect.right)
		m_rect.right = v;

	v = pCell->GetYLoc() * m_cellSize + GetYOffset();
	if(v < m_rect.top)
		m_rect.top = v;

	v += m_cellSize;
	if(v > m_rect.bottom)
		m_rect.bottom = v;
}

//------------------------------------------------------------------------------
//	CGrid::AdjustRect
//------------------------------------------------------------------------------
void
CGrid::AdjustRect(
				  IN const CModule*	pModule
				  )
/*++

Routine Description:

	Adjust the rectangle to include the module

Arguments:

	IN pModule -	Module to include in the rectangle

Return Value:

	None

--*/
{
	if(!pModule)
		return;

	if(pModule->GetRect().left < m_rect.left)
		m_rect.left = pModule->GetRect().left;

	if(pModule->GetRect().right > m_rect.right)
		m_rect.right = pModule->GetRect().right;

	if(pModule->GetRect().top < m_rect.top)
		m_rect.top = pModule->GetRect().top;

	if(pModule->GetRect().bottom > m_rect.bottom)
		m_rect.bottom = pModule->GetRect().bottom;
}

//------------------------------------------------------------------------------
//	CGrid::IsCellInRect
//------------------------------------------------------------------------------
BOOL 
CGrid::IsCellInRect(
					IN const CCell*	pCell,
					IN const CRect&	rect
					) const
/*++

Routine Description:

	Returns TRUE if the cell is within the rectangle

Arguments:

	IN pCell -	Cell to test
	IN rect -	Rectangle

Return Value:

	TRUE if the cell is within the rectangle

--*/
{
	CRect	result;
	CRect	cRect(pCell->GetXLoc() * m_cellSize + GetXOffset(),
				  pCell->GetYLoc() * m_cellSize + GetYOffset(),
				  (pCell->GetXLoc()+1) * m_cellSize + GetXOffset() - 1,
				  (pCell->GetYLoc()+1) * m_cellSize + GetYOffset() - 1);

	return result.IntersectRect(rect, cRect);

}

//------------------------------------------------------------------------------
//	CGrid::IsModuleInRect
//------------------------------------------------------------------------------
BOOL 
CGrid::IsModuleInRect(
					  IN const CModule*	pModule,
					  IN const CRect&	rect
					  ) const
/*++

Routine Description:

	Returns TRUE if the module is within the rectangle

Arguments:

	IN pModule -	Module to test
	IN rect -		Rectangle

Return Value:

	TRUE if the module is within the rectangle

--*/
{
	CRect	result;
	return result.IntersectRect(rect, pModule->GetRect());
}

//------------------------------------------------------------------------------
//	CGrid::GetHeight
//------------------------------------------------------------------------------
int
CGrid::GetHeight(void) const
/*++

Routine Description:

	Returns the height of the grid in cells

Arguments:

	None

Return Value:

	The height of the grid in cells

--*/
{
	return m_height;
}

//------------------------------------------------------------------------------
//	CGrid::GetWidth
//------------------------------------------------------------------------------
int
CGrid::GetWidth(void) const
/*++

Routine Description:

	Returns the width of the grid in cells

Arguments:

	None

Return Value:

	The width of the grid in cells

--*/
{
	return m_width;
}

//------------------------------------------------------------------------------
//	CGrid::IsOkToScroll
//------------------------------------------------------------------------------
BOOL 
CGrid::IsOkToScroll(void) const
/*++

Routine Description:

	Returns TRUE if it is ok ot scroll.

Arguments:

	None

Return Value:

	TRUE if it is ok ot scroll.

--*/
{
	return ((m_pMoveModule == NULL) && !m_drawLine);
}

//------------------------------------------------------------------------------
//	CGrid::GetVPMixbinIndex
//------------------------------------------------------------------------------
int 
CGrid::GetVPMixbinIndex(
						IN const CModule*	pModule
						) const
/*++

Routine Description:

	Returns the index into the vp mixbin array or -1 if not found

Arguments:

	IN pModule -	VP Mixbins to lookup

Return Value:

	The index into the vp mixbin array or -1 if not found

--*/
{
	int	i;
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(m_pVPMixbins[i] == pModule)
			return i;
	}
	return -1;
}

//------------------------------------------------------------------------------
//	CGrid::GetGPMixbinIndex
//------------------------------------------------------------------------------
int 
CGrid::GetGPMixbinIndex(
						IN const CModule*	pModule
						) const
/*++

Routine Description:

	Returns the index into the gp mixbin array or -1 if not found

Arguments:

	IN pModule -	GP Mixbins to lookup

Return Value:

	The index into the gp mixbin array or -1 if not found

--*/
{
	int	i;
	for(i=0; i<CGridNumMixbins; ++i)
	{
		if(m_pGPMixbins[i] == pModule)
			return i;
	}
	return -1;
}

//------------------------------------------------------------------------------
//	CGrid::DrawLine
//------------------------------------------------------------------------------
void
CGrid::DrawLine(void)
/*++

Routine Description:

	Draws a rubberband line snapped to grid centers either horizontal or
	vertical

Arguments:

	None

Return Value:

	None

--*/
{
	CClientDC	dc(m_pParent);
	CPen*		oldPen;
	int			oldROP;
	int			xOffset = GetXOffset() + m_cellSize / 2;
	int			yOffset = GetYOffset() + m_cellSize / 2;

	// Setup
	oldPen	= (CPen*)dc.SelectStockObject(WHITE_PEN);
	oldROP	= dc.SetROP2(R2_XORPEN);

	// Draw
	dc.MoveTo(m_moveX * m_cellSize + xOffset, m_moveY * m_cellSize + xOffset);
	dc.LineTo(m_moveDx * m_cellSize + yOffset, m_moveDy * m_cellSize + yOffset);

	// Restore
	dc.SetROP2(oldROP);
	dc.SelectObject(oldPen);
}

//------------------------------------------------------------------------------
//	CGrid::DrawPatchCords
//------------------------------------------------------------------------------
void
CGrid::DrawPatchCords(
					  IN int	x0,
					  IN int	y0,
					  IN int	x1,
					  IN int	y1,
					  IN BOOL	tie
					  )
/*++

Routine Description:

	Draws a series of patch cords between the two points. 
	Currently only horizontal or vertical patch cords are supported

Arguments:

	IN x0 -		From
	IN y0 -		From
	IN x1 -		To
	IN y1 -		To
	IN tie -	TRUE to tie togethor any crossed cords

Return Value:

	None

--*/
{
	CCell*	pCell;
	int		i;

	ResetRect();

	// Is it vertical?
	if(x0 == x1)
	{
		if(y1 < y0)
		{
			i = y1;
			y1 = y0;
			y0 = i;
		}
		else if(y1 == y0)
			return;

		// First and last
		pCell = GetCellAlloc(x0, y0);
		if(!pCell->GetModule())
		{
			pCell->SetPatchBottom(TRUE);
			if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
				pCell->SetJoin(TRUE);
			AdjustRect(pCell);
		}
		pCell = GetCellAlloc(x0, y1);
		if(!pCell->GetModule())
		{
			pCell->SetPatchTop(TRUE);
			if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
				pCell->SetJoin(TRUE);
			AdjustRect(pCell);
		}

		// All others
		for(i=y0+1; i<y1; ++i)
		{
			pCell = GetCellAlloc(x0, i);

			// Don't adjust a module's cells
			if(!pCell->GetModule())
			{
				pCell->SetPatchTop(TRUE);
				pCell->SetPatchBottom(TRUE);
				if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
					pCell->SetJoin(TRUE);
				
			AdjustRect(pCell);
			}
		}
		ValidateGrid();
		Draw(m_rect);		
	}

	// Or horizontal?
	else if(y0 == y1)
	{
		if(x1 < x0)
		{
			i = x1;
			x1 = x0;
			x0 = i;
		}
		else if(x1 == x0)
			return;

		// First and last
		pCell = GetCellAlloc(x0, y0);
		if(!pCell->GetModule())
		{
			pCell->SetPatchRight(TRUE);
			if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
				pCell->SetJoin(TRUE);
			AdjustRect(pCell);
		}
		pCell = GetCellAlloc(x1, y0);
		if(!pCell->GetModule())
		{
			pCell->SetPatchLeft(TRUE);
			if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
				pCell->SetJoin(TRUE);
			AdjustRect(pCell);
		}

		// All others
		for(i=x0+1; i<x1; ++i)
		{
			pCell = GetCellAlloc(i, y0);

			// Don't adjust a module's cells
			if(!pCell->GetModule())
			{
				pCell->SetPatchLeft(TRUE);
				pCell->SetPatchRight(TRUE);
				if((pCell->GetPatchCount() == 3) || ((pCell->GetPatchCount() == 4) && tie))
					pCell->SetJoin(TRUE);
				
			AdjustRect(pCell);
			}
		}
		ValidateGrid();
		Draw(m_rect);		
	}
}

//------------------------------------------------------------------------------
//	CGrid::ValidateGrid
//------------------------------------------------------------------------------
void
CGrid::ValidateGrid(void)
/*++

Routine Description:

	Examines the content of the grid to determine if the layout is valid.
	Any cells that are in question, will be marked.

Arguments:

	None

Return Value:

	None

--*/
{
	int		j;
	int		k;
	int		i;
	int		o;
	CCell*	pCell;
	BOOL	valid;
	
	// Start by marking all cells as UNKNOWN
	for(j=0; j<m_numCells; ++j)
	{
		if(m_pCells[j])// && !m_pCells[i]->GetModule())
		{
			m_pCells[j]->SetState(CCell::UNKNOWN);
			m_pCells[j]->SetStateV(CCell::UNKNOWN);
		}
	}

	// INVALID CHECK: Trace all patch cords from the VP mixbins
	for(j=0; j<CGridNumMixbins; ++j)
	{
		for(k=0; k<CGridNumMixbins; ++k)
		{
			if(j != k)
			{
				// INVALID CHECK: VP mixbins cannot be connected to other VP mixbins
				if(IsCellConnectedToCell(m_pVPMixbins[j]->GetOutputs()[0].pCell, m_pVPMixbins[k]->GetOutputs()[0].pCell, CGrid::DIR_NONE))
					MarkPatchCord(m_pVPMixbins[j]->GetOutputs()[0].pCell, CGrid::DIR_NONE, CCell::INVALID);
			}

		// INVALID CHECK: VP mixbins cannot be directly connected to GP mixbins
		if(IsCellConnectedToCell(m_pVPMixbins[j]->GetOutputs()[0].pCell, m_pGPMixbins[k]->GetInputs()[0].pCell, CGrid::DIR_NONE))
			MarkPatchCord(m_pVPMixbins[j]->GetOutputs()[0].pCell, CGrid::DIR_NONE, CCell::INVALID);
		}
	}

	// INVALID CHECK: Trace all patch cords from the GP mixbins
	for(j=0; j<CGridNumMixbins; ++j)
	{
		for(k=0; k<CGridNumMixbins; ++k)
		{
			if(j != k)
			{
				// INVALID CHECK: GP mixbins cannot be connected to GP mixbins
				if(IsCellConnectedToCell(m_pGPMixbins[j]->GetInputs()[0].pCell, m_pGPMixbins[k]->GetInputs()[0].pCell, CGrid::DIR_NONE))
					MarkPatchCord(m_pGPMixbins[j]->GetInputs()[0].pCell, CGrid::DIR_NONE, CCell::INVALID);
			}
		}
	}

	// INVALID CHECK: Trace all inputs from effects
	for(j=0; j<m_numEffects; ++j)
	{
		// Loop through all inputs
		for(i=0; i<m_pEffects[j]->GetNumInputs(); ++i)
		{
			pCell	= m_pEffects[j]->GetInputs()[i].pCell;
			valid	= TRUE;

			// INVALID CHECK: Input cannot be connected to a GP mixbin
			for(k=0; valid && k<CGridNumMixbins; ++k)
			{
				if(IsCellConnectedToCell(pCell, m_pGPMixbins[k]->GetInputs()[0].pCell, CGrid::DIR_NONE))
				{
					MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::INVALID);
					valid = FALSE;
				}
			}

			// INVALID CHECK: Input cannot be connected to own output (yet)
			for(o=0; valid && o<m_pEffects[j]->GetNumOutputs(); ++o)
			{
				if(IsCellConnectedToCell(pCell, m_pEffects[j]->GetOutputs()[o].pCell, CGrid::DIR_NONE))
				{
					MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::INVALID);
					valid = FALSE;
				}
			}
		}
	}

	// INVALID CHECK: Trace all outputs from effects
	for(j=0; j<m_numEffects; ++j)
	{
		// Loop through all outputs
		for(o=0; o<m_pEffects[j]->GetNumOutputs(); ++o)
		{
			pCell	= m_pEffects[j]->GetOutputs()[o].pCell;
			valid	= TRUE;

			// INVALID CHECK: Output cannot be connected to a VP mixbin
			for(k=0; valid && k<CGridNumMixbins; ++k)
			{
				if(IsCellConnectedToCell(pCell, m_pVPMixbins[k]->GetOutputs()[0].pCell, CGrid::DIR_NONE))
				{
					MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::INVALID);
					valid = FALSE;
				}
			}

			// INVALID CHECK: Input cannot be connected to own input (yet) <- this is done above
		}
	}


	// VALID CHECK: Trace all inputs from effects
	for(j=0; j<m_numEffects; ++j)
	{
		// Loop through all inputs
		for(i=0; i<m_pEffects[j]->GetNumInputs(); ++i)
		{
			pCell	= m_pEffects[j]->GetInputs()[i].pCell;
			valid	= TRUE;

			// VALID CHECK: Is it connected to a VP mixin?
			for(k=0; valid && k<CGridNumMixbins; ++k)
			{
				if(IsCellConnectedToCell(pCell, m_pVPMixbins[k]->GetOutputs()[0].pCell, CGrid::DIR_NONE))
				{
					MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::OK);
					valid = FALSE;
				}
			}

			// VALID CHECK: Is it connected to another effect output?
			for(k=0; valid && k<m_numEffects; ++k)
			{
				if(j != k)
				{
					for(o=0; o<m_pEffects[k]->GetNumOutputs(); ++o)
					{
						if(IsCellConnectedToCell(pCell, m_pEffects[k]->GetOutputs()[o].pCell, CGrid::DIR_NONE))
						{
							MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::OK);
							valid = FALSE;
						}
					}
				}
			}
		}
	}

	// VALID CHECK: Trace all outputs from effects
	for(j=0; j<m_numEffects; ++j)
	{
		// Loop through all outputs
		for(o=0; o<m_pEffects[j]->GetNumOutputs(); ++o)
		{
			pCell	= m_pEffects[j]->GetOutputs()[o].pCell;
			valid	= TRUE;

			// VALID CHECK: Is it connected to a GP mixin?
			for(k=0; valid && k<CGridNumMixbins; ++k)
			{
				if(IsCellConnectedToCell(pCell, m_pGPMixbins[k]->GetInputs()[0].pCell, CGrid::DIR_NONE))
				{
					MarkPatchCord(pCell, CGrid::DIR_NONE, CCell::OK);
					valid = FALSE;
				}
			}

			// VALID CHECK: Is it connected to another effect input? <- This is checked above
		}
	}

	// NO_SIGNAL CHECK: Any cords left over should be marked as no signal
	for(j=0; j<m_numCells; ++j)
	{
		if(m_pCells[j])// && !m_pCells[j]->GetModule())
		{
			if(m_pCells[j]->GetState() == CCell::UNKNOWN)
			{	
				AdjustRect(m_pCells[j]);
				m_pCells[j]->SetState(CCell::NO_SIGNAL);
			}
			if(m_pCells[j]->GetStateV() == CCell::UNKNOWN)
			{	
				AdjustRect(m_pCells[j]);
				m_pCells[j]->SetStateV(CCell::NO_SIGNAL);
			}
		}
	}
}

//------------------------------------------------------------------------------
//	CGrid::IsCellConnectedToCell
//------------------------------------------------------------------------------
BOOL 
CGrid::IsCellConnectedToCell(
							 IN const CCell*		pCell0,
							 IN const CCell*		pCell1,
							 IN CGrid::Direction	dir,
							 IN CGrid::PassThrough	passThrough
							 )
/*++

Routine Description:

	Determines if a cell is connected to another cell, passing through effects
	if necessary

Arguments:

	IN pCell0 -			Cell 0
	IN pCell1 -			Cell 1
	IN dir -			Direction of movement
	IN passThrough -	Direction of pass through, NONE, INOUT, or OUTINT

Return Value:

	None

--*/
{
	int		x;	
	int		y;
	int		i;
	BOOL	topBottom;
	BOOL	leftRight;
	CCell*	pNextCell;

	// Initialize the stack
	if(dir == CGrid::DIR_NONE)
		m_si = 0;
	
	// Do the cells match?
	if(pCell0 == pCell1)
		return TRUE;

	// Is the cell patched?
	if(!pCell0->IsPatched())
		return FALSE;
	
	// Has this cell already been processed (check for overflow)?
	if(IsCellInStack(pCell0) || (m_si == 512))
		return FALSE;

		// Insert the cell into the stack
	m_stack[m_si++] = (CCell*)pCell0;

	x			= pCell0->GetXLoc();	
	y			= pCell0->GetYLoc();
	topBottom	= TRUE;
	leftRight	= TRUE;

	if((pCell0->GetPatchCount() == 4) && !pCell0->IsJoined())
	{
		if((dir == CGrid::DIR_TOP) || (dir == CGrid::DIR_BOTTOM))
			leftRight = FALSE;
		else
			topBottom = FALSE;
	}

	if(leftRight)
	{
		if(pCell0->GetPatchLeft() && (dir != CGrid::DIR_LEFT))
		{
			pNextCell = GetCell(x-1, y);
			if(pNextCell && IsCellConnectedToCell(pNextCell, pCell1, CGrid::DIR_RIGHT, passThrough))
				return TRUE;
		}
		else if(pCell0->GetModule() && (passThrough == CGrid::PASS_OUTIN) && (dir == CGrid::DIR_RIGHT))
		{
			for(i=0; i<pCell0->GetModule()->GetNumInputs(); ++i)
			{
				if(IsCellConnectedToCell(pCell0->GetModule()->GetInputs()[i].pCell,
										 pCell1, CGrid::DIR_RIGHT, passThrough))
				{
					return TRUE;
				}
			}
		}

		if(pCell0->GetPatchRight() && (dir != CGrid::DIR_RIGHT))
		{
			pNextCell = GetCell(x+1, y);
			if(pNextCell && IsCellConnectedToCell(pNextCell, pCell1, CGrid::DIR_LEFT, passThrough))
				return TRUE;
		}
		else if(pCell0->GetModule() && (passThrough == CGrid::PASS_INOUT) && (dir == CGrid::DIR_LEFT))
		{
			for(i=0; i<pCell0->GetModule()->GetNumOutputs(); ++i)
			{
				if(IsCellConnectedToCell(pCell0->GetModule()->GetOutputs()[i].pCell,
										 pCell1, CGrid::DIR_LEFT, passThrough))
				{
					return TRUE;
				}
			}
		}
	}

	if(topBottom)
	{
		if(pCell0->GetPatchTop() && (dir != CGrid::DIR_TOP))
		{
			pNextCell = GetCell(x, y-1);
			if(pNextCell && IsCellConnectedToCell(pNextCell, pCell1, CGrid::DIR_BOTTOM, passThrough))
				return TRUE;
		}
		if(pCell0->GetPatchBottom() && (dir != CGrid::DIR_BOTTOM))
		{
			pNextCell = GetCell(x, y+1);
			if(pNextCell && IsCellConnectedToCell(pNextCell, pCell1, CGrid::DIR_TOP, passThrough))
				return TRUE;
		}
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CCell::IsCellInStack
//------------------------------------------------------------------------------
BOOL 
CGrid::IsCellInStack(
					 IN const CCell*	pCell
					 ) const
/*++

Routine Description:

	Returns TRUE if the cell is in the stack

Arguments:

	IN pCell -	Cell to look up

Return Value:

	TRUE if the cell is in the stack, FALSE otherwise

--*/
{
	int	i;
	for(i=m_si-1; i>=0; --i)
	{
		if(m_stack[i] == pCell)
			return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::MarkPatchCord
//------------------------------------------------------------------------------
void
CGrid::MarkPatchCord(
					 IN CCell*				pCell,
					 IN CGrid::Direction	dir,
					 IN CCell::State		state,
					 IN CCell::State		ifState,
					 IN BOOL				markIfState
					 )
/*++

Routine Description:

	Marks a patchcord with a given state

Arguments:

	IN pCell -			A cell of the patchcord to mark
	IN dir -			Direction of movement
	IN state -			State to mark all cells in the patchcord
	IN ifState -		State to compare against
	IN markIfState -	If TRUE, the cell will be marked if it is equal
						to ifState, if FALSE, the cell will be marked if it 
						is not equal to ifState

Return Value:

	None

--*/
{
	int		x;	
	int		y;
	BOOL	topBottom;
	BOOL	leftRight;
	CCell*	pNextCell;

	// Initialize the stack
	if(dir == CGrid::DIR_NONE)
		m_si = 0;
	
	// Is the cell patched?
	if(!pCell->IsPatched())
		return;

	// Has this cell already been processed (check for overflow)?
	if(IsCellInStack(pCell) || (m_si == 512))
		return;

	// Insert the cell into the stack
	m_stack[m_si++] = pCell;

	x			= pCell->GetXLoc();	
	y			= pCell->GetYLoc();
	topBottom	= TRUE;
	leftRight	= TRUE;
	
	if((pCell->GetPatchCount() == 4) && !pCell->IsJoined())
	{
		if((dir == CGrid::DIR_TOP) || (dir == CGrid::DIR_BOTTOM))
			leftRight = FALSE;
		else
			topBottom = FALSE;
	}

	if(leftRight)
	{
		if((markIfState && (pCell->GetState() == ifState)) ||
		   (!markIfState && (pCell->GetState() != ifState)))
		{
			pCell->SetState(state);
		}
		AdjustRect(pCell);
		if(pCell->GetPatchLeft() && (dir != CGrid::DIR_LEFT))
		{
			pNextCell = GetCell(x-1, y);
			if(pNextCell)
				MarkPatchCord(pNextCell, CGrid::DIR_RIGHT, state, ifState, markIfState);
		}
		if(pCell->GetPatchRight() && (dir != CGrid::DIR_RIGHT))
		{
			pNextCell = GetCell(x+1, y);
			if(pNextCell)
				MarkPatchCord(pNextCell, CGrid::DIR_LEFT, state, ifState, markIfState);
		}
	}

	if(topBottom)
	{
		if((markIfState && (pCell->GetStateV() == ifState)) ||
		   (!markIfState && (pCell->GetStateV() != ifState)))
		{
			pCell->SetStateV(state);
		}
		AdjustRect(pCell);
		if(pCell->GetPatchTop() && (dir != CGrid::DIR_TOP))
		{
			pNextCell = GetCell(x, y-1);
			if(pNextCell)
				MarkPatchCord(pNextCell, CGrid::DIR_BOTTOM, state, ifState, markIfState);
		}
		if(pCell->GetPatchBottom() && (dir != CGrid::DIR_BOTTOM))
		{
			pNextCell = GetCell(x, y+1);
			if(pNextCell)
				MarkPatchCord(pNextCell, CGrid::DIR_TOP, state, ifState, markIfState);
		}
	}
}

//------------------------------------------------------------------------------
//	CGrid::BuildEffectChains
//------------------------------------------------------------------------------
void 
CGrid::BuildEffectChains(
						 IN CStdioFile&	file
						 )
/*++

Routine Description:

	Builds a list of effect chains by examining the grid.
	This method assumes there are no invalid cells in the grid.

Arguments:

	IN file -	File to write the data to

Return Value:

	None

--*/
{
	int					e;
	int					i;
	int					o;
	int					m;
	int					b;
	int					ii;
	int					oo;
	int					bb;
	int					oldTempMixbin;
	int					tempMixbin = -1;
	CModule*			pEffect;
	int					index;
	int					tm;
	EffectChainBucket	bucket;
	BOOL				found;
	int*				bins;
	int					numBins;
	BOOL				nullMixbin = FALSE;
	CArray<EffectChainBucket, EffectChainBucket&> buckets;

	// First initialize all connections
	for(e=0; e<m_numEffects; ++e)
	{
		for(i=0; i<m_pEffects[e]->GetNumInputs(); ++i)
			m_pEffects[e]->GetInputs()[i].mixbin = INT_MAX;
		for(o=0; o<m_pEffects[e]->GetNumOutputs(); ++o)
			m_pEffects[e]->GetOutputs()[o].mixbin = INT_MAX;
	}

	// Build the connection graph
	for(e=0; e<m_numEffects; ++e)
	{
		// Only process connected effects
		if(m_pEffects[e]->IsConnected())
		{
			// Check all the effect's inputs
			for(i=0; i<m_pEffects[e]->GetNumInputs(); ++i)
			{
				// Only process unconnected inputs
				if(m_pEffects[e]->GetInputs()[i].mixbin == INT_MAX)
				{
					// Is the input connected to a vp mixbin?
					m = GetMixbin(m_pEffects[e]->GetInputs()[i].pCell, TRUE);
					if(m != -1)
						m_pEffects[e]->GetInputs()[i].mixbin = m;
					
					// Or is it connected to another effect?
					else if(GetEffect(m_pEffects[e]->GetInputs()[i].pCell, FALSE, FALSE, pEffect, index))
					{
						if(pEffect->GetOutputs()[index].mixbin != INT_MAX)
							tm = pEffect->GetOutputs()[index].mixbin;
						else
							tm = INT_MAX;

						// Examine all other connections to see if a temp mixbin has already been setup
						while((tm == INT_MAX) && GetEffect(m_pEffects[e]->GetInputs()[i].pCell, FALSE, TRUE, pEffect, index))
						{
							if(pEffect->GetOutputs()[index].mixbin != INT_MAX)
								tm = pEffect->GetOutputs()[index].mixbin;
						}
						
						// If a temp mixbin was not found, set it
						if(tm == INT_MAX)
							tm = tempMixbin--;

						// Now, set all the temp mixbin id's
						GetEffect(m_pEffects[e]->GetInputs()[i].pCell, FALSE, FALSE, pEffect, index);

						m_pEffects[e]->GetInputs()[i].mixbin	= tm;
						pEffect->GetOutputs()[index].mixbin		= tm;

						// Find all other effects connected to this cell and
						// mark that they will be using the same temp mixbin
						while(GetEffect(m_pEffects[e]->GetInputs()[i].pCell, FALSE, TRUE, pEffect, index))
							pEffect->GetOutputs()[index].mixbin		= tm;
					}
					else
						nullMixbin = TRUE;
				}
			}

			// Check all the effect's outputs
			for(o=0; o<m_pEffects[e]->GetNumOutputs(); ++o)
			{
				// Only process unconnected outputs
				if(m_pEffects[e]->GetOutputs()[o].mixbin == INT_MAX)
				{
					// Is the outputs connected to a gp mixbin?
					m = GetMixbin(m_pEffects[e]->GetOutputs()[o].pCell, FALSE);
					if(m != -1)
						m_pEffects[e]->GetOutputs()[o].mixbin = m;
					
					// Or is it connected to another effect?
					else if(GetEffect(m_pEffects[e]->GetOutputs()[o].pCell, TRUE, FALSE, pEffect, index))
					{
						if(pEffect->GetInputs()[index].mixbin != INT_MAX)
							tm = pEffect->GetInputs()[index].mixbin;
						else
							tm = INT_MAX;

						// Examine all other connections to see if a temp mixbin has already been setup
						while((tm == INT_MAX) && GetEffect(m_pEffects[e]->GetOutputs()[o].pCell, TRUE, TRUE, pEffect, index))
						{
							if(pEffect->GetInputs()[index].mixbin != INT_MAX)
								tm = pEffect->GetInputs()[index].mixbin;
						}
						
						// If a temp mixbin was not found, set it
						if(tm == INT_MAX)
							tm = tempMixbin--;

						// Now, set all the temp mixbin id's
						GetEffect(m_pEffects[e]->GetOutputs()[o].pCell, TRUE, FALSE, pEffect, index);

						m_pEffects[e]->GetOutputs()[o].mixbin	= tm;
						pEffect->GetInputs()[index].mixbin		= tm;

						// Find all other effects connected to this cell and
						// mark that they will be using the same temp mixbin
						while(GetEffect(m_pEffects[e]->GetOutputs()[o].pCell, TRUE, TRUE, pEffect, index))
							pEffect->GetInputs()[index].mixbin = tm;
					}
					else
						nullMixbin = TRUE;
				}
			}
		}
	}

	// Build a list of used tempmixbins
	numBins = -(tempMixbin + 1);
	bins = new int [numBins];
	for(b=0; b<numBins; ++b)
		bins[b] = 0;
	
	// Sort the effects into buckets to build the chains
	for(e=0; e<m_numEffects; ++e)
	{
		// Only process connected effects
		if(m_pEffects[e]->IsConnected())
		{
			// Find the right bucket to insert the effect into
			for(found=FALSE, b=0; !found && b<buckets.GetSize(); ++b)
			{
				// The first bucket that this effect does not have any 
				// dependencies and insert it
				for(found=TRUE, i=0; found && i<buckets[b].effects.GetSize(); ++i)
				{
					if(DoesEffectDependOnEffect(m_pEffects[e], buckets[b].effects[i]))
						found = FALSE;
				}

				// Was a bucket found?
				if(found)
				{
					// Search for reverse dependencies
					for(i=0; found && i<buckets[b].effects.GetSize(); ++i)
					{
						if(DoesEffectDependOnEffect(buckets[b].effects[i], m_pEffects[e]))
							found = FALSE;
					}

					// If no reverse dependencies were found, insert the effect into this bucket
					if(found)
						buckets[b].effects.InsertAt(buckets[b].effects.GetSize(), m_pEffects[e]);

					// Otherwise, insert a new bucket and place the effect into it
					else
					{
						bucket.effects.RemoveAll();
						bucket.inputs.RemoveAll();
						bucket.outputs.RemoveAll();
						bucket.effects.InsertAt(0, m_pEffects[e]);
						buckets.InsertAt(b, bucket);
						found = TRUE;
					}
				}
			}

			// Was the effect inserted?
			if(!found)
			{
				bucket.effects.RemoveAll();
				bucket.inputs.RemoveAll();
				bucket.outputs.RemoveAll();
				bucket.effects.InsertAt(0, m_pEffects[e]);
				buckets.InsertAt(b, bucket);
			}
		}
	}

	// Step through all the buckets and update the input and output lists
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Step through the effects in the bucket
		for(e=0; e<buckets[b].effects.GetSize(); ++e)
		{
			// Look through the inputs of the effect
			for(i=0; i<buckets[b].effects[e]->GetNumInputs(); ++i)
			{
				// Is the input in the inputs list?
				for(ii=0; ii<buckets[b].inputs.GetSize(); ++ii)
				{
					if(buckets[b].effects[e]->GetInputs()[i].mixbin == buckets[b].inputs[ii])
						break;
				}

				// Not in the list? Add it
				if(ii == buckets[b].inputs.GetSize())
				{
					buckets[b].inputs.InsertAt(buckets[b].inputs.GetSize(), 
											   buckets[b].effects[e]->GetInputs()[i].mixbin);
				}
			}

			// Look through the outputs of the effect
			for(o=0; o<buckets[b].effects[e]->GetNumOutputs(); ++o)
			{
				// Is the input in the inputs list?
				for(oo=0; oo<buckets[b].outputs.GetSize(); ++oo)
				{
					if(buckets[b].effects[e]->GetOutputs()[o].mixbin == buckets[b].outputs[oo])
						break;
				}

				// Not in the list? Add it
				if(oo == buckets[b].outputs.GetSize())
				{
					buckets[b].outputs.InsertAt(buckets[b].outputs.GetSize(), 
											    buckets[b].effects[e]->GetOutputs()[o].mixbin);
				}
			}
		}
	}

	// Remove extra temp mixbins
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Examine all the outputs in this bucket
		for(o=0; o<buckets[b].outputs.GetSize(); ++o)
		{
			// Only examine temp mixbins (negative values)
			if(buckets[b].outputs[o] < 0)
			{
				// Step through all previous buckets, looking for a bucket
				// with a duplicate output. If a duplicate is found, this temp mixbin
				// must be kept. If one is not found, try to find an available temp mixbin
				// that is no longer in use
				for(found=FALSE, bb=0; !found && bb<b; ++bb)
				{
					// Examine all the outputs of the bucket
					for(oo=0; oo<buckets[bb].outputs.GetSize(); ++oo)
					{
						if(buckets[bb].outputs[oo] == buckets[b].outputs[o])
							found = TRUE;
					}
				}

				// If not found, try to replace the temp mixbin
				if(!found)
				{
					// Start with the lowest numbered mixbin
					tempMixbin = -1;

					// If the search makes it's way to this mixbin, just bail
					found = TRUE;
					while(found && (buckets[b].outputs[o] != tempMixbin))
					{
						// Otherwise, look at this and all subsequent mixbins
						// and see if any rely on tempMixbin as an input
						// If none do, tempMixbin is a good choice
						for(bb=b; bb<buckets.GetSize(); ++bb)
						{
							// Examine all the inputs in the bucket
							for(i=0; i<buckets[bb].inputs.GetSize(); ++i)
							{
								if(buckets[bb].inputs[i] == tempMixbin)
									break;
							}
							if(i != buckets[bb].inputs.GetSize())
								break;
						}
						if(bb == buckets.GetSize())
							found = FALSE;
						else
							--tempMixbin;
					}

					// Is tempMixbin a good new choice?
					if(!found)
					{
						// Replace all occurences of the mixbin with
						// tempMixbin in this and all subsequent buckets
						oldTempMixbin = buckets[b].outputs[o];
						for(bb=b; bb<buckets.GetSize(); ++bb)
						{
							// Replace all inputs
							for(ii=0; ii<buckets[bb].inputs.GetSize(); ++ii)
							{
								if(buckets[bb].inputs[ii] == oldTempMixbin)
									buckets[bb].inputs[ii] = tempMixbin;
							}

							// Replace all outputs
							for(oo=0; oo<buckets[bb].outputs.GetSize(); ++oo)
							{
								if(buckets[bb].outputs[oo] == oldTempMixbin)
									buckets[bb].outputs[oo] = tempMixbin;
							}

							// Do the same for the real effects in the bucket
							for(e=0; e<buckets[bb].effects.GetSize(); ++e)
							{
								for(ii=0; ii<buckets[bb].effects[e]->GetNumInputs(); ++ii)
								{
									if(buckets[bb].effects[e]->GetInputs()[ii].mixbin == oldTempMixbin)
										buckets[bb].effects[e]->GetInputs()[ii].mixbin = tempMixbin;
								}
								for(oo=0; oo<buckets[bb].effects[e]->GetNumOutputs(); ++oo)
								{
									if(buckets[bb].effects[e]->GetOutputs()[oo].mixbin == oldTempMixbin)
										buckets[bb].effects[e]->GetOutputs()[oo].mixbin = tempMixbin;
								}
							}
						}
					}
				}
			}
		}
	}

	// Figure out which temp mixbins are being used
	for(b=0; b<buckets.GetSize(); ++b)
	{
		// Step through all the effects in the bucket
		for(e=0; e<buckets[b].effects.GetSize(); ++e)
		{
			// Step through all the inputs
			for(i=0; i<buckets[b].effects[e]->GetNumInputs(); ++i)
			{
				// Is this a temp mixbin?
				if(buckets[b].effects[e]->GetInputs()[i].mixbin < 0)
					++bins[-(buckets[b].effects[e]->GetInputs()[i].mixbin + 1)];
			}

			// Step through all the outputs
			for(o=0; o<buckets[b].effects[e]->GetNumOutputs(); ++o)
			{
				// Is this a temp mixbin?
				if(buckets[b].effects[e]->GetOutputs()[o].mixbin < 0)
					++bins[-(buckets[b].effects[e]->GetOutputs()[o].mixbin + 1)];
			}
		}
	}

	// Remove unused tempmixbins to compress the list of bins
	for(tm=0, bb=0; bb<numBins; ++bb)
	{
		// If this mixbin is not used, decrement all other tempmixbins larger than this
		if(bins[bb] == 0)
		{
			tempMixbin = -(bb + 1);
			for(b=0; b<buckets.GetSize(); ++b)
			{
				// Step through all the effects in the bucket
				for(e=0; e<buckets[b].effects.GetSize(); ++e)
				{
					// Step through all the inputs
					for(i=0; i<buckets[b].effects[e]->GetNumInputs(); ++i)
					{
						// Is this a temp mixbin, and is does it need adjusting?
						if(buckets[b].effects[e]->GetInputs()[i].mixbin < tempMixbin)
							++buckets[b].effects[e]->GetInputs()[i].mixbin;
					}

					// Step through all the outputs
					for(o=0; o<buckets[b].effects[e]->GetNumOutputs(); ++o)
					{
						// Is this a temp mixbin, and is does it need adjusting?
						if(buckets[b].effects[e]->GetOutputs()[o].mixbin < tempMixbin)
							++buckets[b].effects[e]->GetOutputs()[o].mixbin;
					}
				}
			}

		}
		else
			++tm;
	}
	
	// Write the file
	try
	{
		CString	string;

		// header
		file.WriteString(_T("[MAIN]\n\n"));

		// Name it the same as the ini file
		string.Format(_T("IMAGE_FRIENDLY_NAME=%s\n"), file.GetFileTitle());

		// CString::GetFileTitle() is not supposed to return a file extension but is
		// The following lines remove the extension if it exists
		i = string.Find(_T(".ini\n"));
		if(i == (string.GetLength() - 5))
			string.Delete(i, 4);

		file.WriteString(string);

		// Number of temp mixbins
		string.Format(_T("FX_NUMTEMPBINS=%d\n"), nullMixbin ? tm+1 : tm);
		file.WriteString(string);

		// Write the graph names
		for(b=0; b<buckets.GetSize(); ++b)
		{
			string.Format(_T("GRAPH%d=Graph%d\n"), b, b);
			file.WriteString(string);
		}
		file.WriteString(_T("\n"));

		// Write the graphs
		for(b=0; b<buckets.GetSize(); ++b)
		{
			string.Format(_T("[Graph%d]\n"), b);
			file.WriteString(string);
			for(e=0; e<buckets[b].effects.GetSize(); ++e)
			{
				string.Format(_T("FX%d=%s\n"), e, buckets[b].effects[e]->GetName());
				file.WriteString(string);
			}
			file.WriteString(_T("\n"));
		}

		for(b=0; b<buckets.GetSize(); ++b)
		{
			for(e=0; e<buckets[b].effects.GetSize(); ++e)
			{
				pEffect = buckets[b].effects[e];
				string.Format(_T("[Graph%d_FX%d_%s]\n\n"), b, e, pEffect->GetName());
				file.WriteString(string);
				file.WriteString(_T("FX_MIXOUTPUT=1\n"));
				string.Format(_T("FX_DSPCODE=%s\n"), pEffect->GetDSPCode());
				file.WriteString(string);
				string.Format(_T("FX_DSPSTATE=%s\n"), pEffect->GetStateFilename());
				file.WriteString(string);
				string.Format(_T("FX_NUMINPUTS=%d\n"), pEffect->GetNumInputs());
				file.WriteString(string);
				string.Format(_T("FX_NUMOUTPUTS=%d\n"), pEffect->GetNumOutputs());
				file.WriteString(string);
				for(i=0; i<pEffect->GetNumInputs(); ++i)
				{
					if(pEffect->GetInputs()[i].mixbin == INT_MAX)
						string.Format("FX_INPUT%d=GPTEMPBIN%d\n", i, tm);
					else if(pEffect->GetInputs()[i].mixbin >= 0)
						string.Format("FX_INPUT%d=VPMIXBIN_%s\n", i, m_pVPMixbins[pEffect->GetInputs()[i].mixbin]->GetName());
					else
						string.Format("FX_INPUT%d=GPTEMPBIN%d\n", i, -(pEffect->GetInputs()[i].mixbin + 1));
					file.WriteString(string);
				}
				for(o=0; o<pEffect->GetNumOutputs(); ++o)
				{
					if(pEffect->GetOutputs()[o].mixbin == INT_MAX)
						string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o, tm);
					else if(pEffect->GetOutputs()[o].mixbin >= 0)
						string.Format("FX_OUTPUT%d=GPMIXBIN_%s\n", o, m_pVPMixbins[pEffect->GetOutputs()[o].mixbin]->GetName());
					else
						string.Format("FX_OUTPUT%d=GPTEMPBIN%d\n", o, -(pEffect->GetOutputs()[o].mixbin + 1));
					file.WriteString(string);
				}
				file.WriteString(_T("\n"));
			}
		}
	}
	catch (CFileException e)
	{
		AfxMessageBox(CString(_T("Error writing DSP Image File: ")) + file.GetFilePath(), MB_OK | MB_ICONEXCLAMATION);
	}
}

//------------------------------------------------------------------------------
//	CGrid::IsEffectConnectedToEffect
//------------------------------------------------------------------------------
BOOL 
CGrid::IsEffectConnectedToEffect(
								 IN const CModule*	pE0,
								 IN const CModule*	pE1
								 )
/*++

Routine Description:

	Returns TRUE if the effects are connected (in-out, in-in, out-out)

Arguments:

	IN pE0 -	Effect 0
	IN pE1 -	Effect 1

Return Value:

	TRUE if the effects are connected, FALSE otherwise

--*/
{
	int	i;
	int	o;

	for(i=0; i<pE0->GetNumInputs(); ++i)
	{
		for(o=0; o<pE1->GetNumOutputs(); ++o)
		{
			if(IsCellConnectedToCell(pE0->GetInputs()[i].pCell, 
									 pE1->GetOutputs()[o].pCell))
			{
				return TRUE;
			}
		}
	}

	for(i=0; i<pE1->GetNumInputs(); ++i)
	{
		for(o=0; o<pE0->GetNumOutputs(); ++o)
		{
			if(IsCellConnectedToCell(pE1->GetInputs()[i].pCell, 
									 pE0->GetOutputs()[o].pCell))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::IsEffectUnconnected
//------------------------------------------------------------------------------
BOOL 
CGrid::IsEffectUnconnected(
						   IN const CModule*	pEffect
						   ) const
/*++

Routine Description:

	Checks to see if an effect is unconnected

Arguments:

	IN pEffect -	Effect to test

Return Value:

	TRUE if the effect is unconnected, FALSE otherwise

--*/
{
	int	i;

	for(i=0; i<pEffect->GetNumInputs(); ++i)
	{
		if(pEffect->GetInputs()[i].pCell->GetState() == CCell::OK)
			return FALSE;
	}
	for(i=0; i<pEffect->GetNumOutputs(); ++i)
	{
		if(pEffect->GetOutputs()[i].pCell->GetState() == CCell::OK)
			return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::GetEffect
//------------------------------------------------------------------------------
BOOL
CGrid::GetEffect(
				 IN const CCell*	pCell,
				 IN BOOL			input,
				 IN BOOL			getNext,
				 OUT CModule*&		pModule,
				 OUT int&			index
				 )
/*++

Routine Description:

	Returns an effect connected to the cell.

Arguments:

	IN pCell -		Cell to get effect connected to
	IN input -		TRUE to examine inputs, FALSE to examine outputs
	IN getNext -	When TRUE, gets the next effect connected to the cell,
					When FALSE, gets the first effect connected to the cell
	OUT pModule -	Effect
	OUT index -		Index to the connection

Return Value:

	TRUE if an effect was found

--*/
{
	static int				effectNum;
	static int				connectionNum;
	CModule::Connection*	connections;
	int						numConnections;
	
	// Start from the begining?
	if(!getNext)
	{
		effectNum		= 0;
		connectionNum	= 0;
	}

	// Step through all the effects
	for(; effectNum < m_numEffects; ++effectNum)
	{
		pModule = m_pEffects[effectNum];

		// Ignore the module of pCell
		if(pModule != pCell->GetModule())
		{
			// Get the list of connections
			if(input)
			{
				connections		= pModule->GetInputs();
				numConnections	= pModule->GetNumInputs();
			}
			else
			{
				connections		= pModule->GetOutputs();
				numConnections	= pModule->GetNumOutputs();
			}

			// Examine the connections
			for(; connectionNum < numConnections; ++connectionNum)
			{
				if(IsCellConnectedToCell(pCell, connections[connectionNum].pCell))
				{
					index = connectionNum++;
					return TRUE;
				}
			}
		}
		connectionNum = 0;
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::GetMixbin
//------------------------------------------------------------------------------
int
CGrid::GetMixbin(
				 IN const CCell*	pCell,
				 IN BOOL			vp
				 )
/*++

Routine Description:

	Returns the mixbin id (vp or gp) that the cell is connected to or -1

Arguments:

	IN pCell -	Cell to lookup
	IN vp -		TRUE to examine VP mixbins, FALSE to examine GP mixbins

Return Value:

	The mixbin id, or -1 if not connected

--*/
{
	int	i;	

	if(vp)
	{
		for(i=0; i<CGridNumMixbins; ++i)
		{
			if(IsCellConnectedToCell(pCell, m_pVPMixbins[i]->GetOutputs()[0].pCell))
				return i;
		}
	}
	else
	{
		for(i=0; i<CGridNumMixbins; ++i)
		{
			if(IsCellConnectedToCell(pCell, m_pGPMixbins[i]->GetInputs()[0].pCell))
				return i;
		}
	}
	return -1;
}

//------------------------------------------------------------------------------
//	CGrid::DoesEffectDependOnEffect
//------------------------------------------------------------------------------
BOOL 
CGrid::DoesEffectDependOnEffect(
								IN const CModule*	pE0,
								IN const CModule*	pE1
								)
/*++

Routine Description:

	Tests to see if effect pE0 depends on pE1. A dependency exists if any input
	of pE0 can be traced to an output (possibly through other effects) of pE1.

Arguments:

	IN pE0 -	Effect to lookup
	IN pE1 -	Effect to attempt to trace to

ReturnValue:

	TRUE if pE0 depends on pE1, FALSE otherwise

--*/
{
	int	i;
	int	o;

	for(i=0; i<pE0->GetNumInputs(); ++i)
	{
		for(o=0; o<pE1->GetNumOutputs(); ++o)
		{
			if(IsCellConnectedToCell(pE0->GetInputs()[i].pCell, 
									 pE1->GetOutputs()[o].pCell, 
									 CGrid::DIR_NONE, CGrid::PASS_OUTIN))
			{
				return TRUE;
			}
		}
	}

	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::IsGridValid
//------------------------------------------------------------------------------
BOOL 
CGrid::IsGridValid(void)
/*++

Routine Description:

	Checks to see if the grid if valid. A grid is valid if there are no invalid
	cells

Arguments:

	None

Return Value:

	TRUE if the grid is valid, FALSE otherwise

--*/
{
	int	c;

	ValidateGrid();
	for(c=0; c<m_numCells; ++c)
	{
		if(m_pCells[c])
		{
			if(m_pCells[c]->GetState() == CCell::INVALID)
				return FALSE;
		}
	}
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::SaveImage
//------------------------------------------------------------------------------
void 
CGrid::SaveImage(
				 IN LPCTSTR	pFilename
				 )
/*++

Routine Description:

	Saves the grid as a dsp image.

Arguments:

	IN pFilename -	Filename to save the grid as

Return Value:

	None

--*/
{
	if(IsGridValid())
	{
		CStdioFile	file;
		if(file.Open(pFilename, CFile::modeCreate | CFile::modeWrite | CFile::typeText))
		{
			BuildEffectChains(file);
			file.Close();
		}
	}

	else
	{
		AfxMessageBox("The grid is invalid and cannot be saved as a DSP Image.", 
					  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
	}
}

//------------------------------------------------------------------------------
//	CGrid::LoadEffects
//------------------------------------------------------------------------------
BOOL
CGrid::LoadEffects(void)
/*++

Routine Description:

	Loads all effects defined by ".ini" files in the ENV_VAR_INI_PATH directory

Arguments:

	None

Return Value:

	TRUE on success, FALSE otherwise

--*/
{
	CHAR*			lpResult;
	HANDLE			fileHandle;
	TCHAR*			lpDir;
	CString			string;
	CString			pathName;
	WIN32_FIND_DATA	findData;
	int				i;
	int				e;
	int				dspCycles;
	int				yMemSize;
	int				numInputs;
	int				numOutputs;
	CString			name;
	CString			dspCode;
	CStringArray	inputs;
	CStringArray	outputs;
	
	// Free old effects
	if(m_pEffectList)
	{
		for(e=0; e<m_numEffectsInList; ++e)
			delete m_pEffectList[e];
		delete [] m_pEffectList;
		m_pEffectList		= NULL;
		m_numEffectsInList	= 0;
	}

	// If the environment directory is set, use it, 
	// otherwise the current dir is used
	lpDir = new TCHAR [256];
	if(GetEnvironmentVariable(ENV_VAR_INI_PATH, lpDir, 256) == 0)
		lpDir[0] = '\0';
	
	// First pass: count the number of ini files to read
	pathName.Format(_T("%s\\*.ini"), lpDir);
	fileHandle = FindFirstFile(pathName, &findData);
	if(fileHandle == INVALID_HANDLE_VALUE)
	{
		delete [] lpDir;
		return FALSE;
	}
	do
	{
		++m_numEffectsInList;
	}
	while(FindNextFile(fileHandle, &findData));
	FindClose(fileHandle);

	// Allocate effects list
	m_pEffectList = new CModule* [m_numEffectsInList];
	for(e=0; e<m_numEffectsInList; ++e)
		m_pEffectList[e] = NULL;
	e = 0;

	// Find all "*.ini" files in the ENV_VAR_INI_PATH directory
	fileHandle = FindFirstFile(pathName, &findData);
	if(fileHandle != INVALID_HANDLE_VALUE)
	{
		// Memory for GetPrivateProfileSection() and GetPrivateProfileString
		lpResult = new CHAR [SECTION_MAX_SIZE];

		do
		{
			// Full filename
			pathName.Format(_T("%s\\%s"), lpDir, findData.cFileName);

			// First pass to see if this is a valid file is to check 
			// for the fstate section
			if(GetPrivateProfileSection(FXSTATE_SECTION_NAME, lpResult, SECTION_MAX_SIZE, 
										pathName) == 0)
				continue;

			// Name
			GetPrivateProfileString(FXSTATE_SECTION_NAME, FX_NAME,
								    _T("Effect"), lpResult, SECTION_MAX_SIZE, 
								    pathName);
			name = lpResult;

			// DSP Code
			GetPrivateProfileString(FXSTATE_SECTION_NAME, FXPARAM_DSPCODE_NAME,
								    _T("dspFilename"), lpResult, SECTION_MAX_SIZE, 
								    pathName);
			dspCode = lpResult;

			// DSP Cycles
			dspCycles = GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
											 FXPARAM_DSPCYCLES_NAME,
											 0, pathName);

			// Y mem size
			yMemSize = GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
											FXPARAM_YMEMORY_SIZE,
											0, pathName);
			
			// Num Inputs
			numInputs = GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
											 FXPARAM_NUMINPUTS_NAME,
											 0, pathName);
			inputs.SetSize(numInputs);

			// Num Outputs
			numOutputs = GetPrivateProfileInt(FXSTATE_SECTION_NAME, 
											  FXPARAM_NUMOUTPUTS_NAME,
											  0, pathName);
			outputs.SetSize(numOutputs);

			// Inputs
			for(i=0; i<numInputs; ++i)
			{
				string.Format(_T("FX_INPUT%d_NAME"), i);
				GetPrivateProfileString(FXSTATE_SECTION_NAME, string,
										_T("In"), lpResult, SECTION_MAX_SIZE, 
										pathName);
				inputs[i] = lpResult;
			}

			// Outputs
			for(i=0; i<numOutputs; ++i)
			{
				string.Format(_T("FX_OUTPUT%d_NAME"), i);
				GetPrivateProfileString(FXSTATE_SECTION_NAME, string,
										_T("Out"), lpResult, SECTION_MAX_SIZE, 
										pathName);
				outputs[i] = lpResult;
			}

			// Create the effect
			m_pEffectList[e++] = new CModule(this, -(e+1), CModule::EFFECT, 
											 name, dspCycles, yMemSize, dspCode, 
											 findData.cFileName, &inputs, &outputs);
		}
		while((e < m_numEffectsInList) && FindNextFile(fileHandle, &findData));
		FindClose(fileHandle);

		// Free memory
		delete [] lpResult;

		// Adjust actual number of effects found
		m_numEffectsInList = e;
	}
	delete [] lpDir;
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::ReadKeyword
//------------------------------------------------------------------------------
BOOL 
CGrid::ReadKeyword(
				   IN OUT CStdioFile&	file,
				   OUT CString&			keyword
				   )
/*++

Routine Description:

	Reads a keyword from an effects file

Arguments:

	IN OUT file -	File
	OUT keyword -	Keyword

Return Value:

	TRUE on success, FALSE on error

--*/
{
	CString	string;
	DWORD	pos;

	while(1)
	{
		pos = file.GetPosition();
		if(file.ReadString(string))
		{
			if(string.Find(':') >= 0)
			{
				if(string.Replace(':', '\0'))
				{
					keyword = (LPCTSTR)string;
					file.Seek(pos + keyword.GetLength() + 1, CFile::begin);
					return TRUE;
				}
			}
		}
		else
			return FALSE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::ReadInt
//------------------------------------------------------------------------------
BOOL 
CGrid::ReadInt(
			   IN OUT CStdioFile&	file,
			   OUT int&				num
			   )
/*++

Routine Description:

	Reads an integer from an effects file

Arguments:

	IN OUT file -	File
	OUT keyword -	Keyword

Return Value:

	TRUE on success, FALSE on error

--*/
{
	CString	string;

	if(ReadString(file, string))
	{
		num = (int)atol((LPCTSTR)string);
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::ReadString
//------------------------------------------------------------------------------
BOOL 
CGrid::ReadString(
				  IN OUT CStdioFile&	file,
				  OUT CString&			string
				  )
/*++

Routine Description:

	Reads a string from an effects file. The string can include spaces

Arguments:

	IN OUT file -	File
	OUT keyword -	Keyword

Return Value:

	TRUE on success, FALSE on error

--*/
{
	if(file.ReadString(string))
	{
		string.TrimLeft();
		string.TrimRight();
		return TRUE;
	}
	return FALSE;
}

//------------------------------------------------------------------------------
//	CGrid::InsertEffect
//------------------------------------------------------------------------------
void
CGrid::InsertEffect(
					IN const CPoint&	point,
					IN int				index
					)
/*++

Routine Description:

	Inserts an effect into the grid

Arguments:

	IN index -	Index of effect

Return Value:

	None

--*/
{
	int	x;
	int	y;

	if(!GetXYFromPoint(point, x, y))
		return;

	m_insertEffect	= TRUE;
	m_pTempModule	= new CModule(*m_pEffectList[index]);
	m_pMoveModule	= m_pEffectList[index];
	m_moveX			= x;
	m_moveY			= y;
	m_moveDx		= m_pMoveModule->GetWidth() / 2;
	m_moveDy		= 0;
	m_pTempModule->SetLocation(x-m_moveDx, y);
	m_pTempModule->SetState(IsPlacementValid(m_pMoveModule, x-m_moveDx, y) ? CModule::OK : CModule::INVALID);
}

//------------------------------------------------------------------------------
//	CGrid::Save
//------------------------------------------------------------------------------
BOOL 
CGrid::Save(
			IN LPCTSTR	pFilename
			)
/*++

Routine Description:

	Saves the grid 

Arguments:

	IN pFilename -	File to save to

Return Value:

	TRUE if the save succeeded, FALSE otherwise

--*/
{
	CFile	file;
	CString	string;
	BYTE	byte;
	WORD	word;
	BYTE*	pByte;
	int		i;
	CCell*	pCell;

	if(pFilename && strlen(pFilename))
		m_filename = pFilename;

	// Was a filename specified?
	else if(m_filename.IsEmpty())
	{
		CFileDialog	fileDialog(FALSE, _T("dsp"), NULL, OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST,
							   _T("DSP Builder Files (.dsp)|*.dsp|All Files (*.*)|*.*||"));
		if(fileDialog.DoModal() != IDOK)
			return FALSE;
		m_filename = fileDialog.GetPathName();
	}

	// Open the file
	if(!file.Open(m_filename, CFile::modeWrite | CFile::modeCreate | CFile::typeBinary))
	{
		string.Format(_T("Unable to open file: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	// Write the header
	file.Write("DSPBUILDER", 10);

	// Write the version
	byte = 1;
	file.Write(&byte, sizeof(byte));

	// Write the width and height
	word = m_width;
	file.Write(&word, sizeof(word));
	word = m_height;
	file.Write(&word, sizeof(word));

	// Write all the cells
	pByte = new BYTE [m_numCells];
	for(i=0; i<m_numCells; ++i)
	{
		pCell = m_pCells[i];
		if(pCell && !pCell->GetModule())
		{
			pByte[i] = (pCell->GetPatchLeft() ? (1 << 3) : 0) |
					   (pCell->GetPatchRight() ? (1 << 2) : 0) |
					   (pCell->GetPatchTop() ? (1 << 1) : 0) |
					   (pCell->GetPatchBottom() ? 1 : 0) |
					   (pCell->IsJoined() ? (1 << 4) : 0);
		}
		else
			pByte[i] = 0;
	}
	file.Write(pByte, m_numCells);
	delete [] pByte;

	// Write the number of effects
	word = m_numEffects;
	file.Write(&word, sizeof(word));

	// Write all the effects
	for(i=0; i<m_numEffects; ++i)
	{
		if(m_pEffects[i])
		{
			// Effect Name (word, string)
			word = strlen(m_pEffects[i]->GetName()) + 1;
			file.Write(&word, sizeof(word));
			file.Write(m_pEffects[i]->GetName(), word + 1);

			// Grid Location (word, word)
			word = m_pEffects[i]->GetLeft();
			file.Write(&word, sizeof(word));
			word = m_pEffects[i]->GetTop();
			file.Write(&word, sizeof(word));

			// Num Inputs (word)
			word = m_pEffects[i]->GetNumInputs();
			file.Write(&word, sizeof(word));

			// Num Outputs (word)
			word = m_pEffects[i]->GetNumOutputs();
			file.Write(&word, sizeof(word));

			// Width (word)
			word = m_pEffects[i]->GetWidth();
			file.Write(&word, sizeof(word));

			// Height (word)
			word = m_pEffects[i]->GetHeight();
			file.Write(&word, sizeof(word));
		}
	}

	m_bChangesMade	= FALSE;
	file.Close();
	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::New
//------------------------------------------------------------------------------
BOOL 
CGrid::New(
		   IN int	width,
		   IN int	height
		   )
/*++

Routine Description:

	Nukes the grid and restarts fresh

Arguments:

	IN width -	Width of new grid
	IN height -	Height of new grid

Return Value:

	Nothing

--*/
{
	int	i;
	int	y;

	// Prompt to save
	if(m_bChangesMade)
	{
		switch(AfxMessageBox("Save changes?", MB_YESNOCANCEL | MB_APPLMODAL | MB_ICONQUESTION))
		{
		case IDYES:
			if(!Save(m_filename))
				return FALSE;
			break;
		case IDCANCEL:
			return FALSE;
		}
	}

	// Wipe all the data
	if(m_pCells)
	{
		for(i=0; i<m_numCells; ++i)
			SetCell(i, NULL);
		delete [] m_pCells;
		m_pCells = NULL;
	}

	// Delete the Effects
	for(i=0; i<m_numEffects; ++i)
	{
		if(m_pEffects[i])
		{
			delete m_pEffects[i];
			m_pEffects[i] = NULL;
		}
	}
	m_numEffects = 0;

	if(width != -1)
		m_width = width;
	if(height != -1)
		m_height = height;

	// Create the cells
	m_numCells	= m_width * m_height;
	m_pCells	= new CCell* [m_numCells];
	for(i=0; i<m_numCells; ++i)
		m_pCells[i] = NULL;

	// Reset the mixbins
	ResetRect();
	for(y=0, i=0; i<CGridNumMixbins; ++i, y += m_mixbinHeight + 1)
	{
		PlaceModule(m_pVPMixbins[i], 0, y);
		AdjustRect(m_pVPMixbins[i]);
	}

	// Place  the GP Mixbins
	for(y=0, i=0; i<CGridNumMixbins; ++i, y += m_mixbinHeight + 1)
	{
		PlaceModule(m_pGPMixbins[i], m_width - m_mixbinWidth, y);
		AdjustRect(m_pGPMixbins[i]);
	}
	ValidateGrid();
	Draw(m_rect);

	return TRUE;
}

//------------------------------------------------------------------------------
//	CGrid::Open
//------------------------------------------------------------------------------
BOOL 
CGrid::Open(
			IN LPCTSTR	pFilename
			)
/*++

Routine Description:

	Opens a grid file

Arguments:

	IN pFilename -	File to open

Return Value:

	TRUE if the open succeeded, FALSE otherwise

--*/
{
	CFile	file;
	CString	string;
	BYTE	byte;
	WORD	word;
	BYTE*	pByte;
	int		e;
	int		i;
	int		x;
	int		y;
	char	header[11];
	CCell*	pCell;
	char*	buffer;
	CString	effectName;
	int		left;
	int		top;
	int		numInputs;
	int		numOutputs;
	int		width;
	int		height;
	int		effectIndex;
	BOOL	missingEffects = FALSE;

	if(pFilename && strlen(pFilename))
		m_filename = pFilename;

	// Was a filename specified?
	else if(m_filename.IsEmpty())
	{
		CFileDialog	fileDialog(TRUE, _T("dsp"), NULL, OFN_PATHMUSTEXIST,
							   _T("DSP Builder Files (.dsp)|*.dsp|All Files (*.*)|*.*||"));
		if(fileDialog.DoModal() != IDOK)
			return FALSE;
		m_filename = fileDialog.GetPathName();
	}

	// Open the file
	if(!file.Open(m_filename, CFile::modeRead | CFile::typeBinary))
	{
		string.Format(_T("Unable to open file: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	// Read the header
	if(file.Read(header, 10) != 10)
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}
	header[10]	= '\0';
	string		= header;
	if(string != "DSPBUILDER")
	{
		string.Format(_T("File %s is not a valid DSP file"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	// Read the version
	if(file.Read(&byte, sizeof(byte)) != sizeof(byte))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}
	if(byte != 1)
	{
		string.Format(_T("Unsupported DSP file version: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	// Read the width
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}
	width = word;

	// Read the height
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}
	height = word;

	// Allocate the cells
	if(width * height <= 0)
	{
		string.Format(_T("Invalid width / height: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		return FALSE;
	}

	// Create a new blank grid
	if(!New(width, height))
		return FALSE;

	pByte = new BYTE [m_numCells];
	
	// Read the cells
	if(file.Read(pByte, m_numCells) != (UINT)m_numCells)
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		delete [] pByte;
		return FALSE;
	}

	// Process the cells
	ResetRect();
	for(x=0; x<m_width; ++x)
	{
		for(y=0; y<m_height; ++y)
		{
			i = GetCellIndex(x, y);
			if(pByte[i] != 0)
			{
				pCell = GetCellAlloc(x, y);
				if(pByte[i] & (1 << 3))
					pCell->SetPatchLeft(TRUE);
				if(pByte[i] & (1 << 2))
					pCell->SetPatchRight(TRUE);
				if(pByte[i] & (1 << 1))
					pCell->SetPatchTop(TRUE);
				if(pByte[i] & 1)
					pCell->SetPatchBottom(TRUE);
				if(pByte[i] & (1 << 4))
					pCell->SetJoin(TRUE);
				AdjustRect(pCell);
			}
		}
	}
	delete [] pByte;

	// Read all the effects
	if(file.Read(&word, sizeof(word)) != sizeof(word))
	{
		string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
		AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
		New();
		return FALSE;
	}
	m_numEffects = word;
	for(effectIndex=0, i=0; i<m_numEffects; ++i)
	{
		// Effect Name (word, string)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		buffer = new char [word+1];
		if(file.Read(buffer, word + 1) != (UINT)(word+1))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			delete [] buffer;
			New();
			return FALSE;
		}
		effectName = buffer;
		delete [] buffer;

		// Grid Location (word, word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		left = word;
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		top = word;

		// Num Inputs (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		numInputs = word;

		// Num Outputs (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		numOutputs = word;

		// Width (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		width = word;

		// Height (word)
		if(file.Read(&word, sizeof(word)) != sizeof(word))
		{
			string.Format(_T("Error reading: %s"), (LPCTSTR)m_filename);
			AfxMessageBox(string, MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
			New();
			return FALSE;
		}
		height = word;

		// Try to find the effect in the effects list
		for(e=0; e < m_numEffectsInList; ++e)
		{
			// The following need to match: name, width,height, numInputs & numOutputs
			if((effectName == m_pEffectList[e]->GetName()) &&
			   (m_pEffectList[e]->GetWidth() == width) &&
			   (m_pEffectList[e]->GetHeight() == height) &&
			   (m_pEffectList[e]->GetNumInputs() == numInputs) &&
			   (m_pEffectList[e]->GetNumOutputs() == numOutputs))
			   break;
		}

		// If the effect was found add it
		if(e != m_numEffectsInList)
		{
			m_pEffects[effectIndex] = new CModule(*m_pEffectList[e]);
			PlaceModule(m_pEffects[effectIndex], left, top);
			AdjustRect(m_pEffects[effectIndex]);
			++effectIndex;
		}

		// Otherwise drop it and set a flag to display an error
		else
			missingEffects = TRUE;
	}

	// Adjust for errors
	if(missingEffects)
	{
		m_numEffects = effectIndex;
		AfxMessageBox(_T("One or more of the effects in the file are "
						 "not currently available and will not be shown."),
					  MB_OK | MB_ICONEXCLAMATION | MB_APPLMODAL);
	}

	ValidateGrid();
	Draw(m_rect);

	file.Close();
	return TRUE;
}
