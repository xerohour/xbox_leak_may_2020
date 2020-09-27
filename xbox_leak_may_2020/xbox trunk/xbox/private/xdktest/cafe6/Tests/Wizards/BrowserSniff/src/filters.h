///////////////////////////////////////////////////////////////////////////////
//	FILTERS.H
//
//	Created by :			Date :
//		TomSe					1/31/94
//
//	Description :
//		Declaration of the CFilters class
//

#ifndef __FILTERS_H__
#define __FILTERS_H__

#ifndef __STDAFX_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "queries.h"

class CFilters : public CQueries
{
	DECLARE_TEST(CFilters, CSniffDriver)

// Operations
public:
	virtual void Run(void);

// Data
protected:
	COFile   file;
	COBrowse brz;
	COSource src;
};

#endif //__FILTERS_H__
