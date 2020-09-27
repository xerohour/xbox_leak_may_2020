//////////////////////////////////////////////////////////////////////////////
//  COENV.CPP
//
//  Created by :            Date :
//      MichMa                  11/17/95
//
//  Description :
//      Implementation of the COEnvironment class
//

#include "stdafx.h"
#include "coenv.h"
#include "testxcpt.h"
#include "..\..\testutil.h"
#include "guiv1.h"
#include "mstwrap.h"
#include "uiwbmsg.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;


// BEGIN_HELP_COMMENT
// Function: int COEnvironment::PrependDirectory(LPCSTR szNewDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT)
// Description: Prepend a directory to one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szNewDir A pointer to a string containing the directory to prepend.
// Param: dtDirType The environment variable to set: DIR_TYPE_EXE, DIR_TYPE_INC, or DIR_TYPE_LIB.
// Param: intPlatforms The platform(s) to set the environment variable for (one or more PLATFORM_TYPE values).
// END_HELP_COMMENT
int COEnvironment::PrependDirectory(LPCSTR szNewDir, DIR_TYPE dtDirType /* DIR_TYPE_CURRENT */, 
									int intPlatforms /* PLATFORM_CURRENT*/)

	{
	const char* const THIS_FUNCTION = "COEnvironment::PrependDirectory()";

	// make sure a directory name was specified.
	if((szNewDir == NULL) || (szNewDir[0] == 0))
		
		{
		LOG->RecordInfo("%s: no directory specified for platform %d in dir type %d (was NULL or empty).",
						THIS_FUNCTION, intPlatforms, dtDirType);

		return ERROR_ERROR;
		}

	// bring up options dlg, select directories page, and set platform and dir type if specified.
	if(DirectoryCore(dtDirType, intPlatforms) != ERROR_SUCCESS)
		return ERROR_ERROR;

	// since the directories listbox isn't really a listbox, we can't use the MST.WList routines. so we
	// must start from the top and manually count the items in the list. then we add the new item to the
	// end of the list and move it up to the top.
	int intDirCount = 0;
	CString cstrCurrentDir;
	UIWBMessageBox mbAlert;
	
	while(1)
		
		{
		// edit the current directory and get the text.
		MST.DoKeys("^c");
		MST.WEditText("", cstrCurrentDir);

		// if the current directory is empty, we've reached the end of the list.
		if(cstrCurrentDir == "")
			
			{
			// type in the new directory name and commit the edit.
			MST.DoKeys(szNewDir);
			MST.DoKeys("{ENTER}");

			// flag error if we get a msg box.
			if(mbAlert.WaitAttachActive(1000))
				
				{
				LOG->RecordInfo("%s: could not prepend dir %s for platform %d in dir type %d. Check if "
								"directory didn't exist or was a duplicate.", 
								THIS_FUNCTION, szNewDir, intPlatforms, dtDirType);
				
				// take default action (which results in no directory being added).
				MST.DoKeys("{ENTER}");
				// cancel the options dlg.
				m_optdlg.Cancel();
				return ERROR_ERROR;
				}

			break;
			}

		// get out of edit mode and advance to the next directory.
 		MST.DoKeys("{ESC}");
		MST.DoKeys("{DOWN}");
		// update the number of directories we have found in the list.
		intDirCount++;
		}

	// after commiting the new directory, the selection advances to the last (empty) item.
	// we need to select the item we just entered before moving it to the top.
	MST.DoKeys("{UP}");

	// move the new directory to the top of the list.
	for(int i = 0; i < intDirCount; i++)
		MST.DoKeys("%{UP}");

	// ok the options dlg.
	m_optdlg.OK();
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COEnvironment::PrependDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT)
// Description: Prepend multiple directories to one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: CStringArray &cstrArrayDirs A reference to a CStringArray containing the list of directories to prepend.
// Param: dtDirType The environment variable to set: DIR_TYPE_EXE, DIR_TYPE_INC, or DIR_TYPE_LIB.
// Param: intPlatforms The platform(s) to set the environment variable for (one or more PLATFORM_TYPE values).
// END_HELP_COMMENT
int COEnvironment::PrependDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType /* DIR_TYPE_CURRENT */,
									 int intPlatforms /* PLATFORM_CURRENT*/)

	{
	const char* const THIS_FUNCTION = "COEnvironment::PrependDirectories()";

	// bring up options dlg, select directories page, and set platform and dir type if specified.
	if(DirectoryCore(dtDirType, intPlatforms) != ERROR_SUCCESS)
		return ERROR_ERROR;

	// make sure the user specified directories to remove.
	if(cstrArrayDirs.GetSize() == 0)
		
		{
		LOG->RecordInfo("%s: no dirs specified for platform %d in dir type %d.", 
						THIS_FUNCTION, intPlatforms, dtDirType);

		return ERROR_ERROR;
		}

	// since the directories listbox isn't really a listbox, we can't use the MST.WList routines. so we
	// must start from the top and manually count the items in the list. then we add the new item to the
	// end of the list and move it up to the top.
	int intDirCount = 0, i;
	CString cstrCurrentDir;
	UIWBMessageBox mbAlert;
	
	while(1)
		
		{
		// edit the current directory and get the text.
		MST.DoKeys("^c");
		MST.WEditText("", cstrCurrentDir);

		// if the current directory is empty, we've reached the end of the list.
		if(cstrCurrentDir == "")
			
			{
			// loop through each of the specified directories.
			for(i = 0; i < cstrArrayDirs.GetSize(); i++)

				{
				// type in the new directory name and commit the edit.
				MST.DoKeys(cstrArrayDirs[i]);
				MST.DoKeys("{ENTER}");

				// flag error if we get a msg box.
				if(mbAlert.WaitAttachActive(1000))
					
					{
					LOG->RecordInfo("%s: could not prepend dir %s for platform %d in dir type %d. Check if "
									"directory didn't exist or was a duplicate.", 
									THIS_FUNCTION, cstrArrayDirs[i], intPlatforms, dtDirType);
					
					// take default action (which results in no directory being added).
					MST.DoKeys("{ENTER}");
					// cancel the options dlg.
					m_optdlg.Cancel();
					return ERROR_ERROR;
					}
				}

			break;
			}

		// get out of edit mode and advance to the next directory.
 		MST.DoKeys("{ESC}");
		MST.DoKeys("{DOWN}");
		// update the number of directories we have found in the list.
		intDirCount++;
		}

	// starting with the last one entered, move each new directory to the top in the specified order.
	for(i = 0; i < cstrArrayDirs.GetSize(); i++)

		{
		// select the last dir in the list.
		MST.DoKeys("{END}{UP}");

		// move the directory to the top.
		for(int j = 0; j < (intDirCount + cstrArrayDirs.GetSize() - 1); j++) 
			MST.DoKeys("%{UP}");
		}

	// ok the options dlg.
	m_optdlg.OK();
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COEnvironment::RemoveDirectory(LPCSTR szDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT)
// Description: Remove a directory from one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: szDir A pointer to a string containing the directory to remove.
// Param: dtDirType The environment variable to set: DIR_TYPE_EXE, DIR_TYPE_INC, or DIR_TYPE_LIB.
// Param: intPlatforms The platform(s) to set the environment variable for (one or more PLATFORM_TYPE values).
// END_HELP_COMMENT
int COEnvironment::RemoveDirectory(LPCSTR szDir, DIR_TYPE dtDirType /* DIR_TYPE_CURRENT */, 
								   int intPlatforms /* PLATFORM_CURRENT*/)

	{
	const char* const THIS_FUNCTION = "COEnvironment::RemoveDirectory(LPCSTR...)";

	// bring up options dlg, select directories page, and set platform and dir type if specified.
	if(DirectoryCore(dtDirType, intPlatforms) != ERROR_SUCCESS)
		return ERROR_ERROR;

	CString cstrCurrentDir, cstrDir = szDir;

	while(1)
	
		{
		// put the current directory item in edit mode, get the text, and get out of edit mode.
		MST.DoKeys("^c");
		MST.WEditText("", cstrCurrentDir);
		MST.DoKeys("{ESC}");

		// if the current directory is empty, we've reached the end of the list.
		if(cstrCurrentDir == "")
			
			{
			LOG->RecordInfo("%s: could not find dir %s for platform %d in dir type %d.", 
							THIS_FUNCTION, cstrDir, intPlatforms, dtDirType);
			
			// cancel the options dlg.
			m_optdlg.Cancel();
			return ERROR_ERROR;
			}

		// if the current directory matches the specified directory, delete it.
		if(cstrCurrentDir == cstrDir)
			{
			MST.DoKeys("{DEL}");
			break;
			}
		
		// advance to the next directory in the list.
		MST.DoKeys("{DOWN}");
		}

	// ok the options dlg.
	m_optdlg.OK();
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COEnvironment::RemoveDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT)
// Description: Remove multiple directories from one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: cstrArrayDirs A reference to a CStringArray containing the list of directories to remove.
// Param: dtDirType The environment variable to set: DIR_TYPE_EXE, DIR_TYPE_INC, or DIR_TYPE_LIB.
// Param: intPlatforms The platform(s) to set the environment variable for (one or more PLATFORM_TYPE values).
// END_HELP_COMMENT
int COEnvironment::RemoveDirectories(CStringArray &cstrArrayDirs, DIR_TYPE dtDirType /* DIR_TYPE_CURRENT */, 
									 int intPlatforms /* PLATFORM_CURRENT*/)

	{
	const char* const THIS_FUNCTION = "COEnvironment::RemoveDirectories()";

	// make sure the user specified directories to remove.
	if(cstrArrayDirs.GetSize() == 0)
		
		{
		LOG->RecordInfo("%s: no dirs specified for platform %d in dir type %d.", 
						THIS_FUNCTION, intPlatforms, dtDirType);

		return ERROR_ERROR;
		}

	// bring up options dlg, select directories page, and set platform and dir type if specified.
	if(DirectoryCore(dtDirType, intPlatforms) != ERROR_SUCCESS)
		return ERROR_ERROR;

	CString cstrCurrentDir;
	CStringArray cstrArrayDirsTmp;
	int i;
	BOOL bDirDeleted;

	// as we remove each specified directory from the actual list, we'll remove it from our temporary list.
	for(i = 0; i < cstrArrayDirs.GetSize(); i++)
		cstrArrayDirsTmp.Add(cstrArrayDirs[i]);

	// loop until all the specified directories have been removed.
	while(cstrArrayDirsTmp.GetSize() > 0)
	
		{
		// put the current directory item in edit mode, get the text, and get out of edit mode.
		MST.DoKeys("^c");
		MST.WEditText("", cstrCurrentDir);
		MST.DoKeys("{ESC}");

		// if the current directory is empty, we've reached the end of the list.
		if(cstrCurrentDir == "")
			
			{
			LOG->RecordInfo("%s: could not find all dirs for platform %d in dir type %d.", 
							THIS_FUNCTION, intPlatforms, dtDirType);
			
			// cancel the options dlg.
			m_optdlg.Cancel();
			return ERROR_ERROR;
			}

		// we haven't deleted the current dir yet.
		bDirDeleted = FALSE;

		// loop through the specified directories that haven't been deleted yet.
		for(i = 0; i < cstrArrayDirsTmp.GetSize(); i++)
			{
			// if the current dir matches a specified dir, delete it and stop searching.
			if(cstrCurrentDir == cstrArrayDirsTmp[i])
				{
				MST.DoKeys("{DEL}");
				cstrArrayDirsTmp.RemoveAt(i);
				bDirDeleted = TRUE;
				break;
				}
			}

		// if we didn't just delete the current dir, then we need to advance selection to the next dir.
		// otherwise the next dir is selected automatically.	
		if(!bDirDeleted)		
			MST.DoKeys("{DOWN}");
		}

	// ok the options dlg.
	m_optdlg.OK();
	return ERROR_SUCCESS;
	}


// BEGIN_HELP_COMMENT
// Function: int COEnvironment::RemoveDirectory(int intIndexDir, DIR_TYPE dtDirType = DIR_TYPE_CURRENT, int intPlatforms = PLATFORM_CURRENT)
// Description: Remove a directory from one of the environment variables editable on the Directories tab.
// Return: ERROR_SUCCESS if successful; ERROR_ERROR otherwise.
// Param: intIndexDir An 1-based int value specifiying the position of the directory to remove.
// Param: dtDirType The environment variable to set: DIR_TYPE_EXE, DIR_TYPE_INC, or DIR_TYPE_LIB.
// Param: intPlatforms The platform(s) to set the environment variable for (one or more PLATFORM_TYPE values).
// END_HELP_COMMENT
int COEnvironment::RemoveDirectory(int intIndexDir, DIR_TYPE dtDirType /* DIR_TYPE_CURRENT */, 
								   int intPlatforms /* PLATFORM_CURRENT*/)

	{
	const char* const THIS_FUNCTION = "COEnvironment::RemoveDirectory(int...)";
	
	// bring up options dlg, select directories page, and set platform and dir type if specified.
	if(DirectoryCore(dtDirType, intPlatforms) != ERROR_SUCCESS)
		return ERROR_ERROR;

	// cursor down to the specified directory.
	for(int i = 1; i < intIndexDir; i++)
		MST.DoKeys("{DOWN}");

	CString cstrCurrentDir;
	// edit the current directory, get the text, and get out of edit mode.
	MST.DoKeys("^c");
	MST.WEditText("", cstrCurrentDir);
	MST.DoKeys("{ESC}");

	// if the current directory is empty, we've reached the end of the list.
	if(cstrCurrentDir == "")
		
		{
		LOG->RecordInfo("%s: could not find dir %d for platform %d in dir type %d.", 
						THIS_FUNCTION, intIndexDir, intPlatforms, dtDirType);
		
		// canel the options dlg.
		m_optdlg.Cancel();
		return ERROR_ERROR;
		}

	// delete the directory and ok the options dlg.
 	MST.DoKeys("{DEL}");
	m_optdlg.OK();
	return ERROR_SUCCESS;
	}


// this array is accessed by the power-of-two of the specified platform (see wbutil.h for PLATFORM_TYPE).
int arrayPlatformIndex[] = {1, 1, 2, 3, 1, 1, 1};

int COEnvironment::DirectoryCore(DIR_TYPE dtDirType, int intPlatforms)

	{
	const char* const THIS_FUNCTION = "COEnvironment::DirectoryCore()";

	// display the options dlg.
	if(!m_optdlg.Display())
		{
		LOG->RecordInfo("%s: couldn't display options dlg.", THIS_FUNCTION);
		// just in case wrong dlg came up.
		MST.DoKeys("{ESC}");
		return ERROR_ERROR;
		}

	// select the directories page.
	if(!m_optdlg.ShowPage(TAB_DIRECTORIES, 8))
		{
		LOG->RecordInfo("%s: can't find Directories page of options dlg.", THIS_FUNCTION);
		// canel the options dlg.
		m_optdlg.Cancel();
		return ERROR_ERROR;
		}

	// set the platform if the current one isn't to be used.
	if(intPlatforms != PLATFORM_CURRENT)
		
		{
		// REVIEW (michma): we may want to support setting directories for multiple platforms someday.
		int intActualPlatform = intPlatforms;

		// see if the test wants to use the platform specified by the user.
		if(intPlatforms == PLATFORM_USER)
			intActualPlatform = GetUserTargetPlatforms();

		// get the appropriate index into the platform list.
		int intPlatformIndex = arrayPlatformIndex[GetPowerOfTwo(intActualPlatform)];

		// make sure the specified platform exists.
		if(MST.WComboCount("@1") < intPlatformIndex)
		
			{
			LOG->RecordInfo("%s: could not find platform %d.", THIS_FUNCTION, intPlatformIndex);
		
			// canel the options dlg.
			m_optdlg.Cancel();
			return ERROR_ERROR;
			}

		// set the platform.
		MST.WComboItemClk("@1", intPlatformIndex);	
		}

	// set the directory type if the current one isn't to be used.
	if(dtDirType != DIR_TYPE_CURRENT)
		
		{
		// make sure the specified directory type exists.
		if(MST.WComboCount("@2") < dtDirType)
		
			{
			LOG->RecordInfo("%s: could not find dtDirType %d.", THIS_FUNCTION, dtDirType);
		
			// canel the options dlg.
			m_optdlg.Cancel();
			return ERROR_ERROR;
			}

		// set the directory type.
		MST.WComboItemClk("@2", dtDirType);	
		}
		
	// give focus to the Directories list box (which really isn't a list box so we can't use
	// the MST.WList routines) and select the first item.
	//MST.WComboIteDoKeys("%d{HOME}");
	CString HK = "%" ;
	HK += ExtractHotKey(GetLocString(IDS_DIRECTORIES));
	MST.DoKeys(HK.GetBuffer(HK.GetLength()));
	MST.DoKeys("{HOME}") ;

	return ERROR_SUCCESS;
	}


