// apipage.cpp : implementation file
//

#include "stdafx.h"
#include "apiwiz.h"
#include "data.h"
#include "apipage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAPIPage property page

extern TEXTMETRIC tm ;
extern int cxChar, cxCaps, cyChar ;

// Pens and Brushes we need to show selections.
extern CBrush brSelRect ;
extern CBrush brUnSelRect ;
extern CPen   penUnSelRect ;
extern CPen   penSelRect ;
extern CBitmap bmapUnCheck, bmapCheck, bmapTriangle, bmapNone , bmapTestCase;								 

IMPLEMENT_DYNCREATE(CAPIPage, CPropertyPage)

CAPIPage::CAPIPage()
	: CPropertyPage(CAPIPage::IDD)
{
	//{{AFX_DATA_INIT(CAPIPage)
	m_iDisplayClassNames = FALSE;
	m_iDisplayReturnType = FALSE;
	m_iDisplayParams = TRUE;
	//}}AFX_DATA_INIT
}

CAPIPage::~CAPIPage()
{
}

void CAPIPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAPIPage)
	DDX_Control(pDX, IDC_API_CALL, m_ApiCallCtrl);
	DDX_Control(pDX, IDC_DESCRIPTION, m_apiDescription);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_parameterList);
	DDX_Control(pDX, IDC_LIST_API, m_apiList);
	DDX_Control(pDX, IDC_CATEGORY, m_categoryCB);
	DDX_CBString(pDX, IDC_CATEGORY, m_category);  
	DDX_Check(pDX, IDC_DISPLAY_CLASSNAMES, m_iDisplayClassNames);
	DDX_Check(pDX, IDC_DISPLAY_RETURN_TYPE, m_iDisplayReturnType);
	DDX_Check(pDX, IDC_DISPLAY_PARAMS, m_iDisplayParams);
	//}}AFX_DATA_MAP
}



BEGIN_MESSAGE_MAP(CAPIPage, CPropertyPage)

	//{{AFX_MSG_MAP(CAPIPage)
	ON_CBN_SELCHANGE(IDC_CATEGORY, OnSelchangeCategory)
	ON_LBN_DBLCLK(IDC_LIST_AREAS, OnDblclkListAreas)
	ON_LBN_SELCHANGE(IDC_LIST_AREAS, OnSelchangeListAreas)
	ON_LBN_SELCHANGE(IDC_LIST_API, OnSelchangeListApi)
	ON_LBN_DBLCLK(IDC_LIST_API, OnDblclkListApi)
	ON_LBN_SETFOCUS(IDC_LIST_API, OnSetfocusListApi)
	ON_LBN_KILLFOCUS(IDC_LIST_API, OnKillfocusListApi)
	ON_LBN_SELCHANGE(IDC_LIST_PARAMETERS, OnSelchangeListParameters)
	ON_BN_CLICKED(IDC_DISPLAY_CLASSNAMES, OnDisplayClassnames)
	ON_BN_CLICKED(IDC_DISPLAY_RETURN_TYPE, OnDisplayReturnType)
	ON_BN_CLICKED(IDC_DISPLAY_PARAMS, OnDisplayParams)
	ON_BN_CLICKED(IDC_ADD_API, OnAddApi)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAPIPage operations

void CAPIPage::DisplayAreas()
{
    m_areaList.ResetContent();	// Clear Area List box
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

UINT CAPIPage::OpenAPIData(LPCSTR szFileName, UINT nFileType /*= API_FT_TEXT */)
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

BOOL CAPIPage::InterpretLine(LPCSTR szNewLine)
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

char* CAPIPage::RemoveExtraChars(char* pchLine) 	  // REVIEW (EnriqueP): Move this code to CFileTableLine::CFileTableLine(pchStr)
{
	int iCRLF = strcspn(pchLine, "\'\r\n");	 // remove trailing comments , CR or LF
	pchLine[iCRLF] = '\0';
	pchLine += strspn(pchLine, "\t /");	// Remove leading spaces, tabs or '//' comments
	return pchLine;
}


void CAPIPage::FillStructures( LPCSTR szLine )
{
    ASSERT(szLine[0] != '\0');


	while( szLine[0] != '\0' )
	{
		int nSeparator = strcspn(szLine, ": \t");
        CString strLineType =  CString(szLine, nSeparator);	// Get first word
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

LPCSTR CAPIPage::SetCurrentCategory(LPCSTR szLine)	//REVIEW: Change to FillCategoryStruct()
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
	
	index = FindArea();  
    m_areaArray[index]->m_strCategory = m_strCurrentCategory;   
    
	szLine = szLine + nSeparator;  // REVIEW: Is this needed?

    return (szLine);
}


LPCSTR CAPIPage::FillAreaStruct(LPCSTR szLine)
{
	//DefaultAreaCtor();  
    ASSERT(szLine[0] != '\0');
	int nSeparator = strcspn(szLine, "\0 ");
	m_strCurrentArea = CString(szLine,nSeparator);

    // REVIEW: Search to see if the area has already been entered
    CArea* pArea = new CArea;
	pArea->m_strCategory = m_strCurrentCategory;
    pArea->m_strName = m_strCurrentArea;   //REVIEW: Possible bug
    m_areaArray.Add(pArea);
    
    szLine = szLine + nSeparator;  // REVIEW: Is this needed?

    return (szLine);
	
}

LPCSTR CAPIPage::FillClassStruct(LPCSTR szLine)
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

LPCSTR CAPIPage::FillBaseClassStruct(LPCSTR szLine)
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



LPCSTR CAPIPage::FillSubareaStruct(LPCSTR szLine)
{
	//DefaultAreaCtor();  
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

LPCSTR CAPIPage::FillAPIStruct(LPCSTR szLine)
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
    
    return (szLine);
}


LPCSTR CAPIPage::FillDescStruct(LPCSTR szLine)
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


LPCSTR CAPIPage::FillReturnStruct(LPCSTR szLine)
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



LPCSTR CAPIPage::FillParamStruct(LPCSTR szLine, CAPI* pAPI)
{
    ASSERT(szLine[0] != '\0');
	
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

LPCSTR CAPIPage::FillParamDescStruct(LPCSTR szLine)
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


int CAPIPage::FindArea(LPCSTR szArea)
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

BOOL CAPIPage::IsArea(int index)
{
	CBitmap * curpbmap ;
	curpbmap = CBitmap::FromHandle((HBITMAP)m_areaList.GetItemData(index)) ;
	if (curpbmap->GetSafeHandle() != bmapTriangle.GetSafeHandle() )
		return TRUE;
	else
		return FALSE;
}

void CAPIPage::ExpandArea(int index, BOOL bExpand)
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

void CAPIPage::DisplaySubareas( int index, int i )
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

void CAPIPage::HideSubareas( int index, int i )
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

void CAPIPage::DisplayAreaAPIs( int index )
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
			strNew.Empty();		// Reset string
			strClass = 	m_apiArray[i]->m_strClass;
			if (!m_apiArray[i]->m_strClass.IsEmpty()) // Add "::" if it has a class
				strClass += "::";
			// Verify display flags
			if ( m_iDisplayReturnType )
			{
				strNew = m_apiArray[i]->m_strReturnVal+ ' ';
			}
			if ( m_iDisplayClassNames )
			{
				strNew += " " + strClass;
			}
			
			strNew += m_apiArray[i]->m_strName;

			if (m_iDisplayParams)
			{
				
				strNew += '(';
				POSITION pos = m_apiArray[i]->m_paramList.GetHeadPosition();
				int nParam = 0;
				while (pos != NULL )
				{  	
					CParam* pParam =  m_apiArray[i]->m_paramList.GetNext(pos);
					if(nParam++ == 0)
						strNew += ' ' + pParam->m_strName;
					else
						strNew += ", " + pParam->m_strName;
				}
				strNew += " )";
			}
			else
			{
				strNew += "()"; // 
			}

			index = m_apiList.AddString( strNew );	
			m_apiList.SetItemData( index, (DWORD)i );
		}
	}
}
			
void CAPIPage::DisplaySubareaAPIs( int index )
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
			
CString CAPIPage::GetAreaName( int index )
{
	CString	strArea;
	while ( !IsArea(index) )
		index--;	  // Go up the list to find the Area
	ASSERT (index >= 0);
	m_areaList.GetText(index, strArea);
	
	return strArea;
}

void CAPIPage::DisplayParameters( int index, BOOL bAddedToScript )
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

void CAPIPage::DisplayAPIDescription( int index )
{
	
	int i = m_apiList.GetItemData(index);
	CString strDesc = m_apiArray[i]->m_strDesc;
	strDesc += "\n\n Return Type Info: " + m_apiArray[i]->m_strReturnDesc;
	m_apiDescription.SetWindowText( strDesc );
}

void CAPIPage::DisplayParamDescription( int index )
{
	
	CParam* pParam = (CParam*)m_parameterList.GetItemDataPtr(index);
	CString strDesc = pParam->m_strDesc;
	m_apiDescription.SetWindowText( strDesc );
}



CString CAPIPage::CreateVarFromClassName(CString& strClassName)
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
// CAPIPage message handlers


void CAPIPage::OnSelchangeCategory() 
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


BOOL CAPIPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

   	CString path("0",250) ;
	GetModuleFileName(NULL,path.GetBuffer(250),250) ;
	path = path.Left(path.ReverseFind('\\') + 1) ;
	path =path +"appwiz\\apidata.txt" ;

	m_bIsOldFormat = FALSE;


    if (m_apiArray.GetSize() == 0)
        OpenAPIData(path.GetBuffer(250));

	CClientDC ccldc(this) ;         
 	// Add Custom listbox
	// subclass the control	 

	VERIFY(m_areaList.SubclassDlgItem(IDC_LIST_AREAS, this));

	// Set column sizes for Parameter Listbox
	CRect rect;
	m_parameterList.GetClientRect(rect);
	LPRECT pRectParam = rect;
	int nTabStop = (pRectParam->right - pRectParam->left) / 5;
	m_parameterList.SetTabStops( nTabStop  );
	// Display Api's
	
	m_categoryCB.SetCurSel(0);	// Select first category
	OnSelchangeCategory();	
	DisplayAreas();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAPIPage::OnDblclkListAreas() 
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


void CAPIPage::OnSelchangeListAreas() 
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

void CAPIPage::OnSelchangeListApi() 
{
    int index = m_apiList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbo.
		return ;  

	DisplayParameters( index );
	DisplayAPIDescription( index );
}

void CAPIPage::OnDblclkListApi()	// Write API Call to Edit box
{
	int index = m_apiList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbox.
		return ;  
	int i = m_apiList.GetItemData(index);

	CString strAPICall = m_apiArray[i]->m_strName + '(';
	
	POSITION pos = m_apiArray[i]->m_paramList.GetHeadPosition();
	int nParam = 0;
	while (pos != NULL )
	{  	
		CParam* pParam =  m_apiArray[i]->m_paramList.GetNext(pos);
		if(pParam->m_strValue.IsEmpty())
		{
			if(nParam == 0)
				strAPICall += pParam->m_strName;
			else
				strAPICall += ", " + pParam->m_strName;
		}
		else
		{
			if(nParam == 0)
				strAPICall += pParam->m_strValue;
			else
				strAPICall += ", " + pParam->m_strValue;
		}
		nParam++;
	}
	strAPICall += ");";

	m_ApiCallCtrl.SetWindowText(strAPICall.GetBuffer( strAPICall.GetLength() ));
	strAPICall.ReleaseBuffer();		
}



void CAPIPage::OnOK()
{

}


void CAPIPage::OnSetfocusListApi() 
{
	// Enable Add button only when this listbox has focus
	
}

void CAPIPage::OnKillfocusListApi() 
{
	
}


void CAPIPage::OnSelchangeListParameters() 
{
	int index = m_parameterList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbo.
		return ;  

	DisplayParamDescription( index );
}


void CAPIPage::OnDisplayClassnames() 
{
	m_iDisplayClassNames = !m_iDisplayClassNames;
	OnSelchangeListAreas();
}

void CAPIPage::OnDisplayReturnType() 
{
	m_iDisplayReturnType = !m_iDisplayReturnType;
	OnSelchangeListAreas();
}

void CAPIPage::OnDisplayParams() 
{
	m_iDisplayParams = !m_iDisplayParams;
	OnSelchangeListAreas();
}

void CAPIPage::OnAddApi() 
{
	int index = m_apiList.GetCurSel();
	if (LB_ERR ==index || 0 > index) // If an error or no items in the listbox.
		return ;  
	int i = m_apiList.GetItemData(index);

	CString strAPICall = m_apiArray[i]->m_strName + '(';
	
	POSITION pos = m_apiArray[i]->m_paramList.GetHeadPosition();
	int nParam = 0;
	while (pos != NULL )
	{  	
		CParam* pParam =  m_apiArray[i]->m_paramList.GetNext(pos);
		if(pParam->m_strValue.IsEmpty())
		{
			if(nParam == 0)
				strAPICall += pParam->m_strName;
			else
				strAPICall += ", " + pParam->m_strName;
		}
		else
		{
			if(nParam == 0)
				strAPICall += pParam->m_strValue;
			else
				strAPICall += ", " + pParam->m_strValue;
		}
		nParam++;
	}
	strAPICall += ");";

	m_ApiCallCtrl.SetWindowText(strAPICall.GetBuffer( strAPICall.GetLength() ));
	strAPICall.ReleaseBuffer();		
}
