/***************************************************************************
 *
 *  Copyright (C) 2/11/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       toolbar.cpp
 *  Content:    Toolbar and toolbar dock classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/11/2002   dereks  Created.
 *
 ****************************************************************************/

#include "xactctl.h"


/****************************************************************************
 *
 *  CToolBar
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
#define DPF_FNAME "CToolBar::CToolBar"

CToolBar::CToolBar
(
    void
)
{
}


/****************************************************************************
 *
 *  ~CToolBar
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
#define DPF_FNAME "CToolBar::~CToolBar"

CToolBar::~CToolBar
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
 *      Creates a window.
 *
 *  Arguments:
 *      CWindow * [in]: parent window.
 *      UINT [in]: control identifier.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CToolBar::Create"

BOOL
CToolBar::Create
(
    CWindow *               pParent, 
    DWORD                   dwExStyle,
    DWORD                   dwStyle,
    UINT                    nControlId
)
{
    dwStyle |= WS_CHILD;

    //
    // Create the window
    //

    if(!CWindow::Create(pParent, TOOLBARCLASSNAME, NULL, dwExStyle, dwStyle, 0, 0, 0, 0, nControlId))
    {
        return FALSE;
    }

    SendMessage(TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0);
    SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(16, 16));

    //
    // Add the standard image lists
    //

    SendMessage(CCM_SETVERSION, 5, 0);

    LoadImages(IDB_HIST_SMALL_COLOR, 0, HINST_COMMCTRL);
    LoadImages(IDB_STD_SMALL_COLOR, 0, HINST_COMMCTRL);
    LoadImages(IDB_VIEW_SMALL_COLOR, 0, HINST_COMMCTRL);

    return TRUE;
}


/****************************************************************************
 *
 *  LoadImages
 *
 *  Description:
 *      Loads a bitmap into the toolbar's image list.
 *
 *  Arguments:
 *      UINT [in]: bitmap resource identifier.
 *      UINT [in]: bitmap image count.
 *
 *  Returns:  
 *      UINT: index of the image list added.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CToolBar::LoadImages"

UINT
CToolBar::LoadImages
(
    UINT                    nBitmapId,
    UINT                    nImageCount,
    HINSTANCE               hInstance
)
{
    const UINT              nImageList  = m_nNextImageList;
    TBADDBITMAP             tbab;
    
    if(nImageList >= NUMELMS(m_anFirstImageIndex))
    {
        DPF_ERROR("Too many image lists");
        return 0;
    }

    if(!hInstance)
    {
        hInstance = g_hInstance;
    }

    m_nNextImageList++;

    tbab.hInst = hInstance;
    tbab.nID = nBitmapId;

    m_anFirstImageIndex[nImageList] = SendMessage(TB_ADDBITMAP, nImageCount, (LPARAM)&tbab);
    
    return nImageList;
}


/****************************************************************************
 *
 *  AddButton
 *
 *  Description:
 *      Adds a button to the toolbar.
 *
 *  Arguments:
 *      UINT [in]: bitmap index.
 *      UINT [in]: image index (within the bitmap).
 *      UINT [in]: command identifier.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CToolBar::AddButton"

BOOL
CToolBar::AddButton
(
    UINT                    nBitmapIndex,
    UINT                    nImageIndex,
    UINT                    nCommandId
)
{
    TBBUTTON                tbb = { 0 };
    
    tbb.iBitmap = m_anFirstImageIndex[nBitmapIndex] + nImageIndex;
    tbb.idCommand = nCommandId;
    tbb.fsState = TBSTATE_ENABLED;
    tbb.fsStyle = BTNS_BUTTON;
    
    return SendMessage(TB_ADDBUTTONS, 1, (LPARAM)&tbb);
}


/****************************************************************************
 *
 *  AddSeparator
 *
 *  Description:
 *      Adds a separator to the toolbar.
 *
 *  Arguments:
 *      (void)
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CToolBar::AddSeparator"

BOOL
CToolBar::AddSeparator
(
    void
)
{
    TBBUTTON                tbb = { 0 };
    
    tbb.fsStyle = BTNS_SEP;
    
    return SendMessage(TB_ADDBUTTONS, 1, (LPARAM)&tbb);
}


/****************************************************************************
 *
 *  SetButtonState
 *
 *  Description:
 *      Sets the state-mask for a button.
 *
 *  Arguments:
 *      UINT [in]: command identifier.
 *      DWORD [in]: valid bit mask.
 *      DWORD [in]: button state.
 *
 *  Returns:  
 *      BOOL: TRUE on success.
 *
 ****************************************************************************/

#undef DPF_FNAME
#define DPF_FNAME "CToolBar::SetButtonState"

BOOL
CToolBar::SetButtonState
(
    UINT                    nCommandId, 
    DWORD                   dwMask,
    DWORD                   dwState
)
{
    TBBUTTONINFO            tbbi    = { 0 };
    
    tbbi.cbSize = sizeof(tbbi);
    tbbi.dwMask = TBIF_STATE;

    if(!SendMessage(TB_GETBUTTONINFO, nCommandId, (LPARAM)&tbbi))
    {
        return FALSE;
    }

    tbbi.fsState &= ~dwMask;
    tbbi.fsState |= dwState;
    
    return SendMessage(TB_SETBUTTONINFO, nCommandId, (LPARAM)&tbbi);
}


