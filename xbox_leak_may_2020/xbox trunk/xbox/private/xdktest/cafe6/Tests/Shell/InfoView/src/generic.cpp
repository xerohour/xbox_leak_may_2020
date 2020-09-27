///////////////////////////////////////////////////////////////////////////////
//	Generic.CPP
//
//	Created by :					Date : 
//		DougMan							1/6/97
//					  
//	Description :
//		InfoViewer's Generic Sniff Tests
//		A Starting Point


#include "stdafx.h"
//#include "wbutil.h"					//GetLocString function to grab Text from the string table
//#include "testutil.h"
//#include "mstwrap.h"
//#include "umainfrm.h"
//#include "..\eds\ufindrep.h"
//#include "..\eds\COSource.h"
#include "sniff.h"
#include "Generic.h"

#define new DEBUG_NEW

#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;

IMPLEMENT_TEST(CGeneric, CTest, "InfoViewer Generic", -1, CSniffDriver)

void CGeneric::Run(void)
{
  
	//initialization for all tests goes here

	m_pLog->Comment("Comment Printed in the Cafe Log - End of Generic Test");

};
