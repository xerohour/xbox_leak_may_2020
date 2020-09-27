/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Parameter.h

Abstract:

	Effect Parameter

Author:

	Robert Heitkamp (robheit) 29-Nov-2001

Revision History:

	29-Nov-2001 robheit
		Initial Version

--*/
#if !defined(AFX_PARAMETER_H__A010D544_B8C8_4FE9_B1AC_1B9A2C2CBB3C__INCLUDED_)
#define AFX_PARAMETER_H__A010D544_B8C8_4FE9_B1AC_1B9A2C2CBB3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"

//------------------------------------------------------------------------------
//	CParameter 
//------------------------------------------------------------------------------
class CParameter  
{
public:

	CParameter(void);
//	CParameter(const CParameter& param);
//	CParameter(const FX_PARAM_DESCRIPTION& param);
	virtual ~CParameter(void);

	inline LPCTSTR GetName(void) const { return m_name; };
//	inline LPCTSTR GetDescription(void) const { return m_description; };
	inline LPCTSTR GetUnits(void) const { return m_units; };
	inline DWORD GetOffset(void) const { return m_dwOffset; };
	inline DWORD GetDefault(void) const { return m_dwDefault; };
	inline DWORD GetType(void) const { return m_dwType; };
	inline DWORD GetMin(void) const { return m_dwMin; };
	inline DWORD GetMax(void) const { return m_dwMax; };
	inline BOOL GetAtomic(void) const { return m_bAtomic; };
	inline DWORD GetValue(void) const { return m_dwValue; };
	
	inline void SetValue(DWORD value) { m_dwValue = value; };
	inline void SetName(LPCTSTR pName) { m_name = pName; };
//	inline void SetDescription(LPCTSTR pDesc) { m_description = pDesc; };
//	inline void SetUnits(LPCTSTR pUnits) { m_units = pUnits; };
	inline void SetOffset(DWORD dwOffset) { m_dwOffset = dwOffset; };
//	inline void SetDefault(DWORD dwDefault) { m_dwDefault = dwDefault; };
//	inline void SetType(DWORD dwType) { m_dwType = dwType; };
//	inline void SetMin(DWORD dwMin) { m_dwMin = dwMin; };
//	inline void SetMax(DWORD dwMax) { m_dwMax = dwMax; };
//	inline void SetAtmonic(BOOL bAtomic) { m_bAtomic = bAtomic; };

	CParameter& operator = (const CParameter& param);
	CParameter& operator = (const FX_PARAM_DESCRIPTION& param);

private:

	CString	m_name;
	CString	m_description;
	CString	m_units;
	DWORD	m_dwOffset;
	DWORD	m_dwDefault;
	DWORD	m_dwType;
	DWORD	m_dwMin;
	DWORD	m_dwMax;
	BOOL	m_bAtomic;
	DWORD	m_dwValue;
};

#endif // !defined(AFX_PARAMETER_H__A010D544_B8C8_4FE9_B1AC_1B9A2C2CBB3C__INCLUDED_)
