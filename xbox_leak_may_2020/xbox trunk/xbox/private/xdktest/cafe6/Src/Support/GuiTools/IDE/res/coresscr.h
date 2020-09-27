///////////////////////////////////////////////////////////////////////////////
//  CORESSCR.H
//
//  Created by :            Date :
//      EnriqueP                 1/7/94
//
//  Description :
//      Declaration of the COResScript class
//

#ifndef __CORESSCR_H__
#define __CORESSCR_H__

#include "uresedit.h"
#include "..\eds\cofile.h"
#include "..\wrk\uiwrkspc.h"

#include "export.h"

#define ERROR_ERROR -1

///////////////////////////////////////////////////////////////////////////////
//  COResScript class

// BEGIN_CLASS_HELP
// ClassName: COResScript
// BaseClass: COFile
// Category: Editors
// END_CLASS_HELP
class RES_CLASS COResScript : public COFile 
{

public:
	COResScript();
// Attributes

// Data
private:
    
// Generic Utilities
public:
    

//File I/O
public:
	int Create( LPCSTR szSaveAs = NULL );			 	// RC files
	int Open(LPCSTR szFileName, LPCSTR szChDir = NULL );
//	int Close(void);
//	int Save(UINT nParam);
//	int SaveAs(LPCSTR szName, UINT nFileType);

// Resource management									
public:
	int CreateResource( UINT idResType );	     	// Used for predefined resource types	
	int CreateResource( LPCSTR szResName );			// Used for custom resource types	   		
	int CreateResource(UINT idResType, UIResEditor& resEditor);
	int OpenResource(UINT idResType, LPCSTR szResName, UIResEditor& resEditor);
	int AddMenu(LPCSTR szMenuName, LPCSTR szMenuItem, UIResEditor& resEditor);
	int ChangeDialogCaption( LPCSTR szDialogCaption, UIResEditor& resEditor);
	int ChangeDialogID( LPCSTR szNewDialogID, LPCSTR szOldDialogID);
/*	int OpenResource(UINT nResId, RC_TYPE rcType);
	int DeleteResource(UINT nResId, RC_TYPE rcType);
	int CopyResource(UINT nResId, RC_TYPE rcType);
	int CutResource(UINT nResId, RC_TYPE rcType);
	int PasteResource(UINT nResId, RC_TYPE rcType);
	int ExportResource(UINT nResId, RC_TYPE rcType, LPCSTR szName, UINT nFileType);	   */

	
//  To be implemented later

//  CStrint MoveResource(UINT nResId, RC_TYPE rcType, UINT nRCid2);
//	int ImportResource(LPCSTR szName);
//	void CloseAllRes(void);
	
//	LPCSTR GetResProp(UINT nPropId, UINT nRCid);
//	int SetResProp(UINT nPropId, LPCSTR szProp);

};

///////////////////////////////////////////////////////////////////////////////
// COResScript #defines



#endif //__CORESSCR_H__
