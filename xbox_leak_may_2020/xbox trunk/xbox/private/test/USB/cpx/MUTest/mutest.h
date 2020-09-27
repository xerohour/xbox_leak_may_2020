/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    MUTest.h

Abstract:

     header definitions for MUTest.cpp 

Author:

    Dennis Krueger <a-denkru> November 2001

--*/
#ifndef __mutest_h_
#define __mutest_h_

//Global variables

#include "TextScreen.h"


class CXBoxUtility
{
public:
    
	CXBoxUtility() {;};
	~CXBoxUtility() {;};
    void				Init();
    void				Render();
    void				FrameMove();
	void				CheckMus();
	void				Run();
	void				UpdateScreen();


	CTextScreenArray	m_ThisScreen;
	BOOL				m_fMUStatus[20];
	BOOL				m_fRemove;


};




#define MAX_SLOTS XGetPortCount()


typedef enum {notyet, testme, pending, failed, passed} BOXSTATUS;
typedef BOXSTATUS (*BOXFUNC)(int port, int slot);
extern BOXSTATUS gPadStatus[MAX_SLOTS];



extern BOXFUNC b[];
//extern deGAMEPAD * gData;

#ifdef __cplusplus
extern "C" {
#endif
int DebugPrint(char* format, ...);
#ifdef __cplusplus
}
#endif

	BOXSTATUS LowLevelTest(int port, int slot, int &BB1, int &BB2, int &BB3);
    BOOL FormatMU(int port, int slot);
	BOXSTATUS VerifyTest(int port, int slot);
	BOXSTATUS WriteReadTest(CHAR drive,int &readfailed, int &writefailed, int &comparefailed);
	BOXSTATUS CountSectorWriteTest(CHAR drive);

#endif