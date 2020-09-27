#ifndef __TESTNET_MACROS__
#define __TESTNET_MACROS__
//#pragma message("Defining __TESTNET_MACROS__")







//==================================================================================
// Common Defines
//==================================================================================
#ifdef TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllexport)

#else // ! TNCONTRL_EXPORTS

#ifdef DLLEXPORT
#undef DLLEXPORT
#endif // ! DLLEXPORT
#define DLLEXPORT __declspec(dllimport)

#endif // ! TNCONTRL_EXPORTS

#ifndef DEBUG
#ifdef _DEBUG
#define DEBUG
#endif // _DEBUG
#endif // not DEBUG







//==================================================================================
// Macros
//==================================================================================
#ifndef _XBOX // CopyMemory not supported
#define CopyAndMoveDestPointer(pPointer, pSource, dwSize)\
												{ CopyMemory(pPointer, pSource, dwSize);\
												  pPointer += dwSize; }

#define CopyAndMoveSrcPointer(pDest, pPointer, dwSize)\
												{ CopyMemory(pDest, pPointer, dwSize);\
												  pPointer += dwSize; }
#else // ! XBOX
#define CopyAndMoveDestPointer(pPointer, pSource, dwSize)\
												{ memcpy(pPointer, pSource, dwSize);\
												  pPointer += dwSize; }

#define CopyAndMoveSrcPointer(pDest, pPointer, dwSize)\
												{ memcpy(pDest, pPointer, dwSize);\
												  pPointer += dwSize; }
#endif // XBOX

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define OVERWRITE_SR_IF_OK(hresult)		{\
											if (sr == S_OK)\
												sr = hresult;\
										}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define CREATEOUTPUTBUFFER_OR_THROW(type, ptr, size)\
											{\
												ptr = (type) pTNecd->pFinalResult->CreateOutputDataBuffer(size);\
												if (ptr == NULL)\
												{\
													SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);\
												}\
											}

#define SETOUTPUTVARIABLE_OR_THROW(szName, szType, ptr, size)\
											{\
												sr = pTNecd->pFinalResult->SetOutputVariable(szName, szType, ptr, size);\
												if (sr != S_OK)\
												{\
													DPL(0, "Couldn't set output variable \"%s\"!", 1, szName);\
													THROW_SYSTEMRESULT;\
												}\
											}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#define DO_FINAL_REPORT(success)	{\
										sr = pTNecd->pFinalResult->SetResultCodeAndBools(tr, TRUE, success);\
										if (sr != S_OK)\
										{\
											DPL(0, "Setting final test result failed!", 0);\
										}\
									}

#define FINAL_SUCCESS				DO_FINAL_REPORT(TRUE)

#define FINAL_FAILURE				DO_FINAL_REPORT(FALSE)


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#ifdef DEBUG

#define REQUIRE_INPUT_DATA(size)			{\
												if ((pTNecd->pvInputData == NULL) || (pTNecd->dwInputDataSize != (size)))\
												{\
													DPL(0, "Didn't get expected input data (%x is NULL or size %i != %i)!", 3, pTNecd->pvInputData, pTNecd->dwInputDataSize, (size));\
													SETTHROW_SYSTEMRESULT(E_FAIL);\
												}\
											}

#define REQUIRE_MINIMUM_INPUT_DATA(size)	{\
												if ((pTNecd->pvInputData == NULL) || (pTNecd->dwInputDataSize < (size)))\
												{\
													DPL(0, "Didn't get expected initial data (%x is NULL or size %i < %i)!", 3, pTNecd->pvInputData, pTNecd->dwInputDataSize, (size));\
													SETTHROW_SYSTEMRESULT(E_FAIL);\
												}\
											}

#define REQUIRE_ASSIGNREPORT_DATA(size)		{\
												if (pTNwd->dwDataSize != (size))\
												{\
													DPL(0, "Assignment report data is wrong size (%i != %i)!", 2, pTNwd->dwDataSize, (size));\
													return (E_FAIL);\
												}\
											}

#define REQUIRE_SUCCESSREPORT_DATA(size)	{\
												if (pTNwd->dwDataSize != (size))\
												{\
													DPL(0, "Success report data is wrong size (%i != %i)!", 2, pTNwd->dwDataSize, (size));\
													return (E_FAIL);\
												}\
											}
#else // ! DEBUG

#define REQUIRE_INPUT_DATA(size)

#define REQUIRE_MINIMUM_INPUT_DATA(size)

#define REQUIRE_ASSIGNREPORT_DATA(size)

#define REQUIRE_SUCCESSREPORT_DATA(size)

#endif // ! DEBUG

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define GETSUBRESULT_AND_FAILIFFAILED(pSubResult, szCaseID, szFailString)\
												{\
													pSubResult = pTNecd->pSubResults->GetMostRecentResult(szCaseID);\
													if (pSubResult == NULL)\
													{\
														DPL(0, "Couldn't get result for %s!", 1, szCaseID);\
														SETTHROW_SYSTEMRESULT(E_FAIL);\
													}\
													\
													if (! pSubResult->IsSuccess())\
													{\
														DPL(0, szFailString, 0);\
														SETTHROW_TESTRESULT(pSubResult->GetResultCode());\
													}\
												}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define CHECKANDGET_SUBOUTPUTDATA(pSubResult, pvSubOutputData, dwSubOutputDataSize, dwExpectedSize)\
												{\
													sr = pSubResult->GetOutputData(((PVOID*) (&pvSubOutputData)), &dwSubOutputDataSize);\
													if ((sr != S_OK) || (dwSubOutputDataSize != dwExpectedSize))\
													{\
														DPL(0, "Couldn't get sub test output data (%x is NULL), or it's the wrong size (%u != %u)!", 3, pvSubOutputData, dwSubOutputDataSize, dwExpectedSize);\
														\
														if (sr == S_OK)\
															sr = E_FAIL;\
														\
														THROW_SYSTEMRESULT;\
													}\
												}

#define CHECKANDGET_MINIMUM_SUBOUTPUTDATA(pSubResult, pvSubOutputData, dwSubOutputDataSize, dwExpectedSize)\
												{\
													sr = pSubResult->GetOutputData(((PVOID*) (&pvSubOutputData)), &dwSubOutputDataSize);\
													if ((sr != S_OK) || (dwSubOutputDataSize < dwExpectedSize))\
													{\
														DPL(0, "Couldn't get sub test output data (%x is NULL), or it's not large enough (%u < %u)!", 3, pvSubOutputData, dwSubOutputDataSize, dwExpectedSize);\
														\
														if (sr == S_OK)\
															sr = E_FAIL;\
														\
														THROW_SYSTEMRESULT;\
													}\
												}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define CHECKANDGET_SYNCDATA(syncdata, iTesterNum, pvSyncData, dwSyncDataSize, dwExpectedSize)\
												{\
													sr = syncdata.GetDataFromTester(iTesterNum, (PVOID*) &pvSyncData, &dwSyncDataSize);\
													if (sr != S_OK)\
													{\
														DPL(0, "Couldn't get sync data from tester %i!", 1, iTesterNum);\
														THROW_SYSTEMRESULT;\
													}\
													\
													if ((pvSyncData == NULL) || (dwSyncDataSize != dwExpectedSize))\
													{\
														DPL(0, "Got unexpected sync data from tester %i (%x is NULL or size %u != %u)!", 4, iTesterNum, pvSyncData, dwSyncDataSize, dwExpectedSize);\
														SETTHROW_SYSTEMRESULT(E_FAIL);\
													}\
												}

#define CHECKANDGET_MINIMUM_SYNCDATA(syncdata, iTesterNum, pvSyncData, dwSyncDataSize, dwExpectedSize)\
												{\
													sr = syncdata.GetDataFromTester(iTesterNum, (PVOID*) &pvSyncData, &dwSyncDataSize);\
													if (sr != S_OK)\
													{\
														DPL(0, "Couldn't get sync data from tester %i!", 1, iTesterNum);\
														THROW_SYSTEMRESULT;\
													}\
													\
													if ((pvSyncData == NULL) || (dwSyncDataSize < dwExpectedSize))\
													{\
														DPL(0, "Got unexpected sync data from tester %i (%x is NULL or size %u < %u)!", 4, iTesterNum, pvSyncData, dwSyncDataSize, dwExpectedSize);\
														SETTHROW_SYSTEMRESULT(E_FAIL);\
													}\
												}


// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


// This should only be defined for the special documentation builds.
#ifndef TNDOCBUILD


#ifdef USE_TRY_CATCH

#ifndef DONT_SPEW_ON_THROW
#define THROW_ANY_SYSTEMRESULT(hr)	{\
										DPL(0, "Throwing system result %x; %s, line %i", 3, ((HRESULT) hr), __FILE__, __LINE__);\
										throw (hr);\
									}

#define THROW_ANY_TESTRESULT(hr)	{\
										DPL(0, "Throwing test result %x; %s, line %i", 3, ((HRESULT) hr), __FILE__, __LINE__);\
										pTNecd->pExecutor->NoteThrowTestResult(__FILE__, __LINE__);\
										throw (hr);\
									}
#else // ! DONT_SPEW_ON_THROW
#define THROW_ANY_SYSTEMRESULT(hr)	throw (hr)
#define THROW_ANY_TESTRESULT(hr)	throw (hr)
#endif // ! DONT_SPEW_ON_THROW


#define BEGIN_TESTCASE				pTNecd->pExecutor->NoteBeginTestCase(DEBUG_MODULE, DEBUG_SECTION, __FILE__, __LINE__);\
									try


#define CATCH_TESTRESULT			catch (CTNTestResult trCaught)\
									{\
										tr = (HRESULT) trCaught;\
										DPL(0, "Test result catch!  %e.", 1, ((HRESULT) tr));\
										FINAL_FAILURE;\
									}

#define CATCH_SYSTEMRESULT			catch (CTNSystemResult srCaught)\
									{\
										sr = (HRESULT) srCaught;\
										DPL(0, "System result catch!  %e.", 1, ((HRESULT) sr));\
										\
										if (sr == S_OK)\
										{\
											DPL(0, "WARNING: Caught a system result of S_OK, converting to E_FAIL!", 0);\
											sr = E_FAIL;\
										}\
									}

#ifndef _XBOX // this macro doesn't seem to be used, and we don't support GetModuleFileName
#define CATCH_EXCEPTIONLIST			catch (LPCTNExceptionList lpExceptions)\
									{\
										DPL(0, "LPCTNExceptionList catch!", 0);\
										\
										sr = TNHandleCatchExceptionList(lpExceptions, &tr);\
										if (sr != S_OK)\
										{\
											DPL(0, "Couldn't handle ExceptionList catch!", 0);\
										}\
										else\
										{\
											FINAL_FAILURE;\
										}\
									}
#endif // ! XBOX

#ifndef _XBOX // this macro doesn't seem to be used, and we don't support GetModuleFileName
#define CATCH_GENERIC				catch (...)\
									{\
										DPL(0, "Generic catch!", 0);\
										\
										sr = TNHandleCatchGeneric();\
										if (sr != S_OK)\
										{\
											DPL(0, "Couldn't handle generic catch!", 0);\
										}\
										else\
										{\
											sr = E_UNEXPECTED;\
										}\
									}
#endif // ! XBOX


#define END_TESTCASE				CATCH_TESTRESULT\
									CATCH_SYSTEMRESULT\
									CATCH_EXCEPTIONLIST\
									CATCH_GENERIC\
									DNB("Leaving testcase.");
								
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else // ! USE_TRY_CATCH
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#ifndef DONT_SPEW_ON_THROW
#define THROW_ANY_SYSTEMRESULT(hr)	{\
										DPL(0, "Fake throwing system result %x; %s, line %i", 3, ((HRESULT) hr), __FILE__, __LINE__);\
										srThrown = hr;\
										goto SYSTEMFAILURE;\
									}

#define THROW_ANY_TESTRESULT(hr)	{\
										DPL(0, "Fake throwing test result %x; %s, line %i", 3, ((HRESULT) hr), __FILE__, __LINE__);\
										pTNecd->pExecutor->NoteThrowTestResult(__FILE__, __LINE__);\
										trThrown = hr;\
										goto TESTFAILURE;\
									}
#else // ! DONT_SPEW_ON_THROW
#define THROW_ANY_SYSTEMRESULT(hr)	goto SYSTEMFAILURE
#define THROW_ANY_TESTRESULT(hr)	goto TESTFAILURE
#endif // ! DONT_SPEW_ON_THROW


#define BEGIN_TESTCASE				pTNecd->pExecutor->NoteBeginTestCase(DEBUG_MODULE, DEBUG_SECTION, __FILE__, __LINE__);\
									{\
										HRESULT		srThrown;\
										HRESULT		trThrown;


#define HANDLE_TESTFAILURE			{\
										TESTFAILURE:\
										\
										tr = trThrown;\
										DPL(0, "Test result failure!  %e.", 1, ((HRESULT) tr));\
										FINAL_FAILURE;\
										\
										goto ENDTESTCASE;\
									}

#define HANDLE_SYSTEMFAILURE		{\
										SYSTEMFAILURE:\
										\
										sr = srThrown;\
										DPL(0, "System result failure!  %e.", 1, ((HRESULT) sr));\
										\
										if (sr == S_OK)\
										{\
											DPL(0, "WARNING: Got a system result failure of S_OK, converting to E_FAIL!", 0);\
											sr = E_FAIL;\
										}\
										\
										goto ENDTESTCASE;\
									}


#define END_TESTCASE					goto ENDTESTCASE;\
										\
										HANDLE_TESTFAILURE\
										HANDLE_SYSTEMFAILURE\
										\
										ENDTESTCASE:\
										\
										DNB("Leaving testcase.");\
									}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif // ! USE_TRY_CATCH
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -


#define TEST_SECTION(name)						pTNecd->pExecutor->NoteNewTestSection(name, DEBUG_MODULE, DEBUG_SECTION, __FILE__, __LINE__);

#define TESTSECTION_IF(expression)				if (expression)
#define TESTSECTION_ELSE						else
#define TESTSECTION_ELSEIF(expression)			else if (expression)
#define TESTSECTION_ENDIF
#define TESTSECTION_FOR(expression)				for(expression)
#define TESTSECTION_ENDFOR
#define TESTSECTION_DO							do
#define TESTSECTION_DOWHILE(expression)			while (expression)
#define TESTSECTION_WHILE(expression)			while (expression)
#define TESTSECTION_ENDWHILE
#define TESTSECTION_SWITCH(expression)			switch (expression)
#define TESTSECTION_ENDSWITCH
#define TESTSECTION_CASE(expression)			case (expression):
#define TESTSECTION_ENDCASE
#define TESTSECTION_BREAK						break



#ifdef DEBUG

#define LOCALALLOC_OR_THROW(type, ptr, size)	{\
													if (ptr != NULL)\
													{\
														DPL(0, "WARNING: Pointer %X is not NULL at line %i, DEBUGBREAK()-ing.", 2, ptr, __LINE__);\
														DEBUGBREAK();\
													}\
													ptr = (type) LocalAlloc(LPTR, size);\
													if (ptr == NULL)\
													{\
														SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);\
													}\
												}
#else // ! DEBUG

#define LOCALALLOC_OR_THROW(type, ptr, size)	{\
													ptr = (type) LocalAlloc(LPTR, size);\
													if (ptr == NULL)\
													{\
														SETTHROW_SYSTEMRESULT(E_OUTOFMEMORY);\
													}\
												}
#endif // ! DEBUG



// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#else // TNDOCBUILD
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifndef _XBOX // documentation builds not supported

#define THROW_ANY_SYSTEMRESULT(hr)				return (hr)
#define THROW_ANY_TESTRESULT(hr)				return (hr)


#define BEGIN_TESTCASE							pTNecd->pExecutor->DocBuildBeginTestCase(__FILE__, __LINE__);\
												if (FALSE)

#define END_TESTCASE							pTNecd->pExecutor->DocBuildEndTestCase(__FILE__, __LINE__);

#define TEST_SECTION(name)						;}\
												pTNecd->pExecutor->DocBuildNewTestSection(name, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_IF(expression)				;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("If", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ELSE						pTNecd->pExecutor->DocBuildTestSectionControl("Else", NULL, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ELSEIF(expression)			pTNecd->pExecutor->DocBuildTestSectionControl("ElseIf", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ENDIF						pTNecd->pExecutor->DocBuildTestSectionControl("EndIf", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_FOR(expression)				;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("For", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ENDFOR						pTNecd->pExecutor->DocBuildTestSectionControl("EndFor", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_DO							;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("Do", NULL, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_DOWHILE(expression)			pTNecd->pExecutor->DocBuildTestSectionControl("DoWhile", #expression, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_WHILE(expression)			;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("While", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ENDWHILE					pTNecd->pExecutor->DocBuildTestSectionControl("EndWhile", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_SWITCH(expression)			;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("Switch", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ENDSWITCH					pTNecd->pExecutor->DocBuildTestSectionControl("EndSwitch", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_CASE(expression)			;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("Case", #expression, __FILE__, __LINE__);\
												if (FALSE)

#define TESTSECTION_ENDCASE						pTNecd->pExecutor->DocBuildTestSectionControl("EndCase", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{

#define TESTSECTION_BREAK						;}\
												pTNecd->pExecutor->DocBuildTestSectionControl("Break", NULL, __FILE__, __LINE__);\
												if (FALSE)\
												{



#define LOCALALLOC_OR_THROW(type, ptr, size)	ptr = NULL

#endif // ! XBOX
#endif // TNDOCBUILD



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

#define SAFE_LOCALFREE(ptr)						{\
													if (ptr != NULL)\
													{\
														LocalFree(ptr);\
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

// All errors are thrown.
#define HANDLE_SYNC_RESULT				{\
											if (sr == ((HRESULT) TNSR_INSYNC))\
												sr = (HRESULT) S_OK;\
											else\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem syncing!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem syncing!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// All errors are thrown.
#define HANDLE_WAIT_RESULT				{\
											if (sr == ((HRESULT) TNWR_GOTEVENT))\
												sr = (HRESULT) S_OK;\
											else\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem waiting for event or cancel!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem waiting for event or cancel!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// TIMEOUT just drops through, all other errors are thrown.
#define HANDLE_WAIT_RESULT_NOT_TIMEOUT	{\
											if (sr == ((HRESULT) TNWR_GOTEVENT))\
												sr = (HRESULT) S_OK;\
											else if (sr != ((HRESULT) TNWR_TIMEOUT))\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem waiting for event or cancel!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem waiting for event or cancel!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// All errors are thrown.
#define HANDLE_CONNECTWAIT_RESULT\
										{\
											if (sr == ((HRESULT) TNCWR_CONNECTED))\
												sr = (HRESULT) S_OK;\
											else\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem waiting for leech connection!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem waiting for leech connection!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// TIMEOUT just drops through, all other errors are thrown.
#define HANDLE_CONNECTWAIT_RESULT_NOT_TIMEOUT\
										{\
											if (sr == ((HRESULT) TNCWR_CONNECTED))\
												sr = (HRESULT) S_OK;\
											else if (sr != ((HRESULT) TNCWR_TIMEOUT))\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem waiting for leech connection!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem waiting for leech connection!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// All errors are thrown.
#define HANDLE_PROMPTUSER_RESULT		{\
											if (sr != (HRESULT) S_OK)\
											{\
												pTNecd->pExecutor->Log(TNLF_CRITICAL | TNLF_PREFIX_TESTUNIQUEID, "Had problem prompting user!  %e", 1, (HRESULT) sr);\
												DPL(0, "Had problem prompting user!  %e", 1, (HRESULT) sr);\
												THROW_SYSTEMRESULT;\
											}\
										}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define EXPANDSLAVEVARS(szString, pszGrowableBuffer, dwBufferSize, dwTemp)\
		{\
			pTNgid->pMaster->ExpandSlaveStringVars(szString, pTNgid->pMachine, NULL, &dwTemp);\
			if (dwTemp > dwBufferSize)\
			{\
				SAFE_LOCALFREE(pszGrowableBuffer);\
				\
				dwBufferSize = dwTemp;\
				pszGrowableBuffer = (char*) LocalAlloc(LPTR, dwBufferSize);\
				if (pszGrowableBuffer == NULL)\
				{\
					hr = E_OUTOFMEMORY;\
					goto DONE;\
				}\
			}\
			\
			hr = pTNgid->pMaster->ExpandSlaveStringVars(szString, pTNgid->pMachine, pszGrowableBuffer, &dwTemp);\
			if (hr != S_OK)\
			{\
				DPL(0, "Failed expanding string variables!", 0);\
				goto DONE;\
			}\
		}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// This is like a Sleep, except it allows TestNet to do its thing.
// The "NULL, 0" means no items, "NULL, -1" means all the testers are important.
#define TNSLEEP(dwTimeout)				{\
											sr = pTNecd->pExecutor->WaitForEventOrCancel(NULL, 0, NULL, -1, dwTimeout, NULL);\
											HANDLE_WAIT_RESULT_NOT_TIMEOUT;\
											sr = S_OK;\
										}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define NEWSUBGROUP(pParentGroup, pszID, pszName, ppNewSubGroup)\
										{\
											HRESULT		hr;\
											\
											\
											hr = pParentGroup->NewSubGroup(pszID, pszName, ppNewSubGroup);\
											if (hr != S_OK)\
											{\
												DPL(0, "Couldn't create new subgroup \"%s\" under group \"%s\"!", 2, pszID, pParentGroup->m_pszID);\
												return (hr);\
											}\
										}

#define ADDTESTTOGROUP(pAddTestData, pGroup)\
										{\
											HRESULT		hr;\
											\
											\
											hr = pGroup->AddTest(pAddTestData);\
											if (hr != S_OK)\
											{\
												DPL(0, "Couldn't add test \"%s\" to group \"%s\"!", 2, (pAddTestData)->pszCaseID, pGroup->m_pszID);\
												return (hr);\
											}\
										}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define WRITEDATA(szFormatString, item, pszTempPtr)\
										{\
											TNsprintf(&pszTempPtr, szFormatString, 1, (item));\
											pTNwd->pWriteStoredData->AddString(pszTempPtr);\
											TNsprintf_free(&pszTempPtr);\
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




#else //__TESTNET_MACROS__
//#pragma message("__TESTNET_MACROS__ already included!")
#endif //__TESTNET_MACROS__
