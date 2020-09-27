// Low-level codemgr routines (ie those requiring inside
// knowledge of CV400)

BOOL GetSourceFrompADDR(LPADDR,LPSTR,WORD,LONG FAR *,SHOFF FAR *,HSF FAR *);

CView * PASCAL MoveEditorToAddr(LPADDR);

HBPI FAR PASCAL AsyncBPCommitBP(LPPBP ppbp);

void FAR PASCAL AsyncBPDelete(HBPI hbpi);

XOSD PASCAL DoAllCallBacks(void);

VOID DoCallBacksUntil(void);

XOSD PASCAL CheckCallBack(BOOL);

// Get the HSF for a document
DIS_RET PASCAL GetHsfFromDoc(CDocument *pDoc, HSF FAR * lpHsf);

void GoUntil(PADDR paddr, HTHD hthd, int BpType, BOOL fGoException, BOOL fFlipScreen) ;

void PrimeDbgMessagePump(void);

void DbgMessagePump(BOOL fPump = TRUE);
