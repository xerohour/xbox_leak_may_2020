#ifndef _prxycust_h
#define _prxycust_h

///////////////////////////////////////////////////////////////////////////////
//      CToolCustomizer
class CToolCustomizer : public CCmdTarget
{
public:
	CToolCustomizer(CDockManager* m_pManager, CDlgTab* pDlg,
		const CObArray& aBars, BOOL bTemp = FALSE, BOOL bUndo = FALSE);
	~CToolCustomizer();
	
	void SetSelection(CToolBarProxy* pBar, int nIndex);
	BOOL DeleteSelection(BOOL bDeferRemove=FALSE);
	void DrawSelection(CDC* pDC);

	// This specifies what kind of button is being moved with DoButtonMove
	enum MOVETYPE
	{
		cBar=0,
		cCommand=1
	};
	// This function is now able to deal with dragging from the 'all commands' or 'menus' list,
	// where there is no source bar. To signal this kind of move, pass pBar==NULL, nIndex ignored, 
	// and the appropriate MOVETYPE code and nCommand the command id (or menu index) to be placed 
	// on the toolbar. martynl 23Apr96
	// This function now returns false if nothing was move, and whether a delete is pending in bDeletePending
	BOOL DoButtonMove(CPoint pt, CToolBarProxy* pBar, int nIndex, BOOL *bDeletePending, MOVETYPE mtType=cBar, int nCommand=0);
	BOOL TrackButtonMove(CPoint &pt, BOOL& bCopy, CToolBarProxy **ppBar, int *nIndex, ORIENTATION *or, UINT *pnHTSize, int nCmdIDSrc);
	void DoButtonSize(CPoint pt, CToolBarProxy* pBar, int nIndex);
	BOOL TrackButtonSize(CPoint pt, CRect& rectFinal);
	// creates a TRANSFER_TBUTTON appropriate for inserting a separator.
	static TRANSFER_TBBUTTON *CreateSeparatorTransfer(void);
	// displays the button customisation menu
	BOOL DisplayButtonMenu(UINT nFlags, CPoint point);
	static POPDESC *GetButtonMenu(void);

protected:
	void DrawTrackRect(CDC* pdc, const CRect* prect1,
		const CRect* prect2 = NULL);
	// for a given point, this determines which bar, and after which button, the insertion would occur.
	// If the insertion would be before the first button, then *nIndex is -1 after the call. If previous
	// is non null, then it will be filled with the rect of the preceding button, or of the first button
	// if nIndex is -1.
	// If pBarDest is NULL, the button was dropped in empty space (nIndex==0) or on the parent dialog (nIndex==-1)
	// It also returns the correct orientation to be used when expanding the bar, and the ht code
	void GetDropButton(CPoint pt, CToolBarProxy **ppBar, int *nIndex, CRect *previous, ORIENTATION *or, UINT *pnHTSize);
	// draws the insertion point cursor on a bar
	void DrawDragFeedback(CDC *pDC, const CRect &rect, ORIENTATION or, BOOL bBefore);

	// implementation helpers
	// creates a TRANSFER_TBUTTON appropriate for moving the given command id from nowhere. Puts
	// up the choose button dialogue
	TRANSFER_TBBUTTON *CreateCommandTransfer(int nCommand, CToolBarProxy *pBarDest);

public:
	BOOL m_bTemp:1;
	BOOL m_bUndo:1;
	CDockManager* m_pManager;
	CDlgTab* m_pDialog;
	CToolBarProxy* m_pSelectBar;
	int m_nSelectIndex;

    //{{AFX_MSG(CToolCustomizer)
	afx_msg void OnButtonCopyImage();
	afx_msg void OnButtonPasteImage();
	afx_msg void OnButtonResetImage();
	afx_msg void OnButtonChooseAppearance();
	afx_msg void OnButtonReset();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonImageOnly();
	afx_msg void OnButtonTextOnly();
	afx_msg void OnButtonImageText();
	afx_msg void OnButtonGroupStart();
	afx_msg void OnUpdateButtonCopyImage(CCmdUI *);
	afx_msg void OnUpdateButtonPasteImage(CCmdUI *);
	afx_msg void OnUpdateButtonResetImage(CCmdUI *);
	afx_msg void OnUpdateButtonChooseAppearance(CCmdUI *);
	afx_msg void OnUpdateButtonReset(CCmdUI *);
	afx_msg void OnUpdateButtonDelete(CCmdUI *);
	afx_msg void OnUpdateButtonImageOnly(CCmdUI *);
	afx_msg void OnUpdateButtonTextOnly(CCmdUI *);
	afx_msg void OnUpdateButtonImageText(CCmdUI *);
	afx_msg void OnUpdateButtonGroupStart(CCmdUI *);
	//}}AFX_MSG
    
    DECLARE_MESSAGE_MAP()

protected:
	const CObArray& m_aBars;
};

extern UINT DSM_COMMANDREMOVED;
// This message will be sent to the dialog whenever a command is deleted from a bar or menu
// during toolbar customisation
// WPARAM: nCmd - index of newly removed command
// LPARAM: must be 0 (reserved)

#endif
