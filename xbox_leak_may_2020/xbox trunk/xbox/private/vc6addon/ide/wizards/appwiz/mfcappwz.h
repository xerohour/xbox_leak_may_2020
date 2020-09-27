// Wizard for generating a new app

#ifndef VS_PACKAGE
#include <path.h>
#include <ids.h>
#endif	// VS_PACKAGE

#ifdef VS_PACKAGE
#include <tabdlg.h>
#endif

#include <dlgbase.h>


#include "resource.h"
#include "extend.h"

#ifdef VS_PACKAGE
#include "devbld.h"
#include "awiface.h"
#include <bldguid.h>
#endif

#include <bldapi.h>

#ifdef VS_PACKAGE
#include "path.h"
#endif

// special defines
#define NUM_PROJECTS 3

#define NORMAL_MENUS 0
#define REBAR_MENUS 1

#ifdef VS_PACKAGE
//BUGBUG: TEMPORARY until we get hooked up with the build system again
#define BUILDER_EXT    ".dsp"
#endif

                    // project name is the "file name" edit field of COMMDLG
#define MAX_SHORT_TMP  32  // max size of short temporary string
#define NUM_DLG_CLASSES 10  // Total number of main sequential dlgs appwiz brings up

// Field verification
#define MAX_TAG     6       /* limit for 2 digits for synthetic name */
#define MAX_OLE_SHORT 15
#define MAX_PROGID 39
#define MAX_LONGNAME 40
#define MAX_FILTER 100

// Values fed to GetBitmapRect(); they represent the different sizes
//  of bitmaps appwiz uses.
enum
{
	BMPSIZE_STANDARD,		// Same size used for most bitmaps
	BMPSIZE_FLAG,			// Flag is bigger
	BMPSIZE_3D,				// 3D checkbox bitmap is smaller
	BMPSIZE_SUM,			// Union of first two sizes; used for invalidation
};

enum { PS_MFCNORMAL, PS_EXPLORER };


// CFont* GetStdFont(BOOL bBold);
BOOL IsMacInstalled();
BOOL IsMacOle();
BOOL IsMacOcx();
BOOL IsMacODBC();

/////////////////////////////////////////////////////////////////////////////
// Structures

struct AppWizTermStatus
{
	int nTermType;
	int nProjType;
	_TCHAR szFullName[MAX_PATH];
	CStringList strlDepFiles;
	CStringList strlNonPrjFiles;
	CStringList strlNonBldFiles;
	CStringList strlResFiles;
	CStringList strlHelpFiles;
	CStringList strlTemplFiles;
	CString strDefFile;
	//CString strClwFile;
	AppWizTermStatus* nextAppWizTermStatus;
};

extern AppWizTermStatus* pTermStatus;

// the class roots
enum
{
    classView,
    classApp,
    classFrame,
	classChildFrame,
    classDoc,
    classDlg,
    classRecset,
    classSrvrItem,
    classCntrItem,
    classIPFrame,
	classDlgAutoProxy,
    classLeftView,
    classWndView,
    classMax
};

// Stores names of classes, files, etc.
struct NAMES
{
    CString strClass[classMax];
    BOOL bCategoryEnabled[classMax];
    CString strBaseClass[classMax];
    CString strHFile[classMax];
    CString strIFile[classMax];
	CString strTitle;    // Text of title of main frame or dialog.

    // special doc fields
    CString strDocFileExt;
	CString strDocRegID;

	// Mac-specific
	CString strDocFileType;
	CString strDocFileCreator;
	CString strMacFilter;

	BOOL m_bUpdateNames;
};

enum { OLE_NO, OLE_CONTAINER, OLE_MSERVER, OLE_FSERVER, OLE_CSERVER, OLE_MAX };

enum { PROJTYPE_SDI, PROJTYPE_MDI, PROJTYPE_DLG, PROJTYPE_DLL, PROJTYPE_COUNT };

enum {
	dbNone = -1,
	dbOdbc,
	dbDao,
	dbOledb,
	dbAdo
};

// ProjOptions contains what used to be member variables of the dialog classes.
//  They're made global here so that communication between dialogs is easier,
//  and doesn't need the mediation of CPromptDlg (which is impossible now that
//  the dialogs are independent & sequential).
struct ProjOptions
{
    int  m_options;        // general user-set flags
	WORD m_projOptions;    // general user-set project flags
	BOOL m_bExtdll;		   // Are we an extension DLL?
    NAMES m_names;         // names of classes, files, etc.
    int m_nOleType;        // container, server, etc.
    BOOL m_bOleAutomation;
	BOOL m_bOcx;	 	   // Ole control container
	BOOL m_bActiveDocServer;   // ActiveX document server
	BOOL m_bActiveDocContainer;   // ActiveX document server
	BOOL m_bOleCompFile;
    BOOL m_bPrevMDI;       // Remembers state of MDI before Mini-Server is selected
    BOOL m_bDBSimple;      // Whether we're a non-doc simple database viewer.
    BOOL m_bDBHeader;      // Whether we're DB enabled at all.
	int m_nDataSource;           // TRUE if we are using DAO recordsets instead of ODBC
	BOOL m_bAutoDetect;	   // TRUE if recordset should auto detect dirty fields
	int m_nFrameStyles;    // Main window frame styles flags.
	int m_nMenuStyle;      // Main menu style.
	BOOL m_bSplitter;  	   // Splitter window in SDI app.
	int m_nChildFrameStyles;        // MDI Child frame style flags.
    int m_nProjType;       // SDI, MDI, Dlg, extDll.
    int m_nProjStyle;       // Explorer or Normal.
	UINT m_nSizeMRU;       // # entries in MRU file menu.
	
	CUserExtension m_UserExtension;	// Keeps a handle to user extension
	BOOL m_bMayBeAppWizTrack;		// If an extension is controlling appwiz, this
		// flag tells us whether to set symbols at the end (in SetSymbols).
	
    CDir m_ProjDir;		   // Gotten from CPromptDlg, used during generation
    CString m_strProj;     // ditto
	CString m_strProjTag;  // "Safe" version of project name (no double-byte characters)
	
	BOOL m_bUpdateRegID;   // Tracking flag for nonloc entry in doc template strings tab
	BOOL m_bATLSupport;		// ATL Support
	int  m_nStartRes;		// Resource Base #
	BOOL m_bNoDocView;		// No Doc view architechture support
};

extern ProjOptions projOptions;

BOOL IsMDI();
void SetMDI(BOOL bMDI);

struct AppWizTermStatus;
class CMainDlg;

// Main app
class CTheDLL : public CObject
{
public:
    CTheDLL();
#ifndef VS_PACKAGE
	int RunAppWiz(HWND hWnd, HBUILDER* phBld);
#endif	// VS_PACKAGE
    virtual BOOL InitInstance();
    virtual int ExitInstance();

	void InitAppWiz(HWND hWnd);
	int RunAppWizSteps();
	void ExitAppWiz();


    HKEY m_hKey;
	CString m_strAppwizDir;   // Directory from which appwiz was launched (and
	                          //  thus where it should search for localized dlls).
	BOOL m_bQuit;
	BOOL m_bDeleteMainDlg;
	CMainDlg* m_pMainDlg;
private:
	BOOL m_bFirstTime;

};

extern CTheDLL theDLL;

/////////////////////////////////////////////////////////////////////////////
// CMainDlg dialog

class CSeqDlg;
class CAppWizStepDlg;
class CMainDlg : public C3dDialog
{
friend class CSeqDlg;
friend class CAppWizStepDlg;
friend class CTheDLL;
// Construction
public:
    CMainDlg(CWnd* pParent = NULL); // standard constructor
	~CMainDlg();
    int Create(CWnd *pWnd)
        { return C3dDialog::Create(MAKEINTRESOURCE(IDD_MAINDLG), pWnd); }

    virtual BOOL PreTranslateMessage(MSG* pMsg);
    BOOL FinalProcessing();
	void RedrawBitmap(int nBmpSize);
	CAppWizStepDlg* ExtKeyFromID(AppWizDlgID nID);
	CSeqDlg* ExtSeqDlgFromKey(CAppWizStepDlg* pStepDlg);
	void SetNumberOfSteps(int nSteps);
    void InitializeOptions();
	void DestroyUserDlgs();
	void UserDeletedStandardStep(CAppWizStepDlg* pDlg);

	// Called by the child/sub-dialogs when F1 is pressed
	virtual void ChildRequestingHelp() { OnHelp(); }
	
	// This is either IDOK or IDC_END.  See "InterchangeEndAndOK" in
	//  mfcappwz.cpp
	int m_nRightmostBtn;

	CDialog* m_pConfirmDlg;
	CString m_strLastSubdir;
	
// Dialog Data
    //{{AFX_DATA(CMainDlg)
    enum { IDD = IDD_MAINDLG };
        // NOTE: the ClassWizard will add data members here
    //}}AFX_DATA

// Implementation
protected:
	void SetCurrentTrack();
	int m_nHelpIDs[NUM_DLG_CLASSES];  // See mfcappwz.cpp: CMainDlg::FillHelpIDs()
    CSeqDlg* m_pDialogClasses[NUM_DLG_CLASSES];
    int m_nCurrIndex;
    int* m_nCurrTrack;
    int m_nCurrLastIndex;
	BOOL m_bFirstTimeFileDlg;

	CAppWizStepDlg* m_pCurrUserStepDlg;
	CSeqDlg* m_pCurrUserSeqDlg;
	BOOL IsUserDlg();
	CAppWizStepDlg* m_pExtensionKeys[NUM_DLG_CLASSES];
	CTypedPtrMap<CMapPtrToPtr, CAppWizStepDlg*, CSeqDlg*> m_DlgMap;
	CTypedPtrMap<CMapPtrToPtr, CAppWizStepDlg*, void*> m_UserDlgsToDestroy;
	int m_nCurrUserStep;
	int m_nMaxUserSteps;

	int GetSeqDlgIndex(CSeqDlg* pSeqDlg);
	void InitializeUserVars();
	void GetAllSymbols();
	CDialog* GetInnerDlg();
	BOOL DismissCurrentDlg(BOOL bShouldDestroy = TRUE);
    void ActivateDlg(int nDlg);
    BOOL StartAppWizSteps();
    BOOL ActivateConfirmDlg();
	void SetFocusOnStep(CDialog* pDlg);
	void ActivateUserDlg(BOOL bDirIsNext);
	void UpdateStepDisplay();
	void ArrangeButtons(int nAddHelpButton);
	int m_nHasHelpButton;

	int m_nChildDlgLeft;
	void CalculateDimensions();
    virtual void PostNcDestroy();
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void AdjustEndAndOK(BOOL bIsLastDlg);
	void InterchangeEndAndOK();
	void DrawBitmap(CDC* pdc, CSeqDlg* pDlg, int nBmp);
	void DrawLowerBackground(CDC* pdc, CBrush* pBrush, CPen* pOldPen);

	CString m_strCWDProjGen;

		
    // Generated message map functions
    //{{AFX_MSG(CMainDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnClickedBack();
    afx_msg void OnDestroy();
    afx_msg void OnClickedEnd();
    virtual void OnCancel();
    virtual void OnOK();
    afx_msg void OnHelp();
	afx_msg void OnPaint();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	//}}AFX_MSG

	BOOL DoHelpInfo(HELPINFO* pInfo);
    BOOL Generate(CWnd* pProjStatus, CWnd* pFileStatus);  
    BOOL m_bWait;
    void FakeShellExecute(HWND hWnd, LPCSTR szVerb, LPCSTR szFile);
    LRESULT OnDDEAck(WPARAM wParam, LPARAM lParam);
    
    DECLARE_MESSAGE_MAP()

private:
    void InitDialogs();
    void DeleteDialogs();   
};

#define WM_GOTO_BEGINNING WM_USER+100

enum {WIN, MAC, POWERMAC, JAVA, XBOX, PLATFORM_MAX};
extern const TCHAR* szPlatforms[];
extern CMapStringToString g_PlatformsSelected; // Long names of selected platforms
extern CStringList g_strlPlatformsDisplayed;
extern CStringList strlProjTypes;


/////////////////////////////////////////////////////////////////////////////

int GetDefaultFrameStyles();
int GetDefaultChildFrameStyles();
int GetDefaultExtraChildFrameOptions();

void GetTag(CString& tag, LPCTSTR szRoot = NULL);
void SetOleSymbols();
void ResetOleSymbols();
void MyResetCheck(CButton* pBtn);
void UpdateTrackingNames(NAMES& names, const char* pszRoot);
void FillDefaultNames(NAMES& names, const char* pszRoot);
BOOL CanViewClassBeUsedWithSplitter(const CString &rstrViewClassName);
BOOL IsLearningBox();

#ifdef VS_PACKAGE
class CBldWiz : public IBuildWizard
{
public:
	
	STDMETHODIMP QueryInterface(REFIID /* riid */, LPVOID * /* ppv */) { return E_NOTIMPL; }
	STDMETHODIMP_(ULONG) AddRef() { return 0; }
	STDMETHODIMP_(ULONG) Release() { return 0; }

	STDMETHODIMP SetActiveTarget(HBLDTARGET hTarg, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetActiveTarget((DWORD) hTarg, (DWORD) hBld);
	}

	STDMETHODIMP SetDefaultTarget(HBLDTARGET hTarg, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetActiveTarget((DWORD) hTarg, (DWORD) hBld);
	}

	STDMETHODIMP GetTargetTypeNames(const CStringList **ppList, LPCSTR pchPlat, BOOL fUIDesc)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->GetTargetTypeNames((DWORD *) ppList, pchPlat, fUIDesc) ;
	}

	STDMETHODIMP AssignCustomBuildStep(LPCSTR pchCommand, LPCSTR pchOutput, 
		LPCSTR pchDescription, HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AssignCustomBuildStep(pchCommand, pchOutput, 
				pchDescription, (DWORD) hTarg, (DWORD) hFile, (DWORD) hBld);
	}

	STDMETHODIMP UnassignCustomBuildStep(HBLDTARGET hTarg, HBLDFILE hFile, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->UnassignCustomBuildStep((DWORD) hTarg, (DWORD) hFile, 
			(DWORD) hBld);
	}

	STDMETHODIMP SetToolSettings(HBLDTARGET hTarg, LPCSTR pchSettings, LPCSTR pchTool, 
		BOOL fAdd, BOOL fClear, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetToolSettings((DWORD) hTarg, pchSettings, 
			pchTool, fAdd, fClear, (DWORD) hBld);
	}

	STDMETHODIMP SetToolSettings(HBLDTARGET hTarg, HBLDFILE hFile, LPCSTR pchSettings, 
		LPCSTR pchTool, BOOL fAdd, BOOL fClear)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetToolSettings2((DWORD) hTarg, (DWORD) hFile,
				pchSettings, pchTool, fAdd, fClear);
	}

	STDMETHODIMP AddTarget(HBLDTARGET *pHandle, LPCSTR pchTarg, LPCSTR pchPlat, LPCSTR pchType, 
		BOOL fUIDesc, BOOL fDebug, TrgCreateOp trgop, HBLDTARGET hOtherTarg, SettingOp setop, 
		HBLDTARGET hSettingsTarg, BOOL fQuiet, BOOL fOutDir, BOOL fUseMFC, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddTarget((DWORD *) pHandle, (DWORD) pchTarg, 
			(DWORD) pchPlat, (DWORD) pchType, fUIDesc, fDebug, (DWORD) trgop, 
			(DWORD) hOtherTarg, (DWORD) setop, (DWORD) hSettingsTarg, fQuiet, 
			fOutDir, fUseMFC, (DWORD)hBld);
	}

	STDMETHODIMP AddDefaultTargets(LPCSTR pchPlat, LPCSTR pchType, 
		HBLDTARGET &hDebugTarg, HBLDTARGET &hReleaseTarg, BOOL fUIDesc, BOOL fOutDir,
		BOOL fUseMFC, HBLDTARGET hMirrorTarg, HBUILDER hBld, LPCSTR pchTarg)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddDefaultTargets(pchPlat, pchType, 
			(DWORD *) &hDebugTarg, (DWORD *) &hReleaseTarg, fUIDesc, fOutDir,
			fUseMFC, (DWORD) hMirrorTarg, (DWORD) hBld, pchTarg);
	}

	STDMETHODIMP GetTargetOutDir(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);	
		LPTSTR psz = NULL ;
		HRESULT hr = g_AppWizIFace.m_pBldWizX->GetTargetOutDir((DWORD) hTarg, 
				(DWORD *) psz, (DWORD) hBld);

		if (SUCCEEDED(hr))
			strOut = psz;

		return hr ;
	}

	STDMETHODIMP SetTargetOutDir(HBLDTARGET hTarg, CString &strOut, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);	
		return g_AppWizIFace.m_pBldWizX->SetTargetOutDir((DWORD) hTarg, 
				(DWORD *) strOut.GetBuffer(MAX_PATH), (DWORD) hBld);
		strOut.ReleaseBuffer();
	}

	STDMETHODIMP SetTargetIntDir(HBLDTARGET hTarg, CString &strInt, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);	
		return g_AppWizIFace.m_pBldWizX->SetTargetOutDir((DWORD) hTarg, 
				(DWORD *) strInt.GetBuffer(MAX_PATH), (DWORD) hBld);
		strInt.ReleaseBuffer();
	}

	STDMETHODIMP SetTargetIsAppWiz(HBLDTARGET hTarg, int iIsAppWiz, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetTargetIsAppWiz((DWORD) hTarg, 
					iIsAppWiz, (DWORD) hBld);
	}

	STDMETHODIMP SetTargetUseOfMFC(HBLDTARGET hTarg, int iUseOfMFC, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetTargetUseOfMFC((DWORD) hTarg, 
					iUseOfMFC, (DWORD) hBld);
	}

	STDMETHODIMP SetTargetDefExt(HBLDTARGET hTarg, CString &strExt, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);	
		LPTSTR psz = NULL ;
		HRESULT hr = g_AppWizIFace.m_pBldWizX->GetTargetOutDir((DWORD) hTarg, 
				(DWORD *) psz, (DWORD) hBld);

		if (SUCCEEDED(hr))
			strExt = psz;

		return hr ;
	}

	STDMETHODIMP SetBuildExclude(HBLDTARGET hTarg, HBLDFILE hFile, BOOL fExclude)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetBuildExclude((DWORD) hTarg, (DWORD) hFile, fExclude);
	}

	STDMETHODIMP AddFolder(HFOLDER *pHandle, HFILESET hFileSet, const TCHAR *pszFolder, HBUILDER hBld, 
		HFOLDER hFolder, const TCHAR * pszFilter)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddFolder((DWORD *) pHandle, (DWORD) hFileSet, (DWORD *) pszFolder,
				(DWORD) hBld, (DWORD) hFolder, (DWORD *) pszFilter);
	}

	STDMETHODIMP AddFile(HBLDFILE *pHandle, HFILESET hFileSet, const CPath *ppathFile, 
		int fSettings, HBUILDER hBld, HFOLDER hFolder)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddFile((DWORD *) pHandle, (DWORD) hFileSet, (DWORD *) ppathFile, 
			fSettings, (DWORD) hBld, (DWORD) hFolder);
	}

	STDMETHODIMP AddMultiFiles(HFILESET hFileSet, const CPtrList *plstpathFiles, 
		CPtrList *plstHBldFiles, int fSettings, HBUILDER hBld, HFOLDER hFolder)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddMultiFiles((DWORD) hFileSet, (DWORD *) plstpathFiles, 
				(DWORD *) plstHBldFiles, fSettings, (DWORD) hBld, (DWORD) hFolder);
	}

	STDMETHODIMP AddDependencyFile(HFILESET hFileSet, const CPath *ppathFile, HBUILDER hBld, HFOLDER hFolder)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->AddDependencyFile((DWORD) hFileSet, (DWORD *) ppathFile, 
			(DWORD) hBld, (DWORD) hFolder);
	}

	STDMETHODIMP GetFileSet(HBUILDER hBld, HBLDTARGET hTarg, HFILESET *pHandle)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->GetFileSet((DWORD) hBld, (DWORD) hTarg, (DWORD *) pHandle);
	}

	STDMETHODIMP GetAutoProject(HBUILDER hBld, IBuildProject** ppProject)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->GetAutoProject((DWORD) hBld, (DWORD *) ppProject);
	}

	STDMETHODIMP GetTargetUseOfMFC(HBLDTARGET hTarg, int *iUseOfMFC, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->GetTargetUseOfMFC((DWORD) hTarg, iUseOfMFC, (DWORD) hBld);
	}

	STDMETHODIMP SetUserDefinedDependencies(LPCSTR pchUserDeps, HBLDTARGET hTarg, 
		HBLDFILE hFile, HBUILDER hBld)
	{
		ASSERT(NULL != g_AppWizIFace.m_pBldWizX);
		return g_AppWizIFace.m_pBldWizX->SetUserDefinedDependencies(pchUserDeps, (DWORD) hTarg, 
			(DWORD) hFile, (DWORD) hBld);
	}
};

extern CBldWiz g_CBldWiz ;

#endif //VS_PACKAGE
