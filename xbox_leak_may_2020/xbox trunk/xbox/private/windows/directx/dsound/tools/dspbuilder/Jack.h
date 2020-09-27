/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Jack.h

Abstract:

	A jack to connect a patch cord to a module

Author:

	Robert Heitkamp (robheit) 08-Nov-2001

Revision History:

	22-Nov-2001 robheit
		Initial Version

--*/

#if !defined(AFX_JACK_H__D0FEF4C9_4354_40B5_AB83_7DD99C0E9B8C__INCLUDED_)
#define AFX_JACK_H__D0FEF4C9_4354_40B5_AB83_7DD99C0E9B8C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "dspbuilder.h"
#include "PatchCord.h"
#include "Unique.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CModule;

//------------------------------------------------------------------------------
//	CJack:
//------------------------------------------------------------------------------
class CJack : public CUnique
{
public:

	CJack(CModule* pModule=NULL);
//	CJack(const CJack& jack);
	virtual ~CJack(void);

	void Draw(CDC*);
	CPatchCord* GetPatchCord(void) const;
	void SetRect(const CRect& rect);
	void SetInput(BOOL bInput);
	void Connect(CPatchCord* pPatchCord, BOOL bEnd);
	virtual void Write(CFile&); // throw(CFileException);
	void FixPointers(const CGraph* pGraph);
	virtual BOOL Read(CFile& file, BYTE version);
	void GetConnections(CArray<CJack*, CJack*>& list, BOOL bClearList = TRUE, 
						BOOL bClearPatchCords = TRUE);
	void GetAllConnections(CArray<CJack*, CJack*>& list);
						
	void Invalidate(void);

	inline BOOL IsPatched(void) const { return m_pPatchCord->IsPatched(); };
	inline CModule* GetModule(void) const { return m_pModule; };
//	inline void SetModule(CModule* pModule) { m_pModule = pModule; };
	inline void SetName(LPCTSTR pName) { m_name = pName; };
//	inline const CString& GetName(void) const { return m_name; };
//	inline const CRect& GetRect(void) const { return m_rect; };
	inline void SetMixbin(int mixbin) { m_mixbin = mixbin; };
	inline int GetMixbin(void) const { return m_mixbin; };
	
	inline BOOL GetInput(void) const { return m_bInput; };
//	inline void Disconnect(void) { m_pPatchCord->ClearPatch(); };
	inline int GetY(void) const { return m_line[0].y; };
	inline CPatchCord* GetOwnPatchCord(void) const { return m_pPatchCord; };
	inline void SetOverwrite(BOOL bFlag) { m_bOverwrite = bFlag; };
	inline BOOL GetOverwrite(void) const { return m_bOverwrite; };

	// Operators
	CJack& operator = (const CJack&);

private:

	void SetLine(void);

private:

	CModule*	m_pModule;
	CString		m_name;
	CPatchCord*	m_pPatchCord;
	CRect		m_rect;
	int			m_mixbin; // Negative is a temp mixbin
	BOOL		m_bInput;
	CPoint		m_line[2];
	UINT		m_textFlag;
	BOOL		m_bBadData;
	BOOL		m_bOverwrite;
};

#endif // !defined(AFX_JACK_H__D0FEF4C9_4354_40B5_AB83_7DD99C0E9B8C__INCLUDED_)
