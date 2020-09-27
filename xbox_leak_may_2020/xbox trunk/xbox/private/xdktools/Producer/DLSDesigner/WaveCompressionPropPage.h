#if !defined(AFX_WAVECOMPRESSIONPROPPAGE_H__395B7B27_C929_11D1_A876_00C04FA3726E__INCLUDED_)
#define AFX_WAVECOMPRESSIONPROPPAGE_H__395B7B27_C929_11D1_A876_00C04FA3726E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// WaveCompressionPropPage.h : header file
//
#include "resource.h"
#include <mmsystem.h>
#include <mmreg.h>
#include <msacm.h>
#include "WaveCompressionManager.h"

class CWave;
class CWaveCompressionManager;

/////////////////////////////////////////////////////////////////////////////
// CWaveCompressionPropPage dialog

class CWaveCompressionPropPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CWaveCompressionPropPage)

// Construction
public:
	CWaveCompressionPropPage();
	~CWaveCompressionPropPage();

	void SetObject(CWave* pWave);

	void FillAttributesComboBox(const FORMAT_TAG_INF0* pFormatInfo);
	void SetCompressionInfo();
	void InitializeDialogValues();


// Dialog Data
	//{{AFX_DATA(CWaveCompressionPropPage)
	enum { IDD = IDD_WAVE_COMPRESSION_PROP_PAGE };
	CButton	m_NoPrerollCheck;
	CStatic	m_DecompressedStartLabel2;
	CStatic	m_DecompressedStartLabel;
	CEdit	m_DecompressedStartEdit;
	CStatic	m_MSLabel;
	CStatic	m_ReadAheadLabel;
	CButton	m_StreamCheck;
	CSpinButtonCtrl	m_ReadAheadSpin;
	CStatic	m_ReadAheadStatic;
	CEdit	m_ReadAheadEdit;
	CStatic	m_WaveSize;
	CStatic	m_CompressionRatio;
	CComboBox	m_TypeComboBox;
	CComboBox	m_AttributesComboBox;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CWaveCompressionPropPage)
	public:
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CWaveCompressionPropPage)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeCombo1();
	afx_msg void OnCompressed();
	afx_msg void OnSelchangeCombo2();
	afx_msg void OnCloseupCombo1();
	afx_msg void OnCloseupCombo2();
	afx_msg void OnDropdownCombo2();
	afx_msg void OnDropdownCombo1();
	afx_msg void OnStreamCheck();
	afx_msg void OnDeltaposReadaheadSpin(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKillfocusReadaheadEdit();
	afx_msg void OnKillfocusDecompressedStartEdit();
	afx_msg void OnNoprerollCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	void	FillTypeComboBox();
	void	ClearTypeComboBox();
	void	ClearAttributesComboBox();
	bool	Compress();
	void	SetSelectedCompressionType(WORD wFormatTag);
	void	SetSelectedFormat(const WAVEFORMATEX* pWaveFormat);	
	void	EnableStreamingControls(BOOL bEnable = TRUE);
	void	EnableReadAheadControls(BOOL bEnable = TRUE);
	void	EnableDecompressedStartControls();
	void	UpdateReadAheadSamples(DWORD dwReadAheadTime);
	

//Attributes
private:
	static int m_nLastCompressionType;
	static int m_nLastCompressionAttribute;

public:
    CWave*	m_pWave;	 
	bool	m_bTypeCBClosed;	
	bool	m_bAttributesCBClosed;	

	BOOL	m_fNeedToDetach;
	HANDLE	m_hConversionEvent;

	CSpinButtonCtrl m_DecompressedStartSpin;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WAVECOMPRESSIONPROPPAGE_H__395B7B27_C929_11D1_A876_00C04FA3726E__INCLUDED_)
