/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	meter.cpp

Abstract:

	Mixbin Meter 

Author:

	Robert Heitkamp (robheit) 09-Oct-2001

Environment:

	Xbox only

Revision History:

	09-Oct-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <wchar.h>
#include "meter.h"
#include "bitfont.h"

//------------------------------------------------------------------------------
//	Static Member Variables:
//------------------------------------------------------------------------------
BitFont CMixbinMeter::m_font;

//------------------------------------------------------------------------------
//	CMixbinMeter::CMixbinMeter
//------------------------------------------------------------------------------
CMixbinMeter::CMixbinMeter(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_bActive		= FALSE;
	m_bError		= FALSE;
	m_level			= 0.0f;
	m_currentLevel	= 0.0f;
	m_pLabel		= NULL;
	m_width			= 10.0f;
	m_height		= 300.0f;
	m_x				= 100.0f;
	m_y				= 100.0f;
	m_labelWidth	= 0;
	m_outlineColor	= 0xffffffff;
	m_barColor		= 0xff007700;
	m_activeColor	= 0xff00ff00;
	m_levelColor	= 0xffffffff;
	m_errorColor	= 0xffff0000;
	m_labelColor	= 0xffffffff;
	m_peakColor		= 0xffff0000;
	m_peak			= 0.0f;

	BuildMeter();
}

//------------------------------------------------------------------------------
//	CMixbinMeter::~CMixbinMeter
//------------------------------------------------------------------------------
CMixbinMeter::~CMixbinMeter(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	if(m_pLabel)
		free(m_pLabel);
}

//------------------------------------------------------------------------------
//	CMixbinMeter::Draw
//------------------------------------------------------------------------------
void
CMixbinMeter::Draw(
				   IN IDirect3DDevice8*		pDevice,
				   IN IDirect3DSurface8*	pText
				   )
/*++

Routine Description:

	Draws the meter

Arguments:

	IN pDevice -	D3D Device
	IN pText -		Surface to draw text to

Return Value:

	None

--*/
{
	// Vertex shader
	pDevice->SetVertexShader(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

	// Draw the bar
	if(m_currentLevel != 0.0)
	{
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, m_barVerts, 
								 sizeof(CMixbinMeter::Vertex));
	}

	// Draw the peak
	if(m_peak != 0.0f)
	{
		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, m_peakVerts, 
								 sizeof(CMixbinMeter::Vertex));
	}


	// Active
	if(m_bActive)
	{
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, m_activeVerts, 
								 sizeof(CMixbinMeter::Vertex));
	}

	//  or Error?
	else if(m_bError)
	{
		pDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 1, m_errorVerts, 
								 sizeof(CMixbinMeter::Vertex));
	}

	// Draw the outline
	pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 22, m_lineVerts, 
							 sizeof(CMixbinMeter::Vertex));

	// Draw the level
	if(m_level != 0.0f)
	{
		pDevice->DrawPrimitiveUP(D3DPT_LINELIST, 1, m_levelVerts, 
								 sizeof(CMixbinMeter::Vertex));
		pDevice->DrawPrimitiveUP(D3DPT_POINTLIST, 8, m_points, 
								 sizeof(CMixbinMeter::Vertex));
	}

	// Draw the label
	if(m_pLabel)
	{
		m_font.DrawText(pText, m_pLabel, 
						((int)(m_right + m_left) - m_labelWidth) / 2, 
						(int)m_bottom+1, 0, m_labelColor, 0);
	}
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetLevel
//------------------------------------------------------------------------------	
void 
CMixbinMeter::SetLevel(
					   IN float	level
					   )
/*++

Routine Description:

	Sets the level of the meter.

Arguments:

	IN level -	Level of meter

Return Value:

	None

--*/
{
	if(level < 0.0f)
		m_level = 0.0f;
	else if(level > 1.0f)
		m_level = 1.0f;
	else
		m_level = level;

	// Adjust the vertices
	m_levelVerts[0].y	= (m_bottom - m_top) * (1.0f - m_level) + m_top;
	m_levelVerts[1].y	= m_levelVerts[0].y;

	// Adjust the points
	m_points[0].y	= m_levelVerts[0].y;
	m_points[1].y	= m_points[0].y - 1;
	m_points[2].y	= m_points[0].y;
	m_points[3].y	= m_points[0].y + 1;
	m_points[4].y	= m_points[0].y;
	m_points[5].y	= m_points[0].y - 1;
	m_points[6].y	= m_points[0].y;
	m_points[7].y	= m_points[0].y + 1;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetCurrentLevel
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetCurrentLevel(
							  IN float	level
							  )
/*++

Routine Description:

	Sets the current level of the meter

Arguments:

	None

Return Value:

	None

--*/
{
	if(level < 0.0f)
		m_currentLevel = 0.0f;
	else if(level > 1.0f)
		m_currentLevel = 1.0f;
	else
		m_currentLevel = level;

	if(!m_bActive && (m_currentLevel != 0.0f))
		m_bError = TRUE;

	// Adjust the vertices
	m_barVerts[1].y	= ((m_bottom - m_top) * (1.0f - m_currentLevel)) + m_top;
	m_barVerts[2].y	= m_barVerts[1].y;

	// Adjust the peak
	if(m_currentLevel > m_peak)
	{
		m_peak				= m_currentLevel;
		m_peakVerts[0].y	= ((m_bottom - m_top) * (1.0f - m_peak)) + m_top;
		m_peakVerts[1].y	= m_peakVerts[0].y;
	}
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetLabel
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetLabel(
					   IN WCHAR*	pLabel
					   )
/*++

Routine Description:

	Sets the label of the meter. The label should be very short.

Arguments:

	IN pLabel -	Label

Return Value:

	None

--*/
{
	DWORD	width;
	DWORD	height;

	if(m_pLabel)
	{
		free(m_pLabel);
		m_pLabel = NULL;
	}

	if(pLabel)
	{
		int length = wcslen(pLabel);
//		m_pLabel = _wcsdup(pLabel);
		m_pLabel = (WCHAR*)malloc((length+1) * sizeof(WCHAR));
		wcscpy(m_pLabel, pLabel);
		m_font.GetTextStringLength(&width, &height, m_pLabel);
		m_labelWidth = width;
	}
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetWidth
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetWidth(
					   IN float	width
					   )
/*++

Routine Description:

	Sets the width, in pixels, of the meter

Arguments:

	IN width -	Width

Return Value:

	None

--*/
{
	m_width = width;
	BuildMeter();
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetHeight
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetHeight(
						IN float	height
						)
/*++

Routine Description:

	Sets the height, in pixels, of the meter

Arguments:

	IN height -	Height

Return Value:

	None

--*/
{
	m_height = height;
	BuildMeter();
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetLocation
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetLocation(
						  IN float	x, 
						  IN float	y
						  )
/*++

Routine Description:

	Sets the location of the meter, in pixels. This is the upper left corner.

Arguments:

	IN x -	X location
	IN y -	Y location

Return Value:

	None

--*/
{
	m_x = x;
	m_y = y;
	BuildMeter();
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetActive
//------------------------------------------------------------------------------
void 
CMixbinMeter::SetActive(
						IN BOOL	bFlag
						)
/*++

Routine Description:

	Sets the active state of the mixbin meter. If set active the error flag will
	be cleared.

Arguments:

	IN bFlag -	Flag

Return Value:

	None

--*/
{
	m_bActive = bFlag;
	if(m_bActive)
		m_bError = FALSE;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetOutlineColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetOutlineColor(
								   IN D3DCOLOR	color
								   )
/*++

Routine Description:

	Sets the color of the outline

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_outlineColor = color;

	for(int i=0; i<44; ++i)
		m_lineVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetBarColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetBarColor(
							   IN D3DCOLOR	color
							   )
/*++

Routine Description:

	Sets the color of the meter

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_barColor = color;

	for(int i=0; i<4; ++i)
		m_barVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetActiveColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetActiveColor(
								  IN D3DCOLOR	color
								  )
/*++

Routine Description:

	Sets the color of the active display

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_activeColor = color;

	for(int i=0; i<3; ++i)
		m_activeVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetLevelColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetLevelColor(
								 IN D3DCOLOR	color
								 )
/*++

Routine Description:

	Sets the color of the level meter

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_levelColor = color;

	for(int i=0; i<2; ++i)
		m_levelVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetErrorColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetErrorColor(
								 IN D3DCOLOR	color
								 )
/*++

Routine Description:

	Sets the color of the error display

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_errorColor = color;

	// Already an error?
	for(int i=0; i<4; ++i)
		m_errorVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::SetPeakColor
//------------------------------------------------------------------------------
void CMixbinMeter::SetPeakColor(
							    IN D3DCOLOR	color
							    )
/*++

Routine Description:

	Sets the color of the peak display

Arguments:

	IN color -	Color: 0xAARRGGBB

Return Value:

	None

--*/
{
	m_peakColor = color;

	// Already an error?
	for(int i=0; i<2; ++i)
		m_peakVerts[i].color = color;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::ResetPeak
//------------------------------------------------------------------------------
void CMixbinMeter::ResetPeak(void)
/*++

Routine Description:

	Resets the peak

Arguments:

	None

Return Value:

	None

--*/
{
	m_peak				= 0.0f;
	m_peakVerts[0].y	= m_bottom;
	m_peakVerts[1].y	= m_bottom;
}

//------------------------------------------------------------------------------
//	CMixbinMeter::BuildMeter
//------------------------------------------------------------------------------
void
CMixbinMeter::BuildMeter(void)
/*++

Routine Description:

	Builds the meter with the current settings

Arguments:

	None

Return Value:

	None

--*/
{
	int		i;
	int		ii;
	float	y;
	float	dy;
	float	dx;

	// Calculate the rectangle of the meter
	//m_top		= m_y + (m_width * 1.5f);
	m_top		= m_y + (m_height / 10);
	m_bottom	= m_y + m_height - 10; //m_width;
	m_left		= m_x;
	m_right		= m_x + m_width;
	
	// Outline params
	dy	= (float)(m_bottom - m_top) / 10.0f;
	dx	= (m_width / 4.0f) < 2.0f ? 2.0f : (m_width / 4.0f);
	
	// Build the outline
	for(ii=0, y=(float)m_top+dy, i=0; i<9; ++i, y+=dy)
	{
		m_lineVerts[ii].x		= m_left;
		m_lineVerts[ii].y		= y;
		m_lineVerts[ii].z		= 0.0f;
		m_lineVerts[ii].rhw		= 1.0f;
		m_lineVerts[ii++].color	= m_outlineColor;
		m_lineVerts[ii].x		= m_left+dx;
		m_lineVerts[ii].y		= y;
		m_lineVerts[ii].z		= 0.0f;
		m_lineVerts[ii].rhw		= 1.0f;
		m_lineVerts[ii++].color	= m_outlineColor;
		m_lineVerts[ii].x		= m_right-dx;
		m_lineVerts[ii].y		= y;
		m_lineVerts[ii].z		= 0.0f;
		m_lineVerts[ii].rhw		= 1.0f;
		m_lineVerts[ii++].color	= m_outlineColor;
		m_lineVerts[ii].x		= m_right;
		m_lineVerts[ii].y		= y;
		m_lineVerts[ii].z		= 0.0f;
		m_lineVerts[ii].rhw		= 1.0f;
		m_lineVerts[ii++].color	= m_outlineColor;
	}

	m_lineVerts[ii].x		= m_left;
	m_lineVerts[ii].y		= m_bottom;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_right;
	m_lineVerts[ii].y		= m_bottom;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_right;
	m_lineVerts[ii].y		= m_bottom;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_right;
	m_lineVerts[ii].y		= m_top;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_right;
	m_lineVerts[ii].y		= m_top;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_left;
	m_lineVerts[ii].y		= m_top;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_left;
	m_lineVerts[ii].y		= m_top;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;
	m_lineVerts[ii].x		= m_left;
	m_lineVerts[ii].y		= m_bottom;
	m_lineVerts[ii].z		= 0.0f;
	m_lineVerts[ii].rhw		= 1.0f;
	m_lineVerts[ii++].color	= m_outlineColor;

	// Bar
	dy					= m_bottom - m_top;
	m_barVerts[0].x		= m_left;
	m_barVerts[0].y		= m_bottom;
	m_barVerts[0].z		= 0.0f;
	m_barVerts[0].rhw	= 1.0f;
	m_barVerts[0].color	= m_barColor;
	m_barVerts[1].x		= m_left;
	m_barVerts[1].y		= (dy * (1.0f - m_currentLevel)) + m_top;
	m_barVerts[1].z		= 0.0f;
	m_barVerts[1].rhw	= 1.0f;
	m_barVerts[1].color	= m_barColor;
	m_barVerts[2].x		= m_right;
	m_barVerts[2].y		= m_barVerts[1].y;
	m_barVerts[2].z		= 0.0f;
	m_barVerts[2].rhw	= 1.0f;
	m_barVerts[2].color	= m_barColor;
	m_barVerts[3].x		= m_right;
	m_barVerts[3].y		= m_bottom;
	m_barVerts[3].z		= 0.0f;
	m_barVerts[3].rhw	= 1.0f;
	m_barVerts[3].color	= m_barColor;

	// Level
	m_levelVerts[0].x		= m_left;
	m_levelVerts[0].y		= dy * (1.0f - m_level) + m_top;
	m_levelVerts[0].z		= 0.0f;
	m_levelVerts[0].rhw		= 1.0f;
	m_levelVerts[0].color	= m_levelColor;
	m_levelVerts[1].x		= m_right;
	m_levelVerts[1].y		= m_levelVerts[0].y;
	m_levelVerts[1].z		= 0.0f;
	m_levelVerts[1].rhw		= 1.0f;
	m_levelVerts[1].color	= m_levelColor;

	// Active 
	m_activeVerts[0].x		= m_left;
	m_activeVerts[0].y		= m_y;
	m_activeVerts[0].z		= 0.0f;
	m_activeVerts[0].rhw	= 1.0f;
	m_activeVerts[0].color	= m_activeColor;
	m_activeVerts[1].x		= m_right;
	m_activeVerts[1].y		= m_y;
	m_activeVerts[1].z		= 0.0f;
	m_activeVerts[1].rhw	= 1.0f;
	m_activeVerts[1].color	= m_activeColor;
	m_activeVerts[2].x		= (m_right + m_left) / 2.0f;
	m_activeVerts[2].y		= m_top - 2;//m_y + m_width;
	m_activeVerts[2].z		= 0.0f;
	m_activeVerts[2].rhw	= 1.0f;
	m_activeVerts[2].color	= m_activeColor;

	// Error 
	m_errorVerts[0].x		= m_left;
	m_errorVerts[0].y		= m_y;
	m_errorVerts[0].z		= 0.0f;
	m_errorVerts[0].rhw		= 1.0f;
	m_errorVerts[0].color	= m_errorColor;
	m_errorVerts[1].x		= m_right;
	m_errorVerts[1].y		= m_y;
	m_errorVerts[1].z		= 0.0f;
	m_errorVerts[1].rhw		= 1.0f;
	m_errorVerts[1].color	= m_errorColor;
	m_errorVerts[2].x		= (m_right + m_left) / 2.0f;
	m_errorVerts[2].y		= m_top - 2; //m_y + m_width;
	m_errorVerts[2].z		= 0.0f;
	m_errorVerts[2].rhw		= 1.0f;
	m_errorVerts[2].color	= m_errorColor;

	// Points for the level
	m_points[0].x		= m_right+1;
	m_points[0].y		= m_levelVerts[0].y;
	m_points[0].z		= 0.0f;
	m_points[0].rhw		= 1.0f;
	m_points[0].color	= 0xffffffff;
	m_points[1].x		= m_right+2;
	m_points[1].y		= m_points[0].y - 1;
	m_points[1].z		= 0.0f;
	m_points[1].rhw		= 1.0f;
	m_points[1].color	= 0xffffffff;
	m_points[2].x		= m_right+2;
	m_points[2].y		= m_points[0].y;
	m_points[2].z		= 0.0f;
	m_points[2].rhw		= 1.0f;
	m_points[2].color	= 0xffffffff;
	m_points[3].x		= m_right+2;
	m_points[3].y		= m_points[0].y + 1;
	m_points[3].z		= 0.0f;
	m_points[3].rhw		= 1.0f;
	m_points[3].color	= 0xffffffff;
	m_points[4].x		= m_left-1;
	m_points[4].y		= m_points[0].y;
	m_points[4].z		= 0.0f;
	m_points[4].rhw		= 1.0f;
	m_points[4].color	= 0xffffffff;
	m_points[5].x		= m_left-2;
	m_points[5].y		= m_points[0].y - 1;
	m_points[5].z		= 0.0f;
	m_points[5].rhw		= 1.0f;
	m_points[5].color	= 0xffffffff;
	m_points[6].x		= m_left-2;
	m_points[6].y		= m_points[0].y;
	m_points[6].z		= 0.0f;
	m_points[6].rhw		= 1.0f;
	m_points[6].color	= 0xffffffff;
	m_points[7].x		= m_left-2;
	m_points[7].y		= m_points[0].y + 1;
	m_points[7].z		= 0.0f;
	m_points[7].rhw		= 1.0f;
	m_points[7].color	= 0xffffffff;

	// Peak
	m_peakVerts[0].x		= m_left + 1.0f;
	m_peakVerts[0].y		= dy * (1.0f - m_level) + m_top;
	m_peakVerts[0].z		= 0.0f;
	m_peakVerts[0].rhw		= 1.0f;
	m_peakVerts[0].color	= m_peakColor;
	m_peakVerts[1].x		= m_right - 1.0f;
	m_peakVerts[1].y		= m_peakVerts[0].y;
	m_peakVerts[1].z		= 0.0f;
	m_peakVerts[1].rhw		= 1.0f;
	m_peakVerts[1].color	= m_peakColor;
}
