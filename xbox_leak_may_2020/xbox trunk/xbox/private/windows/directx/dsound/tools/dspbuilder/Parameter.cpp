/*++

Copyright (c) 2001 Microsoft Corporation

Module Name:

	Parameter.cpp

Abstract:

	Effect Parameters

Author:

	Robert Heitkamp (robheit) 29-Nov-2001

Revision History:

	29-Nov-2001 robheit
		Initial Version

--*/

//------------------------------------------------------------------------------
//	Includes:
//------------------------------------------------------------------------------
#include "Parameter.h"

//------------------------------------------------------------------------------
//	CParameter::CParameter
//------------------------------------------------------------------------------
CParameter::CParameter(void)
/*++

Routine Description:

	Constructor

Arguments:

	None

Return Value:

	None

--*/
{
	m_dwOffset	= 0;
	m_dwDefault	= 0;
	m_dwType	= 0;
	m_dwMin		= 0;
	m_dwMax		= 0;
	m_bAtomic	= FALSE;
	m_dwValue	= 0;
}

#if 0
//------------------------------------------------------------------------------
//	CParameter::CParameter
//------------------------------------------------------------------------------
CParameter::CParameter(
					   IN const CParameter& param
					   )
/*++

Routine Description:

	Copy constructor

Arguments:

	IN param -	Class to copy

Return Value:

	None

--*/
{
	*this = param;
}

//------------------------------------------------------------------------------
//	CParameter::CParameter
//------------------------------------------------------------------------------
CParameter::CParameter(
					   IN const FX_PARAM_DESCRIPTION& param
					   )
/*++

Routine Description:

	Copy constructor

Arguments:

	IN param -	Class to copy

Return Value:

	None

--*/
{
	*this = param;
}
#endif

//------------------------------------------------------------------------------
//	CParameter::~CParameter
//------------------------------------------------------------------------------
CParameter::~CParameter(void)
/*++

Routine Description:

	Destructor

Arguments:

	None

Return Value:

	None

--*/
{
}

//------------------------------------------------------------------------------
//	CParameter::operator =
//------------------------------------------------------------------------------
CParameter&
CParameter::operator = (
						IN const CParameter& param
						)
/*++

Routine Description:

	Assignment operator

Arguments:

	IN param -	Parameter to copy

Return Value:

	None

--*/
{
	m_name			= param.m_name;
	m_description	= param.m_description;
	m_units			= param.m_units;
	m_dwOffset		= param.m_dwOffset;
	m_dwDefault		= param.m_dwDefault;
    m_dwType		= param.m_dwType;
    m_dwMin			= param.m_dwMin;
    m_dwMax			= param.m_dwMax;
    m_bAtomic		= param.m_bAtomic;
	m_dwValue		= param.m_dwValue;

	return *this;
}

//------------------------------------------------------------------------------
//	CParameter::operator =
//------------------------------------------------------------------------------
CParameter&
CParameter::operator = (
						IN const FX_PARAM_DESCRIPTION& param
						)
/*++

Routine Description:

	Assignment operator

Arguments:

	IN param -	Parameter to copy

Return Value:

	None

--*/
{
	m_name			= param.ParameterName;
	m_description	= param.ParameterDesc;
	m_units			= param.UnitsName;
	m_dwOffset		= param.dwOffset;
	m_dwDefault		= param.dwDefaultValue;
    m_dwType		= param.dwParameterType;
    m_dwMin			= param.dwParameterMin;
    m_dwMax			= param.dwParameterMax;
    m_bAtomic		= param.bIsAtomic;
	m_dwValue		= m_dwDefault;

	return *this;
}
