//
// Debug API - FileSystem Functions
//

#include "tests.h"
#include "logging.h"

#ifdef _XBOX

void testFileSystem() {} // these APIs are xboxdbg (Win32) only

#else

static void testDmSendFileA();
static void testDmSendFileW();
static void testDmReceiveFileA();
static void testDmReceiveFileW();
static void testDmGetFileAttributes();
static void testDmSetFileAttributes();
static void testDmMkdir();
static void testDmRenameFile();
static void testDmDeleteFile();
static void testDmWalkDir();
static void testDmCloseDir();

void testFileSystem()
{
	testDmSendFileA();
	testDmSendFileW();
	testDmReceiveFileA();
	testDmReceiveFileW();
	testDmGetFileAttributes();
	testDmSetFileAttributes();
	testDmMkdir();
	testDmRenameFile();
	testDmDeleteFile();
	testDmWalkDir();
	testDmCloseDir();
}

void testDmWalkDir()
{
	HRESULT				hr;
	PDM_WALK_DIR		pWalkDir = NULL;
	DM_FILE_ATTRIBUTES	buffer;

	// Pass NULL for WalkDir

	pWalkDir = NULL;
	memset(&buffer, 0, sizeof buffer);
	VERIFY_HRESULT(DmWalkDir(NULL, "e:\\", &buffer), E_INVALIDARG);
	if (hr != E_INVALIDARG)
		DmCloseDir(pWalkDir);

	// Pass NULL for szDir

	pWalkDir = NULL;
	memset(&buffer, 0, sizeof buffer);
	VERIFY_HRESULT(DmWalkDir(&pWalkDir, NULL, &buffer), E_INVALIDARG);
	if (hr != E_INVALIDARG)
		DmCloseDir(pWalkDir);

	// Pass NULL for DM_FILE_ATTRIBUTES ptr

	pWalkDir = NULL;
	VERIFY_HRESULT(DmWalkDir(&pWalkDir, "e:\\", NULL), E_INVALIDARG);
	if (hr != E_INVALIDARG)
		DmCloseDir(pWalkDir);

	// Pass valid WalkDir

	pWalkDir = NULL;
	memset(&buffer, 0, sizeof buffer);
	VERIFY_HRESULT_ALT(DmWalkDir(&pWalkDir, "e:\\samples", &buffer), XBDM_NOERR, XBDM_ENDOFLIST);
	if (hr == XBDM_NOERR || hr == XBDM_ENDOFLIST)
		hr = DmCloseDir(pWalkDir);

	// Pass invalid directory for szDir

	pWalkDir = NULL;
	memset(&buffer, 0, sizeof buffer);
	VERIFY_HRESULT(DmWalkDir(&pWalkDir, "e:\\bogusdirectory", &buffer), XBDM_CANNOTACCESS);
	if (hr == XBDM_NOERR || hr == XBDM_ENDOFLIST)
		DmCloseDir(pWalkDir);
	
	// Pass root directory for szDir

	pWalkDir = NULL;
	memset(&buffer,0,sizeof buffer);
	VERIFY_HRESULT(DmWalkDir(&pWalkDir, "e:\\", &buffer), XBDM_NOERR);
	if (hr == XBDM_NOERR || hr == XBDM_ENDOFLIST)
		hr = DmCloseDir(pWalkDir);
	
	// Pass long directory name for szDir

	pWalkDir = NULL;
	memset(&buffer,0,sizeof buffer);
	VERIFY_HRESULT_ALT(DmMkdir("e:\\123456789012345678901234567890123456789012"), , XBDM_NOERR, XBDM_ALREADYEXISTS);
	VERIFY_HRESULT_ALT(DmWalkDir(&pWalkDir, "e:\\123456789012345678901234567890123456789012", &buffer), XBDM_NOERR, XBDM_ENDOFLIST);
	if (hr == XBDM_NOERR || hr == XBDM_ENDOFLIST)
		DmCloseDir(pWalkDir);
}

void testDmCloseDir()
{
	HRESULT				hr;
	PDM_WALK_DIR		pWalkDir = NULL;
	DM_FILE_ATTRIBUTES	buffer;

	// using Null

	hr = DmCloseDir(NULL);
	VERIFY_HRESULT(DmCloseDir(NULL), E_INVALIDARG);

	// using valid object

	hr = DmWalkDir(&pWalkDir, "e:\\", &buffer);
	if (hr != XBDM_NOERR)
		LogTestResult("DmCloseDir(valid PDM_WALK_DIR)", false, "Unable to obtain valid DM_WALK_DIR object");
	else 
		VERIFY_HRESULT(DmCloseDir(pWalkDir), XBDM_NOERR);
}

void testDmMkdir()
{
	HRESULT			hr;
	char			buffer[1024];

	// Pass NULL

	VERIFY_HRESULT(DmMkdir(NULL), E_INVALIDARG);

	// Pass empty string

	VERIFY_HRESULT(DmMkdir(""), XBDM_CANNOTCREATE);

	// Pass existing directory

	VERIFY_HRESULT(DmMkdir("e:\\dxt"), XBDM_ALREADYEXISTS);

	// Pass existing file

	GetEnvironmentVariable("SystemRoot", buffer, sizeof(buffer));
	strcat(buffer, "\\taskman.exe");

	hr = DmSendFile(buffer, "e:\\PreExistingFile");
	VERIFY_HRESULT(DmMkdir("e:\\PreExistingFile"), XBDM_ALREADYEXISTS);

	// Pass relative path

#if 0 // Bug 3443, resolved by design in your absence
	VERIFY_HRESULT(DmMkdir("e:\\dxt\\..\\bogus"), hr);
#endif

	// Pass invalid path

	VERIFY_HRESULT(DmMkdir("o:\\"), XBDM_NOSUCHFILE);

	// Pass long directory name

	VERIFY_HRESULT_ALT(DmMkdir("e:\\123456789012345678901234567890123456789012"), XBDM_NOERR, XBDM_ALREADYEXISTS);

	// Pass long path

	VERIFY_HRESULT_ALT(DmMkdir("e:\\dxt\\123456789012345678901234567890123456789012"), XBDM_NOERR, XBDM_ALREADYEXISTS);
	
	// Pass directory name with leading spaces

	VERIFY_HRESULT(DmMkdir("  e:\\leading spaces in dir name"), XBDM_CANNOTCREATE);

	// Pass directory name with trailing spaces

	VERIFY_HRESULT(DmMkdir("e:\\trailing spaces in dir name   "), XBDM_NOERR);
}


void testDmDeleteFile()
{
	HRESULT				hr;
	DM_FILE_ATTRIBUTES	FileAttributes;
	char				szFileToSend[1024];

	GetEnvironmentVariable("SystemRoot", szFileToSend, sizeof(szFileToSend));
	strcat(szFileToSend, "\\taskman.exe");

	// Pass NULL with IsDirectory = false

	VERIFY_HRESULT(DmDeleteFile(NULL, false), E_INVALIDARG);

	// Pass NULL with IsDirectory = true

	VERIFY_HRESULT(DmDeleteFile(NULL, true), E_INVALIDARG);

	// Use existing file with IsDirectory==false

	hr = DmSendFile(szFileToSend, "e:\\PreExistingFile");
	VERIFY_HRESULT(DmDeleteFile("e:\\PreExistingFile", false), XBDM_NOERR);

	// Use existing file with IsDirectory==true

	hr = DmSendFile(szFileToSend, "e:\\PreExistingFile");
	VERIFY_HRESULT(DmDeleteFile("e:\\PreExistingFile", true), XBDM_CANNOTACCESS);

	// Use invalid name with IsDirectory==false

	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFile_bogus_name", false), XBDM_NOSUCHFILE);
	
	// Use invalid name with IsDirectory==true

	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFile_bogus_name", true), XBDM_NOSUCHFILE);
	
	// Use valid directory empty with IsDirectory==false

	hr = DmMkdir("e:\\DmDeleteFileEmptyDir1");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileEmptyDir1", false), XBDM_CANNOTACCESS);

	// Use valid directory empty with IsDirectory==true

	hr = DmMkdir("e:\\DmDeleteFileEmptyDir2");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileEmptyDir2", true), XBDM_NOERR);

	// Use valid directory non-empty with IsDirectory==false

	hr = DmMkdir("e:\\DmDeleteFileNonEmptyDir1");
	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileNonEmptyDir1\taskman");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileNonEmptyDir1", false), XBDM_CANNOTACCESS);

	// Use valid directory non-empty with IsDirectory==true

	hr = DmMkdir("e:\\DmDeleteFileNonEmptyDir2");
	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileNonEmptyDir2\\taskman");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileNonEmptyDir2", true), XBDM_DIRNOTEMPTY);
	
	// Use read-only directory

	hr = DmMkdir("e:\\DmDeleteFileReadOnlyDir");
	hr = DmGetFileAttributes("e:\\DmDeleteFileReadOnlyDir", &FileAttributes);
	FileAttributes.Attributes |= FILE_ATTRIBUTE_READONLY;
	hr = DmSetFileAttributes("e:\\DmDeleteFileReadOnlyDir", &FileAttributes);
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileReadOnlyDir", true), XBDM_CANNOTACCESS);

	// Use hidden directory

	hr = DmMkdir("e:\\DmDeleteFileHiddenDir");
	hr = DmGetFileAttributes("e:\\DmDeleteFileHiddenDir", &FileAttributes);
	FileAttributes.Attributes |= FILE_ATTRIBUTE_HIDDEN;
	hr = DmSetFileAttributes("e:\\DmDeleteFileHiddenDir", &FileAttributes);
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileHiddenDir", true), XBDM_NOERR);

	// Use read-only file

	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileReadOnlyFile");
	hr = DmGetFileAttributes("e:\\DmDeleteFileReadOnlyFile", &FileAttributes);
	FileAttributes.Attributes |= FILE_ATTRIBUTE_READONLY;
	hr = DmSetFileAttributes("e:\\DmDeleteFileReadOnlyFile", &FileAttributes);
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileReadOnlyFile", false), XBDM_CANNOTACCESS);

	// Use hidden file

	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileHiddenFile");
	hr = DmGetFileAttributes("e:\\DmDeleteFileHiddenFile", &FileAttributes);
	FileAttributes.Attributes |= FILE_ATTRIBUTE_HIDDEN;
	hr = DmSetFileAttributes("e:\\DmDeleteFileHiddenFile", &FileAttributes);
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileHiddenFile", false), XBDM_NOERR);
	
	// Use filename with leading spaces

	hr = DmSendFile(szFileToSend, "e:\\   DmDeleteFileFilenameLeadingSpaces");
	VERIFY_HRESULT(DmDeleteFile("e:\\   DmDeleteFileFilenameLeadingSpaces", false), XBDM_NOERR);

	// Use filename with trailing spaces

	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileFilenameTrailingSpaces   ");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileFilenameTrailingSpaces   ", false), XBDM_NOERR);

	// Use directory with leading spaces

	hr = DmMkdir("e:\\   DmDeleteFileDirLeadingSpaces");
	VERIFY_HRESULT(DmDeleteFile("e:\\   DmDeleteFileDirLeadingSpaces", true), XBDM_NOERR);

	// Use directory with trailing spaces

	hr = DmMkdir("e:\\DmDeleteFileDirTrailingSpaces   ");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileDirTrailingSpaces   ", true), XBDM_NOERR);

	// Use long filename

	hr = DmSendFile(szFileToSend, "e:\\DmDeleteFileLongFileName123456789012345678");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileLongFileName123456789012345678", false), XBDM_NOERR);

	// Use long directory name

	hr = DmMkdir("e:\\DmDeleteFileLongDirName1234567890123456789");
	VERIFY_HRESULT(DmDeleteFile("e:\\DmDeleteFileLongDirName1234567890123456789", true), XBDM_NOERR);
}

void testDmRenameFile()
{
	HRESULT hr;
	char	szFileToSend[1024];

	GetEnvironmentVariable("SystemRoot", szFileToSend, sizeof(szFileToSend));
	strcat(szFileToSend, "\\taskman.exe");

	// Pass NULL for OldName

	VERIFY_HRESULT(DmRenameFile(NULL, "e:\\DmRenameFileTemp1b"), E_INVALIDARG);

	// Pass NULL for NewName

	hr = DmSendFile(szFileToSend, "e:\\DmRenameFileTemp2a");
	VERIFY_HRESULT(DmRenameFile("e:\\DmRenameFileTemp2a", NULL), E_INVALIDARG);

	// Pass empty string for OldName

	VERIFY_HRESULT(DmRenameFile("", "e:\\DmRenameFileTemp3b"), XBDM_CANNOTACCESS);

	// Pass empty string for NewName

	hr = DmSendFile(szFileToSend, "e:\\DmRenameFileTemp4a");
	VERIFY_HRESULT(DmRenameFile("e:\\DmRenameFileTemp4a", ""), XBDM_BADFILENAME);

	// Pass valid file for OldName
	// Pass directory for OldName
	// Pass invalid file for OldName
	// Pass read-only file for OldName
	// Pass valid file with leading/trailing spaces for OldName
	// Pass valid name for NewName
	// Pass valid name with leading/trailing spaces for NewName
	// Pass directory for NewName
	// Pass existing file for NewName
	// Pass existing read-only file for NewName
}

void testDmGetFileAttributes()
{
	HRESULT hr;
	DM_FILE_ATTRIBUTES FileAttr;
	char	szFileToSend[1024];

	GetEnvironmentVariable("SystemRoot", szFileToSend, sizeof(szFileToSend));
	strcat(szFileToSend, "\\taskman.exe");

	// Pass Null for filename

	VERIFY_HRESULT(DmGetFileAttributes(NULL, &FileAttr), E_INVALIDARG);
	
	// Pass Null for FileAttributes

	VERIFY_HRESULT(DmGetFileAttributes("e:\\dxt", NULL), E_INVALIDARG);

	// Pass empty string for filename

	VERIFY_HRESULT(DmGetFileAttributes("", &FileAttr), XBDM_NOSUCHFILE);
	
	// Pass valid filename

	hr = DmSendFile(szFileToSend, "e:\\DmGetFileAttributesValidFile");
	VERIFY_HRESULT(DmGetFileAttributes("e:\\DmGetFileAttributesValidFile", &FileAttr), XBDM_NOERR);

	// Pass directory name

	VERIFY_HRESULT(DmGetFileAttributes("e:\\dxt", &FileAttr), XBDM_NOERR);

	// Pass valid filename with leading/trailing spaces
	// Pass invalid filename
	// Pass long filename
	// Pass long directory name
	// Pass MAX_PATH filename
}

void testDmSetFileAttributes()
{
	HRESULT hr;
	DM_FILE_ATTRIBUTES FileAttr;
	char	szFileToSend[1024];

	GetEnvironmentVariable("SystemRoot", szFileToSend, sizeof(szFileToSend));
	strcat(szFileToSend, "\\taskman.exe");

	// Pass Null for filename

	VERIFY_HRESULT(DmSetFileAttributes(NULL, &FileAttr), E_INVALIDARG);

	// Pass Null for FileAttributes

	VERIFY_HRESULT(DmSetFileAttributes("e:\\dxt", NULL), E_INVALIDARG);

	// Pass empty string for filename

	VERIFY_HRESULT(DmSetFileAttributes("", &FileAttr), XBDM_CANNOTACCESS);
	
	// Pass valid filename

	hr = DmSendFile(szFileToSend, "e:\\DmSetFileAttributesValidFile1");
	hr = DmGetFileAttributes("e:\\DmSetFileAttributesValidFile1", &FileAttr);
	FileAttr.Attributes |= FILE_ATTRIBUTE_READONLY;
	VERIFY_HRESULT(DmSetFileAttributes("e:\\DmSetFileAttributesValidFile1", &FileAttr), XBDM_NOERR);

	// Pass directory name

	hr = DmGetFileAttributes("e:\\dxt", &FileAttr);
	FileAttr.Attributes |= FILE_ATTRIBUTE_READONLY;
	VERIFY_HRESULT(DmSetFileAttributes("e:\\dxt", &FileAttr), XBDM_NOERR);

	// Pass valid filename with leading/trailing spaces
	// Pass invalid filename
	// Pass long filename
	// Pass long directory name
	// Pass MAX_PATH filename

	// insure that DmSetFileAttributes does NOT modify its arguments
	// bug 3420

	DM_FILE_ATTRIBUTES FileAttr2;
	hr = DmSendFile(szFileToSend, "e:\\DmGetFileAttributesValidFile2");
	hr = DmGetFileAttributes("e:\\DmGetFileAttributesValidFile2", &FileAttr);
	FileAttr.Attributes |= FILE_ATTRIBUTE_READONLY;
	FileAttr2 = FileAttr;
	hr = DmSetFileAttributes("e:\\DmGetFileAttributesValidFile2", &FileAttr);
	if (memcmp(&FileAttr, &FileAttr2, sizeof(FileAttr)) != 0)
		LogTestResult("DmSetFileAttributes() - argument not modified", false, "argument modified");
	else
		LogTestResult("DmSetFileAttributes() - argument not modified", true, "");
}

void testDmSendFile(BOOL fUseWideChar)
{
	HRESULT hr;
	char	szFileToSend[MAX_PATH];
	WCHAR	wszFileToSend[MAX_PATH];

	if (fUseWideChar)
	{
		GetEnvironmentVariableW(L"SystemRoot", wszFileToSend, sizeof(wszFileToSend));
		wcscat(wszFileToSend, L"\\taskman.exe");
	}
	else
	{
		GetEnvironmentVariableA("SystemRoot", szFileToSend, sizeof(szFileToSend));
		strcat(szFileToSend, "\\taskman.exe");
	}

	// Pass NULL for LocalName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmSendFileW(NULL, "e:\\DmSendFileTemp1b"), E_INVALIDARG);
	else
		VERIFY_HRESULT(DmSendFileA(NULL, "e:\\DmSendFileTemp1b"), E_INVALIDARG);

	// Pass NULL for RemoteName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmSendFileW(wszFileToSend, NULL), E_INVALIDARG);
	else
		VERIFY_HRESULT(DmSendFileA(szFileToSend, NULL), E_INVALIDARG);

	// Pass empty string for LocalName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmSendFileW(L"", "e:\\DmSendFileTemp3b"), XBDM_BADFILENAME);
	else
		VERIFY_HRESULT(DmSendFileA("", "e:\\DmSendFileTemp3b"), XBDM_BADFILENAME);

	// Pass empty string for NewName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmSendFileW(wszFileToSend, ""), XBDM_BADFILENAME);
	else
		VERIFY_HRESULT(DmSendFileA(szFileToSend, ""), XBDM_BADFILENAME);

	// Pass read-only file for LocalName
	// Pass invalid file for LocalName
	// Pass directory for LocalName
	// Pass valid file with leading/trailing spaces for LocalName
	// Pass read-only file for RemoteName
	// Pass invalid file for RemoteName
	// Pass directory for RemoteName
	// Pass valid file with leading/trailing spaces for RemoteName
}

void testDmSendFileA() { testDmSendFile(false); }
void testDmSendFileW() { testDmSendFile(true); }

void testDmReceiveFile(BOOL fUseWideChar)
{
	HRESULT hr;
	char	szFileToReceive[MAX_PATH];
	WCHAR	wszFileToReceive[MAX_PATH];

	// Pass NULL for LocalName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmReceiveFileW(NULL, "e:\\samples\\cartoon\\cartoon.xbe"), E_INVALIDARG);
	else
		VERIFY_HRESULT(DmReceiveFileA(NULL, "e:\\samples\\cartoon\\cartoon.xbe"), E_INVALIDARG);

	// Pass NULL for RemoteName

	if (fUseWideChar)
	{
		GetEnvironmentVariableW(L"Temp", wszFileToReceive, sizeof(wszFileToReceive));
		wcscat(wszFileToReceive, L"\\DmReceiveFileTemp2a");
	}
	else
	{
		GetEnvironmentVariable("Temp", szFileToReceive, sizeof(szFileToReceive));
		strcat(szFileToReceive, "\\DmReceiveFileTemp2a");
	}

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmReceiveFileW(L"c:\\winnt\\temp\\DmReceiveFileTemp2a", NULL), E_INVALIDARG);
	else
		VERIFY_HRESULT(DmReceiveFileA("c:\\winnt\\temp\\DmReceiveFileTemp2a", NULL), E_INVALIDARG);

	// Pass empty string for LocalName

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmReceiveFileW(L"", "e:\\samples\\cartoon\\cartoon.xbe"), XBDM_BADFILENAME);
	else
		VERIFY_HRESULT(DmReceiveFileA("", "e:\\samples\\cartoon\\cartoon.xbe"), XBDM_BADFILENAME);

	// Pass empty string for RemoteName

	if (fUseWideChar)
	{
		GetEnvironmentVariableW(L"Temp", wszFileToReceive, sizeof(wszFileToReceive));
		wcscat(wszFileToReceive, L"\\DmReceiveFileTemp4a");
	}

	if (fUseWideChar)
		; //VERIFY_HRESULT(DmReceiveFileW(wszFileToReceive, ""), XBDM_BADFILENAME);
	else
		VERIFY_HRESULT(DmReceiveFileA(szFileToReceive, ""), XBDM_BADFILENAME);

	// Pass read-only file for LocalName
	// Pass invalid file for LocalName
	// Pass directory for LocalName
	// Pass valid file with leading/trailing spaces for LocalName
	// Pass read-only file for RemoteName
	// Pass invalid file for RemoteName
	// Pass directory for RemoteName
	// Pass valid file with leading/trailing spaces for RemoteName
}

void testDmReceiveFileA() { testDmReceiveFile(false); }
void testDmReceiveFileW() { testDmReceiveFile(true); }

#endif // #ifdef _XBOX
