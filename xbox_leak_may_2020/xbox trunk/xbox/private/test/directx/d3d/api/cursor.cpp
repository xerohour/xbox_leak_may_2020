#include "d3dapi.hpp"

LINKME(2) 

//doesn't matter if this goes before or after other d3d device tests...

/*
    HRESULT WINAPI SetCursorProperties(UINT XHotSpot, UINT YHotSpot, IDirect3DSurface8 *pCursorBitmap);
    void    WINAPI SetCursorPosition(UINT XScreenSpace, UINT YScreenSpace, DWORD Flags);
    BOOL    WINAPI ShowCursor(BOOL bShow);
*/

#pragma data_seg(".d3dapi$test061") 

BEGINTEST(testDeviceCursor)
{

}
ENDTEST()

#pragma data_seg()
