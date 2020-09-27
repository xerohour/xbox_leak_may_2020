/****************************************************************************************
*                                    SPARTA project                                     *
*                                                                                       *
* (TCP/IP test team)                                                                    *
*                                                                                       *
* Filename: CIPOptionsVector.hpp                                                               *
* Description: This is the implementation of the IPHeader functions                  *
*                                                                                       *
*                                                                                       *
* Revision history:     name          date         modifications                        *
*                                                                                       *
*                       deepakp      4/25/2000    created                              *
*                                                                                       *
*                                                                                       *
*                (C) Copyright Microsoft Corporation 1999-2000                          *
*****************************************************************************************/


#ifndef __SPARTA_IPOPTIONVECTOR_H__
#define __SPARTA_IPOPTIONVECTOR_H__

#include "IPOptions.hpp"

class CIPOptionsVector
{
	CIPOption *m_optionList[100];
	DWORD m_nextElementIndex;
	DWORD m_endElementIndex;

public:
	CIPOptionsVector();
	CIPOptionsVector(CIPOption** optionArray,DWORD nOptions);
	~CIPOptionsVector();

	// NOT to be provided as a COM API
	void FillWithOptions(CIPOption** optionList,DWORD nOptions);

	bool HasMoreIPOptions();

	CIPOption* NextIPOption();

	CIPOption* IPOptionAt(DWORD index);

	void ResetOptionVector();

	DWORD NumberOfOptions();
};


#endif // __SPARTA_IPOPTIONVECTOR_H__

