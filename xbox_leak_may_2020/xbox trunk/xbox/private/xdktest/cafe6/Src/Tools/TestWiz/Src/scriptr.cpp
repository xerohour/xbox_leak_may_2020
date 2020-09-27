#include "stdafx.h"
#include "scriptr.h"

extern CBitmap  bmapTestCase;								 
//Implementation for the Script listbox tree .
CScriptTree *  CScriptTree::ScriptHead = NULL ; //Initialize head.

CString CScriptTree::GetObjectName(CString ClassName)
{	
	 CScriptTree * tmp ;
	int varCount = m_VarInScope.GetSize() ;
	for(int i = 0 ; i < varCount ; i++)
	{
		CVariable * var = (CVariable *) m_VarInScope.GetAt(i) ;
		if(var->ClsName == ClassName)
		{
			return var->VarName ;
		}
	}
	if (this == (CScriptTree::ScriptHead)) //We've come to the root of the tree in this scope
		return "NULL" ;
	
		tmp = CScriptTree::ScriptHead->FindParent(m_parent) ;
		CString result ;
		if ((result = tmp->GetObjectName(ClassName)) != "NULL")
	   	{
	   		return result ;
		}
	
		return "NULL" ;
}


CScriptTree * CScriptTree::FindParent(int index, CScriptTree * parent /*  = ScriptHead */)
{
	// The index passed in the index of the child. Use FindNode if you need to find a node before 
	// getting the parent index. 

	int children = m_Children.GetSize() ;

	if(m_index == index)
		return parent ;
	// Find a node with the given index
   	for(int i = 0 ; i < children; i++)
	{
		CScriptTree *Node ;
		Node = (CScriptTree *)(m_Children.GetAt(i)) ; 	
	   	if (Node = Node->FindParent(index, this)) 
			return Node ; 
	}

	return NULL ;
}

void CScriptTree::NotifyHeadofAddition(CScriptTree * head, int index) 
{
	int children = head->m_Children.GetSize() ;
  	// Find all nodes which are bellow the added one and update their location
  	for(int i = 0 ; i < children; i++)
	 {
		CScriptTree *Node ;
		Node = (CScriptTree *)(head->m_Children.GetAt(i)) ;		
	   	if(Node->m_index >= index ) // If the added node is above this in the listbox
		{	
			Node->m_index++ ;
			if(Node->m_parent >= index )  //Update the parent too if the new item is above.
				Node->m_parent++ ;
		}

		Node->NotifyHeadofAddition(Node,index);
	 }
}

void CScriptTree::NotifyHeadofDelete(CScriptTree * head, int index) 
{
	int children = head->m_Children.GetSize() ;
  	// Find all nodes which are bellow the deleted one and update their location
   	for(int i = 0 ; i < children; i++)
	 {
		CScriptTree *Node ;
		Node = (CScriptTree *)(head->m_Children.GetAt(i)) ;		
	   	if(Node->m_index > index ) // If the deleted node was above the this in the listbox
		{	
			Node->m_index-- ;
			if(Node->m_parent > index )  //Update the parent too if the new item is above.
				Node->m_parent-- ;

		}

		Node->NotifyHeadofDelete(Node,index);
	 }
}

CScriptTree * CScriptTree::FindNode(int index)
{
	int children = m_Children.GetSize() ;

	if(m_index == index)
		return this ;
	// Find a node with the given index
   	for(int i = 0 ; i < children; i++)
	{
		CScriptTree *Node ;
		Node = (CScriptTree *)(m_Children.GetAt(i)) ;		
	   	if (Node = Node->FindNode(index)) 
			return Node ;
	}

	return NULL ;
}

void CScriptTree::CollapseTree(BOOL KillSelf /* = FALSE*/)
{	
	int index = m_Listbx->GetCurSel() ;
	
	if(KillSelf) //We don't delete an item that the user clicked on.
	{
		index++ ; 
		m_Show = FALSE ;
		//m_Listbx->DeleteString(index+1) ;	
	}
		// If its just a leaf, ignore it.
	if(IsTree())
	{	// Only care about expanded trees, else just delete them.
	    if(IsExpanded())
		{	
		  	SetBitmap((HBITMAP)bmapMoreItems.GetSafeHandle());
			// How many children does it have?
			int children = m_Children.GetSize() ;
			CScriptTree *Node ;
			//If an item is a tree object collapse it, else delete the item
			for(int i = 0 ; i < children; i++)
			{
				Node = (CScriptTree *)(m_Children.GetAt(i)) ;		
				Node->CollapseTree(TRUE);
			}
		}
	   //Don't delete the item we are trying to collapse
		if(KillSelf)
		{
		//	m_Listbx->DeleteString(index) ;	
			m_Show = FALSE ;
		//	NotifyHeadofDelete(CScriptTree::ScriptHead,index); //Notify head 
		}
		else
		{ 
		//	m_Listbx->SetCurSel(index+1);
		//	SetBitmap((HBITMAP)bmapMoreItems.GetSafeHandle()) ;
		}
	}
	//Don't delete the item we are trying to collapse
	if(KillSelf)
	{
	//	m_Listbx->DeleteString(index) ;
		m_Show = FALSE ;	
	//	NotifyHeadofDelete(CScriptTree::ScriptHead,index); //Notify head 
	}


}

void CScriptTree::AddNode(CScriptTree * NewNode,int at /* = -999 */)
{
	int children = m_Children.GetSize() ;
	int index = m_index ;
	// Insert Node text 
	if ( -999 == at)
	{
		if(((NewNode->m_Text).SpanExcluding("{}")).IsEmpty()) // Don't indent braces from the parent.
			NewNode->m_level = m_level  ;
		else
			NewNode->m_level = m_level +1  ;	

		NewNode->m_parent = m_index ;
 		NewNode->SetIndex(m_index+children +1) ;  // Offset all the children from the parent index
		NotifyHeadofAddition(CScriptTree::ScriptHead,NewNode->m_index /*m_index+children +1*/) ;
   		m_Children.Add(NewNode) ;
  	}
	else
	{
		//Find a slot for the new node
		int i ;
		for(i = 0; i < children; i++)
		{
			CScriptTree *Node ;
			Node = (CScriptTree *)(m_Children.GetAt(i)) ;
			
			if (Node->m_index >= at) //WAS == not >= but changed for moving stuff.
				break ;		
		}
		NewNode->m_level = m_level + 1 ;
		NewNode->m_parent = m_index ;
		NewNode->SetIndex(at) ;
		NotifyHeadofAddition(CScriptTree::ScriptHead,at) ;
		m_Children.InsertAt(i ,NewNode) ;
	}
}


void CScriptTree::ExpandTree(int index ) //index = Current selection.
{
//	static int index = m_Listbx->GetCurSel() ;
 	int children = m_Children.GetSize() ;

	index++ ; // Expand the items under the parent!
	// Change the bitmap.
	if(IsCollapsed())
	{
		SetBitmap((HBITMAP)bmapNomoreItems.GetSafeHandle());
	}
	//Expand all the children of the clicked item.
	for(int i = 0 ; i < children; i++)
	{
		CScriptTree * Node ;
		Node = (CScriptTree *) m_Children.GetAt(i) ;
		Node->m_Show = TRUE ;
		Node->ExpandTree(index);
	}


}

void CScriptTree::DumpIntoString(CString * buffer)
{
	// Dumps the script text into the CString object pointer provided.
	//Start entering text from the 2nd level.
	if(m_level)
	{ 	//Insert a newline 
		*buffer = *buffer + "\n" ;
			for(int tabs = 1; tabs < m_level; tabs++)
			{
				*buffer = *buffer + "\t" ;
			}

		//If this is a TestCase item, prepend the className
		if(IsTestCase())
		{
			MakeUniqueEntries(buffer,m_Text) ;
		}
	/*	if((m_Text.SpanExcluding("{}").IsEmpty())||IsTestCase() || m_Text == " ")
		{
		  	*buffer = *buffer + m_Text ;  //No ";" on braces.
		}
		else */
	  	*buffer = *buffer + m_Text ; 
	}
	int children =m_Children.GetSize() ;
	for(int i = 0 ; i < children; i++)
	{
		CScriptTree *Node ;
		Node = (CScriptTree *)(m_Children.GetAt(i)) ;
	  	Node->DumpIntoString(buffer) ; 
	}
}

void CScriptTree::MakeUniqueEntries(CString * buffer, CString FuncName )
{
   	//Get the Testcase classname.
	static int iCases = 1 ;	 //used in IMPLEMENT_GET_NUM_TESTS macro in cases.cpp
	CString TestClass, numCases ;
	awx.m_Dictionary.Lookup("TestClass",TestClass) ;
	*buffer = *buffer +"void "+TestClass + "::" ; 

	// Make an entry into the Run function.
	CString TestFuncs ;
	awx.m_Dictionary.Lookup("TestFunctions",TestFuncs) ;
	TestFuncs = TestFuncs+"\n"+FuncName +";" ;
	awx.m_Dictionary.SetAt("TestFunctions",TestFuncs);

	//Make an entry in the header file and for the confirmation dialog.
	CString FuncDecs ; //declarations	  
	CString confDecs ;

	awx.m_Dictionary.Lookup("ConfDeclarations",confDecs) ;
	confDecs = confDecs+"\n"+"\t"+FuncName ;
	awx.m_Dictionary.SetAt("ConfDeclarations", confDecs) ;

	awx.m_Dictionary.Lookup("FuncDeclarations",FuncDecs) ;
	FuncDecs = FuncDecs+"\n"+"\t"+"void " + FuncName +";" ;
	awx.m_Dictionary.SetAt("FuncDeclarations",FuncDecs) ;

	numCases.Format("%d",iCases);
	awx.m_Dictionary.SetAt("NumCases",numCases) ;
	iCases++ ;

}


void CScriptTree::DrawScript(int index /* = 0 */) 
{
	if (m_Show &&(m_Hidable != HIDDEN)) 
	{ 
	   if(!index)
			ClearListbox() ;
	  	int count = m_Listbx->AddString(m_Text) ; //addstring GetCount
		// Attach Node bitmap 
		m_Listbx->SetItemData(count,(DWORD)this/*(DWORD)m_bmap->GetSafeHandle()*/) ;
  		index++ ;
		int children =m_Children.GetSize() ;
		for(int i = 0 ; i < children; i++)
		{
			CScriptTree *Node ;
			Node = (CScriptTree *)(m_Children.GetAt(i)) ;
	   		Node->DrawScript(index) ; 
		}
	}
}

// Tuggles the showing of Variables and block makers in the testscript.
void CScriptTree::HideVars_Blocks(int iHide/* = 0 */) 
{
	if(m_Hidable)
	{	
		if(!iHide)
			m_Hidable = NOTHIDDEN ;
		else
			m_Hidable = HIDDEN ;
	}
   	int children =m_Children.GetSize() ;
	for(int i = 0 ; i < children; i++)
	{
		CScriptTree *Node ;
		Node = (CScriptTree *)(m_Children.GetAt(i)) ;
   		Node->HideVars_Blocks(iHide) ; 
	}
}

void CScriptTree::ClearListbox() 
{			   
	m_Listbx->ResetContent() ;
}

void CScriptTree::MoveNode(int UpOrDown)  //ALL deletes all children
{
	int children ;
	CScriptTree * Child ; 

	m_index = m_index + UpOrDown ; 
	//Find the node to delete
	children = m_Children.GetSize();
	for(int i = 0 ; i < children; i++)
	{
		Child  = (CScriptTree *)m_Children.GetAt(i) ;
	    Child->MoveNode(UpOrDown) ;
	}
	return ; 
}

void CScriptTree::DissownChild(int childIndex ) 
{
	int children ;
	CScriptTree * Target ; 
	children = m_Children.GetSize();
	for(int i = 0 ; i < children; i++)
	{
		Target  = (CScriptTree *)m_Children.GetAt(i) ;
		if (childIndex == Target->m_index)
		{
			m_Children.RemoveAt(i) ;
			break ;
		}
	}
	
}

void CScriptTree::SetFocusOnListBox() 
{
	m_Listbx->SetFocus() ;	
}

BOOL CScriptTree::IsFocusOnListBox()
{
	 HWND focus = GetFocus() ;
	 return (focus == m_Listbx->m_hWnd) ;
} 


void CScriptTree::DeleteChild(int childIndex /* = ALL*/ )  //ALL deletes all children
{
	int children ;
	CScriptTree * Target ; 
	//Find the node to delete
	if(childIndex != ALL)
	{
		Target = FindNode(childIndex) ;
		Target->DeleteChild(ALL) ;
		//Delete the Variables in this node 
		int varCount = Target->m_VarInScope.GetSize() ;
		if(varCount)
		{
			for(int i = 0 ;i < varCount; i++)
			{
				delete Target->m_VarInScope.GetAt(i) ;
			}
		}
		//fix up the indices after a delete
		NotifyHeadofDelete(CScriptTree::ScriptHead, Target->m_index) ;

		// Remove the item from the chilren's array.
		children = m_Children.GetSize();
		for(int i = 0 ; i < children; i++)
		{
			Target  = (CScriptTree *)m_Children.GetAt(i) ;
			if (childIndex == Target->m_index)
			{
				m_Children.RemoveAt(i) ;
				break ;
			}
		}
		delete Target ;
		return ;
	}
	else
	{
		children = m_Children.GetSize();
		for(int i = 0 ; i < children; i++)
		{
			Target  = (CScriptTree *)m_Children.GetAt(i) ;
		    Target->DeleteChild(ALL) ;
		
			//Delete the Variables in this node 
			int varCount = Target->m_VarInScope.GetSize() ;
			if(varCount)
			{
				for(int i = 0 ;i < varCount; i++)
				{
					delete Target->m_VarInScope.GetAt(i) ;
				}
			}
			//fix up the indices after a delete
			NotifyHeadofDelete(CScriptTree::ScriptHead, Target->m_index) ;
			//Remove item from the array
			m_Children.RemoveAt(i) ;
			delete Target ;
			return ;
		}
	} 
}

BOOL CScriptTree::IsIndexInTree() 
{ 
	return TRUE ;
}

