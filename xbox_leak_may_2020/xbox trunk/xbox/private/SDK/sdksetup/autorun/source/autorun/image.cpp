#include "stdafx.h"
#include "bitmap.h"
#include "image.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CImage* CreateImage(
    DWORD   dwType, 
    LPCTSTR lpctszImageFile)
{
    CImage* pcImage = NULL;

    switch(dwType) {
    case IMAGE_TYPE_BITMAP:
        {
            CBitmapFileImage* pcBitmap = new CBitmapFileImage;
            if (!pcBitmap && !pcBitmap->InitData(lpctszImageFile)) {
                delete pcBitmap;
                pcBitmap = NULL;
            }
            pcImage = pcBitmap;
        }
        break;
    default:
        ASSERT(FALSE);
        break;
    }

    return pcImage;
}
