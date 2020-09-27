/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Graph.h

Abstract:

	Graph of effects

Author:

	Robert Heitkamp (robheit) 08-Nov-2001

Revision History:

	08-Nov-2001 robheit
		Initial Version

--*/
#if !defined GRAPH_H
#define GRAPH_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include <afxtempl.h>
#include "Freeze.h"
#include "WinDspImageBuilder.h"

//------------------------------------------------------------------------------
//	Forward Declarations:
//------------------------------------------------------------------------------
class CModule;
class CChildView;
class CPatchCord;
class CJack;

//------------------------------------------------------------------------------
//	Constants:
//------------------------------------------------------------------------------
static const BYTE	cFileVersion	= 4;

//------------------------------------------------------------------------------
//	CGraph
//------------------------------------------------------------------------------
class CGraph : public CFreeze
{
public:

	CGraph(CChildView* pParent = NULL);
	virtual ~CGraph(void);

	// Useful public methods:
	void Draw(const CRect&, CDC* pDC = NULL);
	void ToggleGrid(void);
	void DeletePatchCord(CPatchCord*);
	void SetRect(const CRect& rect);
	const CRect& GetBounds(void);
	void SetPaths(LPCTSTR pIniPath, LPCTSTR pDspCodePath);
	BOOL IsEmpty(void) const;
	BOOL New(BOOL bAllowCancel=TRUE);
	BOOL Save(LPCTSTR);
	BOOL SelectBuildOptions(void);
	BOOL Open(LPCTSTR);
	int GetDSPCycles(void) const;
	int GetYMemSize(void) const;
	int GetScratchLength(void) const;
	float GetDSPCyclesPercent(void) const;
	float GetYMemSizePercent(void) const;
//	float GetScratchLengthPercent(void) const;
	LPCTSTR GetIniFilename(void) const;
	BOOL SaveImage(void);
	BOOL IsGraphValid(void);
	void MarkPatchCords(BOOL mark);
	LPCTSTR GetBinFilename(void) const;
	LPCTSTR GetHFilename(void) const;
	void SetModuleIndex(LPCTSTR pIniName, DWORD index);
	void TransmitModuleParameters(const CModule* pModule, int index = -1) const;

	static BOOL	ReadString(CFile& file, CString& string);
	static void ShortestPath(const CJack*, const CJack*, CArray<CPatchCord*, CPatchCord*>& path);

	// Utility methods
	CPatchCord* GetPatchCordFromId(DWORD id) const;
	CModule* GetModuleFromId(DWORD id) const;
	CJack* GetJackFromId(DWORD id) const;

	// Inlined methods
	inline const CRect& GetRect(void) const { return m_rect; };
	inline BOOL IsPatched(void) const { return ((m_patchCords.GetSize() != 0) ? TRUE : FALSE); };
	inline BOOL GetShowGrid(void) const { return m_bDrawGrid; };
	inline void ToggleSnapToGrid(void) { m_bSnapToGrid = !m_bSnapToGrid; };
	inline BOOL GetSnapToGrid(void) const { return m_bSnapToGrid; };
	inline LPCTSTR GetIniFilePath(void) const { return (LPCTSTR)m_iniPath; };
	inline LPCTSTR GetDspCodePath(void) const { return (LPCTSTR)m_dspCodePath; };
	inline BOOL AreEffectsLoaded(void) const { return m_bEffectsLoaded; };
	inline BOOL GetChangesMade(void) const { return m_bChangesMade; };
//	inline void SetIncludeXtalk(BOOL bFlag) { m_bIncludeXtalk = bFlag; };
//	inline BOOL GetIncludeXtalk(void) const { return m_bIncludeXtalk; };
	inline BOOL IsImageSaved(void) const { return m_bImageSaved; };
	inline DWORD GetXTalkIndex(void) const { return m_dwXTalkIndex; };
	inline DWORD GetI3DL2Index(void) const { return m_dwI3DL2Index; };
	inline void ChangesMade(void) { m_bChangesMade = TRUE; };
	inline void SetParent(CChildView* pParent) { m_pParent = pParent; };
	inline CChildView* GetParent(void) const { return m_pParent; };

	// Mouse Handlers
	void OnLButtonDown(UINT nFlags, const CPoint& point);
	void OnRButtonDown(UINT nFlags, const CPoint& point);
	void OnLButtonUp(UINT nFlags, const CPoint& point);
	void OnMouseMove(UINT nFlags, const CPoint& point);
	void OnLButtonDblClk(UINT nFlags, const CPoint& point);

private:

	enum PatchEnd
	{
		PATCHEND_NONE,
		PATCHEND_INPUT,
		PATCHEND_OUTPUT,
		PATCHEND_HORIZONTAL,
		PATCHEND_VERTICAL,
		PATCHEND_POINT
	};

	struct PatchLine
	{
		CArray<CPoint, CPoint&>	points;
		PatchEnd				firstPatchEnd;
		PatchEnd				lastPatchEnd;
		CJack*					pFirstJack;
		CPatchCord*				pFirstPatchCord;
		CJack*					pLastJack;
		CPatchCord*				pLastPatchCord;
	};

	// This is for support of version 1 files
	struct Grid
	{
		Grid(void)
		{
			pModule				= NULL;
			bLeft				= FALSE;
			bRight				= FALSE;
			bTop				= FALSE;
			bBottom				= FALSE;
			bJoined				= FALSE;
			pPatchCordLeft		= NULL;
			pPatchCordRight		= NULL;
			pPatchCordTop		= NULL;
			pPatchCordBottom	= NULL;
			index				= -1;
			bEndLeft			= FALSE;
			bEndRight			= FALSE;
			bEndTop				= FALSE;
			bEndBottom			= FALSE;
		}

		CModule*	pModule;
		BOOL		bLeft;
		BOOL		bRight;
		BOOL		bTop;
		BOOL		bBottom;
		BOOL		bJoined;
		int			index;
		CPatchCord*	pPatchCordLeft;
		CPatchCord*	pPatchCordRight;
		CPatchCord*	pPatchCordTop;
		CPatchCord*	pPatchCordBottom;
		BOOL		bEndLeft;
		BOOL		bEndRight;
		BOOL		bEndTop;
		BOOL		bEndBottom;
	};

	enum Direction
	{
		LEFT,
		RIGHT,
		TOP,
		BOTTOM
	};

private:

	BOOL LoadEffects(void);
	void ClearEffects(void);
	void CreateMixbins(void);
	void ClearMixbins(void);
	void SetPaths(void);
	int Compare(LPCTSTR, LPCTSTR) const;
	void InsertModule(const CPoint& point, int index);
	BOOL IsPlacementValid(const CModule* pMovingModule, const CModule* pOriginalModule=NULL) const;
	BOOL IntersectRects(const CRect& rect0, const CRect& rect1) const;
	void CalcUpperLeft(const CModule* pModule);
	CModule* GetModule(const CPoint& point) const;
	void DeleteModule(CModule* pModule);
	void SetMouse(const CPoint& point);
	CPatchCord* GetPatchCord(const CPoint& point) const;
	void DrawPatchLine(void);
	CPatchCord* NewPatchCord(void);
	void UpdatePatchLine(void);
	void ValidateGraph(void);
	void SetTitle(void);
	void EnableMixbinsOnMenu(void);
	BOOL ReadVersion1(CFile& file);
	BOOL ReadVersion2or3or4(CFile& file, BYTE version);
	BOOL BuildEffectChains(LPCTSTR);
	BOOL DoesModuleDependOnModule(const CModule*, const CModule*);
	BOOL CanJackBeRoutedToJack(CJack* pJack0, const CJack* pJack1);
	LPCTSTR GetMixbinName(int mixbin) const;
	void SetInUse(void);
	void TraceAndBuildPatchCord(int x, int y);
	void TraceAndMarkPatchCord(int x, int y, CPatchCord* pPatchCord, Direction dir, 
							   CArray<CPoint, CPoint&>& points);
	void DropModules(void);
	void CancelDrawComplexLine(void);

private:

	CChildView*							m_pParent;
	CRect								m_rect;
	CRect								m_bounds;
	CMenu								m_moduleMenu;
	CMenu								m_patchCordMenu;
	CMenu								m_rootMenu;
	CMenu								m_effectMenu; 
	CMenu								m_inputMixbinMenu; 
	CMenu								m_outputMixbinMenu; 
	CMenu								m_configMenu;
	CMenu								m_inputSpeakerMenu;
	CMenu								m_inputXtalkMenu;
	CMenu								m_outputSpeakerMenu;
	CMenu								m_outputXtalkMenu;
	CArray<CModule*, CModule*&>			m_effects;
	CArray<CModule*, CModule*&>			m_inputMixbins;
	CArray<CModule*, CModule*&>			m_outputMixbins;
	CFont								m_font;
	CModule*							m_pXTalk;
	CModule*							m_pI3DL2;
	CModule*							m_pI3DL2_24K;
	CModule*							m_p2x1Mixer;
	CString								m_iniPath;
	CString								m_dspCodePath;
	BOOL								m_bEffectsLoaded;
	CArray<CPatchCord*, CPatchCord*&>	m_patchCords;
	CArray<CModule*, CModule*&>			m_modules;
	BOOL								m_bChangesMade;
	CPoint								m_mouse;
	CPoint								m_upperLeft;
	CArray<CModule*, CModule*&>			m_moveModules;
	CArray<CModule*, CModule*&>			m_originalModules;
	BOOL								m_bInsertModules;
	BOOL								m_bLeftMouseDown;
	BOOL								m_bDrawGrid;
	BOOL								m_bSnapToGrid;
	int									m_snapSize;
	BOOL								m_bDeleteConfigMenu;
	BOOL								m_bUseOffset;
	CPoint								m_offset;
	PatchLine							m_patchLine;
	BOOL								m_bDrawLine;
	BOOL								m_bDrawComplexLine;
	BOOL								m_bStartDrawLine;
	BOOL								m_bDragPatch;
	CPatchCord*							m_pDragPatch;
	CString								m_filename;
	BOOL								m_bImageSaved;
	CString								m_i3dl2Config;
	BOOL								m_bPatchXtalkToLFE;
	CArray<BOOL, BOOL>					m_inputMixbinInUse;
	CArray<BOOL, BOOL>					m_outputMixbinInUse;
	DWORD								m_dwXTalkIndex;
	DWORD								m_dwI3DL2Index;
	BOOL								m_bIncludeXtalk;
	CWinDspImageBuilder*				m_imageBuilder;
	CArray<int, int&>					m_inUse;
	Grid*								m_pGrid;
	int									m_gridWidth;
	int									m_gridHeight;
	BOOL								m_bDead;
//	BOOL								m_bDspChangesMade;
};

#endif
