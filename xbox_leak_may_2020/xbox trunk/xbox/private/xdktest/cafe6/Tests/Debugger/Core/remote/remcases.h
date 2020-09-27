///////////////////////////////////////////////////////////////////////////////
//	REMCASES.H
//
//	Created by :			Date :
//		MichMa					8/24/94
//
//	Description :
//		Declaration of the CRemoteDebuggingCases class
//

#ifndef __REMCASES_H__
#define __REMCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "dbgset.h"

///////////////////////////////////////////////////////////////////////////////
//	CRemoteDebuggingCases class

class CRemoteDebuggingCases : public CDebugTestSet
	{
		DECLARE_TEST(CRemoteDebuggingCases, CCORESubSuite)

	// Operations (test cases)
	public:
		
		virtual void PreRun(void);
		virtual void Run(void);
		
		void GeneralCases(PLATFORM_TYPE platform, CONNECTION_TYPE connection);
		void TCPIPCases(PLATFORM_TYPE platform);
		void Win32SerialCases(PLATFORM_TYPE platform);
		void Mac68kSerialCases(void);
		void MacPPCSerialCases(void);
		void AppleTalkCases(PLATFORM_TYPE platform);

	// Data
	private:

		CODebug dbg;
		COProject prj;
		COConnection *pconnec;

		BOOL fGeneralCasesDone;		
	};

#endif //__REMCASES_H__
