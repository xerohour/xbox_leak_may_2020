/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    base.cpp

Abstract:

   implementation of general base functions classes and helper functions
Author:

    Dennis Krueger <a-denkru>   May 2000

Revision History:

--*/
#include "base.h"
#include <draw.h>

void
CRect::Draw(DWORD dwColor)
{
	drBox((float) m_TopLeft.GetX(),
		   (float) m_TopLeft.GetY(),
		   (float) m_BottomRight.GetX(),
		   (float) m_BottomRight.GetY(),
		   dwColor
		   );
};

void CRect::OffsetRect(CPoint Point)
{
	m_TopLeft.SetX(m_TopLeft.GetX() + Point.GetX());
	m_TopLeft.SetY(m_TopLeft.GetY() + Point.GetY());
	m_BottomRight.SetX(m_BottomRight.GetX() + Point.GetX());
	m_BottomRight.SetY(m_BottomRight.GetY() + Point.GetY());
}


			