//scriptr.h : Tree structure for the testcase structure.

#include "stdafx.h"

#ifndef _SCRIPT_TREE_
#define _SCRIPT_TREE_

#define HIDDEN 1
#define NOTHIDDEN 2
#define ALL -999
      
extern CBitmap bmapUnCheck, bmapCheck, bmapNomoreItems, bmapMoreItems,	bmapNone, bmapWarn ;								 

class CVariable: public CObject 
{
	public:
	CString ClsName ;
	CString VarName ;
} ;

class CScriptTree: public CObject
{
	public:
		CScriptTree(){m_Show = TRUE ; m_Hidable = FALSE ; m_TestCase = FALSE ; m_apiIndex = -1; }

		static CScriptTree * ScriptHead ;
		CListBox * m_Listbx ;
		int m_Hidable ;
		int m_level ;
		int m_index ;
		int m_parent ;
		int m_apiIndex;
		CBitmap *m_bmap ;
		CString m_Text ;
		CObArray m_Children ;
		CObArray m_VarInScope ;
		BOOL m_Show ;
		BOOL m_TestCase ;

		BOOL IsFocusOnListBox() ;
		void SetFocusOnListBox() ;
		BOOL IsTestCase() {return m_TestCase; } ;
		void HideVars_Blocks(int index = 0 ) ; 
		void AddToVarsInScope(CVariable * Var) {m_VarInScope.Add(Var);} ;
		void SetListBox(CWnd *plb) {m_Listbx = (CListBox *) plb ;} ;
		void SetBitmap(HBITMAP hbmap) {m_bmap =  CBitmap::FromHandle(hbmap); } ;
		void SetText(CString text) {m_Text = text ;} ;
		void SetIndex(int Index) {m_index = Index ;} ;
		void NotifyHeadofAddition(CScriptTree * head, int index) ;
		void NotifyHeadofDelete(CScriptTree * head, int index );//collapse = false, delete = true
		void CollapseTree(BOOL KillSelf = FALSE);
		void AddNode(CScriptTree * NewNode,int at = -999) ; //Update a node spacified by the index i.
		void ExpandTree(int index);
		void DeleteChild(int childIndex) ;
		void DumpIntoString(CString * buffer) ;
		void DrawScript(int index = 0)  ;
		void AddChild(CScriptTree * Child) ;
		void MakeUniqueEntries(CString * buffer, CString FuncName ) ;
		void MoveNode(int UpOrDown);
		void MoveUp(){MoveNode(-1) ;} ;
		void MoveDown(){MoveNode(1);} ;
		void DissownChild(int childIndex ); 


		CString GetObjectName(CString Class) ;
		CScriptTree * FindNode(int index) ;
		CScriptTree * FindParent(int index,CScriptTree * parent = ScriptHead) ;
		void ClearListbox() ;
		BOOL IsIndexInTree() ;
		BOOL IsTree() {return !((HBITMAP)bmapNone.GetSafeHandle() == (HBITMAP) m_bmap->GetSafeHandle());} ;
		BOOL IsExpanded() {return ((HBITMAP)bmapNomoreItems.GetSafeHandle()== (HBITMAP) m_bmap->GetSafeHandle());} ;
		BOOL IsCollapsed() {return ((HBITMAP)bmapMoreItems.GetSafeHandle()== (HBITMAP) m_bmap->GetSafeHandle());} ;
		BOOL IsWarningOFF() {return ((HBITMAP)bmapNone.GetSafeHandle()== (HBITMAP) m_bmap->GetSafeHandle());} ;
		BOOL IsWarningON() {return ((HBITMAP)bmapWarn.GetSafeHandle()== (HBITMAP) m_bmap->GetSafeHandle());} ;
		void WarningOFF() {SetBitmap((HBITMAP)bmapNone.GetSafeHandle());} ;
		void WarningON() {SetBitmap((HBITMAP)bmapWarn.GetSafeHandle());} ;


} ;

#endif // _SCRIPT_TREE_
