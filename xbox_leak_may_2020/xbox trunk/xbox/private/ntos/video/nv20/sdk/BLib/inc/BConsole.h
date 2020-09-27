#ifndef _BCONSOLE_H
#define _BCONSOLE_H

#define MAP_MESSAGE(msg,handler)        case msg: return handler(uMsg,wParam,lParam);
#define DECL_MESSAGE(handler)           LRESULT handler (UINT uMsg,WPARAM wParam,LPARAM lParam);

//////////////////////////////////////////////////////////////////////////////
// constants
//
#define WM_MUSTCLOSE        (WM_USER + 0)
#define WM_MOUSEWHEEL       0x020A

//////////////////////////////////////////////////////////////////////////////
// screen
//
class BScreen
{
    //
    // constants
    //
public:
    struct MESSAGE
    {
        HWND   hWnd;
        UINT   uMessage;
        WPARAM wParam;
        LPARAM lParam;
    };
    enum PMRESULT
    {
        consumed,
        buffer      // ask screen to buffer this message (retrieve with getMessage later)
    };

protected:
    typedef PMRESULT (*PROCESSMESSAGEFUNCTION)(void *pContext,UINT uMsg,WPARAM wParam,LPARAM lParam);

    enum THREAD_STATUS // order sensitive
    {
        threadStarting,
        threadRunning,
        threadStopped,
        threadAbnormalTermination
    };
    enum
    {
        flagCaretCreated = 1,
        flagCaretEnabled = 2,
        flagCaretOn      = 4,
    };

    //
    // members
    //
protected:
    SIZE      m_sizeBuffer;
    SIZE      m_sizeCell;
    WORD     *m_pawScreen;
    POINT     m_ptTopLeft;
    HINSTANCE m_hInstance;
    HWND      m_hWindow;
    INT      *m_paiSpacing;
    COLORREF  m_acolPalette[16];

    HFONT     m_hFont;
    BString   m_strFontName;
    HBITMAP   m_hbmpFontMap;     // if not empty then we have user defined
    SIZE      m_sizeFont;
    SIZE      m_sizeCellOrganization;   // user font only

    DWORD     m_dwFlags;
    DWORD     m_dwCaretCount;
    int       m_iCaretWidth;
    POINT     m_ptCursor;
    DWORD     m_dwCurrentColor;

    HANDLE           m_hThread;
    DWORD            m_dwThreadID;
    THREAD_STATUS    m_enumThreadStatus;    // current thread status
    CRITICAL_SECTION m_csThread;            // thread status interlock

    BLinkedList<MESSAGE,const MESSAGE&>  m_listMessage;
    PROCESSMESSAGEFUNCTION               m_pfnHandler;
    void                                *m_pMHContext;

protected:
    friend LRESULT CALLBACK __BScreen_windowProc   (HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
    friend DWORD   WINAPI   __BScreen_createScreen (LPVOID lpContext);

    //
    // window interface
    //
protected:
    LRESULT windowProc (HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
    {
        switch (uMsg)
        {
            MAP_MESSAGE (WM_GETMINMAXINFO,  getMinMaxInfo)
            MAP_MESSAGE (WM_HSCROLL,        hScroll)
            MAP_MESSAGE (WM_VSCROLL,        vScroll)
            MAP_MESSAGE (WM_MOUSEWHEEL,     mouseWheel)
            MAP_MESSAGE (WM_PAINT,          paint)
            MAP_MESSAGE (WM_MUSTCLOSE,      mustClose)
            MAP_MESSAGE (WM_SETFOCUS,       setFocus)
            MAP_MESSAGE (WM_KILLFOCUS,      killFocus)

            MAP_MESSAGE (WM_CLOSE,          queueMessage)
            MAP_MESSAGE (WM_MOUSEMOVE,      queueMessage)
            MAP_MESSAGE (WM_LBUTTONDOWN,    queueMessage)
            MAP_MESSAGE (WM_LBUTTONUP,      queueMessage)
            MAP_MESSAGE (WM_RBUTTONDOWN,    queueMessage)
            MAP_MESSAGE (WM_RBUTTONUP,      queueMessage)
            MAP_MESSAGE (WM_KEYDOWN,        queueMessage)
            MAP_MESSAGE (WM_KEYUP,          queueMessage)
            MAP_MESSAGE (WM_CHAR,           queueMessage)
        }
        return DefWindowProc(hWnd,uMsg,wParam,lParam);
    }

    DECL_MESSAGE (getMinMaxInfo);
    DECL_MESSAGE (hScroll);
    DECL_MESSAGE (vScroll);
    DECL_MESSAGE (mouseWheel);
    DECL_MESSAGE (paint);
    DECL_MESSAGE (mustClose);
    DECL_MESSAGE (setFocus);
    DECL_MESSAGE (killFocus);

    DECL_MESSAGE (queueMessage);

    //
    // methods
    //
protected:
    int  createFont    (void);
    void destroyFont   (void);
    int  createScreen  (void);
    void destroyScreen (void);
    void drawText      (HDC hdc,HDC hFontMap,const POINT *pt,const char *psz,DWORD dwCharCount,DWORD dwColor);
    
    void caretOff      (void);
    void caretOn       (void);
    void updateCaret   (BOOL bForceRecreate = FALSE);
    void scrollToCaret (void);

    void getMutex     (void) { EnterCriticalSection (&m_csThread); }
    void releaseMutex (void) { LeaveCriticalSection (&m_csThread); }

    void scrollBuffer (void);

    void forceProperSize (void)
    {
        RECT r;
        GetWindowRect (m_hWindow,&r);
        SetWindowPos  (m_hWindow,NULL,0,0,r.right - r.left,r.bottom - r.top,SWP_NOZORDER | SWP_NOMOVE);
    }

public:
    // messaging
    int  getMessage               (MESSAGE *pMessage); // only stores messages if no handler is installed, or when handler rejects
    void registerMessageHandler   (PROCESSMESSAGEFUNCTION pfn,void* pContext) { getMutex(); m_pfnHandler = pfn; m_pMHContext = pContext; releaseMutex(); }
    void unregisterMessageHandler (void)                                      { getMutex(); m_pfnHandler = NULL; releaseMutex(); }

    // sizes
    int setFont (const BString& strFont,int iRecommendedFontHeight = 0);
    int setFont (HBITMAP hbmpFont,unsigned uCellsInWidth,unsigned uCellsInHeight,unsigned uCellWidth,unsigned uCellHeight);

    // window
    void setCaption    (const BString& str)       { SetWindowText (m_hWindow,(const char*)str); }
    void setWindowPos  (int iX,int iY)            { SetWindowPos (m_hWindow,NULL,iX,iY,0,0,SWP_NOSIZE | SWP_NOZORDER); }
    void getWindowPos  (SIZE *pSize)        const { RECT r; GetWindowRect (m_hWindow,&r); pSize->cx = r.left; pSize->cy = r.top; }
    void setWindowSize (int iX,int iY)            { SetWindowPos (m_hWindow,NULL,0,0,iX,iY,SWP_NOMOVE | SWP_NOZORDER); }
    void getWindowSize (SIZE *pSize)        const { RECT r; GetWindowRect (m_hWindow,&r); pSize->cx = r.right - r.left; pSize->cy = r.bottom - r.top; }

    void hideWindow      (void)       { ShowWindow (m_hWindow,SW_MINIMIZE); ShowWindow (m_hWindow,SW_HIDE); }
    void showWindow      (void)       { ShowWindow (m_hWindow,SW_SHOW); ShowWindow (m_hWindow,SW_RESTORE); }
    int  isWindowVisible (void) const { return IsWindowVisible(m_hWindow); }

    // buffer
    void setCell   (int iX,int iY,WORD wValue)       { getMutex(); m_pawScreen[iY * m_sizeBuffer.cx + iX] = wValue; InvalidateRect (m_hWindow,NULL,FALSE); releaseMutex(); }
    WORD getCell   (int iX,int iY)             const { return m_pawScreen[iY * m_sizeBuffer.cx + iX]; }
    void fillCells (int iX,int iY,int iWidth,int iHeight,WORD wValue)
    {
        getMutex();
        WORD *line = &m_pawScreen[iY * m_sizeBuffer.cx + iX];
        for (int y = iY,cy = iHeight; cy; y++,cy--)
        {
            fillWord (line,wValue,iWidth);
            line += m_sizeBuffer.cx;
        }
        InvalidateRect (m_hWindow,NULL,FALSE);
        releaseMutex();
    }

    // palettes
    void setPalette (const COLORREF *pacolPalette) { getMutex(); memcpy (m_acolPalette,pacolPalette,sizeof(m_acolPalette)); InvalidateRect (m_hWindow,NULL,TRUE); releaseMutex(); }
    void getPalette (COLORREF *pacolPalette) const { memcpy (pacolPalette,m_acolPalette,sizeof(m_acolPalette)); }

    // print
    void  setColor   (DWORD dwIndex)                { m_dwCurrentColor = dwIndex; }
    DWORD getColor   (void)                         { return m_dwCurrentColor; }
    void  print      (const BString& str);
    void  clear2EOL  (void);

    // cursor
    int  gotoXY    (int iX,int iY)
    { 
        if ((iX < 0) 
         || (iY < 0) 
         || (iX >= m_sizeBuffer.cx) 
         || (iY >= m_sizeBuffer.cy)) return 0; 
        getMutex(); 
        caretOff(); 
        m_ptCursor.x = iX; 
        m_ptCursor.y = iY; 
        caretOn(); 
        releaseMutex(); 
        return 1; 
    }
    int  gotoXY    (const POINT &pt)         { return gotoXY(pt.x,pt.y); }
    void getXY     (POINT *pPt)        const { *pPt = m_ptCursor; }
    void getXY     (int *piX,int *piY) const { *piX = m_ptCursor.x; *piY = m_ptCursor.y; }
    void showCaret (void)                    
    { 
        getMutex(); 
        if (!m_dwCaretCount // do not allow below zero
         || !(--m_dwCaretCount))
        {
            m_dwFlags |=  flagCaretEnabled; 
            updateCaret();
        }
        releaseMutex(); 
    }
    void hideCaret (void)
    { 
        getMutex(); 
        if (++m_dwCaretCount)
        {
            m_dwFlags &= ~flagCaretEnabled; 
            updateCaret(); 
        }
        releaseMutex(); 
    }
    void setCaretWidth (int iWidth)
    {
        getMutex(); 
        iWidth        = max(iWidth,2);
        m_iCaretWidth = min(iWidth,m_sizeCell.cx);
        updateCaret (TRUE);
        releaseMutex();
    }

public:
    BOOL create  (HINSTANCE hInstance,int iBufferWidth = 80,int iBufferHeight = 25);
    BOOL destroy (void);

public:
    BScreen (void);
    ~BScreen (void);
};

//////////////////////////////////////////////////////////////////////////////
// console
//
class BConsole
{
    //
    // constants
    //
public:
    enum READRESULT
    {
        closed,
        success,

        user = 10000,
    };
protected:
    enum
    {
        flagScreenClosed   = 1,
        flagOverwrite      = 2,
        flagHistory        = 4, // command history is enabled
        flagNameCompletion = 8, // complete names when TAB is pressed

        flagResetCmdLine   = 0x80000000,
    };

    //
    // types
    //
public:
    typedef READRESULT (*READIDLEFUNC)(void *pContext);

    //
    // members
    //
protected:
    BScreen m_Screen;
    SIZE    m_sizeBuffer;
    DWORD   m_dwFlags;
    DWORD   m_dwHistoryCount;

    BLinkedList<BString,const BString&> m_listHistory;
    BLinkedList<BString,const BString&> m_listName;

    //
    // methods
    //
protected:
    void showLine (const BString& strLine,const POINT& ptStart,DWORD dwCrsr);
    int  findName (const BString& strMatch,BString *pstrResult,BOOL bList);

public:
    // con in and out
    void       reset       (void); // move current command line to cursor
    void       print       (const char *pszFormat,...);
    READRESULT read        (const BString& strPrompt,BString *pstrText,READIDLEFUNC pfnIdleFunc = NULL,void *pContext = NULL);
    
    void       clear2EOL   (void) { m_Screen.clear2EOL(); }

    // state
    void enableCommandHistory  (void) { m_dwFlags |=  flagHistory; }
    void disableCommandHistory (void) { m_dwFlags &= ~flagHistory; } // disable command history
    void enableNameCompletion  (void) { m_dwFlags |=  flagNameCompletion; }
    void disableNameCompletion (void) { m_dwFlags &= ~flagNameCompletion; }

    // UI
    void setCaption    (const BString& str) { m_Screen.setCaption (str); }
    void setWindowPos  (int iX,int iY)      { m_Screen.setWindowPos (iX,iY); }
    void setWindowSize (int iX,int iY)      { m_Screen.setWindowSize (iX,iY); }

    void hideWindow      (void)       { m_Screen.hideWindow(); }
    void showWindow      (void)       { m_Screen.showWindow(); }
    int  isWindowVisible (void) const { return m_Screen.isWindowVisible(); }

    // name completion
    void ncAdd       (const BString& str) { m_listName.addHead (str); }
    void ncRemove    (const BString& str) { BPosition pos = m_listName.findForward(str,m_listName.getHeadPosition()); if (pos) { m_listName.removeAt (pos); } }
    void ncRemoveAll (void)               { m_listName.removeAll(); }

public:
    BOOL create  (HINSTANCE hInstance,int iBufferWidth = 80,int iBufferHeight = 25);
    BOOL destroy (void);

public:
    BConsole (void);
    ~BConsole (void);
};

#endif