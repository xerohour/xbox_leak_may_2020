/**************************************************************************
** wksappsupport.c
**
** This module handles the ESC_NV_WKS_APP_SUPPORT escape to 
** support the workstation tools interface.
**
** Copyright (c) 2000, NVIDIA, Corporation.  All rights reserved.
**
** THE INFORMATION CONTAINED HEREIN IS PROPRIETARY AND CONFIDENTIAL TO
** NVIDIA, CORPORATION.  USE, REPRODUCTION OR DISCLOSURE TO ANY THIRD PARTY
** IS SUBJECT TO WRITTEN PRE-APPROVAL BY NVIDIA, CORPORATION.
**
** FNicklisch 09.11.2000: New
** FNicklisch 21.11.2000: Implemented SUBESC_WKS_WINDOW_STEREOMODE
**************************************************************************/

// import
#include "precomp.h"
#include "driver.h"
#include "Nvcm.h"
#include "nvapi.h"
#include "nv32.h"
#include "oglDD.h"
#include "oglstereo.h"
#include "wincommon.h"
#include "pixelfmt.h"

// export
#include "wksappsupport.h"


static BOOL bWksWindowStereoSupport(
    IN     PPDEV ppdev,
    IN     NV_WKS_WINDOW_STEREOMODE_IN  *pWindowStereoModeIn ,
       OUT NV_WKS_WINDOW_STEREOMODE_OUT *pWindowStereoModeOut);


#define CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cj, cjNeeded, lRet, pHeadOut, lErrorCode); \
    if (cj != cjNeeded)                     \
    {                                       \
        if (NULL != pHeadOut)               \
        {                                   \
            pHeadOut->ulSize  = cjNeeded;   \
            pHeadOut->lResult = lErrorCode; \
        }                                   \
        lRet = WKS_ESC_SUCCEED;             \
        goto Exit;                          \
    }

#define CHECK_CJINOUT_AND_GOTO_EXIT_IF_FAIL(cjIn, cjInNeeded, cjOut, cjOutNeeded, lRet, pHeadOut);   \
    CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjIn , cjInNeeded , lRet, pHeadOut, NV_WKS_RESULT_ERROR_INSIZE ); \
    CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjOut, cjOutNeeded, lRet, pHeadOut, NV_WKS_RESULT_ERROR_OUTSIZE);


//*************************************************************************
// lWksAppSupport
//
// handles all ESC_NV_WKS_APP_SUPPORT subescapes.
// return WKS_ESC_SUCCEED     - escape was handled, detailed status
//                              in NV_WKS_HEAD_OUT.
//        WKS_ESC_UNSUPPORTED - unsupported escape
//        WKS_ESC_FAILED      - escape failed
//*************************************************************************
LONG lWksAppSupport(
    SURFOBJ *pso, 
    ULONG   cjIn, 
    VOID    *pvIn,
    ULONG   cjOut, 
    VOID    *pvOut)
{
    LONG             lRet        = WKS_ESC_FAILED; // default to error
    NV_WKS_HEAD_IN  *pHeadIn     = (NV_WKS_HEAD_IN *)pvIn;
    NV_WKS_HEAD_OUT *pHeadOut    = NULL;
    PPDEV            ppdev       = NULL;

    ASSERT(NULL != pso);
    ASSERT(NULL != pvIn);
    ASSERT(cjIn > 0);

    if (   (NULL == pvIn)
        || (cjIn < sizeof(NV_WKS_HEAD_IN))
        || ( (cjOut != 0) && (NULL == pvOut) ) 
        || ( (cjOut != 0) && (cjOut < sizeof(NV_WKS_HEAD_OUT) ) )
       )
    {
        // interface error, escape used in a wrong manner
        goto Exit;
    }

    //
    // Check and initialize out structure
    //
    if (cjOut != 0)        
    {
        ASSERT(NULL != pvOut);
        ASSERT(cjOut>= sizeof(NV_WKS_HEAD_OUT));

        RtlZeroMemory(pvOut, cjOut);

        pHeadOut            = (NV_WKS_HEAD_OUT *)pvOut;
        pHeadOut->ulVersion = NV_WKS_VERSION;

        CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjIn , pHeadIn->ulInSize , lRet, pHeadOut, NV_WKS_RESULT_ERROR_INSIZE );
        CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjOut, pHeadIn->ulOutSize, lRet, pHeadOut, NV_WKS_RESULT_ERROR_OUTSIZE);

        pHeadOut->ulSize    = pHeadIn->ulOutSize;
        pHeadOut->lResult   = NV_WKS_RESULT_ERROR; // until now we assume we fail
    }

    ppdev = (PDEV *)pso->dhpdev;

    //
    // handle known escapes
    //
    switch (pHeadIn->ulSubEsc)
    {
        case SUBESC_WKS_GET_CAPS:
        {
            ULONG               status;
            NV_WKS_GET_CAPS_IN  *pGetCapsIn  = pvIn;
            NV_WKS_GET_CAPS_OUT *pGetCapsOut = pvOut;

            CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjIn , sizeof(NV_WKS_GET_CAPS_IN ), lRet, pHeadOut, NV_WKS_RESULT_ERROR_INSIZE );
            CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjOut, sizeof(NV_WKS_GET_CAPS_OUT), lRet, pHeadOut, NV_WKS_RESULT_ERROR_OUTSIZE);

            //
            // Get data
            //

            // translate the config bits that possibly change in future to a stable interface
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_AA_LINES       ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_AA_LINES       ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_AA_POLYS       ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_AA_POLYS       ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_AKILL_USERCLIP ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_AKILL_USERCLIP ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_LOGIC_OPS      ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_LOGIC_OPS      ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_NV15_ALINES    ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_NV15_ALINES    ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_2SIDED_LIGHTING) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_2SIDED_LIGHTING;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_QUADRO_GENERIC ;
            if (ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_UBB ) 
                pGetCapsOut->flGraphicsCaps |= NV_WKS_GRAPHICS_CAPS_UBB ;

            pGetCapsOut->ulMaxClipRects = HWGFXCAPS_MAXCLIPS(ppdev);

            if ((ppdev->hwGfxCaps & NV_CFG_GRAPHICS_CAPS_QUADRO_GENERIC))
            {
                ULONG ulPFDCaps=0;
                ASSERT(pGetCapsOut->flGraphicsCaps & NV_WKS_GRAPHICS_CAPS_QUADRO_GENERIC);

                pGetCapsOut->flWksCaps |= NV_WKS_CAPS_WORKSTATION_BOARD;

                bOglPfdCheckFlags(ppdev, &ulPFDCaps);

                if (ulPFDCaps & WIN_PFD_SUPPORTS_OVERLAYS)
                    pGetCapsOut->flWksCaps |= NV_WKS_CAPS_OVERLAY_CAPABLE;

                if (ulPFDCaps & WIN_PFD_SUPPORTS_STEREO)
                    pGetCapsOut->flWksCaps |= NV_WKS_CAPS_STERO_CAPABLE;
            }

            pHeadOut->lResult = NV_WKS_RESULT_SUCCESS;
            break;
        }

        case SUBESC_WKS_WINDOW_STEREOMODE:
        {
            CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjIn , sizeof(NV_WKS_WINDOW_STEREOMODE_IN ), lRet, pHeadOut, NV_WKS_RESULT_ERROR_INSIZE );
            CHECK_CJ_AND_GOTO_EXIT_IF_FAIL(cjOut, sizeof(NV_WKS_WINDOW_STEREOMODE_OUT), lRet, pHeadOut, NV_WKS_RESULT_ERROR_OUTSIZE);

            if (!bWksWindowStereoSupport(ppdev, pvIn, pvOut))
            {
                // severe error!
                goto Exit;
            }

            break;
        }

        default:
        {
            // unsupported escape
            if (cjOut >= sizeof(NV_WKS_HEAD_OUT))
            {
                pHeadOut->ulSize  = sizeof(NV_WKS_HEAD_OUT); // can only feed header
                pHeadOut->lResult = NV_WKS_RESULT_ERROR_SUBESC_NOT_SUPPORTED;
            }
            break;
        }
    }

    lRet = WKS_ESC_SUCCEED; // escape handled

Exit:
    return (lRet);
}


//*************************************************************************
// bWksWindowStereoSupport
//
// handles SUBESC_WKS_WINDOW_STEREOMODE escape. In and out structures 
// must be valid.
//
// NOTE: example escape code below
//
// return: FALSE - failed, pWindowStereoModeOut not valid
//         TRUE  - succeeded, pWindowStereoModeOut valid
//*************************************************************************
static BOOL bWksWindowStereoSupport(
    IN     PPDEV ppdev,
    IN     NV_WKS_WINDOW_STEREOMODE_IN  *pWindowStereoModeIn ,
       OUT NV_WKS_WINDOW_STEREOMODE_OUT *pWindowStereoModeOut)
{
    BOOL                        bRet            = FALSE;
    NV_OPENGL_CLIENT_INFO_LIST *clientList      = NULL;
    NV_OPENGL_CLIENT_INFO      *clientInfo      = NULL;
    BOOL                        bStereoClient   = FALSE;

    ASSERT(NULL != ppdev               );
    ASSERT(NULL != pWindowStereoModeIn );
    ASSERT(NULL != pWindowStereoModeOut);

    pWindowStereoModeOut->hwnd = pWindowStereoModeIn->hwnd;


    // work on specified window?
    if (NULL != pWindowStereoModeIn->hwnd)
    {
        // Set or query for specified window
        clientList = OglFindClientInfoFromHWnd(ppdev, pWindowStereoModeIn->hwnd);

        if (NULL != clientList)
        {
            clientInfo = &clientList->clientInfo;

            bStereoClient = bOglIsStereoClient(clientInfo);

            switch (pWindowStereoModeIn->ulMode)
            {
                case NV_WKS_WINDOW_STEREOMODE_ENABLE :
                case NV_WKS_WINDOW_STEREOMODE_DISABLE:
                {
                    if (bStereoClient)
                    {
                        // set stereo clients stereo mode
                        clientInfo->bStereoDisabled = (NV_WKS_WINDOW_STEREOMODE_DISABLE == pWindowStereoModeIn->ulMode) ? TRUE : FALSE;

                        //
                        // KEY: Update stereo mode now!
                        //
                        bOglStereoModeUpdate(ppdev);

                        pWindowStereoModeOut->ulMode      = clientInfo->bStereoDisabled ? NV_WKS_WINDOW_STEREOMODE_DISABLED 
                                                                                        : NV_WKS_WINDOW_STEREOMODE_ENABLED;
                        pWindowStereoModeOut->hdr.lResult = NV_WKS_RESULT_SUCCESS;
                    }
                    else
                    {
                        // error, cannot set stereo mode on monoscopic window
                        pWindowStereoModeOut->ulMode      = NV_WKS_WINDOW_STEREOMODE_NO_STEREO_HWND;
                        pWindowStereoModeOut->hdr.lResult = NV_WKS_RESULT_ERROR_PARAMETER;
                    }
                    break;
                }
                case NV_WKS_WINDOW_STEREOMODE_QUERY:
                {
                    if (bStereoClient)
                    {
                        pWindowStereoModeOut->ulMode  = clientInfo->bStereoDisabled ? NV_WKS_WINDOW_STEREOMODE_DISABLED 
                                                                                    : NV_WKS_WINDOW_STEREOMODE_ENABLED;
                    }
                    else
                    {
                        pWindowStereoModeOut->ulMode  = NV_WKS_WINDOW_STEREOMODE_NO_STEREO_HWND;
                    }
                    // succeed because it's a query
                    pWindowStereoModeOut->hdr.lResult = NV_WKS_RESULT_SUCCESS;
                    break;
                }
            }
        }
        else
        {
            // error, given window not found
            pWindowStereoModeOut->ulMode      = NV_WKS_WINDOW_STEREOMODE_INVALID_HWND;
            pWindowStereoModeOut->hdr.lResult = NV_WKS_RESULT_ERROR_PARAMETER;
        }
    } 
    else
    {
        // No window given, query only!

        switch (pWindowStereoModeIn->ulMode)
        {
            case NV_WKS_WINDOW_STEREOMODE_QUERY:
            {
                if (0==cOglStereoClients(ppdev))
                {
                    pWindowStereoModeOut->ulMode = NV_WKS_WINDOW_STEREOMODE_NO_STEREO;
                }
                else
                {
                    if (ppdev->bOglStereoActive)
                        pWindowStereoModeOut->ulMode = NV_WKS_WINDOW_STEREOMODE_ENABLED;
                    else
                        pWindowStereoModeOut->ulMode = NV_WKS_WINDOW_STEREOMODE_DISABLED;
                }

                pWindowStereoModeOut->hdr.lResult    = NV_WKS_RESULT_SUCCESS;
                break;
            }
            case NV_WKS_WINDOW_STEREOMODE_ENABLE :
            case NV_WKS_WINDOW_STEREOMODE_DISABLE:
            {
                // error, not allowed without window
                pWindowStereoModeOut->ulMode      = NV_WKS_WINDOW_STEREOMODE_INVALID_HWND;
                pWindowStereoModeOut->hdr.lResult = NV_WKS_RESULT_ERROR_PARAMETER;
                break;
            }
            default:
            {
                // error, unkown parameter
                pWindowStereoModeOut->hdr.lResult            = NV_WKS_RESULT_ERROR_PARAMETER;
                break;
            }
        }
    }

    bRet = TRUE;

    return (bRet);
}



#if 0 

// 
// Example code, please do not delete!
//

/* 
// SUBESC_WKS_GET_CAPS
{
    HDC                 hDC;
    NV_WKS_GET_CAPS_IN  wksGetCapsIn;
    NV_WKS_GET_CAPS_OUT wksGetCapsOut;
    int                 iResult;

    hDC = GetDC(hWnd); // NOTE: a NULL hWnd will fail on Windows 2000 multiscreen

    if (NULL != hDC)
    {

        ZeroMemory(&wksGetCapsIn,  sizeof(NV_WKS_GET_CAPS_IN));
        ZeroMemory(&wksGetCapsOut, sizeof(NV_WKS_GET_CAPS_OUT));

        NV_WKS_INIT_HEAD_IN(&wksGetCapsIn, SUBESC_WKS_GET_CAPS, sizeof(NV_WKS_GET_CAPS_IN), sizeof(NV_WKS_GET_CAPS_OUT), 0);

        iResult = ExtEscape(hDC,                         // handle to DC
                            ESC_NV_WKS_APP_SUPPORT,      // escape function
                            sizeof(NV_WKS_GET_CAPS_IN),  // size of input structure
                            (LPCSTR)&wksGetCapsIn,       // input structure
                            sizeof(NV_WKS_GET_CAPS_OUT), // size of output structure
                            (LPSTR)&wksGetCapsOut);      // output structure
        if (iResult > 0)
        {
            // Escape succeeded, wksGetCapsOut valid
            if (   (sizeof(NV_WKS_GET_CAPS_OUT) == wksGetCapsOut.hdr.ulSize) // expected size must match
                && (wksGetCapsOut.hdr.ulVersion >= NV_WKS_VERSION)           // driver must at least have this version of interface
                && (wksGetCapsOut.hdr.lResult   >  0) )                      // no error
            {
                // Succeeded!
                //
                // TODO: add code to work with 
                //  wksGetCapsOut.flGraphicsCaps;
                //  wksGetCapsOut.ulMaxClipRects;
                //  wksGetCapsOut.flWksCaps;
            }
            else
            {
                // ERROR: Any internal error in usage of escape (sizes, state of driver, ...)
            }
        }
        else if (0 == iResult)
        {
            // ERROR: Escape not implemented
        }
        else // iResult < 0
        {
            // ERROR: Escape failed
        }

        ReleaseDC(hWnd, hDC);
    }
    else
    {
        // ERROR: couldn't retrieve a HDC
    }
}


//SUBESC_WKS_WINDOW_STEREOMODE

//*************************************************************************
// bNvWksWindowStereomodeSelect
//
// enables or disables stereo mode on given window. The function will only 
// succeed if the window is an OpenGL window that has a stereo pixelformat 
// selected.
//
// return: TRUE  - succeeded, returns current status to pbNewStereoStatus
//         FALSE - failed, pbNewStereoStatus not touched
//
// FNicklisch 21.11.2000: new
//*************************************************************************
BOOL bNvWksWindowStereomodeSelect(
    IN     HWND hWnd,               // hwnd of stereo window
       OUT BOOL bStereoModeEnable,  // TRUE: enable stereo mode, FALSE: disable stereo mode
       OUT BOOL *pbNewStereoStatus)   // pointer to return status or NULL
{
    BOOL                         bRet          = FALSE;
    NV_WKS_WINDOW_STEREOMODE_IN  wksWindowStereomodeIn ;
    NV_WKS_WINDOW_STEREOMODE_OUT wksWindowStereomodeOut;
    size_t                       inSize ;
    size_t                       outSize;
    int                          iResult;
    HDC                          hDC;

    hDC = GetDC(hWnd);

    if (NULL != hDC)
    {
        inSize  = sizeof(NV_WKS_WINDOW_STEREOMODE_IN );
        outSize = sizeof(NV_WKS_WINDOW_STEREOMODE_OUT);

        ZeroMemory(&wksWindowStereomodeIn , inSize );
        ZeroMemory(&wksWindowStereomodeOut, outSize);

        NV_WKS_INIT_HEAD_IN(&wksWindowStereomodeIn, SUBESC_WKS_WINDOW_STEREOMODE, inSize, outSize, 0);

        wksWindowStereomodeIn.hwnd   = hWnd;
        wksWindowStereomodeIn.ulMode = bStereoModeEnable ? NV_WKS_WINDOW_STEREOMODE_ENABLE : NV_WKS_WINDOW_STEREOMODE_DISABLE;

        iResult = ExtEscape(hDC,                             // handle to DC
                            ESC_NV_WKS_APP_SUPPORT,          // escape function
                            inSize,                          // size of input structure
                            (LPCSTR)&wksWindowStereomodeIn, // input structure
                            outSize,                         // size of output structure
                            (LPSTR)&wksWindowStereomodeOut); // output structure

        if (iResult > 0)
        {
            if (   (outSize == wksWindowStereomodeOut.hdr.ulSize)           // expected size must match
                && (wksWindowStereomodeOut.hdr.ulVersion >= NV_WKS_VERSION) // driver must at least have this version of interface
                && (wksWindowStereomodeOut.hdr.lResult   == NV_WKS_RESULT_SUCCESS) )            // no error
            {
                if (wksWindowStereomodeOut.hwnd != hWnd)
                {
                    // ERROR, wrong out hwnd
                }
                else
                {
                    switch (wksWindowStereomodeOut.ulMode)
                    {
                        case NV_WKS_WINDOW_STEREOMODE_DISABLED:
                        {
                            if (pbNewStereoStatus)
                                *pbNewStereoStatus = FALSE;
                            bRet = TRUE;
                            break;
                        }

                        case NV_WKS_WINDOW_STEREOMODE_ENABLED:
                        {
                            if (pbNewStereoStatus)
                                *pbNewStereoStatus = TRUE;
                            bRet = TRUE;
                            break;
                        }

                        default:
                        {
                            // any error, wrong usage of escape
                            break;
                        }
                    }
                }
            }
            else
            {
                // ERROR: Any internal error in usage of escape (sizes, state of driver, ...)
            }
        }
        else if (0 == iResult)
        {
            // ERROR: Escape not implemented
        }
        else // iResult < 0
        {
            // ERROR: Escape failed
        }
        ReleaseDC(hWnd,hDC);
    }

    return (bRet);
}
  
    

  
    
*/
#endif

// End of wksappsupport.c
