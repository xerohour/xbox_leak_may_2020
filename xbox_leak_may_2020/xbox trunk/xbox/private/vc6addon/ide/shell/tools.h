/////////////////////////////
// 
// tools.h
// 
// class definition for CTools which maintain option.tool data structure
//
//	CTool	 : option.tool data structure
//	CToolList: tool manager, manages a list of Tools
//
//////////////////////////////
#ifndef __TOOLS_H__

#define __TOOLS_H__

////////////////////////////
// include files
////////////////////////////
#ifndef __PATH_H_
#include "path.h"
#endif

#ifndef __IDS_H_
#include "ids.h"
#endif

class CConsoleSpawner;

// FUTURE: Should MAXTOOLMENU be smaller, say 16, so as to assure small menu?
#define MAXTOOLMENU	30
#define MAXPATHNAME 	_MAX_PATH
#define MAXINITDIR	_MAX_PATH
#define MAXARGUMENT 	_MAX_PATH
#define INVALID_OUTPUT_WINDOW	-1
////////////////////////////
// CTool : Class definition
////////////////////////////

class CTool: public CObject
{
// Construction
public:
	CTool() { m_pSpawner = NULL; }
	~CTool() { }

// Implementation
	BOOL CanSpawn() { return(m_pSpawner == NULL); }
	UINT Spawn();
	void CancelSpawn();

// define operator
	const CTool &operator = (const CTool &toolSrc);

public:
	CString		m_strMenuName;		// Menu name in Tool menu
	CString		m_strPathName;		// Full path name of file
	CString		m_strDefaultArgs;	// Default command line args
	CString		m_strCurrentArgs;	// Current command line args
	CString		m_strInitialDir;	// Default directory
	BOOL 		m_fAskArguments;	// Ask for arguments
	BOOL 		m_fGUITool;			// Is this tool a GUI app?
	BOOL 		m_fOutputRedirectDefault;	// Default Redirect to output window
	BOOL 		m_fOutputRedirect;	// Redirect to output window
	INT  		m_nOutputWindowID;	// Output window ID
	BOOL 		m_fCloseWindow;		// Close window after task complete
	BOOL		m_fUseSearchPath;	// FALSE if path to tool is hard-coded; TRUE if we should use the PATH
	BOOL		m_fVisibleOnMenu;	// TRUE if the user has checked the box in Tools.Customize.Tools
	BOOL		m_fReloadNoPrompt;	// TRUE if the tool will modify a single file and we want the IDE
									// to reload the file without prompting

	CConsoleSpawner *m_pSpawner;
};

///////////////////////
// class CToolList
///////////////////////
#define MAXTOOLIST	(IDM_USERTOOLS_LAST - IDM_USERTOOLS_BASE + 1)

class CToolList: public CObject
{
protected:
	int		m_nTools;
	int		m_nCurTool;
public:
	CTool  *m_toolArray[MAXTOOLIST];

	CToolList() { m_nTools = 0; m_nCurTool = -1; }
	~CToolList();
	int		AddTool(CTool *newTool);
	void 	DeleteTool(int nIndex);
	void	SwapTool(int nIndex1, int nIndex2 );
	void	DestroyContent();
	CTool  *GetTool(int nIndex);
	CTool  *GetCurToolObject();
	int		GetCurTool() { return m_nCurTool; };
	void	SetCurTool(int nIndex) { m_nCurTool = nIndex; }
	CToolList&  operator = (CToolList& toolListSrc);
	int		NumTools() { return m_nTools; }
} ;

extern CToolList toolList;
extern CToolList toolListBackup;

extern void SaveToolListSetting();
extern void LoadToolListSetting();

#endif	// __TOOLS_H__
