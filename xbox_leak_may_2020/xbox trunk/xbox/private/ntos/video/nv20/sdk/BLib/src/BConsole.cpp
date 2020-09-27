#include <windows.h>
#include <stdio.h>
#pragma hdrstop
#include "..\inc\blib.h"

//////////////////////////////////////////////////////////////////////////////
// BScreen::close
//
LRESULT BScreen::queueMessage
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    if (m_pfnHandler)
    {
        if (m_pfnHandler(m_pMHContext,uMsg,wParam,lParam) == consumed)
        {
            return 0; // done
        }
        // we have to buffer this event since the handler refused to handle it right away.
    }

    EnterCriticalSection (&m_csThread);
    MESSAGE msg;
    msg.hWnd     = m_hWindow;
    msg.uMessage = uMsg;
    msg.wParam   = wParam;
    msg.lParam   = lParam;
    m_listMessage.addTail (msg);
    LeaveCriticalSection (&m_csThread);

    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::mustClose
//
LRESULT BScreen::mustClose
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    PostQuitMessage (0);
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::setFocus
//
LRESULT BScreen::setFocus
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    updateCaret();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::killFocus
//
LRESULT BScreen::killFocus
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    updateCaret();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::getMinMaxInfo
//
LRESULT BScreen::getMinMaxInfo
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    int iMaxX = m_sizeBuffer.cx * m_sizeCell.cx
              + GetSystemMetrics(SM_CXSIZEFRAME) * 2
              + GetSystemMetrics(SM_CXVSCROLL);
    int iMaxY = m_sizeBuffer.cy * m_sizeCell.cy
              + GetSystemMetrics(SM_CYSIZEFRAME) * 2
              + GetSystemMetrics(SM_CYHSCROLL)
              + GetSystemMetrics(SM_CYCAPTION);

    LPMINMAXINFO lpInfo      = (LPMINMAXINFO)lParam;
    lpInfo->ptMaxSize.x      = min(iMaxX,GetSystemMetrics(SM_CXSCREEN));
    lpInfo->ptMaxSize.y      = min(iMaxY,GetSystemMetrics(SM_CYSCREEN));
    lpInfo->ptMaxTrackSize   = lpInfo->ptMaxSize;
    lpInfo->ptMinTrackSize.x = 5 * m_sizeCell.cx;
    lpInfo->ptMinTrackSize.y = 5 * m_sizeCell.cy;
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::hScroll
//
LRESULT BScreen::hScroll
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    // read info
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo (m_hWindow,SB_HORZ,&si);

    // act
    int iMax    = m_sizeBuffer.cx * m_sizeCell.cx;
    int iOldPos = m_ptTopLeft.x;
    switch (wParam & 0xffff)
    {
        case SB_BOTTOM:        m_ptTopLeft.x  = iMax;          break;
        case SB_TOP:           m_ptTopLeft.x  = 0;             break;
        case SB_LINEUP:        m_ptTopLeft.x -= m_sizeCell.cx; break;
        case SB_LINEDOWN:      m_ptTopLeft.x += m_sizeCell.cx; break;
        case SB_PAGEUP:        m_ptTopLeft.x -= si.nPage;      break;
        case SB_PAGEDOWN:      m_ptTopLeft.x += si.nPage;      break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION: m_ptTopLeft.x  = wParam >> 16;  break;
    }
    if (m_ptTopLeft.x < 0)    m_ptTopLeft.x = 0;
    if (m_ptTopLeft.x > iMax) m_ptTopLeft.x = iMax;

    // set
    if (iOldPos != m_ptTopLeft.x)
    {
        InvalidateRect (m_hWindow,NULL,TRUE);
    }

    // done
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::vScroll
//
LRESULT BScreen::vScroll
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    // read info
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo (m_hWindow,SB_VERT,&si);

    // act
    int iMax    = m_sizeBuffer.cy * m_sizeCell.cy;
    int iOldPos = m_ptTopLeft.y;
    switch (wParam & 0xffff)
    {
        case SB_BOTTOM:        m_ptTopLeft.y  = iMax;          break;
        case SB_TOP:           m_ptTopLeft.y  = 0;             break;
        case SB_LINEUP:        m_ptTopLeft.y -= m_sizeCell.cy; break;
        case SB_LINEDOWN:      m_ptTopLeft.y += m_sizeCell.cy; break;
        case SB_PAGEUP:        m_ptTopLeft.y -= si.nPage;      break;
        case SB_PAGEDOWN:      m_ptTopLeft.y += si.nPage;      break;
        case SB_THUMBTRACK:
        case SB_THUMBPOSITION: m_ptTopLeft.y  = wParam >> 16;  break;
    }
    if (m_ptTopLeft.y < 0)    m_ptTopLeft.y = 0;
    if (m_ptTopLeft.y > iMax) m_ptTopLeft.y = iMax;

    // set
    if (iOldPos != m_ptTopLeft.y)
    {
        InvalidateRect (m_hWindow,NULL,TRUE);
    }

    // done
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::mouseWheel
//
LRESULT BScreen::mouseWheel
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    // read info
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_ALL;
    GetScrollInfo (m_hWindow,SB_VERT,&si);

    // act
    int iMax       = m_sizeBuffer.cy * m_sizeCell.cy;
    int iOldPos    = m_ptTopLeft.y;
    m_ptTopLeft.y -= int((short int)(wParam >> 14)) / m_sizeCell.cy;
    if (m_ptTopLeft.y < 0)    m_ptTopLeft.y = 0;
    if (m_ptTopLeft.y > iMax) m_ptTopLeft.y = iMax;

    // set
    if (iOldPos != m_ptTopLeft.y)
    {
        InvalidateRect (m_hWindow,NULL,TRUE);
    }

    // done
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::paint
//
LRESULT BScreen::paint
(
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    //
    // prepare to draw
    //
    getMutex();
    caretOff();

    //
    // init
    //
    RECT rectDirty;
    GetUpdateRect (m_hWindow,&rectDirty,FALSE);
    RECT rectClient;
    GetClientRect (m_hWindow,&rectClient);

    //
    // condition buffer such that we do not draw over allowed limits
    //
    int iMaxX = m_sizeBuffer.cx * m_sizeCell.cx;
    int iMaxY = m_sizeBuffer.cy * m_sizeCell.cy;
    if (int(m_ptTopLeft.x + rectClient.right - rectClient.left) > iMaxX)
    {
        m_ptTopLeft.x = iMaxX - (rectClient.right - rectClient.left);
    }
    if (int(m_ptTopLeft.y + rectClient.bottom - rectClient.top) > iMaxY)
    {
        m_ptTopLeft.y = iMaxY - (rectClient.bottom - rectClient.top);
    }

    //
    // update scroll bars
    //    SB has a bug where thumb is n-1 for max-min set to n
    //
    SCROLLINFO si;
    si.cbSize = sizeof(si);
    si.fMask  = SIF_PAGE | SIF_RANGE | SIF_POS;
    si.nMin   = 0;
    si.nMax   = iMaxX;
    si.nPage  = rectClient.right - rectClient.left;
    si.nPos   = m_ptTopLeft.x;
    SetScrollInfo (m_hWindow,SB_HORZ,&si,TRUE);
    si.nMin   = 0;
    si.nMax   = iMaxY;
    si.nPage  = rectClient.bottom - rectClient.top; 
    si.nPos   = m_ptTopLeft.y;
    SetScrollInfo (m_hWindow,SB_VERT,&si,TRUE);


    //
    // start drawing
    //
    PAINTSTRUCT ps;
    BeginPaint (m_hWindow,&ps);
    assert (m_hbmpFontMap || m_hFont); // one of these MUST be defined

    //
    // select default drawing objects
    //
    HFONT hOldFont = NULL;
    HDC   hFontMap = NULL;
    if (m_hbmpFontMap)
    {
        hFontMap = CreateCompatibleDC(ps.hdc);
        SelectObject (hFontMap,m_hbmpFontMap);
    }
    else
    {
        hOldFont = (HFONT)SelectObject(ps.hdc,m_hFont);
    }

    //
    // draw visible text (will fail if window exceeds buffer)
    //
    char  *pszAccBuffer  = new char[m_sizeBuffer.cx];
    POINT  ptTopLeftChar = { m_ptTopLeft.x / m_sizeCell.cx, m_ptTopLeft.y / m_sizeCell.cy }; // topleft char in buffer
    POINT  ptTopLeftCell = { -(m_ptTopLeft.x % m_sizeCell.cx), -(m_ptTopLeft.y % m_sizeCell.cy) };
    POINT  ptChar;   // current cell in buffer
    RECT   rectCell; // current cell on screen that corresponds to ptChar;
    rectCell.top    = ptTopLeftCell.y;
    rectCell.bottom = rectCell.top + m_sizeCell.cy;
    ptChar.y        = ptTopLeftChar.y;
    for (DWORD y = m_ptTopLeft.y; rectCell.top < rectClient.bottom; y++)
    {
        // trivial reject full lines if possible
        RECT rect,rect2;
        rect.left   = rectClient.left;
        rect.right  = rectClient.right;
        rect.top    = rectCell.top;
        rect.bottom = rectCell.bottom;
        if (IntersectRect(&rect2,&rect,&rectDirty)) // line in region?
        {
            // init span optimizer
            DWORD  dwColor = m_pawScreen[ptChar.y * m_sizeBuffer.cx + ptTopLeftChar.x] & 0xff00;
            char  *pch     = pszAccBuffer;
            LONG   lStartX = ptTopLeftCell.x;

            // for all x
            rectCell.left  = ptTopLeftCell.x;
            rectCell.right = rectCell.left + m_sizeCell.cx;
            ptChar.x       = ptTopLeftChar.x;
            for (DWORD x = m_ptTopLeft.x; rectCell.left < rectClient.right; x++)
            {
                // collect text
                DWORD dwChar = m_pawScreen[ptChar.y * m_sizeBuffer.cx + ptChar.x];
                if ((dwChar & 0xff00) == dwColor)
                {
                    // color match, keep
                    *pch = (char)dwChar;
                    pch++;
                }
                else
                {
                    // color change - dump string and start new
                    if (pch != pszAccBuffer)
                    {
                        POINT pt = { lStartX, rectCell.top };
                        drawText (ps.hdc,
                                  hFontMap,
                                  &pt,
                                  pszAccBuffer,
                                  pch - pszAccBuffer,
                                  dwColor >> 8);
                    }

                    // start new
                    pch     = pszAccBuffer;
                    lStartX = rectCell.left;
                    dwColor = dwChar & 0xff00;

                    // add first
                    *pch = (char)dwChar;
                    pch++;
                }

                // next x
                rectCell.left  += m_sizeCell.cx;
                rectCell.right += m_sizeCell.cx;
                ptChar.x       ++;
            }
            // draw last, if any
            if (pch != pszAccBuffer)
            {
                POINT pt = { lStartX, rectCell.top };
                drawText (ps.hdc,
                          hFontMap,
                          &pt,
                          pszAccBuffer,
                          pch - pszAccBuffer,
                          dwColor >> 8);
            }
        }

        // next y
        rectCell.top    += m_sizeCell.cy;
        rectCell.bottom += m_sizeCell.cy;
        ptChar.y        ++;
    }
    delete[] pszAccBuffer;

    //
    // reset drawing objects
    //
    if (m_hbmpFontMap)
    {
        DeleteDC (hFontMap);
    }
    else
    {
        SelectObject (ps.hdc,hOldFont);
    }

    //
    // end drawing
    //
    EndPaint (m_hWindow,&ps);
    caretOn();
    releaseMutex();
    return 0;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::windowProc
//
LRESULT CALLBACK __BScreen_windowProc
(
    HWND   hWnd,
    UINT   uMsg,
    WPARAM wParam,
    LPARAM lParam
)
{
    BScreen *pThis = (BScreen*)GetWindowLong(hWnd,0);
    if (!pThis)
    {
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }
    
    return pThis->windowProc(hWnd,uMsg,wParam,lParam);
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::createFont
//
int BScreen::createFont
(
    void
)
{
    if (!m_hbmpFontMap) // user font, do not create GDI font
    {
        //
        // create font
        //
        m_hFont = CreateFont(m_sizeFont.cy,0,0,0,0,0,0,0,0,0,0,0,0,m_strFontName);
        if (!m_hFont)
        {
            destroyScreen();
            EnterCriticalSection (&m_csThread);
            m_enumThreadStatus = threadAbnormalTermination;
            LeaveCriticalSection (&m_csThread);
            return 0;
        }

        //
        // compute cell sizes
        //
        HDC hdc = GetDC(NULL);
        HFONT hOldFont = (HFONT)SelectObject(hdc,m_hFont);
        SIZE size;
        GetTextExtentPoint32 (hdc,"MWqgchiL",8,&size);
        m_sizeFont.cx = size.cx / 8;
        m_sizeFont.cy = size.cy;
        SelectObject (hdc,hOldFont);
        ReleaseDC (NULL,hdc);

        //
        // create spacing array
        //
        delete[] m_paiSpacing;
        m_paiSpacing = new INT[m_sizeBuffer.cx + 1];
        fillDWord (m_paiSpacing,m_sizeCell.cx,m_sizeBuffer.cx + 1);
    }

    m_sizeCell.cx = m_sizeFont.cx + 0;
    m_sizeCell.cy = m_sizeFont.cy + 0;

    // done
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::destroyFont
//
void BScreen::destroyFont
(
    void
)
{
    delete[] m_paiSpacing;
    m_paiSpacing = NULL;

    if (m_hFont)
    {
        DeleteObject (m_hFont);
        m_hFont = NULL;
    }
    if (m_hbmpFontMap)
    {
        DeleteObject (m_hbmpFontMap);
        m_hbmpFontMap = NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::createScreen
//
int BScreen::createScreen
(
    void
)
{
    //
    // setup font
    //
    if (!createFont()) return 0;

    //
    // create buffer
    //
    m_pawScreen = new WORD[m_sizeBuffer.cy * m_sizeBuffer.cx];
    if (!m_pawScreen)
    {
        destroyScreen();
        EnterCriticalSection (&m_csThread);
        m_enumThreadStatus = threadAbnormalTermination;
        LeaveCriticalSection (&m_csThread);
        return 0;
    }
    fillWord (m_pawScreen,0x0820,m_sizeBuffer.cy * m_sizeBuffer.cx);

    //
    // register class
    //
    WNDCLASS wndClass;
    memset (&wndClass,0,sizeof(wndClass));
    wndClass.cbWndExtra    = 4;
    wndClass.hCursor       = LoadCursor(NULL,IDC_ARROW);
    wndClass.hIcon         = LoadIcon(NULL,IDI_APPLICATION);
    wndClass.hInstance     = m_hInstance;
    wndClass.lpfnWndProc   = __BScreen_windowProc;
    wndClass.lpszClassName = "BScreenClass";
    wndClass.style         = CS_HREDRAW | CS_VREDRAW;
    if (!RegisterClass(&wndClass))
    {
        destroyScreen();
        EnterCriticalSection (&m_csThread);
        m_enumThreadStatus = threadAbnormalTermination;
        LeaveCriticalSection (&m_csThread);
        return 0;
    }

    //
    // create window
    //
    m_hWindow = CreateWindow("BScreenClass",
                             "BScreen::Caption",
                             WS_HSCROLL | WS_VSCROLL | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU,
                             CW_USEDEFAULT,
                             CW_USEDEFAULT,
                             0,
                             0,
                             NULL,
                             NULL,
                             m_hInstance,
                             this);
    if (!m_hWindow)
    {
        destroyScreen();
        EnterCriticalSection (&m_csThread);
        m_enumThreadStatus = threadAbnormalTermination;
        LeaveCriticalSection (&m_csThread);
        return 0;
    }
    SetWindowLong (m_hWindow,0,(LONG)this);
    SetWindowPos (m_hWindow,NULL,0,0,GetSystemMetrics(SM_CXSCREEN) / 2,GetSystemMetrics(SM_CYSCREEN) / 2,SWP_NOZORDER | SWP_NOMOVE); // force window as large as possible, but using minmax info
    ShowWindow (m_hWindow,SW_NORMAL);
    UpdateWindow (m_hWindow);

    // on win 98 this is needed otherwise nothing show up - not sure why
    destroyFont();
    createFont();

    //
    // message pump
    //
    EnterCriticalSection (&m_csThread);
    m_enumThreadStatus = threadRunning;
    LeaveCriticalSection (&m_csThread);
    
    MSG msg;
    for (;;)
    {
        if (PeekMessage(&msg,NULL,0,0,PM_NOREMOVE))
        {
            if (GetMessage(&msg,NULL,0,0))
            {
                TranslateMessage (&msg);
                DispatchMessage (&msg);
            }
            else
            {
                break;
            }
        }
        else
        {
            Sleep (10);
        }
    }

    //
    // done
    //
    destroyScreen();
    EnterCriticalSection (&m_csThread);
    m_enumThreadStatus = threadStopped;
    LeaveCriticalSection (&m_csThread);
    return 1;
}

DWORD WINAPI __BScreen_createScreen
(
    LPVOID lpContext
)
{
    return ((BScreen*)lpContext)->createScreen();
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::destroyScreen
//
void BScreen::destroyScreen
(
    void
)
{
    //
    // clean up
    //
    delete[] m_pawScreen;
    m_pawScreen = NULL;

    destroyFont();

    DestroyCaret();
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::drawText
//
void BScreen::drawText
(
          HDC       hdc,
          HDC       hFontMap,
    const POINT    *pt,
    const char     *psz,
          DWORD     dwCharCount,
          DWORD     dwColor
)
{
    // draw text/user
    if (m_hbmpFontMap)
    {
        if (!dwCharCount) return;
        POINT ptd = *pt;
        // for all chars
        for (; dwCharCount; psz++, dwCharCount--)
        {
            POINT pts;
            DWORD dwChar = (dwColor << 8) | DWORD((unsigned char)psz[0]);
            pts.x = (dwChar % m_sizeCellOrganization.cx) * m_sizeFont.cx;
            pts.y = (dwChar / m_sizeCellOrganization.cx) * m_sizeFont.cy;
            // draw cell
            BitBlt (hdc,ptd.x,ptd.y,m_sizeFont.cx,m_sizeFont.cy,
                    hFontMap,pts.x,pts.y,
                    SRCCOPY);
            // next
            ptd.x += m_sizeFont.cx;
        }
    }
    else
    {
        // compute bounding rect
        RECT rect = { pt->x, pt->y, pt->x + dwCharCount * m_sizeCell.cx, pt->y + m_sizeCell.cy };
        // set up colors
        SetTextColor (hdc,m_acolPalette[(dwColor >> 0) & 15]);
        SetBkColor   (hdc,m_acolPalette[(dwColor >> 4) & 15]);
        // draw
        ExtTextOut (hdc,rect.left,rect.top,ETO_OPAQUE | ETO_CLIPPED,&rect,psz,dwCharCount,m_paiSpacing);
    }
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::caretOff
//
void BScreen::caretOff
(
    void
)
{
    m_dwFlags &= ~flagCaretOn;
    updateCaret();
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::caretOn
//
void BScreen::caretOn
(
    void
)
{
    m_dwFlags |= flagCaretOn;
    updateCaret();
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::updateCaret
//
void BScreen::updateCaret
(
    BOOL bForceRecreate
)
{
    //
    // show
    //
    if (!bForceRecreate &&
        (GetFocus() == m_hWindow))
    {
        if (m_dwFlags & flagCaretEnabled)
        {
            if (!(m_dwFlags & flagCaretCreated))
            {
                CreateCaret (m_hWindow,NULL,m_iCaretWidth,m_sizeCell.cy);
                m_dwFlags |= flagCaretCreated;
            }
            int x = m_ptCursor.x * m_sizeCell.cx - m_ptTopLeft.x;
            int y = m_ptCursor.y * m_sizeCell.cy - m_ptTopLeft.y;
            SetCaretPos (x,y);
            ShowCaret (m_hWindow);
            m_dwFlags |= flagCaretOn;
            return;
        }
    }

    //
    // hide
    //
    if (m_dwFlags & flagCaretCreated)
    {
        if (m_dwFlags & flagCaretOn)
        {
            HideCaret (m_hWindow);
        }
        DestroyCaret();
        m_dwFlags &= ~(flagCaretOn | flagCaretCreated);
    }

    if (bForceRecreate)
    {
        updateCaret(); // recreate
        InvalidateRect (m_hWindow,NULL,FALSE); // for some reason the caret needs a paint job when recreated - win32 bug
    }
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::scrollToCaret
//
void BScreen::scrollToCaret
(
    void
)
{
    assert (m_ptCursor.x >= 0);
    assert (m_ptCursor.x < m_sizeBuffer.cx);
    assert (m_ptCursor.y >= 0);
    assert (m_ptCursor.y < m_sizeBuffer.cy);

    BOOL b = FALSE;
    int  x[2] = { m_ptCursor.x * m_sizeCell.cx - m_ptTopLeft.x, x[0] + m_sizeCell.cx };
    int  y[2] = { m_ptCursor.y * m_sizeCell.cy - m_ptTopLeft.y, y[0] + m_sizeCell.cy };
    RECT r;
    GetClientRect (m_hWindow,&r);

    if (x[0] < r.left)
    {
        m_ptTopLeft.x += x[0];
        b = TRUE;
    }
    if (y[0] < r.top)
    {
        m_ptTopLeft.y += y[0];
        b = TRUE;
    }
    if (x[1] >= r.right)
    {
        m_ptTopLeft.x += x[1] - r.right + m_sizeCell.cx;
        b = TRUE;
    }
    if (y[1] >= r.bottom)
    {
        m_ptTopLeft.y += y[1] - r.bottom  + m_sizeCell.cy;
        b = TRUE;
    }
    if (b)
    {
        InvalidateRect (m_hWindow,NULL,TRUE);
    }
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::scrollBuffer
//
void BScreen::scrollBuffer
(
    void
)
{
    memmove (m_pawScreen,m_pawScreen + m_sizeBuffer.cx,m_sizeBuffer.cx * (m_sizeBuffer.cy - 1) * sizeof(WORD));
    fillWord (m_pawScreen + m_sizeBuffer.cx * (m_sizeBuffer.cy - 1),(m_dwCurrentColor << 8) | 0x20,m_sizeBuffer.cx);
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::getMessage
//
int BScreen::getMessage
(
    MESSAGE *pMsg
)
{
    EnterCriticalSection (&m_csThread);
    if (m_listMessage.isEmpty()) 
    {
        LeaveCriticalSection (&m_csThread);
        return 0;
    }
    *pMsg = m_listMessage.getHead();
    m_listMessage.removeHead();
    LeaveCriticalSection (&m_csThread);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::setFont
//
int BScreen::setFont
(
    const BString& strFont,
    int            iRecommendedFontHeight
)
{
    EnterCriticalSection (&m_csThread);
    destroyFont();

    m_strFontName = strFont;
    m_sizeFont.cy = iRecommendedFontHeight;

    createFont();
    forceProperSize();
    InvalidateRect (m_hWindow,NULL,TRUE);
    LeaveCriticalSection (&m_csThread);
    return 1;
}

int BScreen::setFont
(
    HBITMAP  hbmpFont,
    unsigned uCellsInWidth,
    unsigned uCellsInHeight,
    unsigned uCellWidth,
    unsigned uCellHeight
)
{
    EnterCriticalSection (&m_csThread);
    destroyFont();
    
    m_hbmpFontMap             = hbmpFont;
    m_sizeFont.cx             = uCellWidth;
    m_sizeFont.cy             = uCellHeight;
    m_sizeCellOrganization.cx = uCellsInWidth;
    m_sizeCellOrganization.cy = uCellsInHeight;

    createFont(); // if new bitmap is NULL then we recreate the last GDI font selected (but default size)
    forceProperSize();
    InvalidateRect (m_hWindow,NULL,TRUE);
    LeaveCriticalSection (&m_csThread);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::print
//
void BScreen::print
(
    const BString& str
)
{
    getMutex();

    DWORD dwColor = (m_dwCurrentColor << 8) & 0xff00;
    for (const char *psz = str; psz[0]; psz++)
    {
        switch (psz[0])
        {
            case '\n':   m_ptCursor.y++;                         break;
            case '\r':   m_ptCursor.x = 0;                       break;
            case '\t':   m_ptCursor.x = (m_ptCursor.x + 4) & ~3; break;
            case '\x01': clear2EOL(); break;
            default:
            {
                m_pawScreen[m_ptCursor.y * m_sizeBuffer.cx + m_ptCursor.x] = WORD(dwColor) | WORD(psz[0]);
                m_ptCursor.x++;
                break;
            }
        }

        if (m_ptCursor.x >= m_sizeBuffer.cx)
        {
            m_ptCursor.x = 0;
            m_ptCursor.y++;
        }
        assert (m_ptCursor.x < m_sizeBuffer.cx);
        if (m_ptCursor.y >= m_sizeBuffer.cy)
        {
            m_ptCursor.y--;
            scrollBuffer();
        }
        assert (m_ptCursor.y < m_sizeBuffer.cy);
    }

    scrollToCaret(); 
    releaseMutex();
    InvalidateRect (m_hWindow,NULL,FALSE);
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::clear2EOL
//
void BScreen::clear2EOL
(
    void
)
{
    for (int x = m_ptCursor.x; x < m_sizeBuffer.cx; x++)
    {
        m_pawScreen[m_ptCursor.y * m_sizeBuffer.cx + x] = WORD(m_dwCurrentColor << 8) | WORD(0x20);
    }
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::create
//
BOOL BScreen::create
(
    HINSTANCE hInstance,
    int       iBufferWidth,
    int       iBufferHeight
)
{
    //
    // fail to create an already existing window
    //
    if (m_hThread)
    {
        return FALSE;
    }

    //
    // init
    //
    m_hInstance     = hInstance;
    m_sizeBuffer.cx = iBufferWidth;
    m_sizeBuffer.cy = iBufferHeight;
    m_ptTopLeft.x   = 0;
    m_ptTopLeft.y   = 0;
    m_sizeCell.cx   = 0; // will be set by font 
    m_sizeCell.cy   = 0; // will be set by font 
    m_sizeFont.cx   = 0; // will be set by font 
    m_sizeFont.cy   = 0; // desired - OS may adjust
    m_strFontName   = "FixedSys";

    m_ptCursor.x     = 0;
    m_ptCursor.y     = 0;
    m_dwCurrentColor = 0x08;
    m_iCaretWidth    = 2;

    //
    // default palette
    //
    m_acolPalette[ 0] = RGB(0x00,0x00,0x00);
    m_acolPalette[ 1] = RGB(0x00,0x00,0x80);
    m_acolPalette[ 2] = RGB(0x00,0x80,0x00);
    m_acolPalette[ 3] = RGB(0x00,0x80,0x80);
    m_acolPalette[ 4] = RGB(0x80,0x00,0x00);
    m_acolPalette[ 5] = RGB(0x80,0x00,0x80);
    m_acolPalette[ 6] = RGB(0x80,0x80,0x00);
    m_acolPalette[ 7] = RGB(0x80,0x80,0x80);
    m_acolPalette[ 8] = RGB(0x80,0x80,0xf0);
    m_acolPalette[ 9] = RGB(0x00,0x00,0xff);
    m_acolPalette[10] = RGB(0x00,0xff,0x00);
    m_acolPalette[11] = RGB(0x00,0xff,0xff);
    m_acolPalette[12] = RGB(0xff,0x00,0x00);
    m_acolPalette[13] = RGB(0xff,0x00,0xff);
    m_acolPalette[14] = RGB(0xff,0xff,0x00);
    m_acolPalette[15] = RGB(0xff,0xff,0xff);


    //
    // create worker thread
    //
    m_enumThreadStatus = threadStarting;
    m_hThread          = CreateThread(NULL,0,__BScreen_createScreen,this,0,&m_dwThreadID);
    if (!m_hThread)
    {
        destroy();
        return FALSE;
    }

    //
    // wait for thread to signal creation success
    //
    THREAD_STATUS status;
    do
    {
        Sleep (1);
        EnterCriticalSection (&m_csThread);
        status = m_enumThreadStatus;
        LeaveCriticalSection (&m_csThread);
    } while (status == threadStarting);
    if (status == threadAbnormalTermination)
    {
        // window creation failed
        destroy();
        return FALSE;
    }

    //
    // done
    //
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::destroy
//
BOOL BScreen::destroy
(
    void
)
{
    //
    // kill worker thread
    //
    if (m_hThread)
    {
        THREAD_STATUS status;
        SendMessage (m_hWindow,WM_MUSTCLOSE,0,0L);
        
        // try to shut down for 100ms (works around a bug where the OS wont let
        //  the window die while you are trying to use it (resize/scroll/capture), even though you post WM_CLOSE)
        for (int i = 0; i < 100; i++)
        {
            Sleep (1);
            EnterCriticalSection (&m_csThread);
            status = m_enumThreadStatus;
            LeaveCriticalSection (&m_csThread);

            if (status > threadRunning)
            {
                break;
            }

            SendMessage (m_hWindow,WM_MUSTCLOSE,0,0L);
        } 

        CloseHandle (m_hThread);
        m_hThread = NULL;
    }

    //
    // done
    //
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::BScreen
//
BScreen::BScreen (void)
{
    //
    // init
    //
    InitializeCriticalSection (&m_csThread);
    m_hThread      = NULL;
    m_pawScreen    = NULL;
    m_hFont        = NULL;
    m_paiSpacing   = NULL;
    m_dwFlags      = 0;
    m_dwCaretCount = 1;
    m_pfnHandler   = NULL;
    m_hbmpFontMap  = NULL;
}

//////////////////////////////////////////////////////////////////////////////
// BScreen::~BScreen
//
BScreen::~BScreen (void)
{
    //
    // clean up
    //
    destroy();
    DeleteCriticalSection (&m_csThread);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// console
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// BConsole::showLine
//
void BConsole::showLine
(
    const BString& strLine,
    const POINT&   ptStart,
          DWORD    dwCrsr
)
{
    m_Screen.gotoXY (ptStart);
    m_Screen.print  (strLine);
    m_Screen.clear2EOL();
    m_Screen.gotoXY ((ptStart.x + dwCrsr) % m_sizeBuffer.cx,
                     (ptStart.x + dwCrsr) / m_sizeBuffer.cx + ptStart.y);
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::findName
//
int BConsole::findName
(
    const BString&  strMatch,
          BString  *pstrResult,
          BOOL      bList
)
{
    // any work?
    if (m_listName.isEmpty()) return 0;

    // create a list of sub-matches
    BLinkedList<BString,const BString&> list;
    BPosition pos = m_listName.getHeadPosition();
    while (pos)
    {
        BString str = m_listName.getNextPosition(pos);
        if (!strncmp(str,strMatch,strMatch.length()))
        {
            list.addHead (str);
        }
    }
    if (list.isEmpty()) return 0;

    // find the longest string all words have in common
    if (bList) print ("\r\n");
    DWORD dwCommonLen = ~0;
    pos = list.getHeadPosition();
    while (pos)
    {
        BString   s1 = list.getNextPosition(pos);
        dwCommonLen  = bmin(dwCommonLen,(DWORD)s1.length());
        BPosition p2 = pos;
        while (p2)
        {
            BString s2  = list.getNextPosition(p2);
            dwCommonLen = bmin(dwCommonLen,(DWORD)s2.length());

            const char  *c1 = s1;
            const char  *c2 = s2;
                  DWORD  dw = dwCommonLen;
            while (c1[0] && dw && (*c1 == *c2)) c1++,c2++,dw--;
            if (dw) dwCommonLen -= dw;
        }
        if (bList) print ("%s\t",(const char*)s1);
    }
    if (bList) print ("\r\n");

    // prepare to return
    *pstrResult = BString(list.getHead()).left(dwCommonLen);
    return 1;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::reset
//
void BConsole::reset
(
    void
)
{
    m_dwFlags |= flagResetCmdLine;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::print
//
void BConsole::print
(
    const char *pszFormat,
    ...
)
{
    //
    // compose
    //
    char szBuffer[8192];
    vsprintf (szBuffer,pszFormat,(char*)(DWORD(&pszFormat) + 4));

    //
    // print
    //
    m_Screen.print (szBuffer);
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::read
//
BConsole::READRESULT BConsole::read
(
    const BString&  strPrompt,
    BString        *pstrText,
    READIDLEFUNC    pfnIdleFunc,
    void           *pContext
)
{
    //
    // start new input
    //
    BString   strLine       = "";
    DWORD     dwCrsr        = 0;
    DWORD     dwHistoryPos  = 0; // zero means current line
    DWORD     dwLastTab     = 0;
    DWORD     dwLastTabCrsr = ~0;
    POINT     ptStart;

    // prompt
    print ("%s",(const char*)strPrompt);
    m_Screen.getXY (&ptStart);

    //
    // process messages
    //
    READRESULT res;
    for (BOOL bQuit = FALSE; !bQuit; )
    {
        if (m_dwFlags & flagResetCmdLine)
        {
            print ("%s",(const char*)strPrompt);
            m_Screen.getXY (&ptStart);
            showLine (strLine,ptStart,dwCrsr);
            m_dwFlags &= ~flagResetCmdLine;
        }

        BScreen::MESSAGE msg;
        if (m_Screen.getMessage(&msg))
        {
            switch (msg.uMessage)
            {
                case WM_KEYDOWN:
                {
                    switch (msg.wParam)
                    {
                        case VK_HOME:
                        {
                            dwCrsr = 0;
                            showLine (strLine,ptStart,dwCrsr);
                            break;
                        }
                        case VK_END:
                        {
                            dwCrsr = strLine.length();
                            showLine (strLine,ptStart,dwCrsr);
                            break;
                        }
                        case VK_LEFT:
                        {
                            if (dwCrsr) dwCrsr--;
                            showLine (strLine,ptStart,dwCrsr);
                            break;
                        }
                        case VK_RIGHT:
                        {
                            if (dwCrsr < (DWORD)strLine.length()) dwCrsr++;
                            showLine (strLine,ptStart,dwCrsr);
                            break;
                        }
                        case VK_INSERT:
                        {
                            m_dwFlags ^= flagOverwrite;
                            m_Screen.setCaretWidth ((m_dwFlags & flagOverwrite) ? 1000 : 0); // full width or most narrow
                            break;
                        }
                        case VK_DELETE:
                        {
                            if (dwCrsr < (DWORD)strLine.length())
                            {
                                strLine.remove (dwCrsr);
                                showLine (strLine,ptStart,dwCrsr);
                            }
                            break;
                        }
                        case VK_DOWN:
                        {
                            if (m_dwFlags & flagHistory)
                            {
                                if (dwHistoryPos)
                                {
                                    dwHistoryPos--;
                                    if (dwHistoryPos)
                                    {
                                        BPosition pos = m_listHistory.getIndexPosition(dwHistoryPos - 1);
                                        strLine = pos ? m_listHistory.getData(pos) : "";
                                    }
                                    else
                                    {
                                        strLine = "";
                                    }
                                    dwCrsr  = strLine.length();
                                    showLine (strLine,ptStart,dwCrsr);
                                }
                            }
                            break;
                        }
                        case VK_UP:
                        {
                            if (m_dwFlags & flagHistory)
                            {
                                if (dwHistoryPos < m_dwHistoryCount)
                                {
                                    dwHistoryPos++;
                                    BPosition pos = m_listHistory.getIndexPosition(dwHistoryPos - 1);
                                    strLine = pos ? m_listHistory.getData(pos) : "";
                                    dwCrsr  = strLine.length();
                                    showLine (strLine,ptStart,dwCrsr);
                                }
                            }
                            break;
                        }
                    }
                    break;
                }
                case WM_CHAR:
                {
                    switch (msg.wParam)
                    {
                        case '\t':
                        {
                            // name completion
                            if ((m_dwFlags & flagNameCompletion)     // enabled
                             && (dwCrsr == (DWORD)strLine.length())) // crsr must be at EOL
                            {
                                // get time to see if we have a double-tab
                                DWORD dwNow = GetTickCount();
                                BOOL  bList = ((dwNow - dwLastTab) < 500)
                                           && ((dwCrsr == dwLastTabCrsr));
                                dwLastTab     = dwNow;
                                dwLastTabCrsr = dwCrsr;
                                // get current word
                                const char *sz = strLine;
                                const char *ch = sz + strlen(sz);
                                while ((ch > sz) && (!isspace(ch[-1]))) ch--;
                                // find best match
                                BString strMatch;
                                if (ch[0] 
                                 && findName(BString(ch),&strMatch,bList))
                                {
                                    if (bList) m_Screen.getXY (&ptStart);
                                    strLine.place (ch - sz,strMatch);
                                    dwCrsr = strLine.length();
                                }
                                else
                                {
                                    //if (bList) m_Screen.getXY (&ptStart);
                                }
                                showLine (strLine,ptStart,dwCrsr);
                            }
                            break;
                        }
                        case 13:
                        {
                            if (m_dwFlags & flagHistory)
                            {
                                m_listHistory.addHead (strLine);
                                m_dwHistoryCount ++;
                                if (m_dwHistoryCount > 5000)
                                {
                                    m_listHistory.removeTail();
                                    m_dwHistoryCount--;
                                }
                            }

                            m_Screen.print ("\r\n");
                            *pstrText = strLine;
                            res       = success;
                            bQuit     = TRUE;
                            break;
                        }
                        case 8:
                        {
                            if (dwCrsr
                             && strLine.length())
                            {
                                dwCrsr--;
                                strLine.remove (dwCrsr);
                                showLine (strLine,ptStart,dwCrsr);
                            }
                            break;
                        }
                        default:
                        {
                            if (msg.wParam >= 32)
                            {
                                char sz[2] = { char(msg.wParam),0 };
                                if (m_dwFlags & flagOverwrite) strLine.place  (dwCrsr,sz);
                                                          else strLine.insert (dwCrsr,sz);
                                dwCrsr++;
                                showLine (strLine,ptStart,dwCrsr);
                            }
                            break;
                        }
                    }
                    break;
                }
                case WM_CLOSE:
                {
                    m_dwFlags |= flagScreenClosed;
                    res        = closed;
                    bQuit      = TRUE;
                    break;
                }
            }
        }
        else
        {
            if (pfnIdleFunc) 
            {
                res = pfnIdleFunc(pContext);
                if (res != success) bQuit = TRUE; // caller asked for termination
            }
            Sleep (10);
        }
    }

    //
    // done
    //
    return res;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::create
//
BOOL BConsole::create
(
    HINSTANCE hInstance,
    int       iBufferWidth,
    int       iBufferHeight
)
{
    //
    // create screen
    //
    if (!m_Screen.create(hInstance,iBufferWidth,iBufferHeight))
    {
        return FALSE;
    }
    m_sizeBuffer.cx = iBufferWidth;
    m_sizeBuffer.cy = iBufferHeight;

    //
    // init
    //
    m_dwFlags        = 0;
    m_dwHistoryCount = 0;
    m_listHistory.removeAll();

    //
    // prepare for console mode
    //
    m_Screen.setCaretWidth ((m_dwFlags & flagOverwrite) ? 1000 : 0); // full width or most narrow
    m_Screen.showCaret();

    //
    // done
    //
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::destroy
//
BOOL BConsole::destroy
(
    void
)
{
    //
    // destroy screen
    //
    m_Screen.destroy();

    //
    // clean up
    //
    m_dwFlags |= flagScreenClosed;

    //
    // done
    //
    return TRUE;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::BConsole
//
BConsole::BConsole
(
    void
)
{
    //
    // init
    //
    m_dwFlags |= flagScreenClosed;
}

//////////////////////////////////////////////////////////////////////////////
// BConsole::~BConsole
//
BConsole::~BConsole
(
    void
)
{
}