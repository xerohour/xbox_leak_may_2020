/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	VPMixbin.h

Abstract:

	CMixbin module

Author:

	Robert Heitkamp (robheit) 04-May-2001

Revision History:

	04-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_VPMIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_)
#define AFX_VPMIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_

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
//	CVPMixbin
//------------------------------------------------------------------------------
class CVPMixbin : public CModule  
{
public:

	CVPMixbin(int, CGrid*, LPCTSTR);
	virtual ~CVPMixbin(void);
	virtual void Draw(CDC*);
	void CalculateLabelWidth(CDC&);
	void Resize(CDC&);
	int GetId(void) const;
	virtual BOOL AreInputsPatched(void) const;
	virtual BOOL AreOutputsPatched(void) const;
	virtual void DisconnectOutputs(void);
	
private:

	CCell*	m_patchCell;
	int		m_id;
};

#endif // !defined(AFX_MIXBIN_H__F3DB6724_1CCF_495C_8D90_EAAD3832BDAB__INCLUDED_)
