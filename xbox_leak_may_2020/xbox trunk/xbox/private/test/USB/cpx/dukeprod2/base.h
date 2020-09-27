/*++

Copyright (c) 2001  Microsoft Corporation

Module Name:

    base.h

Abstract:

   general base functions declarations, type definitions, includes, etc.
Author:

    Dennis Krueger <a-denkru>   May 2000

Revision History:

--*/
#ifndef __base_h_
#define __base_h_

#include <xtl.h>



#define BlueColor	0xff0000ff
#define GrayColor	0xff707070
#define RedColor	0xffff0000
#define GreenColor	0xff00ff00
#define YellowColor 0xffffff00



class CPoint
{
public:
	CPoint() {;};
	~CPoint() {;};

	int GetX() { return m_X; };
	int GetY() { return m_Y; };
	void SetX(int X) { m_X = X; };
	void SetY(int Y) { m_Y = Y; };
	void SetXY(int X, int Y) { m_X = X; m_Y = Y;};
private:	
	int m_X;
	int m_Y;
};

class CRect
{
public:
	CRect() { ; };
	CRect(CPoint TopLeft, CPoint BottomRight) { m_TopLeft = TopLeft; m_BottomRight = BottomRight;};

	~CRect() {;};
	void Set(CPoint TopLeft, CPoint BottomRight) { m_TopLeft = TopLeft; m_BottomRight = BottomRight; };
	void SetTopLeft(CPoint TopLeft) { m_TopLeft = TopLeft; };
	void SetBottomRight(CPoint BottomRight) { m_BottomRight = BottomRight; };
	CPoint GetTopLeft() { return m_TopLeft; };
	CPoint GetBottomRight() { return m_BottomRight; };
	void OffsetRect(CPoint Point);
	void Draw(DWORD dwColor);
private:
	CPoint m_TopLeft;
	CPoint m_BottomRight;
};


#endif // __base_h_