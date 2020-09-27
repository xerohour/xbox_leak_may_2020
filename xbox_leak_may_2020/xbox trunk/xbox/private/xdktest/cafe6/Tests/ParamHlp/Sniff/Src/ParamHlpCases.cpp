///////////////////////////////////////////////////////////////////////////////
//      ParamHlpCases.cpp
//
//      Description :
//              Implementation of CParamHlpCases

#include "stdafx.h"
#include "ParamHlpCases.h"
#include "TextFile.h"

#define new DEBUG_NEW

#define MTT_TYPEPREFIX 0
#define MTT_TYPEPOSTFIX 1
#define MTT_NAME 2
#define MTT_DESCRIPTION 3

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CParamHlpCases, CTest, "ParamHlp", -1, CParamHlpSuite)

void CParamHlpCases::Run(void)
{
	EditSource();
}

void CParamHlpCases::EditSource()
{
	int		nCount = 0;
	int		nResult;
	LRESULT lLength;
	HWND	hWnd;
	TCHAR*	pText;
	CString strMethod = "malloc";		// Need to put this in a string resource

	m_pLog->Comment("Open a source file.");

	// Open a source file
	nResult = src.Open("ParamHlp01.cpp");

	if (nResult == ERROR_ERROR) {
		m_pLog->RecordFailure("Couldn't open source file ParamHlp01.cpp");
	}
	else {
		// Move down the file to a place where 
		// we can enter some Parameter Help code.
		src.GoToLine(53);
		
		// Wait for Go To dialog to go away
		while ((MST.WFndWnd("Go To", FW_DEFAULT)) && (nCount < 1000)) {
			nCount++;
		}

		MST.DoKeys("{ENTER 2}");
		MST.DoKeys("{UP 2}");

		// while loop through the list of APIs
		CTextFile fileIn;
		CTextFile fileOut;
		
		if (!fileIn.Open("baseln\\Win32API.txt", CFile::modeRead)) {
			m_pLog->RecordFailure("Couldn't open Win32API.txt");
		}
		else {
		
			// Open file to write failures
			fileOut.Open("failures.txt", CFile::modeCreate | CFile::modeWrite);

			while (fileIn.ReadLn(strMethod )) {
				MST.DoKeys(strMethod);
				MST.DoKeys("{(}");

				hWnd = MST.WFndWndWaitC("", "VsTipWindow", FW_DEFAULT, 1);

				if (hWnd == 0) {
					m_pLog->RecordFailure("Couldn't find Parameter Help window. Function: %s", strMethod);
					fileOut.WriteLn(strMethod);
				}
				else {
					if (MST.WIsVisible(hWnd)) {
						// Get the method name
						SendMessage(hWnd, WM_SETTITLETOMETHOD, MTT_NAME, 0);
						lLength = GetWindowTextLength(hWnd) + sizeof(TCHAR);
						pText = new TCHAR[lLength];
						GetWindowText(hWnd, pText, lLength);
				
						if (strcmp(pText, strMethod) != 0) {
							m_pLog->RecordFailure("Parameter Help does not match expectation.\n\t\tParameter Help = '%s'\n\t\tExpected '%s'", pText, strMethod);
							fileOut.WriteLn(strMethod);
						}

						delete [] pText;
					}
					else {
						m_pLog->RecordFailure("Parameter Help window is not visible. Function: %s", strMethod);
						fileOut.WriteLn(strMethod);
					}
				}

				MST.DoKeys("{)};");
				MST.DoKeys("{ENTER}");
			}
			
			fileIn.Close();
			fileOut.Close();
		}

		src.Close();
	}
}
