
/*@ mktime

Test Cases:  Daylight Savings Time (DST) boundry tests.
-----------
 1. mktime failure boundries.
    Test extremes for failure (TM_MKTIME_FAILURE) return.

 2. _mktime64 failure boundries. (if MSC_VER > 1200)
    Test extremes for failure (TM_MKTIME_FAILURE) return.

 3. mktime DST boundry test.
    Each year is tested (68 total).
	The last ten days March through the first nine days 
	of April are tested for each year.  Also, for each
	year the last ten days of October through the first 
	three days of Novemeber are tested.
	Midnight and every four hours through the day is 
	tested for each day.
	Note that the exact hour of a DST switch is either
	invalid or undefined.

 4. _mktime64 DST boundry test. (if MSC_VER > 1200)
	Each year within 8 years of the turn of the century 
	(21st through 30th) is tested.
	The last ten days March through the first nine days 
	of April are tested for each year.  Also, for each
	year the last ten days of October through the first 
	three days of Novemeber are tested.
	Midnight and every four hours through the day is 
	tested for each day.
	Note that the exact hour of a DST switch is either
	invalid or undefined.
  
@*/

#include <time.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>

//	"constant" values for test result
#define	TEST_PASS		0
#define	TEST_FAIL		1
#define	TEST_SKIPPED	2
#define	TEST_CASCADE	3
#define	TEST_NORESULT	4

// macros
//
#define	MAX_FAILED_MSG_PER_TC	4096
#define	MAX_32TM_YEARS			51
#define	MAX_CENTURIES			10
#define	NUM_BOUNDRIES			16
#define	NUM_FAILURES			16
#define	TM_NO_DAYS_CHECKED		32
#define	TM_NO_YEARS_CHECKED		16
#define	EUROPE_HOUR				1
#define	USA_HOUR				2
#define	TM_INVALID_VALUE		0x4000
#define	TM_INVALID_DOW			7
#define	TM_UNDETIRMINED_ISDST	-1
#define	TM_MKTIME_FAILURE		-1
#define	TM_BASE_YEAR			87
#define	True					1
#define	False					0

enum	DSTType
{
	none,
	fixed,
	usa,
	europe
};

char*	g_szFile = __FILE__ ;
int		g_nTC1TestsFailed = 0;
int		g_nTC2TestsFailed = 0;
int		g_nTC3TestsFailed = 0;
int		g_nTC4TestsFailed = 0;

/*
 *  Structures for tests of type 1 above.
 */

tm	TM_Base[TM_NO_DAYS_CHECKED] = 
{
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 22,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 23,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 24,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 25,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 26,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 27,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 28,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 29,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 30,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 31,  2, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  1,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  2,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  3,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  4,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  5,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  6,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  7,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  8,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  9,  3, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 22,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 23,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 24,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 25,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 26,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 27,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 28,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 29,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 30,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE, 31,  9, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  1, 10, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  2, 10, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST},
	{TM_INVALID_VALUE, TM_INVALID_VALUE, TM_INVALID_VALUE,  3, 10, 0, TM_INVALID_DOW, TM_INVALID_VALUE, TM_UNDETIRMINED_ISDST}
};


// Helper test functions.  
/*
 *  dstInit() takes dstTest array an array of structures made up of 16 type "tm" structures
 *  initializes tm fields and copies the time fields from the parameters provided.  
 *  Also initializes the year field from 8 years before the turn of the century to 8 years 
 *  after the century.  Starts at the 21st century and will continue for MAX_CENTURIES centuries.
 */

void	tmInit(tm tmTest[MAX_32TM_YEARS * TM_NO_DAYS_CHECKED], int nHour = 12, int nMin = 0, int nSec = 0)
{
	tm*	ptm = tmTest;
	int	nYear = TM_BASE_YEAR;

	for	(int i=0; i<(MAX_32TM_YEARS); ++i)
	{
		memcpy(ptm, TM_Base, sizeof(TM_Base));

		for	(int k=0; k<TM_NO_DAYS_CHECKED; ++k)
		{
			ptm->tm_sec = nSec;
			ptm->tm_min = nMin;
			ptm->tm_hour = nHour;
			ptm->tm_year = nYear;
			++ptm;
		}

		++nYear;
	}
}

void	tmInitFailures(tm tmTest[NUM_FAILURES], int nHour = 12, int nMin = 0, int nSec = 0)
{
	tmTest[0].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[0].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[0].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[0].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[0].tm_mon = 0;		/* months since January - [0,11] */
	tmTest[0].tm_year = 0;		/* years since 1900 */
	tmTest[0].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[0].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[0].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[1].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[1].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[1].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[1].tm_mday = 30;		/* day of the month - [1,31] */
	tmTest[1].tm_mon = 5;		/* months since January - [0,11] */
	tmTest[1].tm_year = 0;		/* years since 1900 */
	tmTest[1].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[1].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[1].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[2].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[2].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[2].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[2].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[2].tm_mon = 6;		/* months since January - [0,11] */
	tmTest[2].tm_year = 0;		/* years since 1900 */
	tmTest[2].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[2].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[2].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[3].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[3].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[3].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[3].tm_mday = 31;		/* day of the month - [1,31] */
	tmTest[3].tm_mon = 11;		/* months since January - [0,11] */
	tmTest[3].tm_year = 0;		/* years since 1900 */
	tmTest[3].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[3].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[3].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[4].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[4].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[4].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[4].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[4].tm_mon = 0;		/* months since January - [0,11] */
	tmTest[4].tm_year = 69;		/* years since 1900 */
	tmTest[4].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[4].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[4].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[5].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[5].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[5].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[5].tm_mday = 30;		/* day of the month - [1,31] */
	tmTest[5].tm_mon = 5;		/* months since January - [0,11] */
	tmTest[5].tm_year = 69;		/* years since 1900 */
	tmTest[5].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[5].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[5].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[6].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[6].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[6].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[6].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[6].tm_mon = 6;		/* months since January - [0,11] */
	tmTest[6].tm_year = 69;		/* years since 1900 */
	tmTest[6].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[6].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[6].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[7].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[7].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[7].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[7].tm_mday = 30;		/* day of the month - [1,31] */
	tmTest[7].tm_mon = 11;		/* months since January - [0,11] */
	tmTest[7].tm_year = 69;		/* years since 1900 */
	tmTest[7].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[7].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[7].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */


	tmTest[8].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[8].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[8].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[8].tm_mday = 20;		/* day of the month - [1,31] */
	tmTest[8].tm_mon = 0;		/* months since January - [0,11] */
	tmTest[8].tm_year = 138;	/* years since 1900 */
	tmTest[8].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[8].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[8].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[9].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[9].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[9].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[9].tm_mday = 30;		/* day of the month - [1,31] */
	tmTest[9].tm_mon = 5;		/* months since January - [0,11] */
	tmTest[9].tm_year = 138;	/* years since 1900 */
	tmTest[9].tm_wday = TM_INVALID_DOW;		/* days since Sunday - [0,6] */
	tmTest[9].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[9].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[10].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[10].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[10].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[10].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[10].tm_mon = 6;		/* months since January - [0,11] */
	tmTest[10].tm_year = 138;	/* years since 1900 */
	tmTest[10].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[10].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[10].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[11].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[11].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[11].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[11].tm_mday = 31;	/* day of the month - [1,31] */
	tmTest[11].tm_mon = 11;		/* months since January - [0,11] */
	tmTest[11].tm_year = 138;	/* years since 1900 */
	tmTest[11].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[11].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[11].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[12].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[12].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[12].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[12].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[12].tm_mon = 0;		/* months since January - [0,11] */
	tmTest[12].tm_year = 1100;	/* years since 1900 */
	tmTest[12].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[12].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[12].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[13].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[13].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[13].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[13].tm_mday = 30;	/* day of the month - [1,31] */
	tmTest[13].tm_mon = 5;		/* months since January - [0,11] */
	tmTest[13].tm_year = 1100;	/* years since 1900 */
	tmTest[13].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[13].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[13].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[14].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[14].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[14].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[14].tm_mday = 1;		/* day of the month - [1,31] */
	tmTest[14].tm_mon = 6;		/* months since January - [0,11] */
	tmTest[14].tm_year = 1100;	/* years since 1900 */
	tmTest[14].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[14].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[14].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */

	tmTest[15].tm_sec = nSec;	/* seconds after the minute - [0,59] */
	tmTest[15].tm_min = nMin;	/* minutes after the hour - [0,59] */
	tmTest[15].tm_hour = nHour;	/* hours since midnight - [0,23] */
	tmTest[15].tm_mday = 31;	/* day of the month - [1,31] */
	tmTest[15].tm_mon = 11;		/* months since January - [0,11] */
	tmTest[15].tm_year = 1100;	/* years since 1900 */
	tmTest[15].tm_wday = TM_INVALID_DOW;	/* days since Sunday - [0,6] */
	tmTest[15].tm_yday = TM_INVALID_VALUE;	/* days since January 1 - [0,365] */
	tmTest[15].tm_isdst = TM_UNDETIRMINED_ISDST;/* daylight savings time flag */
}

int	CheckTM(DSTType enumDSTType, const tm& tmCheck, int* pbDST, int* pbNext, 
				int nSpMon = 2, int nSpDay = 30, int nFallMon = 9, int nFallDay = 25, int nFixedHour = 1)
{
	if	(*pbNext)
	{
		*pbNext = False;

		if	(*pbDST)
			*pbDST = False;
		else
			*pbDST = True;
	}
	else
	{
		switch	(enumDSTType)
		{
			case fixed:
			{
				if	(!*pbDST && (tmCheck.tm_mon == nSpMon) && (tmCheck.tm_mday == nSpDay))
				{
					if	(tmCheck.tm_hour >= nFixedHour)
						*pbDST = True;
					else
						*pbNext = True;
				}

				if	(*pbDST && (tmCheck.tm_mon == nSpMon) && (tmCheck.tm_mday == nFallDay))
				{
					if	(tmCheck.tm_hour >= nFixedHour -1)
						*pbDST = False;
					else
						*pbNext = True;
				}

				break;
			}

			case usa:
			{
				if	(!*pbDST && (tmCheck.tm_mon == 3) && (tmCheck.tm_wday == 0))
				{
					if	(tmCheck.tm_hour >= USA_HOUR)
						*pbDST = True;
					else
						*pbNext = True;
				}

				if	(*pbDST && (tmCheck.tm_mon == 9) && (tmCheck.tm_wday == 0) && (tmCheck.tm_mday > 24))
				{
					if	(tmCheck.tm_hour >= USA_HOUR - 1)
						*pbDST = False;
					else
						*pbNext = True;
				}

				break;
			}

			case europe:
			{
				if	(!*pbDST && (tmCheck.tm_mon == 2) && (tmCheck.tm_wday == 0) && (tmCheck.tm_mday > 24))
				{
					if	(tmCheck.tm_hour >= EUROPE_HOUR)
						*pbDST = True;
					else
						*pbNext = True;
				}

				if	(*pbDST && (tmCheck.tm_mon == 9) && (tmCheck.tm_wday == 0) && (tmCheck.tm_mday > 24))
				{
					if	(tmCheck.tm_hour >= EUROPE_HOUR -1)
						*pbDST = False;
					else
						*pbNext = True;
				}

				break;
			}
		}
	}

	if	(*pbDST && !tmCheck.tm_isdst)
		return	False;
	else if	(!*pbDST && tmCheck.tm_isdst)
		return	False;

	return	True;
}


int TC1()
{
	char		szMsg[400];
	tm			tmTest[NUM_FAILURES];
	tm*			ptm = tmTest;
	DSTType		enumDSTType = usa;
	int		bDST = False, bNext = False, bRet = True;

	// test mktime for boundries that should fail
	tmInitFailures(tmTest);
	ptm = tmTest;

	for	(int i=0; i<NUM_FAILURES; ++i)
	{
		if	(!(mktime(ptm) == time_t(TM_MKTIME_FAILURE)))
		{
			sprintf(szMsg, "File %s FAILED: mktime ( %d/%d/%d ) did not return -1\n", 
							g_szFile, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_year+1900);
						
			if	(g_nTC1TestsFailed < MAX_FAILED_MSG_PER_TC)
				printf(szMsg);

			++g_nTC1TestsFailed;
			bRet = False;
		}

		++ptm;
	}

    return bRet;
}

int TC2()
{
	char	szMsg[400];
	tm		tmTest[MAX_32TM_YEARS * TM_NO_DAYS_CHECKED];
	tm*		ptm = tmTest;
	DSTType	enumDSTType = usa;
	int		i, j, nHour;
	int		bDST = False, bNext = False, bRet = True;

	for	(nHour=0; nHour<24; ++nHour)
	{
		// test mktime DST change boundries for all years
		tmInit(tmTest, nHour);
		ptm = tmTest;

		for	(i=0; i<MAX_32TM_YEARS; ++i)
		{
			bDST = False;
			bNext = False;

			for	(j=0; j<TM_NO_DAYS_CHECKED; ++j)
			{
				if	(mktime(ptm) == time_t(TM_MKTIME_FAILURE))
				{
					sprintf(szMsg, "File %s FAILED: mktime ( %d/%d/%d  %d hundred hours ) returned -1\n", 
									g_szFile, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_year+1900, ptm->tm_hour);
						
					if	(g_nTC3TestsFailed < MAX_FAILED_MSG_PER_TC)
						printf(szMsg);

					++g_nTC3TestsFailed;
					bRet = False;
				}
				else
				{
					if	(CheckTM(enumDSTType, *ptm, &bDST, &bNext) == 0)
					{
						sprintf(szMsg, "File %s FAILED: mktime ( %d/%d/%d  %d hundred hours ), tm_isdst = %d\n", 
								g_szFile, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_year+1900, ptm->tm_hour, ptm->tm_isdst);
						
						if	(g_nTC3TestsFailed < MAX_FAILED_MSG_PER_TC)
							printf(szMsg);

						++g_nTC3TestsFailed;
						bRet = False;
					}
				}
				
				++ptm;
			}
		}
	}

    return bRet;
}

int	main()
{
	int	nRet = TEST_SKIPPED;

	if	(sizeof(time_t) == 4)
	{
		int	bSucceed = TC1();
		
		nRet = TEST_FAIL;
		bSucceed &= TC2();

		if	(bSucceed)
		{
			printf("\ntest succeeded\n");
			nRet = TEST_PASS;
		}
	}

	return	nRet;
};
