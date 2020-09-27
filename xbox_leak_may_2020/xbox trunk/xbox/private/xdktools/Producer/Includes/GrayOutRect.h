#if !defined(AFX_GRAYOUTRECT_H__6442911A_B521_11D0_A980_00A0C922E6EB__INCLUDED_)
#define AFX_GRAYOUTRECT_H__6442911A_B521_11D0_A980_00A0C922E6EB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
				
// GrayOutRect.h

inline void GrayOutRect( HDC hDC, const RECT *pRect )
{
	/*
	if( ::GetDeviceCaps( hDC, RASTERCAPS ) & RC_BITBLT )
	{
		WORD awBitMap[8] = { 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA, 0x55, 0xAA };
		HBITMAP hBitmap = CreateBitmap( 8, 8, 1, 1, awBitMap ); 
		if( hBitmap )
		{
			HBRUSH hBrush = CreatePatternBrush( hBitmap );
			if( hBrush )
			{
				HBRUSH hOrigBrush = static_cast<HBRUSH>(::SelectObject( hDC, hBrush ));
				COLORREF crText = SetTextColor(hDC, RGB(255, 255, 255));
				COLORREF crBk = SetBkColor(hDC, RGB(0, 0, 0));
        		::PatBlt( hDC, pRect->left, pRect->top, pRect->right - pRect->left, pRect->bottom - pRect->top, 0x00A000C9 );
				SetTextColor( hDC, crText );
				SetBkColor( hDC, crBk );
    				SelectObject( hDC, hOrigBrush );
				::DeleteObject( hBrush );
			}
			::DeleteObject( hBitmap );
		}
	}
	else
	*/
	{
		::InvertRect( hDC, pRect );
	}
}

#endif // !defined(AFX_GRAYOUTRECT_H__6442911A_B521_11D0_A980_00A0C922E6EB__INCLUDED_)
