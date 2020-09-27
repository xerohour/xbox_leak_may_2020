// seqdlg.h - header for inner sequential dialogs of AppWizard
//

#ifndef SEQDLG_H
#define SEQDLG_H

class CSeqDlg : public C3dDialog
{
// Construction
public:
    CSeqDlg (UINT IDD, CWnd* pParent = NULL);
    virtual BOOL Create(UINT nIDTemplate, CWnd* pParentWnd);
    virtual BOOL CanDestroy() { return TRUE; }
	virtual BOOL ShouldShowPicture() { return TRUE; }
	virtual BOOL ShowTwoBitmaps() { return FALSE; }
	virtual void WriteValues() = 0;
	virtual int GetBitmapSizeIndex() { return BMPSIZE_STANDARD; }
	virtual UINT ChooseBitmap() { return 0; }
	virtual UINT ChooseBitmap2() { return 0; }
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	UINT m_nTemplate;
    
protected:
    //{{AFX_MSG(CSeqDlg)
    //}}AFX_MSG(CSeqDlg)
    afx_msg LRESULT OnGotoBeginning(WPARAM wparam, LPARAM lparam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
	afx_msg BOOL OnHelpInfo(HELPINFO *pHelpInfo);
    virtual BOOL OnCommand( WPARAM wParam, LPARAM lParam );
    DECLARE_MESSAGE_MAP()
};


#endif //SEQDLG_H
