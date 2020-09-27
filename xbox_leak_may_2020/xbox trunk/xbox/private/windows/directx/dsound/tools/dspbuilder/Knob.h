/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Knob.h

Abstract:

	Graphical knob control

Author:

	Robert Heitkamp (robheit) 30-Nov-2001

Revision History:

	30-Nov-2001 robheit
		Initial Version

--*/

#ifndef KNOB_H
#define KNOB_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "afxwin.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CModule;

//------------------------------------------------------------------------------
//	CKnob
//------------------------------------------------------------------------------
class CKnob : public CStatic
{
public:

	enum DataType
	{
		TYPE_FLOAT,
		TYPE_DOUBLE,
		TYPE_CHAR,
		TYPE_UNSIGNED_CHAR,
		TYPE_INT,
		TYPE_UNSIGNED_INT,
		TYPE_LONG,
		TYPE_UNSIGNED_LONG
	};

	enum Mode
	{
		DISCRETE,	// Invalid for types FLOAT and DOUBLE
		CONTINUOUS
	};

public:

	CKnob(void);
	CKnob(CModule* pParent, int index, LPCTSTR pName, BOOL bAtomic, DataType type, 
		  double min, double max, double pos, double def, LPCTSTR pFormat, 
		  LPCTSTR pUnits, Mode mode=CONTINUOUS, double increment=0.0);
	virtual ~CKnob(void);

	void Reset(void);
	void Default(void);

	// Inlined methods
	inline double GetPos(void) const { return m_pos; };
//	inline double GetMin(void) const { return m_minPos; };
//	inline double GetMax(void) const { return m_maxPos; };
//	inline double GetDefault(void) const { return m_default; };
	inline DataType GetDataType(void) const { return m_dataType; };
//	inline double GetLastPos(void) const { return m_lastPos; };
	inline void SetLastPos(double pos) { m_lastPos = pos; };
	inline BOOL GetAtomic(void) const { return m_bAtomic; };
	inline int GetIndex(void) const { return m_index; };
	inline void EnableRealtime(BOOL bEnable) { m_bRealtime = bEnable; };
	
protected:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKnob)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CKnob)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSysColorChange();
	afx_msg void OnEditChange();
	afx_msg void OnEditLoseFocus();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

private:
	
	void CreateBitmaps(void);
	double PointToAngle(const CPoint& point) const;
	CPoint AngleToPoint(double angle) const;
	void UpdateEdit(void);
	void UpdatePoint(const CPoint& point);

	inline double PositionToAngle(double pos) const { return (((pos - m_minPos) / (m_maxPos - m_minPos)) * (m_maxAngle - m_minAngle) + m_minAngle); };
	inline double AngleToPosition(double angle) const { return (((angle - m_minAngle) / (m_maxAngle - m_minAngle)) * (m_maxPos - m_minPos) + m_minPos); };
	
private:

	CModule*	m_pParent;
	int			m_index;
	CString		m_name;
	CString		m_units;
	Mode		m_mode;
	double		m_increment;
	CBitmap		m_knobBitmap;
	CBitmap		m_bumpBitmap;
	double		m_angle;
	double		m_radius;
	double		m_minPos;
	double		m_maxPos;
	double		m_pos;
	CRect		m_rect;
	CRect		m_knobRect;
	CRect		m_nameRect;
	CRect		m_unitsRect;
	CRect		m_bumpRect;
	CFont		m_font;
	CPoint		m_knobCenter;
	CPoint		m_bumpCenter;
	BOOL		m_bLeftMouseDown;
	COLORREF	m_backgroundColor;
	COLORREF	m_textColor;
	double		m_minAngle;
	double		m_maxAngle;
	DataType	m_dataType;
	CEdit*		m_pEdit;
	CString		m_format;
	double		m_default;
	double		m_lastPos;
	BOOL		m_bAtomic;
	BOOL		m_bRealtime;
};

#endif

