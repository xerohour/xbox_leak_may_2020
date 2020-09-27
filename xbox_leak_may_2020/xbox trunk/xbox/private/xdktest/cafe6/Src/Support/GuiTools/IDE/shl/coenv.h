///////////////////////////////////////////////////////////////////////////////
//  COENV.H
//
//  Created by :            Date :
//      MichMa                  11/17/95
//
//  Description :
//      Declaration of the COEnvironment class
//

#ifndef __COENV_H__
#define __COENV_H__

#include "uioptdlg.h"
#include "shlxprt.h"

///////////////////////////////////////////////////////////////////////////////
//  COEnvironment class

enum DIR_TYPE {DIR_TYPE_CURRENT, DIR_TYPE_EXE, DIR_TYPE_INC, DIR_TYPE_LIB};

// BEGIN_CLASS_HELP
// ClassName: COEnvironment
// BaseClass: none
// Category: Shell
// END_CLASS_HELP
class SHL_CLASS COEnvironment

	{
	// Data
	private:
		UIOptionsTabDlg m_optdlg;

	// Private Utilites:
	private:
		int DirectoryCore(DIR_TYPE dtDirType, int intPlatforms);

	// Public Utilities
	public:
		int PrependDirectory(LPCSTR szNewDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT);
		int PrependDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT); 
		int RemoveDirectory(LPCSTR szDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT);
		int RemoveDirectory(int iIndexDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT);
		int RemoveDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT); 
	};

#endif // __COENV_H__
