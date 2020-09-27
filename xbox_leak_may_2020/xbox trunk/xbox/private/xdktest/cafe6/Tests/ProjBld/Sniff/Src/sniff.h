///////////////////////////////////////////////////////////////////////////////
//	SNIFF.H
//
//	Created by :			Date :
//		TomWh					10/18/93
//
//	Description :
//		Declaration of the CSniffDriver class
//

#ifndef __SNIFF_H__
#define __SNIFF_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

///////////////////////////////////////////////////////////////////////////////
// CSniffDriver class

class CSniffDriver : public CIDESubSuite
{
	DECLARE_SUBSUITE(CSniffDriver)

	DECLARE_TESTLIST()

// overrides
public:
	virtual void SetUp(BOOL bCleanUp);
	virtual void CleanUp(void);
private:
	void DeleteFileByWildCard(CString strWildCard);
	void CleanUpVSSDroppings();
};

// Global variables
#define WSPEXTENSION	".DSW"
#define PRJEXTENSION	".DSP"
#define NEWEXEINTDIR	"EXEInt"
#define NEWEXEOUTDIR	"EXEOut"
#define BUILDRES		"buildres.out"
#define PROJNAME		"Hello"
#define PROJMAKFILE		"Hello.DSP"
#define PLATFORMS		PLATFORM_WIN32_X86 + PLATFORM_MAC_68K + PLATFORM_MAC_PPC
// #define EXERELEASETARGET	"Hello - Win32 Release"
#define V4PROJDIR		"Hello.V4"

#if defined(_MIPS_)
#define TOOLSDIR "tools\\mips"
#elif defined(_ALPHA_)
#define TOOLSDIR "tools\\alpha"
#else
#define TOOLSDIR "tools\\x86"
#endif


#endif //__SNIFF_H__
