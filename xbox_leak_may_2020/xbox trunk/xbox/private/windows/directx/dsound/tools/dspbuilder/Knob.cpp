/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Knob.cpp

Abstract:

	Graphical knob control.

	The layout of angles is as follows

           180
            |
            |
            |
	90 -----+----- 270
            |
            |
            |
            0
Author:

	Robert Heitkamp (robheit) 30-Nov-2001

Revision History:

	30-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <math.h>
#include "resource.h"
#include "Knob.h"
#include "Module.h"

//------------------------------------------------------------------------------
//	Stuff
//------------------------------------------------------------------------------
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//------------------------------------------------------------------------------
//	Macros:
//------------------------------------------------------------------------------
#define MAX(a,b) (((a) >= (b)) ? (a) : (b))

//------------------------------------------------------------------------------
//	Constants
//------------------------------------------------------------------------------
const double cPi		= 3.14159265359;
const double cTwoPi		= cPi * 2.0;

//------------------------------------------------------------------------------
//	Message Map
//------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CKnob, CStatic)
	//{{AFX_MSG_MAP(CKnob)
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SYSCOLORCHANGE()
	ON_EN_CHANGE(1, OnEditChange)
	ON_EN_KILLFOCUS(1, OnEditLoseFocus)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//------------------------------------------------------------------------------
//	CKnob::CKnob
//------------------------------------------------------------------------------
CKnob::CKnob(void) : CStatic()
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	HFONT	font;

	m_name				= _T("Knob");
	m_mode				= CKnob::CONTINUOUS;
	m_dataType			= CKnob::TYPE_DOUBLE;
	m_increment			= 0.0;
	m_format			= _T("%.2lf");
	m_radius			= 0.0;
	m_bLeftMouseDown	= FALSE;
	m_pEdit				= NULL;
	m_backgroundColor	= RGB(255,0,0);
	m_minAngle			= 30.0 * cPi / 180.0;
	m_maxAngle			= 330.0 * cPi / 180.0;
	m_textColor			= GetSysColor(COLOR_BTNTEXT);
	m_minPos			= 0.0;
	m_maxPos			= 1.0;
	m_pos				= m_minPos;
	m_lastPos			= m_pos;
	m_default			= 0.0;
	m_angle				= PositionToAngle(m_pos);
	m_bAtomic			= TRUE;
	m_pParent			= NULL;
	m_index				= -1;
	m_bRealtime			= FALSE;

	font = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					  PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
					  "MS Sans Serif");
	m_font.Attach(font);
}

//------------------------------------------------------------------------------
//	CKnob::CKnob
//------------------------------------------------------------------------------
CKnob::CKnob(
			 IN CModule*	pParent,
			 IN int			index,
			 IN LPCTSTR		pName,
			 IN BOOL		bAtomic,
			 IN DataType	type,
			 IN double		min,
			 IN double		max,
			 IN	double		pos,
			 IN double		def,
			 IN LPCTSTR		pFormat,
			 IN LPCTSTR		pUnits,
			 IN CKnob::Mode	mode,
			 IN double		increment
			 ) : CStatic()
/*++

Routine Description:

	Constructor

Arguments:

	IN pParent -	Parent module
	IN index -		Index (ID)
	IN pName -		Name of knob
	IN bAtomic -	TRUE if the knob is atomic (can be adjusted in realtime)
	IN type -		Type of the data
	IN min -		Min value of the data
	IN max -		Max value of the data
	IN pos -		Current position (min - max)
	IN def -		Default position (min - max)
	IN pFormat -	Format for the data display
	IN pUnits -		Units of the data
	IN mode -		Mode of operation
	IN increment -	Used for DISCRETE mode

Return Value:

	None

--*/
{
	HFONT	font;

	m_pParent			= pParent;
	m_index				= index;
	m_name				= pName;
	m_mode				= mode;
	m_bAtomic			= bAtomic;
	m_dataType			= type;
	m_increment			= increment;
	m_format			= pFormat;
	m_units				= pUnits;
	m_radius			= 0.0;
	m_bLeftMouseDown	= FALSE;
	m_pEdit				= NULL;
	m_backgroundColor	= RGB(255,0,0);
	m_minAngle			= 30.0 * cPi / 180.0;
	m_maxAngle			= 330.0 * cPi / 180.0;
	m_textColor			= GetSysColor(COLOR_BTNTEXT);
	m_bRealtime			= FALSE;

	font = CreateFont(15, 0, 0, 0, 0, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
					  PROOF_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
					  "MS Sans Serif");
	m_font.Attach(font);

	// Min and max
	if(min <= max)
	{
		m_minPos	= min;
		m_maxPos	= max;
	}
	else
	{
		m_minPos	= max;
		m_maxPos	= min;
	}

	// Position
	if(pos < m_minPos)
		m_pos = m_minPos;
	else if(pos > m_maxPos)
		m_pos = m_maxPos;
	else
		m_pos = pos;

	m_lastPos	= m_pos;
	m_default	= def;

	// Angle
	m_angle = PositionToAngle(m_pos);

	// Adjust units - remove "None"
	if(!m_units.CompareNoCase(_T("none")))
		m_units.Empty();
}

//------------------------------------------------------------------------------
//	CKnob::~CKnob
//------------------------------------------------------------------------------
CKnob::~CKnob(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_knobBitmap.DeleteObject();
	m_bumpBitmap.DeleteObject();
	delete m_pEdit;
	m_font.DeleteObject();
}

//------------------------------------------------------------------------------
//	CKnob::Reset
//------------------------------------------------------------------------------
void 
CKnob::Reset(void)
/*++

Routine Description:

	Reset to last applied setting

Arguments:

	None

Return Value:

	None

--*/
{
	m_pos	= m_lastPos;
	m_angle	= PositionToAngle(m_pos);
	UpdateEdit();
	Invalidate(TRUE);
}

//------------------------------------------------------------------------------
//	CKnob::Default
//------------------------------------------------------------------------------
void 
CKnob::Default(void)
/*++

Routine Description:

	Reset to default setting

Arguments:

	None

Return Value:

	None

--*/
{
	m_pos	= m_default;
	m_angle = PositionToAngle(m_pos);
	UpdateEdit();
	Invalidate(TRUE);
}

//------------------------------------------------------------------------------
//	CKnob::PreSubclassWindow
//------------------------------------------------------------------------------
void 
CKnob::PreSubclassWindow(void) 
/*++

Routine Description:

	Set the window style

Arguments:

	None

Return Value:

	None

--*/
{
	CStatic::PreSubclassWindow();

	ModifyStyle(SS_BITMAP, SS_OWNERDRAW | SS_NOTIFY | WS_TABSTOP);
}

//------------------------------------------------------------------------------
//	CKnob::OnCreate
//------------------------------------------------------------------------------
int 
CKnob::OnCreate(LPCREATESTRUCT lpCreateStruct)
/*++

Routine Description:

	Create method

Arguments:

	IN OUT lpCreateStruct -	Creation struct

Return Value:

	None

--*/
{
	// Base class callback
	if (CWnd ::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create everything
	CreateBitmaps();
	return 0;
}

//------------------------------------------------------------------------------
//	CKnob::OnPaint
//------------------------------------------------------------------------------
void 
CKnob::OnPaint() 
/*++

Routine Description:

	Draws the knob

Arguments:

	None

Return Value:

	None

--*/
{
	CDC			memDC;
	CPaintDC	dc(this);
	CBitmap*	pOldBitmap;
    CString		string;
	CFont*		pOldFont;
	CPoint		point;

	dc.SetBkColor(::GetSysColor(COLOR_BTNFACE));
	dc.SetTextColor(m_textColor);

	// Draw the name
	pOldFont = dc.SelectObject(&m_font);
	dc.DrawText(m_name, m_nameRect, DT_NOPREFIX | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);

	// Draw the units
	dc.DrawText(m_units, m_unitsRect, DT_NOPREFIX | DT_NOCLIP | DT_SINGLELINE | DT_VCENTER | DT_CENTER);
	dc.SelectObject(pOldFont);

	// Create memory dc
	VERIFY(memDC.CreateCompatibleDC(&dc));

	// Draw the bitmap
	pOldBitmap = memDC.SelectObject(&m_knobBitmap);
	dc.BitBlt(m_knobRect.left, m_knobRect.top, m_knobRect.Width(), m_knobRect.Height(), &memDC, 0, 0, SRCCOPY);

	// Draw the bump
	memDC.SelectObject(&m_bumpBitmap);
	point = AngleToPoint(m_angle);
	dc.BitBlt(point.x - m_bumpCenter.x, point.y - m_bumpCenter.y, 
			  m_bumpRect.Width(), m_bumpRect.Height(), &memDC, 0, 0, SRCCOPY);

	// Restore
	if(pOldBitmap != NULL)
		memDC.SelectObject(memDC);
}

//------------------------------------------------------------------------------
//	CKnob::OnLButtonDown
//------------------------------------------------------------------------------
void 
CKnob::OnLButtonDown(
					 IN UINT	nFlags, 
					 IN CPoint	point
					 ) 
/*++

Routine Description:

	Capture the mouse

Arguments:

	IN nFlags -	Flags
	IN point -	Mouse location

Return Value:

	None

--*/
{
	double	distance;

	// Find the distance to the center
	distance = sqrt(((double)(point.x - m_knobCenter.x) * (double)(point.x - m_knobCenter.x)) +
					((double)(point.y - m_knobCenter.y) * (double)(point.y - m_knobCenter.y)));

	// Is the mouse within the knob?
	if((distance > (m_radius * 0.01f)) && (distance < (m_radius * 1.1f)))
	{
		m_bLeftMouseDown = TRUE;
		if(GetSafeHwnd() != ::GetCapture())
			SetCapture();

		// Adjust the angle
		UpdatePoint(point);
	}
	SetFocus();
	CStatic::OnLButtonDown(nFlags, point);
}

//------------------------------------------------------------------------------
//	CKnob::OnLButtonUp
//------------------------------------------------------------------------------
void 
CKnob::OnLButtonUp(
				   IN UINT		nFlags, 
				   IN CPoint	point
				   )
/*++

Routine Description:

	Release the mouse

Arguments:

	IN nFlags -	Flags
	IN point -	Mouse location

Return Value:

	None

--*/
{
	m_bLeftMouseDown = FALSE;
	ReleaseCapture();
	CStatic::OnLButtonUp(nFlags, point);
}

//------------------------------------------------------------------------------
//	CKnob::OnMouseMove
//------------------------------------------------------------------------------
void 
CKnob::OnMouseMove(
				   IN UINT		nFlags, 
				   IN CPoint	point
				   )
/*++

Routine Description:

	Adjust the knob

Arguments:

	IN nFlags -	Flags
	IN point -	Mouse location

Return Value:

	None

--*/
{
	if((nFlags & MK_LBUTTON) && (m_bLeftMouseDown))
		UpdatePoint(point);

	CStatic::OnMouseMove(nFlags, point);
}

//------------------------------------------------------------------------------
//	CKnob::OnSysColorChange
//------------------------------------------------------------------------------
void 
CKnob::OnSysColorChange() 
/*++

Routine Description:

	Handles system color change requests

Arguments:

	None

Return Value:

	None

--*/
{
	CStatic::OnSysColorChange();
	m_knobBitmap.DeleteObject();
	m_bumpBitmap.DeleteObject();
	CreateBitmaps();
	Invalidate(TRUE);
}

//------------------------------------------------------------------------------
//	CKnob::OnEditChange
//------------------------------------------------------------------------------
void
CKnob::OnEditChange(void)
/*++

Routine Description:

	Handles changes to the edit

Arguments:

	None

Return Value:

	None

--*/
{
	CString	string;

	if(m_pEdit)
	{
		m_pEdit->GetWindowText(string);
		m_pos	= atof(string);
		if(m_pos < m_minPos)
			m_pos = m_minPos;
		else if(m_pos > m_maxPos)
			m_pos = m_maxPos;
		m_angle	= PositionToAngle(m_pos);
		Invalidate(TRUE);
	}
}

//------------------------------------------------------------------------------
//	CKnob::OnEditLoseFocus
//------------------------------------------------------------------------------
void
CKnob::OnEditLoseFocus(void)
/*++

Routine Description:

	Updates the display when the edit loses focus

Arguments:

	None

Return Value:

	None

--*/
{
	UpdateEdit();
}

//------------------------------------------------------------------------------
//	CKnob::CreateBitmaps
//------------------------------------------------------------------------------
void 
CKnob::CreateBitmaps(void)
/*++

Routine Description:

	description-of-function

Arguments:

	None

Return Value:

	None

--*/
{
	BITMAP		bitmap;
	CString		string;
	CSize		nameSize;
	CSize		unitsSize;
	CDC			memDC;
	CBitmap*	pOldBitmap;
	int			x;
	int			y;
	COLORREF	color;
	int			width;
	CRect		rect;
	CClientDC	dc(this);
	CFont*		pOldFont;

	// Get the position
	GetWindowRect(&m_rect);
	ScreenToClient(&m_rect);

	// Load the bitmaps
	VERIFY(m_knobBitmap.LoadBitmap(IDB_KNOB) != FALSE);
	VERIFY(m_bumpBitmap.LoadBitmap(IDB_KNOB_BUMP) != FALSE);

	pOldFont = dc.SelectObject(&m_font);

	// Read the sizes
	m_knobBitmap.GetBitmap(&bitmap);
	nameSize	= dc.GetTextExtent(m_name);
	unitsSize	= dc.GetTextExtent(m_units);

	dc.SelectObject(pOldFont);
	
	width = MAX(bitmap.bmWidth, nameSize.cx);
	width = MAX(width, unitsSize.cx);

	// name rect
	m_nameRect.left		= m_rect.left;
	m_nameRect.top		= m_rect.top;
	m_nameRect.right	= m_nameRect.left + width;
	m_nameRect.bottom	= m_nameRect.top + nameSize.cy;

	// Knob rect
	m_knobRect.left		= m_rect.left + ((width - bitmap.bmWidth) / 2);
	m_knobRect.top		= m_nameRect.bottom;
	m_knobRect.right	= m_knobRect.left + width;
	m_knobRect.bottom	= m_knobRect.top + bitmap.bmHeight;

	// edit rect
	rect.left			= m_rect.left;
	rect.top			= m_knobRect.bottom + 5;
	rect.right			= rect.left + width;
	rect.bottom			= rect.top + MAX(nameSize.cy, unitsSize.cy);

	// Units rect
	m_unitsRect.left	= m_rect.left;
	m_unitsRect.top		= rect.bottom;
	m_unitsRect.right	= m_unitsRect.left + width;
	m_unitsRect.bottom	= m_unitsRect.top + unitsSize.cy;

	// Now, update the entire rect
	m_rect.right		= m_rect.left + width;
	m_rect.bottom		= m_unitsRect.bottom;

	// Some other calculations
	m_knobCenter.x		= m_rect.Width() / 2;
	m_knobCenter.y		= m_knobRect.top + (m_knobRect.Height() / 2);

	// Radius of knob
	m_radius = (double)bitmap.bmWidth / 2.0;

	// Create the memory dc to draw the bitmap
	VERIFY(memDC.CreateCompatibleDC(&dc));

	// Select the bitmap
	pOldBitmap = memDC.SelectObject(&m_knobBitmap);

	// Replace all pixels in the bitmap background with the screen 
	// background color
	for(x=0; x<bitmap.bmWidth; ++x)
	{
		for(y=0; y<bitmap.bmHeight; ++y)
		{
			color = memDC.GetPixel(x, y);
			if(color == m_backgroundColor)
				memDC.SetPixel(x, y, GetSysColor(COLOR_BTNFACE));
		}
	}

	// Adjust the knob's position
	SetWindowPos(NULL, 0, 0, m_rect.right, m_rect.bottom, 
				 SWP_NOMOVE | SWP_NOZORDER);

	// Set the rect size to the size of the bitmap
	m_bumpBitmap.GetBitmap(&bitmap);
	m_bumpRect.left		= 0;
	m_bumpRect.right	= bitmap.bmWidth;
	m_bumpRect.top		= 0;
	m_bumpRect.bottom	= bitmap.bmHeight;
	m_bumpCenter.x		= m_bumpRect.Width() / 2;
	m_bumpCenter.y		= m_bumpRect.Height() / 2;

	// Select the bitmap
	pOldBitmap = memDC.SelectObject(&m_bumpBitmap);

	// Restore the old bitmap
	memDC.SelectObject(pOldBitmap);

	// Create the edit
	if(!m_pEdit)
	{
		m_pEdit = new CEdit;
		m_pEdit->Create(WS_CHILD | WS_VISIBLE, rect, this, 1);
		m_pEdit->SetFont(&m_font);
		UpdateEdit();
	}
}

//------------------------------------------------------------------------------
//	CKnob::PointToAngle
//------------------------------------------------------------------------------
double
CKnob::PointToAngle(
					IN const CPoint& point
					) const
/*++

Routine Description:

	Converts a mouse point in the bitmap to an angle

Arguments:

	IN point -	Point

Return Value:

	angle

--*/
{
	double	x;
	double	y;
	double	angle;

	// Convert to angle system (described at top of file)
	x	= (double)(point.y - m_knobCenter.y);
	y	= (double)(m_knobCenter.x - point.x);

	if(x == 0.0) 
	{
		// Center: undefined so use angle of 0
		if(y == 0.0)
			angle = 0.0;

		// Otherwise a vertical line (90 or 270)
		else if(y > 0.0)
			angle = cPi / 2.0;
		else
			angle = 3.0 * cPi / 2.0;
	}

	// Horizontal line?
	else if(y == 0.0)
	{
		if(x > 0.0)
			angle = 0.0f;
		else
			angle = cPi;
	}

	// Otherwise, use atan
	else
	{
		angle = atan(y / x);   
		if(x < 0.0)
		{
			if(y > 0.0)
				angle += cPi;
			else
				angle -= cPi;
		}
		while(angle < 0.0)
			angle += cTwoPi;
	}
	
	return angle;
}

//------------------------------------------------------------------------------
//	CKnob::AngleToPoint
//------------------------------------------------------------------------------
CPoint
CKnob::AngleToPoint(
					IN double angle
					) const
/*++

Routine Description:

	Converts an angle to a point on the bitmap suitable to place the bump

Arguments:

	IN angle -	Angle

Return Value:

	Point on bitmap

--*/
{
	int	x = m_knobCenter.x - (int)(m_radius * sin(angle) * 0.7);
	int	y = m_knobCenter.y + (int)(m_radius * cos(angle) * 0.7);
	return CPoint((int)x, (int)y);
}

//------------------------------------------------------------------------------
//	CKnob::UpdateEdit
//------------------------------------------------------------------------------
void 
CKnob::UpdateEdit(void)
/*++

Routine Description:

	Updates the CEdit

Arguments:

	None

Return Value:

	None

--*/
{
	CString	string;

	// Update the edit
	switch(m_dataType)
	{
	case CKnob::TYPE_FLOAT:
		string.Format(m_format, (float)m_pos);
		break;
	case CKnob::TYPE_DOUBLE:
		string.Format(m_format, m_pos);
		break;
	case CKnob::TYPE_CHAR:
	case CKnob::TYPE_INT:
		string.Format(m_format, (int)m_pos);
		break;
	case CKnob::TYPE_UNSIGNED_CHAR:
	case CKnob::TYPE_UNSIGNED_INT:
		string.Format(m_format, (unsigned)m_pos);
		break;
	case CKnob::TYPE_LONG:
		string.Format(m_format, (long)m_pos);
		break;
	case CKnob::TYPE_UNSIGNED_LONG:
		string.Format(m_format, (unsigned long)m_pos);
		break;
	}
	if(m_pEdit)
		m_pEdit->SetWindowText(string);
}

//------------------------------------------------------------------------------
//	CKnob::Update
//------------------------------------------------------------------------------
void
CKnob::UpdatePoint(
				   IN const CPoint&	point
				   )
/*++

Routine Description:

	Updates the knob based on the point

Arguments:

	IN point -	Mouse point

Return Value:

	None

--*/
{
	//Convert xy position to an angle.
	m_angle = PointToAngle(point);
	if(m_angle < m_minAngle)
		m_angle = m_minAngle;
	else if(m_angle > m_maxAngle)
		m_angle = m_maxAngle;

	m_pos = AngleToPosition(m_angle);

	// Update the edit
	UpdateEdit();

	// Send the position
	if(m_pParent && m_bRealtime)
		m_pParent->ChangeParameter(this, TRUE);

	Invalidate();
}

