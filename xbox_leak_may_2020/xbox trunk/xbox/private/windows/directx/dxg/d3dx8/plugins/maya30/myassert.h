#ifndef MyAssert_h
#define MyAssert_h


#include <assert.h>


#define INIT								try	{															

#define EXIT								}																\
											catch (char* szError) {											\
												cerr << "ERROR [" << szError << "]" << endl;				\
																											\
												hr	= E_FAIL;												\
											}


#define ASSERT(condition, error_msg)		if (!(condition)) {												\
												throw error_msg;											\
											}

#define HR_ATTEMPT(hresult, error_msg)		ASSERT(SUCCEEDED(hresult), (error_msg))


// assert one type of Dt function
#define	DT_ATTEMPT(statement)				statement






#endif