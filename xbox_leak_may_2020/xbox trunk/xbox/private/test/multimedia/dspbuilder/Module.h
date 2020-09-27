/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Module.h

Abstract:

	Module composed of multiple cells

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_)
#define AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CCell;
class CGrid;

//------------------------------------------------------------------------------
//	CModule
//------------------------------------------------------------------------------
class CModule
{
public:

	enum State
	{
		OK,
		INVALID,
		INSERT,
		SWAP,
		MOVING
	};

	enum Type 
	{
		VPMIXBIN,
		GPMIXBIN,
		EFFECT
	};


	struct Connection
	{
		CString	name;
		CCell*	pCell;
		CRect	rect;
		int		mixbin; // Negative is a temp mixbin
	};

public:

	CModule(CGrid*, int, Type, LPCTSTR, int, int, 
			LPCTSTR, LPCTSTR, const CStringArray*, 
			const CStringArray*);
	CModule(const CModule&);
	virtual ~CModule(void);

	void Draw(CDC*);
	void SetLocation(int, int);
	int GetWidth(void) const;
	int GetHeight(void) const;
	int GetLeft(void) const;
	int GetRight(void) const;
	int GetTop(void) const;
	int GetBottom(void) const;
	CCell* GetCell(int, int);
	BOOL AreInputsPatched(void) const;
	BOOL AreOutputsPatched(void) const;
	BOOL IsPatched(void) const;
	void Disconnect(void);
	void DisconnectInputs(void);
	void DisconnectOutputs(void);
	void DrawState(CDC*);
	void SetState(State state);
	State GetState(void) const;
	void SetType(Type type);
	Type GetType(void) const;
	int	GetId(void) const;
	const CRect& GetRect(void) const;
	Connection* GetInputs(void) const { return m_inputs; };
	Connection* GetOutputs(void) const { return m_outputs; };
	int GetNumInputs(void) const { return m_numInputs; };
	int GetNumOutputs(void) const { return m_numOutputs; };
	BOOL IsConnected(void) const;
	LPCTSTR GetName(void) const { return (LPCTSTR)m_name; };
	LPCTSTR GetDSPCode(void) const { return (LPCTSTR)m_dspCode; };
	LPCTSTR GetStateFilename(void) const { return (LPCTSTR)m_stateFilename; };

private:

	void CreatePenAndBrush(void);

private:

	int				m_id;
	Type			m_type;
	static CFont*	m_stateFont;
	State			m_state;
	static int		m_stateFontRef;
	CPen			m_pen;
	CBrush			m_brush;
	CGrid*			m_pGrid;
	int				m_width;
	int				m_height;
	CCell**			m_pCells;
	int				m_xLoc;
	int				m_yLoc;
	CRect			m_rect;
	CString			m_name;
	CRect			m_nameRect;
	int				m_numInputs;
	int				m_numOutputs;
	Connection*		m_inputs;
	Connection*		m_outputs;
	CString			m_stats;
	CRect			m_statsRect;
	CString			m_dspCode;
	CString			m_stateFilename;
	int				m_dspCycles;
	int				m_yMemSize;
};

#endif // !defined(AFX_MODULE_H__C19F8972_4C8D_4389_8E1C_D8A84B1C418A__INCLUDED_)
