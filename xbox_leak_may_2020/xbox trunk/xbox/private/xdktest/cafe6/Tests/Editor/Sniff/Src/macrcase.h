///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//      macrcase.CPP
//
//      Created by :                    Date :
//        Craigs                          7/17/96
//
//      Description :
//        Declaration of the CMacroTestCases class
//

#ifndef __MACROCASES_H__
#define __MACROCASES_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "edsntype.h"
#include "emulcase.h"

///////////////////////////////////////////////////////////////////////////////
// CMacroTestCases class

class CMacroTestCases : public CTest
{
  DECLARE_TEST(CMacroTestCases, EditorSniff)

// Operations
public:
	virtual void Run(void);

// Data
protected:

// Test Cases
protected:
};

#endif //__MACROCASES_H__
