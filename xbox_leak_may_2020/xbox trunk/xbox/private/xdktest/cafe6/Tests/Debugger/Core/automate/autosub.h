///////////////////////////////////////////////////////////////////////////////
//	autosub.h
//
//	Created by : MichMa		Date: 9-17-96			
//		VCBU QA
//
//	Description :
//		Declaration of the CAutomationSubSuite class
//

#ifndef __AUTOSUB_H__
#define __AUTOSUB_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CAutomationSubSuite class

class CAutomationSubSuite : public CIDESubSuite
	{
	public:
		DECLARE_SUBSUITE(CAutomationSubSuite)
		DECLARE_TESTLIST()
	public:
		BOOL fProjectReady;
		void CleanUp(void);
		void LoadMacroFile(void);
		void RunMacro(LPCSTR macro);
		void WriteOutputWindowToLog(void);
		void CloseWorkspaceAndCreateOptionsFile(void);
	};

#endif //__AUTOSUB_H__
