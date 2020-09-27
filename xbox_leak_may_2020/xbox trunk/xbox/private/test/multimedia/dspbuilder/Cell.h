/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Cell.h

Abstract:

	Cell class

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_CELL_H__A0AFBC56_6083_4A7F_97BB_4AD463D3DA19__INCLUDED_)
#define AFX_CELL_H__A0AFBC56_6083_4A7F_97BB_4AD463D3DA19__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Module.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CGrid;

//------------------------------------------------------------------------------
//	CCell
//------------------------------------------------------------------------------
class CCell
{
public:

	enum State
	{
		OK,
		INVALID,
		NO_SIGNAL,
		UNKNOWN
	};

public:

	CCell(CGrid*);
	CCell(CGrid*, int, int);
	virtual ~CCell(void);

	CModule* GetModule(void) const { return m_pModule; };
	void SetModule(CModule* pModule) { m_pModule = pModule; };

	BOOL GetPatchTop(void) const { return m_patchTop; };
	BOOL GetPatchBottom(void) const { return m_patchBottom; };
	BOOL GetPatchLeft(void) const { return m_patchLeft; };
	BOOL GetPatchRight(void) const { return m_patchRight; };

	void SetPatchTop(BOOL);
	void SetPatchBottom(BOOL);
	void SetPatchLeft(BOOL);
	void SetPatchRight(BOOL);

	BOOL IsPatched(void) const;
	void SetJoin(BOOL join) { m_join = join; };
	BOOL IsJoined(void) const { return m_join; };
	int GetPatchCount(void) const { return m_patchCount; };

	void SetState(State s) { m_state = s; };
	void SetStateV(State s) { m_stateV = s; };

	State GetState(void) const { return m_state; };
	State GetStateV(void) const { return m_stateV; };

	void SetLocation(int, int);
	int GetXLoc(void) const { return m_xLoc; };
	int GetYLoc(void) const { return m_yLoc; };
	virtual void Draw(CDC*);

private:

	void SetPenForState(CDC*, State);

private:

	static CPen	m_invalidPen;
	static CPen m_noSignelPen;
	static CPen m_unknownPen;
	CGrid*		m_pGrid;
	int			m_xLoc;
	int			m_yLoc;
	BOOL		m_patchTop;
	BOOL		m_patchBottom;
	BOOL		m_patchLeft;
	BOOL		m_patchRight;
	CModule*	m_pModule;
	int			m_left;
	int			m_right;
	int			m_top;
	int			m_bottom;
	int			m_mx;
	int			m_my;
	BOOL		m_join;
	int			m_patchCount;
	State		m_state;	// Handles all states unless m_patchCount == 4
	State		m_stateV;
};

#endif // !defined(AFX_CELL_H__A0AFBC56_6083_4A7F_97BB_4AD463D3DA19__INCLUDED_)
