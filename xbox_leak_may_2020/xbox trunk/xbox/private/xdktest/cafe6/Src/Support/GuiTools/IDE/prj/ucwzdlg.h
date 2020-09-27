///////////////////////////////////////////////////////////////////////////////
//  UITABDLG.H
//
//  Created by :            Date :
//      Ivanl             1/27/94
//
//  Description :
//      Declaration of the UIClassWizard class
//

#ifndef __UCWZDLG_H__
#define __UCWZDLG_H__
#include "odbcdlgs.h"
#include "..\sym\clswiz.h"
#include "..\sym\vcpp32.h"
#include "..\shl\uitabdlg.h"

#include "prjxprt.h"

#ifndef __UITABDLG_H__
	#error include 'uitabdlg.h' before including this file
#endif

///////////////////////////////////////////////////////////////////////////////
//  UIClassWizard class

//	Tools Options tabs	 (These are acually unique control id's found in each page) REVIEW: Use True tab id's
typedef enum{ TAB_CLASSINFO= CLSWIZ_IDC_RESOURCENAME, 
			  TAB_MESSAGEMAP= CLSWIZ_IDC_MESSAGELIST, 
			  TAB_OLE_AUTOMATION=CLSWIZ_IDC_DATA_BINDING, 
			  TAB_OLE_EVENTS= CLSWIZ_ID_ADD_CUSTOM, 
			  TAB_MEMBER_VARIABLES = CLSWIZ_ID_BIND_ALL } ;
typedef enum{ NO_OLE = CLSWIZ_IDC_NO_OLE, AUTOMATION = CLSWIZ_IDC_OLE_AUTO, ID_OLE = CLSWIZ_IDC_OLE_CREATE } OLE_TYPE ;
// The implementation types for OLE Events, Methods, and properties.
typedef enum  {STOCK,CUSTOM,MEMBER_VAR,GET_SET_MEHOD} IMPLEMENTATION ;
#define ID_NODELETE	05 
#define IDSS_CW_TITLE "MFC ClassWizard"   // Move this to .RC

// Control ID's for Directories Page  ( original id's defined in vproj.h)
///////////////////////////////////////////////////////////////////////////////
 
class PRJ_CLASS UIAutomationDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIAutomationDlg, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return (""); }		// Options
		         
			virtual int SetExtName(int TypeIndex) ;
			virtual int SetExtName(CString Name) ; 
			virtual int SetImplementation(IMPLEMENTATION STOCK) ;     
			virtual int SetIntName(CString Name) {return NULL;} ;             
			virtual int SetReturnType(CString Type) {return NULL;} ;      
			virtual CString SetReturnType(int Index) ;   
			virtual int AddParameter(int Type, CString Name = "NULL") ;
			virtual int ParamTypeCount() ;
			virtual int GetRetTypeCount() ;
			virtual int GetStockCount() ;
			virtual int Create() ;             
 } ; 


class PRJ_CLASS UIMemVarDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIMemVarDlg, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return (""); }		// Options
		         
			int CategoryCount() ;
			int SetVarName(CString Name) ; 
			int SetCategory(int index) ;     
			int SetType(int index) ; 
  			int SetCategory(CString Strcat) {return ERROR_SUCCESS;};     
			int SetType(CString Type){return ERROR_SUCCESS;} ; 
			CString GetCategory(int index) ;
    			
 } ; 

class PRJ_CLASS UIAddMethod : public UIAutomationDlg 
{
    UIWND_COPY_CTOR (UIAddMethod, UIAutomationDlg) ; 
} ;

class PRJ_CLASS UIAddProperty : public UIAutomationDlg 
{
    UIWND_COPY_CTOR (UIAddProperty, UIAutomationDlg) ; 

} ; 

class PRJ_CLASS UIAddEvent : public UIAutomationDlg 
{
    UIWND_COPY_CTOR (UIAddEvent, UIAutomationDlg) ; 
 } ; 


class PRJ_CLASS UIAddClassDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIAddClassDlg, UIDialog) ; 

	UIDataBaseDlgs m_DBdlgs ;
// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return (IDSS_CW_TITLE); }		// Options

			BOOL Show() ;               
			int SetType(int TypeIndex) ;
			int SetName(CString Name) ;        
			int SetHeader(CString DotH) ;      
			int SetCPP(CString DotCPP ) ;
			int CheckOle(OLE_TYPE OleType= ID_OLE);
			int CheckOleCreateable(CString ExtName) ;
			int CheckAddToGallery(BOOL bCheck = TRUE);
			int AttachDlgID(int index = 1) ;
			int AttachDlgID(CString Name) ;
			int Create() ; 
			void SetDataSourceName(CString Name) ;

          
 } ; 
// BEGIN_CLASS_HELP
// ClassName: UIClassWizard
// BaseClass: UITabbedDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIClassWizard : public UITabbedDialog 
	{
    UIWND_COPY_CTOR (UIClassWizard, UITabbedDialog) ; 

// Private data
    private:
			int m_ActiveClass ;
			UIAutomationDlg * m_AutoDlg ;
			UIAddClassDlg m_AddClassDlg ;
			UIMemVarDlg * m_AddMemDlg ;
// General Utilities
	public:
			
			inline CString ExpectedTitle(void) const
			{ return (IDSS_CW_TITLE); }		// Options
			
			HWND Display(void) ;
			HWND Close() ;
			virtual void OnUpdate(void);

// Directories 
	public:
			int AddFunction(int ClassIndex, int FuncIndex, LPCSTR szObject);
			int AddFunction(int ClassIndex, int FuncIndex, int ObjectIndex);
			int AddFunction2(int ClassIndex, int FuncIndex, int ObjectIndex);
            int AddClass(int TypeIndex ,CString Name = "", CString DotH = "" , CString DotCPP = "");
            int AddClass2(CString BaseClassName ,CString Name = "", CString DotH = "" , CString DotCPP = "");
			CString GetActiveClass() ;
			int GetClassCount() ;
			int EditCode() ;
			CString GetClassFile() ;
			int DeleteFunction(int ClassIndex, int FuncIndex, int ObjectIndex ) ;
			int ImportClass(CString Name,  CString DotH, CString DotCPP) ;
			UIAddMethod * AddOleMethod() ;
			UIAddProperty *AddOleProperty() ;
			UIAddEvent * AddOleEvent() ;
			UIMemVarDlg * AddMemberVar(int index) ;
			UIMemVarDlg * AddMemberVar(CString CtrId) ;
			int GetControlCount() ;
			void SetDataSourceName(CString Name) ;
   } ;

// BEGIN_CLASS_HELP
// ClassName: UIAddClassDlg
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP


// BEGIN_CLASS_HELP
// ClassName: UIImportOrCreateDlg
// BaseClass: UIDialog
// Category: Project
// END_CLASS_HELP
class PRJ_CLASS UIImportOrCreateDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIImportOrCreateDlg, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return ("Adding a Class"); }// TODO Use String constant not yet available
			
			int Show() ;               
			void AddNew() ;
			void AddImport() ;        
			HWND Cancel();
 } ; 

/* class PRJ_CLASS UIAddMethodDlg : public UIDialog 
	{
    UIWND_COPY_CTOR (UIAddMethodDlg, UIDialog) ; 

// General Utilities
	public:
			inline CString ExpectedTitle(void) const
			{ return (IDSS_CW_TITLE); }		// Options

			int Show() ;               
			int SetType(int TypeIndex) ;
			int SetName(CString Name) ;        
			int SetHeader(CString DotH) ;      
			int SetCPP(CString DotCPP ) ;
			int CheckOle();
			int CheckOleCreateable(CString ExtName) ;
			int AttachDlgID(int index = 1) ;
			int AttachDlgID(CString Name) ;
			int Create() ;             
 } ;  */

#endif //__UCWZDLG_H__          
