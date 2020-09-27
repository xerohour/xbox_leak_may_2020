//=----------------------------------------------------------------------=
// DbgUtil.H
//=----------------------------------------------------------------------=
// Common debug utilities that we should all be linking with.
//
// @author BrianPe, 4/98
//
#ifndef _INC_DBGUTIL_H
#define _INC_DBGUTIL_H

#if DEBUG
#define InvalidateRect DebugInvalidateRect
BOOL DebugInvalidateRect(HWND hWnd, CONST RECT *pRect, BOOL fErase);
#endif

#endif // _INC_DBGUTIL_H
