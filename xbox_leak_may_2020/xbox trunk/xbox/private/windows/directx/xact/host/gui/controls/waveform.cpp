/***************************************************************************
 *
 *  Copyright (C) 1/30/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       waveform.cpp
 *  Content:    Renders a waveform bitmap.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  1/30/2002   dereks  Created.
 *
 ****************************************************************************/

// #define WAVEFORM_USE_XBOX_COLORS

#include "xactctl.h"

const UINT CWaveformRenderer::m_nDefaultScale = 16;
const UINT CWaveformRenderer::m_nDefaultResolution = 8;

const LPCTSTR CWaveformControl::m_pszClassName = TEXT("WAVEFORM");
const UINT CWaveformControl::m_nSectionDivisor = 4;


/****************************************************************************
 *
 *  CWaveformRenderer
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::CWaveformRenderer"

CWaveformRenderer::CWaveformRenderer
(
    void
)
{
    m_nState = WAVEFORM_RENDER_FULL;
    m_hWndDisplay = NULL;
    m_hdcDraw = NULL;
    m_hbrActiveBackground = NULL;
    m_hbrInactiveBackground = NULL;
    m_hpnWaveform = NULL;
    m_pvWaveData = NULL;
    m_cbWaveData = NULL;
    m_cbHorizOffset = 0;
    m_nScale = m_nDefaultScale;
    m_nResolution = m_nDefaultResolution;

    SetColors(NULL);
    SetRect(NULL);
}


/****************************************************************************
 *
 *  ~CWaveformRenderer
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::~CWaveformRenderer"

CWaveformRenderer::~CWaveformRenderer
(
    void
)
{
    if(m_hdcDraw)
    {
        DeleteDC(m_hdcDraw);
    }

    if(m_hbrActiveBackground)
    {
        DeleteObject(m_hbrActiveBackground);
    }

    if(m_hbrInactiveBackground)
    {
        DeleteObject(m_hbrInactiveBackground);
    }

    if(m_hpnWaveform)
    {
        DeleteObject(m_hpnWaveform);
    }
}


/****************************************************************************
 *
 *  Initialize
 *
 *  Description:
 *      Initializes the object.
 *
 *  Arguments:
 *      HWND [in]: window handle to draw on.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::Initialize"

BOOL
CWaveformRenderer::Initialize
(
    HWND                    hWnd
)
{
    BOOL                    fSuccess    = TRUE;
    HDC                     hdc;
    RECT                    rc;

    m_hWndDisplay = hWnd;
    
    // 
    // Create the offscreen device context
    //
    
    hdc = GetDC(hWnd);

    if(!(m_hdcDraw = CreateCompatibleDC(hdc)))
    {
        DPF_ERROR("Failed to create compatible DC");
        fSuccess = FALSE;
    }

    if(hdc)
    {
        ReleaseDC(hWnd, hdc);
    }

    //
    // Default the default bitmap size to the client area of the window
    //

    if(fSuccess)
    {
        GetClientRect(hWnd, &rc);

        fSuccess = SetRect(&rc);
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  SetRect
 *
 *  Description:
 *      Sets the drawing rectangle.
 *
 *  Arguments:
 *      LPCRECT [in]: rectangle.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::SetRect"

BOOL
CWaveformRenderer::SetRect
(
    LPCRECT                 prcDrawing
)
{
    if(prcDrawing)
    {
        m_rcDisplay = *prcDrawing;
    }
    else
    {
        ZeroMemory(&m_rcDisplay, sizeof(m_rcDisplay));
    }

    m_nWidth = m_rcDisplay.right - m_rcDisplay.left;
    m_nHeight = m_rcDisplay.bottom - m_rcDisplay.top;

    SetRenderState(WAVEFORM_RENDER_CREATEBITMAP);

    return TRUE;
}


/****************************************************************************
 *
 *  SetColors
 *
 *  Description:
 *      Sets the colors used by the renderer.
 *
 *  Arguments:
 *      LPCWAVEFORMCOLORS [in]: color data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::SetColors"

BOOL
CWaveformRenderer::SetColors
(
    LPCWAVEFORMCOLORS       pColors
)
{
    if(pColors)
    {
        m_colors = *pColors;
    }
    else
    {

#ifdef WAVEFORM_USE_XBOX_COLORS

        m_colors.rgbBackground = RGB(0, 0, 0);
        m_colors.rgbInactiveBackground = RGB(127, 127, 127);
        m_colors.rgbWaveform = RGB(0, 255, 0);

#else // WAVEFORM_USE_XBOX_COLORS

        m_colors.rgbBackground = GetSysColor(COLOR_WINDOW);
        m_colors.rgbInactiveBackground = GetSysColor(COLOR_GRAYTEXT);
        m_colors.rgbWaveform = GetSysColor(COLOR_WINDOWTEXT);

#endif // WAVEFORM_USE_XBOX_COLORS

    }

    SetRenderState(WAVEFORM_RENDER_DRAWINGTOOLS);

    return TRUE;
}


/****************************************************************************
 *
 *  SetWaveData
 *
 *  Description:
 *      Sets the wave data used by the renderer.
 *
 *  Arguments:
 *      LPCWAVEFORMDATA [in]: wave data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::SetWaveData"

BOOL
CWaveformRenderer::SetWaveData
(
    LPCWAVEFORMDATA         pWaveData
)
{
    BOOL                    fSuccess    = TRUE;
    
    if(pWaveData)
    {
        if(!pWaveData->pwfxFormat)
        {
            DPF_ERROR("No wave data format supplied");
            fSuccess = FALSE;
        }
        else if(!pWaveData->pvWaveData)
        {
            DPF_ERROR("No wave data buffer supplied");
            fSuccess = FALSE;
        }
        else if(!pWaveData->cbWaveData)
        {
            DPF_ERROR("No wave data size supplied");
            fSuccess = FALSE;
        }
        else if(WAVE_FORMAT_PCM != pWaveData->pwfxFormat->wFormatTag)
        {
            DPF_ERROR("Format not PCM");
            fSuccess = FALSE;
        }

        if(fSuccess)
        {
            CopyMemory(&m_wfxFormat, pWaveData->pwfxFormat, sizeof(m_wfxFormat) - sizeof(m_wfxFormat.cbSize));

            m_wfxFormat.cbSize = 0;

            m_pvWaveData = pWaveData->pvWaveData;
            m_cbWaveData = pWaveData->cbWaveData;
            m_cbHorizOffset = 0;
        }
    }
    else
    {
        ZeroMemory(&m_wfxFormat, sizeof(m_wfxFormat));

        m_pvWaveData = NULL;
        m_cbWaveData = NULL;
    }

    SetRenderState(WAVEFORM_RENDER_REDRAW);

    return fSuccess;
}


/****************************************************************************
 *
 *  SetOffset
 *
 *  Description:
 *      Sets the bitmap Offset.
 *
 *  Arguments:
 *      UINT [in]: bitmap width.
 *      UINT [in]: bitmap height.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::SetOffset"

BOOL
CWaveformRenderer::SetOffset
(
    UINT                    cbOffset
)
{
    m_cbHorizOffset = cbOffset;

    SetRenderState(WAVEFORM_RENDER_REDRAW);

    return TRUE;
}


/****************************************************************************
 *
 *  SetScale
 *
 *  Description:
 *      Sets the scaling factor used to render the wave data.
 *
 *  Arguments:
 *      UINT [in]: scale.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::SetScale"

BOOL
CWaveformRenderer::SetScale
(
    UINT                    nScale
)
{
    m_nScale = max(1, nScale);

    SetRenderState(WAVEFORM_RENDER_REDRAW);

    return TRUE;
}


/****************************************************************************
 *
 *  Render
 *
 *  Description:
 *      Draws the waveform.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::Render"

BOOL
CWaveformRenderer::Render
(
    HDC                     hdcDisplay,
    LPCRECT                 prcUpdate
)
{
    BOOL                    fSuccess    = TRUE;
    BOOL                    fReleaseDC  = FALSE;

    if(!hdcDisplay)
    {
        if(!(hdcDisplay = GetDC(m_hWndDisplay)))
        {
            DPF_ERROR("Failed to get display DC");
            fSuccess = FALSE;
        }

        if(fSuccess)
        {
            fReleaseDC = TRUE;
        }
    }

    if(fSuccess && (m_nState >= WAVEFORM_RENDER_CREATEBITMAP))
    {
        fSuccess = CreateBitmap(hdcDisplay);
    }

    if(fSuccess && (m_nState >= WAVEFORM_RENDER_DRAWINGTOOLS))
    {
        fSuccess = CreateDrawingTools();
    }
    
    if(fSuccess && (m_nState >= WAVEFORM_RENDER_REDRAW))
    {
        fSuccess = DrawBackground();
    }

    if(fSuccess && (m_nState >= WAVEFORM_RENDER_REDRAW))
    {
        fSuccess = DrawWaveform();
    }

    if(fSuccess)
    {
        m_nState = WAVEFORM_RENDER_NONE;
    }

    if(fSuccess)
    {
        fSuccess = DisplayBitmap(hdcDisplay, prcUpdate);
    }

    if(fReleaseDC)
    {
        ReleaseDC(m_hWndDisplay, hdcDisplay);
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  CreateBitmap
 *
 *  Description:
 *      (Re)creates the waveform bitmap.
 *
 *  Arguments:
 *      HDC [in]: display DC.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::CreateBitmap"

BOOL
CWaveformRenderer::CreateBitmap
(
    HDC                     hdcDisplay
)
{
    BOOL                    fSuccess    = TRUE;
    HBITMAP                 hbmDraw;

    ASSERT(hdcDisplay);

    if(!(hbmDraw = CreateCompatibleBitmap(hdcDisplay, m_nWidth, m_nHeight)))
    {
        DPF_ERROR("Failed to create compatible bitmap");
        fSuccess = FALSE;
    }

    if(fSuccess)
    {
        if(hbmDraw = (HBITMAP)SelectObject(m_hdcDraw, hbmDraw))
        {
            if(HGDI_ERROR == hbmDraw)
            {
                DPF_ERROR("Failed to select the off-screen bitmap");
                fSuccess = FALSE;
            }
            else
            {
                DeleteObject(hbmDraw);
            }
        }
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  CreateDrawingTools
 *
 *  Description:
 *      Creates the drawing objects.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::CreateDrawingTools"

BOOL
CWaveformRenderer::CreateDrawingTools
(
    void
)
{
    BOOL                    fSuccess        = TRUE;

    if(m_hbrActiveBackground)
    {
        DeleteObject(m_hbrActiveBackground);
        m_hbrActiveBackground = NULL;
    }

    if(m_hbrInactiveBackground)
    {
        DeleteObject(m_hbrActiveBackground);
        m_hbrActiveBackground = NULL;
    }

    if(m_hpnWaveform)
    {
        DeleteObject(m_hpnWaveform);
        m_hpnWaveform = NULL;
    }

    //
    // Create the background brushes
    //

    if(!(m_hbrActiveBackground = CreateSolidBrush(m_colors.rgbBackground)))
    {
        DPF_ERROR("Failed to create active background brush");
        fSuccess = FALSE;
    }

    if(fSuccess)
    {
        if(!(m_hbrInactiveBackground = CreateHatchBrush(HS_DIAGCROSS, m_colors.rgbInactiveBackground)))
        {
            DPF_ERROR("Failed to create inactive background brush");
            fSuccess = FALSE;
        }
    }

    //
    // Create the foreground pen
    //

    if(fSuccess)
    {
        if(!(m_hpnWaveform = CreatePen(PS_SOLID, 1, m_colors.rgbWaveform)))
        {
            DPF_ERROR("Failed to create waveform pen");
            fSuccess = FALSE;
        }
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  DrawBackground
 *
 *  Description:
 *      Draws the waveform background.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::DrawBackground"

BOOL
CWaveformRenderer::DrawBackground
(
    void
)
{
    const RECT              rc  = { 0, 0, m_nWidth, m_nHeight };
    
    ASSERT(m_hdcDraw);
    ASSERT(m_hbrActiveBackground);

    //
    // Paint the whole background as active.  The waveform render function
    // will repaint the inactive region for us
    //

    return FillRect(m_hdcDraw, &rc, m_hbrActiveBackground);
}


/****************************************************************************
 *
 *  DrawWaveform
 *
 *  Description:
 *      Draws the waveform.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::DrawWaveform"

BOOL
CWaveformRenderer::DrawWaveform
(
    void
)
{
    CTempSelectObject       SelectPen;
    BOOL                    fSuccess;

    ASSERT(m_hdcDraw);
    ASSERT(m_hpnWaveform);

    if(fSuccess = SelectPen.SelectObject(m_hdcDraw, m_hpnWaveform))
    {
        switch(m_wfxFormat.wBitsPerSample)
        {
            case 8:
                RenderWaveform((const unsigned char *)m_pvWaveData, 0x100, FALSE);
                break;

            case 16:
                RenderWaveform((const short *)m_pvWaveData, 0x10000, TRUE);
                break;

            default:
                DPF_ERROR("Unsupported bit resolution");
                fSuccess = FALSE;
                break;
        }
    }

    return fSuccess;
}


/****************************************************************************
 *
 *  RenderWaveform
 *
 *  Description:
 *      Draws the waveform in a DC.  This function assumes all GDI objects
 *      have already been set up and simply draws the waveform.
 *
 *  Arguments:
 *      const SampleType * [in]: wave data.  This is only here to work around
 *                               a bug in template functions.
 *      UINT [in]: value range of a sample.
 *      BOOL [in]: TRUE if the samples are signed.
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::RenderWaveform"

template <class SampleType>
void
CWaveformRenderer::RenderWaveform
(
    const SampleType *      pWaveData,
    UINT                    nWaveRange,
    BOOL                    fSigned
)
{
    static const int        nBaseX              = 0;
    const int               nBaseY              = m_nHeight / 2;
    const int               nHalfWaveRange      = nWaveRange / 2;
    const float             flVerticalScale     = (float)m_nHeight / (float)nWaveRange;
    const int               nSampleIncrement    = (m_nScale / m_nResolution) + 1;
    const int               cbSample            = sizeof(*pWaveData) * m_wfxFormat.nChannels;
    const int               nSampleOffset       = m_cbHorizOffset / cbSample;
    int                     cSamples            = m_cbWaveData / cbSample;
    int                     nPerPixelCount      = 0;
    long                    lSample;
    RECT                    rcUnused;
    int                     x, y;
    int                     i;

    if(!m_nWidth || !m_nHeight || !m_nScale)
    {
        return;
    }

    //
    // Reset the pen position to the 0,0 location
    //

    x = nBaseX;
    y = nBaseY;
    
    MoveToEx(m_hdcDraw, x, y, NULL);

    //
    // Offset the buffer and sample count
    //

    pWaveData += nSampleOffset * m_wfxFormat.nChannels;
    cSamples -= nSampleOffset;

    //
    // Enter the drawing loop
    //

    while((cSamples > 0) && (x < (int)m_nWidth))
    {
        //
        // Merge all channels into one averaged sample
        //

        if(fSigned)
        {
            for(i = 0, lSample = 0; i < m_wfxFormat.nChannels; i++)
            {
                lSample += (LONG)*pWaveData++;
            }
        }
        else
        {
            for(i = 0, lSample = 0; i < m_wfxFormat.nChannels; i++)
            {
                lSample += (long)*pWaveData++;
                lSample -= nHalfWaveRange;
            }
        }

        lSample /= m_wfxFormat.nChannels;
        
        ASSERT(lSample >= -nHalfWaveRange);
        ASSERT(lSample < nHalfWaveRange);

        //
        // Convert the sample's scale to our window height
        //

        lSample = (long)((float)lSample * flVerticalScale);

        //
        // Convert the sample value to a positive
        //

        y = nBaseY + lSample;

        //
        // Draw a line from the current position to the sample's position on
        // the graph
        //

        ASSERT(y >= 0);
        ASSERT(y < (int)m_nHeight);
        
        LineTo(m_hdcDraw, x, y);

        //
        // Skip ahead a few samples based on the resolution.  This is here to
        // prevent us from processing every single sample when the scaling 
        // factor is high.
        //

        cSamples -= nSampleIncrement;
        pWaveData += (nSampleIncrement - 1) * m_wfxFormat.nChannels;

        //
        // Increment the per-pixel sample counter.  When this number exceeds
        // the scaling factor, it's time to move the x-coordinate.
        //

        nPerPixelCount += nSampleIncrement;

        if(nPerPixelCount >= (int)m_nScale)
        {
            x++;

            nPerPixelCount = 0;
        }
    }

    //
    // Paint the remaining unused region
    //

    if(x < (int)m_nWidth)
    {
        rcUnused.left = x;
        rcUnused.top = 0;
        rcUnused.right = m_nWidth;
        rcUnused.bottom = m_nHeight;
    
        FillRect(m_hdcDraw, &rcUnused, m_hbrInactiveBackground);
    }
}


/****************************************************************************
 *
 *  DisplayBitmap
 *
 *  Description:
 *      Displays the bitmap we've drawn on the window.
 *
 *  Arguments:
 *      HDC [in]: display DC.
 *      LPCRECT [in]: update region.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformRenderer::DisplayBitmap"

BOOL
CWaveformRenderer::DisplayBitmap
(
    HDC                     hdcDisplay,
    LPCRECT                 prcUpdate
)
{
    int                     x;
    int                     y;
    int                     nWidth;
    int                     nHeight;

    ASSERT(hdcDisplay);

    if(!prcUpdate)
    {
        prcUpdate = &m_rcDisplay;
    }

    x = prcUpdate->left - m_rcDisplay.left;
    y = prcUpdate->top - m_rcDisplay.top;

    nWidth = prcUpdate->right - x;
    nHeight = prcUpdate->bottom - y;

    return BitBlt(hdcDisplay, prcUpdate->left, prcUpdate->top, nWidth, nHeight, m_hdcDraw, x, y, SRCCOPY);
}


/****************************************************************************
 *
 *  CWaveformControl
 *
 *  Description:
 *      Object constructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::CWaveformControl"

CWaveformControl::CWaveformControl
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CWaveformControl
 *
 *  Description:
 *      Object destructor.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::~CWaveformControl"

CWaveformControl::~CWaveformControl
(
    void
)
{
}


/****************************************************************************
 *
 *  Create
 *
 *  Description:
 *      Creates the window.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *      DWORD [in]: extended window style.
 *      int [in]: x-coordinate.
 *      int [in]: y-coordinate.
 *      UINT [in]: width.
 *      UINT [in]: height.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::Create"

BOOL
CWaveformControl::Create
(
    CWindow *               pParent,
    DWORD                   dwExStyle,
    int                     x, 
    int                     y, 
    UINT                    nWidth, 
    UINT                    nHeight,
    UINT                    nControlId
)
{
    static const DWORD      dwGeneralStyle  = WS_HSCROLL | WS_VSCROLL | WS_TABSTOP;
    static const DWORD      dwChildStyle    = WS_CHILD;
    static const DWORD      dwParentStyle   = WS_OVERLAPPEDWINDOW;
    WNDCLASSEX              wc              = { 0 };
    DWORD                   dwStyle;

    dwStyle = dwGeneralStyle;

    if(pParent)
    {
        dwStyle |= dwChildStyle;
    }
    else
    {
        dwStyle |= dwParentStyle;
        nControlId = 0;
    }

    //
    // Register the window class
    //    

    wc.cbSize = sizeof(wc);
    wc.style = CS_DBLCLKS | CS_OWNDC;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = g_hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = m_pszClassName;

    RegisterClassEx(&wc);

    //
    // Create the window
    //

    return CWindow::Create(pParent, m_pszClassName, NULL, dwExStyle, dwStyle, x, y, nWidth, nHeight, nControlId);
}


/****************************************************************************
 *
 *  SetScale
 *
 *  Description:
 *      Sets the scaling factor used to render the wave data.
 *
 *  Arguments:
 *      UINT [in]: scale.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::SetScale"

BOOL
CWaveformControl::SetScale
(
    UINT                    nScale
)
{
    //
    // Hand off to the renderer
    //

    if(!CWaveformRenderer::SetScale(nScale))
    {
        return FALSE;
    }

    //
    // Reset the scrollbars
    //

    SetupOffsetScrollbar();

    return TRUE;
}


/****************************************************************************
 *
 *  SetWaveData
 *
 *  Description:
 *      Sets the wave data used by the renderer.
 *
 *  Arguments:
 *      LPCWAVEFORMDATA [in]: wave data.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::SetWaveData"

BOOL
CWaveformControl::SetWaveData
(
    LPCWAVEFORMDATA         pWaveData
)
{
    if(!CWaveformRenderer::SetWaveData(pWaveData))
    {
        return FALSE;
    }

    SetupOffsetScrollbar();

    return TRUE;
}


/****************************************************************************
 *
 *  SetOffset
 *
 *  Description:
 *      Sets the bitmap Offset.
 *
 *  Arguments:
 *      UINT [in]: bitmap width.
 *      UINT [in]: bitmap height.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::SetOffset"

BOOL
CWaveformControl::SetOffset
(
    UINT                    cbOffset
)
{
    if(!CWaveformRenderer::SetOffset(cbOffset))
    {
        return FALSE;
    }

    SetupOffsetScrollbar();

    return TRUE;
}


/****************************************************************************
 *
 *  SetupOffsetScrollbar
 *
 *  Description:
 *      Sets up the offset scrollbar.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::SetupOffsetScrollbar"

void
CWaveformControl::SetupOffsetScrollbar
(
    void
)
{
    const int               cbSample        = m_wfxFormat.nChannels * m_wfxFormat.wBitsPerSample / 8;
    SCROLLINFO              si              = { 0 };
    int                     nSectionCount;
    int                     nSectionOffset;

    //
    // How many "pages" are there currently?  A page is defined as the amount
    // of wave data that can currently be displayed in the window.
    //

    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;

    m_nPageSize = m_nWidth * cbSample * m_nScale;
    m_nSectionSize = m_nPageSize / m_nSectionDivisor;
    
    if(m_nSectionSize)
    {
        nSectionCount = (m_cbWaveData + m_nSectionSize - 1) / m_nSectionSize;
        nSectionOffset = m_cbHorizOffset / m_nSectionSize;

        //
        // Fix the current offset so that no more than one unused section is
        // visible
        //

        if(nSectionCount <= (int)m_nSectionDivisor)
        {
            nSectionOffset = 0;

            CWaveformRenderer::SetOffset(0);
        }
        else if(nSectionOffset > nSectionCount - (int)m_nSectionDivisor)
        {
            nSectionOffset = m_cbHorizOffset / m_nSectionSize;

            CWaveformRenderer::SetOffset((nSectionCount - (int)m_nSectionDivisor) * m_nSectionSize);
        }

        si.nMin = 0;
        si.nMax = nSectionCount - 1;
        si.nPage = m_nSectionDivisor;
        si.nPos = nSectionOffset;
    }

    SetScrollInfo(m_hWnd, SB_HORZ, &si, TRUE);
}


/****************************************************************************
 *
 *  SetupScaleScrollbar
 *
 *  Description:
 *      Sets up the scaling scrollbar.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      (void)
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::SetupScaleScrollbar"

void
CWaveformControl::SetupScaleScrollbar
(
    void
)
{
    SCROLLINFO              si  = { 0 };

    si.cbSize = sizeof(si);
    si.fMask = SIF_PAGE | SIF_POS | SIF_RANGE;
    si.nMin = 0;
    si.nMax = 15;
    si.nPage = 4;
    
    for(si.nPos = 0; si.nPos < 31; si.nPos++)
    {
        if(m_nScale & (1UL << si.nPos))
        {
            break;
        }
    }

    SetScrollInfo(m_hWnd, SB_VERT, &si, TRUE);
}


/****************************************************************************
 *
 *  OnCreate
 *
 *  Description:
 *      Handles WM_CREATE messages.
 *
 *  Arguments:
 *      LPVOID [in]: window creation context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::OnCreate"

BOOL
CWaveformControl::OnCreate
(
    LPVOID                  pvContext,
    LRESULT *               plResult
)
{
    if(CWindow::OnCreate(pvContext, plResult))
    {
        return TRUE;
    }
    
    //
    // Initialize the renderer
    //

    if(!CWaveformRenderer::Initialize(m_hWnd))
    {
        *plResult = FALSE;
        return TRUE;
    }

    //
    // Setup the scrollbars
    //

    SetupScaleScrollbar();
    SetupOffsetScrollbar();

    return FALSE;
}


/****************************************************************************
 *
 *  OnSize
 *
 *  Description:
 *      Handles WM_SIZE messages.
 *
 *  Arguments:
 *      UINT [in]: resize type.
 *      UINT [in]: client width.
 *      UINT [in]: client height.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::OnSize"

BOOL
CWaveformControl::OnSize
(
    UINT                    nType, 
    UINT                    nWidth, 
    UINT                    nHeight, 
    LRESULT *               plResult
)
{
    const RECT              rcDisplay   = { 0, 0, nWidth, nHeight };
    
    if(CWindow::OnSize(nType, nWidth, nHeight, plResult))
    {
        return TRUE;
    }
    
    //
    // Update the renderer
    //

    CWaveformRenderer::SetRect(&rcDisplay);

    //
    // Update the offset scrollbar
    //

    SetupOffsetScrollbar();

    //
    // Force a repaint
    //

    InvalidateRect(m_hWnd, NULL, FALSE);

    return FALSE;
}


/****************************************************************************
 *
 *  OnEraseBackground
 *
 *  Description:
 *      Handles WM_ERASEBKGND messages.
 *
 *  Arguments:
 *      HDC [in]: device context.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::OnEraseBackground"

BOOL
CWaveformControl::OnEraseBackground
(
    HDC                     hdc,
    LRESULT *               plResult
)
{
    if(!m_hdcDraw)
    {
        return FALSE;
    }
    
    *plResult = 0;

    return TRUE;
}


/****************************************************************************
 *
 *  OnPaint
 *
 *  Description:
 *      Handles WM_PAINT messages.
 *
 *  Arguments:
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::OnPaint"

BOOL
CWaveformControl::OnPaint
(
    LRESULT *               plResult
)
{
    PAINTSTRUCT             ps;

    if(!m_hdcDraw)
    {
        return FALSE;
    }
    
    //
    // Repaint the window
    //

    BeginPaint(m_hWnd, &ps);

    CWaveformRenderer::Render(ps.hdc, &ps.rcPaint);

    EndPaint(m_hWnd, &ps);
    
    *plResult = 0;

    return TRUE;
}


/****************************************************************************
 *
 *  OnScroll
 *
 *  Description:
 *      Handles WM_H/VSCROLL messages.
 *
 *  Arguments:
 *      UINT [in]: scrollbar type (SB_HORZ/SB_VERT).
 *      HWND [in]: scrollbar control window handle.
 *      INT [in]: new scrollbar position.
 *      LRESULT * [out]: message result code.
 *
 *  Returns:  
 *      BOOL: FALSE to allow the message to be passed to the default handler.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CWaveformControl::OnScroll"

BOOL
CWaveformControl::OnScroll
(
    UINT                    nScrollBarType, 
    HWND                    hWndScrollBar, 
    INT                     nPosition, 
    LRESULT *               plResult
)
{
    if(CWindow::OnScroll(nScrollBarType, hWndScrollBar, nPosition, plResult))
    {
        return TRUE;
    }
    
    //
    // If the vertical scrollbar changed, update the scale.  If it was the
    // horizontal, update the offset.
    //

    if(SB_VERT == nScrollBarType)
    {
        SetScale(1UL << nPosition);
        SetupOffsetScrollbar();
    }
    else if(SB_HORZ == nScrollBarType)
    {
        SetOffset(nPosition * m_nSectionSize);
    }
    
    //
    // Force a repaint
    //

    InvalidateRect(m_hWnd, NULL, FALSE);

    return FALSE;
}


