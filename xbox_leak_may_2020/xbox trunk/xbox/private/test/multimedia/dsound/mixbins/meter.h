/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	meter.h

Abstract:

	Output meter

Author:

	Robert Heitkamp (robheit) 09-Oct-2001

Environment:

	Xbox only

Revision History:

	09-Oct-2001 robheit
		Initial Version

--*/

#ifndef __METER_H__
#define __METER_H__

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <xtl.h>
#include "bitfont.h"

//------------------------------------------------------------------------------
//	CMixbinMeter
//------------------------------------------------------------------------------
class CMixbinMeter
{
public:

	struct Vertex
	{
		FLOAT		x, y, z, rhw;	// The transformed position for the vertex
		D3DCOLOR	color;			// Color of vertex
	};

public:

	CMixbinMeter(void);
	virtual ~CMixbinMeter(void);

	void Draw(IDirect3DDevice8*, IDirect3DSurface8*);

	void SetLevel(float);
	void SetCurrentLevel(float);
	void SetLabel(WCHAR*);
	void SetWidth(float);
	void SetHeight(float);
	void SetLocation(float, float);
	void SetActive(BOOL);

	void SetOutlineColor(D3DCOLOR);
	void SetBarColor(D3DCOLOR);	
	void SetActiveColor(D3DCOLOR);
	void SetLevelColor(D3DCOLOR);
	void SetErrorColor(D3DCOLOR);

	void SetLabelColor(D3DCOLOR color) { m_labelColor = color; };
	void ResetError(void) { m_bError = FALSE; };
	BOOL GetError(void) const { return m_bError; };
	float GetCurrentLevel(void) const { return m_currentLevel; };
	BOOL IsActive(void) const { return m_bActive; };
	float GetLevel(void) const { return m_level; };

private:

	void BuildMeter(void);

private:

	static BitFont	m_font;
	BOOL			m_bActive;
	BOOL			m_bError;
	float			m_level;
	float			m_currentLevel;
	WCHAR*			m_pLabel;
	float			m_width;
	float			m_height;
	float			m_x;
	float			m_y;
	float			m_top;
	float			m_bottom;
	float			m_left;
	float			m_right;
	D3DCOLOR		m_outlineColor;
	D3DCOLOR		m_barColor;
	D3DCOLOR		m_activeColor;
	D3DCOLOR		m_levelColor;
	D3DCOLOR		m_errorColor;
	D3DCOLOR		m_labelColor;
	Vertex			m_lineVerts[44];
	Vertex			m_barVerts[4];
	Vertex			m_activeVerts[3];
	Vertex			m_errorVerts[3];
	Vertex			m_levelVerts[2];
	int				m_labelWidth;
	Vertex			m_points[8];
};

#endif
