///////////////////////////////////////////////////////////////////////////////
//	FileVerTestcase.H
//
//	Created by :			
//		Anushas
//				
//	Description :
//		Declaration of the CFileVerTestcase class
//

#ifndef __FileVerTestcase_H__
#define __FileVerTestcase_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "VerCheckSuite.h"
#include "FCompare.h"
#include <string>
#include <list>
///////////////////////////////////////////////////////////////////////////////
//	CAFileVerTestcase class

class CFileVerTestcase : public CTest
{
private:
	DECLARE_TEST(CFileVerTestcase, CVerCheckSuite)
	void WriteLog();//Write into a log file
	CFileCompare fc;//instance of the filecompare object
	std::string GetDrive(std::string);//gets the system dirve attached to teh pre or post file
// Operations
public:
	virtual void Run(void);

};

#endif //__FileVerTestcase_H__
