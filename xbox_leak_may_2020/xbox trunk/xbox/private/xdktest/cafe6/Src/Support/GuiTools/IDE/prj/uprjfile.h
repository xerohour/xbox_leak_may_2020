///////////////////////////////////////////////////////////////////////////////
//  UPRJFILE.H
//
//  Created by :            Date :
//      MichMa              12/01/93
//
//  Description :
//      Declaration of the UIProjectFiles class
//

#ifndef __UIPRJFILE_H__
#define __UIPRJFILE_H__

#include "..\..\udialog.h"

#include "prjxprt.h"

#ifndef __UIDIALOG_H__
   #error include 'udialog.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIQuickWatch class

#define UIPF_TITLE       		"Project Files"
#define UIPF_BTN_ADD     		"&Add"
#define UIPF_BTN_ADD_ALL 		"Add A&ll"
#define UIPF_EB_FILENAME 		"File &Name:"

#define UIPF_CB_FILES_OF_TYPE	"List Files of &Type:"
#define UIPF_FOT_SOURCE			"@1"
#define UIPF_FOT_HEADER			"@2"
#define UIPF_FOT_RES_SCRIPT		"@3"
#define UIPF_FOT_DEF_FILE		"@4"
#define UIPF_FOT_ODL_FILE		"@5"
#define UIPF_FOT_LIB			"@6"
#define UIPF_FOT_OBJ_FILE		"@7"
#define UIPF_FOT_ALL			"@8"

// BEGIN_CLASS_HELP
// ClassName: UIProjectFiles
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIProjectFiles : public UIDialog

   {
   UIWND_COPY_CTOR(UIProjectFiles, UIDialog);

   // Utilities

   public:
      CString ExpectedTitle(void) const {return UIPF_TITLE;}
      BOOL AddAllFiles(LPCSTR szType = UIPF_FOT_ALL, LPCSTR szProjFilesPath = NULL);
	  BOOL AddFile(LPCSTR szFileName);
	  virtual HWND Close(void);
   };

#endif //__UIPRJFILE_H__
