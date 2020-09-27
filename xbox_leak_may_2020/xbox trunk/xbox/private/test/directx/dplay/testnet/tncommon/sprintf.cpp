//==================================================================================
// Includes
//==================================================================================
#define INITGUID
#include <windows.h>

#ifndef _XBOX
#ifndef NO_SPRINTF_WINSOCK
#include <winsock.h>
#endif // ! NO_SPRINTF_WINSOCK
#else // ! XBOX
#ifndef NO_SPRINTF_WINSOCK
#include <winsock.h>
#endif // ! NO_SPRINTF_WINSOCK
#endif // XBOX

#ifndef NO_SPRINTF_DPLAY
#include <dplobby.h>
#endif // ! NO_SPRINTF_DPLAY

#ifndef NO_SPRINTF_DPLAY8
#include <dplay8.h>
#include <dpaddr.h>
#include <dpsp8.h>
#endif // ! NO_SPRINTF_DPLAY8

#ifndef NO_SPRINTF_DSOUND
#include <mmreg.h> // NTBUILD requires this for some reason
#include <dsound.h>
#endif // ! NO_SPRINTF_DSOUND

#ifndef NO_SPRINTF_DVOICE
#include <dvoice.h>
#endif // ! NO_SPRINTF_DVOICE

#include <initguid.h>


#include "main.h"
#include "errors.h"
#include "strutils.h"
#if ((defined (TNCOMMON_EXPORTS)) || (defined (USE_SPRINTF_SYMBOLLOOKUP)))
#include "symbols.h"
#endif // TNCOMMON_EXPORTS or USE_SPRINTF_SYMBOLLOOKUP
#include "sprintf.h"


//==================================================================================
// Local defines
//==================================================================================
#define INITIAL_BUFFER_SIZE		256



//==================================================================================
// Local structures
//==================================================================================
typedef struct TNSPRINTFBUFFER * PTNSPRINTFBUFFER;

struct TNSPRINTFBUFFER
{
	PTNSPRINTFBUFFER	pNextBuffer; // pointer to next buffer object in pool or list
	char*				pszBuffer; // pointer to actual string buffer
	DWORD				dwBufferSize; // size of actual string buffer, in bytes
};



//==================================================================================
// Globals
//==================================================================================
DWORD				g_dwTNsprintfRefcount = 0;
CRITICAL_SECTION	g_csBuffersLock;
PTNSPRINTFBUFFER	g_pFirstTNsprintfBufferInPool = NULL;
PTNSPRINTFBUFFER	g_pFirstTNsprintfBufferInOutstandingList = NULL;





//==================================================================================
// TNsprintfInitialize
//----------------------------------------------------------------------------------
//
// Description: Initializes TNsprintf functionality.  Should be called before any
//				other TNsprintf functions are used.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNsprintfInitialize(void)
{
	DWORD				dwTemp;
	PTNSPRINTFBUFFER	pBuffer;


	//BUGBUG according to MSDN, this won't necessarily work on Win95, because "if
	//		 the result of the operation is greater than zero, the return value is
	//		 positive, but it is not necessarily equal to the result".
	dwTemp = InterlockedIncrement((LPLONG) &g_dwTNsprintfRefcount);

	// If this process has already initialized TNsprintf at least once, we're done.
	if (dwTemp > 1)
		return;

	// Otherwise, we're the first caller, set up the stuff we need.

	//BUGBUG It's possible for another thread to try to use these before this first
	//       thread has completely set them up.

	InitializeCriticalSection(&g_csBuffersLock);

	g_pFirstTNsprintfBufferInPool = (PTNSPRINTFBUFFER) LocalAlloc(LPTR, sizeof (TNSPRINTFBUFFER));
	if (g_pFirstTNsprintfBufferInPool == NULL)
		return;

	pBuffer = g_pFirstTNsprintfBufferInPool;
	pBuffer->dwBufferSize = INITIAL_BUFFER_SIZE;
	pBuffer->pszBuffer = (char*) LocalAlloc(LPTR, pBuffer->dwBufferSize);
	if (pBuffer->pszBuffer == NULL)
	{
		LocalFree(g_pFirstTNsprintfBufferInPool);
		g_pFirstTNsprintfBufferInPool = NULL;
		return;
	} // end if (couldn't allocate memory)

	// End up with a total of 5 buffers.
	for(dwTemp = 1; dwTemp < 5; dwTemp++)
	{
		pBuffer->pNextBuffer = (PTNSPRINTFBUFFER) LocalAlloc(LPTR, sizeof (TNSPRINTFBUFFER));
		if (pBuffer->pNextBuffer == NULL)
			return;

		pBuffer = pBuffer->pNextBuffer;

		pBuffer->dwBufferSize = INITIAL_BUFFER_SIZE;
		pBuffer->pszBuffer = (char*) LocalAlloc(LPTR, pBuffer->dwBufferSize);
		if (pBuffer->pszBuffer == NULL)
			return;
	} // end for (each buffer)
} // TNsprintfInitialize





//==================================================================================
// TNsprintfCleanup
//----------------------------------------------------------------------------------
//
// Description: Cleans up TNsprintf functionality.  Must be called for each call
//				to TNsprintfInitialize.  Last caller in the process shuts down all
//				items allocated.
//
// Arguments: None.
//
// Returns: None.
//==================================================================================
void TNsprintfCleanup(void)
{
	PTNSPRINTFBUFFER	pBuffer;
	PTNSPRINTFBUFFER	pNextBuffer;


	// BUGBUG if someone calls Initialize while we're inside the final cleanup code,
	//		  they will get hosed.


	// If this process still has more users of TNDebug functions, we're done.
	if (g_dwTNsprintfRefcount > 1)
	{
		g_dwTNsprintfRefcount--;
		return;
	} // end if (we're not the last one to use the function)


	// Otherwise, shut down the stuff that was in use.

	pNextBuffer = g_pFirstTNsprintfBufferInPool;
	g_pFirstTNsprintfBufferInPool = NULL;

	while (pNextBuffer != NULL)
	{
		pBuffer = pNextBuffer;
		pNextBuffer = pNextBuffer->pNextBuffer;

		if (pBuffer->pszBuffer != NULL)
			LocalFree(pBuffer->pszBuffer);

		LocalFree(pBuffer);
	} // end while (buffers to free)

#ifdef DEBUG
	if (g_pFirstTNsprintfBufferInOutstandingList != NULL)
	{
#ifdef _X86_
		OutputDebugString("There are still TNsprintf buffers outstanding!  int 3-ing.\n");
		{_asm int 3};
#else // _X86_
		OutputDebugString("There are still TNsprintf buffers outstanding!  DebugBreak()-ing.\n");
		DebugBreak();
#endif // _X86_
	} // end if (buffers not returned)
#endif // DEBUG

	DeleteCriticalSection(&g_csBuffersLock);

	g_dwTNsprintfRefcount--; // should be 0 after this
#ifdef DEBUG
	if (g_dwTNsprintfRefcount != 0)
	{
#ifdef _X86_
		OutputDebugString("TNsprintf refcount is screwed!  int 3-ing.\n");
		{_asm int 3};
#else // _X86_
		OutputDebugString("TNsprintf refcount is screwed!  DebugBreak()-ing.\n");
		DebugBreak();
#endif // _X86_
	} // end if (refcount is screwed)
#endif // DEBUG
} // TNsprintfCleanup






#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNsprintf()"
//==================================================================================
// TNsprintf
//----------------------------------------------------------------------------------
//
// Description: Builds a string from a required format string and additional
//				optional parameters.
//				NOTE: memory is allocated here, so the caller must call
//				TNsprintf_free when done using string.
//
//				The available special tokens are:
//					"%b" - bool, "false" is printed if the parameter is 0, otherwise
//							"true" is printed.
//					"%B" - BOOL, "FALSE" is printed if the parameter is 0, otherwise
//							"TRUE" is printed.
//					"%c" - ANSI character.
//					"%C" - Unicode character.
//					"%d" - signed decimal integer.  Same as %i.  [-][0][width] are
//							options which can be between the % and the d, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%e" - error name string.  Param is HRESULT.
//					"%E" - error name and description string.  Param is HRESULT.
//					"%f" - double precision floating point value.  Param is a
//							pointer to a double.
//					"%F" - floating point value.  Param is a pointer to a float.
//					"%g" - GUID. Param is a pointer to a GUID.
//					"%G" - GUID + attempts to resolve string name for GUID.  Param
//							is a pointer to a GUID.
//					"%i" - signed decimal integer.  Same as %d.  [-][0][width] are
//							options which can be between the % and the i, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%o" - socket address, param is pointer to SOCKADDR_IN.
//					"%s" - ANSI string.
//					"%S" - Unicode string.
//					"%t" - flag table. First parameter is the DWORD_PTR containing
//							flags to convert, second parameter is a pointer to the
//							TNFLAGTABLEITEM array for conversion, and third
//							parameter is number of items in the TNFLAGTABLEITEM
//							array.
//					"%u" - Unsigned decimal integer.  [-][0][width] are options
//							which can be between the % and the u, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%v" - binary data, printed in byte chunks, hex.  First
//							parameter is pointer to data, second is size of data.
//					"%x" - hexadecimal DWORD with 0x prefix.
//					"%X" - hexadecimal DWORD + attempts to look up a symbol
//							associated with the param.
//					"%z" - sized ANSI string.  First parameter is pointer to string
//							start, second is number of characters to print.
//
//
// Arguments:
//	char** ppszOutput		Pointer to pointer that will be set to point at the new
//							string.
//	char* szFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms		How many parameters are in the following variable
//							parameter list.
//	...						Variable list of parameters to parse.
//
// Returns: None.
//==================================================================================
void TNsprintf(char** ppszOutput, char* szFormatString, DWORD dwNumParms, ...)
{
#ifndef _XBOX // need to overwrite this with xLogging
	PVOID*		papvParms = NULL;
	va_list		currentparam;
	DWORD		dwTemp = 0;


	if (dwNumParms > 0)
	{
		papvParms = (PVOID*) LocalAlloc(LPTR, (dwNumParms * sizeof (PVOID)));
		if (papvParms == NULL)
			return;

		va_start(currentparam, dwNumParms);

		for(dwTemp = 0; dwTemp < dwNumParms; dwTemp++)
		{
			papvParms[dwTemp] = va_arg(currentparam, PVOID);
		} // end for (each parameter)

		va_end(currentparam);
	} // end if (there are parameters to check)


	// Call the array version of this function.
	TNsprintf_array(ppszOutput, szFormatString, dwNumParms, papvParms);


	LocalFree(papvParms);
	papvParms = NULL;
#else // ! XBOX
//#pragma TODO(tristanj, "Need to map TNsprintf to an xLog based function")
#endif // XBOX

} // TNsprintf
#undef DEBUG_SECTION
#define DEBUG_SECTION	""




// Save the current warning settings
#pragma warning(push)

// Turn off:
// warning C4800: 'void *' : forcing value to bool 'true' or 'false' (performance warning)
// see case 'b' below.
#pragma warning(disable : 4800)




#ifndef _XBOX // no logging
#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNsprintf_array()"
//==================================================================================
// TNsprintf_array
//----------------------------------------------------------------------------------
//
// Description: Builds a string from a required format string and additional
//				optional parameters in the form of a variable sized array.
//				NOTE: memory is allocated here, so the caller must call
//				TNsprintf_free when done using string.
//
//				The available special tokens are:
//					"%b" - bool, "false" is printed if the parameter is 0, otherwise
//							"true" is printed.
//					"%B" - BOOL, "FALSE" is printed if the parameter is 0, otherwise
//							"TRUE" is printed.
//					"%c" - ANSI character.
//					"%C" - Unicode character.
//					"%d" - signed decimal integer.  Same as %i.  [-][0][width] are
//							options which can be between the % and the d, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%e" - error name string.  Param is HRESULT.
//					"%E" - error name and description string.  Param is HRESULT.
//					"%f" - double precision floating point value.  Param is a
//							pointer to a double.
//					"%F" - floating point value.  Param is a pointer to a float.
//					"%g" - GUID. Param is a pointer to a GUID.
//					"%G" - GUID + attempts to resolve string name for GUID.  Param
//							is a pointer to a GUID.
//					"%i" - signed decimal integer.  Same as %d.  [-][0][width] are
//							options which can be between the % and the i, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%o" - socket address, param is pointer to SOCKADDR_IN.
//					"%s" - ANSI string.
//					"%S" - Unicode string.
//					"%t" - flag table. First parameter is the DWORD_PTR containing
//							flags to convert, second parameter is a pointer to the
//							TNFLAGTABLEITEM array for conversion, and third
//							parameter is number of items in the TNFLAGTABLEITEM
//							array.
//					"%u" - Unsigned decimal integer.  [-][0][width] are options
//							which can be between the % and the u, where "-"
//							specifies right justified padding, 0 means use zero
//							padding instead of spaces, and width indicates the
//							minimum number of characters to display (i.e. padding,
//							but not the number to truncate to if longer).
//					"%v" - binary data, printed in byte chunks, hex.  First
//							parameter is pointer to data, second is size of data.
//					"%x" - hexadecimal DWORD with 0x prefix.
//					"%X" - hexadecimal DWORD + attempts to look up a symbol
//							associated with the param.
//					"%z" - sized ANSI string.  First parameter is pointer to string
//							start, second is number of characters to print.
//
//
// Arguments:
//	char** ppszOutput		Pointer to pointer that will be set to point at the new
//							string.
//	char* szFormatString	String (with optional tokens) to print.
//	DWORD dwNumParms		How many parameters are in the following array.
//	PVOID* apvParms			Array of parameter items to parse.
//
// Returns: None.
//==================================================================================
void TNsprintf_array(char** ppszOutput, char* szFormatString,
					DWORD dwNumParms, PVOID* apvParms)
{
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

//BUGBUG DWORD to int conversion sometimes
	//This essentially performs a realloc.
	#define ENSUREBUFFERLENGTH(buffer, chars)	{\
													if ((int) (chars) >= buffer->dwBufferSize)\
													{\
														buffer->dwBufferSize += chars + INITIAL_BUFFER_SIZE;\
														pvTemp = LocalAlloc(LPTR, (buffer->dwBufferSize * sizeof (char)));\
														if (pvTemp == NULL)\
														{\
															goto DONE;\
														}\
														strcpy((char*) pvTemp, buffer->pszBuffer);\
														LocalFree(buffer->pszBuffer);\
														buffer->pszBuffer = (char*) pvTemp;\
														pvTemp = NULL;\
													}\
												}
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	DWORD_PTR			dwParm = 0;
	int					i;
	PTNSPRINTFBUFFER	pBuffer = NULL;
	PTNSPRINTFBUFFER	pTempBuffer = NULL;
	DWORD				dwCurrentTempIndex = 0;
	DWORD				dwUsedOutputLength = 0;
	BOOL				fInToken = FALSE;
	BOOL				fNumbersNotAllowed = FALSE;
	BOOL				fDashesNotAllowed = FALSE;
	BOOL				fPeriodsNotAllowed = FALSE;
	PVOID				pvTemp = NULL;
	DWORD				dwSize;
	DWORD_PTR			dwFlags;
	char				szNumber[32];
	GUID*				pGuid;
	char*				pszString;


	// Get 2 buffers, one to pass back to user and the other to use temporarily.
	// First try to get each from the pool.  If those are all taken, allocate a new
	// one.
	EnterCriticalSection(&g_csBuffersLock);
	for(i = 2; i > 0; i--)
	{
		pBuffer = g_pFirstTNsprintfBufferInPool;
		if (pBuffer == NULL)
		{
			pBuffer = (PTNSPRINTFBUFFER) LocalAlloc(LPTR, sizeof (TNSPRINTFBUFFER));
			if (pBuffer == NULL)
			{
				LeaveCriticalSection(&g_csBuffersLock);
				goto DONE;
			} // end if (out of memory)

			pBuffer->dwBufferSize = INITIAL_BUFFER_SIZE;
			pBuffer->pszBuffer = (char*) LocalAlloc(LPTR, pBuffer->dwBufferSize);
			if (pBuffer->pszBuffer == NULL)
			{
				LeaveCriticalSection(&g_csBuffersLock);
				goto DONE;
			} // end if (out of memory)
		} // end if (no buffers in pool)
		else
		{
			// We took the item out of the pool.
			g_pFirstTNsprintfBufferInPool = g_pFirstTNsprintfBufferInPool->pNextBuffer;

			// Not in any lists right now.
			pBuffer->pNextBuffer = NULL;
		} // end else (buffers left in pool)

		// First time through the loop was for the temp buffer.
		if (i == 2)
			pTempBuffer = pBuffer;
	} // end for (returned buffer and temp buffer)
	LeaveCriticalSection(&g_csBuffersLock);


#ifdef DEBUG
	// I'm not sure how this could happen, but it seems to be, so try to catch it
	// early.
	if (pTempBuffer == pBuffer)
	{
#ifdef _X86_
		OutputDebugString("\nTemp buffer and final buffer are the same!  int 3-ing.\n");
		{_asm int 3};
#else // _X86_
		OutputDebugString("\nTemp buffer and final buffer are the same!  DebugBreak()-ing.\n");
		DebugBreak();
#endif // _X86_
	} // end if (got same buffer)
#endif // DEBUG


	// i should be 0 here
	while (szFormatString[i] != 0)
	{
		// Make sure the temp buffer can hold more characters.
		ENSUREBUFFERLENGTH(pTempBuffer, dwCurrentTempIndex);

		// Default to just copying the character over.
		pTempBuffer->pszBuffer[dwCurrentTempIndex] = szFormatString[i];
		pTempBuffer->pszBuffer[dwCurrentTempIndex + 1] = 0; // NULL terminate it

		if (fInToken)
		{
			switch (szFormatString[i])
			{
				case '0':
				case '1':
				case '2':
				case '3':
				case '4':
				case '5':
				case '6':
				case '7':
				case '8':
				case '9':
					// If we already moved past an acceptable place for numbers,
					// abort this token.
					if (fNumbersNotAllowed)
					{
						// Abort this token string, and dump what we have
						fInToken = FALSE;
					} // end if (not the first character)

					// Movin' on up
					dwCurrentTempIndex++;

					// Dashes are not allowed after numbers.
					fDashesNotAllowed = TRUE;
				  break;

				case '-':
					// If we already moved past an acceptable place for a dash,
					// abort this token.
					if (fDashesNotAllowed)
					{
						// Abort this token string, and dump what we have
						fInToken = FALSE;
					} // end if (not the first character)

					// Movin' on up
					dwCurrentTempIndex++;
				  break;

				case '.':
					// If we already moved past an acceptable place for a period,
					// abort this token.
					if (fPeriodsNotAllowed)
					{
						// Abort this token string, and dump what we have
						fInToken = FALSE;
					} // end if (not the first character)

					// Movin' on up
					dwCurrentTempIndex++;

					// Dashes are not allowed after periods.
					fDashesNotAllowed = TRUE;
				  break;

				case 'b':
					bool	bData;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%b") == 0)
					{
						// This will generate a compiler warning on all platforms,
						// but it is disabled above.
						bData = (bool) apvParms[dwParm++];


						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough for larger of
						// ("true" | "false").
						//ENSUREBUFFERLENGTH(pTempBuffer, 5);

						if (bData)
							strcpy(pTempBuffer->pszBuffer, "true");
						else
							strcpy(pTempBuffer->pszBuffer, "false");
					} // end if (valid token)
				  break;
				  
				case 'B':
					BOOL	fData;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%B") == 0)
					{
// Unfortunately can't use the pragmas, because debugprint.h isn't (and can't be)
// included.
						fData = (BOOL)(DWORD_PTR) apvParms[dwParm++];


						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough for larger of
						// ("TRUE" | "FALSE").
						//ENSUREBUFFERLENGTH(pTempBuffer, 5);

						if (fData)
							strcpy(pTempBuffer->pszBuffer, "TRUE");
						else
							strcpy(pTempBuffer->pszBuffer, "FALSE");
					} // end if (valid token)
				  break;
				  
				case 'c':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%c") == 0)
					{
						// Make sure the buffer is large enough.
						ENSUREBUFFERLENGTH(pTempBuffer, 1);

						wsprintfA(pTempBuffer->pszBuffer, "%c", (char) apvParms[dwParm++]);
					} // end if (valid token)
				  break;
				  
				case 'C':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%C") == 0)
					{
						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough.
						//ENSUREBUFFERLENGTH(pTempBuffer, 1);

						wsprintfA(pTempBuffer->pszBuffer, "%C", (WCHAR) apvParms[dwParm++]);
					} // end if (valid token)
				  break;
				  
				case 'd':
				case 'i':
				case 'u':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;

					// %d and %i are ints, %u is a DWORD, but they all get passed
					// straight to wsprintfA the same way (formatting info and all).
					wsprintfA(szNumber, pTempBuffer->pszBuffer, apvParms[dwParm++]);

					// Commented out because we know it will be large enough....
					//
					// Make sure the buffer is large enough.
					//ENSUREBUFFERLENGTH(pTempBuffer, strlen(szNumber));

					strcpy(pTempBuffer->pszBuffer, szNumber);
				  break;
				  
				case 'e':
				case 'E':
					LONG_PTR	hrValue;
					DWORD		dwShortcut;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;

// Unfortunately can't use the pragmas, because debugprint.h isn't (and can't be)
// included.
//#pragma TODO(BrettHu,"Can you use a LONG_PTR in plcase of an HRESULT? Is this good coding practice for IA 64 ?")
					hrValue = (LONG_PTR) apvParms[dwParm++];

					dwFlags = ERRORSFLAG_PRINTHEXVALUE | ERRORSFLAG_PRINTDECIMALVALUE;

					// Uppercase E means they want the description, too.
					if (strcmp(pTempBuffer->pszBuffer, "%E") == 0)
						dwFlags |= ERRORSFLAG_PRINTDESCRIPTION;

					// We want to optimize, so initialize the shortcut.
					dwShortcut = ERRORSSHORTCUT_NOTSEARCHED;


					// Ignore error, assume BUFFER_TOO_SMALL
					GetErrorDescription(hrValue, dwFlags, NULL, &dwSize, &dwShortcut);

					// Make sure the buffer is large enough.
					ENSUREBUFFERLENGTH(pTempBuffer, dwSize);

					// Ignore error
					GetErrorDescription(hrValue, dwFlags, pTempBuffer->pszBuffer,
										&dwSize, &dwShortcut);
				  break;

				case 'f':
				case 'F':
					int		iSrcChar;
					int		iDestChar;
					int		iDec;
					int		iSign;


					fInToken = FALSE;

					if (szFormatString[i] == 'F')
					{
						pszString = _fcvt(*((float*) (apvParms[dwParm++])),
											10, &iDec, &iSign);

						if (pszString[0] == '\0')
						{
							// Commented out because we know it will be big enough.
							//ENSUREBUFFERLENGTH(pTempBuffer, strlen("__unprintable_float__") + 1);

							strcpy(pTempBuffer->pszBuffer, "__unprintable_float__");
							break; // bail out of the case statement
						} // end if (not a valid string)
					} // end if (float type proper)
					else
					{
						pszString = _ecvt(*((double*) (apvParms[dwParm++])),
											10, &iDec, &iSign);

						if (pszString[0] == '\0')
						{
							// Commented out because we know it will be big enough.
							//ENSUREBUFFERLENGTH(pTempBuffer, strlen("__unprintable_double__") + 1);

							strcpy(pTempBuffer->pszBuffer, "__unprintable_double__");
							break; // bail out of the case statement
						} // end if (not a valid string)
					} // end else (double type)


					// If the decimal point is to the right of the start of the string,
					// then we only need room for the decimal point.
					// Otherwise, we need to include room for leading zeros.
					// The 3 comes from: 1 for possible '-', 1 for decimal point, 1
					// for NULL termination
					if (iDec < 1)
						iSrcChar = (-1 * iDec) + 4; // + 1 for extra leading zero
					else
						iSrcChar = 3;

					ENSUREBUFFERLENGTH(pTempBuffer, strlen(pszString) + iSrcChar);

					iSrcChar = 0;
					iDestChar = 0;

					// If it's negative, add the minus sign
					if (iSign)
					{
						pTempBuffer->pszBuffer[iDestChar++] = '-'; // add minus sign
					} // end if (it's negative)
					
					// If -1 < number < 1, then put leading 0s.
					if (iDec < 1)
					{
						// Insert extra leading zero.
						pTempBuffer->pszBuffer[iDestChar++] = '0';
						// Insert decimal place
						pTempBuffer->pszBuffer[iDestChar++] = '.';

						// For even smaller numbers, add more zeroes.
						while (iDec < 0)
						{
							// Add another leading 0.
							pTempBuffer->pszBuffer[iDestChar++] = '0';
							iDec++;
						} // end while (need more leading zeros)

						// We've already placed the decimal point, so make sure
						// we don't do it again.
						iDec = -1;
					} // end if (less than |1|)

					do
					{
						// Insert the decimal point if it's located here.
						if (iDec == iSrcChar)
						{
							pTempBuffer->pszBuffer[iDestChar++] = '.';
						} // end if (should insert decimal point)


						// Copy the character over
						pTempBuffer->pszBuffer[iDestChar] = pszString[iSrcChar];

						if (pszString[iSrcChar] == '\0')
							break;

						iSrcChar++;
						iDestChar++;
					} // end do (while haven't hit end of string)
					while (TRUE);

					// Go back until the first non trailing zero.
					do
					{
						iDestChar--;
					} // end while (not at non-zero character)
					while (pTempBuffer->pszBuffer[iDestChar] == '0');

					// If the decimal point was at the end, that looks dumb, so
					// leave one trailing zero.
					if (pTempBuffer->pszBuffer[iDestChar] == '.')
						iDestChar++;

					// Make sure the string is terminated at the end here
					pTempBuffer->pszBuffer[iDestChar + 1] = '\0';
				  break;
				  
				case 'g':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%g") == 0)
					{
						pGuid = (GUID*) apvParms[dwParm++];

						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough.  This is the size of
						// the longest possible GUID string.
						//ENSUREBUFFERLENGTH(pTempBuffer, 38);

						if (pGuid == NULL)
						{
							strcpy(pTempBuffer->pszBuffer, "{NULL GUID pointer}");
							break; // get out of the case statement
						} // end if (NULL pointer)

						wsprintfA(pTempBuffer->pszBuffer, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
								pGuid->Data1, pGuid->Data2, pGuid->Data3,
								pGuid->Data4[0], pGuid->Data4[1], pGuid->Data4[2], pGuid->Data4[3],
								pGuid->Data4[4], pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);
					} // end if (valid token)
				  break;
				  
				case 'G':
					BOOL		fFound;
					DWORD		dwType;
#ifndef NO_SPRINTF_REGISTRY
					HKEY		hKey;
					char		szTemp2[1024];
#endif // ! NO_SPRINTF_REGISTRY 


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%G") == 0)
					{
						pGuid = (GUID*) apvParms[dwParm++];

						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough.  This is the size of
						// the longest possible GUID string.  We'll probably do more
						// size checking in a second.
						//ENSUREBUFFERLENGTH(pTempBuffer, 38);

						if (pGuid == NULL)
						{
							strcpy(pTempBuffer->pszBuffer, "{NULL GUID pointer}");
							break; // get out of the case statement
						} // end if (NULL pointer)

						fFound = FALSE;
						dwType = 0;

						wsprintfA(pTempBuffer->pszBuffer, "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
								pGuid->Data1, pGuid->Data2, pGuid->Data3,
								pGuid->Data4[0], pGuid->Data4[1], pGuid->Data4[2], pGuid->Data4[3],
								pGuid->Data4[4], pGuid->Data4[5], pGuid->Data4[6], pGuid->Data4[7]);

#ifndef NO_SPRINTF_REGISTRY
						strcpy(szTemp2, "CLSID\\");
						strcat(szTemp2, pTempBuffer->pszBuffer);

						// Look for a COM Class ID that matches this and pull out its
						// name, if possible.

						if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szTemp2, 0, KEY_QUERY_VALUE, &hKey) == S_OK)
						{
							dwSize = 1024; // string buffer size
							if ((RegQueryValueEx(hKey, NULL, 0, &dwType, (LPBYTE) szTemp2, &dwSize) == S_OK) &&
								(dwType == REG_SZ))
							{
								strcat(szTemp2, " - ");

								// Make sure the buffer is large enough.
								ENSUREBUFFERLENGTH(pTempBuffer, 38 + strlen(szTemp2));

								StringPrepend(pTempBuffer->pszBuffer, szTemp2);
								fFound = TRUE;
							} // end if (there's a default string value for the key)

							RegCloseKey(hKey); // ignore error
						} // end if (we could open the specified key)

						if (fFound)
							break; // stop further processing


						// We didn't find a COM class that matched it, so look through
						// the DPlay SP GUIDs we know about.

						strcpy(szTemp2, "SOFTWARE\\Microsoft\\DirectPlay\\Service Providers");

						if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemp2, 0, KEY_ENUMERATE_SUB_KEYS, &hKey) == S_OK)
						{
							DWORD	dwKeyIndex;
							char	szName[1024];
							DWORD	dwNameLength;
							HKEY	hSubKey;


							dwKeyIndex = 0; // start at beginning
							while (TRUE)
							{
								dwNameLength = 1024 - 1; // size of buffer - zero terminator
								if (RegEnumKeyEx(hKey, dwKeyIndex, szName, &dwNameLength, 0, NULL, NULL, NULL) != S_OK)
									break; // get out of the loop, assume it's a non-fatal error

								dwKeyIndex++; // move on to next key

								strcpy(szTemp2, "SOFTWARE\\Microsoft\\DirectPlay\\Service Providers\\");
								strcat(szTemp2, szName);

								if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, szTemp2, 0, KEY_QUERY_VALUE, &hSubKey) == S_OK)
								{
									dwSize = 1024; // string buffer size
									if ((RegQueryValueEx(hSubKey, "Guid", 0, &dwType, (LPBYTE) szTemp2, &dwSize) == S_OK) &&
										(dwType == REG_SZ))
									{
										// Make it lower case since the letters output
										// by the binary to string conversion for the
										// GUID above uses lower case letters.
										if (strcmp(_strlwr(szTemp2), pTempBuffer->pszBuffer) == 0)
										{
											strcat(szName, " - ");

											// Make sure the buffer is large enough.
											ENSUREBUFFERLENGTH(pTempBuffer, 38 + strlen(szName));

											StringPrepend(pTempBuffer->pszBuffer, szName);
											fFound = TRUE;
										} // end if (the GUIDs match)
									} // end if (there's a valid GUID string value for the sub key)

									RegCloseKey(hSubKey); // ignore error
								} // end if (we could open the sub key)
							} // end while (no error and there are more sub keys)

							RegCloseKey(hKey); // ignore error
						} // end if (we could open the specified key)
#endif // ! NO_SPRINTF_REGISTRY

						if (fFound)
							break; // stop further processing


						// We didn't find a DPlay SP that matched it, so check for any
						// other type of GUIDs we know about.  We don't bother checking
						// to make sure the temp buffer is big enough because we know
						// it will be.

#ifndef NO_SPRINTF_DPLAY
						if (IsEqualGUID((*pGuid), DPAID_ComPort))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_ComPort - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_INet))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_INet - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_INetW))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_INetW - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_INetPort))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_INetPort - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_LobbyProvider))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_LobbyProvider - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_Modem))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_Modem - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_ModemW))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_ModemW - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_Phone))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_Phone - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_PhoneW))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_PhoneW - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_ServiceProvider))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_ServiceProvider - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPAID_TotalSize))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPAID_TotalSize - ");
							break;
						} // end if (we know about this GUID)
#endif // ! NO_SPRINTF_DPLAY

#ifndef NO_SPRINTF_DPLAY8
						if (IsEqualGUID((*pGuid), IID_IDP8ServiceProvider))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDP8ServiceProvider - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8Client))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8Client - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8Server))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8Server - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8Peer))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8Peer - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8Address))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8Address - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8AddressInternal))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8AddressInternal - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8AddressIP))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8AddressIP - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8AddressIPX))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8AddressIPX - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8AddressModem))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8AddressModem - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlay8AddressSerial))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlay8AddressSerial - ");
							break;
						} // end if (we know about this GUID)
#endif // ! NO_SPRINTF_DPLAY8

#ifndef NO_SPRINTF_DVOICE

				//TODO: IIDs for different versions

						if (IsEqualGUID((*pGuid), IID_IDirectPlayVoiceClient))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlayVoiceClient - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlayVoiceServer))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlayVoiceServer - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlayVoiceTest))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlayVoiceTest - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_TRUESPEECH))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_TRUESPEECH - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_GSM))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_GSM - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_NONE))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_NONE - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_ADPCM))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_ADPCM - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_SC03))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_SC03 - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_SC06))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_SC06 - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), DPVCTGUID_VR12))
						{
							StringPrepend(pTempBuffer->pszBuffer, "DPVCTGUID_VR12 - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlayVoiceNotify))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlayVoiceNotify - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectPlayVoiceTransport))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectPlayVoiceTransport - ");
							break;
						} // end if (we know about this GUID)
#endif // ! NO_SPRINTF_DVOICE

#ifndef NO_SPRINTF_DSOUND
						if (IsEqualGUID((*pGuid), IID_IDirectSound))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSound - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSoundBuffer))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSoundBuffer - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSound3DListener))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSound3DListener - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSound3DBuffer))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSound3DBuffer - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSoundCapture))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSoundCapture - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSoundCaptureBuffer))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSoundCaptureBuffer - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IDirectSoundNotify))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IDirectSoundNotify - ");
							break;
						} // end if (we know about this GUID)

						if (IsEqualGUID((*pGuid), IID_IKsPropertySet))
						{
							StringPrepend(pTempBuffer->pszBuffer, "IID_IKsPropertySet - ");
							break;
						} // end if (we know about this GUID)
#endif // ! NO_SPRINTF_DSOUND


						// Nobody knows about this GUID, so copy in a default string.

						StringPrepend(pTempBuffer->pszBuffer, "Unknown GUID - ");
					} // end if (valid token)
				  break;
				  				  
#ifndef NO_SPRINTF_WINSOCK
				case 'o':
					SOCKADDR_IN*	pSockAddrIn;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%o") == 0)
					{
						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough for
						// "255.255.255.255:65535"
						//ENSUREBUFFERLENGTH(pTempBuffer, 21);

						pSockAddrIn = (SOCKADDR_IN*) (apvParms[dwParm++]);
						wsprintfA(pTempBuffer->pszBuffer, "%s:%i", inet_ntoa(pSockAddrIn->sin_addr),
								ntohs(pSockAddrIn->sin_port));
					} // end if (valid token)
				  break;
#endif // NO_SPRINTF_WINSOCK

				case 's':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%s") == 0)
					{
						pszString = (char*) (apvParms[dwParm++]);

						if (pszString == NULL)
						{
							// Commented out because we know it will be large enough....
							//
							// Make sure the buffer is large enough for
							// "_NULL-string_"
							//ENSUREBUFFERLENGTH(pTempBuffer, 13);
							strcpy(pTempBuffer->pszBuffer, "_NULL-string_");
						} // end if (it's a NULL string)
						else
						{
							// Make sure it's a valid string.
							try
							{
								// Make sure the buffer is large enough.
								ENSUREBUFFERLENGTH(pTempBuffer, strlen(pszString));
								strcpy(pTempBuffer->pszBuffer, pszString);
							} // end try
							catch (...)
							{
								wsprintfA(szNumber, "0x" SPRINTF_PTR, pszString);

								// Commented out because we know it will be large enough....
								//
								// Make sure the buffer is large enough.
								//ENSUREBUFFERLENGTH(pTempBuffer, strlen(szNumber) + strlen("_invalid-string-_"));

								strcpy(pTempBuffer->pszBuffer, "_invalid-string-");
								strcat(pTempBuffer->pszBuffer, szNumber);
								strcat(pTempBuffer->pszBuffer, "_");
							} // end catch (all)
						} // end else (a valid string pointer)
					} // end if (valid token)
				  break;

				case 'S':
					WCHAR*	pwszString;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%S") == 0)
					{
						pwszString = (WCHAR*) (apvParms[dwParm++]);

						if (pwszString == NULL)
						{
							// Commented out because we know it will be large enough....
							//
							// Make sure the buffer is large enough for
							// "_NULL-string_"
							//ENSUREBUFFERLENGTH(pTempBuffer, 13);
							strcpy(pTempBuffer->pszBuffer, "_NULL-string_");
						} // end if (it's a NULL string)
						else
						{
							// Make sure it's a valid string.
							try
							{
								// Size is in bytes...
								dwSize = WideCharToMultiByte(CP_ACP, 0, pwszString, -1,
															NULL, 0, NULL, NULL);
								if (dwSize != 0)
								{
									// Make sure the buffer is large enough.
									ENSUREBUFFERLENGTH(pTempBuffer, dwSize);

									// Ignoring error
									WideCharToMultiByte(CP_ACP, 0, pwszString, -1,
														pTempBuffer->pszBuffer,
														(pTempBuffer->dwBufferSize * sizeof (char)),
														NULL, NULL);
								} // end if (there's a valid size)
							} // end try
							catch (...)
							{
								wsprintfA(szNumber, "0x" SPRINTF_PTR, pwszString);

								// Commented out because we know it will be large enough....
								//
								// Make sure the buffer is large enough.
								//ENSUREBUFFERLENGTH(pTempBuffer, strlen(szNumber) + strlen("_invalid-string-_"));

								strcpy(pTempBuffer->pszBuffer, "_invalid-string-");
								strcat(pTempBuffer->pszBuffer, szNumber);
								strcat(pTempBuffer->pszBuffer, "_");
							} // end catch (all)
						} // end else (a valid string pointer)
					} // end if (valid token)
				  break;

				case 't':
					DWORD_PTR			dwFlags;
					PTNFLAGTABLEITEM	paFlagTable;
					DWORD_PTR			dwNumFlagsInTable;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%t") == 0)
					{
						dwFlags = (DWORD_PTR) apvParms[dwParm++];

						if (dwParm < dwNumParms)
						{
							paFlagTable = (PTNFLAGTABLEITEM) apvParms[dwParm++];

							if (dwParm < dwNumParms)
							{
								dwNumFlagsInTable = (DWORD_PTR) apvParms[dwParm++];

								// Ignore error, assumer BUFFER_TOO_SMALL.
								StringAddFlags(dwFlags, paFlagTable, dwNumFlagsInTable,
												NULL, &dwSize);

								// Make sure the buffer is large enough to hold the
								// expand flags string
								ENSUREBUFFERLENGTH(pTempBuffer, dwSize);

								// Make sure the string is empty.
								pTempBuffer->pszBuffer[0] = '\0';

								// Ignore error
								StringAddFlags(dwFlags, paFlagTable, dwNumFlagsInTable,
												pTempBuffer->pszBuffer, &dwSize);
							} // end if (there's at least one more parameter in the array)
						} // end if (there's at least one more parameter in the array)
					} // end if (valid token)
				  break;

				case 'v':
					PVOID	pvData;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%v") == 0)
					{
						pvData = apvParms[dwParm++];

						if (dwParm < dwNumParms)
						{
							dwSize = (DWORD)(DWORD_PTR) apvParms[dwParm++];

							// Make sure the buffer is large enough.  Each byte
							// expands to 3 characters except the last one.
							ENSUREBUFFERLENGTH(pTempBuffer, (dwSize * 3) - 1);

							StringBufferToHexStr(pvData, dwSize, pTempBuffer->pszBuffer);
						} // end if (there's at least one more parameter in the array)
					} // end if (valid token)
				  break;
				  
				case 'x':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%x") == 0)
					{
						// Commented out because we know it will be large enough....
						//
						// Make sure the buffer is large enough for "0xFFFFFFFF".
						//ENSUREBUFFERLENGTH(pTempBuffer, 10);

						wsprintfA(pTempBuffer->pszBuffer, "0x" SPRINTF_PTR, apvParms[dwParm++]);
					} // end if (valid token)
				  break;

#ifndef _XBOX // not supporting symbol lookup
#if ((defined (TNCOMMON_EXPORTS)) || (defined (USE_SPRINTF_SYMBOLLOOKUP)))
				case 'X':
					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%X") == 0)
					{
// Unfortunately can't use the pragmas, because debugprint.h isn't (and can't be)
// included.
//#pragma BUGBUG(vanceo, "Be able to resize to use a large enough buffer")

						// Ignore error
						TNSymResolve(apvParms[dwParm++], pTempBuffer->pszBuffer, 
									(pTempBuffer->dwBufferSize * sizeof (char)));
					} // end if (valid token)
				  break;
#endif // TNCOMMON_EXPORTS or USE_SPRINTF_SYMBOLLOOKUP
#else // ! XBOX
//#pragma BUGBUG(tristanj, "May need to come up with an alternative for symbol lookup")
#endif // XBOX

				case 'z':
					char*	pszData;


					// If we didn't get a garbage token, convert it.  Otherwise
					// we'll abort the token string and dump what we have.
					fInToken = FALSE;
					if (strcmp(pTempBuffer->pszBuffer, "%z") == 0)
					{
						pszData = (char*) apvParms[dwParm++];

						if (dwParm < dwNumParms)
						{
							dwSize = (DWORD)(DWORD_PTR) apvParms[dwParm++];

							// Make sure the buffer is large enough for "0xFFFFFFFF".
							ENSUREBUFFERLENGTH(pTempBuffer, dwSize);

							CopyMemory(pTempBuffer->pszBuffer, pszData, (dwSize * sizeof (char)));
							pTempBuffer->pszBuffer[dwSize] = 0; // NULL terminate the string
						} // end if (there's at least one more parameter in the array)
					} // end if (valid token)
				  break;

				default:
					// Unrecognized token string, abort this token string, and
					// dump what we have.
					fInToken = FALSE;
				  break;
			} // end switch (on the current token character)
		} // end if (in a token)
		else
		{
			if (szFormatString[i] == '%')
			{
// Unfortunately can't use the pragmas, because debugprint.h isn't (and can't be)
// included.
//#pragma BUGBUG(vanceo, "What about possible tokens which don't require parms?")

				// Only parse special tokens if there are more parameters in the array
				if (dwParm < dwNumParms)
				{
					fInToken = TRUE;
					fNumbersNotAllowed = FALSE;
					fDashesNotAllowed = FALSE;
					fPeriodsNotAllowed = FALSE;
					dwCurrentTempIndex++;
				} // end if (there are still parms left in the string)
			} // end if (we're now in a token)
		} // end else (not in a token)

		i++; // move to next character

		// If we're not currently in a token or we've hit the end of the string,
		// dump what we have
		if ((! fInToken) || (szFormatString[i] == 0))
		{
			dwCurrentTempIndex = 0; // go back to the beginning
				  

			// If the buffer we're returning can't hold this latest string, increase
			// it so it does room.
			ENSUREBUFFERLENGTH(pBuffer, (dwUsedOutputLength + strlen(pTempBuffer->pszBuffer)));

			// Tack the current bit onto the end.
			strcpy((pBuffer->pszBuffer + dwUsedOutputLength), pTempBuffer->pszBuffer);
			dwUsedOutputLength += strlen(pTempBuffer->pszBuffer);
		} // end if (we're not in a token)
	} // end while (we're not at the end of the string)


DONE:

	if (pvTemp != NULL)
	{
		LocalFree(pvTemp);
		pvTemp = NULL;
	} // end if (still have memory)

	// If have temp buffer, return it to the pool.
	if (pTempBuffer != NULL)
	{
		EnterCriticalSection(&g_csBuffersLock);
		pTempBuffer->pNextBuffer = g_pFirstTNsprintfBufferInPool;
		g_pFirstTNsprintfBufferInPool = pTempBuffer;
		LeaveCriticalSection(&g_csBuffersLock);
	} // end if (have item )

	// If have buffer for user, put it on the outstanding list and return it to the
	// user.
	if (pBuffer != NULL)
	{
		EnterCriticalSection(&g_csBuffersLock);
		pBuffer->pNextBuffer = g_pFirstTNsprintfBufferInOutstandingList;
		g_pFirstTNsprintfBufferInOutstandingList = pBuffer;
		LeaveCriticalSection(&g_csBuffersLock);

		(*ppszOutput) = pBuffer->pszBuffer;
	} // end if (have item )

	return;

	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	#undef ENSURETEMPBUFFERLENGTH
	// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
} // TNsprintf_array
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
#endif // ! XBOX


// Restore the previous warning settings
#pragma warning( pop ) 







#undef DEBUG_SECTION
#define DEBUG_SECTION	"TNsprintf_free()"
//==================================================================================
// TNsprintf_free
//----------------------------------------------------------------------------------
//
// Description: Releases the memory allocated by a call to TNsprintf.
//
//
// Arguments:
//	char** ppszBuffer	Pointer to pointer retrieved by previous call to TNsprintf.
//
// Returns: None.
//==================================================================================
void TNsprintf_free(char** ppszBuffer)
{
	PTNSPRINTFBUFFER	pLastBuffer = NULL;
	PTNSPRINTFBUFFER	pBuffer;
	char				szNumber[32];


	if ((*ppszBuffer) == NULL)
		return;


	EnterCriticalSection(&g_csBuffersLock);

	pBuffer = g_pFirstTNsprintfBufferInOutstandingList;
	while (pBuffer != NULL)
	{
		// Is this the buffer corresponding to the string we gave them?
		if (pBuffer->pszBuffer == (*ppszBuffer))
		{
			// Yes, so pull it out of the list...
			if (pLastBuffer != NULL)
				pLastBuffer->pNextBuffer = pBuffer->pNextBuffer;
			else
				g_pFirstTNsprintfBufferInOutstandingList = pBuffer->pNextBuffer;

			// ...and place it back in the pool.
			pBuffer->pNextBuffer = g_pFirstTNsprintfBufferInPool;
			g_pFirstTNsprintfBufferInPool = pBuffer;

			LeaveCriticalSection(&g_csBuffersLock);

			return;
		} // end if (found buffer)

		pLastBuffer = pBuffer;
		pBuffer = pBuffer->pNextBuffer;
	} // end while (haven't hit end of list)

	LeaveCriticalSection(&g_csBuffersLock);


	// If we got here, we didn't find the corresponding buffer in our outstanding
	// list.

#ifdef DEBUG
	wsprintf(szNumber, "0x" SPRINTF_PTR, (*ppszBuffer));
	OutputDebugString("Didn't find TNsprintf buffer ");
	OutputDebugString(szNumber);
	OutputDebugString(" (\"");
	OutputDebugString((*ppszBuffer));

#ifdef _X86_
	OutputDebugString("\") in outstanding buffers list!  int 3-ing.\n");
	{_asm int 3};
#else // _X86_
	OutputDebugString("\") in outstanding buffers list!  DebugBreak()-ing.\n");
	DebugBreak();
#endif // _X86_

#endif // DEBUG
} // TNsprintf_free
#undef DEBUG_SECTION
#define DEBUG_SECTION	""
