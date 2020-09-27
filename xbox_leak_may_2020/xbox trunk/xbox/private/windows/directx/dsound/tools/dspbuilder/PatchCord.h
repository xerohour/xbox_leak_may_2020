/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	PatchCord.h

Abstract:

	Patch Cord class. This object connects modules

Author:

	Robert Heitkamp (robheit) 05-Oct-2001


Revision History:

	05-Oct-2001 robheit
		Initial Version

--*/
#if !defined(AFX_PATCHCORD_H__94E63C94_9F86_4138_B073_A64F83BA8AE5__INCLUDED_)
#define AFX_PATCHCORD_H__94E63C94_9F86_4138_B073_A64F83BA8AE5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxtempl.h>
#include "Unique.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CModule;
class CGraph;
class CJack;

//------------------------------------------------------------------------------
//	CPatchCord
//------------------------------------------------------------------------------
class CPatchCord : public CUnique
{
public:

	struct PatchNode
	{
		CPatchCord*	pPatchCord;
		BOOL		bEnd;
		PatchNode*	pNext;
		PatchNode*	pPrev;
	};

	enum PointOn
	{
		POINTON_NONE,
		POINTON_HORIZONTAL,
		POINTON_VERTICAL,
		POINTON_POINT
	};

public:

	CPatchCord(CJack* pJack=NULL);
	virtual ~CPatchCord(void);

	void Draw(CDC* pDC);
	void DrawXOR(CDC* pDC);
	CPatchCord* Split(const CPoint& point);
	void Join(CPatchCord*& pPatchCord);
	CPatchCord* Connect(CPatchCord*& pPatchCord, const CPoint& point, BOOL bEnd);
	void SetPoints(const CArray<CPoint, CPoint&>& points);
	void AddPatchCord(CPatchCord* pPatchCord, const CPoint& point);
	void AddPatchCord(CPatchCord* pPatchCord, BOOL bEnd);
	void RemovePatchCord(CPatchCord* pPatchCord);
	void SwapPatchCord(CPatchCord* pOld, CPatchCord* pNew);
	BOOL IsPatched(const CPatchCord* pPatchCord= NULL) const;
	BOOL IsPatchedAtEnds(void) const;
	BOOL IsPatchedToModule(const CModule*) const;
	BOOL IsPatchedToEffect(const CModule*) const;
	BOOL IntersectRect(const CRect& rect) const;
	BOOL IsPointOnLine(const CPoint& point, int epsilon=0);
	void ClearPatch(void);
	void Write(CFile& file); // throw(CFileException);
	void FixPointers(const CGraph* pGraph);
	virtual BOOL Read(CFile& file, BYTE version);
	void GetConnections(CArray<CJack*, CJack*>& list);
	void GetPatchCords(CArray<CPatchCord*, CPatchCord*>& list);
	void Invalidate(void);
	void SetOverwriteEnable(void);
	RECT SetOverwrite(BOOL bFlag);
	void CalcRect(void);

	inline void Mark(BOOL bMark) { m_bMark = bMark; };
//	inline BOOL GetMark(void) const { return m_bMark; };
	inline CJack* GetJack(void) const { return m_pJack; };
//	inline void SetJack(CJack* pJack) { m_pJack = pJack; };
	inline void SetInvalid(BOOL bFlag) { m_bInvalid = bFlag; };
	inline BOOL IsInvalid(void) const { return m_bInvalid; };
	inline CRect GetRect(void) const { return m_rect; };
	inline CArray<CPoint, CPoint&>& GetPoints(void) { return m_points; };
	inline PatchNode* GetPatchNode(void) const { return m_pPatchNode; };
	inline PointOn GetLastPointOn(void) const { return m_lastPointOn; };
	inline const CPoint& GetLastPoint(void) const { return m_lastPoint; };
	inline void Highlight(BOOL bFlag) { m_bHighlight = bFlag; };
	inline BOOL GetOverwrite(void) const { return m_bOverwrite; };
	inline BOOL GetOverwriteEnable(void) const { return m_bOverwriteEnable; };
	inline void SetOverwriteEnable(BOOL bFlag) { m_bOverwriteEnable = bFlag; };

private:

	// Saved to file
	BOOL						m_bInvalid;		// TRUE if invalid
	CRect						m_rect;			// Bounding rect
	CJack*						m_pJack;		// Only the id is saved
	BOOL						m_bOverwrite;
	BOOL						m_bOverwriteEnable;
	CArray<CPoint, CPoint&>		m_points;		// Points in line
	PatchNode*					m_pPatchNode;	// Connection(s)

	// Not saved to file
	PointOn						m_lastPointOn;	// Last point location
	CPoint						m_lastPoint;	// Last point
	BOOL						m_bBadData;		// TRUE if the pointers are bad
	BOOL						m_bMark;
	BOOL						m_bHighlight;
};

#endif // !defined(AFX_PATCHCORD_H__94E63C94_9F86_4138_B073_A64F83BA8AE5__INCLUDED_)
