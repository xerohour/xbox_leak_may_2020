///////////////////////////////////////////////////////////////////////////////
//	Debugger.cpp
//
//	Created by :			Date :
//		Waynebr					2/18/94
//
//	Description :
//		Debugger tests helpers
//

#include "stdafx.h"

#define new DEBUG_NEW

///////////////////////////////////////////////////////////////////////////////
//	Debugging helpers

int ReturnCode =0;
bp  * ReturnBP =0;

void LogResult(int result,LPCSTR szoperation, int code=0) {
	CString szextra;
	//if (code==0)  szextra="Error Code="+itoa(code);
	if (!result) {
		LOG->RecordFailure(szextra + szoperation);
	}
	else {
		LOG->RecordInfo(szextra + szoperation);
	}
};

//
//	Returns the current directory of the first time this function was called.
//
LPCSTR HomeDir()
{
	static CString szCurDir;

	if( szCurDir.IsEmpty() )
	{
		char* psz = szCurDir.GetBufferSetLength(_MAX_DIR);

		GetCurrentDirectory(_MAX_DIR - 1, psz);
		szCurDir.ReleaseBuffer(-1);
		if( szCurDir.Right(1) != '\\' )
			szCurDir += "\\";
	}
 	return szCurDir;
 }

//
//	Returns the fully qualified path with szAddPath appended to the sniff dir.	
//
 LPCSTR FullPath( LPCSTR szAddPath )
 {
	static CString szFilespec;

	szFilespec = HomeDir();
	szFilespec += szAddPath;

	return szFilespec;
 }
