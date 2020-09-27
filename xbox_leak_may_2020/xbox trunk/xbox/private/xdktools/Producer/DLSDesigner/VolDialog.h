//{{AFX_INCLUDES()
#include "adsrenvelope.h"
//}}AFX_INCLUDES
#if !defined(AFX_VOLDIALOG_H__55AEE3E3_EC78_11D0_876A_00AA00C08146__INCLUDED_)
#define AFX_VOLDIALOG_H__55AEE3E3_EC78_11D0_876A_00AA00C08146__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// VolDialog.h : header file
//

#include "resource.h"
#include "myslider.h"
#include "Collection.h"
#include "Instrument.h"
#include "Region.h"
#include "DLSLoadSaveUtils.h"

class CDLSStatic;
class CArticulation;

/////////////////////////////////////////////////////////////////////////////
// CVolDialog dialog

class CVolDialog : public CDialog, CSliderCollection
{

// Construction
public:
	CVolDialog(CArticulation* pArticulation);

// Dialog Data
	//{{AFX_DATA(CVolDialog)
	enum { IDD = IDD_VOLUME_PAGE };
	CADSREnvelope	m_VEGASDREnvelope;
	float	m_flAttack;
	float	m_flDecay;
	float	m_flRelease;
	float	m_flSustain;
	float	m_flDelay;
	float	m_flHold;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVolDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

	// CSliderCollection overrides
	virtual bool OnSliderUpdate(MySlider *pms, DWORD dwmscupdf);
	
// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CVolDialog)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnADSRESustainLevelChangedVegAdsrenvelopectrl(long NewSustainLevel);
	afx_msg void OnADSREReleaseTimeChangedVegAdsrenvelopectrl(long NewReleaseTime);
	afx_msg void OnADSREAttackTimeChangedVegAdsrenvelopectrl(long NewAttackTime);
	afx_msg void OnADSREDecayTimeChangedVegAdsrenvelopectrl(long NewDecayTime);
	virtual BOOL OnInitDialog();
	afx_msg void OnKillfocusVegDattack();
	afx_msg void OnKillfocusVegDdecay();
	afx_msg void OnKillfocusVegDrelease();
	afx_msg void OnKillfocusVegDsustain();
	afx_msg LRESULT OnValidate(UINT wParam,LONG lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnRealDecayChangedVegAdsrenvelopectrl(long lNewRealDecay);
	afx_msg void OnRealReleaseChangedVegAdsrenvelopectrl(long lNewRealRelease);
	afx_msg void OnADSRMouseMoveStart();
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnKillfocusVegDelay();
	afx_msg void OnKillfocusVegHold();
	afx_msg void OnADSREDelayTimeChangedVegAdsrenvelopectrl(long lNewDelayTime);
	afx_msg void OnADSREHoldTimeChangedVegAdsrenvelopectrl(long lNewHoldTime);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	afx_msg void OnEditChange( UINT nEditControl );
	DECLARE_MESSAGE_MAP()

public:	// J3 make private
	void UpdateArticulation(CArticulation* pNewArt);
	CArticulation*	m_pArticulation;
private:
	void ValidateDelayTime();
	void ValidateAttackTime();
	void ValidateHoldTime();
	void ValidateDecayTime();
	void ValidateSustainLevel();
	void ValidateReleaseTime();
	void SelectEditAndBringWindowToTop(CEdit* pEdit);
	
	void EnableDLS2Controls(BOOL bEnable = TRUE);

private:
	HRESULT SaveUndoState(UINT uStringRes);
	void	CollectStatics();
	CDLSStatic* GetStaticControl(UINT nID);
	void UpdateInstrument();
	
	/* In order to solve problems with using OnKillFocus handlers to validate and update
		edit controls I am adding a bit field - m_bfTouched. Each bit to be set high in 
		response to EN_CHANGE message from given controls. The enumeration below defines 
		the bit flag for each of the four edit controls in the Volume page.  I have made 
		IDC_VEG_DATTACK, IDC_VEG_DDECAY, IDC_VEG_DSUSTAIN and IDC_VEG_DRELEASE contigous
		in the resource editor. I use a ON_CONTROL_RANGE message map macro to handle 
		EN_CHANGE's. If any more edit controls get added to this dialog template the macro
		and the handler OnEditChange() might have to be modified. OnEditChange() gets one
		parameter which is the control ID of the edit control that changed. from this ID
		I subtract FIRST_CONTROL to come up with the argument to a shift left (<<) of 0x01.
		This creates the bit flag which corresponds to the ones in the enum. The ones in
		enum are used in comparison in the OnKillFocusXXX functions.
	*/
	enum { FIRST_CONTROL = IDC_FIRST_EDIT2VALIDATE, fVEG_DATTACK = 0x01,
		fVEG_DDECAY = 0x02, fVEG_DSUSTAIN = 0x04, fVEG_DRELEASE = 0x08, fVEG_DDELAY = 0x10, fVEG_DHOLD = 0x20};

	CPtrList m_lstStatics;	// List of static controls in the dialog

	CCollection*			m_pCollection;	
	CInstrument*			m_pInstrument;
	ArticParams*			m_pArticParams;
    MySlider*				m_pmsMSCDefaultPan;
    MySlider*				m_pmsVEGVel2Attack;
	MySlider*				m_pmsVEGKey2Hold;
    MySlider*				m_pmsVEGKey2Decay;
	MySlider*				m_pmsVEGShutdownTime;
	bool					m_bInitingDialog;
	bool					m_bTouchedByProgram;// has edit control value changed by SetWindowText?
	DWORD					m_bfTouched; // bit field meaning "has the edit control really been 
								 // changed?" for OnKillFocus()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VOLDIALOG_H__55AEE3E3_EC78_11D0_876A_00AA00C08146__INCLUDED_)
