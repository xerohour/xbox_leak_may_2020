///////////////////////////////////////////////////////////////////////////////
//	Debugger.H
//
//	Created by :			Date :
//		Waynebr					2/18/94
//
//	Description :
//		Debugger tests helpers
//

#ifndef __DEBUGGER_H__
#define __DEBUGGER_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
//	Debugging helpers

extern int ReturnCode;
extern bp  * ReturnBP;
extern void LogResult(int result,LPCSTR szoperation, int code=0);
extern LPCSTR HomeDir(void);
extern LPCSTR FullPath(LPCSTR szAddPath);

#define EXPECT_TRUE(f)    ((f)?LogResult(TRUE,#f):LogResult(FALSE,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(TRUE,#f):LogResult(FALSE,#f,ReturnCode));
#define EXPECT_VALIDBP(f) (((ReturnBP=(f))!=NULL)?LogResult(TRUE,#f):LogResult(FALSE,#f));


#endif //__DEBUGGER_H__
