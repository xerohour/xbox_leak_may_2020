///////////////////////////////////////////////////////////////////////////////
//  UIWKSPC.H
//
//  Created by :            Date :
//      JimGries            4/25/95
//
//  Description :
//      Declaration for the dockable Workspace window.
//

#ifndef __UIWRKSPC_H__
#define __UIWRKSPC_H__

#include "..\shl\udockwnd.h"
#include "..\sym\dockids.h"
#include "..\src\cosource.h"

#include "export.h"

#ifndef __UIWRKSPC_H__
    #error include 'udockwnd.h' before including this file
#endif

// The following are error codes returned by UIWorkspaceWindow member functions.
#define SUCCESS					 0
#define ERROR_ERROR				-1
#define ERROR_NO_PROJECT		-2
#define ERROR_NO_RESFILE		-3
#define ERROR_NO_CLASS			-4
#define	ERROR_NO_MEMBER			-5
#define ERROR_NO_RESOURCE		-6
#define ERROR_NO_FILE			-7
#define ERROR_NO_GLOBAL			-8
#define ERROR_NO_DECLARATION	-9
#define ERROR_NO_DEFINITION		-10
#define ERROR_GOTO			    -11

// The following defines are control IDs that are used to acces information on
// property dialogs.  See IDE source Clsview/resource.h
#define IDC_CLASSNAME 1012
#define IDC_FUNCTNAME 1009
#define IDC_DATANAME  1010
#define IDC_TARGNAME  1017

///////////////////////////////////////////////////////////////////////////////
//  UIWorkspaceWindow

// BEGIN_CLASS_HELP
// ClassName: UIWorkspaceWindow
// BaseClass: UIDockWindow
// Category: Workspace
// END_CLASS_HELP
class WRK_CLASS UIWorkspaceWindow : public UIDockWindow
{
public:
	void RemoveCharFromString(CString *, char);

	enum MEMBER_TYPE { Function, Data };
	typedef MEMBER_TYPE GLOBAL_TYPE;

    UIWorkspaceWindow();
	operator HWND ( ) {return HWnd();} ;
	operator BOOL() {return  HWnd() == 0;} ;

    virtual BOOL Activate(void);
    UINT GetID(void) const;

	BOOL ActivateBuildPane();
	BOOL ActivateDataPane();
	BOOL ActivateHelpPane();
	BOOL ActivateClassPane();
	BOOL ActivateResourcePane();

	int SelectProject(LPCSTR);
	int SelectClass(LPCSTR, LPCSTR = NULL);
	int SelectGlobal(LPCSTR, GLOBAL_TYPE, LPCSTR = NULL);
	int SelectClassMember(LPCSTR, MEMBER_TYPE, LPCSTR = NULL, LPCSTR = NULL);
	int SelectResource(LPCSTR, LPCSTR, LPCSTR = NULL);
	int SelectResource2(LPCSTR, LPCSTR, LPCSTR = NULL);
	int SelectFile(LPCSTR, LPCSTR = NULL);


	int GotoClassMemberDeclaration(LPCSTR, MEMBER_TYPE, COSource*, LPCSTR = NULL, LPCSTR = NULL);
	int GotoClassMemberDefinition(LPCSTR, MEMBER_TYPE, COSource*, LPCSTR = NULL, LPCSTR = NULL);
	int GotoGlobalDefinition(LPCSTR, GLOBAL_TYPE, COSource*, LPCSTR = NULL);
	int GotoClassDefinition(LPCSTR, COSource*, LPCSTR = NULL);
	int GotoClassDeclaration(LPCSTR, COSource*, LPCSTR = NULL);
	int GotoDefinitionFromHere(LPCSTR);
	int GotoDeclarationFromHere(LPCSTR);

	BOOL SetActiveProject(LPCSTR projName);

};

#endif //__UIWRKSPC_H__
