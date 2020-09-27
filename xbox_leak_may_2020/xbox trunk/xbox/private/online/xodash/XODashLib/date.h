/*************************************************************************************************\
Date.h  			: Implementation of all necessary Date functions
Creation Date		: 2/12/2002 9:47:54 AM
Copyright (c) Microsoft Corporation.  All rights reserved
Author				: Victor Blanco
//-------------------------------------------------------------------------------------------------
Notes				: 
\*************************************************************************************************/

#pragma once

class CDateObject
{
public:
	CDateObject();
    ~CDateObject();

    void setFileTime ( FILETIME ftTime );
	int getDate();
	int getDay();
	int getFullYear();
	int getHours();
	int getMilliseconds();
	int getMinutes();
	int getMonth();
	int getSeconds();
	int getUTCDate();
	int getUTCDay();
	int getUTCFullYear();
	int getUTCHours();
	int getUTCMilliseconds();
	int getUTCMinutes();
	int getUTCMonth();
	int getUTCSeconds();
	int getYear();
    void clearDate();
    void setDay( unsigned int nDay );
    void setMonth( unsigned int nMonth );
    void setYear( unsigned int nYear );
    void zeroTime( void );
    int calcAge( void );

	int isLeapYear(int nYear);
	int getDaysInMonth(int nMonth, int nYear);

	void SetSystemClock();

	TCHAR* toGMTString();
	TCHAR* toLocaleString();
	TCHAR* toUTCString();

	FILETIME m_time; // 100-nanosecond intervals since January 1, 1601
};
