/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Effect.h

Abstract:

	Effect module

Author:

	Robert Heitkamp (robheit) 04-May-2001

Revision History:

	04-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_MIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_)
#define AFX_MIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_

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
class CCell;

//------------------------------------------------------------------------------
//	CEffect
//------------------------------------------------------------------------------
class CEffect : public CModule  
{
public:

	struct Info
	{
		CString			m_name;		// Name of effect
		CStringArray	m_inputs;	// Input names (length == num inputs)
		CStringArray	m_outputs;	// Output names (length == num outputs)
		CString			m_stats;	// Additional statistical info
	};

	CEffect(CGrid*, Info&);
	virtual ~CEffect(void);
	virtual void Draw(CDC*);
	virtual void SetLocation(int, int);
	virtual BOOL AreInputsPatched(void) const;
	virtual BOOL AreOutputsPatched(void) const;
	virtual void DisconnectInputs(void);
	virtual void DisconnectOutputs(void);

private:

	struct Connection
	{
		CString	m_name;
		CCell*	m_pCell;
		CRect	m_rect;
	};


private:

	int			m_numInputs;
	int			m_numOutputs;
	Connection*	m_inputs;
	Connection*	m_outputs;
	CString		m_stats;
	CRect		m_statsRect;
};

#endif // !defined(AFX_MIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_)
