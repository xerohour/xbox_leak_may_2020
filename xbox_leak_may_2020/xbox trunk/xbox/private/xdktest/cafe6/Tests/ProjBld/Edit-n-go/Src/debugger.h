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
#define EXPECT_FALSE(f)   ((f)?LogResult(FALSE,#f):LogResult(TRUE,#f));
#define EXPECT_SUCCESS(f) (((ReturnCode=(f))==0)?LogResult(TRUE,#f):LogResult(FALSE,#f,ReturnCode));
#define EXPECT_VALIDBP(f) (((ReturnBP=(f))!=NULL)?LogResult(TRUE,#f):LogResult(FALSE,#f));


#if defined(LEGO) // Lego meeds minimal case only comment out logging.
	#define WRITELOG(bool, text)	// 
	#define VCOMMENT(text,val)		// 
	#define COMMENT(text)			// 
#else
	// VCOMMENT is a comment with a single value
	#define VCOMMENT(text,val)	m_pLog->Comment(text,val);
	#define COMMENT(text)		m_pLog->Comment(text);
	// General use Writelog Macro
	#define Writelog(bool,text) if(bool) m_pLog->RecordInfo(text); else m_pLog->RecordFailure(text);			
	// Returns on Failure used in Editgo
	#define WRITELOG(bool,text) if(bool) m_pLog->RecordInfo(text); else {m_pLog->RecordFailure(text); return FALSE;}			
#endif  // LEGO


#endif //__DEBUGGER_H__
