#include "stdafx.h"
#include <atlbase.h>
#include <initguid.h>
#include <ObjModel/bldguid.h>
#include <ObjModel/bldauto.h>
#include <utilauto.h>
#include "keywords.h"
#include <htmlhelp.h>

BOOL DoXboxHelp(LPCTSTR lpszKeyword)
{
	CComBSTR  bstrTemp;
	CString   strProjName;
	CString   strConfigName;
	BOOL      bXboxProj = FALSE;
	BOOL      bLaunchMSDN = FALSE;
	CString   strKeyword = lpszKeyword;
	IDispatch *iDisp = NULL;

	if( !theApp.m_pAutoApp ) return FALSE;

	try
	{
		// this call will throw a custom dev studio 
		// exception if there is no project loaded
		// and a tacky messagebox will be displayed
		// if I don't handle it
		iDisp = theApp.m_pAutoApp->GetActiveProject();
	}
	catch( CException* e )
	{
		// suppress the exception and messagebox
		// then let MSDN handle this F1 event
	    e->Delete();
	    return FALSE;
	}

	if( !iDisp ) return FALSE;
	IGenericProject *pActiveProj;
	iDisp->QueryInterface(IID_IGenericProject, (LPVOID FAR *)&pActiveProj);
	if(!pActiveProj) return FALSE;
	pActiveProj->get_Name(&bstrTemp);
	strProjName = bstrTemp;
	SysFreeString(bstrTemp);
//MessageBox(NULL,"ProjName",strProjName,MB_OK);
	pActiveProj->Release();
	iDisp->Release();

	// Get active configuration's name
	iDisp = theApp.m_pAutoApp->GetActiveConfiguration();
	IConfiguration *pActiveConfig; 
	iDisp->QueryInterface(IID_IConfiguration, (LPVOID FAR *)&pActiveConfig);
	if(!pActiveConfig) return FALSE;
	pActiveConfig->get_Name(&bstrTemp);
	strConfigName = bstrTemp;
	SysFreeString(bstrTemp);
//MessageBox(NULL,"ConfigName",strConfigName,MB_OK);
	pActiveConfig->Release();
	iDisp->Release();

	// Is this an Xbox project?
	CString strTestForXbox = strProjName + L" - Xbox";
	if(!strConfigName.Left(strTestForXbox.GetLength()).Compare(strTestForXbox))
	{
		// The final F1 help solution might use the project type as 
		// an index into an INI file. Each section describes what 
		// actions to take based on the project type. 
		bXboxProj = TRUE;
	}

	// Launch help
	if(bXboxProj)
	{
		strKeyword.TrimLeft();
		strKeyword.TrimRight();

		// Get path to Xbox help
		HKEY  hkXDK = NULL;
		char  szPathBuf[MAX_PATH];
		DWORD dwSize = sizeof(szPathBuf);

		memset(szPathBuf,0,sizeof(szPathBuf));
		
		RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Microsoft\\XboxSDK",NULL,KEY_QUERY_VALUE,&hkXDK);
		RegQueryValueEx(hkXDK,"InstallPath",NULL,NULL,(LPBYTE)szPathBuf,&dwSize);
		RegCloseKey(hkXDK);

		CString strChmPath = szPathBuf;

		// Did we get a valid path?
		if(strChmPath.IsEmpty())
		{
			// TODO: Needs to be a real dialog that has a "Don't show this in the future" checkbox.
			theApp.EnableModeless(VARIANT_FALSE);
			::MessageBox(NULL, "Cannot locate \"XboxSDK.chm\". The Xbox SDK may not be properly installed.", "Cannot locate \"XboxSDK.chm\"" , MB_OK | MB_ICONEXCLAMATION);
			theApp.EnableModeless(VARIANT_TRUE);
			// TODO: Display "launching default chm" in the msgbox above - and launch the default MSDN CHM here.
			// Launch MSDN Help
			bLaunchMSDN = TRUE;
		}
		else
		{
			strChmPath += L"\\Doc";
			CString strKwdFile = strChmPath;
			CString strChmFile = strChmPath;

			strKwdFile += "\\XboxSDK.kwd";
			strChmFile += "\\XboxSDK.chm";

			// does the chm even exist?
			if( -1 == GetFileAttributes(strChmFile) )
			{
				// TODO: Needs to be a real dialog that has a "Don't show this in the future" checkbox.
				theApp.EnableModeless(VARIANT_FALSE);
				::MessageBox(NULL, "Cannot locate \"XboxSDK.chm\". The Xbox SDK may not be properly installed.", "Cannot locate \"XboxSDK.chm\"" , MB_OK | MB_ICONEXCLAMATION);
				theApp.EnableModeless(VARIANT_TRUE);
				// TODO: Display "launching default chm" in the msgbox above - and launch the default MSDN CHM here.
				// Launch MSDN Help
				bLaunchMSDN = TRUE;
			}
			else
			{
				// Inefficient, to create this object each time, 
				// but I want this addin to be stateless.
				CKeywords kwdXbox(strKwdFile);
				
				if(kwdXbox.IsKeyword(strKeyword))
				{
					// Launch Xbox Help
					HH_AKLINK link;
	
					link.cbStruct =     sizeof(HH_AKLINK) ;
					link.fReserved =    FALSE ;
					link.pszKeywords =  strKeyword ; 
					link.pszUrl =       NULL ; 
					link.pszMsgText =   NULL ; 
					link.pszMsgTitle =  NULL ; 
					link.pszWindow =    NULL ;
					link.fIndexOnFail = TRUE ;
	
					// This will open the TOC to the entry found. 
					// I killed it since I want to behave just like VC's F1 help.
					// HtmlHelp(GetDesktopWindow(),szChmPath,HH_DISPLAY_TOC,NULL);
	
					HtmlHelp(GetDesktopWindow(),strChmFile,HH_KEYWORD_LOOKUP,(DWORD)&link);
				}
				else
				{
					// Launch MSDN Help
					bLaunchMSDN = TRUE;
				}
			}
		}
	}
	else
	{
		// Launch MSDN Help
		bLaunchMSDN = TRUE;
	}

	// Launch MSDN if we weren't able to handle the keyword.
	// I reverse the boolean so it looks cleaner for the caller.
	// TRUE means we handled it, FALSE means MSND should take a stab at it.
	return bLaunchMSDN ? FALSE : TRUE;
}
