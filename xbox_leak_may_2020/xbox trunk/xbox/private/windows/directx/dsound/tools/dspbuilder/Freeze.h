/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Freeze.h

Abstract:

	Controls activity - basically a counter

Author:

	Robert Heitkamp (robheit) 08-Nov-2001

Revision History:

	21-Nov-2001 robheit
		Initial Version

--*/

#if !defined(AFX_FREEZE_H__D4FC802F_3801_4967_A5D7_ED754BFDBD2C__INCLUDED_)
#define AFX_FREEZE_H__D4FC802F_3801_4967_A5D7_ED754BFDBD2C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	CFreeze
//------------------------------------------------------------------------------
class CFreeze  
{
public:

	CFreeze(void)				{ m_freeze = 0; };
	virtual ~CFreeze(void)		{ };

	inline int PushFreeze(void)	{ return ++m_freeze; };
	inline int PopFreeze(void)	{ return ((m_freeze > 0) ? --m_freeze : 0); };
	inline BOOL IsFrozen(void)	{ return ((m_freeze > 0) ? TRUE : FALSE); };
	inline void Freeze(void)	{ m_freeze = INT_MAX; };
//	inline void Thaw(void)		{ m_freeze = 0; };

private:

	int	m_freeze;

};

#endif // !defined(AFX_FREEZE_H__D4FC802F_3801_4967_A5D7_ED754BFDBD2C__INCLUDED_)
