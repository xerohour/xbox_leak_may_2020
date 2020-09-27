/* _Getwctype -- return character classification flags for wide character */
#include <xlocinfo.h>
#include <wchar.h>
#include <awint.h>

_STD_BEGIN
_CRTIMP2 short __cdecl _Getwctype(wchar_t _Ch,
	const _Ctypevec *_Ctype)
	{	/* return character classification flags for _Ch */
#ifndef _XBOX
	short _Mask;
	return ((short)(__crtGetStringTypeW(CT_CTYPE1, &_Ch, 1,
		(LPWORD)&_Mask, _Ctype->_Page, _Ctype->_Hand) == 0
		? 0 : _Mask));
#else
    return (short)iswctype(_Ch, -1);
#endif // XBOX
	}

_CRTIMP2 const wchar_t * __cdecl _Getwctypes(
	const wchar_t *_First, const wchar_t *_Last,
		short *_Dest, const _Ctypevec *_Ctype)
	{	/* get mask sequence for elements in [_First, _Last) */
#ifndef _XBOX
	__crtGetStringTypeW(CT_CTYPE1, _First, (int)(_Last - _First),
		(LPWORD)_Dest, _Ctype->_Page, _Ctype->_Hand);
#else
    while(_First < _Last)
        *_Dest++ = (short)iswctype(*_First++, -1);
#endif
	return (_Last);		
	}
_STD_END

/*
* Copyright (c) 1992-2001 by P.J. Plauger.  ALL RIGHTS RESERVED.
 * Consult your license regarding permissions and restrictions.
 V3.10:0009 */
