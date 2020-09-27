#ifndef _INCLUDE_NT4MULTIDEV
#define _INCLUDE_NT4MULTIDEV

#ifdef NT4_MULTI_DEV

#define MAX_MULTIDEV 16

#define PMDEV_UNIQ 'MDEV'
 
typedef struct _MUL_PIXELFORMAT
{
    PIXELFORMATDESCRIPTOR  pfd;
    LONG                   iPixelFormat[MAX_MULTIDEV] ;
}MUL_PIXELFORMAT,*PMUL_PIXELFORMAT;

typedef struct _MUL_PIXELFORMAT_LIST
{
    PMUL_PIXELFORMAT pPixelFormats;
    LONG             iNumberOfPixelFormats;
}MUL_PIXELFORMAT_LIST,*PMUL_PIXELFORMAT_LIST;

typedef struct _BOARDDESC
{
    BOOL            bIsActive;
    HANDLE          hDriver;
    RECTL           rclBoard;
    DEVMODEW        DevMode;
    union
    {
    DHPDEV          dhpdev;
    struct _PDEV   *ppdev;
    };
    union
    {
    DHSURF          dhsurf;
    struct _DSURF  *pdsurf;
    };
    CLIPOBJ        *pcoBoard;
    CLIPOBJ        *pco;
    RECTL           rclSavedBounds;
    ULONG           cjCaps;
    union
    {
    ULONG          *pdevcaps;
    GDIINFO        *pGdiInfo;
    };
    ULONG           cjDevInfo;
    DEVINFO        *pdi;
    HSURF           hsurf;
    SURFOBJ        *pso;
    HSURF           hsurfBitmap;
    SURFOBJ        *psoBitmap;
    // openGl stuff
    ULONG           ulRoot;
}
BOARDDESC, *PBOARDDESC;


typedef struct _MDEV
{
    DWORD                   dwUniq;
    HDEV                    hdev;
    HANDLE                  hDriver;
    RECTL                   rclDesktop;
    DEVMODEW                DevMode;
    HSURF                   hsurf;
    SURFOBJ                *pso;
    CLIPOBJ                *pco;
    FLONG                   flHooks;
    ULONG                   ulNumDevicesActive;
    ULONG                   ulNumDevicesInSystem;
    BOARDDESC               abdDevices[MAX_MULTIDEV];
    MUL_PIXELFORMAT_LIST    PixelFormatList;
}
MDEV, *PMDEV;


typedef struct _ENUMDEV 
{
  ULONG c;
  PBOARDDESC apbdDevices[1];
} ENUMDEV;

typedef struct _ENUMDEV16 
{
  ULONG c;
  PBOARDDESC apbdDevices[16];
} ENUMDEV16;

typedef struct _DEVOBJ
{
    PMDEV       pmdev;
    SURFOBJ     *pso;
    PBOARDDESC  apbdDevices[MAX_MULTIDEV];
    ULONG       enumStart;
    ULONG       cDevices;
}
DEVOBJ, *PDEVOBJ;



BOOL _inline bMatchingDevModes(DEVMODEW*   pdm1, DEVMODEW*   pdm2)
{
    BOOL bRet = FALSE;

    ASSERT(pdm1);
    ASSERT(pdm2);

    if (pdm1->dmBitsPerPel == pdm2->dmBitsPerPel
    &&  pdm1->dmPelsWidth == pdm2->dmPelsWidth
    &&  pdm1->dmPelsHeight == pdm2->dmPelsHeight
    &&  pdm1->dmDisplayFrequency == pdm2->dmDisplayFrequency
    &&  pdm1->dmPanningWidth == pdm2->dmPanningWidth
    &&  pdm1->dmPanningHeight == pdm2->dmPanningHeight
    )
        bRet = TRUE;
    return bRet;
}

BOOL _inline bFindMatchingDevMode(IN DEVMODEW*   pdmReference,
                                  IN DEVMODEW*   pdmArray,
                                  IN ULONG       ulSize)
{
    BOOL bRet = FALSE;
    DEVMODEW    *pdm;
    ASSERT(pdmReference);
    ASSERT(pdmArray);

#if 1
    if (pdmReference->dmPelsWidth < 1024 ||
        pdmReference->dmPelsHeight < 768 ||
        pdmReference->dmBitsPerPel != 32 ||
        pdmReference->dmDisplayFrequency != 75)
    return FALSE;
#endif  

    for (pdm = pdmArray; (BYTE *)pdm < ((BYTE *)pdmArray + ulSize); pdm++)
    {
        if (bMatchingDevModes(pdmReference, pdm))
        {
            bRet = TRUE;
            break;
        }

    }
    return bRet;
}


//*********************************************************
//
//  pmdevGetFromPso
//
//  gives back a pmdev if contained in pso
//  pso can be NULL or any type of SURFOBJ
//
//********************************************************
PMDEV _inline pmdevGetFromPso(SURFOBJ *pso)
{
    if(   (NULL != pso)
        &&(NULL != pso->dhpdev)
        &&(NULL != ((PPDEV)pso->dhpdev)->pmdev)
       )
    {
        ASSERT(((PPDEV)pso->dhpdev)->pmdev->dwUniq == PMDEV_UNIQ);
        return ((PPDEV)pso->dhpdev)->pmdev;
    }
    else
    {
        return NULL;
    }
}


//*********************************************************
//
//  bNeedToEnumerate
//
//  checks if pso is the multidev- primary surface
//
//********************************************************
BOOL _inline bNeedToEnumerate(SURFOBJ *pso)
{
    BOOL    bRet = FALSE;

    if(  (NULL != pmdevGetFromPso(pso))
       &&(STYPE_DEVICE == pso->iType)
      )
        bRet = TRUE;

    return bRet;
}

//*********************************************************
//
//  psoGetDevicePSO
//
//  get singleboard primary of ulDevID
//
//********************************************************
SURFOBJ _inline *psoGetDevicePSO(SURFOBJ *pso, ULONG ulDevID)
{

    if (bNeedToEnumerate(pso))
    {
        PMDEV pmdev = pmdevGetFromPso(pso);  

        ASSERT(NULL != pmdev );
        ASSERT(ulDevID < MAX_MULTIDEV);

        pso = pmdev->abdDevices[ulDevID].pso;
    }

    return pso;
}

SURFOBJ _inline *psoGetBoardRectAndSync( IN SURFOBJ      *pso,
                                         IN BOARDDESC    *pBoardDesc,
                                         IN BOARDDESC    *pOtherBoardDesc,  // needed to decide if we export the bitmap surface
                                         OUT RECTL       *prclBoard)
{
    ASSERT(prclBoard);


    if (pso)
    {
        if (bNeedToEnumerate(pso))
        {
            ASSERT(pBoardDesc);

            if (pOtherBoardDesc)
            {
                pso = pBoardDesc->psoBitmap;
                DrvSynchronize(pBoardDesc->dhpdev, NULL);   // this indicates that we'll use this surface as the source for dev to dev blits -> sync
            }
            else
            {
                pso = pBoardDesc->pso;
            }
            *prclBoard = pBoardDesc->rclBoard;

        }
        else
        {
            prclBoard->left   = 0;
            prclBoard->top    = 0;
            prclBoard->right  = pso->sizlBitmap.cx;
            prclBoard->bottom = pso->sizlBitmap.cy;
        }
    }
    return pso;
}

ULONG DEVOBJ_cEnumStart(IN PDEVOBJ  pdo,
                        IN SURFOBJ  *pso,
                        IN RECTL    *prclBounds, // don´t care if bAll = TRUE
                        IN BOOL     bAll,       // TRUE: enumerates all, FALSE enumerates only dev clipped against prclbounds
                        IN ULONG    iDir);  // CD_ANY...

BOOL DEVOBJ_bEnum(IN PDEVOBJ    pdo,
                  IN ENUMDEV16  *pEnumDev16);



typedef struct _BLT_PARAM
{
    SURFOBJ *psoDst;
    SURFOBJ *psoSrc;
    RECTL   rclClip;
    RECTL   *prclClip;
    RECTL   rclDst;
    RECTL   *prclDst;
    POINTL  ptlSrc;
    POINTL  *pptlSrc;
    POINTL  ptlMask;
    POINTL  *pptlMask;
    POINTL  ptlBrush;
    POINTL  *pptlBrush;
}
BLT_PARAM;


BOOL APIENTRY MulResetPDEV(DHPDEV dhpdevOld, DHPDEV dhpdevNew);

DHPDEV APIENTRY MulEnablePDEV(DEVMODEW* pdm, PWSTR pwszLogAddr, ULONG  cPat, HSURF* phsurfPatterns, 
                    ULONG cjCaps, ULONG* pdevcaps, ULONG cjDevInfo, DEVINFO* pdi,            
                    HDEV  hdev, PWSTR pwszDeviceName, HANDLE hDriver);

VOID APIENTRY MulDisablePDEV(DHPDEV  dhpdev);

VOID APIENTRY MulCompletePDEV(DHPDEV dhpdev,HDEV   hdev);

VOID APIENTRY MulSynchronize(DHPDEV dhpdev, RECTL  *prcl);

HSURF APIENTRY MulEnableSurface(DHPDEV dhpdev);

VOID APIENTRY MulDisableSurface(DHPDEV dhpdev);

BOOL APIENTRY MulAssertMode(DHPDEV  dhpdev,BOOL    bEnable);

ULONG APIENTRY MulGetModes(HANDLE hDriver, ULONG cjSize, DEVMODEW*   pdm);

BOOL APIENTRY MulEnableDirectDraw(DHPDEV dhpdev, DD_CALLBACKS*  pCallBacks,
                            DD_SURFACECALLBACKS*    pSurfaceCallBacks, DD_PALETTECALLBACKS*    pPaletteCallBacks);

BOOL APIENTRY MulGetDirectDrawInfo(DHPDEV dhpdev, DD_HALINFO* pHalInfo, DWORD* lpdwNumHeaps,
                          VIDEOMEMORY* pvmList, DWORD* lpdwNumFourCC, DWORD* lpdwFourCC);

VOID APIENTRY MulDisableDirectDraw(DHPDEV      dhpdev);

ULONG APIENTRY MulEscape(SURFOBJ*    pso, ULONG       iEsc, ULONG       cjIn,
                VOID*       pvIn, ULONG       cjOut, VOID*       pvOut);

ULONG APIENTRY MulDrawEscape(SURFOBJ *pso, ULONG    iEsc, CLIPOBJ *pco,
                    RECTL   *prcl, ULONG    cjIn, PVOID    pvIn);

VOID APIENTRY MulDestroyFont(FONTOBJ *pfo);

BOOL APIENTRY MulStretchBlt(
SURFOBJ*            psoDst,
SURFOBJ*            psoSrc,
SURFOBJ*            psoMask,
CLIPOBJ*            pco,
XLATEOBJ*           pxlo,
COLORADJUSTMENT*    pca,
POINTL*             pptlHTOrg,
RECTL*              prclDst,
RECTL*              prclSrc,
POINTL*             pptlMask,
ULONG               iMode);

HBITMAP APIENTRY MulCreateDeviceBitmap(DHPDEV dhpdev, SIZEL sizl, ULONG iFormat);

VOID APIENTRY MulDeleteDeviceBitmap(DHSURF dhsurf);

BOOL APIENTRY MulRealizeBrush(
BRUSHOBJ* pbo,
SURFOBJ*  psoTarget,
SURFOBJ*  psoPattern,
SURFOBJ*  psoMask,
XLATEOBJ* pxlo,
ULONG     iHatch);

BOOL APIENTRY MulPaint(
SURFOBJ*  pso,
CLIPOBJ*  pco,
BRUSHOBJ* pbo,
POINTL*   pptlBrushOrg,
MIX       mix);

BOOL APIENTRY MulFillPath(
SURFOBJ*  pso,
PATHOBJ*  ppo,
CLIPOBJ*  pco,
BRUSHOBJ* pbo,
POINTL*   pptlBrushOrg,
MIX       mix,
FLONG     flOptions);

BOOL APIENTRY MulStrokePath(
SURFOBJ*   pso,
PATHOBJ*   ppo,
CLIPOBJ*   pco,
XFORMOBJ*  pxo,
BRUSHOBJ*  pbo,
POINTL*    pptlBrushOrg,
LINEATTRS* plineattrs,
MIX        mix);

BOOL APIENTRY MulLineTo(
SURFOBJ*    pso,
CLIPOBJ*    pco,
BRUSHOBJ*   pbo,
LONG        x1,
LONG        y1,
LONG        x2,
LONG        y2,
RECTL*      prclBounds,
MIX         mix);

BOOL APIENTRY MulTextOut(
SURFOBJ*  pso,
STROBJ*   pstro,
FONTOBJ*  pfo,
CLIPOBJ*  pco,
RECTL*    prclExtra,
RECTL*    prclOpaque,
BRUSHOBJ* pboFore,
BRUSHOBJ* pboOpaque,
POINTL*   pptlOrg,
MIX       mix);

BOOL APIENTRY MulBitBlt(
SURFOBJ*  psoDst,
SURFOBJ*  psoSrc,
SURFOBJ*  psoMask,
CLIPOBJ*  pco,
XLATEOBJ* pxlo,
RECTL*    prclDst,
POINTL*   pptlSrc,
POINTL*   pptlMask,
BRUSHOBJ* pbo,
POINTL*   pptlBrush,
ROP4      rop4);

BOOL APIENTRY MulCopyBits(
SURFOBJ*  psoDst,
SURFOBJ*  psoSrc,
CLIPOBJ*  pco,
XLATEOBJ* pxlo,
RECTL*    prclDst,
POINTL*   pptlSrc);

BOOL APIENTRY MulSetPalette(
DHPDEV  dhpdev,
PALOBJ* ppalo,
FLONG   fl,
ULONG   iStart,
ULONG   cColors);

ULONG APIENTRY MulDitherColor(
DHPDEV dhpdev,
ULONG  iMode,
ULONG  rgb,
ULONG* pul);

ULONG APIENTRY MulSetPointerShape(
SURFOBJ*  pso,
SURFOBJ*  psoMask,
SURFOBJ*  psoColor,
XLATEOBJ* pxlo,
LONG      xHot,
LONG      yHot,
LONG      x,
LONG      y,
RECTL*    prcl,
FLONG     fl);

VOID APIENTRY MulMovePointer(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl);

BOOL APIENTRY MulSwapBuffers(SURFOBJ *psoDst, WNDOBJ *pwo);

BOOL APIENTRY MulSetPixelFormat(
    SURFOBJ *pso,
    LONG    iPixelFormat,
    HWND    hWnd);


LONG APIENTRY MulDescribePixelFormat(
    DHPDEV                  dhpdev,
    LONG                    iPixelFormat,
    ULONG                   cjpfd,
    PIXELFORMATDESCRIPTOR   *ppfd);

ULONG_PTR APIENTRY MulSaveScreenBits(
    SURFOBJ   *pso,
    ULONG      iMode,
    ULONG_PTR  ident,
    RECTL     *prcl );


#endif // NT4_MULTI_DEV

#endif //_INCLUDE_NT4MULTIDEV
