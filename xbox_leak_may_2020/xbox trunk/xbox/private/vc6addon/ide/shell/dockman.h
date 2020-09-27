///////////////////////////////////////////////////////////////////////////////
// DOCKMAN.H
//

#ifndef __DOCKMAN_H__
#define __DOCKMAN_H__

#ifndef __DOCKWNDS_H__
#include "dockwnds.h"
#endif

#define IDR_DOCKBASE 1000

// Layout version history
// 12: Shipped with 5.0 Beta1
// 13: Incremented when build minibar was renumbered
// 14: TBBS_CUSTOMTEXT
// 15: Incremented after beta 2 ssync, to ensure squashing of beta 2 created registries
// 16: Associate workers
#define LAYOUT_VERSION 16

class CDockManager;

///////////////////////////////////////////////////////////////////////////////
//  CDockWorker
//

#define DEFAULT_STRETCH_WIDTH 100
#define DEFAULT_STRETCH_HEIGHT 100

// struct for placing dockable windows in the docks.
struct DOCKPLACEMENT {
	DOCKPOS dp;	// == dpNil for serialization.
	union
	{
		RECT rectAbsolute;		// Absolute position for serialization.
		DOCKRESTORE drRelative;	// Relative position.
	};
};

// struct for placing docking views in MDI windows.
struct DESKPLACEMENT {
	int nZOrder;
	WINDOWPLACEMENT place;
};

// struct for stretchy dragging.
struct SHELL_DRAGINFO {
	DOCKPOS dp;
	int iRow;
	int cAlongRow;
	int cAlongOther;
};

class CDockWorker : public CObject
{
public:
	// returns the default minimum size, as a point
	static CPoint GetDefMin(void);
	static CPoint s_ptDefMax;

	static LPCSTR s_lpszLayoutSection;

	static CDockWorker* s_pActiveDocker;
	static BOOL s_bLockDocker;

	static void LockWorker(BOOL bLock = TRUE);
	static int GetInsertTolerance(void) { return s_nInsertTolerance; };
	static void SetInsertTolerance(int nInsertTolerance) { s_nInsertTolerance=nInsertTolerance; };

private:
	static int s_nInsertTolerance;

// Constructor.
public:
    CDockWorker(DOCKINIT FAR* lpInit);
    ~CDockWorker();

    BOOL Create(CDockManager* pManager, CWnd* pOwner = NULL,
    	BOOL bForceVisible = FALSE);
	void Reset(DOCKINIT FAR* lpInit);
	void Reset(CDockWorker* pDockerOld);
	void PreTransfer(CDockManager* pManager);
    void SetManager(CDockManager* pManager);	// For defering creation.

public:
	void LoadData();
	void SaveData();

    LONG OnMessage(UINT message, LPARAM lParam);

	void OnChildSetFocus(CWnd* pWndLoseFocus);
	void OnChildKillFocus(CWnd* pWndGetFocus);
	void OnWindowActivate(BOOL bActive, CWnd* pWndOther);
	BOOL OnNcLButtonDown(UINT nHitTest, CPoint pt);
	BOOL OnNcLButtonDblClk(UINT nHitTest, CPoint pt);
    void DoSize(CPoint pt, UINT nHitTest);
    void DoMove(CPoint pt);
    void ToggleFloat();
	void ToggleMDIState();
   	void ShowWindow(BOOL bShow);
	void ActivateView();
	void DestroyWindow(BOOL bInitData = TRUE);
	void Move(DOCKPOS dp, CRect& rect);
	// Associates the docker worker id with this dock worker. If bHost is false, nID is hosting this worker
	// if bHost is TRUE, this worker is hosting nID.
	void Associate(BOOL bHost, UINT nID);

	void SaveStretchSize();
    void SaveFloatPos();
	void SaveMDIPos();
    void SaveDockPos(BOOL bRelative = TRUE);

    BOOL IsVisible();
    BOOL IsAvailable();
	BOOL IsLastAvailable();
	BOOL IsDestroyOnHide();

    void UpdateAvailable(BOOL bShow);
    CSize GetMoveSize(DOCKPOS dp, CSize size);
    CSize GetTrackSize(UINT nHitTest, CSize size);
	BOOL GetTrackRect(CRect& rect, const CRect& rectOld, CPoint pt,
		CSize size, DOCKPOS dp,	DOCKPOS dpOld, BOOL bWasOutside,
		const CRect& rectDesk, const CRect& rectClient,
		BOOL bForce = FALSE, SHELL_DRAGINFO* pDrag = NULL);
	DOCKPOS GetDock();
	void SetDocks(DWORD fInit);
    void GetText(CString& str);
    void SetText(const CString& str);
	CPartView* GetView() const;
	void SetMDIState(BOOL bMDIBound);
	void EndLoad();

	void DeparentWindow(CWnd* pWndNewParent = NULL,
		CDockManager* pManagerNew = NULL);

	MAX_STATE GetMaxState();
	void MaximizeRestore();

protected:
	HWND GetPackageWindow();
    BOOL TrackSize(CPoint pt, UINT nHitTest, CRect& rectFinal);
    BOOL TrackMove(CPoint pt, DOCKPOS& dpFinal, CRect& rectFinal);
    void DrawTrackRect(CDC* pdc,
    	DOCKPOS dp1, const CRect* prect1, BOOL bChildRect1 = TRUE,
        DOCKPOS dp2 = dpNil, const CRect* prect2 = NULL,
        BOOL bChildRect2 = TRUE);

    BOOL FloatWnd(const CRect& rect);
	BOOL DockWnd(DOCKPLACEMENT* pDockPlace = NULL, BOOL bRelative = TRUE);
	BOOL DeskWnd(const CRect& rect = CFrameWnd::rectDefault);

    CSize GetFloatSize();
    void GetFloatRect(CRect& rect);
#if 0
    CPoint GetMaxSize();
    CPoint GetMinSize();
	void GetBoundaryRect(BOOL bLeft, CRect& rect);
#endif

	BOOL FWndLockWorker(CWnd* pWndOther);

//Serialization
    CDockWorker();
    virtual void Serialize(CArchive& ar);

    DECLARE_SERIAL(CDockWorker)

// Attributes
public:
	HGLOBAL m_hglobInit;
	DESKPLACEMENT* m_pDeskPlace;
	DOCKPLACEMENT* m_pDockPlace;	// Relative dock placement.
	DOCKPLACEMENT* m_pDockSerial;	// Absolute dock placement.
    CWnd* m_pWnd;
	UINT m_nIDWnd;
	UINT m_nIDCmd;
	UINT m_nIDPackage;
	UINT m_nIDPacket;
	UINT m_nIDAssociate;

    DOCKTYPE m_dtWnd;

protected:
	BOOL m_bCanHorz:1;
	BOOL m_bCanVert:1;
	BOOL m_bPosition:1;
	BOOL m_bAsk:1;
	BOOL m_bVisible:1;
	BOOL m_bAvailable:1;
	BOOL m_bMDIBound:1;
	BOOL m_bLoading:1;
	BOOL m_bLastAvailable:1;
	BOOL m_bDestroyOnHide:1;
	BOOL m_bTranspose:1;
	UINT m_bHost:1;					// TRUE if the docking window is hosting an embedded window

    DOCKPOS m_dp;
    CDockManager* m_pManager;
	CSize m_sizeLastStretch;
    CRect m_rectFloat;
    CString m_strTitle;

	friend class CDockManager;
};

inline void CDockWorker::LockWorker(BOOL bLock /*=TRUE*/)
{	if (s_pActiveDocker != NULL)
		s_bLockDocker = bLock; }

inline BOOL CDockWorker::IsVisible()
{	
	return m_bAvailable && m_bVisible; 
}


inline BOOL CDockWorker::IsAvailable()
{	return m_bAvailable; }
inline BOOL CDockWorker::IsLastAvailable()
{	return m_bLastAvailable; }
inline BOOL CDockWorker::IsDestroyOnHide()
{	return m_bDestroyOnHide; }
inline DOCKPOS CDockWorker::GetDock()
{	return m_bMDIBound ? dpMDI : m_dp; }
inline void CDockWorker::SetDocks(DWORD fInit)
{	m_bCanVert = (fInit & INIT_CANVERT) != 0;
	m_bCanHorz = (fInit & INIT_CANHORZ) != 0; }
inline void CDockWorker::GetText(CString& str)
{	str = m_strTitle; }
inline void CDockWorker::SetManager(CDockManager* pManager)
{	m_pManager = pManager; }
inline void CDockWorker::EndLoad()
{	delete m_pDockSerial;
	m_pDockSerial = NULL;
	m_bLoading = FALSE; }

//	Worker messages

extern UINT DMM_CHILDSETFOCUS;
extern UINT DMM_CHILDKILLFOCUS;
extern UINT DMM_NCLBUTTONDOWN;
extern UINT DMM_NCLBUTTONDBLCLK;

struct MOUSESTRUCT {
	UINT nHitTest;
	POINT pt;
};

/////////////////////////////////////////////////////////////////////////////
//  CDockManager
//

class CDockManager : public CObject
{
public:
    CDockManager();
    ~CDockManager();

    BOOL Create(CFrameWnd* pFrame, CWnd* pView,
    	CDockManager* pManagerOld = NULL);

    CDockWnd* GetDock(DOCKPOS dp);
	BOOL HasDock(DOCKPOS dp);
    void GetDeskRects(CRect& rectDesk, CRect& rectClient);
    UINT GetCustomID();

    void RegisterInfo(UINT nInfoCount, DOCKINIT FAR* lpdiArray);
	CDockWorker* RegisterToolbar(DOCKINIT FAR* lpdi,
		TOOLBARINIT FAR* lptbi, UINT FAR* lpIDArray);
    CDockWorker* RegisterWnd(HWND hwnd, DOCKINIT FAR* lpInit);
    CDockWorker* RegisterWnd(CWnd* pWnd, DOCKINIT FAR* lpInit);
    void RemoveWnd(CWnd* pWnd);
    void RemoveWnd(UINT nID);
    void RemoveWorker(CDockWorker* pDocker, POSITION pos);
    LONG WorkerMessage(UINT message, HWND hwnd, LPARAM lParam);

    void InsertWorker(CDockWorker* pDocker);
    CDockWorker* WorkerFromID(UINT nID, POSITION* posFinal = NULL);
    CDockWorker* WorkerFromWindow(CWnd* pWnd, POSITION* posFinal = NULL);
	CDockWorker* WorkerFromAutoType(LPCTSTR szAutoType);
    void ArrayOfType(DOCKTYPE dt, CObArray* paWorkers,
    	BOOL bExclude = FALSE, BOOL bAvailable = FALSE, DOCKTYPE dtAnother=dtNil);

	// Hide/Show command handling.
	void MapCmdToWorker(UINT nCmd, UINT nIDWnd);
	void HandleWorkerCmd(UINT nCmd, BOOL bToggle, CCmdUI* pCmdUI = NULL);

    void ShowMenu(CPoint pt);

    void SaveWorkers();

	void EnableManagerInput(BOOL bEnable);
    void ShowManager(BOOL bShow, UPDATE_SHOW update = updNow, BOOL bForceUpdate = FALSE);
	void ShowPalettes(BOOL bShow, BOOL bForceUpdate = FALSE);
	BOOL DocObjectMode(BOOL bShow);
    BOOL IsVisible();
	BOOL IsShowDelayed();
	BOOL IsInDocObjectMode();


// Palette & MDI child functions.
public:
	void AddMDIChild(CMDIChildDock* pWnd);
	void RemoveMDIChild(CMDIChildDock* pWnd,
		CWnd* pWndNewParent = NULL, CDockManager* pManagerNew = NULL);

    void AddPalette(CPaletteWnd* pWnd);
    void RemovePalette(CPaletteWnd* pWnd,
    	CWnd* pWndNewParent = NULL, CDockManager* pManagerNew = NULL);
    void ActivatePalettes(BOOL bActive);

	// This inline function is called by the Idle loop: it needs to remain
	// small and efficient!
	BOOL NeedIdle() {
			if (m_pFrame == NULL || m_pFrame->IsIconic())
				return TRUE;
			return (m_bAvailableWndDirty || m_bDelayedShow);
	}

    void IdleUpdate();
	void IdleUpdateAvailableWnds();
	void SetAvailableWndDirty();

	void SysColorChange();

//Serialization
protected:
    virtual void Serialize(CArchive& ar);

    DECLARE_SERIAL(CDockManager)

// Implementation.
public:
    CFrameWnd* m_pFrame;
	CWnd* m_pView;

protected:
	BOOL m_bVisible:1;
	BOOL m_bDelayedShow:1;
	BOOL m_bAvailableWndDirty:1;
	BOOL m_bInDocObjectMode:1;
    CObList m_listFloat;
	CObList m_listDesk;
    CObList m_listWorker;
    CDockWnd* m_rgpDock[dpCount];
	CMapWordToOb m_mapCmdToWorker;
};

inline BOOL CDockManager::IsVisible()
{	
	return m_bVisible != m_bDelayedShow;
}

inline BOOL CDockManager::IsShowDelayed()
{	return !m_bVisible && m_bDelayedShow;	}
inline BOOL CDockManager::HasDock(DOCKPOS dp)
{   ASSERT(dpFirst <= dp && dp < dpFirst + dpCount);
    return (m_rgpDock[dp - dpFirst] != NULL);	}
inline void CDockManager::SetAvailableWndDirty()
{	m_bAvailableWndDirty = TRUE; }
inline BOOL CDockManager::IsInDocObjectMode()
{	return m_bInDocObjectMode;	}


#endif  // __DOCKMAN_H__
