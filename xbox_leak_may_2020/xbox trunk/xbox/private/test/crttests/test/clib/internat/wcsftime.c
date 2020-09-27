
/***************************************************************************

		   Copyright (c) 1993 Microsoft Corporation

Method: Arrays of test data.  Loop through the arrays.


Switches:   NONE


Abstract:   This test is mainly a check to see that all of formats are 
	recognized. It also runs through all months and days (abbreviated 
	and long forms) to see that they are correct.

	4 dates were chosen for initial test version- 2 during non-leap years 
	and 2 during leap years. Additional tests require additions to 
	descriptions array (the expected output) and the test array (which 
	defines the tm structure sent to the function strftime()). A better 
	and more complete test would call time() and localtime(), generate 
	the expected information and check it against information returned 
	by strftime().

	If future versions of strftime() add additional formats, changes need 
	to be made to the formats array, descriptions array and the test array.


===========================================================================
Syntax

===========================================================================
OS Version:
CPU:
Dependencies:           
Environment Vars:       
Verification Method:    
Priority:               1
Notes:                  
Products:               WIN NT


Revision History:

    Date        emailname   description
----------------------------------------------------------------------------
    20-Jun-1989 mrw         created
    21-Aug-1991 xc          change void main() to int main() to
    22-Mar-93   kevinboy    ported f_strftm.c
    9-Jul-1993  a-marca     added putenv call to set timezone.
    29-Dec-1994 a-timke     Fixed parameter type mismatch for wcsftime().

----------------------------------------------------------------------------

EndHeader:

****************************************************************************/

#include <stdio.h>
#include <time.h>
#include <string.h>
#include "test.h"
#include <wchar.h>



#define BUFF_SIZE   128
#define NUM_MONTHS   12
#define NUM_DAYS      7

static wchar_t *short_months[12]= {
		       L"Jan",
		       L"Feb",
		       L"Mar",
		       L"Apr",
		       L"May",
		       L"Jun",
		       L"Jul",
		       L"Aug",
		       L"Sep",
		       L"Oct",
		       L"Nov",
		       L"Dec"
		      };
static wchar_t *long_months[12] = {
		       L"January",
		       L"February",
		       L"March",
		       L"April",
		       L"May",
		       L"June",
		       L"July",
		       L"August",
		       L"September",
		       L"October",
		       L"November",
		       L"December"
		      };
static wchar_t *short_days[7] = {
		       L"Sun",
		       L"Mon",
		       L"Tue",
		       L"Wed",
		       L"Thu",
		       L"Fri",
		       L"Sat"
		     };

static wchar_t *long_days[7] = {
		       L"Sunday",
		       L"Monday",
		       L"Tuesday",
		       L"Wednesday",
		       L"Thursday",
		       L"Friday",
		       L"Saturday"
		     };

static wchar_t *formats[] = {
		    L"%a",
		    L"%A",
		    L"%b",
		    L"%B",
		    L"%c",
		    L"%d",
		    L"%H",
		    L"%I",
		    L"%j",
		    L"%m",
		    L"%M",
		    L"%p",
		    L"%S",
		    L"%U",
		    L"%w",
		    L"%W",
		    L"%x",
		    L"%X",
		    L"%y",
		    L"%Y",
		    L"%z"
		   };

#define ARRAY_SIZE sizeof(formats)/sizeof(char *)

static wchar_t *descriptions[][ARRAY_SIZE]=

{
 { L"Tue",L"Tuesday",L"Jan",L"January",L"01/01/99 00:00:59",L"01",L"00",L"12",
 L"001",L"01",L"00",L"AM",L"59",L"00",L"2",
 L"00",L"01/01/99",L"00:00:59",L"99",L"1999",L"PST"
 },

/* 1st day after feb 29, 1996 */
 {
 L"Fri",L"Friday",L"Mar",L"March",L"03/01/96 23:59:59",L"01",L"23",L"11",
 L"061",L"03",L"59",L"PM",L"59",L"08",L"5",
 L"09",L"03/01/96",L"23:59:59",L"96",L"1996",L"PST"
 },

/* last day of a leap year in the next century */
 {
 L"Wed",L"Wednesday",L"Dec",L"December",L"12/31/08 12:01:30",L"31",L"12",L"12",
 L"366",L"12",L"01",L"PM",L"30",L"52",L"3",
 L"52",L"12/31/08",L"12:01:30",L"08",L"2008",L"PST"
 },

 {
 L"Sun",L"Sunday",L"Jun",L"June",L"06/15/91 01:15:55",L"15",L"01",L"01",
 L"166",L"06",L"15",L"AM",L"55",L"24",L"0",
 L"23",L"06/15/91",L"01:15:55",L"91",L"1991",L"PDT"
 }


} ;



struct tm test_array[] = {
				{ 59 , 0  , 0  , 1  , 0  , 99 , 2 , 0  , 0 },
				{ 59 , 59 , 23 , 1  , 2  , 96 , 5 , 60 , 0 },
				{ 30 , 01 , 12 , 31 , 11 , 108, 3 , 365, 0 },
				{ 55 , 15 , 1  , 15 , 5  , 91 , 0 , 165, 1 }
			       };

#define NUM_TESTS sizeof(test_array)/sizeof(struct tm)


static int perfect;
static char test[] = "wcsftime";

int wcsftimeEntry(void);
int wcsftimeEntry( void ){
  wchar_t buff[BUFF_SIZE];
  int i,j;


  startest();
  
  // set the time zone
  _putenv("TZ=PST8PDT");
  _tzset();	//To register time zone

  for (j = 0 ; j < NUM_TESTS ; j++)
   for (i = 0 ; i < ARRAY_SIZE ; i++ )
    {
      wcsftime(buff,BUFF_SIZE,formats[i],&test_array[j]);
      if (wcscmp(buff,descriptions[j][i]) != 0)
	{
	DbgPrint ( "\n\nexpected = %ls\nactual = %ls\n", descriptions[j][i], buff );
       faill();
	}
    }
      wcsftime(buff,BUFF_SIZE,L"%%",&test_array[j]);
      if (wcscmp(buff,L"%") != 0)
       faill();

 for ( i = 0 ; i <  NUM_MONTHS ; i++ )
  {
   test_array[0].tm_mon = i;
   wcsftime(buff,BUFF_SIZE,L"%b",&test_array[0]);
   if (wcscmp(buff,short_months[i]) != 0 )
	faill();

   wcsftime(buff,BUFF_SIZE,L"%B",&test_array[0]);
   if (wcscmp(buff,long_months[i]) != 0 )
	faill();
  }
 for ( i = 0 ; i <  NUM_DAYS ; i++  )
  {
   test_array[1].tm_wday = i;
   wcsftime(buff,BUFF_SIZE,L"%a",&test_array[1]);
   if (wcscmp(buff,short_days[i]) != 0 )
	faill();

   wcsftime(buff,BUFF_SIZE,L"%A",&test_array[1]);
   if (wcscmp(buff,long_days[i]) != 0 )
	faill();
  }
 finish();
}
