///////////////////////////////////////////////////////////////////////////////
// 
// dbgenc.cpp
//
// Support for edit-and-continue functionality
//
///////////////////////////////////////////////////////////////////////////////
 			   		   
#include "stdafx.h"
#pragma hdrstop		  

#include "gmap.h"
#include "dbgenc.h"
#include "utilbld_.h"
#define assert ASSERT
#if defined(_DEBUG)
#define Debug(x)    x
#else
#define Debug(x)
#endif

#include "ref.h"
#include "simparray.h"
#include "enc.h"
#include "comenvi.h"
#include "enclog.h"
#include "mrengine.h"

// define max number of file to compile during and ENC session
#define MAX_ENC_COMPILANDS 150

// Max length of spawned commands (current limit is 32K)
#define MAX_SPAWN_CMD_LEN 32768

using namespace enc;

extern void FAR PASCAL CLFreeFmeData( LPV );
extern void PASCAL CLSetProcAddr ( LPFME );
extern char * PASCAL CLGetProcFromHfme ( HFME, char *, int, BOOL );

static bool g_fSetTmpBp = true;
static ISymbolHandler *pSymbolHandler;
static Enc* (*pTheEnc)();
static CStringArray rgProcNames;

static bool g_fEncLoaded = false;
// the following can only be used after
// calling InitENCProxy
#define theEnc (*((*pTheEnc)()))

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

static LPCTSTR ENCCreateTempName(LPCTSTR lszObjName);
static LPCTSTR szCanonicalPath(CString& str);
BOOL MyMoveFileEx(LPCTSTR lpszExisting, LPCTSTR	lpszNew, DWORD fdwFlags);
static void ApplyMUList(RefMUList & rlist);
static void LoadAllIdbFiles();
static void OutputMessage(LPCTSTR lsz, BOOL fToolError = FALSE);
static void OutputMessage(int idsMsg);
static bool fCanDefer(HRESULT hResult);
static void EncClearOutput();
static DWORD ImageBase(HEXE hexe);

HLLI	hlliFmeEnc = NULL;

bool
IsEncLoaded(
	)
{
	return g_fEncLoaded;
}

void
SetEncLoaded(
	bool	fEncLoaded
	)
{
	g_fEncLoaded = fEncLoaded;

	//
	// Maybe send to output that ENC is not loaded.
	//
}


// a line hint for the enc engine using the TraceInfo data
class LineHint 
{
	ILINE m_iLineHint;
public:
	ILINE Get() { return m_iLineHint; }
	// Set() needs to be called before the edit has been applied,
	// since it uses old line information that is no longer available
	// after the edit
	void Set()
	{
		char SrcName[_MAX_PATH];
		ILINE SrcLine;
		UOFFSET SrcDelta;
		HSF hsf ;
		m_iLineHint = 0;
		if (GetCurrentSource(SrcName, sizeof(SrcName), &SrcLine, &SrcDelta, &hsf) &&
			( gpISrc->GetDocFromHsf(hsf) == TraceInfo.pDoc || 
			  gpISrc->GetDocFromFileName(SrcName) == TraceInfo.pDoc )) {
			m_iLineHint =  TraceInfo.CurTraceLine;
		}
	}
	void Invalidate() { m_iLineHint = 0; }
};

static LineHint currentLineHint;

// 
// Process, Thread, StackFrame
// Classes used for live frame editing
//

class Process;
class Thread;

//
// class StackFrame
// Used for updating a single live frame on the call stack
// 
class StackFrame {
	HPID	m_hpid;			// Process ID
	HTHD	m_hthd;			// Thread handle
	HTID	m_hvtid;		// Virtual Thread ID
	int		m_iFrame;		// Frame index (frame #0 is the current TOS)
	HRESULT m_hResult;		// Result returned by UpdateIpToEditIp
	ADDR	m_addrIpStack;	// Stack address that contains the frame's IP
	ADDR	m_addrIp;		// New value of frame's IP
	ADDR	m_addrSp;		// New value of SP (TOS only, TMP bp handling only)
	RefMUList m_rlist;		// Array of memory patches returned by ENC engine
	int		m_iName;		// Name index into rgProcNames;

public:
	StackFrame() {}
	StackFrame(int iFrame, HPID hpid, HTHD hthd, HTID hvtid, PADDR paddrIpStack): 
	  m_iFrame(iFrame), m_hpid(hpid), m_hthd(hthd), m_hvtid(hvtid), m_hResult(0), m_iName(-1)
	{
		m_addrIpStack = *paddrIpStack;
		memset (&m_addrIp, 0, sizeof(m_addrIp));
		memset (&m_addrSp, 0, sizeof(m_addrSp));
	}
	StackFrame& operator = (const StackFrame& sf) 
	{
		memcpy(&m_addrIpStack, &sf.m_addrIpStack, sizeof(m_addrIpStack));
		memcpy(&m_addrIp, &sf.m_addrIp, sizeof(m_addrIp));
		memcpy(&m_addrSp, &sf.m_addrSp, sizeof(m_addrSp));
		m_hpid		= sf.m_hpid;	
		m_hthd		= sf.m_hthd;	
		m_hvtid		= sf.m_hvtid;	
		m_iFrame	= sf.m_iFrame;
		m_hResult	= sf.m_hResult;
		m_rlist		= sf.m_rlist;	
		m_iName		= sf.m_iName;
		return *this;
	}

	// Gather info on what needs to be changed by the edit
	bool PreEdit(bool fHandlingTmpBp = false);

	// Commit edit to memory image
	void CommitEdit(bool fMoveIP, bool fSetTmpBp);

	// Called whenever we hit a temporary breakpoint after a non-updated
	// frame executing old code reaches the top of stack. 
	void HandleTmpBpHit();

	bool IsTopOfStack() const { return m_iFrame == 0; }
	HRESULT HResult() const { return m_hResult; }
	bool MovedIP() const { 
		return (m_hResult == S_IPPOSCHANGE || m_hResult == S_IPPOSCHANGEEH);
	}
	int Index() const {return m_iFrame;}
	LPCSTR SzName() const ;
	unsigned long Tid() const {return TidFromHthd(m_hthd);} 
	bool fTouchedByLastEdit() { 
		return theEnc.FEditReplacesAddr( &m_addrIp, true ); 
	}

private:
	bool GetAddrEH(PADDR paddr);
	bool GetAddrRet(PADDR paddr);
	HPID HPid() const {return m_hpid;}
	HTID HTid() const {return HtidFromHthd(m_hthd);}
	HTHD Hthd() const { return m_hthd; }
	void GetProcName();
	bool ValidateIpLocation();
};

typedef CList<StackFrame, StackFrame&> CStackFrameList;

// 
// class Thread
// Used for updating all the live frames of a thread 
//
class Thread {
	HPID	m_hpid;			
	HTHD	m_hthd;			
	bool	m_fMovedIP;
	CStackFrameList m_sfList;	// list of affected stack frames
public:
	Thread(){}
	Thread(HPID hpid, HTHD hthd): m_hpid(hpid), m_hthd(hthd) {};
	Thread& operator = (const Thread& thd) 
	{
		m_hpid = thd.m_hpid;
		m_hthd = thd.m_hthd;
		m_sfList.AddTail((CList<StackFrame, StackFrame&> *)&thd.m_sfList);
		return *this;
	}
	bool PreEdit();
	void CommitEdit(bool fMoveIP, bool fSetTmpBp);
	bool MovedIP() {return m_fMovedIP;}

	HTID HTid() { return HtidFromHthd(m_hthd);}
	HTHD Hthd() { return m_hthd; }
	HPID HPid() { return m_hpid; }
	unsigned long Tid() {return TidFromHthd(m_hthd);} 
	CStackFrameList & StackFrameList() { return m_sfList; } 

private:
	bool IsCurrent() { return hthdCurr == m_hthd; }
	bool IsDead() 
	{
		LPTHD lpthd = (LPTHD) LLLpvFromHlle (m_hthd);
		bool fDead = !!(lpthd->tdf & tdfDead);
		UnlockHlle (m_hthd);
		return fDead;
	}
	bool IsInExceptionState();
	bool GetTibStackBase(UOFF32 *poff32);
};

// 
// class Process
// used for updating all the threads of a given process
// 
class Process {
	HPID	m_hpid;
	CList<Thread, Thread&> m_thdList;	// list of threads
	bool m_fCanceled;		// Edit was canceled by the user
	bool m_fMovedIP;		// IP was moved (S_IPPOSCHANGE or S_IPPOSCHANGEEH)
public:
	Process(HPID hpid): m_hpid(hpid), m_fCanceled(false), m_fMovedIP(false) {}
	bool StackEdit();
	void CommitEdit();
	HPID HPid() {return m_hpid;}
	bool IsCanceled() {return m_fCanceled;}
	bool MovedIP() {return m_fMovedIP;}
};


//
// Edit and Continue Stack Edit Dialog
// 

class CStackEditDlg : public C3dDialog
{

  private:

	CStackFrameList&	m_sfList;

  public:

	enum { IDD = IDD_STACK_EDIT };

  public:	// operations

	CStackEditDlg(
		CStackFrameList& sfList,
		CWnd*	pParent = NULL
		);
	
	~CStackEditDlg(
		);
		

  protected:
  
	virtual BOOL
	OnInitDialog(
		);
		
	virtual void
	OnCancel(
		);

	virtual BOOL
	OnNotify(
		WPARAM		wParam,
		LPARAM		lParam,
		LRESULT*	lResult
		);

	BOOL
	OnLvnColumnClick(
		NM_LISTVIEW*	ListData,
		LRESULT*		lResult
		);

	BOOL
	OnLvnItemChanged(
		NMHDR*	Data,
		LRESULT*	lResult
		);

	void
	FillFrameList(
		);
	
	void
	SetReasonText(
		int nItem
		);

	void 
	OnYesClicked(
		);

	void
	OnNoClicked(
		);

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CStackEditDlg, C3dDialog)
	ON_BN_CLICKED(IDYES, OnYesClicked)
	ON_BN_CLICKED(IDNO, OnNoClicked)	
END_MESSAGE_MAP()

enum {
	FRAME_INDEX_NAME	= 0,
	FRAME_INDEX_THREAD	= 1,
	FRAME_INDEX_FRAME	= 2,
};


CStackEditDlg::CStackEditDlg(
	CStackFrameList& sfList,
	CWnd*	pParent // = NULL
	)
	: C3dDialog (CStackEditDlg::IDD, pParent), m_sfList(sfList)
{
}

CStackEditDlg::~CStackEditDlg(
	)
{
}

BOOL
CStackEditDlg::OnInitDialog(
	)
{
	C3dDialog::OnInitDialog ();

	CListCtrl*	frameList = (CListCtrl*)GetDlgItem (IDC_FRAME_LIST);
    CString str;

    str.LoadString(IDS_Enc_Function);
	frameList->InsertColumn (FRAME_INDEX_NAME, str, LVCFMT_LEFT, 170);

    str.LoadString(IDS_Enc_Thread);
	frameList->InsertColumn (FRAME_INDEX_THREAD, str, LVCFMT_CENTER, 75);

    str.LoadString(IDS_Enc_Call);
	frameList->InsertColumn (FRAME_INDEX_FRAME, str, LVCFMT_CENTER, 90);

	FillFrameList ();
	return TRUE;
}

void
CStackEditDlg::OnCancel(
	)
{
	EndDialog(IDNO);
}


void 
CStackEditDlg::OnYesClicked()
{
	EndDialog(IDYES);
}

void 
CStackEditDlg::OnNoClicked()
{
	EndDialog(IDNO);
}

int CALLBACK
SfCompareFunction(
	LPARAM	lParam1,
	LPARAM	lParam2,
	LPARAM	nSortField
	)
{
	StackFrame*	sf1 = (StackFrame*)lParam1;
	StackFrame*	sf2 = (StackFrame*)lParam2;
	
	
	switch (nSortField)
	{
		case FRAME_INDEX_THREAD:
			return ((int)sf1->Tid() - (int)sf2->Tid());

		case FRAME_INDEX_FRAME:
			return ((int)sf1->Index() - (int)sf2->Index());

		case FRAME_INDEX_NAME:
		{
			LPCSTR pchName = sf2->SzName();
			LPSTR szName = (LPSTR) _alloca(strlen(pchName) + 1);
			strcpy(szName, pchName);
			return lstrcmpi (sf1->SzName(), szName);
		}

		default:
			ASSERT (FALSE);
	}

	return 0;
}

BOOL
CStackEditDlg::OnNotify(
	WPARAM		wParam,
	LPARAM		lParam,
	LRESULT*	lResult
	)
{
	switch ( ((LPNMHDR)lParam)->code )
	{
		case LVN_COLUMNCLICK:
			return OnLvnColumnClick ((NM_LISTVIEW*) lParam, lResult);
		case LVN_ITEMCHANGED:
			return OnLvnItemChanged((NMHDR*) lParam, lResult);
	}
	return 0;
}

BOOL
CStackEditDlg::OnLvnColumnClick(
	NM_LISTVIEW*	ListData,
	LRESULT*		lResult
	)
{
	
	CListCtrl*	frameList = (CListCtrl*) GetDlgItem (IDC_FRAME_LIST);
	ULONG		nSortField;

	ASSERT (frameList);

	//
	//	this is a sort request
	
	frameList->SortItems (SfCompareFunction, ListData->iSubItem);

	return TRUE;
}

BOOL
CStackEditDlg::OnLvnItemChanged(
	NMHDR*		Data,
	LRESULT*	lResult
	)
{
	int			nItem;
	CListCtrl*	frameList = (CListCtrl*) GetDlgItem (IDC_FRAME_LIST);
	CString		str;

	ASSERT (frameList);

	nItem = frameList->GetNextItem (-1, LVNI_SELECTED);

	SetReasonText (nItem);

	return TRUE;
}

void
CStackEditDlg::SetReasonText(
	int nItem
	)
{
	CListCtrl*	frameList = (CListCtrl*) GetDlgItem (IDC_FRAME_LIST);
	CStatic*	stReason = (CStatic *) GetDlgItem (IDC_ReasonText);
	ASSERT (stReason);
	ASSERT (frameList);
	if (nItem >= 0)
	{
		StackFrame * psf = (StackFrame *) frameList->GetItemData (nItem);
		CString str;
		SeErrorText(str, psf->HResult());
		stReason->SetWindowText(str);
	}
}

void
CStackEditDlg::FillFrameList(
	)
{
	TCHAR	szTid [16];
	TCHAR	szIndex [16];
	int		nItem;
	CListCtrl*	frameList = (CListCtrl*)GetDlgItem (IDC_FRAME_LIST);
	POSITION pos = m_sfList.GetHeadPosition();
	while (pos)
	{
		const StackFrame& sf = m_sfList.GetNext(pos);
		_ultoa (sf.Tid(), szTid, 16);
		_ultoa (sf.Index(), szIndex, 10);
			
		nItem = frameList->InsertItem (0, sf.SzName());
		frameList->SetItemText (nItem, 1, szTid);
		frameList->SetItemText (nItem, 2, szIndex);
		frameList->SetItemData (nItem, (LPARAM) &sf);
	}

	// Sort on frame index, then on thread id
	frameList->SortItems (SfCompareFunction, FRAME_INDEX_FRAME);
	frameList->SortItems (SfCompareFunction, FRAME_INDEX_THREAD);

	frameList->SetItemState (0, LVNI_SELECTED | LVIS_FOCUSED, LVNI_SELECTED | LVIS_FOCUSED);
	SetReasonText(0);
	frameList->Update(0);
}


bool Process::StackEdit()
{
	HLLI llthd = LlthdFromHprc (hprcCurr);
	bool retval = true;

	HTHD hthd = 0;
	CStackFrameList sfListAll;
	while ((hthd = LLHlleFindNext (llthd, hthd)) != 0)
	{
		Thread thd(m_hpid, hthd);
		if (thd.PreEdit())
			m_thdList.AddTail(thd);

		CStackFrameList & sfList = thd.StackFrameList();
		POSITION pos = sfList.GetHeadPosition();
		while (pos) {
			StackFrame& sf = sfList.GetNext(pos);
			HRESULT hResult = sf.HResult();
			if (FAILED (hResult) && sf.fTouchedByLastEdit() && 
				!(fCanDefer(hResult) && !sf.IsTopOfStack())) {
				// display only the frames for which no deferred attempt will be performed
				sfListAll.AddTail(sf);
			}
		}
	}

	if (!sfListAll.IsEmpty()) {
		CStackEditDlg dlg(sfListAll);

		switch (dlg.DoModal()) {

		case IDYES:
			ENCOutputErr(Enc_Err_CantUpdateFrame);
			NET_LOG(LOG_DLGYES);
			break;

		case IDNO:
		{
			m_fCanceled = true;
			retval = false;
			NET_LOG(LOG_DLGNO);
			break;
		}

		default:
			ASSERT(0);
		}
	}

	return retval;
}

void Process::CommitEdit()
{
	if (!m_thdList.IsEmpty()) {
		POSITION pos = m_thdList.GetHeadPosition();
		while (pos) {
			Thread& thd = m_thdList.GetNext(pos);
			thd.CommitEdit(true, g_fSetTmpBp);
			if (!m_fMovedIP && thd.MovedIP()) {
				// IP was moved for at least one thread
				m_fMovedIP = true;
			}
		}
	}
}

bool Thread::GetTibStackBase(UOFF32 *poff32)
{
	NT_TIB tib;
	UOFF32 off;
	if (xosdNone == OSDReadRegister(hpidCurr, HTid(), CV_ALLREG_TEB, &off)) {
		ADDR addr  = {0};
		SetAddrOff(&addr, off);
		ModeInit( &(modeAddr(addr)),TRUE,TRUE,FALSE,FALSE);
		if (sizeof(tib) == DHGetDebuggeeBytes(addr, sizeof (tib), &tib)) {
			*poff32 = (UOFF32) tib.StackBase;
			return true;
		}
	}
	return false;
}

bool Thread::IsInExceptionState()
{
	TST ThreadStatus;

	if (!IsDead())
	{
		VERIFY (OSDGetThreadStatus (HPid(), HTid(), &ThreadStatus) == xosdNone);
		return ((ThreadStatus.dwState & tstExceptionMask) == tstExcept2nd);
	}
	return false;
}


bool Thread::PreEdit()
{
	if (IsDead()) {
		return false;
	}

	bool fCompleteStackWalk = false;
	UOFF32 offStack = 0;
	VERIFY(GetTibStackBase(&offStack));

	int cMaxFrames = 1000;
	HTID hvtid = HTid();
	bool fDone = false; 

	ADDR addrBp4;

	for (int iFrame = 0; !fDone && iFrame < cMaxFrames; iFrame++) {
		memset(&addrBp4, 0, sizeof(ADDR));
		if (iFrame > 0) {
			SYGetAddr ( HPid(), hvtid, adrBase, &addrBp4 );
			SYFixupAddr(&addrBp4);
			// Heuristic to see if we have walked the user portion of the stack
			// Assume we're OK if we are close enough to the stack base
			if (offStack - GetAddrOff(addrBp4) < 0x20) {
				fCompleteStackWalk = true;
			}
			// Hack for modifying return values of live frames:
			// Since SYSetAddr doesn't work for virtual threads, we try to compute the location
			// of the return address, by using the BP of the called frame.
			// The return address is normally on BP+4, or BP+0 if the called frame has FPO
			// We run the risk of using a bogus BP value if some FPO info is missing.
			// We'll assume that BP is sane and return address is on BP+4. The stack frame 
			// code will try to validate this assumption (StackFrame::ValidateIpLocation)
			addrBp4.addr.off += sizeof(UOFF32);
		}

		fDone = OSDGetFrame ( HPid(), hvtid, 1, &hvtid ) != xosdNone;
		if ( !fDone ) {
			StackFrame sf(iFrame, m_hpid, m_hthd, hvtid, &addrBp4);
			if (sf.PreEdit()) {
				m_sfList.AddTail(sf);
			}
		}
	}

	if (!fCompleteStackWalk) {
		// REVIEW: Is there a better way or heuristic to find if we walked the entire stack?
		// Warn user that we haven't walked the entire stack
		ENCOutputErr(Enc_Err_StackWalk, Tid());
	}
	
	return true;
}

void Thread::CommitEdit(bool fMoveIP, bool fSetTmpBp)
{
	POSITION pos = m_sfList.GetHeadPosition();
	while (pos) {
		StackFrame& sf = m_sfList.GetNext(pos);
		sf.CommitEdit(fMoveIP, fSetTmpBp);
		if (!m_fMovedIP && sf.MovedIP()) {
			// IP was moved for at least one thread
			m_fMovedIP = true;
		}
	}
}

bool StackFrame::GetAddrEH(PADDR paddr)
{
	UOFF32 off;
	if (xosdNone == OSDReadRegister(HPid(), HTid(), CV_ALLREG_TEB, &off)) {
		memset(paddr, sizeof(ADDR), 0);
		SetAddrOff(paddr, off);
		ModeInit( &(modeAddr(*paddr)),TRUE,TRUE,FALSE,FALSE);
		SYUnFixupAddr(paddr);
		return true;
	}
	return false;
}

void StackFrame::HandleTmpBpHit()
{
	currentLineHint.Invalidate();
	// PreEdit has to return true, since there
	// one past edit that affects the current IP
	VERIFY (PreEdit(true));
	CString strMsg;
	if (SUCCEEDED( m_hResult ))  {
		CommitEdit(true,false);
		CLFreeWalkbackStack (TRUE);
		CLGetWalkbackStack (hpidCurr, htidCurr, (UINT) NULL);
		ADDR addr = {0};
		SYGetAddr (hpidCurr, htidCurr, adrPC, &addr);
		MoveEditorToAddr( &addr );		
		UpdateDebuggerState	(UPDATE_ALLSTATES | UPDATE_ENC);
	}
}

bool StackFrame::PreEdit(bool fHandlingTmpBp)
{
	ADDR addrIp = {0};
	ADDR addrBp = {0};
	SYGetAddr ( HPid(), m_hvtid, adrBase, &addrBp );
	SYGetAddr ( HPid(), m_hvtid, adrPC, &addrIp );

	m_addrIp = addrIp;

	// return true if frame is affected by the edit
	// so that we can either commit the change or set a breakpoint
	// We only care about the latest Edit unless when we handle a TMP bp
	if (theEnc.FEditReplacesAddr( &addrIp, !fHandlingTmpBp)) {
		ADDR addrSp = {0};
		ADDR addrEH = {0};
		if (IsTopOfStack()) {
			SYGetAddr ( HPid(), m_hvtid, adrStack, &addrSp );
			GetAddrEH(&addrEH);

			m_addrSp = addrSp;

			if (fHandlingTmpBp) {
				// HACK: Heuristic for avoiding non-canonical stack (x86 only)
				// TODO: This should probably moved to the engine.
				// If the return address contains a stack adjustment instruction of the
				// form 
				//
				//		addrRet:		83 C4 XX	add esp, XX
				//
				// where XX is a direct byte adjustment, then don't use addrIp,
				// since the stack will probably not be in canonical form when we retry to 
				// move the IP. Instead use the sp/ip as if the next instruction had been
				// executed. This does not cover all cases (e.g., nested function call), 
				// but improves considerably the mainstream single _cdecl call case

				enum { 
					b0 = 0x83,
					b1 = 0xC4,
				};

				BYTE x86instr[3];
				if (fTargetIs386 &&
					DHGetDebuggeeBytes(addrIp, sizeof(x86instr), x86instr) &&
					x86instr[0]	== b0 && x86instr[1] == b1) {
					SYFixupAddr(&addrIp);
					SYFixupAddr(&addrSp);
					OffAddrT(addrAddr(addrIp)) += sizeof(x86instr);
					OffAddrT(addrAddr(addrSp)) += x86instr[2];
				}
			}					
		}
		ULONG lineHint = (IsTopOfStack() && HTid() == htidCurr) ? currentLineHint.Get() : 0;
		m_hResult = theEnc.UpdateIpToEditIp(
			&addrIp,
			&addrBp,
			&addrSp,
			&addrEH,
			IsTopOfStack(),
			lineHint,
			m_rlist
			);
		// compute procedure name -- we may need to display it 
		// Note that we store the full name, not the hProc, since another call to 
		// UpdateIpToEditIp may end up flushing the sapi symbols and render hProc invalid
		GetProcName();
		if (SUCCEEDED (m_hResult) && 
			!IsTopOfStack() &&
			// try to make sure that m_addrIpStack really contains the location of the IP
			!ValidateIpLocation()) {
			// Do we want to introduce a new error for this case?
			// For the time pretend that the edit does not affect the frame
			TRACE("ENC: Cannot validate IP location. Ignoring frame\n");
			return false;
		}

		if (SUCCEEDED (m_hResult)) {
			// update new IP, Sp addresses
			m_addrIp = addrIp;
			m_addrSp = addrSp;
		}
		else {
			CString strReason;
			ENCOutputErr(Enc_Err_FrameFailed, Tid(), m_iFrame, SzName(),
				SeErrorText(strReason, m_hResult));
		}

		return true;
	}
	return false;
}


bool StackFrame::ValidateIpLocation()
{
	ASSERT (!IsTopOfStack());

	ULONG off32;
	ADDR addrIp = {0};
	VERIFY(xosdNone == SYGetAddr(HPid(), m_hvtid, adrPC, &addrIp));
	SYFixupAddr(&addrIp);

	if (sizeof(off32) == DHGetDebuggeeBytes(m_addrIpStack, sizeof(off32), &off32) &&
		off32 == GetAddrOff(addrIp)) {
		return true;
	}

	// if the called frame had no BP, we are possibly 4 bytes off. Check whether the
	// IP is in m_addrIpStack - 4

	SYFixupAddr(&m_addrIpStack);
	GetAddrOff(m_addrIpStack) -= 4;

	return (sizeof(off32) == DHGetDebuggeeBytes(m_addrIpStack, sizeof(off32), &off32) &&
		off32 == GetAddrOff(addrIp));
}

void StackFrame::CommitEdit(bool fMoveIP, bool fSetTmpBp)
{
	if (ADDR_IS_LI(m_addrIp)) {
		SYFixupAddr(&m_addrIp);
	}
	ASSERT (!ADDR_IS_LI(m_addrIp));
	if (IsTopOfStack() && 
		(S_IPPOSNOCHANGE == m_hResult || (fMoveIP && SUCCEEDED(m_hResult)))
	){
		// use actual htid, no virtual thread for SYSetAddr
		SYSetAddr( HPid(), HTid(), adrPC, &m_addrIp);
		SYSetAddr( HPid(), HTid(), adrStack, &m_addrSp);
		SYFixupAddr(&m_addrIp);
		ApplyMUList(m_rlist);
		if (m_hResult == S_IPPOSCHANGE || m_hResult == S_IPPOSCHANGEEH) {
			ENCOutputErr(Enc_Err_TosEdited, Tid());
		}
	}
	else {
		if (SUCCEEDED (m_hResult)) {
			UOFF32 off = GetAddrOff(m_addrIp);
			VERIFY (sizeof(off) == DHPutDebuggeeBytes(m_addrIpStack, sizeof (off), &off));
			ApplyMUList(m_rlist);
			ENCOutputErr(Enc_Err_FrameEdited, Tid(), m_iFrame, SzName(), off);
		}
		else if (fSetTmpBp && fCanDefer(m_hResult) && !IsTopOfStack())	 {
			// set temporary breakpoint so that we notify the 
			// user when the old code reaches the top of the stack
			FSetUpTempBp(&m_addrIp, Hthd(), BPTMPENC);
		}
	}
}

void StackFrame::GetProcName()
{
	// Get process name, store it in global array
	// and set index m_iName
	static char szName[512];
	szName[0] = '\0';
	HFME hfme = LLHlleCreate( hlliFmeEnc );
	if ( hfme ) {
		FRAME FAR * lpframe;
		LPFME		lpfme;
		ADDR	addrPC = {0};
		ADDR	addrBP = {0};
		ADDR		addrData = {0};
		SEGMENT		segData;

		SYGetAddr ( HPid(), HTid(), adrData, &addrData );
		SYFixupAddr ( &addrData );
		segData = GetAddrSeg ( addrData );

		lpfme = (LPFME)LLLpvFromHlle( hfme );

		SYGetAddr ( HPid(), m_hvtid, adrPC, &addrPC );
		SYGetAddr ( HPid(), m_hvtid, adrBase, &addrBP );

		lpfme->addrCSIP = addrPC;

		if ( ADDR_IS_LI ( addrBP ) ) {
			SYFixupAddr ( &addrBP );
		}
		lpframe = &lpfme->Frame;

		lpframe->mode = addrBP.mode;
		SetFrameBPSeg ( *lpframe , GetAddrSeg ( addrBP ) );
		SetFrameBPOff ( *lpframe , GetAddrOff ( addrBP ) );
		SetFrameSLPSeg ( *lpframe , 0 );
		SetFrameSLPOff ( *lpframe , 0 );
		lpframe->SS  = (SEG16)GetAddrSeg ( addrBP );
		lpframe->DS = (SEG16)segData;
		lpframe->TID = HTid();
		lpframe->PID = HPid();

		CLSetProcAddr ( lpfme );
		UnlockHlle( hfme );
		CLGetProcFromHfme (hfme, szName, sizeof(szName), FALSE);
	}
	m_iName = rgProcNames.GetSize();
	rgProcNames.SetAtGrow(m_iName, szName);
}


LPCSTR StackFrame::SzName() const
{
	static char szName[512];
	szName[0] = '\0';

	if (m_iName >= 0) {
		// We know the proc name for this frame
		_tcscpy(szName, rgProcNames[m_iName]);
	}

	// Form proper function name: Currently the ENC engine appends a suffix
	// to old function names that have been replaced with new ones. 
	// The suffix has the form <updXXX> where XXX is a decimal number
	// For the time, just remove this portion to avoid confusing the 
	// user when displaying old function names in ENC UI dialogs

	const char szMunge[] = "<updXXX>";
	const int lenMunge = sizeof(szMunge) - 1;
	char *pch = szName;
	while (pch = _tcschr(pch, '<')) {
		if (!_tcsncmp(pch, szMunge, 4) && 
			_tcslen(pch) >= lenMunge &&
			pch[lenMunge - 1] == szMunge[lenMunge - 1] ) {
			pch += lenMunge;
			// Remove the <updXXX> portion
			memmove(pch - lenMunge, pch, _tcslen(pch) + 1);
			break;
		}
		else {
			pch ++;
		}
	}

	return szName;
}


//
// Support for File tracking and basic ENC operations
//
class CSafeMod {
	HMOD m_hmod;
	HEXE m_hexe;

public:
	BOOL Init(HMOD hmod) {
		m_hmod = hmod; 
		m_hexe = SHHexeFromHmod(m_hmod);
		if (m_hexe == NULL) {
			NET_LOG(LOG_INIT);
			return FALSE;
		}
		else
			return TRUE;
	}

	__inline BOOL operator == (const CSafeMod& smod) const{
		return smod.m_hmod == m_hmod &&
			smod.m_hexe == m_hexe;
	}

	__inline HEXE GetHmod() {
		return m_hmod;
	}

	__inline HEXE GetHexe() {
		return m_hexe;
	}

	// check whether handles are still valid in SAPI
	// and correspond to the given source path
	//
	// NOTE: This is meant for handling the problem of hmod lifetime.
	// If hmods are valid throughout the duration of the debugging 
	// session, then this check is unnecessary
	//
	BOOL IsValid(LPCTSTR lpszPath) {
		HEXE hexe = (HEXE) 0;
		while (hexe = SHGetNextExe(hexe)) {
			if (hexe != m_hexe) 
				continue;
			HMOD hmod = (HMOD) 0;
			while (hmod = SHGetNextMod(hexe, hmod)) {
				if (hmod == m_hmod) {
					return SHModSupportsEC(hmod);
						// Commented out 12/15/97:
						// We can no longer use the src path for validating 
						// the hmod if we want to allow editing of header
						// files for ENC (so lpszPath may be different from
						// the src path.)
						// && !_tcsicmp(lpszPath, SHGetSrcPath(hmod));
				}
			}
		}
		return FALSE;
	}

	__inline LPCTSTR GetSrcPath() {
		return SHGetSrcPath(m_hmod);
	}

	__inline LPCTSTR GetObjPath() {
		return SHGetObjPath(m_hmod);
	}

	__inline LPCTSTR GetDllPath() {
		return SHGetExeName(m_hexe);
	}

	__inline LPCTSTR GetCompileEnv() {
		return SHGetCompileEnv(m_hmod);
	}

	LPCTSTR GetSrcFileName(CString& strName) {
		CPath path;
		if (path.Create(GetSrcPath())) {
			strName = path.GetFileName();
		}
		return strName;
	}

	BOOL IsEligibleForENC() {
		BOOL fRet = TRUE;
		
		CPath pathObj;
		VERIFY(pathObj.Create(GetObjPath()));

		CPath pathDll;
		VERIFY(pathDll.Create(GetDllPath()));

		CPath pathSrc;
		VERIFY(pathSrc.Create(GetSrcPath()));

		FILETIME timeObj;
		FILETIME timeDll;

		CString strMsg;

		int encErr = 0;
		// Check existence / eligibility of the obj file
		// reject obj files that are newer than respective dll
		// This is a validation workaround until we have some CRC in the PDB
		if (SHModLinkedFromLib(m_hmod)) {
			ENCOutputErr(Enc_Err_LinkedFromLib, (LPCTSTR) pathObj);
			ENCOutputErr(Enc_Err_IgnoringFile, (LPCTSTR) pathSrc); 
			fRet = FALSE;
		}
		else if (!pathObj.ExistsOnDisk()) {
			ENCOutputErr(Enc_Err_CantFindObj, (LPCTSTR) pathObj);
			ENCOutputErr(Enc_Err_IgnoringFile, (LPCTSTR) pathSrc); 
			fRet = FALSE;
		}
		else if ( !(theEnc.isEditedObject(m_hmod)) &&
			pathObj.GetFileTime(&timeObj) && 
			pathDll.GetFileTime(&timeDll) &&
			CTime(timeObj) > CTime(timeDll) ) {
			ENCOutputErr(Enc_Err_ObjMismatch, (LPCTSTR) pathObj);
			ENCOutputErr(Enc_Err_IgnoringFile, (LPCTSTR) pathSrc); 
			fRet = FALSE;
		}
		else if (!ImageBase(m_hexe)) {
			// Unless a dll is loaded, we can't patch the image
			ENCOutputErr(Enc_Err_IgnoringFile, (LPCTSTR) pathSrc); 
			fRet = FALSE;
		}

		return fRet;
	}

	BOOL SaveOldObj() {
		CString strObj = GetObjPath();
		CString strEnc;
		if (!strObj.IsEmpty() && GetActualFileCase(strObj)) {
			strEnc = ENCCreateTempName(strObj);
			if (CopyFile(strObj, strEnc, FALSE))
				return TRUE;
		}
		ENCOutputErr(Enc_Err_CantCreateFile, strEnc);
		TRACE("ENC: Failed to move %s to %s\n", strObj, strEnc);
		return FALSE;
	}

	BOOL RestoreOldObj() {
		if (runDebugParams.fENCRepro) {
			// if fENCRepro is set, we leave the old objs
			// to assist reproducing / debugging ENC test cases
			return FALSE;
		}
		CString strObj = GetObjPath();
		CString strEnc = ENCCreateTempName(strObj);
		if (!MyMoveFileEx(strEnc, strObj, MOVEFILE_REPLACE_EXISTING)) {
			TRACE("ENC: Failed to move %s to %s\n", strEnc, strObj);
			return FALSE;
		}
		return TRUE;
	}

	BOOL DeleteOldObj() {
		if (runDebugParams.fENCRepro) {
			// if fENCRepro is set, we leave the old objs
			// to assist reproducing / debugging ENC test cases
			return FALSE;
		}

		CPath path;
		CString strEnc = ENCCreateTempName(GetObjPath());
		path.Create (strEnc);
		if (path.ExistsOnDisk()) {
			SetFileAttributes(strEnc, FILE_ATTRIBUTE_NORMAL);
			if (!::DeleteFile(strEnc)) {
				TRACE("ENC: Could not delete file \"%s\"\n", strEnc);
				return FALSE;
			}
		}
		return TRUE;
	}


	BOOL AddToEC() {
		if ( FAILED	( theEnc.InsertObject (GetHmod(), GetObjPath())) ) {
			TRACE("ENC: Failed to insert object \"%s\"\n", GetObjPath());
			return FALSE;
		}
		return TRUE;
	}

};


typedef CList<CSafeMod, CSafeMod&> CSafeModList;


class CEncSession: public CObject {
	// map full source path to corresponding list of affected ECMods
	CGrowingMap< CString, LPCTSTR, CSafeModList*, CSafeModList*& > m_map;
	bool m_fRelinkedImages;
	bool m_fLoadedIdbFiles;

public:

	CEncSession(): m_fRelinkedImages(false), m_fLoadedIdbFiles(false) {}

	~CEncSession() {
		Reset();
	}

	void AddMod(HMOD hmod, LPCTSTR lszSrc) {
		CSafeMod smod;
		if (smod.Init(hmod)) {
			CSafeModList *plst;
			CString strSrc = lszSrc;
			LPCTSTR szCanon = szCanonicalPath(strSrc);
			if (!m_map.Lookup(szCanon, plst)) {
				plst = new CSafeModList;
				plst->AddTail(smod);
				m_map.SetAt(szCanon, plst);
			}
			else if (!plst->Find(smod)) {
				// don't try to validate the elements of the list now
				// We'll do that later only for the small subset of 
				// files changed by the user
				plst->AddTail(smod);
			}
		}
	}

	BOOL FindModList(LPCTSTR lszPath, CSafeModList* &pList) {
		CSafeModList *plst;
		CString strPath = lszPath;
		LPCTSTR lszSrc = szCanonicalPath(strPath);
		if (!m_map.Lookup(lszSrc, plst)) {

			if (!m_fLoadedIdbFiles && runDebugParams.fENCLoadIdb) {
				LoadAllIdbFiles();
				m_fLoadedIdbFiles = true;
				return FindModList(lszPath, pList);
			}

			return FALSE;
		}

		// handle mainstream case first
		if (plst->GetCount() == 1) {
			CSafeMod smod = plst->GetHead();
			if (smod.IsValid(lszSrc)) {
				pList = plst;
				return TRUE;
			}
		}

		// Validate list
		POSITION pos = plst->GetHeadPosition();
		while (pos) {
			POSITION posSav = pos;
			CSafeMod smod = plst->GetNext(pos);
			if (!smod.IsValid(lszSrc)) {
				plst->RemoveAt(posSav);
			}
		}

		if (plst->IsEmpty()) {
			m_map.RemoveKey(lszSrc);
			delete plst;
			return FALSE;
		}		 

		pList = plst;
		return TRUE;
	}


	BOOL Reset() {
		POSITION pos = m_map.GetStartPosition();
		CSafeModList *plst;
		CString str;
		while (pos) {
			m_map.GetNextAssoc(pos, str, plst);
			delete plst;
		}
		m_map.RemoveAll();
		m_fRelinkedImages = false;
		m_fLoadedIdbFiles = false;
		return TRUE;
	}

	bool RelinkImages(bool fAsync) {
		if (m_fRelinkedImages) {
			// In some scenarios we are getting called twice.
			// Avoid relinking in that case
			return true;
		}

		m_fRelinkedImages = true;

		CStringList lstExePath;		// path of exe/dll to relink
		CStringList lstCwd;			// path where original link took place
		CStringList lstCmd;			// path to the link command
		CStringList lstOutFile;		// path of file produced by link command

		EnumLink* pEnum;
		if (theEnc.getEnumLink (&pEnum)) {
			LPCSTR pszExePath;
			LPCSTR pszCwd;
			LPCSTR pszCmd;
			LPCSTR pszOutFile;
			while (pEnum->next()) {
				pEnum->get(
					&pszExePath, 
					&pszCwd,
					&pszCmd,
					&pszOutFile);
				lstExePath.AddTail(pszExePath);
				lstCwd.AddTail(pszCwd);
				lstCmd.AddTail(pszCmd);
				lstOutFile.AddTail(pszOutFile);
			}
		}
		
		if (lstExePath.IsEmpty()) 
			return true;

		CString strMsg;
		POSITION posExePath = lstExePath.GetHeadPosition();
		POSITION posCwd = lstCwd.GetHeadPosition();
		POSITION posCmd = lstCmd.GetHeadPosition();
		POSITION posOutFile = lstOutFile.GetHeadPosition();
		bool fRetVal = true;
		HRESULT hResult;

		EncClearOutput();

		while (posExePath) {
			CStringList lstCmd;
			CString& strExePath = lstExePath.GetNext(posExePath);
			CString& strCwd = lstCwd.GetNext(posCwd);
			CString& strCmd = lstCmd.GetNext(posCmd);
			CString strOutPath = lstOutFile.GetNext(posOutFile);

			// suppress logo for relink
			strCmd += " -nologo";
			lstCmd.AddTail (strCmd);

			OutputMessage(MsgText(strMsg, IDS_Enc_RelinkingFile, strOutPath));

			gpISrc->StatusText (IDS_Enc_Relinking, STATUS_INFOTEXT, FALSE);
			DWORD dwErr = 0;
			DWORD dwWarn = 0;
			hResult = gpIBldSys->DoSpawn(lstCmd, strCwd, FALSE, fAsync, &dwErr, &dwWarn);

			CPath pathOutFile;
			pathOutFile.Create(strOutPath);

			if (FAILED (hResult)) {
				NET_LOG(LOG_RELINKFAILED);
				fRetVal = false;
				if (hResult == E_ABORT)
					break;
			}
			else {
				// Relink was successful. Check if the resulting image 
				// is in a different directory than the one used for debugging,
				// and notify user
				if (strExePath.CompareNoCase(pathOutFile) ) {
					CPath pathExe;
					pathExe.Create(strExePath);
					pathExe.GetActualCase(TRUE);
					ENCOutputErr(Enc_Err_ExePath, (LPCTSTR)pathOutFile, (LPCTSTR)pathExe);
					dwWarn ++;
				}
			}

			OutputMessage(MsgText(strMsg, IDS_Enc_ErrorSummary, (LPCTSTR)pathOutFile, dwErr, dwWarn));
			OutputMessage("");
			gpISrc->StatusText(SYS_StatusClear, STATUS_INFOTEXT, FALSE);
		}

		if (fRetVal && pDebugCurr) {
			// reset flag to indicate that restart is possible
			pDebugCurr->SetEditedByENC(false);
		}
		return fRetVal;
	}

};	  

// The entire debugging session that 
// may involve 0 or more ENC operations
static CEncSession encSession;

class CEncOperation: public CObject {
	
	CSafeModList m_buildList;
	BOOL m_fAddedEC;
	BOOL m_fRestoredObjs;
	BOOL m_fCanceled;
	BOOL m_fDetectedNoChanges;
	DWORD m_cErrors;
	DWORD m_cWarnings;
	HANDLE m_hEncEvent;	// Event used by the compiler to indicate a /ZX-related failure

private:
	const char * SzEncEvent() {
		static char lszEvent[32];
		sprintf(lszEvent, "EditAndContinue%8.8x", GetCurrentProcessId());
		return lszEvent;
	}

public:
	CEncOperation(): m_hEncEvent(NULL){
		Reset();
	}

	~CEncOperation() {
		Reset();
		if (m_hEncEvent) {
			CloseHandle(m_hEncEvent);
		}
	};

	void Reset() {
		m_buildList.RemoveAll();
		m_fAddedEC = FALSE;
		m_fRestoredObjs = FALSE;
		m_fCanceled = FALSE;
		m_fDetectedNoChanges = FALSE;
		m_cErrors = 0;
		m_cWarnings = 0;
		if (m_hEncEvent) {
			ResetEvent(m_hEncEvent);
		}
	}

	HANDLE GetEncEvent() {
		if (m_hEncEvent == NULL) {
			m_hEncEvent = CreateEvent(NULL, TRUE, FALSE, SzEncEvent());
		}
		return m_hEncEvent;
	}

	BOOL Init(const CStringList& lstStr) {
		Reset();
		POSITION pos = lstStr.GetHeadPosition();
		while (pos) {
			CString str = lstStr.GetNext(pos);
			CSafeModList *plst;
			if (!encSession.FindModList(str, plst)) 
				continue;

			CPath pathSrc;
			pathSrc.Create(str);

			CDocument *pDoc;
			if (pathSrc.IsReadOnlyOnDisk() && 
				(pDoc = gpISrc->GetDocFromFileName(pathSrc)) &&
				pDoc->IsModified()) {
				// treat this as an error or else SaveAll will 
				// start complaining later
				ENCOutputErr(Enc_Err_ReadOnly, (LPCTSTR) pathSrc);
				return FALSE;
			}

			POSITION posMod = plst->GetHeadPosition();
			while (posMod) {
				CSafeMod smod = plst->GetNext(posMod);

				if (m_buildList.Find(smod)) {
					// don't put duplicate mods in the build list
					continue;
				}

				if (smod.IsEligibleForENC()) {
					NET_LOG_HMOD(smod.GetHmod());
					m_buildList.AddTail(smod);
					if (m_buildList.GetCount() > MAX_ENC_COMPILANDS) {
						// If we have too many files then considering the time 
						// to save old objs, prepare compile commands, rebuild
						// (with no batching) and restore objs in case of 
						// failure, it might be better for the user to do a 
						// full build instead of ENC  [vc98 #34941]
						ENCOutputErr(Enc_Err_TooManyFiles);
						return FALSE;
					}
				}
			}
		}
		return !m_buildList.IsEmpty();
	}

	void Abort() {
		if (m_fAddedEC) {
			// REVIEW: What should we do if this fails?
			if (FAILED (theEnc.PopEdit())) {
				NET_LOG(LOG_POPEDIT);
				TRACE("ENC: PopEdit failed!\n");
			}
		}
	}

	void RestoreOldObjs() {
		if (m_fRestoredObjs)
			return;
		POSITION pos = m_buildList.GetHeadPosition();
		while (pos) {
			CSafeMod smod = m_buildList.GetNext(pos);
			if (!smod.RestoreOldObj()) {
				smod.DeleteOldObj();
			}
		}
		m_fRestoredObjs = TRUE;
	}

	BOOL SaveOldObjs() {
		POSITION pos = m_buildList.GetHeadPosition();
		while (pos) {
			CSafeMod smod = m_buildList.GetNext(pos);
			if (!smod.SaveOldObj()) {
				NET_LOG(LOG_SAVEOLDOBJS);
				return FALSE;
			}
		}
		return TRUE;
	}

	void DeleteOldObjs() {
		POSITION pos = m_buildList.GetHeadPosition();
		while (pos) {
			CSafeMod smod = m_buildList.GetNext(pos);
			smod.DeleteOldObj();
		}
	}

	BOOL CreateCmdList(CStringList& lstCmd, CStringList& lstDesc) {
		POSITION pos = m_buildList.GetHeadPosition();
		CString strMsg;
		CString strFileName;
		CSafeMod smod;
		int nLength = 0;
		while (pos) {
			smod = m_buildList.GetNext(pos);
			const TCHAR* szCmd = _T("");
			const TCHAR *buf = smod.GetCompileEnv();
			if ( !buf ) {
				NET_LOG(LOG_GETCOMPILEENV);
				TRACE("ENC: GetCompileEnv failed\n");
				return FALSE;
			}

			StrEnviron strenv( buf );
			CString strCmd = strenv.szValueForTag( _T("cmd") );
			CString strCl = strenv.szValueForTag( _T("cl") );
			if (::ScanPathForSpecialCharacters(strCmd)) {
				// path should be quoted
				strCl = CString ("\"") + strCl + CString("\"");
			}
			CString strSrc = strenv.szValueForTag( _T("src") );
			if (::ScanPathForSpecialCharacters(strSrc)) {
				// path should be quoted
				strSrc = CString ("\"") + strSrc + CString("\"");
			}

			strCl += " ";
			strCl += strCmd;
			// Append /ZX so that compiler opens PDB in read only mode
			strCl += " -ZX";

			// Append event handle name for /ZX errors
			strCl += SzEncEvent();
			strCl += " ";

			strCl += strSrc;

			// TODO: check whether we can merge multiple calls to cl into one
			
			MsgText(strMsg, IDS_Enc_RecompilingFile, smod.GetSrcFileName(strFileName));

			// If this compile creates a pch, then we want to execute first
			if (strCmd.Find(" -Yc") != -1) {
				lstCmd.AddHead(strCl);
				lstDesc.AddHead(strMsg);
			}
			else {
				lstCmd.AddTail(strCl);
				lstDesc.AddTail(strMsg);
			}

			nLength += (strCl.GetLength() + 1);

			// The build system will combine all commands into a long command line
			// Assume that the extra stuff to be added by the build system on 
			// the command line will be less than 1K. If the combined command 
			// becomes too long, issue an error. 
			// TODO: Use cl with response file to work around this limitation
			if (nLength > MAX_SPAWN_CMD_LEN - 1024) {
				// Set event to indicate unsupported edit
				HANDLE hEncEvent = GetEncEvent();
				if (hEncEvent) {
					SetEvent(hEncEvent);
				}
				ENCOutputErr(Enc_Err_TooManyFiles);
				return FALSE;
			}
		}
		return !lstCmd.IsEmpty();
	}

	BOOL DoCompileAsync()
	{
		CStringList lstCmd;
		CStringList lstDesc;
		if (!CreateCmdList(lstCmd, lstDesc)) 
			return FALSE;
		gpISrc->StatusText (STA_Recompiling, STATUS_INFOTEXT, FALSE);
		TCHAR szDir[_MAX_PATH];
		VERIFY (GetCurrentDirectory(sizeof(szDir), szDir));
		DWORD dwErr = 0;
		DWORD dwWarn = 0;
		OutputMessage(IDS_Enc_RecompilingFile);
		HRESULT hResult = gpIBldSys->DoSpawn(lstCmd, szDir, FALSE, TRUE, &dwErr, &dwWarn);
		m_cErrors += dwErr;
		m_cWarnings += dwWarn;
		if (hResult == E_ABORT) {
			m_fCanceled = TRUE;
		}
		if (FAILED (hResult)) {
			NET_LOG(LOG_COMPILEERROR);
		}
		gpISrc->StatusText(SYS_StatusClear, STATUS_INFOTEXT, FALSE);
		return SUCCEEDED(hResult);
	}

	BOOL DoCompile()
	{
		BOOL retval;
		// We are going to recompile asyncronously. 
		// Update the mode name to indicate
		// that this is a special mode (different than "run"
		// or "break"). No debugging operations like step
		// or go are allowed while in this state
		SetModeName(TBR_Mode_ENC);
		retval = DoCompileAsync();
		SetModeName(TBR_Mode_Break);
		return retval;
	}

	BOOL AddModsToEC() {
		POSITION pos = m_buildList.GetHeadPosition();
		while (pos) {
			CSafeMod smod = m_buildList.GetNext(pos);
			if (!smod.AddToEC())
				return FALSE;
		}
		return TRUE;
	}

	BOOL DoImport() {

		HRESULT hResult = theEnc.pushNewEdit();
		if ( FAILED (hResult) ) {
			NET_LOG(LOG_PUSHNEWEDIT);
			NET_LOG_HRESULT(hResult);
			TRACE("ENC: Failed to push an edit\n");
			return FALSE;
		}

		// remember to remove EC on Abort()
		m_fAddedEC = TRUE;

		if (!AddModsToEC()) {
			NET_LOG(LOG_ADDMODSTOEC);
			return FALSE;
		}

		hResult = theEnc.ApplyCurrentEdit();
		if ( FAILED (hResult) ) {
			NET_LOG(LOG_APPLYCURRENTEDIT);
			NET_LOG_HRESULT(hResult);
			TRACE("ENC: Failed to apply edit\n");
			return FALSE;
		}
		if ( S_NOCODECHANGE == hResult ) {
			m_fDetectedNoChanges = TRUE;
		}
		return TRUE;
	}

	BOOL IsCanceled() { return m_fCanceled; }

	BOOL DetectedNoChanges() { return m_fDetectedNoChanges; } 

	void OutputError(LPCSTR lsz, int errorCode)
	{

		// 1000 - 1999: error
		// 2000 - 2999: warning
		// 3000 - 3999: info
		// 4000 - 4999: info

		int level = errorCode / 1000;

		if (level <= runDebugParams.iENCTraceLevel) {

			int ids = IDS_Enc_InfoCode;

			switch (level) {
			case 1:
				// error
				m_cErrors ++;
				ids = IDS_Enc_ErrorCode;
				break;
			case 2:
				// warning
				m_cWarnings ++;
				ids = IDS_Enc_WarningCode;
				break;
			}

			CString strMsg;
			CString strPrefix;
			strPrefix.LoadString(IDS_EncOutPrefix);
			MsgText(strMsg, ids, strPrefix, errorCode, lsz); 

			OutputMessage(strMsg, TRUE);
		}
	}			

	void OutputErrorSummary()
	{
		if (!m_fCanceled) {
			CString strMsg;
			CString strPrefix;
			strPrefix.LoadString(IDS_EncOutPrefix);
			MsgText(strMsg, IDS_Enc_ErrorSummary, strPrefix, m_cErrors, m_cWarnings);
			OutputMessage("");
			OutputMessage(strMsg);
		}
	}
};

static CEncOperation encOperation;

// Highlight the first error in the output window
void ShowFirstError()
{
	// Highlight the first error message in the output window
	// and synchronize the source
	// We need to wait for a few error messages to appear
	Sleep(1000);
	MSG msg;
	// dequeue all WM_TIMER messages.
	// This will trigger dequeueing of output window hits
	while (PeekMessage (&msg, 0, WM_TIMER, WM_TIMER, PM_REMOVE))
	{
		if (!theApp.PreTranslateMessage (&msg))
			DispatchMessage (&msg);
	}

	// Unlock any locked-until-idle messages
	StatusOnIdle();

	// (We probably need an interface from the editor for this)
	AfxGetMainWnd()->SendMessage(WM_COMMAND, IDM_VIEW_NEXTERROR);
}

DWORD DoEditAndContinue()
{
	START_PERF_CLOCK();
	DWORD dwEncFlags = 0;

	encOperation.Reset();

	CStringList lstSrc;
	if (!gpISrc->GetSrcPathsForENC(&lstSrc) || lstSrc.IsEmpty())
		return dwEncFlags;

	EncClearOutput();

	if( lstSrc.IsEmpty() ||
		!encOperation.Init(lstSrc) ||
		!encOperation.SaveOldObjs()) {
		PERF_LOG(start_clock, szFailure);
		ENCOutputErr(Enc_Err_CantInitialize);
		return dwEncFlags;
	}

	if (!theApp.SaveAll(gpISrc->GetSrcEnvironParam(ENV_PROMPT_BEFORE_SAVE), 
			FILTER_PROJECT | FILTER_DEBUG | FILTER_NEW_FILE)) {
		// cancel entire enc operation
		encOperation.Abort();
		encOperation.RestoreOldObjs();
		return dwEncFlags | EncCancel;
	}

	HANDLE hEncEvent = encOperation.GetEncEvent();

	BOOL fCompileOK = encOperation.DoCompile();

	if (encOperation.IsCanceled()){
		dwEncFlags |= EncCancel;
	}
	else if (!fCompileOK) {
		// check if the compiler raised an event to indicate /ZX failure
		// in that case we have an unsupported edit 
		if (hEncEvent && WaitForSingleObject(hEncEvent, 0) == WAIT_TIMEOUT) {
			dwEncFlags |= EncCompileError;
		}
		else {
			dwEncFlags |= EncUnsupportedEdit;
		}
	}

	BOOL fImportOK = FALSE;
	BOOL fStackEditOK = FALSE;

	if ( !DebuggeeAlive() ) {
		// debugging must have stopped during
		// the recompile
		PERF_LOG(start_clock, szFailure);
		return dwEncFlags;
	}

	// We need to set the src line hint before 
	// applying the edit, since the relevant code
	// depends on the old line info
	currentLineHint.Set();

	if (fCompileOK) {

		dwEncFlags |= EncCompiledFiles;
		// The import step may take a couple of seconds in a big 
		// project, so change the cursor to hourglass
		HCURSOR hSaveCursor = SetCursor (LoadCursor (NULL, IDC_WAIT));

#ifdef NEVER
		// Disabled: We no longer switch to the debug pane. 
		// All ENC output goes to the build pane
		OutputWindowShowVwin(OwinGetDebugCommandID());
		OutputWindowSelectVwin(OwinGetDebugCommandID(), TRUE);
		CDocument *pDoc;
		CString strErrorsWin;
		// We want to select to scroll the debug output window to the end
		// so that the user can see the new ENC messages.
		if (strErrorsWin.LoadString(SYS_ErrorsWin_Title) &&
			gpISrc->FindDocument( strErrorsWin, &pDoc, FALSE )) {
			(gpISrc->FirstView(pDoc))->SendMessage(WM_COMMAND, IDM_CURSOR_FILEEND);
		}
#endif

		fImportOK = encOperation.DoImport();
		if (!fImportOK) {
			dwEncFlags |= EncUnsupportedEdit;
		}
		SetCursor(hSaveCursor);
	}
	else {
		encOperation.RestoreOldObjs();
	}

	hlliFmeEnc = LLHlliInit( sizeof( FME ), llfNull, CLFreeFmeData, NULL );

	ASSERT (rgProcNames.GetSize() == 0);
	Process prc(hpidCurr);

	if (fImportOK && encOperation.DetectedNoChanges()) {
		// no need to edit the stack 
		fStackEditOK = true;
	}
	else if (fImportOK) {
		fStackEditOK = prc.StackEdit();
		if (prc.IsCanceled()) {
			encOperation.RestoreOldObjs();
			dwEncFlags |= EncCancel;
		}
	}

	LLChlleDestroyLl( hlliFmeEnc );
	hlliFmeEnc = NULL;

	if (fCompileOK) {
		// Output message for success, failure, or no changes
		CString strMsg;
		if (prc.IsCanceled()) {
			OutputMessage(IDS_EncOutCancel);
		}
		else if (!fImportOK) {
			ENCOutputErr(Enc_Err_UnsupportedEdit);
		}
	}

	if (!fStackEditOK) {
		
		PERF_LOG(start_clock, szFailure);
		encOperation.Abort();
		rgProcNames.RemoveAll();
		return dwEncFlags;
	}


	OutputMessage(IDS_Enc_ApplyingChanges);
	prc.CommitEdit();
	rgProcNames.RemoveAll();

	UpdateDebuggerState(UPDATE_ENC);
	// We actually need to reparse /rebind BPs, since unfixed 
	// up addresses may have changed
	// TODO: find a way to do selective rebinding of the BP list
	BHUnBindBPList(TRUE);
	if (BHBindBPList()) {
		dwEncFlags |= EncBoundBPs;
	}

	if (prc.MovedIP()) {
		dwEncFlags |= EncMovedIP;
	}

	dwEncFlags |= EncUpdatedImage;

	// TODO: Notify EM

	gpISrc->ResetProjectState();
	
	encOperation.DeleteOldObjs();
	ASSERT(pDebugCurr);
	pDebugCurr->SetEditedByENC (TRUE); 
	PERF_LOG(start_clock, szSuccess);
	NET_LOG(LOG_SUCCESS);
	return dwEncFlags;
}

//
//	ENCCreateTempName
// 
//		Create a temporary name for the saved obj file
//
//	Arguments
//
//		lsObjName: full pathname to obj file
//
//	Return Value
//
//		pointer to static buffer containing new name if successful
//		NULL otherwise
//
//	Remarks
//
//		If the obj filename is "c:\foo.obj" the temp name is "c:\foo.obj.enc" 
//		In the presence of 8.3 restrictions the temp name is "c:\foo.enc"
//
 
static const TCHAR szExtEnc[] = _T(".enc");
#define cbExtEnc (sizeof(szExtEnc) / sizeof(TCHAR) - sizeof(TCHAR))

LPCTSTR ENCCreateTempName(LPCTSTR lszObjName)
{
	TCHAR szRoot [ _MAX_PATH];
	static TCHAR szEncName[ _MAX_PATH ];
	// TODO: Cache last results returned from GetVolumeInformation for quick access

	_tsplitpath(lszObjName, szRoot, NULL, NULL, NULL);
	if (! *szRoot) {
		// we should have a UNC path
		_tcscpy(szRoot, lszObjName);
		ASSERT (szRoot[0] == '\\' && szRoot[1] == '\\');
		TCHAR *pch = szRoot + 2;
		ASSERT (*pch);
		pch = _tcschr(pch, '\\');
		ASSERT (pch);
		pch = _tcschr(pch+1, '\\');
		ASSERT (pch);
		*pch = '\0';
	}
	_tcscat(szRoot, _T("\\"));
	DWORD cbCompMax;

	if (!GetVolumeInformation(szRoot, NULL, 0, NULL, &cbCompMax, NULL, NULL, 0))
		return NULL;

	_tcscpy(szEncName, lszObjName);

	if (cbCompMax != 255 || (_tcslen(lszObjName) + cbExtEnc > cbCompMax)) {
		// form foo.enc
		TCHAR *pch = _tcsrchr(szEncName, _T('.'));
		if ( pch )
			*pch = _T('\0');
	}

	_tcscat(szEncName, szExtEnc);
	return szEncName;
}

// all these functions are expected to be called by the same thread    


// 
// Reset the Edit & Continue state
// 
void ENCReset()  
{
	encSession.Reset();
	encOperation.Reset();
	g_fSetTmpBp = true;
	// reset Enc engine
	if (IsEncLoaded ()) {
		theEnc.Close();
	}
}

// 
// Add the mods contained in a dll to the 
// Edit & Continue File Set
//
void ENCAddDll (HEXE hexe) 
{
	if (!IsEncLoaded ()) {
		return ;
	}
	
	if (SHExeSupportsEC(hexe)) {

		TRACE("ENCAddDll: Adding dll %s\n", SHGetExeName(hexe));

		HMOD hmod = (HMOD) NULL;
		while (hmod = SHGetNextMod (hexe, hmod)) 
		{
			if (!SHModSupportsEC(hmod))
				continue;

			char * szPath = SHGetSrcPath(hmod); 

			if (!CString(szPath).IsEmpty()) {
				encSession.AddMod(hmod, szPath);
			}
		} 
	}
}


// 
// Checks whether a file belongs to the
// "Edit & Continue" File Set
// 
	
BOOL ENCProjectFile(CPath *ppath)
{
	if (!IsEncLoaded ()) {
		return false;
	}
	
	CSafeModList *plst;
	return encSession.FindModList(*ppath, plst);
}

// 
// Handles a temporary ENC breakpoint
// This is hit when a stack frame which was could not be updated by ENC
// reaches the top of stack. In such a case we try the update again. 
// (We have much better chances of succeeding this time, since there are
// much less restrictions updating a TOS frame than a non-TOS frame)
// 
void ENCHandleTmpBp()
{
	ADDR addr;
	SYGetAddr(hpidCurr, htidCurr, adrPC, &addr);

	UINT uWidgetFlag;
	if(BPFirstBPI() &&
		BPIsAddr(&addr,1,hthdCurr,TRUE,0,&uWidgetFlag, BPTMPENC)) {

		EncClearOutput();

		OutputMessage(IDS_Enc_ApplyingChanges);

		encOperation.Reset();

		// Don't UPDATE_SOURCE to avoid bringing up the DAM window
		UpdateDebuggerState(UPDATE_ALLSTATES & ~UPDATE_SOURCE);

		// Create a StackFrame representing the current TOS and 
		// let it handle the update

		HTID hvtid = htidCurr;
		VERIFY(xosdNone == OSDGetFrame(hpidCurr, htidCurr, 1, &hvtid));
		ADDR addrIpStack = {0};

		ASSERT (hlliFmeEnc == NULL);
		hlliFmeEnc = LLHlliInit( sizeof( FME ), llfNull, CLFreeFmeData, NULL );

		ASSERT (rgProcNames.GetSize() == 0);
		StackFrame sf(0, hpidCurr, hthdCurr, hvtid, &addrIpStack);
		sf.HandleTmpBpHit();

		HRESULT hResult = sf.HResult();

		if (FAILED (hResult)) {
			ENCOutputErr(Enc_Err_CantUpdateFrame);
		}

		encOperation.OutputErrorSummary();

		if (SUCCEEDED (hResult)) {
			MsgBox (Information, IDS_EncTmpBpHitOK );
		}
		else {
			CString strMsg;
			CString strMsg2;
			LPCTSTR lszMsg2 = SeErrorText(strMsg2, hResult);
			MsgBox (Error, MsgText(strMsg, IDS_EncTmpBpHit, lszMsg2), MB_OK);
		}

		rgProcNames.RemoveAll();

		LLChlleDestroyLl( hlliFmeEnc );
		hlliFmeEnc = NULL;

	}
}

bool ENCReplacesAddr(ADDR *paddr)
{
	if (!IsEncLoaded ()) {
		return false;
	}
	return (theEnc.FEditReplacesAddr( paddr , false ));
}

bool ENCIsEditedImage(HEXE hexe)
{
	if (!IsEncLoaded ()) {
		return false;
	}
	return theEnc.isEditedImage( hexe );
}

bool ENCReloadImage(HEXE hexe)
{
	if (!IsEncLoaded ()) {
		return false;
	}
	return SUCCEEDED (theEnc.ReloadImage( hexe ));
}


// Canonicalize path
// This function should return a canonical representation
// of a path name in order to use it as a key for hashing

static LPCTSTR szCanonicalPath(CString& strPath)
{
	// REVIEW: If this ends up being a performance hog,
	// we should have the compiler put out the actual
	// file name for ENC_SRC using the correct case.
	// for the time let's convert everything to lowercase
	ASSERT (!strPath.IsEmpty());
	strPath.MakeLower();
	return strPath;
}


//
// MoveFileEx not fully implemented in Win95.
// Following code borrowed from the edit package
//
BOOL MyMoveFileEx(
	LPCTSTR	lpszExisting,   
	LPCTSTR	lpszNew, 
	DWORD 	fdwFlags )
{
    if (!MoveFileEx(lpszExisting, lpszNew, fdwFlags))
	{
        ASSERT( (fdwFlags & MOVEFILE_DELAY_UNTIL_REBOOT) == 0 );

		if( fdwFlags & MOVEFILE_REPLACE_EXISTING )
			// delete the target file in case it exists
			DeleteFile( lpszNew );

		// try to move the file
		if( !MoveFile( lpszExisting, lpszNew ) )
		{
			// see if error is ok
			if( (fdwFlags & MOVEFILE_COPY_ALLOWED) && (GetLastError()==ERROR_NOT_SAME_DEVICE) )
				// ok, do a copy file
				return CopyFile( lpszExisting, lpszNew, TRUE );
			else
				return FALSE;
		}
		return TRUE;
	}
    return TRUE;
} 

// 
// Relink affected images at the 
// end of a debugging session
//
bool ENCRelink(bool fAsync)
{
	if (!IsEncLoaded ()) {
		return true;
	}	

	if (!runDebugParams.fENCRelink) {
		return true;
	}

	return encSession.RelinkImages(fAsync);
}

// 
// Is a rebuild for ENC in progress
//
bool ENCIsRebuilding()
{
	if (!IsEncLoaded ()) {
		return false;
	}	
	return gpIBldSys->IsBuildInProgress() == S_OK;
}

// The following should be called when an Edit and Continue operation
// has produced new objs but either failed to apply the edit or was 
// canceled by the user, and the user doesn't want to do a rebuild
void ENCRestoreObjs()
{
	if (!IsEncLoaded ()) {
		return;
	}
	encOperation.RestoreOldObjs();
}

// Helper for proxy dll
class EditHelperImpl: public EditHelper
{
public:
    virtual bool ReadImage(     // read bytes from the image
        PADDR paddr, 
        ULONG cb, 
        BYTE* pb ) 
	{
		return (DHGetDebuggeeBytes (*paddr, cb, pb) == cb);
	}
		
    virtual bool WriteImage(    
        PADDR paddr, 
        ULONG cb, 
        BYTE* pb ) 
	{
		// Avoid using DHPutDebuggeeBytes, to prevent failure when this
		// is called by the DM thread (i.e., when reloading a dll that
		// has already been edited)
		if ( ADDR_IS_LI ( *paddr ) ) {
			SYFixupAddr ( paddr );
		}
		// check (native) addresses are 32-bit, flat, protected mode
		ASSERT( paddr->mode.fSql || paddr->mode.fJava || (ADDR_IS_FLAT( *paddr ) && ADDR_IS_OFF32( *paddr) && !ADDR_IS_REAL( *paddr ) ) );

		DWORD cbWritten;
		return ((OSDWriteMemory( hpidCurr, htidCurr, paddr, pb, cb, &cbWritten) == xosdNone) &&
			cb == cbWritten);
	}

    virtual bool FixupAddr(        
        PADDR paddr )
	{
		return !!SYFixupAddr(paddr);
	}

    virtual bool UnFixupAddr(        
        PADDR paddr )
	{
		return !!SYUnFixupAddr(paddr);
	}

	virtual DWORD ImageBase(HEXE hexe) 
	{
		return ::ImageBase(hexe);	   
	}

	virtual void Trace( int level, const char* lsz)
	{
		// enable only when ENCDebugging is ON 
		// to avoid displaying ENC engine 
		// messages all the time (could be annoying)
        if (runDebugParams.fENCDebugging) {
			TRACE("ENC Engine: %s\n",lsz);
		}	

		encOperation.OutputError(lsz, level);
	}

	virtual bool FindImage(   // return full path and base addr for named DLL
		const char* name, 
		char fullPath[ _MAX_PATH ],	// out
		PADDR paddrBase )			// out
	{
		XOSD xosd;
		LPMODULE_LIST pList;
		bool fRet = false;

		xosd = OSDGetModuleList(
			hpidCurr, 
			htidCurr,
			(char *)name,
			&pList
		);

		if (xosd == xosdNone && ModuleListCount(pList) == 1) {
			LPMODULE_ENTRY pme = FirstModuleEntry(pList);

			_tcscpy(fullPath, ModuleEntryName(pme));
			AddrInit(paddrBase,
					ModuleEntryEmi(pme),
					ModuleEntrySegment(pme),
					ModuleEntryBase(pme),
					ModuleEntryFlat(pme),
					TRUE, 
					FALSE,
					ModuleEntryReal(pme)
					);
			fRet = true;
		}

		LDSFfree(pList);
		return fRet;
	}
};

static EditHelperImpl encHelper;

static HINSTANCE hInstProxy = 0;
// 
// InitENCProxy: Load and initialize the enc dll.
// Registers PDBOpenValidateExAlt with SAPI
// 
bool InitENCProxy()
{
	static const TCHAR szProxyDll[] = _T("msenc10.dll");
	static const TCHAR szProxyEntry[] = _T("?theEnc@Enc@enc@@SAPAU12@XZ");
	// Entry point: public: static struct enc::Enc * __cdecl enc::Enc::theEnc(void)

	if (!(hInstProxy = LoadLibrary(szProxyDll))) {
		SetEncLoaded (false);
		return IsEncLoaded ();
	}

	*((FARPROC *)&pTheEnc) = GetProcAddress(hInstProxy, szProxyEntry);

	if (pTheEnc == NULL) {
		SetEncLoaded (false);
		return IsEncLoaded ();
	}

	pSymbolHandler = new ISymbolHandlerImpl(lpshf); 
	theEnc.FInit(0, &encHelper, pSymbolHandler);

	SetEncLoaded (true);
	return IsEncLoaded ();
}


// 
// UnInitENCProxy: UnLoad enc dll.
// 
bool UnInitENCProxy()
{
	if (!IsEncLoaded ()) {
		return true;
	}
	
	FreeLibrary(hInstProxy);
	hInstProxy = 0;
	pTheEnc = NULL;
	delete pSymbolHandler;
	pSymbolHandler = NULL;
	SetEncLoaded (false);
	return true;
}

void EncClearOutput()
{
	// Save current view 
	CView *pViewSav = GetCurView();

	// Prepare the build pane for ENC messages
	OutputWindowVwinClear(OwinGetBuildCommandId());
	OutputWindowShowVwin(OwinGetBuildCommandId());
	OutputWindowSelectVwin(OwinGetBuildCommandId(), TRUE);

	// Restore current view
	if (pViewSav) {
		pViewSav->SetFocus();
	}

}

void ENCOutputErr(EncError encErr, ...)
{
	if (encErr / 1000 <= runDebugParams.iENCTraceLevel) {
		CString strMsg;
		CString strFormat;
		va_list VarArguments;
		va_start(VarArguments, encErr);

		SubstituteBytes(strMsg, EncErrorText(strFormat, encErr), VarArguments);

		encOperation.OutputError(strMsg, encErr);		
	}
}

void ENCOutputErrorSummary()
{
	encOperation.OutputErrorSummary();
}

void OutputMessage(const char *lsz, BOOL fToolError)
{
	OutputWindowQueueHit(
		OwinGetBuildCommandId(), 
		lsz,
		fToolError, 
		TRUE
		);
}
	
void OutputMessage(int idsMsg)
{
	CString strMsg;
	strMsg.LoadString(idsMsg);
	OutputMessage(strMsg, FALSE);
}

static bool fCanDefer(HRESULT hResult)
{
	// Deferred attempts make sense only if hResult has one of 
	// the following values; otherwise the deferred attempt will 
	// fail with the same error
	return (hResult == E_FUNCBYTEMATCH || hResult == E_UNSAFEFRAMECOND);
}

void ApplyMUList(RefMUList & rlist)
{
	if (rlist != NULL) {
		for (int i=0; i < (*rlist).Count(); i++) {
			ADDR *paddr = &(*rlist)[i].addr;
			RefPtr< SimpleArray< BYTE > > data = (*rlist)[i].data;
			SYFixupAddr (paddr);
			TRACE("Writing %d bytes to 0x%x\n", (*data).Count(), GetAddrOff(*paddr)); 
			DHPutDebuggeeBytes(*paddr, (*data).Count(), (*data).Base());
		}
	}
}


static HMOD FindModFromName(HEXE hexe, LPCTSTR szMod)
{
	if (szMod) {
		HMOD hmod = (HMOD) 0;
		while (hmod = SHGetNextMod(hexe, hmod)) {
			if (!_tcsicmp(szMod, SHGetObjPath(hmod)))
				return hmod;
		}
	}
	return NULL;
}


static BOOL MRECALL
FEnumFile ( PMREUtil pmre, EnumFile & ef, EnumType et ) {
	if ( et == etSource ) {
		HEXE hexe = (HEXE) (ef.pvContext);
		HMOD hmod = FindModFromName(hexe, ef.szFileTrg);
		if (hmod) {
			ef.pvContext = PVOID(hmod);
			pmre->EnumDepFiles ( ef, FEnumFile );
		}
	}
	else {	
		HMOD hmod = (HMOD) (ef.pvContext);
		if (hmod && ef.szFileSrc) {
			// make sure we are adding a non null entry
			encSession.AddMod(hmod, ef.szFileSrc);
		}
	}
	return true;
}


static void LoadIdbFile(HEXE hexe, LPCTSTR szIdb)
{
	PMREngine	pmre;
	EC			ec;
	TCHAR		szErr[ cbErrMax ];

	if ( MREngine::FOpen ( &pmre, szIdb, ec, szErr, TRUE, FALSE ) && pmre ) {
		PMREUtil	pmreutil;
		pmre->QueryMreUtil ( pmreutil );
		pmreutil->EnumSrcFiles ( FEnumFile, NULL, PVOID(hexe) );
		pmreutil->FRelease();
		pmre->FClose(FALSE);
	}
}

static void LoadIdbFilesForExe(HEXE hexe)
{
	HMOD hmod = (HMOD) 0;
	// A list of the .idbs we have opened so far for this hexe
	// This is to avoid loading the same idb more than once 
	// (such a reload would be time consuming, but otherwise harmless)
	CStringList lstOpen;		

	while (hmod = SHGetNextMod(hexe, hmod)) {
		if (!SHModSupportsEC(hmod)) {
			continue;
		}
		
		const TCHAR * penv;
		VERIFY (penv = SHGetCompileEnv(hmod));

		StrEnviron strenv( penv );
		const TCHAR *lszPdb = strenv.szValueForTag( _T("pdb") );

		if (!lszPdb || lstOpen.Find(lszPdb)) {
			continue;
		}

		lstOpen.AddHead(lszPdb);

		CPath pathIdb;
		pathIdb.Create(lszPdb);
		pathIdb.ChangeExtension(".idb");

		LoadIdbFile(hexe, pathIdb);
	}
}


//
// Load idb files to determine header dependencies
//
static void LoadAllIdbFiles()
{
	HEXE hexe = (HEXE) 0;
	// first count the modules 
	int count = 0;
	while (hexe = SHGetNextExe(hexe)) {
		count ++;
	}

	// load idbs and provide some visual indication
	// on the progress. 
	CWaitCursor wc;
	ShowCursor(TRUE);
	CString szLoadingDeps;
	szLoadingDeps.LoadString(IDS_Enc_LoadingIdb);
	StatusBeginPercentDone(szLoadingDeps);
	int i = 0;
	while (hexe = SHGetNextExe(hexe)) {
		if (SHExeSupportsEC(hexe)) {
			LoadIdbFilesForExe(hexe);		
		}
		i++;
		StatusPercentDone( (int)(i * 100 / count) );
	}
	ShowCursor(FALSE);
	StatusPercentDone(100);
	StatusEndPercentDone();
}

DWORD ImageBase(HEXE hexe) 
{
	XOSD xosd;
	LSZ	lszExe = SHGetExeName(hexe);
	LPMODULE_LIST pList;
	DWORD dwBase = 0;

	xosd = OSDGetModuleList(
		hpidCurr, 
		htidCurr,
		lszExe,
		&pList
	);

	if (xosd == xosdNone && ModuleListCount(pList) == 1) {
		dwBase = ModuleEntryBase(FirstModuleEntry(pList));
	}

	LDSFfree(pList);
	return dwBase;	   
}

