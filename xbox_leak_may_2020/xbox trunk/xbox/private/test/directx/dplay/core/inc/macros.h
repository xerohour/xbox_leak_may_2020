#ifndef __TESTNET_MACROS__
#define __TESTNET_MACROS__

namespace DPlayCoreNamespace {

//==================================================================================
// Pragma messages
//==================================================================================

#define __TODO(user, msgstr, n)								message("TODO: " #user ": " __FILE__ "(" #n ") : " msgstr)
#define __BUGBUG(user, msgstr, n)							message("BUGBUG: " #user ": " __FILE__ "(" #n ") : " msgstr)
#define __WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)	message("WAITFORDEVFIX: " #user ": " __FILE__ "(" #n ") : " #bugdb " bug #" #bugnum" : " titlestr)
#define __PRINTVALUE(itemnamestr, itemvaluestr, n)			message("PRINTVALUE: " __FILE__ "(" #n ") : " itemnamestr " = " itemvaluestr)


#define _TODO(user, msgstr, n)								__TODO(user, msgstr, n)
#define _BUGBUG(user, msgstr, n)							__BUGBUG(user, msgstr, n)
#define _WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)	__WAITFORDEVFIX(user, bugdb, bugnum, titlestr, n)
#define _PRINTVALUE(itemstr, item, n)						__PRINTVALUE(itemstr, #item, n)


#ifdef TODO_OFF
#define TODO(user, msgstr)
#else
#define TODO(user, msgstr)									_TODO(user, msgstr, __LINE__)
#endif

#ifdef BUGBUG_OFF
#define BUGBUG(user, msgstr)
#else
#define BUGBUG(user, msgstr)								_BUGBUG(user, msgstr, __LINE__)
#endif

#ifdef WAITFORDEVFIX_OFF
#define WAITFORDEVFIX(user, bugdb, bugnum, titlestr)
#else
#define WAITFORDEVFIX(user, bugdb, bugnum, titlestr)		_WAITFORDEVFIX(user, bugdb, bugnum, titlestr, __LINE__)
#endif

#ifdef PRINTVALUE_OFF
#define PRINTVALUE(item)
#else
#define PRINTVALUE(item)					_PRINTVALUE(#item, item, __LINE__)
#endif

//==================================================================================
// Macros
//==================================================================================
#define CopyAndMoveDestPointer(pPointer, pSource, dwSize)\
												{ memcpy(pPointer, pSource, dwSize);\
												  pPointer += dwSize; }

#define CopyAndMoveSrcPointer(pDest, pPointer, dwSize)\
												{ memcpy(pDest, pPointer, dwSize);\
												  pPointer += dwSize; }

#define DEBUGBREAK() { _asm int 3 }

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define OVERWRITE_SR_IF_OK(hresult)		{\
											if (sr == S_OK)\
												sr = hresult;\
										}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define DO_FINAL_REPORT(log, success)	//xLog(log, XLL_INFO, "%s test reported %s", DEBUG_SECTION, success ? "SUCCESS" : "FAILURE");

#define FINAL_SUCCESS					DO_FINAL_REPORT(hLog, TRUE)

#define FINAL_FAILURE					DO_FINAL_REPORT(hLog, FALSE)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define THROW_ANY_SYSTEMRESULT(hr)	srThrown = hr;\
									goto SYSTEMFAILURE
#define THROW_ANY_TESTRESULT(hr)	trThrown = hr;\
									goto TESTFAILURE

#define BEGIN_TESTCASE				{\
										HRESULT		srThrown = S_OK;\
										HRESULT		trThrown = S_OK;\
										BOOL		fInVariation = FALSE;\
										BOOL		fPassed = TRUE;


#define HANDLE_TESTFAILURE(log)		{\
										TESTFAILURE:\
										\
										tr = trThrown;\
										xLog(log, XLL_INFO, "Test result failure!  0x%08x.", ((HRESULT) tr));\
										fPassed = FALSE;\
										FINAL_FAILURE;\
										\
										goto ENDTESTCASE;\
									}

#define HANDLE_SYSTEMFAILURE(log)	{\
										SYSTEMFAILURE:\
										\
										sr = srThrown;\
										xLog(log, XLL_INFO, "System result failure!  0x%08x.", ((HRESULT) sr));\
										fPassed = FALSE;\
										\
										if (sr == S_OK)\
										{\
											xLog(log, XLL_WARN, "Got a system result failure of S_OK, converting to E_FAIL!");\
											sr = E_FAIL;\
										}\
										\
										goto ENDTESTCASE;\
									}


#define END_TESTCASE				goto ENDTESTCASE;\
										\
										HANDLE_TESTFAILURE(hLog)\
										HANDLE_SYSTEMFAILURE(hLog)\
										\
										ENDTESTCASE:\
										\
										if(fInVariation)\
										{\
											if(fPassed)\
												xLog(hLog, XLL_PASS, "Test passed!");\
											else\
												xLog(hLog, XLL_FAIL, "Test failed!");\
											\
											fInVariation = FALSE;\
											xEndVariation(hLog);\
										}\
										\
										xLog(hLog, XLL_INFO, "Leaving testcase.");\
									}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#define TEST_SECTION(name)			if(fInVariation)\
									{\
										if(fPassed)\
											xLog(hLog, XLL_PASS, "Test passed!");\
										else\
											xLog(hLog, XLL_FAIL, "Test failed!");\
										\
										xEndVariation(hLog);\
										fInVariation = FALSE;\
									}\
									fPassed = TRUE;\
									xStartVariation(hLog, name);\
									fInVariation = TRUE

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#define SETTHROW_ANY_SYSTEMRESULT(hr, value)	{\
													hr = (HRESULT) value;\
													THROW_ANY_SYSTEMRESULT(hr);\
												}
#define SETTHROW_ANY_TESTRESULT(hr, value)		{\
													hr = (HRESULT) value;\
													THROW_ANY_TESTRESULT(hr);\
												}

#define THROW_SYSTEMRESULT						THROW_ANY_SYSTEMRESULT(sr)
#define THROW_TESTRESULT						THROW_ANY_TESTRESULT(tr)

#define SETTHROW_SYSTEMRESULT(value)			SETTHROW_ANY_SYSTEMRESULT(sr, value)
#define SETTHROW_TESTRESULT(value)				SETTHROW_ANY_TESTRESULT(tr, value)




// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define LOCALALLOC_OR_THROW(type, ptr, size)	{\
														if (ptr != NULL)\
														{\
															xLog(hLog, XLL_WARN, "Pointer %X is not NULL at line %i, DEBUGBREAK()-ing.", ptr, __LINE__);\
															DEBUGBREAK();\
														}\
														ptr = (type) MemAlloc(size);\
														if (ptr == NULL)\
														{\
															SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);\
														}\
													}

#define CREATEEVENT_OR_THROW(handle, lpEventAttributes, fManualReset, fInitialState, pszName)\
												{\
													if (handle != NULL)\
													{\
														xLog(hLog, XLL_WARN, "Handle %x is not NULL at line %i, DEBUGBREAK()-ing.", handle, __LINE__);\
														DEBUGBREAK();\
													}\
													handle = CreateEvent(lpEventAttributes, fManualReset, fInitialState, pszName);\
													if (handle == NULL)\
													{\
														sr = GetLastError();\
														xLog(hLog, XLL_FAIL, "Couldn't create event!");\
														THROW_SYSTEMRESULT;\
													}\
												}

#define SAFE_LOCALFREE(ptr)						{\
													if (ptr != NULL)\
													{\
														MemFree(ptr);\
														ptr = NULL;\
													}\
												}

#define IS_VALID_HANDLE(x)      				(x && (INVALID_HANDLE_VALUE != x))

#define SAFE_CLOSEHANDLE(h)						{\
													if (IS_VALID_HANDLE(h))\
													{\
														CloseHandle(h);\
														h = NULL;\
													}\
												}												

#define SAFE_RELEASE(p)						    {\
													if (p)\
													{\
														p->Release();\
														p = NULL;\
													}\
												}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline BOOL IsZero(PVOID pvPointer, DWORD dwSize)
{
	LPBYTE	lpCurrent = (LPBYTE) pvPointer;
	LPBYTE	lpEnd = lpCurrent + dwSize;
	

	while (lpCurrent < lpEnd)
	{
		if ((*lpCurrent) != 0)
			return (FALSE);

		lpCurrent++;
	} // end while (haven't hit end of buffer)

	return (TRUE);
};

inline void FillWithDWord(PVOID pvPointer, DWORD dwSize, DWORD dwPattern)
{
	LPBYTE	lpCurrent = (LPBYTE) pvPointer;
	DWORD	dwPatternByteIndex = 0;
	LPBYTE	lpEnd = lpCurrent + dwSize;
	

	while (lpCurrent < lpEnd)
	{
		(*lpCurrent) = ((LPBYTE) (&dwPattern))[dwPatternByteIndex];

		lpCurrent++;
		dwPatternByteIndex++;
		if (dwPatternByteIndex >= sizeof (DWORD))
			dwPatternByteIndex = 0;
	} // end while (haven't hit end of buffer)
};

inline BOOL IsFilledWithDWord(PVOID pvPointer, DWORD dwSize, DWORD dwPattern)
{
	LPBYTE	lpCurrent = (LPBYTE) pvPointer;
	DWORD	dwPatternByteIndex = 0;
	LPBYTE	lpEnd = lpCurrent + dwSize;
	

	while (lpCurrent < lpEnd)
	{
		if ((*lpCurrent) != ((LPBYTE) (&dwPattern))[dwPatternByteIndex])
			return (FALSE);

		lpCurrent++;
		dwPatternByteIndex++;
		if (dwPatternByteIndex >= sizeof (DWORD))
			dwPatternByteIndex = 0;
	} // end while (haven't hit end of buffer)

	return (TRUE);
};

} // namespace DPlayCoreNamespace

#endif //__TESTNET_MACROS__
