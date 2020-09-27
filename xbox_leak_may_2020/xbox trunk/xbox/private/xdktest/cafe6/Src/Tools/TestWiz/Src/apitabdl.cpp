// apitabdl.cpp : implementation file
//

#include "stdafx.h"
#include "testwiz.h"
#include "data.h"
#include "scriptr.h"
#include "apitabdl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ApiTabdlg property page

extern TEXTMETRIC tm ;
extern int cxChar, cxCaps, cyChar ;

// Pens and Brushes we need to show selections.
extern CBrush brSelRect ;
extern CBrush brUnSelRect ;
extern CPen   penUnSelRect ;
extern CPen   penSelRect ;
extern CBitmap bmapUnCheck, bmapCheck, bmapTriangle, bmapNomoreItems, bmapMoreItems, bmapNone , bmapTestCase, bmapWarn;								 
CRect CasesListRect ;
BOOL MoveStartedInScriptBox = FALSE ;
BOOL WasInMotion = FALSE ;	

IMPLEMENT_DYNCREATE(ApiTabdlg, CPropertyPage)

ApiTabdlg::ApiTabdlg()
	: CPropertyPage(ApiTabdlg::IDD)
{
	//{{AFX_DATA_INIT(ApiTabdlg)
	m_HideVarsBraces = FALSE;
	m_ScriptItemTxt = _T("");
	m_iDisplayClassNames = FALSE;
	//}}AFX_DATA_INIT
}

ApiTabdlg::~ApiTabdlg()
{
}

void ApiTabdlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(ApiTabdlg)
	DDX_Control(pDX, IDC_SCRIPTTEXT, m_ScriptItemCtrl);
	DDX_Control(pDX, IDC_DIRECTIVES, m_directivesList);
	DDX_Control(pDX, IDC_ADD, m_AddButton);
	DDX_Check(pDX, IDC_HIDE, m_HideVarsBraces);
	DDX_Control(pDX, IDC_DESCRIPTION, m_apiDescription);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_parameterList);
	DDX_Control(pDX, IDC_LIST_API, m_apiList);
	DDX_Control(pDX, IDC_CATEGORY, m_categoryCB);
	DDX_CBString(pDX, IDC_CATEGORY, m_category);  
	DDX_Text(pDX, IDC_SCRIPTTEXT, m_ScriptItemTxt);
	DDX_Check(pDX, IDC_CLASS_NAME, m_iDisplayClassNames);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(ApiTabdlg, CPropertyPage)

	//{{AFX_MSG_MAP(ApiTabdlg)
	ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
	ON_LBN_DBLCLK(IDC_LIST_AREAS, OnDblclkListAreas)
	ON_LBN_SELCHANGE(IDC_LIST_AREAS, OnSelchangeListAreas)
	ON_LBN_SELCHANGE(IDC_LIST_API, OnSelchangeListApi)
	ON_LBN_DBLCLK(IDC_LIST_API, OnDblclkListApi)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_LBN_DBLCLK(IDC_SCRIPTLIST, OnDblclkScriptlist)
	ON_WM_RBUTTONDOWN()
	ON_WM_PARENTNOTIFY()
	ON_COMMAND(ID_POPUP_DELETE, OnPopupDelete)
	ON_COMMAND(ID_POPUP_WARN, OnPopuupWarnings)
	ON_BN_CLICKED(IDC_HIDE, OnHideVarsAndBraces)
	ON_WM_PAINT()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_LBN_DBLCLK(IDC_DIRECTIVES, OnDblclkDirectives)
	ON_BN_CLICKED(IDC_NEWTESTCASE, OnNewtestcase)
	ON_LBN_SELCHANGE(IDC_SCRIPTLIST, OnSelchangeScriptlist)
	ON_WM_VKEYTOITEM()
	ON_WM_KEYDOWN()
	ON_LBN_SETFOCUS(IDC_LIST_API, OnSetfocusListApi)
	ON_LBN_KILLFOCUS(IDC_LIST_API, OnKillfocusListApi)
	ON_BN_CLICKED(IDC_CLASS_NAME, OnDisplayClassNames)
	ON_LBN_SELCHANGE(IDC_LIST_PARAMETERS, OnSelchangeListParameters)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ApiTabdlg operations


void ApiTabdlg::DisplayAreas()
{
    m_areaList.ResetContent();
    CString str;
	int nAreas = m_areaArray.GetUpperBound();
	for (int i=0; i <= nAreas; i++) 
	{
		str = m_areaArray[i]->m_strName;
		if (m_category == m_areaArray[i]->m_strCategory)
		{
			int index = m_areaList.AddString(str);
			m_areaList.SetItemData(index,(DWORD)bmapUnCheck.GetSafeHandle());
		}
	}
    m_areaList.SetCurSel(0);
}

UINT ApiTabdlg::OpenAPIData(LPCSTR szFileName, UINT nFileType /*= API_FT_TEXT */)
{
	ASSERT( szFileName != NULL );
    ASSERT( m_apiArray.GetSize() == 0 );
   
	TRY                             // REVIEW:Use true exceptions
	{
		CStdioFile file(szFileName, CFile::modeRead | CFile::typeText);
		char acLine[512];
		while( file.ReadString(acLine, 511) != NULL )
			InterpretLine(acLine);
	}
	CATCH(CFileException, e)
	{
		AfxMessageBox(IDS_ERROR_CANT_OPEN);
		return FALSE;
	}
	END_CATCH;
	return TRUE;
}   

BOOL ApiTabdlg::InterpretLine(LPCSTR szNewLine)
{
	char* sz = new char[strlen(szNewLine) + 1];		 
	strcpy(sz, szNewLine);
	char* pCleanStr = sz;
	pCleanStr = RemoveExtraChars( pCleanStr );

	if (pCleanStr[0] == '\0')       // delete string if is empty line
	{
		delete[] sz;
		return FALSE;
	}

    FillStructures(pCleanStr);
	return TRUE;
}

char* ApiTabdlg::RemoveExtraChars(char* pchLine) 	  // REVIEW (EnriqueP): Move this code to CFileTableLine::CFileTableLine(pchStr)
{
	int iCRLF = strcspn(pchLine, "\'\r\n");	 // remove trailing comments , CR or LF
	pchLine[iCRLF] = '\0';
	pchLine += strspn(pchLine, "\t /");	// Remove leading spaces, tabs or "//"
	return pchLine;
}


void ApiTabdlg::FillStructures( LPCSTR szLine )
{
    ASSERT(szLine[0] != '\0');
	while( szLine[0] != '\0' )
	{
		int nSeparator = strcspn(szLine, ": \t");
        CString strLineType =  CString(szLine, nSeparator); // Get first word
        szLine = szLine + nSeparator + strspn(szLine + nSeparator, ": \t");  // Advance to next word
		strLineType.MakeUpper();
		
		if (strLineType == "CLASSNAME")			
            szLine = FillClassStruct(szLine);
	    if (strLineType == "BASECLASS")
            szLine = FillBaseClassStruct(szLine);
		if (strLineType == "CATEGORY")
            szLine = SetCurrentCategory(szLine);	

        if (strLineType == "FUNCTION" )
            szLine = FillAPIStruct(szLine);
		if (strLineType == "DESCRIPTION" )
            szLine = FillDescStruct(szLine);
        if (strLineType == "RETURN" )
            szLine = FillReturnStruct(szLine);
		if (strLineType == "PARAM" )
			if (m_bIsOldFormat)
				szLine = FillParamStruct(szLine);		
			else
				szLine = FillParamDescStruct(szLine);
//		Old formats
		if (strLineType == "API" )
		{
			m_bIsOldFormat = TRUE;
            szLine = FillAPIStruct(szLine);
		}
		if (strLineType == "AREA")
		{
			m_bIsOldFormat = TRUE;
            szLine = FillAreaStruct(szLine);
		}
        if (strLineType == "SUBAREA" )
		{
			m_bIsOldFormat = TRUE;
            szLine = FillSubareaStruct(szLine);
		}
// We skip any BEGIN_ OR END_ keywords
	}	
		
}

LPCSTR ApiTabdlg::SetCurrentCategory(LPCSTR szLine)	//REVIEW: Change to FillCategoryStruct()
{
	//DefaultAreaCtor();  
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	m_strCurrentCategory = CString(szLine,nSeparator);

    // REVIEW: Remove string table.

	// Check category is not already in combobox.
	int nStartAt = m_categoryCB.GetCount() - 1;
	int index = m_categoryCB.FindStringExact( nStartAt, m_strCurrentCategory.GetBuffer(m_strCurrentCategory.GetLength()) );
	m_strCurrentCategory.ReleaseBuffer();

	if (index == CB_ERR)
		m_categoryCB.AddString(m_strCurrentCategory);	// Add to Combo box if is not already there
	
	index = FindArea();  // REVIEW: Make sure index is not invalid. Old format will make it fail
    m_areaArray[index]->m_strCategory = m_strCurrentCategory;   
    
	szLine = szLine + nSeparator;  // REVIEW: Is this needed?

    return (szLine);
}


LPCSTR ApiTabdlg::FillAreaStruct(LPCSTR szLine)
{
	//DefaultAreaCtor();  
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	m_strCurrentArea = CString(szLine,nSeparator);

    // REVIEW: Search to see if the area has already been entered
    CArea* pArea = new CArea;
	pArea->m_strCategory = m_strCurrentCategory;
    pArea->m_strName = m_strCurrentArea;   
    m_areaArray.Add(pArea);
    
    szLine = szLine + nSeparator;  

    return (szLine);
	
}


LPCSTR ApiTabdlg::FillClassStruct(LPCSTR szLine)
{
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	m_strCurrentArea = CString(szLine,nSeparator);	// current class

    CArea* pArea = new CArea;
//	pArea->m_strCategory = m_strCurrentCategory;
    // REVIEW: Search to see if the area has already been entered

    pArea->m_strName = m_strCurrentArea;   
    m_areaArray.Add(pArea);
    
    szLine = szLine + nSeparator;  // REVIEW: Is this needed?

    return (szLine);
	
}

LPCSTR ApiTabdlg::FillBaseClassStruct(LPCSTR szLine)
{
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	CString strBaseClass = CString(szLine,nSeparator);
    

    // Search for current area (class) and add subarea to its list of subareas.
    int index = FindArea();  
    m_areaArray[index]->m_strBaseClass = strBaseClass;   
  
  	szLine = szLine + nSeparator;  // REVIEW: Is this needed?
    return (szLine);
}


LPCSTR ApiTabdlg::FillSubareaStruct(LPCSTR szLine)
{
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	CString strSubarea = CString(szLine,nSeparator);
    m_strCurrentSubarea = strSubarea; 
    
    szLine = szLine + nSeparator;  

    // REVIEW: Search for current area and add subarea to its list of subareas.
    int index = FindArea();
    ASSERT(index != -1); // There should be a matching Area in the array
    /* BOOL b = m_arreaArray[index].FindSubarea(m_currentSubArea);  
    if (b) 
        return (szLine); // if subarea already in list don't add another one.  */

    m_areaArray[index]->m_strListSubarea.AddTail(strSubarea);   //REVIEW: Possible bug
    
    return (szLine);
	
 }

LPCSTR ApiTabdlg::FillAPIStruct(LPCSTR szLine)
{
    ASSERT(szLine[0] != '\0');

	CAPI* pAPI = new CAPI;

	if (m_bIsOldFormat)
	{
		pAPI->m_strArea = m_strCurrentArea;			// Old Format
		pAPI->m_strSubarea = m_strCurrentSubarea;
	}
	else
	{
		pAPI->m_strArea = "Unassigned";	// There are no matching Class Names for this API
	}

    int iToken = 0;
	int nSeparator;
	while( szLine[0] != '\0' )
	{
		nSeparator = strcspn(szLine, "(: \t");
		switch( iToken++ )
		{
			case 0:		// parameter 1 : return value
				pAPI->m_strReturnVal += CString(szLine, nSeparator);
				if ( (pAPI->m_strReturnVal == "static") || (pAPI->m_strReturnVal == "virtual") || (pAPI->m_strReturnVal == "const"))
				{
					pAPI->m_strReturnVal+=' ';  // add modifier + return value,ei. virtual int
					iToken = 0;					// the next word is the actual type
				}
				break;
			case 1:		// parameter 2 : class name
				if (szLine[nSeparator] != ':')		// See if the separator is a :: 
				{
					pAPI->m_strClass.Empty();	// No class name, it's not a member function
					if (!m_bIsOldFormat)
						pAPI->m_strArea = "Globals";
					pAPI->m_strName = CString(szLine, nSeparator);
					m_strCurrentAPI = pAPI->m_strName;
					iToken++;
				}
				else
				{
					pAPI->m_strClass = CString(szLine, nSeparator);
					if (!m_bIsOldFormat)
						pAPI->m_strArea = CString(szLine, nSeparator);
				}
				break;
			case 2:		// parameter 3 : API name
				pAPI->m_strName = CString(szLine, nSeparator);
				m_strCurrentAPI = pAPI->m_strName;
				break;
			case 3:		// parameter 4 : Parameters		
				szLine = FillParamStruct(szLine, pAPI);
				nSeparator = 0;  // szLine already points to ')'
				// REVIEW: Add code here to fill param struct if necessary
				break;
			case 4:		// parameter 5 : Description 
				nSeparator = strcspn(szLine, "\0");
				pAPI->m_strDesc = CString(szLine, nSeparator);
				break;
		}
		szLine = szLine + nSeparator + strspn(szLine + nSeparator, "(,:) \t;");   
	}
	
	ASSERT(iToken >= 3);   // Make sure API line is not incomplete;   

	m_apiArray.Add(pAPI);

    // REVIEW: Add class to class list if not already there.
 	CString strClass = pAPI->m_strClass;
    
    return (szLine);
}

LPCSTR ApiTabdlg::FillDescStruct(LPCSTR szLine)
{
	int nSeparator = strcspn(szLine, "\0");
	CString strDescription = CString(szLine,nSeparator);

	ASSERT(m_apiArray.GetSize() != 0);

	CAPI* pAPI = m_apiArray[m_apiArray.GetUpperBound()];	// Last api in array is the current one

	ASSERT(pAPI->m_strName == m_strCurrentAPI);				
	
	pAPI->m_strDesc = strDescription;

	szLine = szLine + nSeparator;  // REVIEW: Is this needed?
    return (szLine);
}


LPCSTR ApiTabdlg::FillReturnStruct(LPCSTR szLine)
{
	int nSeparator = strcspn(szLine, "\0");
	CString strReturnDesc = CString(szLine,nSeparator);

	ASSERT(m_apiArray.GetSize() != 0);

	CAPI* pAPI = m_apiArray[m_apiArray.GetUpperBound()];	// Last api in array is the current one

	ASSERT(pAPI->m_strName == m_strCurrentAPI);				
	
	pAPI->m_strReturnDesc = strReturnDesc;

	szLine = szLine + nSeparator;  // REVIEW: Is this needed?
    return (szLine);
}



LPCSTR ApiTabdlg::FillParamStruct(LPCSTR szLine, CAPI* pAPI)
{
    ASSERT(szLine[0] != '\0');
	if (pAPI == NULL)
		ASSERT(m_apiArray.GetSize() != 0);

    int iParam = 0;
	int nSeparator = strcspn(szLine, ")");
	CString strParam = 	CString(szLine, nSeparator);
	if ( (strParam.Find("void") != -1) || ( strParam.Find("params") != -1 ))
		return szLine + nSeparator;			// takes no parameters	or uses PARAM to fill them in
	
	while( szLine[0] != ')' && szLine[0] != '\0')
	{
		CParam* pParam = new CParam;
		while ( (iParam % 3) < 2)		// Only do this loop 2 times
		{
			nSeparator = strcspn(szLine, ", \t/=)");
			switch( (iParam++) & (0x0001) )				  // REVIEW: We assume only 1 param type
			{
				case 0:		// parameter type
					pParam->m_strType = CString(szLine, nSeparator);
					break;
				case 1:		// parameter name
					pParam->m_strName = CString(szLine, nSeparator);
					break;
			}
			szLine = szLine + nSeparator + strspn(szLine + nSeparator, ", \t");	 // advance to next '=' or param type
			if (szLine[0] == '*')		// Is pointer type
			{
				pParam->m_strType += '*';
				szLine += strspn(szLine, " \t"); // advance to param name
			}
			if (szLine[0] == '=')			// old default value format
			{
				szLine += strspn(szLine, "= \t");  // advance to beginning of  default value
				nSeparator = strcspn(szLine, ", \t)");
				pParam->m_strValue = CString(szLine, nSeparator);
				szLine = szLine + nSeparator + strspn(szLine + nSeparator, ", \t");	 // advance to next param type
			}  

			if (szLine[0] == '/')			//  default value 
			{
				szLine += strspn(szLine, "=*/ \t");  // advance to beginning of  default value
				nSeparator = strcspn(szLine, ", \t*)");
				pParam->m_strValue = CString(szLine, nSeparator);
				szLine = szLine + nSeparator + strspn(szLine + nSeparator, "*/, \t");	 // advance to next param type
			}  
			
		 }
		 if (pAPI == NULL)		// Old format: From PARAM line, API is already in array
		 {
		 	ASSERT(m_apiArray.GetSize() != 0);
			m_apiArray[m_apiArray.GetUpperBound()]->m_paramList.AddTail(pParam); 
		 }
		 else
		 	pAPI->m_paramList.AddTail(pParam);		   // 	API object is created on the fly

		iParam = 0;									// Reset so more parameters can be read.
	}
	
    return (szLine);
	
 }


LPCSTR ApiTabdlg::FillParamDescStruct(LPCSTR szLine)
{
	ASSERT(m_apiArray.GetSize() != 0);

	CAPI* pAPI = m_apiArray[m_apiArray.GetUpperBound()];	// Last api in array is the current one

	ASSERT(pAPI->m_strName == m_strCurrentAPI);				
	

	int nSeparator = strcspn(szLine, "\0");
	CString strDesc = CString (szLine, nSeparator);

	szLine = szLine + nSeparator;  // REVIEW: Is this needed?

	CString strParamDesc;

	POSITION pos = pAPI->m_paramList.GetHeadPosition(); 
	
	while (pos != NULL)
	{
		strParamDesc = pAPI->m_paramList.GetAt(pos)->m_strName;
		if ( strDesc.Find(strParamDesc.GetBuffer(strParamDesc.GetLength() ) ) != -1 )	// Search for parameter name
		{
			pAPI->m_paramList.GetAt(pos)->m_strDesc = strDesc;		// Parameter found, assign description
			return szLine;
		}
		strParamDesc.ReleaseBuffer();
		pAPI->m_paramList.GetNext(pos);
	}
	
	ASSERT(TRUE);

	return szLine;
}


int ApiTabdlg::FindArea(LPCSTR szArea)
{
    CString strArea;
    if (szArea == NULL)
        strArea = m_strCurrentArea;
    else
        strArea = szArea;

    int nAreas = m_areaArray.GetUpperBound();
    for (int i=0; i <= nAreas; i++)
    {
        if (m_areaArray[i]->m_strName == strArea)
        return i;
    }

    return -1;  // Area not found
}

BOOL ApiTabdlg::IsArea(int index)
{
	CBitmap * curpbmap ;
	curpbmap = CBitmap::FromHandle((HBITMAP)m_areaList.GetItemData(index)) ;
	if (curpbmap->GetSafeHandle() != bmapTriangle.GetSafeHandle() )
		return TRUE;
	else
		return FALSE;
}

void ApiTabdlg::ExpandArea(int index, BOOL bExpand)
{
	if (bExpand)
	{
		m_areaList.SetItemData(index,(DWORD)bmapCheck.GetSafeHandle());
		DisplaySubareas(index);
	}
	else
	{
		m_areaList.SetItemData(index,(DWORD)bmapUnCheck.GetSafeHandle());
		HideSubareas(index);
	}
}

void ApiTabdlg::DisplaySubareas( int index, int i )
{
	CString	strText;
	m_areaList.GetText(index, strText);
	if (i == -1)
		i = FindArea(strText); 
	ASSERT( i != -1 );
	CString strNew;
	POSITION pos = m_areaArray[i]->m_strListSubarea.GetHeadPosition();
	while (pos != NULL )
	{
		strNew =  m_areaArray[i]->m_strListSubarea.GetNext(pos);
		index++;
		index = m_areaList.InsertString(index, strNew );
	 	m_areaList.SetItemData(index, (DWORD)bmapTriangle.GetSafeHandle()) ;	
	}
}

void ApiTabdlg::HideSubareas( int index, int i )
{
	CString	strText;
	m_areaList.GetText(index, strText);
	if (i == -1)
		i = FindArea(strText);
	POSITION pos = m_areaArray[i]->m_strListSubarea.GetHeadPosition();
	index++;
	while (pos != NULL)
	{
		m_areaList.DeleteString(index);
		m_areaArray[i]->m_strListSubarea.GetNext(pos);
	}
}

void ApiTabdlg::DisplayAreaAPIs( int index )
{
	CString	strArea;
	CString strNew;
	CString strClass;
	m_areaList.GetText(index, strArea);
	int nLast = m_apiArray.GetUpperBound();

	m_apiList.ResetContent();
	for (int i=0; i <= nLast; i++)
	{
		if (m_apiArray[i]->m_strArea == strArea)
		{
			strClass = 	m_apiArray[i]->m_strClass;
			if (!m_apiArray[i]->m_strClass.IsEmpty()) // If is a member function
				strClass += "::";
			if ( m_iDisplayClassNames )
			{
				strNew = m_apiArray[i]->m_strReturnVal + " " + strClass + m_apiArray[i]->m_strName + "()";
			}
			else
			{
				strNew = m_apiArray[i]->m_strReturnVal + " " + m_apiArray[i]->m_strName + "()"; // Don't display Class Name
			}
			index = m_apiList.AddString( strNew );
			m_apiList.SetItemData( index, (DWORD)i );
		}
	}
}
			
void ApiTabdlg::DisplaySubareaAPIs( int index )
{
	CString	strArea;
	CString strSubarea;
	CString strNew;
	m_areaList.GetText(index, strSubarea);
	strArea = GetAreaName(index);
	int nLast = m_apiArray.GetUpperBound();

	m_apiList.ResetContent();
	for (int i=0; i <= nLast; i++)
	{
		if ( (m_apiArray[i]->m_strArea == strArea) && (m_apiArray[i]->m_strSubarea == strSubarea))
		{
			strNew = m_apiArray[i]->m_strReturnVal + " " + m_apiArray[i]->m_strClass + "::" + m_apiArray[i]->m_strName + "()";
			index = m_apiList.AddString( strNew );
			m_apiList.SetItemData( index, (DWORD)i );
		}
	}
}
			
CString ApiTabdlg::GetAreaName( int index )
{
	CString	strArea;
	while ( !IsArea(index) )
		index--;	  // Go up the list to find the Area
	ASSERT (index >= 0);
	m_areaList.GetText(index, strArea);
	
	return strArea;
}

void ApiTabdlg::DisplayParameters( int index, BOOL bAddedToScript )
{
	m_parameterList.ResetContent();

	int i = index;

	if (!bAddedToScript)
		i = m_apiList.GetItemData(index);  // If Selected API is in API List
		
	POSITION pos = m_apiArray[i]->m_paramList.GetHeadPosition();
	
	while (pos != NULL )
	{  	
		CParam* pParam =  m_apiArray[i]->m_paramList.GetNext(pos);
		CString strParam = pParam->m_strType + "\t" + pParam->m_strName + "\t" + pParam->m_strValue;
		int index = m_parameterList.AddString( strParam );
		m_parameterList.SetItemDataPtr(index, pParam);
	}
}

void ApiTabdlg::DisplayAPIDescription( int index )
{
	
	int i = m_apiList.GetItemData(index);
	CString strDesc = m_apiArray[i]->m_strDesc;
	m_apiDescription.SetWindowText( strDesc );
}


void ApiTabdlg::DisplayParamDescription( int index )
{
	
	CParam* pParam = (CParam*)m_parameterList.GetItemDataPtr(index);
	CString strDesc = pParam->m_strDesc;
	m_apiDescription.SetWindowText( strDesc );
}


CString ApiTabdlg::CreateVarFromClassName(CString& strClassName)
{
	int nLength = strClassName.GetLength();
	ASSERT (nLength > 0);					//REVIEW: Handle this error condition gracefully

	int  i = 0;
	int  nLastUpper = i;
	while ( isupper(strClassName[i]) )
	{
		nLastUpper = i++;
		if (i >= nLength)
			break;	
	}

	CString strVar;
	if ( (nLength - nLastUpper) < 5 )
		strVar = strClassName.Mid(nLastUpper);
	else
		strVar = strClassName.Mid(nLastUpper, 5);
	
	strVar.MakeLower();

	return strVar;
}	

/////////////////////////////////////////////////////////////////////////////
// ApiTabdlg message handlers

BOOL ApiTabdlg::OnSetActive()
{

    BOOL bReturn = CPropertyPage::OnSetActive();
/*	
	CString path("0",250) ;
	GetModuleFileName(NULL,path.GetBuffer(250),250) ;
	path = path.Left(path.ReverseFind('\\') + 1) ;
	path =path +"appwiz\\apidata.txt" ;

    if (m_apiArray.GetSize() == 0)
        OpenAPIData(path.GetBuffer(250));
  */

    return  bReturn;
}


void ApiTabdlg::OnSelchangeCategory() 
{
    CString strCurrentText;
	int index = m_categoryCB.GetCurSel() ;
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbo.
		return ;  
	m_categoryCB.GetLBText( index , strCurrentText );
    m_category = strCurrentText;
	DisplayAreas();
	OnSelchangeListAreas();
}


BOOL ApiTabdlg::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   	CString path("0",250) ;
	GetModuleFileName(NULL,path.GetBuffer(250),250) ;
	path = path.Left(path.ReverseFind('\\') + 1) ;
	
//	Debug version uses the ided directory
	#ifdef _DEBUG
	path =path +"ided\\apidata.txt" ;
	#else
	path =path +"ide\\apidata.txt" ;
	#endif

    m_bIsOldFormat = FALSE;
	
	if (m_apiArray.GetSize() == 0)
        OpenAPIData(path.GetBuffer(250));

	CClientDC ccldc(this) ;         
 	// Add Custom listbox
	// subclass the control	 
	VERIFY(m_ScriptList.SubclassDlgItem(IDC_SCRIPTLIST, this));

	VERIFY(m_areaList.SubclassDlgItem(IDC_LIST_AREAS, this));

   // init the directives combo box
	int index = m_directivesList.AddString("if ( ) ") ;
	index = m_directivesList.AddString("else (  )") ;
	index = m_directivesList.AddString("for ( ) ") ;
	index = m_directivesList.AddString("m_pLog->RecordFailure(\" \")") ;
	index = m_directivesList.AddString("m_pLog->RecordInfo(\" \")") ;
	index = m_directivesList.AddString("m_pLog->Comment(\" \")") ;
	
	//Setup the Script listbox.
	CScriptTree::ScriptHead = new CScriptTree ;
	CScriptTree::ScriptHead->m_level = 0 ; 
	CScriptTree::ScriptHead->SetIndex(0) ;
	CScriptTree::ScriptHead->SetListBox(&m_ScriptList) ;
	CScriptTree::ScriptHead->SetText("TEST SCRIPT") ;
	CScriptTree::ScriptHead->SetBitmap((HBITMAP)bmapNomoreItems.GetSafeHandle()) ;
	// Insert an empty string.
	CScriptTree * empty = new CScriptTree ;
	
	empty->SetListBox(&m_ScriptList) ;
	empty->SetText(" ") ;
	empty->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	CScriptTree::ScriptHead->AddNode(empty) ;
	
   	CScriptTree::ScriptHead->DrawScript() ;

	OnNewtestcase() ; 
	// Set column sizes for Parameter Listbox
	CRect rect;
	m_parameterList.GetClientRect(rect);
	LPRECT pRectParam = rect;
	int nTabStop = (pRectParam->right - pRectParam->left) / 6;
	m_parameterList.SetTabStops( nTabStop  );
	// Display Api's
	
	m_categoryCB.SetCurSel(0);	// Select first category
	OnSelchangeCategory();	
	DisplayAreas();

	m_AddButton.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void ApiTabdlg::OnDblclkListAreas() 
{
	int index =  m_areaList.GetCurSel() ;
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbox.
		return ;  
	if ( IsArea(index) )
	{
		//Expand or collapse Area 
		CBitmap * curpbmap ;
		curpbmap = CBitmap::FromHandle((HBITMAP)m_areaList.GetItemData(index)) ;
		if (curpbmap->GetSafeHandle() == bmapCheck.GetSafeHandle()) // if currently checked
			ExpandArea(index, FALSE);
		else 
			ExpandArea(index);

		m_areaList.SetCurSel(index) ;
	}
}


void ApiTabdlg::OnSelchangeListAreas() 
{
    CString strCurrentText;
    int index = m_areaList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbox.
		return ;  

	if ( IsArea(index) )
		DisplayAreaAPIs(index);
	else
		DisplaySubareaAPIs(index);
}

void ApiTabdlg::OnSelchangeListApi() 
{
    int index = m_apiList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbo.
		return ;  

	DisplayParameters( index );
	DisplayAPIDescription( index );
}

void ApiTabdlg::OnDblclkListApi() 
{
	int index = m_ScriptList.GetCurSel() ;
	if (LB_ERR ==index || 0 > index)
		index = 0 ;
	int iApi = m_apiList.GetCurSel() ;
	if (LB_ERR ==iApi || 0 > iApi) // If an error or no items is selected in listbox.
		return ;  				   // Possible because we call this function when Add button clicked 
  	//Fill prepare an item to be added to the script. 
	CScriptTree *strNode ;
	strNode = new CScriptTree ;
	
	//Associate the listbox with the script tree.
	strNode->SetListBox(&m_ScriptList) ;
	strNode->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	/* Get class type, api name and parameter names */
	int i = m_apiList.GetItemData(iApi);
	strNode->m_apiIndex = i;
	CString strAPI = m_apiArray[i]->m_strName;  // REVIEW: Add Parameters
	CString strClassName = m_apiArray[i]->m_strClass ;
	POSITION pos = m_apiArray[i]->m_paramList.GetHeadPosition();
	CString strParams ='(';
	CString strSeparator = " ";
	while (pos != NULL )
	{
		// Since the API has arguments, give it a warning bitmap
		strNode->SetBitmap((HBITMAP)bmapWarn.GetSafeHandle()) ;
		CParam* pParam =  m_apiArray[i]->m_paramList.GetNext(pos);
		if ( pParam->m_strValue.IsEmpty())
			strParams = strParams + strSeparator + pParam->m_strName;
		else
			break;	   // If is default value don't add to string
		strSeparator = ", ";
	}
	strParams = strParams + " )";

	// Add the new string node to the currently selected parent.
	CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(index));
	int nodeID = SelectedNode->m_index ;
	CScriptTree * temp = CScriptTree::ScriptHead->FindParent(nodeID) ;;
	CString strVarName ;
	if(0 == nodeID) // Cant insert at index 0, reserved for Test Tree keyword.
		nodeID++ ;

	// If no varible of this type is declared yet, declare one.
	if ((strVarName = temp->GetObjectName(strClassName)) == "NULL") 
	{
		
		CScriptTree *varNode ;
		varNode = new CScriptTree ;
	    
		if(!m_HideVarsBraces)
			varNode->m_Hidable = NOTHIDDEN ;
		else
			varNode->m_Hidable = HIDDEN ;
		//Create a variable declaration.
		 CVariable * pNewVar = new CVariable ;

		int iVar ;
		if(temp == CScriptTree::ScriptHead)	   //Choosing a position for the variable.
		 	iVar = temp->m_index +1 ;
		else iVar = temp->m_index + 2 ; 

		varNode->SetListBox(&m_ScriptList) ;
		varNode->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	  	
		pNewVar->ClsName = strClassName; 
		pNewVar->VarName = CreateVarFromClassName(strClassName) ;
		temp->AddToVarsInScope(pNewVar) ;
		
		varNode->SetText(strClassName + "  " + pNewVar->VarName +";"  ) ;// Var declaration
		strNode->SetText(pNewVar->VarName + '.' + strAPI + strParams +";" ) ; 			// Var.API 			
	
		temp->AddNode(strNode,nodeID) ;
		temp->AddNode(varNode,iVar) ;
		index++ ; //Need to move selection down two positions if a veriable was added.	
	}
	else
	{
		strNode->SetText(strVarName + '.' + strAPI + strParams +";" );
		temp->AddNode(strNode,nodeID) ;
	}
	
	CScriptTree::ScriptHead->DrawScript() ; // Redraw the tree. 
	m_ScriptList.SetCurSel(index + 1) ;
	
}

void ApiTabdlg::OnDblclkScriptlist() 
{
	// TODO: Add your control notification handler code here
	int index =  m_ScriptList.GetCurSel() ;
	int nodeID ;
	// Find the node which contains the clicked item.
	CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(index));
	nodeID = SelectedNode->m_index ;

	if(SelectedNode->IsExpanded())
		{SelectedNode->CollapseTree() ;}
	else 
		if(SelectedNode->IsCollapsed())
	 		{SelectedNode->ExpandTree(nodeID) ;}
		else
			return ;
	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(index) ;
}

void ApiTabdlg::OnRButtonDown(UINT nFlags, CPoint point) 
{		
	// Convert the client coordinates to screen cordinates.
	RECT rect ;
	rect.top = point.y ;
	rect.left = point.x;
	rect.bottom = 0 ;
	rect.right = 0;
	ClientToScreen(&rect) ;
	point.x =rect.left ; 
	point.y =rect.top ;

	// TODO: Add your message handler code here and/or call default	 Cwinapp
	if (CasesListRect.PtInRect(point))	 // If we were inside the Cases listbox 
	{
		CMenu mnPopup ;
		mnPopup.CreatePopupMenu() ;
		mnPopup.AppendMenu(MF_STRING,IDC_MOVE_UP,"&Move Up") ;
		mnPopup.AppendMenu(MF_STRING,IDC_MOVE_DOWN,"&Move Down") ;
		mnPopup.AppendMenu(MF_STRING,ID_POPUP_DELETE,"&Delete") ;
		mnPopup.AppendMenu(MF_STRING,ID_POPUP_WARN,"&Warnings") ;
		
		mnPopup.TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,rect.left,rect.top,(this),NULL) ;
		CPropertyPage::OnRButtonDown(nFlags, point);
	}
}


void ApiTabdlg::OnParentNotify(UINT message, LPARAM lParam) 
{
	// Needed to trap the left mouse click on listbox.
	CPoint point(LOWORD(lParam),HIWORD(lParam)) ;
  	if(WM_RBUTTONDOWN == message)
	{
		OnRButtonDown(0,point) ;
	}
	else
		if(WM_LBUTTONDOWN == message)
		{
			OnLButtonDown(0,point) ;	
		}
		else
			if(WM_MOUSEMOVE == message)
			{
			 OnMouseMove(0,point);
			}

	CPropertyPage::OnParentNotify(message, lParam);
}

void ApiTabdlg::OnPopupDelete() 
{
     int icursel =  m_ScriptList.GetCurSel() ;
	if (LB_ERR ==icursel || 0 > icursel)
	{
		MessageBox("First select an item to delete.","Delete Item",MB_OK|MB_ICONINFORMATION) ;
		CScriptTree::ScriptHead->SetFocusOnListBox() ;
		return ;
	}
	if (0 ==icursel)
	{
		MessageBox("You cannot delete this item.","Delete Item",MB_OK|MB_ICONSTOP) ;
		CScriptTree::ScriptHead->SetFocusOnListBox() ;
		return ;
	}
	CString itemTxt("0",30) ;
	m_ScriptList.GetText(icursel,itemTxt) ;
	if((itemTxt.SpanExcluding("{}")).IsEmpty())
	{
		MessageBox("Cannot delete a block character.","Delete Error",MB_OK|MB_ICONSTOP) ;
		CScriptTree::ScriptHead->SetFocusOnListBox() ;
		return ;
	}
	// Find the node which contains the clicked item.
	CScriptTree * temp,  * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel));
	int nodeID = SelectedNode->m_index ;

	if(SelectedNode->m_Children.GetSize())
	 if(MessageBox("Delete the selected item with its children?","Comfirm Delete",MB_OKCANCEL|MB_ICONQUESTION)!= IDOK)
	 	return ;
	temp =  CScriptTree::ScriptHead->FindParent(nodeID) ;
	temp->DeleteChild(nodeID);
	CScriptTree::ScriptHead->DrawScript() ;

	//Select an item
	if(m_ScriptList.GetCount() < icursel)
		icursel-- ;
   	m_ScriptList.SetCurSel(icursel)	;
	CScriptTree::ScriptHead->SetFocusOnListBox() ;
}	

void ApiTabdlg::OnPopuupWarnings() 
{
    int icursel =  m_ScriptList.GetCurSel() ;
	if (LB_ERR ==icursel || 0 > icursel)
		icursel = 0 ;
	if (0 ==icursel)
		return ;
	CString itemTxt("0",30) ;
	m_ScriptList.GetText(icursel,itemTxt) ;
	
	if((itemTxt.SpanExcluding("{}")).IsEmpty())
	return ;
	// Find the currently selected node.
	CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel));
    if(SelectedNode->IsWarningON())
		{SelectedNode->WarningOFF() ;}
	else 
		if(SelectedNode->IsWarningOFF())
			{SelectedNode->WarningON() ;}
		else 
			return;
	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(icursel)	;
	CScriptTree::ScriptHead->SetFocusOnListBox() ;
}

void ApiTabdlg::OnHideVarsAndBraces() 
{
	UpdateData() ;
	//Toggles hiding and displaying variable declarations and braces. 
	CScriptTree::ScriptHead->HideVars_Blocks(m_HideVarsBraces) ;
	CScriptTree::ScriptHead->DrawScript() ;
	
}


void ApiTabdlg::OnAdd() 	// Add API to Script
{
	OnDblclkListApi();
}


BOOL ApiTabdlg::OnKillActive()
{
	CWnd *focus = GetFocus() ;
	CWnd *TxtEdt = GetDlgItem(IDC_SCRIPTTEXT) ;	// If the user pressed Enter after editing 

	if(focus->m_hWnd == TxtEdt->m_hWnd)
	{
		int icursel =  m_ScriptList.GetCurSel() ;
		if (LB_ERR ==icursel || 0 > icursel)
			icursel = 0 ;
		
		if (0 ==icursel) //Can't edit first item
			return FALSE;
			
		CString itemTxt("0",100) ;
		m_ScriptList.GetText(icursel,itemTxt) ;
	
		if((itemTxt.SpanExcluding("{}")).IsEmpty()) //Can't edit block characters.
			return FALSE ;
			
		// Find the node which contains the clicked item.
		CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel));
		
		UpdateData(TRUE); 
		SelectedNode->SetText(m_ScriptItemTxt) ;
	
		CScriptTree::ScriptHead->DrawScript() ;
		//Select an item
		m_ScriptList.SetCurSel(icursel)	;
		CScriptTree::ScriptHead->SetFocusOnListBox() ;
		return FALSE ;
	}
	else 
	 return (MessageBox("Do you want to apply your changes?","Comfirm close",MB_OKCANCEL|MB_ICONQUESTION)== IDOK) ;
	//return TRUE ;
		
}


void ApiTabdlg::OnOK()
{
	// Get all the script text and stick it in a CString object ;
	CString ScriptBuffer ;
	CScriptTree::ScriptHead->DumpIntoString(&ScriptBuffer) ;

	// Add the string object to the dictionary.
	awx.m_Dictionary.SetAt("Script_Text",ScriptBuffer) ;

// create the header for the TestCase class 


//Create the header for the Driver class.	
}

void ApiTabdlg::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Get the sorrounding rect of the CasesListbox
   	CWnd	*hNameCtl ;
   	hNameCtl = (this->GetDlgItem(IDC_SCRIPTLIST)) ;
   	hNameCtl->GetWindowRect(&CasesListRect) ;

	// TODO: Add your message handler code here
	
	// Do not call CPropertyPage::OnPaint() for painting messages
}

void ApiTabdlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
		RECT rect ;
	rect.top = point.y ;
	rect.left = point.x;
	rect.bottom = 0 ;
	rect.right = 0;
	ClientToScreen(&rect) ;
	point.x =rect.left ; 
	point.y =rect.top ;

	//if the Leftmouse button is down, then we might be trying to move something
	if(MoveStartedInScriptBox)
	{
		WasInMotion = TRUE ;
			
		if (CasesListRect.PtInRect(point))	 // If we were inside the Cases listbox 
		{
			//CHANGE CURSOR.	
		}
	}
	WasInMotion = FALSE ;
	CPropertyPage::OnMouseMove(nFlags, point);
}

void ApiTabdlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
		// TODO: Add your message handler code here and/or call default
	RECT rect ;
	rect.top = point.y ;
	rect.left = point.x;
	rect.bottom = 0 ;
	rect.right = 0;
	ClientToScreen(&rect) ;
	point.x =rect.left ; 
	point.y =rect.top ;

	//if the mouse had moved with the left button down
	if(WasInMotion && MoveStartedInScriptBox)
	{	
		if (CasesListRect.PtInRect(point))	 // If we were inside the Cases listbox 
		{
			//Get currently selected item
				AfxMessageBox("Could move this item");
			// Make previously selected item child of currently selected one.

		}
	}
	MoveStartedInScriptBox = FALSE ;
	WasInMotion = FALSE ;	
	CPropertyPage::OnLButtonUp(nFlags, point);
} 

void ApiTabdlg::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
			// TODO: Add your message handler code here and/or call default
	RECT rect ;
	rect.top = point.y ;
	rect.left = point.x;
	rect.bottom = 0 ;
	rect.right = 0;
	ClientToScreen(&rect) ;
	point.x =rect.left ; 
	point.y =rect.top ;

	if (CasesListRect.PtInRect(point))	 // If we were inside the Cases listbox 
	{
		//Get currently selected item
		MoveStartedInScriptBox = TRUE ;
	}
	else
		MoveStartedInScriptBox = FALSE ;

	CPropertyPage::OnLButtonDown(nFlags, point);
}

void ApiTabdlg::OnMoveDown() 
{
 	int icursel =  m_ScriptList.GetCurSel() ;
	if (LB_ERR ==icursel || 0 >= icursel )
	{
		MessageBox("Cannot move this item.","Move Item",MB_OK|MB_ICONINFORMATION) ;
		return ;
	}
	if (m_ScriptList.GetCount() ==icursel+1)
	{
		return ;
	} 
	CString itemTxt("0",30) ;
	m_ScriptList.GetText(icursel,itemTxt) ;
	CString bellowTxt("0",30) ;
	m_ScriptList.GetText(icursel+1,bellowTxt) ;
	if((itemTxt.SpanExcluding("{}")).IsEmpty())
	{
		MessageBox("Cannot move a block character.","Move Error",MB_OK|MB_ICONSTOP) ;
		return ;
	}
	if((bellowTxt.SpanExcluding("{}")).IsEmpty())
	{
		MessageBox("Cannot move this item down.","Move Error",MB_OK|MB_ICONSTOP) ;
		return ;
	}
	 //Select the node bellow and move it up.
  	m_ScriptList.SetCurSel(icursel+1) ;
  	OnMoveUp() ;	
	m_ScriptList.SetCurSel(icursel+1) ;
}

void ApiTabdlg::OnMoveUp() 
{	
	int icursel =  m_ScriptList.GetCurSel() ;
	CString itemTxt("0",30) ;
	m_ScriptList.GetText(icursel,itemTxt) ;
	CString aboveTxt("0",30) ;
	m_ScriptList.GetText(icursel -1,aboveTxt) ;
		
	if (LB_ERR ==icursel || 0 >= icursel )
	{
		MessageBox("Cannot move this item.","Move Item",MB_OK|MB_ICONINFORMATION) ;
		return ;
	}
	if (icursel == 1)
	{
		MessageBox("You cannot move this item up.","Move Error",MB_OK|MB_ICONSTOP) ;
		return ;
	}
	if((itemTxt.SpanExcluding("{}")).IsEmpty())
	{
		MessageBox("Cannot move a block character.","Move Error",MB_OK|MB_ICONSTOP) ;
		return ;
	}
	if((aboveTxt.SpanExcluding("{}")).IsEmpty())
	{
		MessageBox("Cannot move this item up.","Move Error",MB_OK|MB_ICONSTOP) ;
		return ;
	}
	// Find the node which contains the clicked item.
	CScriptTree * prevNode,  * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel));
	int nodeID = SelectedNode->m_index ;
	//Find parent of clicked item so she can dissown the node we want to move.
	CScriptTree * Parent = CScriptTree::ScriptHead->FindParent(nodeID) ;
	Parent->DissownChild(nodeID) ;
	SelectedNode->MoveUp() ;
	//Find the previous node since its going to move down.
	prevNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel-1));
	prevNode->MoveDown() ;
	// Find the parent of the node above since that's the new parent of the moved node.
	Parent = CScriptTree::ScriptHead->FindParent(prevNode->m_index) ;
	//Readd the child.
	Parent->AddNode(SelectedNode,prevNode->m_index) ;
	//Select an item
	icursel-- ;
   	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(icursel)	;
}

void ApiTabdlg::OnDblclkDirectives() 
{
	int index =  m_directivesList.GetCurSel() ;
   //	UpdateData() ;
																	 
	CString strDirective("0",30) ;									 
	m_directivesList.GetText(index,strDirective) ; 					 
	strDirective = strDirective.SpanExcluding(".") ;
 
 	index = m_ScriptList.GetCurSel() ;
	if (LB_ERR ==index || 0 > index)
		index = 0 ;

	//Fill in a ScriptTree node ;
	CScriptTree * keyWord, *ltbrace, *rtbrace, *empty ;

	if ((strDirective == "m_pLog->RecordFailure(\" \")")
		||(strDirective == "m_pLog->RecordInfo(\" \")") //
		||(strDirective == "m_pLog->Comment(\" \")") ) // These three don't need block characters.
	{
	 	keyWord = new CScriptTree ; //Works for all three 
		keyWord->SetListBox(&m_ScriptList) ;
		keyWord->SetText(strDirective +";") ;
		keyWord->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;

		//Find parent of the node they clicked inside of.
		CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(index));
		int nodeID = SelectedNode->m_index ;

		CScriptTree * temp = CScriptTree::ScriptHead->FindParent(nodeID) ;
	
		//Add the directive node.
		if(0 == nodeID) // Cant insert at index 0, reserved for Test Tree keyword.
		nodeID++ ;
		temp->AddNode(keyWord,nodeID) ;
	
	}
	else 
	{
		ltbrace = new CScriptTree ;
		rtbrace = new CScriptTree ;
		empty = new CScriptTree ;
		keyWord= new CScriptTree ;
		//Associate the listbox with the script tree.

		empty->SetListBox(&m_ScriptList) ;
		ltbrace->SetListBox(&m_ScriptList) ;
		rtbrace->SetListBox(&m_ScriptList) ;
		keyWord->SetListBox(&m_ScriptList) ;
	
		keyWord->SetText(strDirective) ;
		ltbrace->SetText("{") ;
		empty->SetText(" ") ;
		rtbrace->SetText("}") ;

		// Turn the braces on
		if(!m_HideVarsBraces)
		{
			ltbrace->m_Hidable = NOTHIDDEN ;
			rtbrace->m_Hidable = NOTHIDDEN ;
		}
		else
		{
			ltbrace->m_Hidable = HIDDEN ;
			rtbrace->m_Hidable = HIDDEN ;
		}
		empty->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
		ltbrace->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
		rtbrace->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
		
		//TestCase keyword is of special interest since it's a function name. CString
	/*	if (strDirective == "TestCase ( )")
		{ 	static icount =1 ;
			CString num ;
			num.Format("%d",icount);
			keyWord->SetText(strDirective.SpanExcluding(" ")+num+"( )");
			keyWord->m_TestCase = TRUE ;
			icount++ ;
		} */
									
		keyWord->SetBitmap((HBITMAP)bmapNomoreItems.GetSafeHandle()) ;
		//Find parent of the node they clicked inside of.
		CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(index));
		int nodeID = SelectedNode->m_index ;

		CScriptTree * temp = CScriptTree::ScriptHead->FindParent(nodeID) ;
	
		//Add the directive node.
		if(0 == nodeID) // Cant insert at index 0, reserved for Test Tree keyword.
		nodeID++ ;
		temp->AddNode(keyWord,nodeID) ;
	
		keyWord->AddNode(ltbrace) ;
		keyWord->AddNode(empty) ;
		keyWord->AddNode(rtbrace) ;
	}
	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(index) ;	
}

void ApiTabdlg::OnNewtestcase() 
{
	// TODO: Add your control notification handler code here
	//Fill in a ScriptTree node ;
	int index = m_ScriptList.GetCurSel() ;
	if (LB_ERR ==index || 0 > index)
		index = 0 ;

	CString strDirective = "TestCase ( )" ;
	CScriptTree * keyWord, *ltbrace, *rtbrace, *empty, *Return ;
	
	Return = new CScriptTree ;
	ltbrace = new CScriptTree ;
	rtbrace = new CScriptTree ;
	empty = new CScriptTree ;
	keyWord= new CScriptTree ;
	//Associate the listbox with the script tree.
	Return->SetListBox(&m_ScriptList) ;
	empty->SetListBox(&m_ScriptList) ;
	ltbrace->SetListBox(&m_ScriptList) ;
	rtbrace->SetListBox(&m_ScriptList) ;
	keyWord->SetListBox(&m_ScriptList) ;

	//keyWord->SetText("TestCase) ;
	Return->SetText("return;") ;
	ltbrace->SetText("{") ;
	empty->SetText(" ") ;
	rtbrace->SetText("}") ;

	// Turn the braces on
	if(!m_HideVarsBraces)
	{
		ltbrace->m_Hidable = NOTHIDDEN ;
		rtbrace->m_Hidable = NOTHIDDEN ;
	}
	else
	{
		ltbrace->m_Hidable = HIDDEN ;
		rtbrace->m_Hidable = HIDDEN ;
	}
	empty->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	ltbrace->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	rtbrace->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	Return->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	
	//TestCase keyword is of special interest since it's a function name. CString
	static icount =1 ;
	CString num ;
	num.Format("%d",icount);
	keyWord->SetText(strDirective.SpanExcluding(" ")+num+"( )");
	keyWord->m_TestCase = TRUE ;
	icount++ ;
	
	keyWord->SetBitmap((HBITMAP)bmapNomoreItems.GetSafeHandle()) ;
	//A new TestCase always goes at the bottom of the listbox, and is a child of the Head.
	int nodeID = m_ScriptList.GetCount() ;
  	//Add the directive node.
	CScriptTree::ScriptHead->AddNode(keyWord,nodeID) ;

	keyWord->AddNode(ltbrace) ;
	keyWord->AddNode(empty) ;
	keyWord->AddNode(Return) ;	
	keyWord->AddNode(rtbrace) ;

	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(nodeID + 2) ;	

}

void ApiTabdlg::OnSelchangeScriptlist() 
{
	//Get the currently selected item text and insert it in the editcontrol on the top of the listbox.

  	int icursel =  m_ScriptList.GetCurSel() ;
	if (LB_ERR ==icursel || 0 > icursel)
		icursel = 0 ;
		
	if (0 ==icursel)
		return ; //Ignore the first item.

		
	CString itemTxt("0",30) ;
	m_ScriptList.GetText(icursel,itemTxt) ;
	
	if((itemTxt.SpanExcluding("{}")).IsEmpty())
		return ; //Can't edit the braces
		
	// Find the node which contains the clicked item.
	CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(icursel));
	m_ScriptItemTxt =  SelectedNode->m_Text ;
	// Display Parameters if an API is selected
	int iAPI = SelectedNode->m_apiIndex;
	if (iAPI != -1)
		DisplayParameters(iAPI, TRUE);

	UpdateData(FALSE) ;
}

int ApiTabdlg::OnVKeyToItem(UINT nKey, CListBox* pListBox, UINT nIndex) 
{
  	CWnd* EditCtrl = GetDlgItem(IDC_SCRIPTLIST) ;

	if(pListBox->m_hWnd == EditCtrl->m_hWnd)
	{
		// If the pressed key is an Insert, add a blank line to the script 
		if(nKey == VK_INSERT)
		{
			OnInsert();
		}
		// If the pressed key is a Delete, delete the selected lineir 
		if(nKey == VK_DELETE)
		{
		 	OnPopupDelete() ;
		}
		CScriptTree::ScriptHead->SetFocusOnListBox() ;
	}
	return -1 ;

	//return CPropertyPage::OnVKeyToItem(nKey, pListBox, nIndex);
}

void ApiTabdlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CPropertyPage::OnKeyDown(nChar, nRepCnt, nFlags);
}

void ApiTabdlg::OnSetfocusListApi() 
{
	// Enable Add button only when this listbox has focus
	m_AddButton.EnableWindow();
	
}

void ApiTabdlg::OnKillfocusListApi() 
{
//	m_AddButton.EnableWindow(FALSE);
	
}

void ApiTabdlg::OnInsert() 
{
	int index = m_ScriptList.GetCurSel() ;
	if (LB_ERR ==index || 0 > index)
		index = 0 ;

	//Fill in a ScriptTree node ;
	CScriptTree  *empty ;
	empty = new CScriptTree ;
	empty->SetListBox(&m_ScriptList) ;
	empty->SetText(" ") ;
	empty->SetBitmap((HBITMAP)bmapNone.GetSafeHandle()) ;
	//Find parent of the node they clicked inside of.
	CScriptTree * SelectedNode = ((CScriptTree* )m_ScriptList.GetItemData(index));
	int nodeID = SelectedNode->m_index ;

	CScriptTree * temp = CScriptTree::ScriptHead->FindParent(nodeID) ;
	
	//Add the directive node.
	if(0 == nodeID) // Cant insert at index 0, reserved for Test Tree keyword.
		nodeID++ ;
	temp->AddNode(empty,nodeID) ;
	CScriptTree::ScriptHead->DrawScript() ;
	m_ScriptList.SetCurSel(index) ;	
}

void ApiTabdlg::OnDisplayClassNames() 
{
	m_iDisplayClassNames = !m_iDisplayClassNames;
	OnSelchangeListAreas();
}

void ApiTabdlg::OnSelchangeListParameters() 
{
	int index = m_parameterList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbo.
		return ;  

	DisplayParamDescription( index );
}
