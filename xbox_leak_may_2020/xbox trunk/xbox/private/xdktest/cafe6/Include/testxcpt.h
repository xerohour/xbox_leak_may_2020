///////////////////////////////////////////////////////////////////////////////
//	testxcpt.h
//
//	Created by :			Date :
//		BrianCr				11/29/94
//
//	Description :
//		Declaration of the CTestException class
//

#ifndef __TESTXCPT_H__
#define __TESTXCPT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "targxprt.h"

#pragma warning(disable: 4251)

///////////////////////////////////////////////////////////////////////////////
// CTestException class

class TARGET_CLASS CTestException/*: public CException*/
{
/*
	DECLARE_DYNAMIC(CTestException)
*/

public:
	enum TestExceptionCause {
		causeUnknown,			// invoker didn't specify a cause
		causeGeneric,			// no current use
		causeOperationFail,		// operation failed
		causeAssumptionFail,	// assumption failed
		causeExpectFail,		// expectation failed
		causeSafetyFail,		// safety failed
		causeTargetGone,		// target app lost the focus
		causeTargetAssert,		// target app asserted
		causeTargetGPF,			// target app crashed
		NUM_CAUSES
	};

// Constructor
	CTestException(CString strMsg = "", TestExceptionCause cause = CTestException::causeUnknown, LPCSTR lpszFileName = NULL, int nLine = -1);

// operations
public:
	CString GetMessage(void)				{ return m_strMsg; }
	TestExceptionCause GetCause(void)		{ return m_cause; }

// data
protected:
	CString m_strMsg;
	TestExceptionCause m_cause;
	LPCSTR m_file;
	int m_line;

/*
#ifdef _DEBUG
	virtual void Dump(CDumpContext& dc) const;
#endif
*/
};


/*	TEST_EXCEPTION
 *		Throws a CTestException with the specified CTestException::enum cause.
 *		Also notes the file and line number, like an assert.
 *			(Note: THROW macro had a bug until MFC 2.5)
 *	SAFETY_EXCEPTION
 *		Throws a CTestException with the specified CTestException::enum cause.
 *		Used by SafetyCheck, which passes a file and line.
 */
/*
#if _MFC_VER < 0x0250		// work around for old MFC bug: THROW macro had ';'
#define TEST_EXCEPTION( cause )\
	afxExceptionContext.Throw(\
		new CTestException("", CTestException::##cause, THIS_FILE, __LINE__) )
#define SAFETY_EXCEPTION( cause, file, line )\
	afxExceptionContext.Throw( \
		new CTestException("", CTestException::##cause, file, line) )
#else			// workaround doesn't work in later MFC - use normal method again
*/
#define TEST_EXCEPTION( cause )\
	throw CTestException("", CTestException::##cause, THIS_FILE, __LINE__)
#define SAFETY_EXCEPTION( cause, file, line )\
	throw CTestException("", CTestException::##cause, file, line)
/*
#endif
*/

/*	EXPECT
 *		Like ASSERT (except it works in RETAIL), this does nothing if the
 *		condition is met, but throws an exception if it fails.
 *		This method is preferrable to an ASSERT in the following cases:
 *			The condition's failure means the target app is in a bad state
 *			You want a failure to write a log rather than bring up an ASSERT
 *		You should still use ASSERT when verifying the integrity of your own
 *		testing code.
 */
#define EXPECT( b )\
			((b) ? (void)0 : TEST_EXCEPTION(causeExpectFail))

/*	EXPECT_EXEC
 *		Like EXPECT, but caches a fail detail
 */
#define EXPECT_EXEC( b, sz )\
			((b) ? (void)0 : throw CTestException(sz, CTestException::causeExpectFail, THIS_FILE, __LINE__))

/*	CATCH_EXPECT
 *		Catches exceptions thrown by EXPECT or EXPECT_EXEC.  All others are
 *		thrown to the next handler.
 */
#define CATCH_EXPECT( e )\
			catch (CTestException e ) { \
			if ( e->m_cause != CTestException::causeExpectFail )\
				throw; \
			}

/*	CATCH_EXPECT_AND_IGNORE
 *		Like CATCH_EXPECT, except that no handler block or END_CATCH is needed.
 *		All EXPECT exceptions are ignored.
 */
#define CATCH_EXPECT_AND_IGNORE\
			CATCH_EXPECT( e )

/*	CATCH_NOEXEC
 *		Catches exceptions thrown by EXPECT and EXPECT_EXEC, then logs a
 *		FAIL NOEXEC for the current test.  If EXPECT_EXEC caused the
 *		exception, a failure detail will be logged as well.  If something
 *		other than an EXPECT caused the exception, it is thrown to the next
 *		handler.
 */
#define CATCH_NOEXEC\
			CATCH_EXPECT( e )
/*
			if ( !HaveFailInCache() )\
				CacheFailDetail( "Expected condition not met (%s %d)", e->m_file, e->m_line );\
			WriteLog( LT_NOEXEC, GetTestDesc() );\
			ClearFailCache();\
			END_CATCH
*/


#endif //__CAFEXCPT_H__
