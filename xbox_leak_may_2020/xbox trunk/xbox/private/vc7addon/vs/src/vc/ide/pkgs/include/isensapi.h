//	ISENSAPI.H
//
//
//	Interfaces for ISense stuff

#pragma once

#include <vsshell.h>
#include <bsc.h>
#include <cppsvc.h>
// Forward Declarations
//

class CStateSaver;
struct STATE_INFO;
struct BscEx;
struct POSDATA;

typedef 
enum _ERRORKIND
    {	ERROR_FATAL	= 0,
	ERROR_ERROR	= ERROR_FATAL + 1,
	ERROR_WARNING	= ERROR_ERROR + 1
    }	ERRORKIND;

struct IVCISense;
struct IVCSourceParser;
struct IVCAutoComplete;
struct IVsDropdownBarClient;
//struct IVCDropdpwnBarClient;
struct IWizBarInfo;
struct IVsTextView;

extern IVCISense *                  GetIVCISense                (void);
extern IVCAutoComplete *            GetIVCAutoComplete          (void);

__interface __declspec(uuid("{C3B426ED-AEE0-4d3c-8D2D-EE3928799251}")) IVCDropdownBarClient : public IVsDropdownBarClient
{
    STDMETHOD(UpdateCombos)(CComBSTR &bstrFunName, BOOL bInFunc, BOOL bInClass, USHORT uFuncIMod, TYP pTyp, ATR32 uAttr);
	STDMETHOD(OnNCBUpdate)(CodeElement *pElement);
};
typedef IVCDropdownBarClient* LPVCDROPDOWNBARCLIENT;


/////////////////////////////////////////////////////////////////////////////
//	IVCTaskProvider interface - used in taskprov.*

__interface __declspec(uuid("{1E5F7D5B-2698-4224-BD2C-1DA0CFC8922A}")) IVCTaskProvider : public IUnknown
{
	// IVCTaskProvider methods
    STDMETHOD(AddErrorTask)(const WCHAR *wszFileName, long iErrorID, ERRORKIND errKind, long iBegLine, long iBegCol, long iEndLine, long iEndCol, PCWSTR pszText, BOOL fDeferUntilRefresh);
    STDMETHOD(AddCommentTask)(long iLen,const WCHAR *pSTR, VSTASKPRIORITY  iPriority,UINT iLine, UINT iCol);
    STDMETHOD(RemoveErrorTasks)();
    STDMETHOD(RefreshTaskList)();
    STDMETHOD(OnTextEdit)(const POSDATA &posStart, const POSDATA &posOldEnd, const POSDATA &posNewEnd, IVsTextView *pVsTextView);
    STDMETHOD(ReloadForView)(IVsTextView *pVsTextView, BOOL fForce, BOOL *pfAddedToQueue);
    STDMETHOD(RemoveTasks)(IVsTextView *pVsTextView);
    STDMETHOD(SetBuffer)(IVsTextLines *pVsTextLines);
    STDMETHOD(ReParseForSquigglies)(IVsTextView *pVsTextView);
    STDMETHOD(ScanCommentTable)(UINT iStart, UINT iEnd, UINT iDelta, IVsTextView *pView);
    STDMETHOD(OnEndSquigglyParse)(DWORD dwHint);
    STDMETHOD(OnTerminateParseThread)();
};
typedef IVCTaskProvider* LPVCTASKPROVIDER;

__interface VCCodeClass;
/////////////////////////////////////////////////////////////////////////////
//	IVCISense interface - implemented by CISense, see isense subdir

__interface __declspec(uuid("{9B8BE586-02AC-437a-B552-905DE23288EF}")) IVCISense : public IUnknown
{
	// IVCISense methods
    STDMETHOD(GetVCTaskProvider)(IVCTaskProvider **ppVCTaskProvider);
	STDMETHOD(GetOverridesDispatch)(VCCodeClass *pClassIn, IDispatch **ppOut);
	STDMETHOD(GetMessageHandlerDispatch)(VCCodeClass *pClassIn, IDispatch **ppOut);
	STDMETHOD(GetEventHandlerDispatch)(VCCodeClass *pClassIn, IDispatch **ppOut);	
	STDMETHOD(GetControlHandlerDispatch)(VCCodeClass *pClassIn, BSTR bstrControlName, BSTR bstrControlType, IDispatch **ppOut);
	STDMETHOD(GetCommandHandlerDispatch)(VCCodeClass *pClassIn, BSTR bstrCommand, IDispatch **ppOut);
	STDMETHOD_(void, ReleaseAllPGrid)();	
    STDMETHOD(CreateWizardBar)(IWizBarInfo **ppOut);
    STDMETHOD(CreateDDBarClient)(IVsCodeWindow *m_pCodeWin, IWizBarInfo *m_pWizBarInfo, IVCDropdownBarClient **ppOut);
    STDMETHOD(SetCurrentDDBarClient)(IVCDropdownBarClient *ppIn, IVsTextView *pView, /*[in]*/ long iNewLine, IMOD pMod);
    STDMETHOD(SetLastLineAndMod)(long iNewLine, IMOD pMod);
    STDMETHOD_(IVCDropdownBarClient*, GetCurrentDDBarClient)();
};
typedef IVCISense* LPVCISENSE;

/////////////////////////////////////////////////////////////////////////////
//	IVCAutoComplete interface - implemented by CAutoComplete in autocomp.*
class CVCCMLocation;
__interface __declspec(uuid("{849AEF6B-CCAC-452c-80B4-C39970DBD134}")) IVCAutoComplete : public IUnknown
{
	// IVCAutoComplete methods
    STDMETHOD_(BOOL, RetrieveACInfo)(int *iBufferIndex, STATE_INFO **ppsi, BscEx** ppBscEx);
    STDMETHOD_(BOOL, GetActiveSI)(STATE_INFO **ppsi);
	STDMETHOD_(void, ResetActiveSI)(BOOL fSuccessWindow, BOOL fParseNeeded, BOOL fBeep);
	STDMETHOD(DoAutoComplete) (ULONG cmdFlags, ULONG charFlags, ULONG ichInsert, ULONG ilineInsert,
		BOOL fJingle, IVsTextView *pTextView, const WCHAR *wszFileName);
	STDMETHOD(OnChangeInTextEditor) (ULONG flags, ULONG ichInsert, ULONG ilineInsert,
		IVsTextView *pTextView, const WCHAR *wszFileName);
	STDMETHOD_(BOOL, OnACParseDone)(int iBufferIndex);
	STDMETHOD_(BOOL, FParamWindowUp)();
    STDMETHOD_(BOOL, FQuickInfoWindowUp)();
	STDMETHOD(DoAllReleases)();
    STDMETHOD_(BOOL, InitOnlyOnce)();
    STDMETHOD(GetCompletionSet)(IVsCompletionSet **ppSet);
    STDMETHOD_(BOOL, IsParseNeeded)();
    STDMETHOD(StartQuickInfo)(TextSpan *pTS, IVsTextView *pVsTextView, const WCHAR *wszFileName);
    STDMETHOD(OnCommand)(const GUID *pguidCmdGroup, DWORD nCmdID, DWORD nCmdExecOpt, VARIANTARG *pvaIn);
	STDMETHOD(GetCommentForIinst) (IINST iinst, BSTR *pbstrComment) PURE;
	STDMETHOD(GetCommentLocationForIinst)(IINST iinst, INT* pnCommentLoc, 
		CVCCMLocation *pvccmLocation, BOOL* pbEolComment) PURE;
};
typedef IVCAutoComplete* LPVCAUTOCOMPLETE;

