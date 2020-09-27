/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Grid.h

Abstract:

	Grid class for keeping track of CCells

Author:

	Robert Heitkamp (robheit) 03-May-2001

Revision History:

	03-May-2001 robheit
		Initial Version

--*/
#if !defined(AFX_GRID_H__1A32C052_D895_492C_A634_4E04E874FD67__INCLUDED_)
#define AFX_GRID_H__1A32C052_D895_492C_A634_4E04E874FD67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Cell.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CModule;

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
static const int CGridMaxNumEffects = 256;
static const int CGridNumMixbins	= 26;

//------------------------------------------------------------------------------
//	CGrid
//------------------------------------------------------------------------------
class CGrid  
{
public:

	enum Direction
	{
		DIR_NONE,
		DIR_LEFT,
		DIR_RIGHT,
		DIR_TOP,
		DIR_BOTTOM
	};

	enum PassThrough
	{
		PASS_NONE,
		PASS_INOUT,
		PASS_OUTIN
	};

public:

	CGrid(void);
	virtual ~CGrid(void);

	void Create(CWnd*);
	void Cleanup(void);
	void SetCellSize(int);
	int GetCellSize(void) const;
	void Draw(const CRect&, CDC* pDC = NULL);
	CCell* GetCellAlloc(int, int);
	CCell* GetCell(int, int) const;
	BOOL IsPlacementValid(const CModule*, int, int);
	BOOL PlaceModuleIfValid(CModule*, int, int);
	int GetXOffset(void) const;
	int GetYOffset(void) const;
	BOOL IsPatched(const CCell*);
	void Disconnect(CCell*, Direction=DIR_NONE);
	int GetHeight(void) const;
	int GetWidth(void) const;
	BOOL IsOkToScroll(void) const;
	int GetNumEffects(void) const { return m_numEffects; };
	void SaveImage(LPCTSTR);
	BOOL IsGridValid(void);
	BOOL Save(LPCTSTR);
	BOOL New(int w=-1, int h=-1);
	BOOL Open(LPCTSTR);
	BOOL GetChangesMade(void) const { return m_bChangesMade; };
	BOOL IsPatched(void) const { return m_numPatches != 0 ? TRUE : FALSE; };

	// Mouse Handlers
	void OnLButtonDown(UINT, CPoint);
	void OnRButtonDown(UINT, CPoint);
	void OnLButtonUp(UINT, CPoint);
	void OnMouseMove(UINT, CPoint);

private:

	BOOL InBox(int, int, int, int, int, int) const;
	void SetCell(int, int, CCell*);
	void SetCell(int, CCell*);
	inline int GetCellIndex(int x, int y) const { return ((x * m_height) + y); };
	BOOL IsValidIndex(int, int) const;
	void PlaceModule(CModule*, int, int);
	BOOL GetXYFromPoint(const CPoint&, int&, int&) const;
	void DeleteEffect(CModule*);
	void ResetRect(void);
	void AdjustRect(const CCell*);
	BOOL IsCellInRect(const CCell*, const CRect&) const;
	BOOL IsModuleInRect(const CModule*, const CRect&) const;
	void AdjustRect(const CModule*);
	void RemoveModule(const CModule*);
	int GetVPMixbinIndex(const CModule*) const;
	int GetGPMixbinIndex(const CModule*) const;
	void DrawLine(void);
	void DrawPatchCords(int, int, int, int, BOOL);
	void ValidateGrid(void);
	BOOL IsCellConnectedToCell(const CCell*, const CCell*, Direction=CGrid::DIR_NONE, PassThrough=CGrid::PASS_NONE);
	BOOL IsCellInStack(const CCell*) const;
	void MarkPatchCord(CCell*, Direction, CCell::State, CCell::State=CCell::UNKNOWN, BOOL=TRUE);
	void BuildEffectChains(CStdioFile&);
	BOOL IsEffectConnectedToEffect(const CModule*, const CModule*);
	BOOL IsEffectUnconnected(const CModule*) const;
	BOOL GetEffect(const CCell* pCell, BOOL input, BOOL getNext, CModule*& pModule, int& index);
	int GetMixbin(const CCell*, BOOL);
	BOOL DoesEffectDependOnEffect(const CModule*, const CModule*);
	BOOL LoadEffects(void);
	BOOL ReadKeyword(CStdioFile&, CString&);
	BOOL ReadInt(CStdioFile&, int&);
	BOOL ReadString(CStdioFile&, CString&);
	void InsertEffect(const CPoint&, int);

private:
	
	CWnd*		m_pParent;
	CCell**		m_pCells;
	int			m_numCells;
	int			m_width;
	int			m_height;
	int			m_cellSize;
	CModule*	m_pVPMixbins[CGridNumMixbins];
	CModule*	m_pGPMixbins[CGridNumMixbins];
	CModule*	m_pEffects[CGridMaxNumEffects];
	CModule**	m_pEffectList;
	CFont		m_font;
	int			m_numEffects;
	int			m_numEffectsInList;
	BOOL		m_drawGrid;
	int			m_mixbinHeight;
	int			m_mixbinWidth;
	CMenu		m_effectsMenu;
	CMenu		m_patchCordMenu;
	CMenu		m_gridMenu;
	CMenu		m_mixbinMenu; 
	CMenu		m_insertEffectMenu; 
	CRect		m_rect;
	CModule*	m_pMoveModule;
	CModule*	m_pTempModule;
	int			m_moveX;
	int			m_moveY;
	int			m_moveDx;
	int			m_moveDy;
	BOOL		m_leftMouseDown;
	BOOL		m_drawLine;
	BOOL		m_drawLeftRight;
	CCell*		m_stack[512];
	int			m_si;
	BOOL		m_insertEffect;
	CString		m_filename;
	BOOL		m_bChangesMade;
	int			m_numPatches;
};

#endif // !defined(AFX_GRID_H__1A32C052_D895_492C_A634_4E04E874FD67__INCLUDED_)
