//
// Automation Logging header
//

#ifndef _LOGGING_H_
#define _LOGGING_H_

#ifdef _XBOX
#include <xtl.h>
#else
#include <windows.h>
#endif

//
// Log levels
//
#define LOG_TITLE    0
#define LOG_RESULTS  1
#define LOG_NOTES    2

//
// Methods
//

void LogWrite(int level, char * str);
bool LogTestResult(char * Title, bool Pass, char * Err);
HRESULT LogVerifyHResult(HRESULT hrActual, char * Title, HRESULT hrExpected);
HRESULT LogVerifyHResultAlt(HRESULT hrActual, char * Title, HRESULT hrExpected, HRESULT hrAlternate);
void LogAPIError(HRESULT hr);
void LogTestResultAPI(char * Title, HRESULT hr);

#define LogNote(x) LogWrite(LOG_NOTES,x)

// Handy macros

#define VERIFY_TRUE(expr)							( LogTestResult(#expr, expr, NULL) )
#define VERIFY_HRESULT(expr, expectedhr)			( hr = expr, LogVerifyHResult(hr, #expr, expectedhr), hr == expectedhr )
#define VERIFY_HRESULT_ALT(expr, expectedhr, althr)	( hr = expr, LogVerifyHResultAlt(hr, #expr, expectedhr, althr), (hr == expectedhr) || (hr == althr) )


#endif // ifndef _LOGGING_H_