/***************************************************************************
 *
 *  Copyright (C) 2/11/2002 Microsoft Corporation.  All Rights Reserved.
 *
 *  File:       toolbar.h
 *  Content:    Toolbar and toolbar dock classes.
 *  History:
 *   Date       By      Reason
 *   ====       ==      ======
 *  2/11/2002   dereks  Created.
 *
 ****************************************************************************/

#ifndef __TOOLBAR_H__
#define __TOOLBAR_H__

#ifdef __cplusplus

//
// ToolBar wrapper class
//

class CToolBar
    : public CWindow
{
public:
    
    // 
    // Default image lists
    //

    enum
    {
        IML_HIST,
        IML_STD,
        IML_VIEW,
        IML_COUNT
    };

protected:
    UINT                    m_anFirstImageIndex[64];
    UINT                    m_nNextImageList;

public:
    CToolBar(void);
    virtual ~CToolBar(void);

public:
    // Creation
    virtual BOOL Create(CWindow *pParent, DWORD dwExStyle, DWORD dwStyle, UINT nControlId);

    // Toolbar buttons
    virtual UINT LoadImages(UINT nBitmapId, UINT nImageCount, HINSTANCE hInstance = NULL);
    virtual BOOL AddButton(UINT nBitmapIndex, UINT nImageIndex, UINT nCommandId);
    virtual BOOL AddSeparator(void);
    virtual BOOL EnableButton(UINT nCommandId, BOOL fEnable);
    virtual BOOL SetButtonState(UINT nCommandId, DWORD dwMask, DWORD dwState);
};

__inline BOOL CToolBar::EnableButton(UINT nCommandId, BOOL fEnable)
{
    return SendMessage(TB_ENABLEBUTTON, nCommandId, MAKELONG(fEnable, 0));
}

#endif // __cplusplus

#endif // __TOOLBAR_H__
