#include <afxwin.h>
#include <windowsx.h>
#include "resource.h"


BOOL CALLBACK MainDlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK LoadRunDlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam);
void CreateWebPageName(void);
void CreateWebPage(void);
void DetermineWhichTestsPassed(void);
void InstallTests(void);
void RunTests(void);
void SetCafeEnvironment(void);
void DeleteRegistryKey(LPCSTR pszKeyName);
void DeleteRegistrySubKeys(HKEY hKey);
void DeleteTree(CString strPath);
void AppendToLogs(CString strLogNew);
void GetRunsListViewDisplayInfo(NMLVDISPINFO *plvDispInfo);


// the different types of tests.
typedef enum {SNIFFS, SNAPS} TEST_TYPE;


// holds all the information about a test run.
class CTestRun
{
public:
	CString		m_strTitle;
	TEST_TYPE	m_TestType;
	BOOL		m_boolPublishOnWeb;
	BOOL		m_boolRerunFailedTests;
	CString		m_strVCRelease;
	CString		m_strVCLang;
	CString		m_strOS;
	CString		m_strOSRelease;
	CString		m_strOSLang;
	CString		m_strCPU;
	CString		m_strNumCPU;
	CString		m_strSpeed;
	CString		m_strRAM;
	CString		m_strDelay;
	CString		m_strPeriod;
	CString		m_strStepSleep;
	CString		m_strTestDir;
	int			m_intFirstTest;
	int			m_intLastTest;
	CString		m_strWebPageName;
} TestRun;


// holds all the information about a test run that can be loaded.
class CTestRunToLoad
{
public:
	CString			m_strTitle;
	CString			m_strVCRelease;
	CString			m_strVCLang;
	CString			m_strOS;
	CString			m_strOSLang;
	CString			m_strCPU;
	CString			m_strNumCPU;
	CString			m_strSpeed;
	CString			m_strRAM;
	CTestRunToLoad	*m_pNext;
};


// always points to the first run in the list so we can traverse it.
CTestRunToLoad *pTestRunsToLoadHome = NULL;
// points to the test run the user chose to load.
CTestRunToLoad *pTestRunChosen;


// holds all the information about a test.
class CTest
{
public:
	LPCSTR	m_szName;
	BOOL	m_boolPassed;
	LPCSTR	m_szGroup;
	LPCSTR	m_szDllPath;
};


// count of sniffs and snaps.
#define SNIFF_COUNT 11
#define SNAP_COUNT	13


// information on the sniffs and snaps.
CTest Tests[] =

{
	// sniffs.
	{"AutoComplete",			FALSE,	"Sniffs",			"AutoComplete\\AutoComplete.dll"},
	{"Browser",					FALSE,	"Sniffs",			"Browser\\Browser.dll"},	
	{"Data",					FALSE,	"Sniffs",			"Data\\Data.dll"},
	{"Debugger",				FALSE,	"Sniffs",			"Debugger\\Debugger.dll"},
	{"Editor",					FALSE,	"Sniffs",			"Editor\\Editor.dll"},
	{"ProjBld",					FALSE,	"Sniffs",			"ProjBld\\ProjBld.dll"},
	{"SDI",						FALSE,	"Sniffs",			"SDI\\SDI.dll"},
	{"Sys",						FALSE,	"Sniffs",			"Sys\\Sys.dll"},
	{"VRes",					FALSE,	"Sniffs",			"VRes\\VRes.dll"},
	{"VShell",					FALSE,	"Sniffs",			"VShell\\VShell.dll"},
	{"Wizards",					FALSE,	"Sniffs",			"Wizards\\Wizards.dll"},
	// debugger snaps.
	{"Breakpoints",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Breakpoints.dll"},
	{"CallStack",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\CallStack.dll"},
	{"Disassembly",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Disassembly.dll"},
	{"ExpressionEvaluation",	FALSE,	"Debugger Snaps",	"Debugger\\Core\\EE.dll"},
	{"Execution",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Execution.dll"},
	{"Memory",					FALSE,	"Debugger Snaps",	"Debugger\\Core\\Memory.dll"},
	{"Namespaces",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Namespaces.dll"},
	{"NLG",						FALSE,	"Debugger Snaps",	"Debugger\\Core\\NLG.dll"},
	{"Registers",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Registers.dll"},
	{"ReturnValues",			FALSE,	"Debugger Snaps",	"Debugger\\Core\\ReturnValues.dll"},
	{"Threads",					FALSE,	"Debugger Snaps",	"Debugger\\Core\\Threads.dll"},
	{"Templates",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Templates.dll"},
	{"Variables",				FALSE,	"Debugger Snaps",	"Debugger\\Core\\Variables.dll"},
/*
	TODO: need to enable these when we have support for picking specific groups of snaps.
	// projbld snaps.
	{"BatchBuild",				FALSE,	"ProjBld Snaps",	"ProjBld\\batchbld.dll"},
	{"Build",					FALSE,	"ProjBld Snaps",	"ProjBld\\build.dll"},
	{"Conversion",				FALSE,	"ProjBld Snaps",	"ProjBld\\convers.dll"},
	{"CustomBuild",				FALSE,	"ProjBld Snaps",	"ProjBld\\customb.dll"},
	{"DefaultProject",			FALSE,	"ProjBld Snaps",	"ProjBld\\defaultp.dll"},
	{"Dependencies",			FALSE,	"ProjBld Snaps",	"ProjBld\\depend.dll"},
	{"ExportMakefile",			FALSE,	"ProjBld Snaps",	"ProjBld\\exportmf.dll"},
	{"External",				FALSE,	"ProjBld Snaps",	"ProjBld\\external.dll"},
	{"Options",					FALSE,	"ProjBld Snaps",	"ProjBld\\options.dll"},
	{"Sub-projects",			FALSE,	"ProjBld Snaps",	"ProjBld\\subprjs.dll"},
	// resedit snaps.
	{"BindCtrl",				FALSE,	"ResEdit Snaps",	"ResEdit\\BindCtrl.dll"},
	// wizards snaps.
	{"Components",				FALSE,	"Wizards Snaps",	"Wizards\\Components.dll"},
*/
};


// count of various main dlg combo box items.
#define TEST_TYPE_COUNT		2
#define VC_RELEASE_COUNT	6
#define LANG_COUNT			3
#define OS_COUNT			7
#define OS_RELEASE_COUNT	9
#define CPU_COUNT			7


// the options for various combo boxes in the dlg.
char *szTestType[TEST_TYPE_COUNT]	= {"Sniffs", "Snaps"};
char *szVCRelease[VC_RELEASE_COUNT]	= {"", "SP1", "SP2", "SP3", "SP4", "SP4-PP"};
char *szLang[LANG_COUNT]			= {"English", "German", "Japanese"};
char *szOS[OS_COUNT]				= {"Millennium", "NT4 Workstation", "NT4 Server",
									   "Win2k Pro", "Win2k Server", "Win95", "Win98"};
char *szOSRelease[OS_RELEASE_COUNT]	= {"", "OSR2", "SE", "SP1", "SP2", "SP3", "SP4", "SP5", "SP6"};
char *szCPU[CPU_COUNT]				= {"Itanium", "K6", "K7", "Pentium", "Pentium II", "Pentium III", "WNI"};


// for referencing each control on the main dlg in the MainControls array below.
enum
{
	EDIT_TITLE,
	RADIO_SNIFFS,
	RADIO_SNAPS,
	CHECK_INSTALL_TESTS,
	CHECK_PUBLISH_RESULTS_ON_WEB,
	CHECK_RERUN_FAILED_TESTS,
	COMBO_VC_RELEASE,
	COMBO_VC_LANG,
	COMBO_OS,
	COMBO_OS_RELEASE,
	COMBO_OS_LANG,
	COMBO_CPU,
	EDIT_NUM_CPU,
	EDIT_SPEED,
	EDIT_RAM,
	EDIT_DELAY,
	EDIT_PERIOD,
	EDIT_STEPSLEEP,
	EDIT_LOG,
} MAIN_CONTROLS;


// holds all the information about a dlg control.
class CControl
{
public:
	HWND	m_hWnd;
	int		m_intID;
};


// count of the controls on the main dlg.
#define MAIN_CONTROL_COUNT 19


// information on the controls on the main dlg.
CControl MainControls[] = 
{
	{NULL, IDC_EDIT_TITLE},
	{NULL, IDC_RADIO_SNIFFS},
	{NULL, IDC_RADIO_SNAPS},
	{NULL, IDC_CHECK_INSTALL_TESTS},
	{NULL, IDC_CHECK_PUBLISH_RESULTS_ON_WEB},
	{NULL, IDC_CHECK_RERUN_FAILED_TESTS},
	{NULL, IDC_COMBO_VC_RELEASE},
	{NULL, IDC_COMBO_VC_LANG},
	{NULL, IDC_COMBO_OS},
	{NULL, IDC_COMBO_OS_RELEASE},
	{NULL, IDC_COMBO_OS_LANG},
	{NULL, IDC_COMBO_CPU},
	{NULL, IDC_EDIT_NUM_CPU},
	{NULL, IDC_EDIT_SPEED},
	{NULL, IDC_EDIT_RAM},
	{NULL, IDC_EDIT_DELAY},
	{NULL, IDC_EDIT_PERIOD},
	{NULL, IDC_EDIT_STEPSLEEP},
	{NULL, IDC_EDIT_LOG},
};


// we want to log all over the place so it's just easier to make these global rather than passing them around.
FILE *pfileDebugLog;
CString strLog, strLogNew;
// the main dlg proc needs this to invoke the Load Run dlg.
HINSTANCE ghInstance;

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	ghInstance = hInstance;
	return DialogBox(hInstance, MAKEINTRESOURCE(IDD_VC6_TEST_RUNNER), NULL, MainDlgProc);
}


BOOL CALLBACK MainDlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

{
	int i;

	switch(msg)		

	{
		case WM_INITDIALOG:
		{
			// get the handles to the windows of all the controls so we can get and set their values.
			for(i = 0; i < MAIN_CONTROL_COUNT; i++)
				MainControls[i].m_hWnd = GetDlgItem(hWndDlg, MainControls[i].m_intID);
			// populate all the combo boxes.
			for(i = 0; i < VC_RELEASE_COUNT; i++)
				SendMessage(MainControls[COMBO_VC_RELEASE].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szVCRelease[i]);
			for(i = 0; i < LANG_COUNT; i++)
				SendMessage(MainControls[COMBO_VC_LANG].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szLang[i]);
			for(i = 0; i < OS_COUNT; i++)
				SendMessage(MainControls[COMBO_OS].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szOS[i]);
			for(i = 0; i < OS_RELEASE_COUNT; i++)
				SendMessage(MainControls[COMBO_OS_RELEASE].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szOSRelease[i]);
			for(i = 0; i < LANG_COUNT; i++)
				SendMessage(MainControls[COMBO_OS_LANG].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szLang[i]);
			for(i = 0; i < CPU_COUNT; i++)
				SendMessage(MainControls[COMBO_CPU].m_hWnd, CB_ADDSTRING, 0, (LPARAM)szCPU[i]);
			// make installing the tests the default.
			SendMessage(MainControls[CHECK_INSTALL_TESTS].m_hWnd, BM_SETCHECK, 1, 0);
			// pre-select the sniffs option, and pre-set the timing settings to the most popular values.
			CheckRadioButton(hWndDlg, IDC_RADIO_SNIFFS, IDC_RADIO_SNAPS, IDC_RADIO_SNIFFS);
			SetWindowText(MainControls[EDIT_DELAY].m_hWnd, "40");
			SetWindowText(MainControls[EDIT_PERIOD].m_hWnd, "45");
			SetWindowText(MainControls[EDIT_STEPSLEEP].m_hWnd, "100");
			return TRUE;
		}

		case WM_COMMAND:
		
		{
			switch(LOWORD(wParam))
			
			{
				case IDOK:

				{
					// get the system directory so we know what the system drive root is, 
					// which is where tests are installed and the debug log is written.
					CString strSystemDir; 
					GetSystemDirectory(strSystemDir.GetBuffer(_MAX_PATH), _MAX_PATH);
					strSystemDir.ReleaseBuffer();
					// the system drive is the first 2 characters of the system directory.
					CString strSystemDrive = strSystemDir.Left(2);
					// this is for debugging problems with runs.
					pfileDebugLog = fopen(strSystemDrive + "\\VC6TestRunner.log", "w");
					AppendToLogs("Starting Run.");

					// get the test type (Sniffs or Snaps).
					TestRun.m_TestType = (TEST_TYPE)SendMessage(MainControls[RADIO_SNAPS].m_hWnd, BM_GETCHECK, 0, 0);
					// we create either VC6Sniffs or VC6Snaps at the system drive root.
					TestRun.m_strTestDir = strSystemDrive + "\\VC6" + szTestType[TestRun.m_TestType];
					// get the timing settings.
					GetWindowText(MainControls[EDIT_DELAY].m_hWnd, TestRun.m_strDelay.GetBuffer(1024), 1023);
					TestRun.m_strDelay.ReleaseBuffer();
					GetWindowText(MainControls[EDIT_PERIOD].m_hWnd, TestRun.m_strPeriod.GetBuffer(1024), 1023);
					TestRun.m_strPeriod.ReleaseBuffer();
					GetWindowText(MainControls[EDIT_STEPSLEEP].m_hWnd, TestRun.m_strStepSleep.GetBuffer(1024), 1023);
					TestRun.m_strStepSleep.ReleaseBuffer();
					// determine whether or not the results will be published on the web.
					TestRun.m_boolPublishOnWeb = 
						SendMessage(MainControls[CHECK_PUBLISH_RESULTS_ON_WEB].m_hWnd, BM_GETCHECK, 0, 0);
					// determine whether or not we are only re-running tests that failed.
					TestRun.m_boolRerunFailedTests = 
						SendMessage(MainControls[CHECK_RERUN_FAILED_TESTS].m_hWnd, BM_GETCHECK, 0, 0);
					
					// if we are re-running failed tests, then publishing on the web is implied.
					if(TestRun.m_boolRerunFailedTests)
						TestRun.m_boolPublishOnWeb = TRUE;

					// get the language of vc that we are targetting. check for a blank entry.
					int intComboItem;

					if((intComboItem = SendMessage(MainControls[COMBO_VC_LANG].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
						TestRun.m_strVCLang	= szLang[intComboItem];

					// we only need the other info if we publish results on the web.
					if(TestRun.m_boolPublishOnWeb)	
					
					{
						// get the edit box settings.
						GetWindowText(MainControls[EDIT_TITLE].m_hWnd, TestRun.m_strTitle.GetBuffer(1024), 1023);
						TestRun.m_strTitle.ReleaseBuffer();
						GetWindowText(MainControls[EDIT_NUM_CPU].m_hWnd, TestRun.m_strNumCPU.GetBuffer(1024), 1023);
						TestRun.m_strNumCPU.ReleaseBuffer();
						GetWindowText(MainControls[EDIT_SPEED].m_hWnd, TestRun.m_strSpeed.GetBuffer(1024), 1023);
						TestRun.m_strSpeed.ReleaseBuffer();
						GetWindowText(MainControls[EDIT_RAM].m_hWnd, TestRun.m_strRAM.GetBuffer(1024), 1023);
						TestRun.m_strRAM.ReleaseBuffer();		
						// get the combo box settings. check for a blank entry.						
						if((intComboItem = SendMessage(MainControls[COMBO_VC_RELEASE].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
							TestRun.m_strVCRelease = szVCRelease[intComboItem];
						if((intComboItem = SendMessage(MainControls[COMBO_OS].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
							TestRun.m_strOS = szOS[intComboItem];
						if((intComboItem = SendMessage(MainControls[COMBO_OS_RELEASE].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
							TestRun.m_strOSRelease = szOSRelease[intComboItem];
						if((intComboItem = SendMessage(MainControls[COMBO_OS_LANG].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
							TestRun.m_strOSLang = szLang[intComboItem];
						if((intComboItem = SendMessage(MainControls[COMBO_CPU].m_hWnd, CB_GETCURSEL, 0, 0)) != -1)
							TestRun.m_strCPU = szCPU[intComboItem];
					
						// create the name of the webpage from the info specified for the run.
						CreateWebPageName();
						
						// calculate the first and last test to run from the Tests list (above), 
						// based on which group of tests were chosen.
						TestRun.m_intFirstTest = (TestRun.m_TestType == SNIFFS) ? 0 : SNIFF_COUNT;
						TestRun.m_intLastTest = (TestRun.m_TestType == SNIFFS) ? SNIFF_COUNT : SNIFF_COUNT + SNAP_COUNT;

						// if we're not re-running failed tests, then we need to create a new page for a new run.
						// otherwise we need to scan at the existing page to see which tests passed and only run 
						// the failed ones.
						if(!TestRun.m_boolRerunFailedTests)
							CreateWebPage();
						else
							DetermineWhichTestsPassed();
					}

					// install the tests if specified.
					if(SendMessage(MainControls[CHECK_INSTALL_TESTS].m_hWnd, BM_GETCHECK, 0, 0))
						InstallTests();

					RunTests();

					// close the debug log.
					fclose(pfileDebugLog);
					return TRUE;
				}
				
				case IDC_BUTTON_LOAD_RUN:

					// the Load Run dlg uses a list view control, so we have to initialize the common controls dll.
					INITCOMMONCONTROLSEX icex;
					icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
					icex.dwICC  = ICC_LISTVIEW_CLASSES;
					InitCommonControlsEx(&icex); 

					DialogBox(ghInstance, MAKEINTRESOURCE(IDD_LOAD_RUN), NULL, LoadRunDlgProc);
				
					if(pTestRunChosen)

					{
						// populate the main dlg controls with the details of the run the user chose to load.
						SetWindowText(MainControls[EDIT_TITLE].m_hWnd, pTestRunChosen->m_strTitle);
						ComboBox_SelectString(MainControls[COMBO_VC_RELEASE].m_hWnd, -1, pTestRunChosen->m_strVCRelease);
						ComboBox_SelectString(MainControls[COMBO_VC_LANG].m_hWnd, -1, pTestRunChosen->m_strVCLang);

						// we store the os and os release as one field in the html page, so we need to break it up here
						// in order to populate the os and os release controls of the main dlgs.
						int intSpaceBetweenOSAndRelease;
						int intOSLength;

						// search for each of the release strings to see if one is specified.
						for(i = 1; i < 9; i++)
						{
							if((intSpaceBetweenOSAndRelease = 
								pTestRunChosen->m_strOS.Find(CString(" ") + szOSRelease[i])) != -1)
								break;
						}
						
						// check if we found one of the release strings.
						if(intSpaceBetweenOSAndRelease == -1)
						{
							// no release string was found, so the os is the entire length of the string.
							intOSLength = pTestRunChosen->m_strOS.GetLength();
							// 0 index specifies no os release ("").
							ComboBox_SetCurSel(MainControls[COMBO_OS_RELEASE].m_hWnd, 0);
						}
						
						else
						
						{
							// a release string was found, so the os is the part of the string up to the space
							// that separates the os from the release.
							intOSLength = intSpaceBetweenOSAndRelease;

							// the release is the rest of the string after the space that separates the os from the release.
							ComboBox_SelectString(
								MainControls[COMBO_OS_RELEASE].m_hWnd, -1, 
								pTestRunChosen->m_strOS.Mid(intSpaceBetweenOSAndRelease + 1));
						}

						// populate the os control based on the length of the os string calculated above.
						ComboBox_SelectString(
							MainControls[COMBO_OS].m_hWnd, -1, pTestRunChosen->m_strOS.Mid(0, intOSLength));

						// populate the rest of the main dlg controls with the details of the run the user chose to load.
						ComboBox_SelectString(MainControls[COMBO_OS_LANG].m_hWnd, -1, pTestRunChosen->m_strOSLang);
						ComboBox_SelectString(MainControls[COMBO_CPU].m_hWnd, -1, pTestRunChosen->m_strCPU);
						SetWindowText(MainControls[EDIT_NUM_CPU].m_hWnd, pTestRunChosen->m_strNumCPU);
						SetWindowText(MainControls[EDIT_SPEED].m_hWnd, pTestRunChosen->m_strSpeed);
						SetWindowText(MainControls[EDIT_RAM].m_hWnd, pTestRunChosen->m_strRAM);
					}

					// clean-up the runs to load list.
					CTestRunToLoad *pTestRunToLoad1;
					pTestRunToLoad1 = pTestRunsToLoadHome;
					CTestRunToLoad *pTestRunToLoad2;

					while(pTestRunToLoad1)
					{
						pTestRunToLoad2 = pTestRunToLoad1;
						pTestRunToLoad1 = pTestRunToLoad1->m_pNext;
						delete pTestRunToLoad2;
					}

					return TRUE;

				case IDCANCEL:
					return SendMessage(hWndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
			}
		}
		case WM_CLOSE:
			return EndDialog(hWndDlg, 0);
		default:
			return 0;
	}
}


BOOL CALLBACK LoadRunDlgProc(HWND hWndDlg, UINT msg, WPARAM wParam, LPARAM lParam)

{
	static HWND hWndListRuns;

	switch(msg)		

	{
		case WM_INITDIALOG:
		
		{
			// get the handle for the Runs listview so we can add columns and items to it.
			hWndListRuns = GetDlgItem(hWndDlg, IDC_LIST_RUNS);
			
			// settings common to all the columns.
			LV_COLUMN ListViewColumn;
			ListViewColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			ListViewColumn.fmt = LVCFMT_LEFT;
			// add each column.
			ListViewColumn.pszText = "RAM";
			ListViewColumn.iSubItem = 8;
			ListViewColumn.cx = 40;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "SPEED";
			ListViewColumn.iSubItem = 7;
			ListViewColumn.cx = 50;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "# CPU";
			ListViewColumn.iSubItem = 6;
			ListViewColumn.cx = 50;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "CPU";
			ListViewColumn.iSubItem = 5;
			ListViewColumn.cx = 75;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "OS LANG";
			ListViewColumn.iSubItem = 4;
			ListViewColumn.cx = 75;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "OS";
			ListViewColumn.iSubItem = 3;
			ListViewColumn.cx = 150;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "VC6 LANG";
			ListViewColumn.iSubItem = 2;
			ListViewColumn.cx = 75;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "VC6";
			ListViewColumn.iSubItem = 1;
			ListViewColumn.cx = 75;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
			ListViewColumn.pszText = "TITLE";
			ListViewColumn.iSubItem = 0;
			ListViewColumn.cx = 295;
			ListView_InsertColumn(hWndListRuns, 0, &ListViewColumn);
	
			// settings common to each item in the list.
			LV_ITEM ListViewItem;
			ListViewItem.mask = LVIF_TEXT | LVIF_STATE;
			ListViewItem.state = 0;
			ListViewItem.stateMask = 0;
			ListViewItem.iSubItem = 0;
			ListViewItem.pszText = LPSTR_TEXTCALLBACK;
		
			// the first run to load hasn't been allocated yet.
			pTestRunsToLoadHome = NULL;
			CTestRunToLoad *pTestRunToLoad = NULL;
			// the index of the nex t item to add to the listview.
			int intListViewItemIndex = 0;

			// open the runs page so we can read in the info for all the runs.
			FILE *pfileRunsPage = fopen("\\\\MSVC\\VCQA\\VC6TestRuns\\VC6TestRuns.htm", "r");					
			CString strLine;
			int intDelimLeft, intDelimRight;

			// read all the lines up until the section where the runs get added.
			while(strLine.Find("add run here") == -1)	
			{
				fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
				strLine.ReleaseBuffer();
			}

			// look for runs until we reach the end of the file.
			while(!feof(pfileRunsPage))
			
			{
				// read until we find the beginning of a run (each row is a run).
				while((strLine.Find("<tr>") == -1) && !feof(pfileRunsPage))	
				{
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
				}

				// if we didn't reach the end of the file, then we found a run.
				if(!feof(pfileRunsPage))
				
				{
					// insert an item for it in the list view.
					ListViewItem.iItem = intListViewItemIndex++;
					ListView_InsertItem(hWndListRuns, &ListViewItem);

					// allocate the new run to load.
					if(!pTestRunsToLoadHome)
					{
						pTestRunToLoad = new CTestRunToLoad();
						pTestRunsToLoadHome = pTestRunToLoad;
					}
					else
					{
						pTestRunToLoad->m_pNext = new CTestRunToLoad();
						pTestRunToLoad = pTestRunToLoad->m_pNext;
					}

					pTestRunToLoad->m_pNext = NULL;

					// read the title line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();

					// preceding the title will be either "<font color=red>" or "<font color=green>".
					// we want the text between that and the "</font>".
					intDelimLeft = strLine.Find("red>");
					
					if(intDelimLeft == -1)
						intDelimLeft = strLine.Find("een>");

					intDelimRight = strLine.Find("</font>");
					pTestRunToLoad->m_strTitle = strLine.Mid(intDelimLeft + 4, intDelimRight - (intDelimLeft + 4));
					// read the vc release line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strVCRelease = strLine.Mid(4, intDelimRight - 4);
					// read the vc lang line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strVCLang = strLine.Mid(4, intDelimRight - 4);
					// read the os line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strOS = strLine.Mid(4, intDelimRight - 4);
					// read the os lang line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strOSLang = strLine.Mid(4, intDelimRight - 4);
					// read the cpu line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strCPU = strLine.Mid(4, intDelimRight - 4);
					// read the num cpu line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strNumCPU = strLine.Mid(4, intDelimRight - 4);
					// read the speed line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();							
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strSpeed = strLine.Mid(4, intDelimRight - 4);
					// read the ram line.
					fgets(strLine.GetBuffer(1024), 1023, pfileRunsPage);
					strLine.ReleaseBuffer();							
					intDelimRight = strLine.Find("</td>");
					pTestRunToLoad->m_strRAM = strLine.Mid(4, intDelimRight - 4);
				}
			}

			fclose(pfileRunsPage);
			return TRUE;
		}
		
		case WM_NOTIFY:
		{
			switch(((LPNMHDR)lParam)->code) 
			{ 
			// fill in the items of the listview on the Load Run dlg.
			case LVN_GETDISPINFO: 
				GetRunsListViewDisplayInfo((NMLVDISPINFO *)lParam);
				return 0;
			default:
				return 0;
			}			
		}

		case WM_COMMAND:
		
		{
			switch(LOWORD(wParam))
			
			{
				case IDOK:
				
				{
					// get the run that was chosen to load.
					int intItem = ListView_GetSelectionMark(hWndListRuns);

					if(intItem >= 0)

					{
						pTestRunChosen = pTestRunsToLoadHome;
						
						// the main dlg procedure will extract info from pTestRunChosen to populate the main dlg.
						for(int i = 0; i < intItem; i++)
							pTestRunChosen = pTestRunChosen->m_pNext;

						return SendMessage(hWndDlg, WM_CLOSE, 0, 0);
					}

					return TRUE;
				}

				case IDCANCEL:
					return SendMessage(hWndDlg, WM_CLOSE, 0, 0);
				default:
					return 0;
			}
		}

		case WM_CLOSE:
			return EndDialog(hWndDlg, 0);
		default:
			return 0;
	}
}


void GetRunsListViewDisplayInfo(NMLVDISPINFO *plvDispInfo) 

{
	static char sz[1024];
	CTestRunToLoad *pTestRunsToLoad = pTestRunsToLoadHome;

	// advance to the run that we need to get display info for.
	for(int i = 0; i < plvDispInfo->item.iItem; i++)
		pTestRunsToLoad = pTestRunsToLoad->m_pNext;

	switch(plvDispInfo->item.iSubItem)
	{
	case 0:
		strcpy(sz, pTestRunsToLoad->m_strTitle);
		break;
	case 1:
		strcpy(sz, pTestRunsToLoad->m_strVCRelease);
		break;
	case 2:
		strcpy(sz, pTestRunsToLoad->m_strVCLang);
		break;
	case 3:
		strcpy(sz, pTestRunsToLoad->m_strOS);
		break;
	case 4:
		strcpy(sz, pTestRunsToLoad->m_strOSLang);
		break;
	case 5:
		strcpy(sz, pTestRunsToLoad->m_strCPU);
		break;
	case 6:
		strcpy(sz, pTestRunsToLoad->m_strNumCPU);
		break;
	case 7:
		strcpy(sz, pTestRunsToLoad->m_strSpeed);
		break;
	case 8:
		strcpy(sz, pTestRunsToLoad->m_strRAM);
		break;
	}

	plvDispInfo->item.pszText = sz;
} 


void CreateWebPageName()

{
	// remove spaces from the title, os, and cpu fields since they are used in the web page name, and we don't
	// want it to have spaces since it can sometimes be a pain in ie.
	int i;
	CString strTitleWithoutSpaces = TestRun.m_strTitle;
	CString strOSWithoutSpaces = TestRun.m_strOS;
	CString strCPUWithoutSpaces = TestRun.m_strCPU;

	for(i = 0; i < strTitleWithoutSpaces.GetLength(); i++)
	{
		if(strTitleWithoutSpaces[i] == ' ')
			strTitleWithoutSpaces.Delete(i, 1);
	}

	for(i = 0; i < strOSWithoutSpaces.GetLength(); i++)
	{
		if(strOSWithoutSpaces[i] == ' ')
			strOSWithoutSpaces.Delete(i, 1);
	}

	for(i = 0; i < strCPUWithoutSpaces.GetLength(); i++)
	{
		if(strCPUWithoutSpaces[i] == ' ')
			strCPUWithoutSpaces.Delete(i, 1);
	}

	// create the webpage name.
	CString strWebPageName;
	
	strWebPageName.Format("%s_VC6%s%s_%s%s%s_%s%s%smhz%smb", strTitleWithoutSpaces, TestRun.m_strVCRelease, 
		TestRun.m_strVCLang, strOSWithoutSpaces, TestRun.m_strOSRelease, TestRun.m_strOSLang, TestRun.m_strNumCPU,
		strCPUWithoutSpaces, TestRun.m_strSpeed, TestRun.m_strRAM);

	// we need the web page name later in RunTests() in order to copy logs out to the website.
	TestRun.m_strWebPageName = strWebPageName;
}


void CreateWebPage()

{
	// create the webpage's directory and the webpage.
	CreateDirectory("\\\\MSVC\\VCQA\\VC6TestRuns\\" + TestRun.m_strWebPageName, NULL);

	FILE *pfileWebPage = fopen(
		"\\\\MSVC\\VCQA\\VC6TestRuns\\" + TestRun.m_strWebPageName + "\\" + TestRun.m_strWebPageName + ".htm", "w");

	// write the header information.
	fprintf(pfileWebPage, "<html>\n");
	fprintf(pfileWebPage, "<center>\n");
	fprintf(pfileWebPage, "<h1>%s</h1>\n", TestRun.m_strTitle);
	fprintf(pfileWebPage, "<b>VC6 %s %s</b><br>\n", TestRun.m_strVCRelease, TestRun.m_strVCLang);
	fprintf(pfileWebPage, "<b>%s %s %s</b><br>\n", TestRun.m_strOS, TestRun.m_strOSRelease, TestRun.m_strOSLang);
	fprintf(pfileWebPage, "<b>%s %s %smhz %smb</b><br>\n", 
		TestRun.m_strNumCPU, TestRun.m_strCPU, TestRun.m_strSpeed, TestRun.m_strRAM);
	fprintf(pfileWebPage, "</center>\n");
	fprintf(pfileWebPage, "<hr>\n");
	// initialize the group to the first group of tests.
	CString strGroup = Tests[TestRun.m_intFirstTest].m_szGroup;
	// write out the header and start the results table for the first group of tests.
	fprintf(pfileWebPage, "<b><h3>%s</h3></b>\n", strGroup);
	fprintf(pfileWebPage, "<table width=25%% border=1>\n");
	fprintf(pfileWebPage, "<tr><td><b>Test</b></td><td><b>Result</b></td></tr>\n");

	// loop through the list of tests to write on the webpage.
	for(int intTest = TestRun.m_intFirstTest; intTest < TestRun.m_intLastTest; intTest++)
	
	{
		// check if we've entered a new group of tests (applies to snaps only).
		if(Tests[intTest].m_szGroup != strGroup)
		{
			// terminate the table for the previous group of tests.
			fprintf(pfileWebPage, "</table>\n\n");
			fprintf(pfileWebPage, "<br>\n");
			// write the header and initiate the results table for the next group.
			fprintf(pfileWebPage, "<b><h3>%s</h3></b>\n", Tests[intTest].m_szGroup);
			fprintf(pfileWebPage, "<table width=25%% border=1>\n");
			fprintf(pfileWebPage, "<tr><td><b>Test</b></td><td><b>Result</b></td></tr>\n");
			// update the current group.
			strGroup = Tests[intTest].m_szGroup;
		}

		// write the test's name.
		fprintf(pfileWebPage, "<tr><td>%s</td><td>\?\?\?</td></tr>\n", Tests[intTest].m_szName);
	}

	// terminate the last results table and close the webpage.
	fprintf(pfileWebPage, "</table>\n");
	fclose(pfileWebPage);

	// add the run to the VC6TestRuns.htm page. we create a new page with the new entry based on the old page.
	CopyFile("\\\\MSVC\\VCQA\\VC6TestRuns\\VC6TestRuns.htm", "\\\\MSVC\\VCQA\\VC6TestRuns\\VC6TestRuns.old", FALSE);
	FILE *pfileRunsPageOld = fopen("\\\\MSVC\\VCQA\\VC6TestRuns\\VC6TestRuns.old", "r");
	FILE *pfileRunsPageNew = fopen("\\\\MSVC\\VCQA\\VC6TestRuns\\VC6TestRuns.htm", "w");
	// read the first line of the old page to get started.
	CString strLine;
	fgets(strLine.GetBuffer(1024), 1023, pfileRunsPageOld);
	strLine.ReleaseBuffer();

	// copy all the lines from the old page that occur before we need to add the run.
	while(strLine.Find("add run here") == -1)	
	{
		fprintf(pfileRunsPageNew, "%s", strLine);
		fgets(strLine.GetBuffer(1024), 1023, pfileRunsPageOld);
		strLine.ReleaseBuffer();
	}

	// copy the line with the "add run here" comment for next time a run is added.
	fprintf(pfileRunsPageNew, "%s", strLine);
	// insert the new run.
	fprintf(pfileRunsPageNew, "\n");
	fprintf(pfileRunsPageNew, "<tr>\n");

	fprintf(pfileRunsPageNew, "<td><a href=\"%s\\%s.htm\"><font color=red>%s</font></a></td>\n", 
		TestRun.m_strWebPageName, TestRun.m_strWebPageName, TestRun.m_strTitle);

	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strVCRelease);
	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strVCLang);
	fprintf(pfileRunsPageNew, "<td>%s", TestRun.m_strOS);
		
	if(TestRun.m_strOSRelease != "")
		fprintf(pfileRunsPageNew, " %s</td>\n", TestRun.m_strOSRelease);
	else
		fprintf(pfileRunsPageNew, "</td>\n");

	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strOSLang);
	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strCPU);
	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strNumCPU);
	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strSpeed);
	fprintf(pfileRunsPageNew, "<td>%s</td>\n", TestRun.m_strRAM);
	fprintf(pfileRunsPageNew, "</tr>\n");

	// read the next line from the old file to get us started again.
	fgets(strLine.GetBuffer(1024), 1023, pfileRunsPageOld);
	strLine.ReleaseBuffer();

	// copy the rest of the lines from the old page.
	while(!feof(pfileRunsPageOld))	
	{
		fprintf(pfileRunsPageNew, strLine);
		fgets(strLine.GetBuffer(1024), 1023, pfileRunsPageOld);
		strLine.ReleaseBuffer();
	}

	// close both the old and new files.
	fclose(pfileRunsPageOld);
	fclose(pfileRunsPageNew);
}


void DetermineWhichTestsPassed()

{
	// open the webpage for the test run for reading.
	FILE *pfileWebPage = fopen(
		"\\\\MSVC\\VCQA\\VC6TestRuns\\" + TestRun.m_strWebPageName + "\\" + TestRun.m_strWebPageName + ".htm", "r");

	CString strLine;

	// read each line of the web page.
	while(!feof(pfileWebPage))
	
	{
		// read the next line.
		fgets(strLine.GetBuffer(1024), 1023, pfileWebPage);
		strLine.ReleaseBuffer();

		// loop through the Tests array, checking if we read a line that contains a test's pass/fail entry.
		for(int intTest = TestRun.m_intFirstTest; intTest < TestRun.m_intLastTest; intTest++)
		{
			if(strLine.Find(CString("<td>") + Tests[intTest].m_szName) != -1)
			{
				// check if this particular test passed.
				if(strLine.Find("<font color=green>PASS") != -1)
					Tests[intTest].m_boolPassed = TRUE;
			}
		}
	}

	fclose(pfileWebPage);
}


void InstallTests()

{
	// clean-up any pre-existing registry.
	DeleteRegistryKey("CAFE v6");
	// clean-up any pre-existing installation of the tests.
	DeleteTree(TestRun.m_strTestDir);
	// create the directory to hold the  newtest tree.
	CreateDirectory(TestRun.m_strTestDir, NULL);

	// structures used by CreateProcess.
	STARTUPINFO			StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	// not cleaning these structures can lead to problems when calling CreateProcess (0 is a safe default). 
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	memset(&ProcessInfo, 0, sizeof(ProcessInfo));
	// initialize the required STARTUPINFO fields.
	StartupInfo.cb = sizeof(STARTUPINFO);
	
	// create the xcopy command that will copy the test tree.
	CString strCmd = 
		CString("xcopy \\\\VSQAAUTO\\VC6") + szTestType[TestRun.m_TestType] + "\\*.* " + TestRun.m_strTestDir + " /e/s/k";
	
	// allocate a new console to run the command.
	AllocConsole();
	SetConsoleTitle(strCmd);

	// start the xcopy process.
	if(CreateProcess(NULL, strCmd.GetBuffer(0), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		// wait for the process to end.
		WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
		CloseHandle(ProcessInfo.hProcess);
		CloseHandle(ProcessInfo.hThread);
	}
	else
	{
		MessageBox(NULL, "Failed to spawn xcopy to copy the test tree!", "VC6 Test Runner", MB_OK);
	}

	strCmd.ReleaseBuffer();
	FreeConsole();

	// modify the environment to include the vc6 directories that we will pass to cafedrv.exe.
	SetCafeEnvironment();
}


void RunTests()

{
	// holds the cafedrv.exe command.
	CString strCmd;
	// some tests require that extra switches be passed to cafedrv.exe.
	CString strExtraSwitches;
	// reads each line of the log.
	CString strLine;
	// for getting cafedrv.exe's exit code so we know when it finished.
	DWORD dwCafeDrvExitCode;
	// for tracking how long the test has been executing.
	int intSeconds;
	// specifies the log we need to examine. this is results.log unless we have to kill cafe, in which case it is details.txt.
	CString strLogName;
	// structures used by CreateProcess.
	STARTUPINFO			StartupInfo;
	PROCESS_INFORMATION ProcessInfo;
	// initialize the required STARTUPINFO fields.
	StartupInfo.cb = sizeof(STARTUPINFO);

	// loop through all the tests and run them.
	for(int intTest = TestRun.m_intFirstTest; intTest < TestRun.m_intLastTest; intTest++)

	{
		// if this test has already passed in a previous run, don't re-run it.
		if(Tests[intTest].m_boolPassed)
			continue;
		
		// the debugger snaps do different tests for the processor pack depending on which processor they are running on.
		if((CString(Tests[intTest].m_szGroup) == "Debugger Snaps") && (TestRun.m_strVCRelease == "SP4-PP"))
		
		{
			if(TestRun.m_strCPU == "Pentium II")
				strExtraSwitches = "-MMX";
			else if(TestRun.m_strCPU == "Pentium III")
				strExtraSwitches = "-MMX -KNI";
			else if(TestRun.m_strCPU == "WNI")
				strExtraSwitches = "-MMX -WNI";
			else if(TestRun.m_strCPU != "Itanium")
				// K6 or K7.
				strExtraSwitches = "-" + TestRun.m_strCPU;
				
			strExtraSwitches = strExtraSwitches + " -PP";
		}
			
		else
			strExtraSwitches = "";

		// delete any pre-existing results.log and details.txt.
		// we don't want the results of this test to be confused with that of the previous test.
		DeleteFile(TestRun.m_strTestDir + "\\results.log");
		DeleteFile(TestRun.m_strTestDir + "\\details.txt");
		// not cleaning these structures can lead to problems when calling CreateProcess (0 is a safe default). 
		memset(&StartupInfo, 0, sizeof(StartupInfo));
		memset(&ProcessInfo, 0, sizeof(ProcessInfo));

		// create the cafedrv.exe command.
		strCmd.Format("%s\\cafedrv.exe -run -Debug=0 -delay=%s -period=%s -StepSleep=%s -lang=%s -stf=%s\\%s %s",
			TestRun.m_strTestDir, TestRun.m_strDelay, TestRun.m_strPeriod, TestRun.m_strStepSleep, TestRun.m_strVCLang,
			TestRun.m_strTestDir, Tests[intTest].m_szDllPath, strExtraSwitches);

		// start the cafedrv.exe process.
		if(CreateProcess(
			NULL, strCmd.GetBuffer(0), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, 
			NULL, TestRun.m_strTestDir, &StartupInfo, &ProcessInfo))
		{
			strLogNew.Format("Running test %s - %s.", Tests[intTest].m_szGroup, Tests[intTest].m_szName);
			AppendToLogs(strLogNew);

			// wait up to 4 hours for the test to end.
			for(intSeconds = 0; intSeconds < 14400; intSeconds++)
		
			{
				Sleep(1000);
				// get cafedrv.exe's exit code (if it has finished).
				GetExitCodeProcess(ProcessInfo.hProcess, &dwCafeDrvExitCode);
				
				// keep sleeping if cafedrv.exe is still running.
				if(dwCafeDrvExitCode != STILL_ACTIVE)
					break;
			}
		}

		strCmd.ReleaseBuffer();

		// kill the test if it has exceeded the time limit (probably hung).
		if(intSeconds == 14400)
		
		{
			strLogNew.Format("ERROR! Test %s - %s exceeded 4 hour time limit.", 
				Tests[intTest].m_szName, Tests[intTest].m_szGroup);

			AppendToLogs(strLogNew);

			// kill cafedrv.exe (which will also kill the msdev.exe it is debugging).
			TerminateProcess(ProcessInfo.hProcess, 1);
			// if cafedrv.exe doesn't shut down properly, results.log doesn't get create.
			// all the interesting info is in details.txt.
			strLogName = "details.txt";
		}
		else
			strLogName = "results.log";

		// create a test-specific name for the log.
		CString strTestSpecificLogName = CString(Tests[intTest].m_szGroup) + Tests[intTest].m_szName + ".log";
		
		// some of the groups have spaces in them, but we don't want spaces in the name of the log.
		// sometimes spaces in names screws up ie.
		for(int i = 0; i < strTestSpecificLogName.GetLength(); i++)
		{
			if(strTestSpecificLogName[i] == ' ')
				strTestSpecificLogName.Delete(i, 1);
		}
		
		// make a copy of the log file with a test-specific name. overwrite any existing file.
		CopyFile(
			CString(TestRun.m_strTestDir) + "\\" + strLogName, 
			CString(TestRun.m_strTestDir) + "\\" + strTestSpecificLogName, FALSE);
 
		// if results are being published on the web, update the result of the test and copy the log to the webserver.
		if(TestRun.m_boolPublishOnWeb)
		
		{
			// the test hasn't passed until we find "Total Failures:    0" in the log.
			BOOL boolTestFailed = TRUE;
			// open the log file.
			FILE *pfileLog = fopen(TestRun.m_strTestDir + "\\" + strLogName, "r");

			// make sure there was actually a log to open.
			if(pfileLog)
			
			{
				// copy the log to a test-specific name in the webserver directory for this run.
				CopyFile(
					CString(TestRun.m_strTestDir) + "\\" + strLogName,
					CString("\\\\MSVC\\VCQA\\VC6TestRuns\\") + TestRun.m_strWebPageName + "\\" + 
					strTestSpecificLogName, FALSE);

				// scan each line of the log.
				while(!feof(pfileLog))
			
				{
					// read the next line.
					fgets(strLine.GetBuffer(1024), 1023, pfileLog);
					strLine.ReleaseBuffer();

					// check if we found the line saying the test passed.
					if(strLine.Find("Total Failures:    0") != -1)
					{
						boolTestFailed = FALSE;
						break;
					}
				}
			}

			CString strRoughResult;

			// if we were able to get a log, then we can provide a link to it in the result (PASS or FAIL).
			if(pfileLog)
			{
				strRoughResult = "<a href=\"" + strTestSpecificLogName + "\"><b>\?\?\?</b></a>";
				fclose(pfileLog);
			}
			else
				strRoughResult = "<b>\?\?\?</b>";

			// update the test result on the run's webpage. we create a new page based on the old page.
			CString strWebPageNameOld = 
				CString("\\\\MSVC\\VCQA\\VC6TestRuns\\") + TestRun.m_strWebPageName + "\\" + 
				TestRun.m_strWebPageName + ".old";
			CString strWebPageNameNew = 
				CString("\\\\MSVC\\VCQA\\VC6TestRuns\\") + TestRun.m_strWebPageName + "\\" + 
				TestRun.m_strWebPageName + ".htm";

			CopyFile(strWebPageNameNew, strWebPageNameOld, FALSE);
			FILE *pfileWebPageOld = fopen(strWebPageNameOld, "r");
			FILE *pfileWebPageNew = fopen(strWebPageNameNew, "w");
			// read the first line of the old page to get started.
			CString strLine;
			fgets(strLine.GetBuffer(1024), 1023, pfileWebPageOld);
			strLine.ReleaseBuffer();

			// copy all the lines from the old page that occur before the test whose result we need to modify.
			while(strLine.Find(CString("<td>") + Tests[intTest].m_szName) == -1)	
			{
				fprintf(pfileWebPageNew, "%s", strLine);
				fgets(strLine.GetBuffer(1024), 1023, pfileWebPageOld);
				strLine.ReleaseBuffer();
			}

			// initialize the final result with the rough result.
			CString strFinalResult =
				CString("<tr><td>") + Tests[intTest].m_szName + "</td><td>" + strRoughResult + "</td></tr>\n";

			// update the test's result. passes are green, fails are red.
			if(boolTestFailed)
				strFinalResult.Replace("\?\?\?", "<font color=red>FAIL</font>");
			else
				strFinalResult.Replace("\?\?\?", "<font color=green>PASS</font>");

			// write the result.
			fprintf(pfileWebPageNew, "%s", strFinalResult);
			// read the next line from the old file to get us started again.
			fgets(strLine.GetBuffer(1024), 1023, pfileWebPageOld);
			strLine.ReleaseBuffer();

			// copy the rest of the lines from the old page.
			while(!feof(pfileWebPageOld))	
			{
				fprintf(pfileWebPageNew, strLine);
				fgets(strLine.GetBuffer(1024), 1023, pfileWebPageOld);
				strLine.ReleaseBuffer();
			}

			// close both the old and new files.
			fclose(pfileWebPageOld);
			fclose(pfileWebPageNew);
		}
	}
}


void SetCafeEnvironment(void)

{
	HKEY hKey;
	CString strInstallDirsValue;
	DWORD dwValueSize = _MAX_PATH;

	// open the key we need to query.
	RegOpenKeyEx(HKEY_CURRENT_USER, "Software\\Microsoft\\DevStudio\\6.0\\Directories", 0, KEY_READ, &hKey);
	// get the Install Dirs value.
	RegQueryValueEx(hKey, "Install Dirs", 0, NULL, (unsigned char*)strInstallDirsValue.GetBuffer(dwValueSize), &dwValueSize);
	strInstallDirsValue.ReleaseBuffer();
	// close the registry
	RegCloseKey(hKey);

	// we need to search this string, and on some os's it uses mixed case while on others it is all upper-case.
	strInstallDirsValue.MakeUpper();
	// everything is based off of the "MICROSOFT VISUAL STUDIO" directory, which precedes "COMMON" in the string.
	int intCommon = strInstallDirsValue.Find("COMMON");
	CString strVSDir = strInstallDirsValue.Left(intCommon - 1);
	// get the path environment variable value.
	CString strPathValue;
	GetEnvironmentVariable("PATH", strPathValue.GetBuffer(4096), 4095);
	strPathValue.ReleaseBuffer();

	// reset the path environment variable with the new value.
	SetEnvironmentVariable(
		"PATH", 
		strPathValue + ";" + strVSDir + "\\COMMON\\MSDEV98\\BIN;" + 
		strVSDir + "\\COMMON\\TOOLS;" + strVSDir + "\\VC98\\BIN;");

	// set the include and lib environment variables.
	SetEnvironmentVariable(
		"INCLUDE", 
		strVSDir + "\\VC98\\INCLUDE;" + strVSDir + "\\VC98\\MFC\\INCLUDE;" + strVSDir + "\\VC98\\ATL\\INCLUDE;");

	SetEnvironmentVariable("LIB", strVSDir + "\\VC98\\LIB;" + strVSDir + "\\VC98\\MFC\\LIB;");
}


void DeleteRegistryKey(LPCSTR pszKeyName)

{
	const char* const pszBaseKey = "Software\\Microsoft\\";
	HKEY hKey;
	DWORD err;

	// open the key, if it exists.
	if((err = RegOpenKeyEx(HKEY_CURRENT_USER, CString(pszBaseKey) + pszKeyName, 0, KEY_ALL_ACCESS, &hKey)) == ERROR_SUCCESS) 
	
	{
		// delete all the subkeys in this key.
		DeleteRegistrySubKeys(hKey);
		RegCloseKey(hKey);

		// delete the key itself.
		if(RegOpenKeyEx(HKEY_CURRENT_USER, pszBaseKey, 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) 
		
		{
			if(RegDeleteKey(hKey, pszKeyName) != ERROR_SUCCESS) 
			{
				fprintf(pfileDebugLog, "Unable to delete key '%s'.", pszKeyName);
				return;
			}
		
			RegCloseKey(hKey);
		}
	
		else 
		{
			fprintf(pfileDebugLog, "Unable to open key '%s'.\n", pszBaseKey);
			return;
		}
	}
	
	else
	{
		fprintf(pfileDebugLog, "Unable to open key '%s'.", CString(pszBaseKey) + pszKeyName);
		return;
	}
}


void DeleteRegistrySubKeys(HKEY hKey)

{
	TCHAR szSubKey[MAX_PATH + 1];
	CStringArray KeyArray;
	int iSubKey = 0;

	// build an array of subkeys in this key
	while(RegEnumKey(hKey, iSubKey++, szSubKey, MAX_PATH ) == ERROR_SUCCESS)
		KeyArray.Add(szSubKey);

	// iterate through the subkeys in this key
	for(int iCur = 0; iCur < KeyArray.GetSize(); iCur++) 
	
	{
		HKEY hSubKey;

		// open the subkey
		if(RegOpenKeyEx(hKey, KeyArray[iCur], 0, KEY_WRITE | KEY_READ, &hSubKey) == ERROR_SUCCESS)
		{
			// delete all its subkeys
			DeleteRegistrySubKeys(hSubKey);
			RegCloseKey(hSubKey);
		}
		else 
			fprintf(pfileDebugLog, "Unable to open key '%s'.", KeyArray[iCur]);

		// delete the subkey
		if(RegDeleteKey(hKey, KeyArray[iCur]) != ERROR_SUCCESS) 
			fprintf(pfileDebugLog, "Unable to delete key '%s'.", KeyArray[iCur]);
	}
}


void DeleteTree(CString strPath)

{
	WIN32_FIND_DATA Win32FindData;
	CString strFilename;
	CString strFullPath;
	bool boolFolder;

	HANDLE hFile = FindFirstFile(strPath + "\\*.*", &Win32FindData);
	
	if(hFile != INVALID_HANDLE_VALUE)
	
	{
		do
		
		{
			strFilename = Win32FindData.cFileName;
			boolFolder = (Win32FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY;
			strFullPath = strPath + "\\" + strFilename;
			
			if(boolFolder && (strFilename != ".") && (strFilename != ".."))
				DeleteTree(strFullPath);

			else if(!boolFolder)			
			
			{
				if(Win32FindData.dwFileAttributes & FILE_ATTRIBUTE_READONLY)
					SetFileAttributes(strFullPath, FILE_ATTRIBUTE_NORMAL);

				DeleteFile(strFullPath);
			}

		} while (FindNextFile(hFile, &Win32FindData));

		FindClose(hFile);
	}

	RemoveDirectory(strPath);
}


void AppendToLogs(CString strLogNew)
{
	strLog = strLog + strLogNew + "\r\n";
	SetWindowText(MainControls[EDIT_LOG].m_hWnd, strLog);
	fprintf(pfileDebugLog, "%s\n", strLogNew);
	fflush(pfileDebugLog);
}
