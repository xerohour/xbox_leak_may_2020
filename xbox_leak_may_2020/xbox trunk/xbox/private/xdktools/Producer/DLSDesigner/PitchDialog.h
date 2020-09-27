//{{AFX_INCLUDES()
#include "adsrenvelope.h"
//}}AFX_INCLUDES
#if !defined(AFX_PITCHDIALOG_H__55AEE3E5_EC78_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_PITCHDIALOG_H__55AEE3E5_EC78_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// PitchDialog.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "DLSLoadSaveUtils.h"

class CDLSStatic;
class CArticulation;
class CCollection;
class CInstrument;

/////////////////////////////////////////////////////////////////////////////
// CPitchDialog dialog

class CPitchDialog : public CDialog, CSliderCollection
{
// Construction
public:
	void UpdateArticulation(CArticulation* pNewArt);
	CPitchDialog(CArticulation* p_Articulation);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPitchDialog)
	enum { IDD = IDD_PITCH_PAGE };
	CSliderCtrl	m_sliderKeyToHold;
	CADSREnvelope	m_PEGADSREnvelope;
	float	m_flAttack;
	float	m_flDecay;
	float	m_flRelease;
	float	m_flSustain;
	float	m_flDelay;
	float	m_flHold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPitchDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPitchDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnADSREDecayTimeChangedPegAdsrenvelopectrl(long NewDecayTime);
	afx_msg void OnADSREAttackTimeChangedPegAdsrenvelopectrl(long NewAttackTime);
	afx_msg void OnADSREReleaseTimeChangedPegAdsrenvelopectrl(long NewReleaseTime);
	afx_msg void OnADSRESustainLevelChangedPegAdsrenvelopectrl(long NewSustainLevel);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusPegDattack();
	afx_msg void OnKillfocusPegDdecay();
	afx_msg void OnKillfocusPegDsustain();
	afx_msg void OnKillfocusPegDrelease();
	afx_msg LRESULT OnValidate(UINT wParam,LONG lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRealDecayChangedPegAdsrenvelopectrl(long lNewRealDecay);
	afx_msg void OnRealReleaseChangedPegAdsrenvelopectrl(long lNewRealRelease);
	afx_msg void OnADSRMouseMoveStart();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusPegDelay();
	afx_msg void OnKillfocusPegHold();
	afx_msg void OnADSREDelayTimeChangedPegAdsrenvelopectrl(long lNewDelayTime);
	afx_msg void OnADSREHoldTimeChangedPegAdsrenvelopectrl(long lNewHoldTime);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnEditChange( UINT nEditControl );
	DECLARE_MESSAGE_MAP()

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);
	
private:
	void ValidateDelayTime();
	void ValidateAttackTime();
	void ValidateHoldTime();
	void ValidateDecayTime();
	void ValidateSustainLevel();
	void ValidateReleaseTime();
	void SelectEditAndBringWindowToTop(CEdit* pEdit);
	void EnableDLS2Controls(BOOL bEnable = TRUE);
	HRESULT SaveUndoState(UINT uStringRes);
	void	CollectStatics();
	CDLSStatic* GetStaticControl(UINT nID);
	void UpdateInstrument();

	/* In order to solve problems with using OnKillFocus handlers to validate and update
		edit controls I am adding a bit field - m_bfTouched. Each bit to be set high in 
		response to EN_CHANGE message from given controls. The enumeration below defines 
		the bit flag for each of the four edit controls in the Pitch page.  I have made 
		IDC_PEG_DATTACK, IDC_PEG_DDECAY, IDC_PEG_DSUSTAIN and IDC_PEG_DRELEASE contigous. 
		in the resource editor. I use a ON_CONTROL_RANGE message map macro to handle 
		EN_CHANGE's. If any more edit controls get added to this dialog template the macro
		and the handler OnEditChange() might have to be modified. OnEditChange() gets one
		parameter which is the control ID of the edit control that changed. from this ID
		I subtract FIRST_CONTROL to come up with the argument to a shift left (<<) of 0x01.
		This creates the bit flag which corresponds to the ones in the enum. The ones in
		enum are used in comparison in the OnKillFocusXXX functions.
	*/
	enum { FIRST_CONTROL = IDC_FIRST_EDIT2VALIDATE, fPEG_DATTACK = 0x01,
		fPEG_DDECAY = 0x02, fPEG_DSUSTAIN = 0x04, fPEG_DRELEASE = 0x08, fPEG_DDELAY = 0x10, fPEG_DHOLD = 0x20};

	CPtrList m_lstStatics;	// List of static controls in the dialog

	CArticulation*			m_pArticulation;
	CCollection*			m_pCollection;
	CInstrument*			m_pInstrument;
	ArticParams*			m_pArticParams;
    MySlider*				m_pmsPEGVel2Attack;
    MySlider*				m_pmsPEGKey2Decay;
	MySlider*				m_pmsPEGKey2Hold;
	MySlider*				m_pmsPEGRange;
	bool					m_bInitingDialog;
	bool					m_bTouchedByProgram;// has edit control value changed by SetWindowText?
	DWORD					m_bfTouched; // bit field meaning "has the edit control really been 
								 // changed?" for OnKillFocus()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PITCHDIALOG_H__55AEE3E5_EC78_11D0_876A_00AA00C08146__INCLUDED_)
