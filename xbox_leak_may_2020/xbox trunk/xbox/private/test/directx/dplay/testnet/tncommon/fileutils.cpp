//==================================================================================
// Includes
//==================================================================================
#include <windows.h>
#include <direct.h>
#include <errno.h>
#ifndef _XBOX // no commctrl
#include <commctrl.h>
#endif // ! XBOX

#include <stdlib.h>	// NT BUILD needs this for _MAX_xxx and _splitpath



#include "main.h"
#include "resource.h"


#ifndef NO_TNCOMMON_DEBUG_SPEW
#include "debugprint.h"
#endif // ! NO_TNCOMMON_DEBUG_SPEW
#include "cppobjhelp.h"
#include "linklist.h"
#include "linkedstr.h"
#include "strutils.h"
#include "sprintf.h"

#include "fileutils.h"





//==================================================================================
// Structures
//==================================================================================
typedef struct tagCOPYTREEDATA
{
	BOOL			fSkip; // whether the entire directory should be skipped (only can happen when updating progress)
	DWORD			dwOptions; // options for copying the tree, see FILECTO_xxx
	PLSTRINGLIST	pSkipFiles; // optional list of filepaths (may use '*' wildcard character) to skip
	HANDLE			hProgressThreadStartedEvent; // handle to event for progress thread to set when ready
	HWND			hProgressWindow; // window to update with progress, if any
	DWORD			dwNumFilesHandled; // number of files that have been handled so far
	DWORD			dwTotalNumFiles; // total number of files in under topmost directory
} COPYTREEDATA, * PCOPYTREEDATA;




//==================================================================================
// Local Prototypes
//==================================================================================
HRESULT CopyTreeInternal(char* szSourceDirPath, char* szDestDirPath,
						PCOPYTREEDATA pCopyTreeData);

#ifndef NO_FILEUTILS_COPYTREEPROGRESS
INT_PTR CALLBACK CopyTreeProgressWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
											LPARAM lParam);

DWORD WINAPI CopyTreeProgressThreadProc(LPVOID lpvParameter);
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS




//==================================================================================
// Globals
//==================================================================================
PLSTRINGLIST	g_pTrackedFiles = NULL;





#ifndef _XBOX // no file reading...
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileReadTextFile()"
//==================================================================================
// FileReadTextFile
//----------------------------------------------------------------------------------
//
// Description: Sets the file specified's attributes to the values passed in.  You
//				can pass NULL for any value you don't want to change.
//
// Arguments:
//	char* szFilePath				Path to file to read.
//	DWORD dwOptions					Options for reading the file (see FILERTFO_xxx).
//	PFILEREADLINEPROC pfnReadLine	Callback function to handle each line.
//	PVOID pvContext					Pointer to user context to specify to function.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileReadTextFile(char* szFilePath, DWORD dwOptions,
						PFILEREADLINEPROC pfnReadLine, PVOID pvContext)
{
	HRESULT			hr;
	HANDLE			hFile = INVALID_HANDLE_VALUE;
	BOOL			fEndOfFile = FALSE;
	DWORD			dwStringBufferSize = 0;
	char*			pszStringBuffer = NULL;
	DWORD			dwCurrentStringIndex;
	char			c = '\0';
	DWORD			dwBytesRead;
	BOOL			fInComment = FALSE;
	char*			pszTemp1 = NULL;
	char*			pszTemp2 = NULL;
	int				iPos;
	DWORD			dwRealLineNumber = 0;
	FILELINEINFO	fli;
	BOOL			fStopReading = FALSE;
#ifdef _XBOX
	WCHAR			szWideFilePath[256];
#endif


	ZeroMemory(&fli, sizeof (FILELINEINFO));


	if ((szFilePath == NULL) || (strcmp(szFilePath, "") == 0))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an invalid filepath!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed an invalid file)

	if (pfnReadLine == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed an invalid callback function!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed an invalid callback)



	// Initialize the buffer
	dwStringBufferSize = READTEXTFILE_BUFFER_SIZE_INCREMENT;
	pszStringBuffer = (char*) LocalAlloc(LPTR, dwStringBufferSize);
	if (pszStringBuffer == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)


	/*
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(1, "Opening file \"%s\".", 1, szFilePath);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	*/


	// Open the file
#ifndef _XBOX // Damn ANSI conversion
	hFile = CreateFile(szFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, 0, NULL);
#else // ! XBOX
	// Convert to wide
	mbstowcs(szWideFilePath, szFilePath, strlen(szFilePath));
	szWideFilePath[strlen(szFilePath)] = 0;

	// Call CreateFile with wide version
	hFile = CreateFile(szWideFilePath, GENERIC_READ, FILE_SHARE_READ, NULL,
						OPEN_EXISTING, 0, NULL);
#endif // XBOX

	if (hFile == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't open file \"%s\"!", 1, szFilePath);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		if (hr == S_OK)
			hr = E_FAIL;

		goto DONE;
	} // end if (couldn't open file)


	// Loop until the end of the file
	do
	{
		// Start at beginning of buffer
		dwCurrentStringIndex = 0;

		dwRealLineNumber++;

		// This will not get updated for escaped lines until the next real newline.
		fli.dwLineNumber = dwRealLineNumber;

		// Loop until the end of the line or the end of the file
		do
		{
			// Increase the buffer size if we won't have enough room
			if (dwCurrentStringIndex >= dwStringBufferSize)
			{
				PVOID	pvTemp;


				pvTemp = LocalAlloc(LPTR, (dwStringBufferSize + READTEXTFILE_BUFFER_SIZE_INCREMENT));
				if (pvTemp == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)

				CopyMemory(pvTemp, pszStringBuffer, dwStringBufferSize);
				dwStringBufferSize += READTEXTFILE_BUFFER_SIZE_INCREMENT;

				LocalFree(pszStringBuffer);
				pszStringBuffer = (char*) pvTemp;
				pvTemp = NULL;
			} // end if (we're not going to have enough room)

			if (! ReadFile(hFile, &c, 1, &dwBytesRead, NULL))
			{
				hr = GetLastError();
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Failed reading from file (line %u)!",
					1, dwRealLineNumber);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (failed reading from file)

			if (dwBytesRead == 0)
			{
				// End of file.  Terminate the string here and stop looping.
				fEndOfFile = TRUE;

				pszStringBuffer[dwCurrentStringIndex] = '\0';
				break;
			} // end if (didn't read anything)

			pszStringBuffer[dwCurrentStringIndex] = c;

			if ((c == '\n') && (dwCurrentStringIndex > 0) &&
				(pszStringBuffer[dwCurrentStringIndex - 1] == '\r'))
			{
				// We found a carraige-return\newline sequence.  If escaped
				// lines are allowed, make sure it's not one of those.

				if (dwOptions & FILERTFO_ALLOW_ESCAPED_NEWLINES)
				{
					if (StringIsCharBackslashEscaped(pszStringBuffer, pszStringBuffer + dwCurrentStringIndex - 1))
					{
						// Move the current index back to overwrite the escaped
						// characters.
						dwCurrentStringIndex -= 3;

						// We still want to increase our total line count (but
						// not our current line number).
						dwRealLineNumber++;
					} // end if (this carraige-return/newline is escaped)
					else
					{
						// Terminate the string at the start of the sequence and
						// stop looping.
						pszStringBuffer[dwCurrentStringIndex - 1] = '\0';
						break;
					} // end else (this carraige-return/newline is not escaped)
				} // end if (we should make sure the first character is not escaped)
				else
				{
					// Terminate the string at the start of the sequence and stop
					// looping.
					pszStringBuffer[dwCurrentStringIndex - 1] = '\0';
					break;
				} // end else (escaped newlines aren't allowed)
			} // end if (this isn't the first character)

			dwCurrentStringIndex++;
		} // end do (while not at end of line or end of file)
		while (TRUE);

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Line %u: \"%s\".", 2, fli.dwLineNumber, pszStringBuffer);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		// If we should ignore things in C style comments, do that.
		if (dwOptions & FILERTFO_IGNORE_C_COMMENTS)
		{
			if (fInComment)
			{
				iPos = StringContainsString(pszStringBuffer, "*/", TRUE, 0);
				if (iPos >= 0)
				{
					// Chop off any stuff before the comment ended
					if (! StringSplit(pszStringBuffer, iPos + 2, NULL, pszStringBuffer))
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "StringSplit() failed (line %u)!",
							1, dwRealLineNumber);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW

						hr = E_FAIL;
						goto DONE;
					} // end if (string split failed)

					fInComment = FALSE;
				} // end if (we found a multiline end comment token)
				else
				{
					// We started off with this line as a comment, and didn't
					// end the comment here, so this line should be ignored.
					continue;
				} // end else (we didn't find a multiline end comment token)
			} // end if (we're already in a comment from a previous line)

			do
			{
				iPos = 0;
				hr = StringGetBetween(pszStringBuffer, "/*", "*/", TRUE, FALSE,
										FALSE, TRUE, FALSE, &iPos,
										pszStringBuffer, NULL, &fInComment);
				if (hr != S_OK)
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Removing multiple line/inline C comments failed (line %u)!",
						1, dwRealLineNumber);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
					goto DONE;
				} // end if (removing stuff failed)
			} // end do
			while (iPos >= 0);

			iPos = 0;
			hr = StringGetBetween(pszStringBuffer, "//", NULL, TRUE, FALSE,
								FALSE, TRUE, FALSE, &iPos, pszStringBuffer,
								NULL, NULL);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Removing single line C comments failed (line %u)!",
					1, dwRealLineNumber);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (removing stuff failed)
		} // end if (C style comments are allowed)


		// If we should ignore things in semicolon comments, do that.
		if (dwOptions & FILERTFO_IGNORE_SEMICOLON_COMMENTS)
		{
			iPos = 0;
			hr = StringGetBetween(pszStringBuffer, ";", NULL, TRUE, FALSE,
								FALSE, TRUE, FALSE, &iPos, pszStringBuffer,
								NULL, NULL);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Removing semicolon comments failed (line %u)!",
					1, dwRealLineNumber);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (removing stuff failed)
		} // end if (C style comments are allowed)


		// Clear off surrounding whitespace, if allowed
		if (dwOptions & FILERTFO_IGNORE_SURROUNDING_WHITESPACE)
		{
			StringPopLeadingChars(pszStringBuffer, "\t ", TRUE); 
			StringPopTrailingChars(pszStringBuffer, "\t ", TRUE);
		} // end if (should ignore whitespace)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Line %u: \"%s\".", 2, fli.dwLineNumber, pszStringBuffer);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		// If the user lets us expand environment variables, check for those.
		if (dwOptions & FILERTFO_EXPAND_ENV_VARS)
		{
			DWORD	dwTempSize;
			char	szBogus[1];


			// There's a bug in Windows 95 where if you pass NULL or a pointer to
			// invalid string buffer to this function it will fail, so we use a temp
			// thing to trick it into shutting up.
			dwTempSize = ExpandEnvironmentStrings(pszStringBuffer, szBogus, 0);

			if (dwTempSize == 0)
			{
				hr = GetLastError();

				if (hr == S_OK)
					hr = E_FAIL;

				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Expanding environment variables in \"%s\" failed (line %u)!",
					2, pszStringBuffer, dwRealLineNumber);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (expanding env vars failed)

			pszTemp1 = (char*) LocalAlloc(LPTR, dwTempSize);
			if (pszTemp1 == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (allocating memory failed)

			if (ExpandEnvironmentStrings(pszStringBuffer, pszTemp1, dwTempSize) == 0)
			{
				hr = GetLastError();

				if (hr == S_OK)
					hr = E_FAIL;

				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Expanding environment variables in \"%s\" failed (line %u)!",
					2, pszStringBuffer, dwRealLineNumber);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (expanding env vars failed)

			// Switch buffers if we don't have enough room, otherwise, copy
			// the new buffer back over the original one.
			if (dwTempSize >= dwStringBufferSize)
			{
				dwStringBufferSize = dwTempSize;
				LocalFree(pszStringBuffer);
				pszStringBuffer = pszTemp1;
				pszTemp1 = NULL;
			} // end if (we're not going to have enough room)
			else
			{
				strcpy(pszStringBuffer, pszTemp1);
				LocalFree(pszTemp1);
				pszTemp1 = NULL;
			} // end else (we have enough room)
		} // end if (we can expand environment variables)

		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(9, "Line %u: \"%s\".", 2, fli.dwLineNumber, pszStringBuffer);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/

		// If it's an empty line (possibly after the above processing), skip it
		// if the caller doesn't want them.
		if ((strcmp(pszStringBuffer, "") == 0) &&
			(dwOptions & FILERTFO_SKIP_EMPTY_LINES))
		{
			continue;
		} // end if (empty line and should be skipped)


		fli.dwType = FILERTFT_NORMAL;

		if (dwOptions & FILERTFO_PARSE_INI_STYLE_SECTIONS)
		{
			if ((StringStartsWith(pszStringBuffer, "[", TRUE)) &&
				(StringEndsWith(pszStringBuffer, "]", TRUE)))
			{
				StringPopLeadingChars(pszStringBuffer, "[", TRUE);
				StringPopTrailingChars(pszStringBuffer, "]", TRUE);

				fli.dwType = FILERTFT_SECTION;


				// Forget the previous section, subsection, and subsubsection

				if (fli.pszCurrentSection != NULL)
				{
					LocalFree(fli.pszCurrentSection);
					fli.pszCurrentSection = NULL;
				} // end if (previously in section)

				if (fli.pszCurrentSubsection != NULL)
				{
					LocalFree(fli.pszCurrentSubsection);
					fli.pszCurrentSubsection = NULL;
				} // end if (previously in subsection)

				if (fli.pszCurrentSubsubsection != NULL)
				{
					LocalFree(fli.pszCurrentSubsubsection);
					fli.pszCurrentSubsubsection = NULL;
				} // end if (previously in subsubsection)


				// Save this new section

				fli.pszCurrentSection = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer) + 1);
				if (fli.pszCurrentSection == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				strcpy(fli.pszCurrentSection, pszStringBuffer);
			} // end if (it's a section)
			else if ((StringStartsWith(pszStringBuffer, "{", TRUE)) &&
				(StringEndsWith(pszStringBuffer, "}", TRUE)))
			{
				StringPopLeadingChars(pszStringBuffer, "{", TRUE);
				StringPopTrailingChars(pszStringBuffer, "}", TRUE);

				fli.dwType = FILERTFT_SUBSECTION;


				// Forget the previous subsection, and subsubsection

				if (fli.pszCurrentSubsection != NULL)
				{
					LocalFree(fli.pszCurrentSubsection);
					fli.pszCurrentSubsection = NULL;
				} // end if (previously in subsection)

				if (fli.pszCurrentSubsubsection != NULL)
				{
					LocalFree(fli.pszCurrentSubsubsection);
					fli.pszCurrentSubsubsection = NULL;
				} // end if (previously in subsubsection)


				// Save this new subsection

				fli.pszCurrentSubsection = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer) + 1);
				if (fli.pszCurrentSubsection == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				strcpy(fli.pszCurrentSubsection, pszStringBuffer);
			} // end else if (it's a subsection)
			else if ((StringStartsWith(pszStringBuffer, "(", TRUE)) &&
				(StringEndsWith(pszStringBuffer, ")", TRUE)))
			{
				StringPopLeadingChars(pszStringBuffer, "(", TRUE);
				StringPopTrailingChars(pszStringBuffer, ")", TRUE);

				fli.dwType = FILERTFT_SUBSUBSECTION;


				// Forget the previous subsubsection

				if (fli.pszCurrentSubsubsection != NULL)
				{
					LocalFree(fli.pszCurrentSubsubsection);
					fli.pszCurrentSubsubsection = NULL;
				} // end if (previously in subsubsection)


				// Save this new subsubsection

				fli.pszCurrentSubsubsection = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer) + 1);
				if (fli.pszCurrentSubsubsection == NULL)
				{
					hr = E_OUTOFMEMORY;
					goto DONE;
				} // end if (couldn't allocate memory)
				strcpy(fli.pszCurrentSubsubsection, pszStringBuffer);
			} // end else if (it's a subsubsection)
		} // end if (section info should be parsed)

		if (StringStartsWith(pszStringBuffer, "#", TRUE))
		{
			DWORD	dwTempSize;
			DWORD	dwFields;


			dwTempSize = strlen(pszStringBuffer) + 1;
			pszTemp1 = (char*) LocalAlloc(LPTR, dwTempSize);
			if (pszTemp1 == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)

			// Separate items by white space
			dwFields = StringSplitIntoFields(pszStringBuffer, " \t", pszTemp1, &dwTempSize);
			if (dwFields > 1)
			{
				// Check for #includes, if allowed
				if ((dwOptions & FILERTFO_ALLOW_POUND_INCLUDES) &&
					(StringCmpNoCase(StringGetFieldPtr(pszTemp1, 0), "#include")))
				{
					char*		pszParentDir;
					char*		pszAbsoluteFile;
					DWORD		dwAbsoluteFileSize;
					int			iPos;


					pszTemp2 = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer) + 1);
					if (pszTemp2 == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate memory)

					iPos = 9; // start after #include
					hr = StringGetBetween(pszStringBuffer, "\"", "\"", TRUE, FALSE,
										FALSE, TRUE, TRUE, &iPos, NULL, pszTemp2,
										NULL);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't get string between quotes in %s (line %u)!",
							2, pszStringBuffer, dwRealLineNumber);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't get string in quotes)

					if (iPos < 0)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "WARNING: Didn't find file to include (no quotes) in \"%s\" (line %u)!  Ignoring.",
							2, pszStringBuffer, dwRealLineNumber);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
					} // end if (didn't find quotes)
					else
					{
						pszParentDir = (char*) LocalAlloc(LPTR, strlen(szFilePath) + 1);
						if (pszParentDir == NULL)
						{
							hr = E_OUTOFMEMORY;
							goto DONE;
						} // end if (couldn't allocate memory)
						FileGetParentDirFromPath(szFilePath, pszParentDir);

						// If there isn't an explicit parent directory, use the current one
						if (strcmp(pszParentDir, "") == 0)
							strcpy(pszParentDir, ".");


						// Ignore error, assume BUFFER_TOO_SMALL
						FileCombinePaths(pszParentDir, pszTemp2, NULL,
										&dwAbsoluteFileSize);

						pszAbsoluteFile = (char*) LocalAlloc(LPTR, dwAbsoluteFileSize);
						if (pszAbsoluteFile == NULL)
						{
							LocalFree(pszParentDir);
							pszParentDir = NULL;
							hr = E_OUTOFMEMORY;
							goto DONE;
						} // end if (couldn't allocate memory)


						hr = FileCombinePaths(pszParentDir, pszTemp2,
											pszAbsoluteFile, &dwAbsoluteFileSize);
						if (hr != S_OK)
						{
							#ifndef NO_TNCOMMON_DEBUG_SPEW
							DPL(0, "Couldn't combine \"%s\" and \"%s\" (line %u)!",
								3, pszParentDir, pszTemp2, dwRealLineNumber);
							#endif // ! NO_TNCOMMON_DEBUG_SPEW
							LocalFree(pszParentDir);
							pszParentDir = NULL;
							LocalFree(pszAbsoluteFile);
							pszAbsoluteFile = NULL;
							goto DONE;
						} // end if (couldn't get string in quotes)

						LocalFree(pszTemp2);
						pszTemp2 = NULL;

						LocalFree(pszParentDir);
						pszParentDir = NULL;


						// Recursively read in this new file we have.
						hr = FileReadTextFile(pszAbsoluteFile, dwOptions,
											pfnReadLine, pvContext);
						if (hr != S_OK)
						{
							#ifndef NO_TNCOMMON_DEBUG_SPEW
							DPL(0, "Couldn't read subfile \"%s\" (line %u)!",
								2, pszAbsoluteFile, dwRealLineNumber);
							#endif // ! NO_TNCOMMON_DEBUG_SPEW
							LocalFree(pszAbsoluteFile);
							pszAbsoluteFile = NULL;
							goto DONE;
						} // end if (couldn't get string in quotes)

						LocalFree(pszAbsoluteFile);
						pszAbsoluteFile = NULL;

						LocalFree(pszTemp1);
						pszTemp1 = NULL;

						// We don't want to hand the user this line
						continue;
					} // end else (found quotes)

					LocalFree(pszTemp2);
					pszTemp2 = NULL;
				} // end if (we found a #include)


				// Check for #defines, if allowed
				if ((dwOptions & FILERTFO_ALLOW_POUND_DEFINES) &&
					(StringCmpNoCase(StringGetFieldPtr(pszTemp1, 0), "#define")))
				{
					char*	pszName;
					int		iPos;
					

					pszName = StringGetFieldPtr(pszTemp1, 1);

					//pszTemp2 = NULL;
					if (dwFields >= 2)
					{
						// Find where the name starts in the string (skipping past
						// #define).
						iPos = StringContainsString(pszStringBuffer, pszName, TRUE, 8);
						if (iPos < 0)
						{
							#ifndef NO_TNCOMMON_DEBUG_SPEW
							DPL(0, "Couldn't find name string %s in \"%s\" (line %u)!",
								3, pszName, pszStringBuffer, dwRealLineNumber);
							#endif // ! NO_TNCOMMON_DEBUG_SPEW
							hr = E_FAIL;
							goto DONE;
						} // end if (couldn't find string)

						// Move past the name
						iPos += strlen(pszName);

						pszTemp2 = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer + iPos) + 1);
						if (pszTemp2 == NULL)
						{
							hr = E_OUTOFMEMORY;
							goto DONE;
						} // end if (couldn't allocate memory)

						strcpy(pszTemp2, (pszStringBuffer + iPos));

						// Get rid of the surrounding whitespace
						StringPopLeadingChars(pszTemp2, " \t", TRUE);
						StringPopTrailingChars(pszTemp2, " \t", TRUE);
					} // end if (there's a value)

#ifndef _XBOX // SetEnvironmentVariable not supported
					// Build an environment variable.  If the user didn't specify a
					// value, use an empty string
					if (! SetEnvironmentVariable(pszName, ((pszTemp2 == NULL) ? "" : pszTemp2)))
					{
						hr = GetLastError();
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't set environment variable \"%s\" to \"%s\" (line %u)!",
							3, pszName, pszTemp2, dwRealLineNumber);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't set an environment variable)
#else // ! XBOX
#pragma TODO(tristanj, "What effect does not setting the defined environment variable have?")
#endif // XBOX
					if (pszTemp2 != NULL)
					{
						LocalFree(pszTemp2);
						pszTemp2 = NULL;
					} // end if (allocated a value)

					LocalFree(pszTemp1);
					pszTemp1 = NULL;

					// We don't want to hand the user this line
					continue;
				} // end if (we found a #define)


				// Check for #pragma message()s
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				if ((StringCmpNoCase(StringGetFieldPtr(pszTemp1, 0), "#pragma")) &&
					(StringStartsWith(StringGetFieldPtr(pszTemp1, 1), "message", FALSE)) &&
					(StringEndsWith(pszStringBuffer, ")", TRUE)))
				{
					char	szLineNumber[64];


					pszTemp2 = (char*) LocalAlloc(LPTR, strlen(pszStringBuffer) + 1);
					if (pszTemp2 == NULL)
					{
						hr = E_OUTOFMEMORY;
						goto DONE;
					} // end if (couldn't allocate memory)

					iPos = 16; // start after #pragma message
					hr = StringGetBetween(pszStringBuffer, "\"", "\"", TRUE, FALSE,
										FALSE, TRUE, TRUE, &iPos, NULL, pszTemp2,
										NULL);
					if (hr != S_OK)
					{
						//#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't get string between quotes in %s (line %u)!",
							2, pszStringBuffer, dwRealLineNumber);
						//#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't get string in quotes)

					// Use the somewhat modified line number, not the real one.  It's
					// actually the top most line number for the current (possible)
					// multi-line.
					wsprintf(szLineNumber, "Line %u", fli.dwLineNumber);

					// We explicitly expand the macro so we can pass in the module and
					// section we want to see.
					TNInternalDebugPrepLog(szFilePath, szLineNumber);
					TNInternalDebugPrintLine(0, pszTemp2, 0);


					LocalFree(pszTemp2);
					pszTemp2 = NULL;

					LocalFree(pszTemp1);
					pszTemp1 = NULL;

					// We don't want to hand the user this line
					continue;
				} // end if (is a #pragma message)
				#endif // ! NO_TNCOMMON_DEBUG_SPEW


				LocalFree(pszTemp1);
				pszTemp1 = NULL;
			} // end if (it appears to be a valid pound directive)
		} // end if (it's starts with a pound char)


		// Call the user back with this line
		hr = pfnReadLine(pszStringBuffer, &fli, pvContext, &fStopReading);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "User's ReadLine callback failed (line %u)!",
				1, dwRealLineNumber);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (user's callback failed)

		if (fStopReading)
		{
			// Okay, do what the user wants.
			break;
		} // end if (the user wants to stop reading)

	} // end do (while not at the end of the file)
	while (! fEndOfFile);

	hr = S_OK;


DONE:

	if (hFile != INVALID_HANDLE_VALUE)
	{
		CloseHandle(hFile);
		hFile = INVALID_HANDLE_VALUE;
	} // end if (opend the file)

	if (pszStringBuffer != NULL)
	{
		LocalFree(pszStringBuffer);
		pszStringBuffer = NULL;
	} // end if (allocated a temp string)

	if (fli.pszCurrentSection != NULL)
	{
		LocalFree(fli.pszCurrentSection);
		fli.pszCurrentSection = NULL;
	} // end if (previously in section)

	if (fli.pszCurrentSubsection != NULL)
	{
		LocalFree(fli.pszCurrentSubsection);
		fli.pszCurrentSubsection = NULL;
	} // end if (previously in subsection)

	if (fli.pszCurrentSubsubsection != NULL)
	{
		LocalFree(fli.pszCurrentSubsubsection);
		fli.pszCurrentSubsubsection = NULL;
	} // end if (previously in subsubsection)

	if (pszTemp1 != NULL)
	{
		LocalFree(pszTemp1);
		pszTemp1 = NULL;
	} // end if (allocated a temp string)

	if (pszTemp2 != NULL)
	{
		LocalFree(pszTemp2);
		pszTemp2 = NULL;
	} // end if (allocated a temp string)

	return (hr);
} // FileReadTextFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // not supported on XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetNameWithoutExtension()"
//==================================================================================
// FileGetNameWithoutExtension
//----------------------------------------------------------------------------------
//
// Description: Get the root name of the file described (minus the ".xxx" extension)
//				and puts it in the result string.
//				If no extension existed, then no period is added (fIncludeDelimiter
//				is ignored).
//
// Arguments:
//	char* szPath			Path (absolute or relative) to file to get.
//	char* szResult			Buffer to store resulting root name in.
//	BOOL fIncludeDelimiter	Whether to include the extension delimiter character
//							(the period '.') in the result string or not.
//
// Returns: None.
//==================================================================================
void FileGetNameWithoutExtension(char* szPath, char* szResult, BOOL fIncludeDelimiter)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];


	_splitpath(szPath, szDrive, szDir, szFname, szExt);

	if ((fIncludeDelimiter) && (strlen(szExt) > 0))
		strcat(szFname, ".");

	strcpy(szResult, szFname);
} // FileGetNameWithoutExtension
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // not supported on XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetExtension()"
//==================================================================================
// FileGetExtension
//----------------------------------------------------------------------------------
//
// Description: Get the ".xxx" extension of the file described (without the period)
//				and puts it in the result string. 
//				If no extension existed, the resulting buffer will be empty.
//
// Arguments:
//	char* szPath			Path (absolute or relative) to item to get.
//	char* szResult			Buffer to store resulting extension in.
//	BOOL fIncludeDelimiter	Whether to include the extension delimiter character
//							(the period '.') in the result string or not.
//
// Returns: None.
//==================================================================================
void FileGetExtension(char* szPath, char* szResult, BOOL fIncludeDelimiter)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];


	_splitpath(szPath, szDrive, szDir, szFname, szExt);

	if (! fIncludeDelimiter)
		StringPopLeadingChars(szExt, ".", TRUE);

	strcpy(szResult, szExt);
} // FileGetExtension
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // not supported on Xbox
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetLastItemInPath()"
//==================================================================================
// FileGetLastItemInPath
//----------------------------------------------------------------------------------
//
// Description: Finds the last item (subdirectory or file) in a path and puts it in
//				the result string. 
//
// Arguments:
//	char* szPath	Path (absolute or relative) to item to get.
//	char* szResult	Place to store result
//
// Returns: None.
//==================================================================================
void FileGetLastItemInPath(char* szPath, char* szResult)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];


	_splitpath(szPath, szDrive, szDir, szFname, szExt);
	strcpy(szResult, szFname);
	strcat(szResult, szExt);
} // FileGetLastItemInPath
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetParentDirFromPath()"
//==================================================================================
// FileGetParentDirFromPath
//----------------------------------------------------------------------------------
//
// Description: Finds the last item (subdirectory or file)'s parent directory in a
//				path and puts the path to it in the result string. 
//
// Arguments:
//	char* szPath	Path (absolute or relative) to item to get.
//	char* szResult	Place to store result
//
// Returns: None.
//==================================================================================
void FileGetParentDirFromPath(char* szPath, char* szResult)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];


	_splitpath(szPath, szDrive, szDir, szFname, szExt);
	strcpy(szResult, szDrive);
	strcat(szResult, szDir);
} // FileGetParentDirFromPath
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetDriveFromPath()"
//==================================================================================
// FileGetDriveFromPath
//----------------------------------------------------------------------------------
//
// Description: Finds the the drive in a path and puts it in the result string. 
//
// Arguments:
//	char* szPath	Path to item.
//	char* szResult	Place to store result.
//
// Returns: None.
//==================================================================================
void FileGetDriveFromPath(char* szPath, char* szResult)
{
	char szDrive[_MAX_DRIVE];
	char szDir[_MAX_DIR];
	char szFname[_MAX_FNAME];
	char szExt[_MAX_EXT];


	_splitpath(szPath, szDrive, szDir, szFname, szExt);
	strcpy(szResult, szDrive);
} // FileGetDriveFromPath
#undef DEBUG_SECTION
#define DEBUG_SECTION	""





#ifndef _XBOX // no file reading
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCombinePaths()"
//==================================================================================
// FileCombinePaths
//----------------------------------------------------------------------------------
//
// Description: Returns a new absolute path given a base path and a relative path to
//				to add.
//				If pszNewPath is NULL, the DWORD pointed to by pdwNewPathSize is
//				filled with the size required, including NULL termination, and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* szBaseDir			Base directory (relative or absolute path) to start
//							with.
//	char* szAdditivePath	Relative path to combine with base path.
//	char* pszNewPath		Pointer to place to store result, or NULL to retrieve
//							size required.
//	DWORD* pdwNewPathSize	Pointer to size of buffer, or place to store size
//							required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileCombinePaths(char* szBaseDir, char* szAdditivePath, char* pszNewPath,
						DWORD* pdwNewPathSize)
{
	HRESULT		hr;
	char		szTemp[MAX_PATH];
	char*		pszOldCurrent = NULL;
	DWORD		dwOldCurrentSize;


	// Save the previous current directory.
	dwOldCurrentSize = GetCurrentDirectory(0, NULL);
	pszOldCurrent = (char*) LocalAlloc(LPTR, dwOldCurrentSize);
	if (pszOldCurrent == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetCurrentDirectory(dwOldCurrentSize, pszOldCurrent);


	// Temporarily move to the given base directory.
	if (! SetCurrentDirectory(szBaseDir))
	{
		hr = GetLastError();
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't temporarily set the current directory to \"%s\"!",
			1, szBaseDir);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't set current directory)


	// Convert the additive path to an absolute path based off of the given base.
	_fullpath(szTemp, szAdditivePath, MAX_PATH);


	// Revert to the old current directory.
	if (! SetCurrentDirectory(pszOldCurrent))
	{
		hr = GetLastError();
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't reset the current directory to the original (\"%s\")!",
			1, pszOldCurrent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't set current directory)


	if (pszNewPath == NULL)
	{
		(*pdwNewPathSize) = strlen(szTemp) + 1;
		hr = ERROR_BUFFER_TOO_SMALL;
		goto DONE;
	} // end if (don't have a buffer)

	strcpy(pszNewPath, szTemp);

	hr = S_OK;


DONE:

	if (pszOldCurrent != NULL)
	{
		LocalFree(pszOldCurrent);
		pszOldCurrent = NULL;
	} // end if (allocated memory)

	return (hr);
} // FileCombinePaths
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // No GetFileAttributes
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetAttributes()"
//==================================================================================
// FileGetAttributes
//----------------------------------------------------------------------------------
//
// Description: Sets the booleans passed in to TRUE if they are true for the file
//				specified, FALSE if not.  You can pass NULL for any value you don't
//				want.
//
// Arguments:
//	char* szItemPath		Path to item to check.
//	BOOL* pfIsDirectory		Boolean to set to whether the item is a directory or not.
//	BOOL* pfIsReadOnly		Boolean to set to whether the item is read only or not.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileGetAttributes(char* szItemPath, BOOL* pfIsDirectory, BOOL* pfIsReadOnly)
{
	DWORD		dwAttributes;



	if (pfIsDirectory != NULL)
		(*pfIsDirectory) = FALSE;
	if (pfIsReadOnly != NULL)
		(*pfIsReadOnly) = FALSE;

	dwAttributes = GetFileAttributes(szItemPath);
	if (dwAttributes == 0xFFFFFFFF)
		return (GetLastError());


	if (pfIsDirectory != NULL)
		(*pfIsDirectory) = (dwAttributes & FILE_ATTRIBUTE_DIRECTORY) ? TRUE : FALSE;

	if (pfIsReadOnly != NULL)
		(*pfIsReadOnly) = (dwAttributes & FILE_ATTRIBUTE_READONLY) ? TRUE : FALSE;

	return (S_OK);
} // FileGetAttributes
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // No GetFileAttributes
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileSetAttributes()"
//==================================================================================
// FileSetAttributes
//----------------------------------------------------------------------------------
//
// Description: Sets the file specified's attributes to the values passed in.  You
//				can pass NULL for any value you don't want to change.
//
// Arguments:
//	char* szItemPath		Path to item to check.
//	BOOL* pfReadOnly		Whether to set the item to be read only or not.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileSetAttributes(char* szItemPath, BOOL* pfReadOnly)
{
	DWORD		dwAttributes;



	dwAttributes = GetFileAttributes(szItemPath);
	if (dwAttributes == 0xFFFFFFFF)
		return (GetLastError());

	if (pfReadOnly != NULL)
	{

		 // If the flag exists, take it out.
		if (dwAttributes & FILE_ATTRIBUTE_READONLY)
			dwAttributes = dwAttributes & ~(FILE_ATTRIBUTE_READONLY);

		// If the flag is supposed to be there, put it back in.
		if ((*pfReadOnly))
			dwAttributes = dwAttributes | FILE_ATTRIBUTE_READONLY;
	} // end if (we're setting the read only flag)

	if (! SetFileAttributes(szItemPath, dwAttributes))
		return (GetLastError());

	return (S_OK);
} // FileSetAttributes
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no file stuff supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetRealFilePath()"
//==================================================================================
// FileGetRealFilePath
//----------------------------------------------------------------------------------
//
// Description: If fFileAlreadyExists is TRUE, returns the full path to the first
//				matching item the system would find based on its search rules:
//					The relative path given, if there are backslashes.  Otherwise,
//						1. The directory from which the application loaded.
//						2. The current directory.
//						3. The System directory(ies).
//						4. The Windows directory.
//						5. Directories in the PATH environment variable.
//				If no file exists at the location(s), ERROR_FILE_NOT_FOUND is
//				returned.
//				If fFileAlreadyExists is FALSE, the absolute path to the item is
//				returned, using the current directory as a base.
//				If pszResultPath is NULL, then pdwResultPath size is set to the
//				size required (including NULL termination), and
//				ERROR_BUFFER_TOO_SMALL is returned.
//
// Arguments:
//	char* szItemPath			Path to file or directory.
//	BOOL fFileAlreadyExists		If TRUE, the places listed above will be searched
//								for a file matching that description.  If FALSE, and
//								the item path isn't already an absolute path, the
//								current directory is used.
//	char* pszResultPath			Place to store resulting absolute path, or NULL to
//								retrieve size.
//	DWORD* pdwResultPathSize	Pointer to size of result buffer, or place to store
//								size required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileGetRealFilePath(char* szItemPath, BOOL fFileAlreadyExists,
							char* pszResultPath, DWORD* pdwResultPathSize)
{
	HRESULT				hr = S_OK;
	char				szTemp[MAX_PATH];
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		founddata;
	char*				pszTempDir = NULL;
	char*				pszPathVar = NULL;
	DWORD				dwSize;
	DWORD				dwNumPaths;
	DWORD				dwTemp;



	// If the path contains backslashes, get the absolute path to that relative one.
	if (StringContainsChar(szItemPath, '\\', 0, TRUE) >= 0)
	{
		_fullpath(szTemp, szItemPath, MAX_PATH);

		if (fFileAlreadyExists)
		{
			hFind = FindFirstFile(szTemp, &founddata);
			if (hFind != INVALID_HANDLE_VALUE)
			{
				// We found it, we're done.
				goto DONE;
			} // end if (couldn't find first file)

			hr = GetLastError();

			if (hr == S_OK)
				hr = E_FAIL;
		} // end if (file should already exist)

		// We can't do any other types of searching if it's not a plain item.
		goto DONE;
	} // end if (path contains backslashes)


	if (fFileAlreadyExists)
	{

#pragma TODO(vanceo, "Get path to current app")

	} // end if (file should already exist)



	// Get the current directory and try that.

	dwSize = GetCurrentDirectory(0, NULL);
	pszTempDir = (char*) LocalAlloc(LPTR, dwSize);
	if (pszTempDir == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetCurrentDirectory(dwSize, pszTempDir);

	strcpy(szTemp, pszTempDir);
	if (! StringEndsWith(pszTempDir, "\\", TRUE))
		strcat(szTemp, "\\");
	strcat(szTemp, szItemPath);

	LocalFree(pszTempDir);
	pszTempDir = NULL;


	// If we're not checking if the file already existed, then we're done (we built
	// the path based off the current directory).  Skip everything else.
	if (! fFileAlreadyExists)
		goto DONE;


	hFind = FindFirstFile(szTemp, &founddata);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// We found it, we're done.
		goto DONE;
	} // end if (couldn't find first file)



	// Get the system directory and try that.

	dwSize = GetSystemDirectory(NULL, 0);
	pszTempDir = (char*) LocalAlloc(LPTR, dwSize);
	if (pszTempDir == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetSystemDirectoryA(pszTempDir, dwSize);

	strcpy(szTemp, pszTempDir);
	if (! StringEndsWith(pszTempDir, "\\", TRUE))
		strcat(szTemp, "\\");
	strcat(szTemp, szItemPath);

	LocalFree(pszTempDir);
	pszTempDir = NULL;

	hFind = FindFirstFile(szTemp, &founddata);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// We found it, we're done.
		goto DONE;
	} // end if (couldn't find first file)



#pragma TODO(vanceo, "Check 16-bit system directory on NT")




	// Get the Windows directory and try that.

	dwSize = GetWindowsDirectory(NULL, 0);
	pszTempDir = (char*) LocalAlloc(LPTR, dwSize);
	if (pszTempDir == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetWindowsDirectoryA(pszTempDir, dwSize);

	strcpy(szTemp, pszTempDir);
	if (! StringEndsWith(pszTempDir, "\\", TRUE))
		strcat(szTemp, "\\");
	strcat(szTemp, szItemPath);

	LocalFree(pszTempDir);
	pszTempDir = NULL;

	hFind = FindFirstFile(szTemp, &founddata);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		// We found it, we're done.
		goto DONE;
	} // end if (couldn't find first file)



	// Get the PATH environment variable

	dwSize = GetEnvironmentVariable("PATH", NULL, 0);
	pszPathVar = (char*) LocalAlloc(LPTR, dwSize);
	if (pszPathVar == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)
	GetEnvironmentVariable("PATH", pszPathVar, dwSize);

	// It's okay to pass the string to split in as the output buffer because we don't
	// care if it gets destroyed.
	dwNumPaths = StringSplitIntoFields(pszPathVar, ";", pszPathVar, &dwSize);

	// Loop through all the paths in the environment variable
	for(dwTemp = 0; dwTemp < dwNumPaths; dwTemp++)
	{
		pszTempDir = StringGetFieldPtr(pszPathVar, dwTemp);

		strcpy(szTemp, pszTempDir);
		if (! StringEndsWith(pszTempDir, "\\", TRUE))
			strcat(szTemp, "\\");
		strcat(szTemp, szItemPath);

		pszTempDir = NULL;


		hFind = FindFirstFile(szTemp, &founddata);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			// We found it, we're done.
			goto DONE;
		} // end if (couldn't find first file)
	} // end for (each path)


	// If we got here, it means we didn't find it.
	hr = ERROR_FILE_NOT_FOUND;


DONE:

	if (hFind != INVALID_HANDLE_VALUE)
	{
		FindClose(hFind); // ignore error
		hFind = INVALID_HANDLE_VALUE;
	} // end if (have find still open)

	if (pszPathVar != NULL)
	{
		LocalFree(pszPathVar);
		pszPathVar = NULL;
	} // end if (allocated string)

	if (pszTempDir != NULL)
	{
		LocalFree(pszTempDir);
		pszTempDir = NULL;
	} // end if (allocated string)

	if (hr == S_OK)
	{
		if (pszResultPath == NULL)
		{
			hr = ERROR_BUFFER_TOO_SMALL;
			(*pdwResultPathSize) = strlen(szTemp) + 1;
		} // end if (there's no buffer to copy to)
		else
		{
			strcpy(pszResultPath, szTemp);
		} // end else (there is a buffer to copy to)
	} // end if (we found the item)

	return (hr);
} // FileGetRealFilePath
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX





#ifndef _XBOX // not supported on Xbox
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileEnsureDirectoryExists()"
//==================================================================================
// FileEnsureDirectoryExists
//----------------------------------------------------------------------------------
//
// Description: Makes sure the specified directory and all parent directories
//				exist, creating them if necessary.
//
// Arguments:
//	char* szDirPath		Directory path to ensure.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileEnsureDirectoryExists(char* szDirPath)
{
	HRESULT		hr = S_OK;
	int			i;



	//BUGBUG what about paths enclosed in quotes?

	// If the path is a share, skip past the machine name and share directory.  If
	// it's local, skip past the drive.
	if (StringStartsWith(szDirPath, "\\\\", TRUE))
	{
		i = StringContainsChar(szDirPath, '\\',  TRUE, 2);
		if (i < 0)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't find share directory in \"%s\"!", 1, szDirPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			hr = ERROR_INVALID_PARAMETER;
			goto DONE;
		} // end if (there isn't a share directory)
	} // end if (the path is a share)
	else
	{
		i = 2; // start past the drive letter, and colon, and backslash
	} // end else (the path is not a share)


	// Keep looping through the path, making sure each directory exists.
	do
	{
		i = StringContainsChar(szDirPath, '\\', TRUE, i + 1);

		// If we found a directory delimiter, temporarily terminate the string at
		// that directory level
		if (i >= 0)
			szDirPath[i] = '\0';


		// Make sure the directory so far exists (if it already did, we're cool).
		if (_mkdir(szDirPath) == -1)
		{
			//hr = errno;
			hr = _doserrno;

			if (hr == ERROR_ALREADY_EXISTS)
				hr = S_OK;
		} // end if (failed creating directory)

		// Restore the path back to it's normal form.
		if (i >= 0)
			szDirPath[i] = '\\';

		// If it failed (but wasn't because it already exists), print out the
		// error and continue.
		// NOTE: This assumes the same error will happen again for subsequent
		// directories, so that the final directory attempted to be created will
		// also fail, and thus hr will not be S_OK when the function exits.
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(1, "NOTE: Failed creating directory \"%z\".  %e",
				3, szDirPath, i, hr);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
		} // end if (unknown failure)
	} // end do
	while (i > -1);


DONE:

	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(0, "Failed ensuring directory \"%z\"!", 1, szDirPath);
	#endif // ! NO_TNCOMMON_DEBUG_SPEW

	return (hr);
} // FileEnsureDirectoryExists
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX





#ifndef _XBOX // no file stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCompareFiles()"
//==================================================================================
// FileCompareFiles
//----------------------------------------------------------------------------------
//
// Description: Compares two files, and returns TRUE if they are the same file,
//				FALSE if not or an error occurred.  The files are compared by
//				dir/not dir attribute, modification date, file size, and optionally,
//				creation date.
//				NOTE: The filename is not checked.
//
// Arguments:
//	char* szFirstPath			Path to first item.
//	char* szSecondPath			Path to second item.
//	BOOL fCheckCreationTime		Whether file creation times must match or not.
//
// Returns: TRUE if they are the same file, FALSE otherwise.
//==================================================================================
BOOL FileCompareFiles(char* szFirstPath, char* szSecondPath, BOOL fCheckCreationTime)
{
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	HRESULT				hr;
	#endif // ! NO_TNCOMMON_DEBUG_SPEW
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		firstdata;
	WIN32_FIND_DATA		seconddata;



	ZeroMemory(&firstdata, sizeof (WIN32_FIND_DATA));
	ZeroMemory(&seconddata, sizeof (WIN32_FIND_DATA));


	hFind = FindFirstFile(szFirstPath, &firstdata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		hr = GetLastError();
		DPL(0, "Couldn't find first item (%s)!  %e", 2, szFirstPath, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
		return (FALSE);
	} // end if (couldn't find first file)

	if (! FindClose(hFind))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		hr = GetLastError();
		DPL(0, "Couldn't close first find!  %e", 1, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (couldn't close find)
	//hFind = INVALID_HANDLE_VALUE;


	hFind = FindFirstFile(szSecondPath, &seconddata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		/*
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		hr = GetLastError();
		DPL(0, "Couldn't find second item (%s)!  %e", 2, szSecondPath, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		*/
		return (FALSE);
	} // end if (couldn't find first file)

	if (! FindClose(hFind))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		hr = GetLastError();
		DPL(0, "Couldn't close second find!  %e", 1, hr);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		return (FALSE);
	} // end if (couldn't close find)
	hFind = INVALID_HANDLE_VALUE;



	if ((firstdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != (seconddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		return (FALSE);
	} // end if (directory attributes don't match)

	if (memcmp(&(firstdata.ftLastWriteTime), &(seconddata.ftLastWriteTime), sizeof (FILETIME)) != 0)
	{
		return (FALSE);
	} // end if (modification times don't match)

	if ((firstdata.nFileSizeHigh != seconddata.nFileSizeHigh) ||
		(firstdata.nFileSizeLow != seconddata.nFileSizeLow))
	{
		return (FALSE);
	} // end if (file sizes don't match)

	if (fCheckCreationTime)
	{
		if (memcmp(&(firstdata.ftCreationTime), &(seconddata.ftCreationTime), sizeof (FILETIME)) != 0)
		{
			return (FALSE);
		} // end if (creation times don't match)
	} // end if (file creation time is significant)

	return (TRUE);
} // FileCompareFiles
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // no file stuff supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCountFilesInDir()"
//==================================================================================
// FileCountFilesInDir
//----------------------------------------------------------------------------------
//
// Description: Counts the number of files in the given directory (and optionally,
//				its subdirectories).  Note that counting files in subdirectories
//				does not include the subdirectories themselves as part of that
//				number.
//				The value pointed to by lpdwCount must be initialized to zero.
//
// Arguments:
//	char* szDirPath				Path to directory containing files to count.
//	BOOL fCountSubdirectories	Whether to count files inside subdirectories as well.
//	DWORD* pdwCount			Place to store total.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileCountFilesInDir(char* szDirPath, BOOL fCountSubdirectories,
							DWORD* pdwCount)
{
	HRESULT				hr;
	HRESULT				temphr;
	DWORD				dwSize;
	char*				pszSearchPattern = NULL;
	DWORD				dwSubItemPathSize = 0;
	char*				pszSubItemPath = NULL;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		finddata;


	if (szDirPath == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed invalid/NULL path!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed bad parameter)


	dwSize = strlen(szDirPath) + 4; // "*.*" + NULL termination

	// Make sure it ends in a backslash
	if (! StringEndsWith(szDirPath, "\\", TRUE))
		dwSize++;

	pszSearchPattern = (char*) LocalAlloc(LPTR, dwSize);
	if (pszSearchPattern == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszSearchPattern, szDirPath);

	if (! StringEndsWith(szDirPath, "\\", TRUE))
		strcat(pszSearchPattern, "\\");

	strcat(pszSearchPattern, "*.*");


	ZeroMemory(&finddata, sizeof (WIN32_FIND_DATA));

	hFind = FindFirstFile(pszSearchPattern, &finddata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't find first item (%s)!", 1, pszSearchPattern);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't find first file)

	// This assumes the first item returned is the "." special directory
	while (FindNextFile(hFind, &finddata))
	{
		// Skip the ".." special directory, too.
		if (strcmp(finddata.cFileName, "..") == 0)
			continue;


		// Build the current source path

		// + room for possible extra backslash, and NULL termination.
		dwSize = strlen(szDirPath) + strlen(finddata.cFileName) + 2;

		// Make sure it ends in a backslash
		if (! StringEndsWith(szDirPath, "\\", TRUE))
			dwSize++;

		if (dwSize >= dwSubItemPathSize)
		{
			if (pszSubItemPath != NULL)
			{
				LocalFree(pszSubItemPath);
				pszSubItemPath = NULL;
			} // end if (had buffer before)

			pszSubItemPath = (char*) LocalAlloc(LPTR, dwSize);
			if (pszSubItemPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			dwSubItemPathSize = dwSize;
		} // end if (the buffer wasn't big enough before)

		strcpy(pszSubItemPath, szDirPath);

		if (! StringEndsWith(szDirPath, "\\", TRUE))
			strcat(pszSubItemPath, "\\");

		strcat(pszSubItemPath, finddata.cFileName);


		// Handle the item

		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (fCountSubdirectories)
			{
				// Recursively call ourselves
				hr = FileCountFilesInDir(pszSubItemPath, fCountSubdirectories,
										pdwCount);
				if (hr != S_OK)
				{
					goto DONE;
				} // end if (a recursive call failed)
			} // end if (should apply to sub directories)
		} // end if (it's a directory)
		else
		{
			// Hey, here's a file, count it.
			(*pdwCount)++;
		} // end else (it's not a directory)
	} // end while (there are more items)

	// There was an error, so just make sure it's the right one.
	hr = GetLastError();

	if (hr == ERROR_NO_MORE_FILES)
		hr = S_OK;

	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed while searching for next item with \"%s\"!",
			1, pszSearchPattern);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end else (got unexpected error)


	if (! FindClose(hFind))
	{
		hr = GetLastError();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't close find!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't close find)
	hFind = INVALID_HANDLE_VALUE;


DONE:

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (! FindClose(hFind))
		{
			temphr = GetLastError();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't close find!  %e", 1, temphr);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			if (hr == S_OK)
				hr = temphr;
		} // end if (couldn't close find)
		hFind = INVALID_HANDLE_VALUE;
	} // end if (there's an open find)

	if (pszSubItemPath != NULL)
	{
		LocalFree(pszSubItemPath);
		pszSubItemPath = NULL;
	} // end if (couldn't allocate memory)

	if (pszSearchPattern != NULL)
	{
		LocalFree(pszSearchPattern);
		pszSearchPattern = NULL;
	} // end if (couldn't allocate memory)

	return (hr);
} // FileCountFilesInDir
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCopyTree()"
//==================================================================================
// FileCopyTree
//----------------------------------------------------------------------------------
//
// Description: Copies all files (including ones in subdirectories) from the source
//				path to the destination path.  The heirarchical structure is created
//				if it doesn't exist.
//
// Arguments:
//	char* szSourceDirPath		Path to root source directory.
//	char* szDestDirPath			Path to root destination directory.
//	DWORD dwOptions				Options for copying the tree, see FILECTO_xxx.
//	PLSTRINGLIST pSkipFiles		Optional list of filepaths (may use wildcard
//								charactesr) to skip, see linkedstr.cpp for more.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileCopyTree(char* szSourceDirPath, char* szDestDirPath, DWORD dwOptions,
					 PLSTRINGLIST pSkipFiles)
{
	HRESULT			hr;
	BOOL			fChoppedOffSourceBackslash = FALSE;
	BOOL			fChoppedOffDestBackslash = FALSE;
	COPYTREEDATA	ctd;
	HANDLE			hProgressThread = NULL;


	ZeroMemory(&ctd, sizeof (COPYTREEDATA));
	//ctd.fSkip = FALSE;
	ctd.dwOptions = dwOptions;
	ctd.pSkipFiles = pSkipFiles;
	//ctd.dwNumFilesHandled = 0;
	//ctd.dwTotalNumFiles = 0;
	//ctd.hProgressWindow = NULL;


	if ((szSourceDirPath == NULL) || (szDestDirPath == NULL))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed invalid/NULL parameters!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed bad parameters)


	// Make sure the caller didn't specify more than one of these, cause that's
	// bogus.
	if ((dwOptions & FILECTO_FAIL_IF_EXISTS) &&
		(dwOptions & FILECTO_FORCE_COPY_IF_SAME) &&
		(dwOptions & FILECTO_EMPTY_COPIED_DIRS))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Must specify only one of FILECTO_FAIL_IF_EXISTS, FILECTO_FORCE_COPY_IF_SAME and FILECTO_EMPTY_COPIED_DIRS!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed bad parameters)


#ifndef NO_FILEUTILS_COPYTREEPROGRESS
	if (dwOptions & FILECTO_DISPLAY_PROGRESS_BAR)
	{
		DWORD	dwThreadID;


		hr = FileCountFilesInDir(szSourceDirPath, TRUE, &(ctd.dwTotalNumFiles));
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Failed counting number of files in \"%s\"!",
				1, szSourceDirPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (failed to count files)

		ctd.hProgressThreadStartedEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
		if (ctd.hProgressThreadStartedEvent == NULL)
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't create progress thread started event!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't create thread)

		hProgressThread = CreateThread(NULL, 0, CopyTreeProgressThreadProc, &ctd,
										0, &dwThreadID);
		if (hProgressThread == NULL)
		{
			hr = GetLastError();
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't create CopyTreeProgress thread!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't create thread)

		hr = WaitForSingleObject(ctd.hProgressThreadStartedEvent, INFINITE);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// Thread is ready
				hr = S_OK;
			  break;

			default:
				// Something wacky happened
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "WaitForSingleObject for progress thread to startup returned a wacky result!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			  break;
		} // end switch (on waiting for thread to start up)

		CloseHandle(ctd.hProgressThreadStartedEvent);
		ctd.hProgressThreadStartedEvent = NULL;
	} // end if (a progress bar should be displayed
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS

	if (StringEndsWith(szSourceDirPath, "\\", TRUE))
	{
		szSourceDirPath[strlen(szSourceDirPath) - 1] = '\0';
		fChoppedOffSourceBackslash = TRUE;
	} // end if (source path ends in backslash)

	if (StringEndsWith(szDestDirPath, "\\", TRUE))
	{
		szDestDirPath[strlen(szDestDirPath) - 1] = '\0';
		fChoppedOffDestBackslash = TRUE;
	} // end if (dest path ends in backslash)


	hr = CopyTreeInternal(szSourceDirPath, szDestDirPath, &ctd);
	if (hr != S_OK)
	{
		DPL(0, "Failed copying tree \"%s\" to \"%s\"!",
			2, szSourceDirPath, szDestDirPath);
		goto DONE;
	} // end if (failed to copy tree)

	if (hProgressThread != NULL)
	{
		if (ctd.hProgressWindow != NULL)
		{
			// Tell the window to go bye-bye
			PostMessage(ctd.hProgressWindow, WM_CLOSE, 0, 0);
		} // end if (there's a progress window)

		// Wait for thread to die
		hr = WaitForSingleObject(hProgressThread, INFINITE);
		switch (hr)
		{
			case WAIT_OBJECT_0:
				// Thread is gone, get exit code
				if (! GetExitCodeThread(hProgressThread, (LPDWORD) (&hr)))
				{
					hr = GetLastError();
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't get exit code for progress thread!", 0);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
					goto DONE;
				} // end if (couldn't get thread exit code)
			  break;

			default:
				// Something wacky happened
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "WaitForSingleObject for progress thread to startup returned a wacky result!", 0);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			  break;
		} // end switch (on waiting for thread to start up)

		CloseHandle(hProgressThread);
		hProgressThread = NULL;
	} // end if (there's an event)


DONE:

	if (ctd.hProgressThreadStartedEvent != NULL)
	{
		CloseHandle(ctd.hProgressThreadStartedEvent);
		ctd.hProgressThreadStartedEvent = NULL;
	} // end if (there's an event)

	if (fChoppedOffSourceBackslash)
	{
		szSourceDirPath[strlen(szSourceDirPath)] = '\\';
		fChoppedOffSourceBackslash = FALSE;
	} // end if (couldn't allocate memory)

	if (fChoppedOffDestBackslash)
	{
		szDestDirPath[strlen(szDestDirPath)] = '\\';
		fChoppedOffDestBackslash = FALSE;
	} // end if (couldn't allocate memory)

	return (hr);
} // FileCopyTree
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // no file copy stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCleanDirectory()"
//==================================================================================
// FileCleanDirectory
//----------------------------------------------------------------------------------
//
// Description: Removes all files from the given directory.  If fDeleteDirectory is
//				TRUE, the directory will be deleted after cleaning. If
//				fApplyToSubDirs is TRUE, all files from subdirectories are deleted
//				(and the subdirs are removed, if fDeleteDirectory is specified).
//				If fDeleteDirectory is TRUE, but fApplyToSubDirs is FALSE and there
//				are subdirectories, this call will fail when trying to remove the
//				directory.
//				If fDeleteDirectory is FALSE, then this call will fail if the
//				directory doesn't exist.
//
// Arguments:
//	char* szDirPath			Path to root source directory.
//	BOOL fDeleteDirectory	Path to root destination directory.
//	BOOL fApplyToSubDirs	Whether to do the same thing to any subdirectories.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileCleanDirectory(char* szDirPath, BOOL fDeleteDirectory, BOOL fApplyToSubDirs)
{
	HRESULT				hr;
	HRESULT				temphr;
	DWORD				dwSize;
	char*				pszSearchPattern = NULL;
	DWORD				dwSubItemPathSize = 0;
	char*				pszSubItemPath = NULL;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		finddata;


	if (szDirPath == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Got passed invalid/NULL path!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got passed bad parameter)


	dwSize = strlen(szDirPath) + 4; // "*.*" + NULL termination

	// Make sure it ends in a backslash
	if (! StringEndsWith(szDirPath, "\\", TRUE))
		dwSize++;

	pszSearchPattern = (char*) LocalAlloc(LPTR, dwSize);
	if (pszSearchPattern == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszSearchPattern, szDirPath);

	if (! StringEndsWith(szDirPath, "\\", TRUE))
		strcat(pszSearchPattern, "\\");

	strcat(pszSearchPattern, "*.*");


	ZeroMemory(&finddata, sizeof (WIN32_FIND_DATA));

	hFind = FindFirstFile(pszSearchPattern, &finddata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();


		// If the directory doesn't exist but we would have deleted it anyway, then
		// that's okay.
		if ((hr == ERROR_PATH_NOT_FOUND) && (fDeleteDirectory))
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(1, "\"%s\" doesn't exist, but we would have deleted it anyway.",
				1, szDirPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			hr = S_OK;
		} // end if (directory doesn't exist but we'd delete it)
		else
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't find first item (%s)!", 1, pszSearchPattern);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
		} // end else (other error)

		goto DONE;
	} // end if (couldn't find first file)

	// This assumes the first item returned is the "." special directory
	while (FindNextFile(hFind, &finddata))
	{
		// Skip the ".." special directory, too.
		if (strcmp(finddata.cFileName, "..") == 0)
			continue;


		// Build the current source path

		// + room for possible extra backslash, and NULL termination.
		dwSize = strlen(szDirPath) + strlen(finddata.cFileName) + 2;

		// Make sure it ends in a backslash
		if (! StringEndsWith(szDirPath, "\\", TRUE))
			dwSize++;

		if (dwSize >= dwSubItemPathSize)
		{
			if (pszSubItemPath != NULL)
			{
				LocalFree(pszSubItemPath);
				pszSubItemPath = NULL;
			} // end if (had buffer before)

			pszSubItemPath = (char*) LocalAlloc(LPTR, dwSize);
			if (pszSubItemPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			dwSubItemPathSize = dwSize;
		} // end if (the buffer wasn't big enough before)

		strcpy(pszSubItemPath, szDirPath);

		if (! StringEndsWith(szDirPath, "\\", TRUE))
			strcat(pszSubItemPath, "\\");

		strcat(pszSubItemPath, finddata.cFileName);


		// Handle the item

		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (fApplyToSubDirs)
			{
				// Recursively call ourselves
				hr = FileCleanDirectory(pszSubItemPath, fDeleteDirectory,
										fApplyToSubDirs);
				if (hr != S_OK)
				{
					goto DONE;
				} // end if (a recursive call failed)
			} // end if (should apply to sub directories)
		} // end if (it's a directory)
		else
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(1, "Deleting file \"%s\".", 1, pszSubItemPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			if (! DeleteFile(pszSubItemPath))
			{
				hr = GetLastError();

				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't delete file \"%s\"!", 1, pszSubItemPath);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				goto DONE;
			} // end if (couldn't delete file)
		} // end else (it's not a directory)
	} // end while (there are more items)

	// There was an error, so just make sure it's the right one.
	hr = GetLastError();

	if (hr == ERROR_NO_MORE_FILES)
		hr = S_OK;

	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Failed while searching for next item with \"%s\"!",
			1, pszSearchPattern);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end else (got unexpected error)


	if (! FindClose(hFind))
	{
		hr = GetLastError();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't close find!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't close find)
	hFind = INVALID_HANDLE_VALUE;

	// If we should delete the directory, too.
	if (fDeleteDirectory)
	{
		//if (! DeleteFile(szDirPath))
		if (_rmdir(szDirPath) == -1)
		{
			//hr = GetLastError();
			//hr = errno;
			hr = _doserrno;

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't delete directory \"%s\" (make sure it's not the current working directory)!",
				1, szDirPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			goto DONE;
		} // end if (couldn't delete file)
	} // end if (we should delete the directory)



DONE:

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (! FindClose(hFind))
		{
			temphr = GetLastError();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't close find!  %e", 1, temphr);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			if (hr == S_OK)
				hr = temphr;
		} // end if (couldn't close find)
		hFind = INVALID_HANDLE_VALUE;
	} // end if (there's an open find)

	if (pszSubItemPath != NULL)
	{
		LocalFree(pszSubItemPath);
		pszSubItemPath = NULL;
	} // end if (couldn't allocate memory)

	if (pszSearchPattern != NULL)
	{
		LocalFree(pszSearchPattern);
		pszSearchPattern = NULL;
	} // end if (couldn't allocate memory)

	return (hr);
} // FileCleanDirectory
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // No creating files
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileCreateAndOpenFile()"
//==================================================================================
// FileCreateAndOpenFile
//----------------------------------------------------------------------------------
//
// Description: Creates a file (using the given options) at the path specified and
//				returns the handle to the file in the given pointer.
//				It is up to the caller to close the file when done by calling
//				CloseHandle on the value returned.
//				If file tracking is on, then this item is added to the list.
//
// Arguments:
//	char* szFilepath			Path of text file to create/open.
//	BOOL fFailIfExists			Whether to return an error if a file already exists
//								at that location.
//	BOOL fAppend				Whether to add to the file if one already exists at
//								that location.  Ignored if fFailIfExists is
//								specified.
//	BOOL fInheritableHandle		Whether the file handle can be inherited by child
//								processes or not.
//	HANDLE* phFile				Pointer to receive handle of file.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileCreateAndOpenFile(char* szFilepath, BOOL fFailIfExists, BOOL fAppend,
							BOOL fInheritableHandle, HANDLE* phFile)
{
	HRESULT					hr;
	char*					pszRealFilepath = NULL;
	DWORD					dwSize = 0;
	DWORD					dwDisp;
	SECURITY_ATTRIBUTES		sa;


	if (phFile == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Must pass a valid pointer to store file handle!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (handle pointer is NULL)


	// Ignore error, assume BUFFER_TOO_SMALL.
	FileGetRealFilePath(szFilepath, FALSE, NULL, &dwSize);

	pszRealFilepath = (char*) LocalAlloc(LPTR, dwSize);
	if (pszRealFilepath == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	hr = FileGetRealFilePath(szFilepath, FALSE, pszRealFilepath, &dwSize);
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't get real file path of \"%s\"!", 1, szFilepath);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (failed getting filepath)


	dwDisp = CREATE_ALWAYS;
	if (fFailIfExists)
		dwDisp = CREATE_NEW;
	else if (fAppend)
		dwDisp = OPEN_ALWAYS;



	ZeroMemory(&sa, sizeof (SECURITY_ATTRIBUTES));
	sa.nLength = sizeof (SECURITY_ATTRIBUTES);
	sa.bInheritHandle = fInheritableHandle;
	sa.lpSecurityDescriptor = NULL;


	(*phFile) = CreateFile(pszRealFilepath, GENERIC_WRITE, FILE_SHARE_READ, &sa,
							dwDisp, 0, NULL);
	if ((*phFile) == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't create file \"%s\"!", 1, pszRealFilepath);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't open file)

	// If we should append to the file, move to the end
	if (fAppend)
	{
		if (SetFilePointer((*phFile), 0, NULL, FILE_END) == 0xFFFFFFFF)
		{
			hr = GetLastError();

			if (hr == S_OK)
				hr = E_FAIL;

			CloseHandle((*phFile));
			(*phFile) = INVALID_HANDLE_VALUE;

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't set file pointer to end of file!", 0);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			goto DONE;
		} // end if (couldn't set file pointer)
	} // end if (should append to the file)


	if (g_pTrackedFiles != NULL)
	{
		hr = g_pTrackedFiles->AddString(pszRealFilepath);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "WARNING: Couldn't add \"%s\" to tracked list!",
				1, pszRealFilepath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			goto DONE;
		} // end if (couldn't add string)
	} // end if (we're tracking files)


DONE:

	if (pszRealFilepath != NULL)
	{
		LocalFree(pszRealFilepath);
		pszRealFilepath = NULL;
	} // end if (allocated string)

	return (hr);
} // FileCreateAndOpenFile
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no file stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileWriteString()"
//==================================================================================
// FileWriteString
//----------------------------------------------------------------------------------
//
// Description: Writes the string to the given (already opened) file.
//
// Arguments:
//	HANDLE hFile		File to write to.
//	char* szString		String to write.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileWriteString(HANDLE hFile, char* szString)
{
	HRESULT		hr = S_OK;
	DWORD		dwBytesWritten;
#ifndef NO_TNCOMMON_DEBUG_SPEW
	char		szTemp[256];
#endif // ! NO_TNCOMMON_DEBUG_SPEW


	// If the string is useless, we're done
	if ((szString == NULL) || (strlen(szString) < 1))
		return (S_OK);


	if (! WriteFile(hFile, szString, strlen(szString), &dwBytesWritten, NULL))
	{
		hr = GetLastError();

#ifndef NO_TNCOMMON_DEBUG_SPEW
		// Can't use DPL because it may be calling us.
		OutputDebugString("Couldn't write string \"");
		OutputDebugString(szString);
		wsprintf(szTemp, "\" to file 0x" SPRINTF_PTR ", hr = 0x%08x\n", hFile, hr);
		OutputDebugString(szTemp);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (couldn't write file)

	return (hr);
} // FileWriteString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no file stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileSprintfWriteString()"
//==================================================================================
// FileSprintfWriteString
//----------------------------------------------------------------------------------
//
// Description: Writes the string to the given (already opened) file.  Any tokens in
//				the string are replaced as appropriate.  See sprintf for valid
//				tokens.
//
// Arguments:
//	HANDLE hFile			File to write to.
//	char* szFormatString	String with tokens to write.
//	DWORD dwNumParms		Number of items in the following variable parameter
//							list.
//	...						Variable parameter list with values to replace tokens
//							with.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileSprintfWriteString(HANDLE hFile, char* szFormatString, DWORD dwNumParms, ...)
{
	HRESULT		hr = S_OK;
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp;
	char*		pszTemp = NULL;
	DWORD		dwBytesWritten;
#ifndef NO_TNCOMMON_DEBUG_SPEW
	char		szTemp[256];
#endif // ! NO_TNCOMMON_DEBUG_SPEW



	// If the string is useless, we're done
	if ((szFormatString == NULL) || (strlen(szFormatString) < 1))
		goto DONE;

	papvParms = (LPVOID*) LocalAlloc(LPTR, dwNumParms * sizeof (PVOID));
	if (papvParms == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Loop through the variable arguments and put them into an array for passing
	// to sprintf.
	va_start(currentparam, dwNumParms);
	for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
	{
		papvParms[dwTemp] = va_arg(currentparam, PVOID);
	} // end for (each variable parameter)
	va_end(currentparam);

	TNsprintf_array(&pszTemp, szFormatString, dwNumParms, papvParms);

	if (! WriteFile(hFile, pszTemp, strlen(pszTemp), &dwBytesWritten, NULL))
	{
		hr = GetLastError();

#ifndef NO_TNCOMMON_DEBUG_SPEW
		// Can't use DPL because it may be calling us.
		OutputDebugString("Couldn't sprintf write string \"");
		OutputDebugString(pszTemp);
		wsprintf(szTemp, "\" to file 0x" SPRINTF_PTR ", hr = 0x%08x\n", hFile, hr);
		OutputDebugString(szTemp);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (couldn't write file)


DONE:

	if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (allocated param list)

	if (pszTemp != NULL)
		TNsprintf_free(&pszTemp);

	return (hr);
} // FileSprintfWriteString
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#ifndef _XBOX // no file stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileWriteLine()"
//==================================================================================
// FileWriteLine
//----------------------------------------------------------------------------------
//
// Description: Writes the string to the given (already opened) file, and places the
//				carriage-return/newline combination after it.  If the pointer is
//				NULL or the string is empty, just the carriage-return and newline
//				are printed.
//
// Arguments:
//	HANDLE hFile		File to write to.
//	char* pszString	Pointer to string to write.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileWriteLine(HANDLE hFile, char* pszString)
{
	HRESULT		hr = S_OK;
	char*		pszTemp = NULL;
	DWORD		dwTempSize = 3; // cr + newline + NULL termination
	DWORD		dwBytesWritten;
#ifndef NO_TNCOMMON_DEBUG_SPEW
	char		szTemp[256];
#endif // ! NO_TNCOMMON_DEBUG_SPEW


	// We allocate a temp string because we want the line to be printed atomically.

	if ((pszString != NULL) && (strlen(pszString) > 0))
		dwTempSize += strlen(pszString);


	pszTemp = (char*) LocalAlloc(LPTR, dwTempSize);
	if (pszTemp == NULL)
	{
		return (E_OUTOFMEMORY);
	} // end if (couldn't allocate memory)

	if (pszString != NULL)
		strcpy(pszTemp, pszString);

	strcat(pszTemp, "\r\n");

	if (! WriteFile(hFile, pszTemp, strlen(pszTemp), &dwBytesWritten, NULL))
	{
		hr = GetLastError();

#ifndef NO_TNCOMMON_DEBUG_SPEW
		// Can't use DPL because it may be calling us.
		OutputDebugString("Couldn't write line \"");
		OutputDebugString(pszTemp);
		wsprintf(szTemp, "\" to file 0x" SPRINTF_PTR ", hr = 0x%08x\n", hFile, hr);
		OutputDebugString(szTemp);
#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (couldn't write file)

	LocalFree(pszTemp);
	pszTemp = NULL;

	return (hr);
} // FileWriteLine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no file stuff
#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileSprintfWriteLine()"
//==================================================================================
// FileSprintfWriteLine
//----------------------------------------------------------------------------------
//
// Description: Writes the string to the given (already opened) file.  Any tokens in
//				the string are replaced as appropriate.  See sprintf for valid
//				tokens.
//
// Arguments:
//	HANDLE hFile			File to write to.
//	char* szFormatString	String with tokens to write.
//	DWORD dwNumParms		Number of items in the following variable parameter
//							list.
//	...						Variable parameter list with values to replace tokens
//							with.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileSprintfWriteLine(HANDLE hFile, char* szFormatString, DWORD dwNumParms, ...)
{
	HRESULT		hr = S_OK;
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp;
	char*		pszOutput = NULL;
#ifndef NO_TNCOMMON_DEBUG_SPEW
	char		szTemp[256];
#endif // ! NO_TNCOMMON_DEBUG_SPEW


	if ((szFormatString == NULL) && (strlen(szFormatString) < 1))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Won't write NULL or empty string to file %x!", 1, hFile);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		hr = ERROR_INVALID_PARAMETER;
		goto DONE;
	} // end if (got handed NULL or empty string)

	papvParms = (PVOID*) LocalAlloc(LPTR, dwNumParms * sizeof (PVOID));
	if (papvParms == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	// Loop through the variable arguments and put them into an array for passing
	// to sprintf.
	va_start(currentparam, dwNumParms);
	for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
	{
		papvParms[dwTemp] = va_arg(currentparam, PVOID);
	} // end for (each variable parameter)
	va_end(currentparam);

	TNsprintf_array(&pszOutput, szFormatString, dwNumParms, papvParms);

	hr = FileWriteLine(hFile, pszOutput);
#ifndef NO_TNCOMMON_DEBUG_SPEW
	if (hr != S_OK)
	{
		// Can't use DPL because it may be calling us.
		OutputDebugString("Couldn't sprintf write line \"");
		OutputDebugString(pszOutput);
		wsprintf(szTemp, "\" to file 0x" SPRINTF_PTR ", hr = 0x%08x\n", hFile, hr);
		OutputDebugString(szTemp);
	} // end if (couldn't write line)
#endif // ! NO_TNCOMMON_DEBUG_SPEW


DONE:

	if (papvParms != NULL)
	{
		LocalFree(papvParms);
		papvParms = NULL;
	} // end if (allocated param list)

	TNsprintf_free(&pszOutput);

	return (hr);
} // FileSprintfWriteLine
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX



#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileStartTrackingOutputFiles()"
//==================================================================================
// FileStartTrackingOutputFiles
//----------------------------------------------------------------------------------
//
// Description: Begins saving the paths for all files created using routines in this
//				file.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileStartTrackingOutputFiles(void)
{
	HRESULT		hr = S_OK;


	//BUGBUG not thread safe
	if (g_pTrackedFiles != NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Already tracking output files.", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (we're already tracking)

	g_pTrackedFiles = new (CLStringList);


DONE:

	return (hr);
} // FileStartTrackingOutputFiles
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileGetOutputFilesArray()"
//==================================================================================
// FileGetOutputFilesArray
//----------------------------------------------------------------------------------
//
// Description: Stops saving the paths for all created files and frees the memory
//				allocated.  If lpapszArray is NULL, the number of entries required
//				to hold the array is placed in lpdwMaxNumEntries, and
//				ERROR_BUFFER_TOO_SMALL is returned.	
//
// Arguments:
//	BOOL fIgnoreDuplicates		Whether to not include duplicates (files whose paths
//								match exactly) only once in the list.
//	char** papszArray			Array to store list in, or NULL to retrieve num
//								entries required.
//	DWORD* pdwMaxNumEntries		Pointer to number of entries in preceding array, or
//								place to store num required.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileGetOutputFilesArray(BOOL fIgnoreDuplicates, char** papszArray,
								DWORD* pdwMaxNumEntries)
{
	HRESULT		hr = S_OK;
	int			i;
	DWORD		dwNumEntries = 0;
	PLSTRING	pItem;
	LLIST		usedlist;


	if (g_pTrackedFiles == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Not currently tracking output files!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (ERROR_BAD_ENVIRONMENT);
	} // end if (not currently tracking files)


	g_pTrackedFiles->EnterCritSection();

	for(i = 0; i < g_pTrackedFiles->Count(); i++)
	{
		pItem = (PLSTRING) g_pTrackedFiles->GetItem(i);
		if (pItem == NULL)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't add alias to item %x to used list!", 1, pItem);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			hr = E_FAIL;
			goto DONE;
		} // end if (couldn't add item to used list)


		// If we should ignore duplicates, make sure we haven't used this item already
		if (fIgnoreDuplicates)
		{
			// If we've already found this item, skip it
			if (usedlist.GetFirstIndex(pItem) >= 0)
				continue;

			// Add an alias to it to our used list.
			hr = usedlist.Add(pItem);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't add alias to item %x to used list!", 1, pItem);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				goto DONE;
			} // end if (couldn't add item to used list)
		} // end if (we should ignore duplicates)

		if (papszArray != NULL)
			papszArray[dwNumEntries] = pItem->GetString();

		dwNumEntries++;
	} // end for (each entry)


DONE:

	g_pTrackedFiles->LeaveCritSection();

	if ((hr == S_OK) && (papszArray == NULL))
	{
		(*pdwMaxNumEntries) = dwNumEntries;
		hr = ERROR_BUFFER_TOO_SMALL;
	} // end if (just retrieving size)

	return (hr);
} // FileGetOutputFilesArray
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#undef DEBUG_SECTION
#define DEBUG_SECTION	"FileStopTrackingOutputFiles()"
//==================================================================================
// FileStopTrackingOutputFiles
//----------------------------------------------------------------------------------
//
// Description: Stops saving the paths for all created files and frees the memory
//				allocated.
//
// Arguments: None.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT FileStopTrackingOutputFiles(void)
{
	HRESULT			hr;
	PLSTRINGLIST	pList;


	pList = g_pTrackedFiles;
	g_pTrackedFiles = NULL;

	if (pList == NULL)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "WARNING: Not currently tracking output files.", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		return (S_OK);
	} // end if (not currently tracking files)

	hr = pList->RemoveAll();
	if (hr != S_OK)
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't remove all tracked files from the list!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
	} // end if (not currently tracking files)

	delete (pList);
	pList = NULL;

	return (hr);
} // FileStopTrackingOutputFiles
#undef DEBUG_SECTION
#define DEBUG_SECTION	""






#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CopyTreeInternal()"
//==================================================================================
// CopyTreeInternal
//----------------------------------------------------------------------------------
//
// Description: Internal function that copies all files (including ones in
//				subdirectories) from the source path to the destination path.  The
//				heirarchical structure is created if it doesn't exist.
//
// Arguments:
//	char* szSourceDirPath			Path to root source directory.  Must not end in
//									a backslash.
//	char* szDestDirPath				Path to root destination directory.  Must not
//									end in a backslash.
//	PCOPYTREEDATA pCopyTreeData		Pointer to additional parameter data to use
//									when copying.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
HRESULT CopyTreeInternal(char* szSourceDirPath, char* szDestDirPath,
						PCOPYTREEDATA pCopyTreeData)
{
	HRESULT				hr;
	HRESULT				temphr;
	DWORD				dwSize;
	char*				pszSearchPattern = NULL;
	DWORD				dwSubItemSourcePathSize = 0;
	char*				pszSubItemSourcePath = NULL;
	DWORD				dwSubItemDestPathSize = 0;
	char*				pszSubItemDestPath = NULL;
	HANDLE				hFind = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATA		finddata;
	BOOL				fPreparedDirectory = FALSE;
	BOOL				fSkip;


	// 5 = "\*.*" + NULL termination
	pszSearchPattern = (char*) LocalAlloc(LPTR, strlen(szSourceDirPath) + 5);
	if (pszSearchPattern == NULL)
	{
		hr = E_OUTOFMEMORY;
		goto DONE;
	} // end if (couldn't allocate memory)

	strcpy(pszSearchPattern, szSourceDirPath);
	strcat(pszSearchPattern, "\\*.*");


	ZeroMemory(&finddata, sizeof (WIN32_FIND_DATA));

	hFind = FindFirstFile(pszSearchPattern, &finddata);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		hr = GetLastError();

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't find first item (%s)!", 1, pszSearchPattern);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW

		goto DONE;
	} // end if (couldn't find first file)


	// If we're not skipping this entire directory and we're supposed to copy
	// empty directories, we can just go ahead and make it now.
	if ((! pCopyTreeData->fSkip) &&
		(pCopyTreeData->dwOptions & FILECTO_COPY_EMPTY_DIRS))
	{
		hr = FileEnsureDirectoryExists(szDestDirPath);
		if (hr != S_OK)
		{
			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't make sure \"%s\" exists!", 1, szDestDirPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW
			goto DONE;
		} // end if (couldn't make sure destination directory exists)

		fPreparedDirectory = TRUE;

		if (pCopyTreeData->dwOptions & FILECTO_EMPTY_COPIED_DIRS)
		{
			hr = FileCleanDirectory(szDestDirPath, FALSE, FALSE);
			if (hr != S_OK)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't empty \"%s\"!", 1, szDestDirPath);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW
				goto DONE;
			} // end if (couldn't clean the directory)
		} // end if (should empty directory)
	} // end if (should always copy the directory)

	// This assumes the first item returned is the "." special directory
	while (FindNextFile(hFind, &finddata))
	{
		// Skip the ".." special directory, too.
		if (strcmp(finddata.cFileName, "..") == 0)
			continue;


		// If we're not skipping everything in this directory, start by
		// assuming we won't skip this item either.
		fSkip = pCopyTreeData->fSkip;


		// Build the current source path

		// + backslash, NULL termination, and room for possible extra backslash.
		dwSize = strlen(szSourceDirPath) + strlen(finddata.cFileName) + 3;
		if (dwSize >= dwSubItemSourcePathSize)
		{
			if (pszSubItemSourcePath != NULL)
			{
				LocalFree(pszSubItemSourcePath);
				pszSubItemSourcePath = NULL;
			} // end if (had buffer before)

			pszSubItemSourcePath = (char*) LocalAlloc(LPTR, dwSize);
			if (pszSubItemSourcePath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			dwSubItemSourcePathSize = dwSize;
		} // end if (the buffer wasn't big enough before)

		strcpy(pszSubItemSourcePath, szSourceDirPath);
		strcat(pszSubItemSourcePath, "\\");
		strcat(pszSubItemSourcePath, finddata.cFileName);


		// Mark this for skipping the file if it's in the list and we're
		// not already skipping.
		if ((! fSkip) && (pCopyTreeData->pSkipFiles != NULL))
		{
			// If it's a directory, we temporarily add a backslash (don't worry we
			// made room up above) for easier filtering.
			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				strcat(pszSubItemSourcePath, "\\");


			fSkip = pCopyTreeData->pSkipFiles->ContainsMatchingWildcardForString(pszSubItemSourcePath,
																				FALSE);
			if (fSkip)
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(1, "\"%s\" matches a pattern, skipping.",
					1, pszSubItemSourcePath);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

#ifndef NO_FILEUTILS_COPYTREEPROGRESS
				// If we don't need to update the progress, we can just move on
				// right now.
				if (! (pCopyTreeData->dwOptions & FILECTO_DISPLAY_PROGRESS_BAR))
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS
					continue;
			} // end if (matched item)

			// Remove that backslash if we had one
			if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				pszSubItemSourcePath[strlen(pszSubItemSourcePath) - 1] = '\0';
		} // end if (there's a list of files to skip)


		// Build the current destination path

		dwSize = strlen(szDestDirPath) + strlen(finddata.cFileName) + 2;
		if (dwSize >= dwSubItemDestPathSize)
		{
			if (pszSubItemDestPath != NULL)
			{
				LocalFree(pszSubItemDestPath);
				pszSubItemDestPath = NULL;
			} // end if (had buffer before)

			pszSubItemDestPath = (char*) LocalAlloc(LPTR, dwSize);
			if (pszSubItemDestPath == NULL)
			{
				hr = E_OUTOFMEMORY;
				goto DONE;
			} // end if (couldn't allocate memory)
			dwSubItemDestPathSize = dwSize;
		} // end if (the buffer wasn't big enough before)

		strcpy(pszSubItemDestPath, szDestDirPath);
		strcat(pszSubItemDestPath, "\\");
		strcat(pszSubItemDestPath, finddata.cFileName);


		// Handle the item

		if (finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			BOOL	fSkipDir;


			// Save the old skip boolean.
			fSkipDir = pCopyTreeData->fSkip;
			// Pass the new one into the function.
			pCopyTreeData->fSkip = fSkip;

			// Recursively call ourselves
			hr = CopyTreeInternal(pszSubItemSourcePath, pszSubItemDestPath,
								pCopyTreeData);
			if (hr != S_OK)
			{
				goto DONE;
			} // end if (a recursive call failed)

			pCopyTreeData->fSkip = fSkipDir;
		} // end if (it's a directory)
		else
		{
			// If we got here, but we're skipping, then we just need to update the
			// progress bar)
			if (fSkip)
			{
#ifdef DEBUG
#ifndef NO_FILEUTILS_COPYTREEPROGRESS
				if (! (pCopyTreeData->dwOptions & FILECTO_DISPLAY_PROGRESS_BAR))
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(1, "How did we get to skip the item processing but we're not displaying progress (item = \"%s\")!?",
						1, pszSubItemSourcePath);
					#endif // NO_TNCOMMON_DEBUG_SPEW

					hr = ERROR_BAD_ENVIRONMENT;
					goto DONE;
				} // end if (not displaying progress)
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS
#endif // DEBUG
				// Update the handled count.
				pCopyTreeData->dwNumFilesHandled++;

				// Update the UI.
				if (pCopyTreeData->hProgressWindow != NULL)
				{
					SetWindowText(GetDlgItem(pCopyTreeData->hProgressWindow, IDT_CURRENTSOURCE),
								pszSubItemSourcePath);

					// Since we're not copying it, make a note of that.
					SetWindowText(GetDlgItem(pCopyTreeData->hProgressWindow, IDT_CURRENTDEST),
								"(not copied)");

					SendMessage(GetDlgItem(pCopyTreeData->hProgressWindow, IDPB_PROGRESSBAR),
								PBM_SETPOS, pCopyTreeData->dwNumFilesHandled, 0);
				} // end if (there's a window)

				// Move on
				continue;
			} // end if (skipping file)

			// If the user doesn't want to re-copy files that are the same or clean
			// the directory prior to copying, check if the file already exists and
			// matches.  If so, then skip it.
			if ((! (pCopyTreeData->dwOptions & (FILECTO_FORCE_COPY_IF_SAME | FILECTO_EMPTY_COPIED_DIRS))) &&
				(FileCompareFiles(pszSubItemSourcePath, pszSubItemDestPath, FALSE)))
			{
				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(1, "\"%s\" is the same as \"%s\", skipping.",
					2, pszSubItemSourcePath, pszSubItemDestPath);
				#endif // NO_TNCOMMON_DEBUG_SPEW
				continue;
			} // end if (files are same and we don't copy those)


			// Since we're actually copying a file, make sure the destination
			// directory exists if we haven't already.
			if (! fPreparedDirectory)
			{
				hr = FileEnsureDirectoryExists(szDestDirPath);
				if (hr != S_OK)
				{
					#ifndef NO_TNCOMMON_DEBUG_SPEW
					DPL(0, "Couldn't make sure \"%s\" exists!", 1, szDestDirPath);
					#endif // ! NO_TNCOMMON_DEBUG_SPEW
					goto DONE;
				} // end if (couldn't make sure destination directory exists)

				fPreparedDirectory = TRUE;

				if (pCopyTreeData->dwOptions & FILECTO_EMPTY_COPIED_DIRS)
				{
					hr = FileCleanDirectory(szDestDirPath, FALSE, FALSE);
					if (hr != S_OK)
					{
						#ifndef NO_TNCOMMON_DEBUG_SPEW
						DPL(0, "Couldn't empty \"%s\"!", 1, szDestDirPath);
						#endif // ! NO_TNCOMMON_DEBUG_SPEW
						goto DONE;
					} // end if (couldn't clean the directory)
				} // end if (should empty directory)
			} // end if (haven't ensured directory yet)

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(1, "Copying file \"%s\" to \"%s\".",
				2, pszSubItemSourcePath, pszSubItemDestPath);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW


#ifndef NO_FILEUTILS_COPYTREEPROGRESS
			// Update the progress bar, if we're doing that
			if (pCopyTreeData->dwOptions & FILECTO_DISPLAY_PROGRESS_BAR)
			{
				// Update the handled count.
				pCopyTreeData->dwNumFilesHandled++;

				// Update the UI.
				if (pCopyTreeData->hProgressWindow != NULL)
				{
					SetWindowText(GetDlgItem(pCopyTreeData->hProgressWindow, IDT_CURRENTSOURCE),
								pszSubItemSourcePath);

					SetWindowText(GetDlgItem(pCopyTreeData->hProgressWindow, IDT_CURRENTDEST),
								pszSubItemDestPath);

					SendMessage(GetDlgItem(pCopyTreeData->hProgressWindow, IDPB_PROGRESSBAR),
								PBM_SETPOS, pCopyTreeData->dwNumFilesHandled, 0);

					InvalidateRect(pCopyTreeData->hProgressWindow, NULL, TRUE);
				} // end if (there's a window)
			} // end if (displaying progress)
#endif // ! NO_FILEUTILS_COPYTREEPROGRESS


			if (! CopyFile(pszSubItemSourcePath, pszSubItemDestPath, ((pCopyTreeData->dwOptions & FILECTO_FAIL_IF_EXISTS) ? TRUE : FALSE)))
			{
				hr = GetLastError();

				#ifndef NO_TNCOMMON_DEBUG_SPEW
				DPL(0, "Couldn't copy \"%s\" to \"%s\"!",
					2, pszSubItemSourcePath, pszSubItemDestPath);
				#endif // ! NO_TNCOMMON_DEBUG_SPEW

				goto DONE;
			} // end if (couldn't copy file)
		} // end else (it's not a directory)
	} // end while (there are more items)

	// There was an error, so just make sure it's the right one.
	hr = GetLastError();

	if (hr == ERROR_NO_MORE_FILES)
		hr = S_OK;
	#ifndef NO_TNCOMMON_DEBUG_SPEW
	else
	{
		DPL(0, "Failed while searching for next item with \"%s\"!",
			1, pszSearchPattern);
	} // end else (got unexpected error)
	#endif // ! NO_TNCOMMON_DEBUG_SPEW


DONE:

	if (hFind != INVALID_HANDLE_VALUE)
	{
		if (! FindClose(hFind))
		{
			temphr = GetLastError();

			#ifndef NO_TNCOMMON_DEBUG_SPEW
			DPL(0, "Couldn't close find!  %e", 1, temphr);
			#endif // ! NO_TNCOMMON_DEBUG_SPEW

			if (hr == S_OK)
				hr = temphr;
		} // end if (couldn't close find)
		hFind = INVALID_HANDLE_VALUE;
	} // end if (there's an open find)

	if (pszSubItemDestPath != NULL)
	{
		LocalFree(pszSubItemDestPath);
		pszSubItemDestPath = NULL;
	} // end if (couldn't allocate memory)

	if (pszSubItemSourcePath != NULL)
	{
		LocalFree(pszSubItemSourcePath);
		pszSubItemSourcePath = NULL;
	} // end if (couldn't allocate memory)

	if (pszSearchPattern != NULL)
	{
		LocalFree(pszSearchPattern);
		pszSearchPattern = NULL;
	} // end if (couldn't allocate memory)

	return (hr);
} // CopyTreeInternal
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


#ifndef NO_FILEUTILS_COPYTREEPROGRESS


#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CopyTreeProgressWindowProc()"
//==================================================================================
// CopyTreeProgressWindowProc
//----------------------------------------------------------------------------------
//
// Description: Message pump that handles the CopyTree progress window.
//
// Arguments:
//	HWND hWnd		Window handle.
//	UINT uMsg		Message identifier.
//	WPARAM wParam	Depends on message.
//	LPARAM lParam	Depends on message.
//
// Returns: Depends on message.
//==================================================================================
INT_PTR CALLBACK CopyTreeProgressWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam,
											LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
		  break;
	} // end switch (on the type of window message)

	return (DefWindowProc(hWnd, uMsg, wParam, lParam));
} // CopyTreeProgressWindowProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX




#ifndef _XBOX // no window logging supported
#undef DEBUG_SECTION
#define DEBUG_SECTION	"CopyTreeProgressThreadProc()"
//==================================================================================
// CopyTreeProgressThreadProc
//----------------------------------------------------------------------------------
//
// Description: Thread that only exists to display the copy tree progress.
//
// Arguments:
//	LPVOID lpvParameter		Parameter.  Cast as pointer to a COPYTREEDATA structure.
//
// Returns: S_OK if successful, error code otherwise.
//==================================================================================
DWORD WINAPI CopyTreeProgressThreadProc(LPVOID lpvParameter)
{
	HRESULT			hr = S_OK;
	PCOPYTREEDATA	pCopyTreeData = (PCOPYTREEDATA) lpvParameter;
	WNDCLASSEX		wcex;
	MSG				msg;



	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(8, "Starting up  (%x)", 1, pCopyTreeData); 
	#endif // ! NO_TNCOMMON_DEBUG_SPEW


	ZeroMemory(&wcex, sizeof (WNDCLASSEX));
	wcex.cbSize = sizeof (WNDCLASSEX);
	GetClassInfoEx(NULL, WC_DIALOG, &wcex);
	wcex.lpfnWndProc = (WNDPROC)CopyTreeProgressWindowProc;
#ifdef LOCAL_TNCOMMON
	wcex.hInstance = NULL;
#else
	wcex.hInstance = s_hInstance;
#endif // ! LOCAL_TNCOMMON
	wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = WINDOWCLASS_FILE_COPYTREEPROGRESS;
	wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (! RegisterClassEx(&wcex))
	{
		hr = GetLastError();
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't register window class!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't register class)


	// Make sure they're loaded.
	InitCommonControls();


#ifdef LOCAL_TNCOMMON
	pCopyTreeData->hProgressWindow = CreateDialog(NULL,
													MAKEINTRESOURCE(IDD_PROGRESS),
													NULL,
													CopyTreeProgressWindowProc);
#else
	pCopyTreeData->hProgressWindow = CreateDialog(s_hInstance,
													MAKEINTRESOURCE(IDD_PROGRESS),
													NULL,
													CopyTreeProgressWindowProc);
#endif // ! LOCAL_TNCOMMON
	if (pCopyTreeData->hProgressWindow == NULL)
	{
		hr = GetLastError();

		if (hr == S_OK)
			hr = E_FAIL;

		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't create progress bar dialog!", 0);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't create dialog)

	// Set the progress bar range.
	SendMessage(GetDlgItem(pCopyTreeData->hProgressWindow, IDPB_PROGRESSBAR),
				PBM_SETRANGE, 0, (MAKELPARAM(0, pCopyTreeData->dwTotalNumFiles)));

	ShowWindow(pCopyTreeData->hProgressWindow, SW_SHOW); // ignoring error


	// Okay, we're ready, signal our creator
	if (! SetEvent(pCopyTreeData->hProgressThreadStartedEvent))
	{
		#ifndef NO_TNCOMMON_DEBUG_SPEW
		DPL(0, "Couldn't set thread ready event (%x)!",
			1, pCopyTreeData->hProgressThreadStartedEvent);
		#endif // ! NO_TNCOMMON_DEBUG_SPEW
		goto DONE;
	} // end if (couldn't set ready event)


	// Enter the window message loop.
	while(GetMessage(&msg, NULL, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	} // end while (there's a non-quit message)



DONE:

	#ifndef NO_TNCOMMON_DEBUG_SPEW
	DPL(8, "Exiting, %e", 1, hr); 
	#endif // ! NO_TNCOMMON_DEBUG_SPEW

	return (hr);
} // CopyTreeProgressThreadProc
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX

#endif // ! NO_FILEUTILS_COPYTREEPROGRESS
