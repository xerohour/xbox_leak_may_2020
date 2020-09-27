/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	medialoader.h

Abstract:

	copies media files from a server to the xbox

Author:

	Jeff Sullivan (jeffsul) 24-Jul-2001

Revision History:

	24-Jul-2001 jeffsul
		Initial Version

//------------------------------------------------------------------------------
//	Regular Expressions for finding files:
//
//	*	find 0 or more matches
//	+	find 1 or more matches
//	?	find 0 or 1 matches
//
//	()	grouping operator
//
//	\w  is the same as [a-z A-Z 0-9]	(alphanumeric)
//	\d	is the same as [0-9]			(numeric)
//	\s	is the same as [ \f\t\r\n]		(white space)
//
//	Example:
//	==> media_directory/file(\d*)(\w?).(/w/w/w)
//	would find any files in media_directory that begin with "file" followed by any number
//	of digits, then one or more alphanumeric characters, and finally a file extension of 
//	three alphanumeric characters (i.e., file112b.wav, file.txt, file336.1b3, etc. )
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//	Getting a certain number of files:
//	
//	%X	where X is the number of files to find
//
//	Example:
//	==> media_directory/(\w*).wav%3
//	would find 3 files that match the search criteria, in this case, 3 wav files in
//	the media_directory directory
//------------------------------------------------------------------------------


--*/

#ifndef __MEDIALOADER_H__
#define __MEDIALOADER_H__

#ifndef _X86_
#define _X86_
#endif

#include <stdarg.h>
#include <windef.h>
#include <winbase.h>
#include <xbox.h>

#define MLCOPY_ALWAYS		0x0000
#define MLCOPY_IFNEWER		0x0001
#define MLCOPY_IFNOTEXIST	0x0002
#define MLCOPY_MAXVALUE		0x0002

#define COPY_ALWAYS			MLCOPY_ALWAYS
#define COPY_IF_NEWER		MLCOPY_IFNEWER
#define COPY_IF_NOT_EXIST	MLCOPY_IFNOTEXIST

typedef struct _MEDIAFILEDESC				// a structure describing which files to copy from
{											// a remote server and how to copy them...
	LPSTR	lpszFromPath;					// file to search for on remote server (can use wildcards)
	LPSTR	lpszToPath;						// directory where to store the remote file on xbox
	DWORD	dwFlags;						// flags describing how to copy the file
} MEDIAFILEDESC, *PMEDIAFILEDESC;


/*++

Routine Description:

	Grabs media files from a server to the xbox

Arguments:

	IN MEDIAFILEDESC MediaFileDesc[]	-	array of structures describing what files to copy to xbox

Return Value:

	S_OK on succes
	E_XX on failure

--*/
extern HRESULT _stdcall
LoadMedia( 
	IN MEDIAFILEDESC MediaFileDesc[]		// array of structures describing what files to copy to xbox
);


/*++

Routine Description:

	Gets media file descriptions from an .ini file and passes them to LoadMedia

Arguments:

	IN LPCSTR	lpszIniFile	-	path to an initialization file that describes which media files to load

Return Value:

	S_OK on succes
	E_XX on failure

--*/
#if 0
extern HRESULT _stdcall
LoadMediaFromIni( 
	IN LPCSTR	lpszIniFile					// path to an initialization file that describes which media files to load
);
#endif 0


/*++

Routine Description:

	sets the error level for media loader, any errors at or below this level will be output
	the default is set to 5 to output all information
	5 – general info about what the medialoader is doing
	4 – notifications about file states (already exists, up to date, overwriting, etc)
	3 – warnings that are produced when media loader is given bad input (couldn’t find as many files as requested, file missing from .ini, etc)
	2 – warnings when media loader couldn’t do what it was instructed to do for whatever reason
	1 – errors generally having to do with file io (usually problems like file names that are too long), usually recoverable but doesn’t guarantee that you get the results that you wanted
	0 – hopefully you’ll never see these, if you do, it means I probably messed something up with my code or the network is down, not very recoverable


Arguments:

	IN INT nErrorLevel - level of errors to be displayed ( 0-1 = errors, 2-3 = warnings, 4-5 = info )

Return Value:

	None

--*/
extern VOID _stdcall
MLSetErrorLevel( 
	IN INT nErrorLevel 
);

#if 0
/*++

Routine Description:

	gets an integer value from an .ini file

Arguments:

	IN LPCSTR	lpAppName	-	section name
	IN LPCSTR	lpKeyName	-	key name
	IN INT		nDefault	-	return value if key name not found
	IN LPCSTR	lpFileName	-	initialization file name

Return Value:

	The value found in the .ini on success
	nDefault on failure

--*/
extern INT _stdcall
GetIniInt( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN INT		nDefault,					// return value if key name not found
	IN LPCSTR	lpFileName					// initialization file name
);


/*++

Routine Description:

	gets a floating point value from an .ini file

Arguments:

	IN LPCSTR	lpAppName	-	section name
	IN LPCSTR	lpKeyName	-	key name
	IN FLOAT	fDefault	-	return value if key name not found
	IN LPCSTR	lpFileName	-	initialization file name

Return Value:

	The value found in the .ini on success
	fDefault on failure

--*/
extern FLOAT _stdcall
GetIniFloat( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN FLOAT	fDefault,					// return value if key name not found
	IN LPCSTR	lpFileName					// initialization file name
);


/*++

Routine Description:

	gets a string from an .ini file and copies it into lpReturnedString

Arguments:

	IN LPCSTR	lpAppName			-	section name
	IN LPCSTR	lpKeyName,			-	key name
	IN LPCSTR	lpDefault,			-	return value if key name not found
	OUT LPSTR	lpReturnedString,	-	holds the returned string
	IN DWORD	nSize,				-	max size of returned string
	IN LPCSTR	lpFileName			-	initialization file name

Return Value:

	The string found in the .ini on success
	lpDefault on failure

--*/
extern INT _stdcall
GetIniString( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN LPCSTR	lpDefault,					// return value if key name not found
	OUT LPSTR	lpReturnedString,			// holds the returned string
	IN DWORD	nSize,						// max size of returned string
	IN LPCSTR	lpFileName					// initialization file name
);


/*++

Routine Description:

	gets a pre-defined constant value from an .ini file

Arguments:

	IN LPCSTR	lpAppName	-	section name
	IN LPCSTR	lpKeyName	-	key name
	IN DWORD	dwDefault	-	return value if key name not found
	IN LPCSTR	lpFileName	-	initialization file name

Return Value:

	The vlaue of the constant found in the .ini on success
	dwDefault on failure

--*/
extern DWORD _stdcall
GetIniConst( 
	IN LPCSTR	lpAppName,					// section name
	IN LPCSTR	lpKeyName,					// key name
	IN DWORD	dwDefault,					// return value if key name not found
	IN LPCSTR	lpFileName					// initialization file name
);


/*++

Routine Description:

	gets a constant or enumerated value from its string equivalent

Arguments:

	IN CONST CHAR*	szStr	-	constant's name
	IN INT			cchStr	-	length of szStr
	OUT PDWORD		pval	-	pointer to DWORd to store value

Return Value:

	returns TRUE if the constant is found, FALSE otherwise

--*/
extern BOOL _stdcall
GetConst(
	IN CONST CHAR*	szStr, 
	IN INT			cchStr, 
	OUT PDWORD		pval
);
#endif 0
	
#endif // #ifndef __MEDIALOADER_H__
