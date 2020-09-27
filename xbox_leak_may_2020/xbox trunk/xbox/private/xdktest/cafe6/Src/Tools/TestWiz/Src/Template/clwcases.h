///////////////////////////////////////////////////////////////////////////////
//	$$cases_h$$.h
//
//	Created by :			
//		$$TestOwner$$
//
//	Description :
//		Declaration of the $$TestClass$$ Class
//

#ifndef __CLWCASES_H__
#define __CLWCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#include "$$type_cpp$$.h"

///////////////////////////////////////////////////////////////////////////////
//	$$TestClass$$ class

class $$TestClass$$ : public CTest
{
    DECLARE_TEST($$TestClass$$, $$DriverClass$$)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
$$FuncDeclarations$$
};

#endif //__CLWCASES_H__
